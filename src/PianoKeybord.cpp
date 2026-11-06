#include "../include/PianoKeyboard.h"
#include <iostream>
using namespace std;

void PianoKeyboard::clearHighlights(){
    cout << "\n--- [ НОВИЙ КАДР ОНОВЛЕННЯ ] ---\n" << endl;
}

void PianoKeyboard::highlightNotes(const std::set<int>& pitches){
    cout << "[UI]: Горят клавіші (MIDI): ";
    if (pitches.empty()) {
        std::cout << "нічого не натиснуто\n";
    return;
    }

    for (int pitch : pitches) {                
        std::cout << pitch << " "; 
    }
    std::cout << "\n";
}

void PianoKeyboard::highlightChords(const std::set<ChordData>& chords){
    std::cout << "[UI]: На екрані горять акорди:\n";
    if (chords.empty()) {
        std::cout << "      (немає розпізнаних акордів)\n";
        return;
    }

    for (const ChordData& chord : chords) {
        std::cout << "      -> " << chord.chordType 
                << " (Тоніка: " << chord.rootNote 
                << ", Бас: " << chord.lowestPitch << ")\n";
    }
    std::cout << "====================================\n";
}

void PianoKeyboard::highlightIntervals(const std::set<IntervalData>& intervals){
    std::cout << "[UI]: На екрані горять інтервали:\n";
    if (intervals.empty()) {
        std::cout << "      (немає розпізнаних інтервалів)\n";
        return;
    }

    for (const IntervalData& interval : intervals) {
        std::cout << "      -> " << interval.intervalType 
                << " (Відстань: " << interval.distance 
                << ", Початкова нота: " << interval.lowestPitch << ")\n";
    }
    std::cout << "====================================\n";
}

void PianoKeyboard::draw(){
    
}