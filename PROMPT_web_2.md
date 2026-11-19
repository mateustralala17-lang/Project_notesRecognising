# Dynamic UI Generation Specification (`web/index.html`)

## Strict Constraints & Pre-Flight Check
- **Read-only inspection first:** Before writing any code, read `web/index.html` to inspect the exact existing container IDs (`#piano-container`, `#intervals-container`, `#chords-container`, `#chord-type-controls`) and CSS classes (`.piano-track`, `.white-key`, `.black-key`, `.intervals-grid`, `.chord-group-grid`, `.matrix-btn`, `.active`).
- **Single-file scope:** Modify ONLY `web/index.html` (primarily inside the `<script>` tag, plus minimal non-destructive CSS rules inside `<style>` for `.panel-header` and equal panel stretching). Do NOT touch any `.cpp`, `.h`, `.bat`, or other project files.
- **No interaction / state requests yet:** Do NOT attach `click` event listeners, do NOT call `GET /state`, and do NOT call `POST /press` or `POST /release` in this step. This step is strictly for fetching `GET /init`, generating the DOM, populating `dataset` attributes, and caching element references in lookup maps.

---

## Expected `GET /init` JSON Schema
The server responds to `GET /init` with:
- `keyboard`: `{ "minPitch": 21, "maxPitch": 108 }`
- `chords`: `{ "types": ["major", "minor"], "inversions": [0, 1, 2] }`
- `intervals`: `{ "availableIntervals": [ { "distance": 0, "intervalType": "perfect unison" }, { "distance": 1, "intervalType": "minor second" }, ... ] }`

---

## Implementation Details

### 1. Minimal CSS Additions (`web/index.html` `<style>`)
- `.panel-header`:
  - `font-size: 15px; font-weight: 600; color: #444444; margin-bottom: 12px;`
- Equal panel height stretching:
  - Update `.bottom-split-row`: `align-items: stretch;`
  - Update `#intervals-container`: `display: flex; flex-direction: column;`
  - Update `.intervals-grid`: `flex-grow: 1; grid-auto-rows: 1fr;`
  - Hide empty filter bar so it does not add a 16px flex gap above chord headers:
    `#chord-type-controls:empty { display: none; }`
  - Wrapper for chord groups (stretches evenly across `#chords-container` so Major/Minor fill the right panel without leaving empty space):
    `.chord-group { display: flex; flex-direction: column; flex: 1 1 160px; }`

### 2. Global Element Lookup Maps (DOM Cache)
Declared at top level inside `<script>`:
```javascript
const keyElements = {};        // keyElements[pitch] = el
const intervalElements = {};   // intervalElements[distance] = el
const chordElements = {};      // chordElements[`${root}_${chordType}`] = el
```

### 3. Fetch `/init` & Lifecycle
On `DOMContentLoaded`:
- Call `fetch('/init')`.
- Parse response as JSON.
- Safely invoke each renderer only if its section exists in the payload:
  - `if (data.keyboard) renderKeyboard(data.keyboard);`
  - `if (data.intervals && Array.isArray(data.intervals.availableIntervals)) renderIntervals(data.intervals);`
  - `if (data.chords && Array.isArray(data.chords.types)) renderChords(data.chords);`
- Wrap entire block in `try / catch (err)` logging errors via `console.error("Failed to load /init:", err);`.

### 4. Piano Keyboard Builder (`renderKeyboard(keyboardData)`)
- Target: `#piano-container`. Clear existing content.
- Create container `<div class="piano-track">`.
- Black key pitch class predicate: `const isBlack = (p) => [1, 3, 6, 8, 10].includes(p % 12);`.
- Iterate `pitch` from `keyboardData.minPitch` to `keyboardData.maxPitch` inclusive.
- **Pass 1 (White keys):**
  - For each non-black pitch:
    - Create `<div class="white-key">`.
    - If `pitch % 12 === 0`, display octave label (e.g., `C1`..`C8` calculated via `C${Math.floor(pitch / 12) - 1}`).
    - Set `dataset.type = "note"`, `dataset.pitch = String(pitch)`.
    - Append to `.piano-track`.
    - Cache `keyElements[pitch] = el`.
    - Record white key index for black key placement.
- **Pass 2 (Black keys):**
  - Total white keys count `whiteCount = whiteKeys.length`.
  - Calculate `whiteWidth = 100 / whiteCount` (in percent).
  - Calculate `blackWidth = whiteWidth * 0.6` (in percent).
  - For each black pitch:
    - Determine preceding white key index `precedingWhiteIndex`.
    - Calculate `left = (precedingWhiteIndex + 1) * whiteWidth - (blackWidth / 2)` (in percent).
    - Create `<div class="black-key">`.
    - Set `style.left = `${left}%``, `style.width = `${blackWidth}%``.
    - Set `dataset.type = "note"`, `dataset.pitch = String(pitch)`.
    - Append to `.piano-track`.
    - Cache `keyElements[pitch] = el`.
- Append `.piano-track` to `#piano-container`.

### 5. Intervals Matrix Builder (`renderIntervals(intervalsData)`)
- Target: `#intervals-container`. Clear existing content.
- Add `<h3 class="panel-header">Intervals</h3>`.
- Create `<div class="intervals-grid">`.
- Short music-theory label mapping:
  ```javascript
  const INTERVAL_LABELS = {
    0: "P1", 1: "m2", 2: "M2", 3: "m3", 4: "M3", 5: "P4",
    6: "TT", 7: "P5", 8: "m6", 9: "M6", 10: "m7", 11: "M7"
  };
  ```
- Iterate over `intervalsData.availableIntervals`:
  - Create `<button type="button" class="matrix-btn">`.
  - Set `textContent = INTERVAL_LABELS[item.distance] ?? item.intervalType`.
  - Populate dataset:
    - `dataset.type = "interval"`
    - `dataset.distance = String(item.distance)`
    - `dataset.intervalType = item.intervalType`
  - Append to `.intervals-grid`.
  - Cache `intervalElements[item.distance] = btn`.
- Append `.intervals-grid` to `#intervals-container`.

### 6. Chords Matrix Builder (`renderChords(chordsData)`)
- Target: `#chords-container`. Clear existing content. Leave `#chord-type-controls` empty.
- Root note names:
  ```javascript
  const ROOT_NAMES = ["C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"];
  ```
- Chord label formatter helper:
  - `"major"` -> `${ROOT_NAMES[root]}`
  - `"minor"` -> `${ROOT_NAMES[root]}m`
  - `"diminished"` -> `${ROOT_NAMES[root]}dim`
  - `"augmented"` -> `${ROOT_NAMES[root]}aug`
  - default -> `${ROOT_NAMES[root]} (${chordType})`
- Iterate over each `chordType` in `chordsData.types`:
  - Create wrapper `<div class="chord-group">`.
  - Header title: capitalize first letter (e.g. `"Major"`, `"Minor"`) -> `<h3 class="panel-header">${title}</h3>`.
  - Create `<div class="chord-group-grid">`.
  - For `root = 0` to `11`:
    - Create `<button type="button" class="matrix-btn">`.
    - Set `textContent` using the formatter helper.
    - Populate dataset:
      - `dataset.type = "chord"`
      - `dataset.root = String(root)`
      - `dataset.inversion = "0"`
      - `dataset.chordType = chordType`
    - Append to `.chord-group-grid`.
    - Cache `chordElements[`${root}_${chordType}`] = btn`.
  - Append header and grid to `.chord-group`.
  - Append `.chord-group` to `#chords-container`.

