#pragma once
#include <algorithm>
#include <set>
#include <vector>

/// @brief Mathematical and combinatoric utilities for musical pitch processing.
namespace MusicUtils {

/// @brief Iterates over all k-element subsets of a set of pitches and invokes a callback.
/// @tparam Func Invocable callback type taking `const std::set<int>&`.
/// @param allPitches Source set of pitches.
/// @param k Subset size.
/// @param callback Function called with each k-subset.
template <typename Func>
void forEachSubset(const std::set<int> &allPitches, int k, Func callback) {
  std::vector<int> pitches(allPitches.begin(), allPitches.end());
  int n = pitches.size();

  if (k <= 0 || k > n)
    return;

  std::vector<int> bitmask(k, 1);
  bitmask.resize(n, 0);

  do {
    std::set<int> subset;
    for (int i = 0; i < n; ++i) {
      if (bitmask[i] == 1) {
        subset.insert(pitches[i]);
      }
    }

    callback(subset);

  } while (std::prev_permutation(bitmask.begin(), bitmask.end()));
}
} // namespace MusicUtils
