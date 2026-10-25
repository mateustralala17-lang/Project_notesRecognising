#pragma once
#include <vector>
#include <set>
#include "DataTypes.h"

class State
{
    protected:
        std::set<int> selectedPitches;
        std::vector<ChordData> selectedChords;
    
    public:
        void addPitch(int pitch);
        void removePitch(int pitch);
        void addChord(const ChordData& chord);
        void removeChord(const ChordData& chord);
        void clearSelectedChords();
        const std::set<int>& getPitches() const;
        const std::vector<ChordData>& getChords() const;
};