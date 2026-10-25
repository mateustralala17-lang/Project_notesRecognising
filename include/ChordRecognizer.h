#pragma once
#include <vector>
#include <set>
#include <map>
#include <string>
#include "DataTypes.h"

class ChordRecognizer
{
    private:
        std::map<std::string, std::set<int>> chordTemplates;

    public:
        ChordRecognizer();
        std::set<int> collapseToOctave(const std::set<int>& pitches);
        std::set<int> inversionOfChord(const std::set<int>& pitches, int candidateRoot);
        int getBassInterval(const std::string& chordType, int inversionType) const;
        int findAnchorMidi(int bassClass, const std::set<int>& selectedPitches);
        std::set<int> getPhysicalNotes(const ChordData& chord);
        ChordData defineChord(const std::set<int>& pitches);
        std::set<int> buildChord(const ChordCommand& cmdChord, const std::set<int>& selectedPitches);
};