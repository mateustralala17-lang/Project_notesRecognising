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
};

struct ChordCommand
{
    int rootNote;
    int inversionType;
    std::string chordType;
};