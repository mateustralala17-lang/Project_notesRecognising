#pragma once
#include <vector>
#include <set>
#include <map>
#include <string>
#include "DataTypes.h"
#include "ChordRecognizer.h"
#include "IntervalRecognizer.h"
#include "State.h"

class AppController
{
    private:
        State currentState;
        ChordRecognizer chordRecognizer;
        IntervalRecognizer intervalRecognizer;
        void updateAll();
    public:
        AppController() {};
        const State& getState() const { return currentState; }
        void press (int clickedPitch);
        void press (const ChordCommand& chord);
        void press (const IntervalCommand& interval);
        void release (int clickedPitch);
        void release (const ChordCommand& chord);
        void release (const IntervalCommand& interval);
};
