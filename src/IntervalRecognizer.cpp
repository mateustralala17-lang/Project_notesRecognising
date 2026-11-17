#include "../include/IntervalRecognizer.h"
#include <algorithm>
#include <iterator>
#include <map>
#include <set>
#include <vector>

IntervalRecognizer::IntervalRecognizer() {
  intervalTemplates[0] = "perfect unison";
  intervalTemplates[1] = "minor second";
  intervalTemplates[2] = "major second";
  intervalTemplates[3] = "minor third";
  intervalTemplates[4] = "major third";
  intervalTemplates[5] = "perfect fourth";
  intervalTemplates[6] = "tritone";
  intervalTemplates[7] = "perfect fifth";
  intervalTemplates[8] = "minor sixth";
  intervalTemplates[9] = "major sixth";
  intervalTemplates[10] = "minor seventh";
  intervalTemplates[11] = "major seventh";
}

std::set<IntervalData>
IntervalRecognizer::findAllIntervals(const std::set<int> &pitches) {
  std::set<IntervalData> results;
  MusicUtils::forEachSubset(pitches, 2, [&](const std::set<int> &pair) {
    auto it = pair.begin();
    int firstNote = *it;
    int secondNote = *std::next(it);

    int distance = secondNote - firstNote;
    int foldedDistance = distance % 12;

    auto itMap = intervalTemplates.find(foldedDistance);
    if (itMap != intervalTemplates.end()) {
      IntervalData data = {firstNote, distance, itMap->second};
      results.insert(data);
    }
  });
  return results;
}

std::set<int> IntervalRecognizer::buildInterval(const IntervalCommand &interval, const std::set<int> &pitches) {
  std::set<int> results;
  int lowestPitch = 60;
  for (auto &pitch : pitches) {
    if (pitch >= 60 && pitch <= 76) {
      lowestPitch = pitch;
      break;
    }
  }
  results.insert(lowestPitch);
  results.insert(lowestPitch + interval.distance);
  return results;
}
