#pragma once
#include "DataTypes.h"
#include "MusicUtils.h"
#include <map>
#include <set>
#include <string>
#include <vector>

class IntervalRecognizer {
private:
  std::map<int, std::string> intervalTemplates;

public:
  IntervalRecognizer();
  const std::map<int, std::string>& getTemplates() const { return intervalTemplates; }
  std::set<IntervalData> findAllIntervals(const std::set<int> &pitches);
  std::set<int> buildInterval(const IntervalCommand &interval, const std::set<int> &pitches);
};