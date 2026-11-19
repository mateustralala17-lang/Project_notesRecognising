# UI Skeleton & Styling Specification (`web/index.html`)

## 1. Scope & Constraints
- **File target:** Modify ONLY `web/index.html`.
- **Zero backend modifications:** Do NOT touch `.cpp`, `.h`, `.bat`, or any configuration files.
- **Pure CSS & Skeleton:** Zero hardcoded item counts (no hardcoded keys, chords, or intervals in HTML). No dynamic `fetch()` calls or event listeners attached yet.

---

## 2. API Contract Verification Summary
Verified from `src/main.cpp`, `PianoKeyboard.cpp`, `ChordMatrix.cpp`, `IntervalMatrix.cpp`:
- `GET /init` returns:
  - `"keyboard": {"minPitch": 21, "maxPitch": 108}`
  - `"chords": {"types": ["major", "minor"], "inversions": [0, 1, 2]}`
  - `"intervals": {"availableIntervals": [{"distance": 0, "intervalType": "perfect unison"}, ...]}`
- `GET /state` returns:
  - `"keyboard": [60, 64, 67, ...]`
  - `"chords": [{"rootNote": 0, "chordType": "major", "inversionType": 0, "lowestPitch": 60}, ...]`
  - `"intervals": [{"lowestPitch": 60, "distance": 4, "intervalType": "major third"}, ...]`

The container hierarchy and CSS classes map 1:1 with these payloads.

---

## 3. HTML Skeleton Structure
```html
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <title>Piano & Harmony Recognizer</title>
  <style>/* complete CSS styling */</style>
</head>
<body>
  <div class="page-wrapper">
    <!-- Top Bar -->
    <header class="top-bar">
      <button id="clear-all-btn" type="button">Clear All</button>
    </header>

    <!-- Main Workspace -->
    <main class="main-workspace">
      <!-- Piano Container -->
      <section id="piano-container"></section>

      <!-- Bottom Two-Column Split Row -->
      <section class="bottom-split-row">
        <!-- Left Panel: Intervals -->
        <div id="intervals-container"></div>

        <!-- Right Panel: Chords Section -->
        <div id="chords-section">
          <div id="chord-type-controls"></div>
          <div id="chords-container"></div>
        </div>
      </section>
    </main>
  </div>
  <script>
    // Placeholder for future dynamic DOM generation & fetch handlers
  </script>
</body>
</html>
```

---

## 4. CSS Design Specifications

### 4.1 Typography, Reset & Page Layout
- Font stack: `system-ui, -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif`.
- Global background: `#ffffff`, text color: `#222222`.
- `.page-wrapper`: `max-width: 1440px; width: 100%; margin: 0 auto; padding: 20px; box-sizing: border-box; display: flex; flex-direction: column; gap: 20px;`.
- Responsive design decision: Strictly single-breakpoint CSS layout without mobile media query adjustments as per user requirement.

### 4.2 Top Bar & Clear Button
- `.top-bar`: `display: flex; justify-content: flex-start; align-items: center;`.
- `#clear-all-btn`:
  - Background: `#e0e0e0`, hover: `#d0d0d0`, active: `#c0c0c0`.
  - Color: `#222222`, border: `1px solid #cccccc`, border-radius: `8px`.
  - Padding: `8px 18px`, font-size: `14px`, font-weight: `600`, cursor: `pointer`.

### 4.3 Piano Frame & Keys
- `#piano-container`: `position: relative; width: 100%; border-radius: 10px; border: 1px solid #d0d0d0; padding: 12px; box-sizing: border-box; background: #fafafa; overflow: hidden; box-shadow: 0 2px 8px rgba(0,0,0,0.04);`.
- `.piano-track`: `display: flex; position: relative; width: 100%; height: 180px; user-select: none;`.
- `.white-key`:
  - `flex: 1 1 0; min-width: 0; height: 100%; background: #ffffff; border: 1px solid #bdbdbd; border-radius: 0 0 5px 5px; box-sizing: border-box; position: relative; z-index: 1; cursor: pointer; display: flex; align-items: flex-end; justify-content: center; padding-bottom: 8px; font-size: 11px; color: #666666; font-weight: 500; transition: background-color 0.12s ease, border-color 0.12s ease;`.
- `.black-key`:
  - `position: absolute; top: 0; height: 60%; background: #1e1e1e; border: 1px solid #111111; border-radius: 0 0 4px 4px; box-sizing: border-box; z-index: 2; cursor: pointer; box-shadow: 0 3px 6px rgba(0,0,0,0.35); transition: background-color 0.12s ease, border-color 0.12s ease;`.
- `.white-key.active`:
  - Background: `#e53935`, border-color: `#c62828`, color: `#ffffff`.
- `.black-key.active`:
  - Background: `#ff5252`, border: `1px solid #ffffff`, box-shadow: `0 3px 6px rgba(229,57,53,0.5)`.

### 4.4 Bottom Panels & Matrices
- `.bottom-split-row`:
  - `display: grid; grid-template-columns: 1fr 1.6fr; gap: 24px; align-items: start; width: 100%; box-sizing: border-box;`.
- `#intervals-container`:
  - `background: #fafafa; border: 1px solid #e0e0e0; border-radius: 10px; padding: 16px; box-sizing: border-box;`.
- `.intervals-grid`:
  - `display: grid; grid-template-columns: repeat(3, minmax(0, 1fr)); gap: 10px;`.
- `#chords-section`:
  - `background: #fafafa; border: 1px solid #e0e0e0; border-radius: 10px; padding: 16px; box-sizing: border-box; display: flex; flex-direction: column; gap: 16px;`.
- `#chord-type-controls`:
  - `display: flex; flex-wrap: wrap; gap: 10px; align-items: center;`.
  - Pill/radio controls styling: neutral rounded buttons (`padding: 6px 14px; border-radius: 20px; border: 1px solid #cccccc; background: #ffffff; color: #444; font-weight: 600; font-size: 13px; cursor: pointer; transition: all 0.12s ease;`).
- `#chords-container`:
  - `display: flex; flex-wrap: wrap; gap: 20px; align-items: flex-start;`.
- `.chord-group-grid`:
  - `display: grid; grid-template-columns: repeat(2, minmax(56px, 1fr)); gap: 10px;`.
- `.matrix-btn`:
  - `min-height: 44px; padding: 8px 10px; border-radius: 8px; background: #ffffff; border: 2px solid #d32f2f; color: #222222; font-weight: 600; font-size: 14px; cursor: pointer; display: flex; align-items: center; justify-content: center; text-align: center; box-sizing: border-box; transition: background-color 0.12s ease, color 0.12s ease, transform 0.08s ease; user-select: none;`.
  - Hover: `background-color: #ffebee;`.
- `.matrix-btn.active`:
  - `background-color: #d32f2f; color: #ffffff; border-color: #b71c1c; box-shadow: 0 2px 6px rgba(211,47,47,0.35);`.

