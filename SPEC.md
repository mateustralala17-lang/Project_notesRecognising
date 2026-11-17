# Specification: Refactoring C++ Backend into MVC REST API Architecture

## 0. Mandatory Pre-Implementation Rules
**Before writing any code**, inspect `include/DataTypes.h`, `include/AppController.h`, `include/PianoKeyboard.h`, `src/PianoKeyboard.cpp`, `include/ChordRecognizer.h`, and `include/IntervalRecognizer.h`.
- **Strict Type & Data Flow Preservation**:
  - Do NOT invent new data formats or change parameter types accepted by existing methods.
  - Preserve the exact data flow: extract the exact same sets and structs from `State` (using its existing public getters) that the existing methods use, and serialize their real fields directly into JSON.

---

## 1. Overview & Architectural Goals
Refactor the C++ backend into a Model-View-Controller (MVC) REST API using `cpp-httplib`.
* **Model**: `State` (stores active pitches, recognized chords, and intervals) and recognizers (`ChordRecognizer`, `IntervalRecognizer`).
* **Controller**: `AppController` manages state mutations (`press`, `release`, `updateAll`) and exposes read-only access to `State`.
* **Views / Presenters**: `UIComponent` interface and its implementations (`PianoKeyboard`, `ChordMatrix`, `IntervalMatrix`) act as stateless view adapters that serialize `const State&` and static layout data into JSON fragments via standard `<sstream>`.
* **API / Glue Layer**: `main.cpp` exposes an HTTP REST server with thread-safe endpoints using `std::mutex`.

---

## 2. Component Design & Interfaces

### 2.1 Abstract Base Class `UIComponent` (`include/UIComponent.h`)
```cpp
#pragma once
#include <string>

class State; // Forward declaration

class UIComponent
{
public:
    virtual ~UIComponent() = default;

    // Returns component key in the combined JSON object ("keyboard", "chords", "intervals")
    virtual std::string getKey() const = 0;

    // Serializes state data into a JSON fragment
    virtual std::string serialize(const State& state) const = 0;

    // Serializes static/initial layout data into a JSON fragment for GET /init
    virtual std::string getInitData() const = 0;
};
```

---

### 2.2 Concrete UI Components

#### A. `PianoKeyboard` (`include/PianoKeyboard.h`, `src/PianoKeyboard.cpp`)
* Inherits from `UIComponent`.
* Removes all legacy console outputs (`std::cout`) and observer methods (`highlightNotes`, `highlightChords`, `highlightIntervals`, `clearHighlights`, `draw`).
* Defines constants: `static constexpr int MIN_PITCH = 21;` (A0) and `static constexpr int MAX_PITCH = 108;` (C8) for standard 88-key keyboard.
* `getKey() const`: returns `"keyboard"`.
* `serialize(const State& state) const`:
  * Returns active MIDI pitches as a JSON array of integers: `[60, 64, 67]`.
* `getInitData() const`:
  * Returns JSON object with pitch bounds: `{"minPitch":21,"maxPitch":108}`.

#### B. `ChordMatrix` (`include/ChordMatrix.h`, `src/ChordMatrix.cpp`)
* Inherits from `UIComponent`.
* `getKey() const`: returns `"chords"`.
* `serialize(const State& state) const`:
  * Reads `state.getChords()`.
  * Returns JSON array of objects directly serializing fields from `ChordData`:
    ```json
    [
      {"rootNote":0,"chordType":"major","inversionType":0,"lowestPitch":60}
    ]
    ```
- `getInitData() const`:
  - Dynamically retrieves all supported chord types and available inversions directly from `ChordRecognizer` ((inspect include/ChordRecognizer.h to use its existing public interface/constants; do not modify it without asking)).
  - Serializes them into a JSON object (e.g., `{"types":[...],"inversions":[...]}` based on the actual templates defined in `ChordRecognizer`).

#### C. `IntervalMatrix` (`include/IntervalMatrix.h`, `src/IntervalMatrix.cpp`)
* Inherits from `UIComponent`.
* `getKey() const`: returns `"intervals"`.
* `serialize(const State& state) const`:
  * Reads `state.getIntervals()`.
  * Returns JSON array of objects directly serializing fields from `IntervalData`:
    ```json
    [
      {"lowestPitch":60,"distance":4,"intervalType":"major third"}
    ]
    ```
- `getInitData() const`:
  - Dynamically retrieves all supported interval distances and names directly from `IntervalRecognizer` ((inspect include/IntervalRecognizer.h to use its existing public interface/constants; do not modify it without asking)).
  - Serializes them into a JSON object (e.g., `{"availableIntervals":[{"distance":...,"intervalType":"..."}, ...]}` matching the exact entries stored in `IntervalRecognizer`).

---

### 2.3 Refactoring `AppController` (`include/AppController.h`, `src/AppController.cpp`)
* **Remove `PianoKeyboard` dependency**:
  * Remove `PianoKeyboard* myPiano` member.
  * Remove `#include "PianoKeyboard.h"` and the `PianoKeyboard*` member from `AppController`.
  * Update constructor to default constructor `AppController();`.
  * Search the entire codebase (including `src/`, `tests/`, or any standalone test files) for existing instantiations like `AppController(&...)` and update them to `AppController()` so all targets compile cleanly.
  * Remove calls to `myPiano->clearHighlights()`, `myPiano->highlightNotes()`, etc. from `updateAll()`.
* **State Accessor**:
  * Add public getter:
    ```cpp
    const State& getState() const { return currentState; }
    ```
* **Preserve all business logic**:
  * Overloaded `press(int)`, `press(const ChordCommand&)`, `press(const IntervalCommand&)` remain unchanged.
  * Overloaded `release(int)`, `release(const ChordCommand&)`, `release(const IntervalCommand&)` remain unchanged.
  * Do NOT modify `DataTypes.h`, `ChordRecognizer`, or `IntervalRecognizer`. If you determine that a modification (such as adding a read-only getter) is strictly required to implement `getInitData()`, stop, clearly explain the exact reason, and ask for my permission before editing those files.

---

## 3. Server & Glue Logic (`src/main.cpp`)

### 3.1 Setup
* Instantiate `AppController controller;`.
* Instantiate components:
  ```cpp
  std::vector<std::unique_ptr<UIComponent>> components;
  components.push_back(std::make_unique<PianoKeyboard>());
  components.push_back(std::make_unique<ChordMatrix>());
  components.push_back(std::make_unique<IntervalMatrix>());
  ```
* Mutex for concurrency control across HTTP handler threads:
  ```cpp
  std::mutex appMutex;
  ```
* Helper function `buildCombinedStateJson(const State& state, const std::vector<std::unique_ptr<UIComponent>>& components)`:
  * Generates combined JSON:
    ```json
    {
      "keyboard": [...],
      "chords": [...],
      "intervals": [...]
    }
    ```

### 3.2 HTTP REST Routes (Port 8080)
1. **`GET /`**:
   * Reads and serves `web/index.html` with `text/html` MIME type.
2. **`GET /init`**:
   * Aggregates `getInitData()` from all `components`:
     ```json
     {
       "keyboard": {"minPitch":21,"maxPitch":108},
       "chords": {"types":["major","minor"],"inversions":[0,1,2]},
       "intervals": {"availableIntervals":[...]}
     }
     ```
3. **`GET /state`**:
   * Acquires `std::lock_guard<std::mutex> lock(appMutex);`.
   * Calls `buildCombinedStateJson(controller.getState(), components)`.
   * Returns `application/json`.
4. **`POST /press` & `POST /release`**:
   * Acquires `std::lock_guard<std::mutex> lock(appMutex);`.
   * Reads URL query parameters via `req.get_param_value(...)`:
     * If `type == "note"`: parses `pitch = std::stoi(req.get_param_value("pitch"))` -> calls `controller.press(pitch)` / `controller.release(pitch)`.
     * If `type == "chord"`: parses `root = std::stoi(...)`, `inversion = std::stoi(...)`, `chordType = ...` -> constructs `ChordCommand{root, inversion, chordType}` -> calls `controller.press(chordCmd)` / `controller.release(chordCmd)`.
     * If `type == "interval"`: parses `distance = std::stoi(...)`, `intervalType = ...` -> constructs `IntervalCommand{distance, intervalType}` -> calls `controller.press(intervalCmd)` / `controller.release(intervalCmd)`.
   * Immediately returns updated combined state JSON (same format as `GET /state`).

---

## 4. Coding Standards & Build Compatibility
* Modern C++17 without third-party dependencies outside standard library and `include/httplib.h`.
* Follow the existing codebase formatting style and verify compilation by running the existing build.bat without modifying it.

