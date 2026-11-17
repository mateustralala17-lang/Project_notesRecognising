#pragma once
#include <algorithm>
#include <set>
#include <vector>


namespace MusicUtils {

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