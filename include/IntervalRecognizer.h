#pragma once
#include "DataTypes.h"
#include "MusicUtils.h"
#include <map>
#include <set>
#include <string>
#include <vector>

/// @brief Recognizes and builds harmonic intervals between pairs of pitches.
class IntervalRecognizer {
private:
  std::map<int, std::string> intervalTemplates;

public:
  /// @brief Initializes standard interval templates (0 to 11 semitones).
  IntervalRecognizer();

  /// @brief Retrieves the map of interval templates.
  /// @return Const reference to map associating semitone distances with interval names.
  const std::map<int, std::string>& getTemplates() const { return intervalTemplates; }

  /// @brief Finds all musical intervals between every 2-note combination in the given pitches.
  /// @param pitches Set of active MIDI pitches to analyze.
  /// @return Set of recognized IntervalData objects.
  std::set<IntervalData> findAllIntervals(const std::set<int> &pitches);

  /// @brief Builds an interval from a command relative to active pitches or default octaves.
  /// @param interval Target interval specification.
  /// @param pitches Currently active pitches used as base reference.
  /// @return Set containing the resulting pair of MIDI pitches.
  std::set<int> buildInterval(const IntervalCommand &interval, const std::set<int> &pitches);
};
