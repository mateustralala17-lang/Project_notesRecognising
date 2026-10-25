#include "../include/ChordRecognizer.h"
#include <algorithm>
#include <set>
#include <iterator>


ChordRecognizer::ChordRecognizer(){
    chordTemplates["major"] = {0, 4, 7};
    chordTemplates["minor"] = {0, 3, 7};
}

std::set<int>  ChordRecognizer::collapseToOctave(const std::set<int>& pitches){
    std::set<int> collapsedPitches;
    for (int pitch : pitches){
        int collapsedPitch = pitch % 12;
        collapsedPitches.insert(collapsedPitch);
    }
    return collapsedPitches;
}

std::set<int> ChordRecognizer::inversionOfChord(const std::set<int>& pitches, int candidateRoot){            
    std::set<int> invertedPitches;
    for (int pitch : pitches){
        int invertedPitch = (pitch - candidateRoot + 12) % 12;
        invertedPitches.insert(invertedPitch);
    }
    return invertedPitches;
}

int ChordRecognizer::getBassInterval(const std::string& chordType, int inversionType) const {    
    return *std::next(chordTemplates.at(chordType).begin(), inversionType);
}

int ChordRecognizer::findAnchorMidi(int bassClass, const std::set<int>& selectedPitches) {
    for (int pitch : selectedPitches) {                
        if (pitch % 12 == bassClass) {
            return pitch;
        }
    }            
    return 60 + bassClass; 
}

std::set<int> ChordRecognizer::getPhysicalNotes(const ChordData& chord){
    int bassInterval = getBassInterval(chord.chordType, chord.inversionType);
    const std::set<int>& degrees = chordTemplates.at(chord.chordType);
    int fantomRoot = chord.lowestPitch - bassInterval;
    std::set<int> physicalNotes;
    int i = 0;
    
    for (int degree : degrees) {                
        int octaveOffset = (i < chord.inversionType) ? 12 : 0;
        physicalNotes.insert(fantomRoot + degree + octaveOffset);
        ++i;
    }
    return physicalNotes;
}

ChordData ChordRecognizer::defineChord(const std::set<int>& pitches){
    if (pitches.empty()){
        return {-1, -1, -1, ""};
    }
    int rootNote = -1;
    std::set<int> chordTemplate;
    int lowestPitch;
    lowestPitch = *pitches.begin();
    int inversionType = 0;
    std::string chordType = "";
    bool found = false;

    std::set<int> collapsedPitches = collapseToOctave(pitches);            
    for (int candidateRoot : collapsedPitches){
        std::set<int> pitchesForComparison = inversionOfChord(collapsedPitches, candidateRoot);                
        for (const auto& i : chordTemplates){
            if (pitchesForComparison == i.second){
                rootNote = candidateRoot;                        
                chordTemplate = i.second;
                chordType = i.first;
                found = true;
                break;
            }
        }
        if (found){
            break;
        }
    }
    if (!found) {
        return {-1, -1, -1, ""};
    }

    int collapsedLowestPitch = lowestPitch % 12;            
    for (int interval : chordTemplate){
        if ((rootNote + interval) % 12 == collapsedLowestPitch){
            return {rootNote, inversionType, lowestPitch, chordType};
        }
        inversionType++;
    }
    return {-1, -1, -1, ""};
}

std::set<int> ChordRecognizer::buildChord(const ChordCommand& cmdChord, const std::set<int>& selectedPitches) {            
    int bassInterval = getBassInterval(cmdChord.chordType, cmdChord.inversionType);
    int bassClass = (cmdChord.rootNote + bassInterval) % 12;
    
    int anchorMidi = findAnchorMidi(bassClass, selectedPitches);

    ChordData chord;
    chord.rootNote = cmdChord.rootNote;
    chord.inversionType = cmdChord.inversionType;
    chord.chordType = cmdChord.chordType;
    chord.lowestPitch = anchorMidi;            
    
    return getPhysicalNotes(chord);
}
