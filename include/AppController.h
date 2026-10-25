#pragma once
#include <vector>
#include <set>
#include <map>
#include <string>
#include "DataTypes.h"
#include "ChordRecognizer.h"
#include "PianoKeyboard.h"
#include "State.h"

class AppController
{
    private:
        PianoKeyboard* myPiano;
        State currentState;
        ChordRecognizer chordRecognizer;
    public:
        AppController(PianoKeyboard* p);
        void updateAll();
        void press (int clickedPitch);
        void press (const ChordCommand& chord);
        void release (int clickedPitch);
        void release (const ChordCommand& chord);
};