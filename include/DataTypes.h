#pragma once
#include <string>

struct ChordData
{
    int rootNote;
    int inversionType;
    int lowestPitch;
    std::string chordType;
    bool operator==(const ChordData& other) const {
        return rootNote == other.rootNote && 
               inversionType == other.inversionType && 
               lowestPitch == other.lowestPitch && 
               chordType == other.chordType;
    }
    bool operator<(const ChordData& other) const {
        if (rootNote != other.rootNote) return rootNote < other.rootNote;
        if (chordType != other.chordType) return chordType < other.chordType;
        if (lowestPitch != other.lowestPitch) return lowestPitch < other.lowestPitch;
        return inversionType < other.inversionType;
    }
};

struct ChordCommand
{
    int rootNote;
    int inversionType;
    std::string chordType;
};

struct IntervalData
{
    int lowestPitch;
    int distance;
    std::string intervalType;
    bool operator==(const IntervalData& other) const {
        return lowestPitch == other.lowestPitch && distance == other.distance;
    }
    bool operator<(const IntervalData& other) const {
        if (lowestPitch != other.lowestPitch) return lowestPitch < other.lowestPitch;
        if (distance != other.distance) return distance < other.distance;
        return intervalType < other.intervalType;
    }
};

struct IntervalCommand
{
    int distance;
    std::string intervalType;
};