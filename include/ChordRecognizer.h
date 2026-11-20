#pragma once
#include <vector>
#include <set>
#include <map>
#include <string>
#include "DataTypes.h"

/// @brief Analyzes note combinations to identify and construct chords and their inversions.
class ChordRecognizer
{
    private:
        std::map<std::string, std::set<int>> chordTemplates;
        std::set<int> collapseToOctave(const std::set<int>& pitches);
        std::set<int> inversionOfChord(const std::set<int>& pitches, int candidateRoot);
        int getBassInterval(const std::string& chordType, int inversionType) const;
        int findAnchorMidi(int bassClass, const std::set<int>& selectedPitches);
        ChordData defineChord(const std::set<int>& pitches);

    public:
        /// @brief Initializes default chord templates (e.g., major, minor).
        ChordRecognizer();

        /// @brief Retrieves the map of supported chord templates.
        /// @return Reference to map mapping chord type names to semitone offsets.
        const std::map<std::string, std::set<int>>& getTemplates() const { return chordTemplates; }

        /// @brief Computes physical MIDI pitch set for a concrete chord instance.
        /// @param chord Descriptor containing root, inversion, and lowest pitch.
        /// @return Set of MIDI notes forming the chord.
        std::set<int> getPhysicalNotes(const ChordData& chord);

        /// @brief Builds concrete MIDI notes from a high-level chord command relative to active pitches.
        /// @param cmdChord Requested chord specifications.
        /// @param selectedPitches Set of currently active MIDI pitches used as anchor references.
        /// @return Set of generated MIDI pitches.
        std::set<int> buildChord(const ChordCommand& cmdChord, const std::set<int>& selectedPitches);

        /// @brief Identifies all valid chords formed by subsets of given pitches.
        /// @param pitches Input set of MIDI pitches.
        /// @return Set of recognized ChordData instances.
        std::set<ChordData> findAllChords(const std::set<int>& pitches);
};
