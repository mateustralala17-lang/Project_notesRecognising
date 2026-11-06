#pragma once
#include <vector>
#include <set>
#include "DataTypes.h"

class State
{
    protected:
        std::set<int> selectedPitches;
        std::set<ChordData> selectedChords;
        std::set<IntervalData> selectedIntervals;
    
    public:
        void addPitch(int pitch);
        void removePitch(int pitch);
        void addChord(const ChordData& chord);
        void removeChord(const ChordData& chord);
        void addInterval(const IntervalData& interval);
        void removeInterval(const IntervalData& interval);
        void clearSelectedChords();
        void clearSelectedIntervals();
        const std::set<int>& getPitches() const;
        const std::set<ChordData>& getChords() const;
        const std::set<IntervalData>& getIntervals() const;
};