#pragma once
#include <string>

/// @brief Represents a recognized musical chord and its physical layout.
struct ChordData
{
    int rootNote;       ///< Root note pitch class (0 = C, 1 = C#, ..., 11 = B).
    int inversionType;  ///< Chord inversion index (0 = root position, 1 = 1st inversion, etc.).
    int lowestPitch;    ///< Lowest MIDI pitch played in the chord.
    std::string chordType; ///< Quality of the chord (e.g., "major", "minor").

    /// @brief Equality comparison operator.
    /// @param other Another ChordData instance to compare against.
    /// @return True if all fields match, false otherwise.
    bool operator==(const ChordData& other) const {
        return rootNote == other.rootNote && 
               inversionType == other.inversionType && 
               lowestPitch == other.lowestPitch && 
               chordType == other.chordType;
    }

    /// @brief Strict weak ordering operator for set storage.
    /// @param other Another ChordData instance to compare against.
    /// @return True if this instance orders before other.
    bool operator<(const ChordData& other) const {
        if (rootNote != other.rootNote) return rootNote < other.rootNote;
        if (chordType != other.chordType) return chordType < other.chordType;
        if (lowestPitch != other.lowestPitch) return lowestPitch < other.lowestPitch;
        return inversionType < other.inversionType;
    }
};

/// @brief Command parameters for constructing or releasing a chord.
struct ChordCommand
{
    int rootNote;          ///< Root note pitch class (0 to 11).
    int inversionType;     ///< Target inversion index.
    std::string chordType; ///< Target chord quality (e.g., "major", "minor").
};

/// @brief Represents a recognized musical interval between two pitches.
struct IntervalData
{
    int lowestPitch;          ///< Lower MIDI pitch of the interval pair.
    int distance;             ///< Semitone distance between pitches.
    std::string intervalType; ///< Descriptive name (e.g., "major third", "perfect fifth").

    /// @brief Equality comparison operator.
    /// @param other Another IntervalData instance to compare against.
    /// @return True if lowestPitch and distance match.
    bool operator==(const IntervalData& other) const {
        return lowestPitch == other.lowestPitch && distance == other.distance;
    }

    /// @brief Strict weak ordering operator for set storage.
    /// @param other Another IntervalData instance to compare against.
    /// @return True if this instance orders before other.
    bool operator<(const IntervalData& other) const {
        if (lowestPitch != other.lowestPitch) return lowestPitch < other.lowestPitch;
        if (distance != other.distance) return distance < other.distance;
        return intervalType < other.intervalType;
    }
};

/// @brief Command parameters for constructing or releasing an interval.
struct IntervalCommand
{
    int distance;             ///< Distance in semitones.
    std::string intervalType; ///< Descriptive name of the interval.
};
