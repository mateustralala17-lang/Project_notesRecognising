#include "../include/State.h"
#include <algorithm>

void State::addPitch(int pitch){
    selectedPitches.insert(pitch);
}

void State::removePitch(int pitch){
    selectedPitches.erase(pitch);
}

void State::addChord(const ChordData& chord){
    selectedChords.push_back(chord);
}

void State::removeChord(const ChordData& chord){
    selectedChords.erase(std::remove(selectedChords.begin(), selectedChords.end(), chord), selectedChords.end());
}

void State::clearSelectedChords(){
    selectedChords.clear();
}

const std::set<int>& State::getPitches() const {
    return selectedPitches;
}

const std::vector<ChordData>& State::getChords() const {
    return selectedChords;
}
