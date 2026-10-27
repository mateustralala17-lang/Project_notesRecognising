#pragma once
#include <vector>
#include <set>
#include <string>
#include "DataTypes.h"
#include "UIComponent.h"

class PianoKeyboard : public UIComponent
{
    private:

    public:
        PianoKeyboard() {};
        void draw() override;
        void clearHighlights();
        void highlightNotes(const std::set<int>& pitches);
        void highlightChords(const std::vector<ChordData>& chords);
};