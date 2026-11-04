#pragma once
#include <vector>
#include <set>
#include <map>
#include <string>
#include "DataTypes.h"
#include "MusicUtils.h"

class IntervalRecognizer
{
    private:
        std::map<std::string, std::set<int>> chordTemplates;

    public:
        IntervalRecognizer();
        //std::set<int> getPhysicalNotes(const ChordData& chord);
        //IntervalData defineInterval(const std::set<int>& pitches);
        //std::set<int> buildChord(const ChordCommand& cmdChord, const std::set<int>& selectedPitches);
};