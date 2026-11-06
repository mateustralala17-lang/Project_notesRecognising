#include "../include/State.h"
#include <algorithm>

void State::addPitch(int pitch){
    selectedPitches.insert(pitch);
}

void State::removePitch(int pitch){
    selectedPitches.erase(pitch);
}

void State::addChord(const ChordData& chord){
    selectedChords.insert(chord);
}

void State::removeChord(const ChordData& chord){
    selectedChords.erase(chord);
}

void State::addInterval(const IntervalData& interval){
    selectedIntervals.insert(interval);
}

void State::removeInterval(const IntervalData& interval){
    selectedIntervals.erase(interval);
}

void State::clearSelectedChords(){
    selectedChords.clear();
}

void State::clearSelectedIntervals(){
    selectedIntervals.clear();
}

const std::set<int>& State::getPitches() const {
    return selectedPitches;
}

const std::set<ChordData>& State::getChords() const {
    return selectedChords;
}

const std::set<IntervalData>& State::getIntervals() const {
    return selectedIntervals;
}
