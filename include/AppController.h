#pragma once
#include <vector>
#include <set>
#include <map>
#include <string>
#include "DataTypes.h"
#include "ChordRecognizer.h"
#include "IntervalRecognizer.h"
#include "State.h"

/// @brief Coordinates user inputs, business logic, and recognizers to mutate application state.
class AppController
{
    private:
        State currentState;                    ///< Central application state model.
        ChordRecognizer chordRecognizer;        ///< Engine for recognizing and constructing chords.
        IntervalRecognizer intervalRecognizer;  ///< Engine for recognizing and constructing intervals.
        void updateAll();
    public:
        /// @brief Default constructor initializing default controller and recognizer state.
        AppController() {};

        /// @brief Retrieves an immutable reference to the application state.
        /// @return Const reference to currentState.
        const State& getState() const { return currentState; }

        /// @brief Presses a single note by MIDI pitch.
        /// @param clickedPitch MIDI pitch value.
        void press (int clickedPitch);

        /// @brief Builds and presses notes corresponding to a chord command.
        /// @param chord Parameters specifying the chord to construct.
        void press (const ChordCommand& chord);

        /// @brief Builds and presses notes corresponding to an interval command.
        /// @param interval Parameters specifying the interval to construct.
        void press (const IntervalCommand& interval);

        /// @brief Releases a single note by MIDI pitch.
        /// @param clickedPitch MIDI pitch value.
        void release (int clickedPitch);

        /// @brief Releases notes belonging to an active chord matching the command.
        /// @param chord Parameters identifying the chord to release.
        void release (const ChordCommand& chord);

        /// @brief Releases notes belonging to active intervals matching the command.
        /// @param interval Parameters identifying the interval to release.
        void release (const IntervalCommand& interval);
};
