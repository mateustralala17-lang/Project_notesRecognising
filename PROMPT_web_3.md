# Specification & Implementation Guide: Frontend-Backend State & Interaction Pipeline (`PROMPT_web_3.md`)

## 1. Strict Constraints & Pre-Flight Rules
- **Single-file scope:** Modify ONLY `web/index.html` exclusively inside the `<script>` tag. Do NOT modify any `.cpp`, `.h`, `.bat`, or any other project files.
- **Pre-flight inspection:** Prior to editing, verify the existing lookup maps (`keyElements`, `intervalElements`, `chordElements`), container IDs (`#piano-container`, `#intervals-container`, `#chords-container`, `#clear-all-btn`), and the backend contracts in `src/main.cpp`.
- **Preserve existing DOM generation:** Do NOT change or break the existing `renderKeyboard`, `renderIntervals`, or `renderChords` functions.

---

## 2. Architecture & Data Flow

### 2.1 State Management & Lookup Maps
At the top level of `<script>`:
```javascript
const keyElements = {};        // keyElements[pitch] = el
const intervalElements = {};   // intervalElements[distance] = el
const chordElements = {};      // chordElements[`${root}_${chordType}`] = el
let currentState = { keyboard: [], chords: [], intervals: [] };
```

---

### 2.2 Concurrency & Sequential Request Queue
To guarantee race-condition prevention across rapid clicks without dropping user interactions, manage network requests through a serialized Promise queue:
```javascript
let requestQueue = Promise.resolve();

function sendRequest(endpoint, params) {
  requestQueue = requestQueue.then(async () => {
    try {
      const query = params ? `?${params.toString()}` : "";
      const res = await fetch(`${endpoint}${query}`, { method: "POST" });
      if (!res.ok) {
        throw new Error(`HTTP ${res.status} on ${endpoint}`);
      }
      const state = await res.json();
      renderState(state);
      return state;
    } catch (err) {
      console.error(`Request failed (${endpoint}):`, err);
    }
  });
  return requestQueue;
}
```

---

### 2.3 State Rendering Engine (`renderState(state)`)
Whenever new state data arrives (from initial load, button clicks, or Clear All):
```javascript
function renderState(state) {
  if (!state) return;
  currentState = state;

  // 1. Reset: remove .active across all cached elements
  for (const el of Object.values(keyElements)) {
    el.classList.remove("active");
  }
  for (const el of Object.values(intervalElements)) {
    el.classList.remove("active");
  }
  for (const el of Object.values(chordElements)) {
    el.classList.remove("active");
  }

  // 2. Highlight active piano keys
  if (Array.isArray(state.keyboard)) {
    for (const pitch of state.keyboard) {
      keyElements[pitch]?.classList.add("active");
    }
  }

  // 3. Highlight active chords
  if (Array.isArray(state.chords)) {
    for (const chord of state.chords) {
      chordElements[`${chord.rootNote}_${chord.chordType}`]?.classList.add("active");
    }
  }

  // 4. Highlight active intervals
  if (Array.isArray(state.intervals)) {
    for (const interval of state.intervals) {
      intervalElements[interval.distance]?.classList.add("active");
    }
  }
}
```

---

### 2.4 Event Delegation & Click Handling
Instead of attaching separate listeners to every key or button, attach delegation handlers to the parent containers:
- `#piano-container`
- `#intervals-container`
- `#chords-container`

Handler pattern:
```javascript
function handleContainerClick(event) {
  const target = event.target.closest("[data-type]");
  if (!target) return;

  const isActive = target.classList.contains("active");
  const endpoint = isActive ? "/release" : "/press";
  const params = new URLSearchParams(target.dataset);

  sendRequest(endpoint, params);
}
```

---

### 2.5 Clear All Implementation (`#clear-all-btn`)
Clicking `#clear-all-btn` enqueues sequential `/release` calls for every note currently in `currentState.keyboard`:
```javascript
const clearBtn = document.getElementById("clear-all-btn");
if (clearBtn) {
  clearBtn.addEventListener("click", () => {
    requestQueue = requestQueue.then(async () => {
      const activePitches = [...(currentState.keyboard || [])];
      for (const pitch of activePitches) {
        try {
          const params = new URLSearchParams({ type: "note", pitch: String(pitch) });
          const res = await fetch(`/release?${params.toString()}`, { method: "POST" });
          if (res.ok) {
            const state = await res.json();
            renderState(state);
          }
        } catch (err) {
          console.error(`Failed to release pitch ${pitch}:`, err);
        }
      }
    });
  });
}
```

---

### 2.6 Lifecycle Integration on `DOMContentLoaded`
Inside `DOMContentLoaded`:
1. `await fetch("/init")` -> parse JSON -> invoke `renderKeyboard`, `renderIntervals`, `renderChords`.
2. Attach event delegation listeners to `#piano-container`, `#intervals-container`, `#chords-container`, and `#clear-all-btn`.
3. Fetch initial state `GET /state` and invoke `renderState(initialState)`.
```javascript
document.addEventListener("DOMContentLoaded", async () => {
  try {
    const res = await fetch("/init");
    if (!res.ok) throw new Error(`HTTP error! status: ${res.status}`);
    const data = await res.json();

    if (data.keyboard) renderKeyboard(data.keyboard);
    if (data.intervals && Array.isArray(data.intervals.availableIntervals)) {
      renderIntervals(data.intervals);
    }
    if (data.chords && Array.isArray(data.chords.types)) {
      renderChords(data.chords);
    }

    // Attach listeners
    document.getElementById("piano-container")?.addEventListener("click", handleContainerClick);
    document.getElementById("intervals-container")?.addEventListener("click", handleContainerClick);
    document.getElementById("chords-container")?.addEventListener("click", handleContainerClick);
    setupClearAll();

    // Initial state synchronization
    const stateRes = await fetch("/state");
    if (stateRes.ok) {
      const initialState = await stateRes.json();
      renderState(initialState);
    }
  } catch (err) {
    console.error("Initialization error:", err);
  }
});
```

