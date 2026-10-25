#pragma once
#include <vector>
#include <set>
#include <string>
#include "DataTypes.h"

class PianoKeyboard
{
    private:

    public:
        void clearHighlights();
        void highlightNotes(const std::set<int>& pitches);
        void highlightChords(const std::vector<ChordData>& chords);
};