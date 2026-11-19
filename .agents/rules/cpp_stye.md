---
trigger: always_on
---

# C++ Coding Style & Naming Conventions

1. **Allman Brace Style (Mandatory):**
   - ALWAYS place opening and closing curly braces on their own new lines for ALL constructs: namespaces, classes, structs, functions, `for`/`while` loops, `if`/`else` branches, and `switch` blocks.
   - NEVER omit curly braces, even if a loop or `if` statement contains only a single line of code.
   - Example:
     ```cpp
     for (int x : items)
     {
         if (x > 0)
         {
             process(x);
         }
     }
     ```

2. **Consistent Variable Naming Across Parallel Classes:**
   - Before naming any new variable, parameter, or helper method, inspect parallel/sibling classes (e.g., `ChordRecognizer` vs `IntervalRecognizer`, `ChordMatrix` vs `IntervalMatrix`).
   - If a variable with the same semantic meaning already exists in a sibling class (such as `lowestPitch`, `rootNote`, `results`, `notesToRemove`), you MUST reuse the exact same name and casing (do NOT invent synonyms like `lowerPitch` or switch to `lowest_pitch`).