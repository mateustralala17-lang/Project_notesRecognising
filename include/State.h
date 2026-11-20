#pragma once
#include <vector>
#include <set>
#include "DataTypes.h"

/// @brief Holds the current application state including active pitches, chords, and intervals.
class State
{
    protected:
        std::set<int> selectedPitches;           ///< Currently active MIDI pitches.
        std::set<ChordData> selectedChords;       ///< Recognized chords present in the active notes.
        std::set<IntervalData> selectedIntervals; ///< Recognized intervals present in the active notes.
    
    public:
        /// @brief Adds a MIDI pitch to the active pitch set.
        /// @param pitch MIDI pitch value.
        void addPitch(int pitch);

        /// @brief Removes a MIDI pitch from the active pitch set.
        /// @param pitch MIDI pitch value.
        void removePitch(int pitch);

        /// @brief Adds a recognized chord to the active chord set.
        /// @param chord ChordData descriptor to insert.
        void addChord(const ChordData& chord);

        /// @brief Removes a chord from the active chord set.
        /// @param chord ChordData descriptor to remove.
        void removeChord(const ChordData& chord);

        /// @brief Adds a recognized interval to the active interval set.
        /// @param interval IntervalData descriptor to insert.
        void addInterval(const IntervalData& interval);

        /// @brief Removes an interval from the active interval set.
        /// @param interval IntervalData descriptor to remove.
        void removeInterval(const IntervalData& interval);

        /// @brief Clears all recognized chords from state.
        void clearSelectedChords();

        /// @brief Clears all recognized intervals from state.
        void clearSelectedIntervals();

        /// @brief Accesses the set of active MIDI pitches.
        /// @return Immutable reference to the active pitch set.
        const std::set<int>& getPitches() const;

        /// @brief Accesses the set of recognized chords.
        /// @return Immutable reference to the recognized chords set.
        const std::set<ChordData>& getChords() const;

        /// @brief Accesses the set of recognized intervals.
        /// @return Immutable reference to the recognized intervals set.
        const std::set<IntervalData>& getIntervals() const;
};
