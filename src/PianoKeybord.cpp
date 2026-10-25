#include "../include/PianoKeyboard.h"
#include <iostream>
using namespace std;

void PianoKeyboard::clearHighlights(){
    cout << "\n--- [ НОВЫЙ КАДР ОБНОВЛЕНИЯ ] ---\n" << endl;
}

void PianoKeyboard::highlightNotes(const std::set<int>& pitches){
    cout << "[UI]: Горят клавиши (MIDI): ";
    if (pitches.empty()) {
        std::cout << "ничего не нажато\n";
    return;
    }

    for (int pitch : pitches) {                
        std::cout << pitch << " "; 
    }
    std::cout << "\n";
}

void PianoKeyboard::highlightChords(const std::vector<ChordData>& chords){
    std::cout << "[UI]: На экране горят аккорды:\n";
    if (chords.empty()) {
        std::cout << "      (нет распознанных аккордов)\n";
        return;
    }

    for (const ChordData& chord : chords) {
        std::cout << "      -> " << chord.chordType 
                << " (Тоника: " << chord.rootNote 
                << ", Бас: " << chord.lowestPitch << ")\n";
    }
    std::cout << "====================================\n";
}