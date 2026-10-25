#include "../include/AppController.h"
#include <iostream>
#include <windows.h>
#include <string>
#include <set>
using namespace std;

AppController::AppController(PianoKeyboard* p){
    myPiano = p;
}

void AppController::updateAll(){
    ChordData chord = chordRecognizer.defineChord(currentState.getPitches());
    currentState.clearSelectedChords();
    if (chord.rootNote != -1){
        currentState.addChord(chord);
    }
    myPiano->clearHighlights();
    myPiano->highlightNotes(currentState.getPitches());
    myPiano->highlightChords(currentState.getChords());
}

void AppController::press (int clickedPitch){
    currentState.addPitch(clickedPitch);
    updateAll();
}

void AppController::press (const ChordCommand& chord){
    set<int> notesToAdd = chordRecognizer.buildChord(chord, currentState.getPitches());
    for (int note : notesToAdd){
        currentState.addPitch(note);
    }
    updateAll();
}

void AppController::release (int clickedPitch){
    currentState.removePitch(clickedPitch);
    updateAll();
}

void AppController::release (const ChordCommand& chord){
    for (const ChordData& activeChord : currentState.getChords()){
        if (activeChord.rootNote == chord.rootNote && activeChord.chordType == chord.chordType){
            set<int> notesToRemove = chordRecognizer.getPhysicalNotes(activeChord);
            for (int note : notesToRemove){
                currentState.removePitch(note);
            }
        }
    }
    updateAll();
}