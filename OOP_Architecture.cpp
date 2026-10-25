#include <windows.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <set>
#include <map>
using namespace std;

struct ChordData
{
    int rootNote;
    int inversionType;
    int lowestPitch;
    string chordType;
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
    string chordType;
};

class AudioPlayer
{
    public:
        void playNote(){
            cout << "Играю ноту" << endl;
        }
};

class State
{
    protected:
        set<int> selectedPitches;
        vector<ChordData> selectedChords;
    
    public:
        void addPitch(int pitch){
            selectedPitches.insert(pitch);
        }
        void removePitch(int pitch){
            selectedPitches.erase(pitch);
        }
        void addChord(ChordData chord){
            selectedChords.push_back(chord);
        }
        void removeChord(ChordData chord){
            selectedChords.erase(remove(selectedChords.begin(), selectedChords.end(), chord), selectedChords.end());
        }
        void clearSelectedChords(){
            selectedChords.clear();
        }
        const set<int>& getPitches() const { 
            return selectedPitches; 
        }
        const vector<ChordData>& getChords() const {
            return selectedChords;
        }
};



class ChordRecognizer
{
    private:
        map<string, set<int>> chordTemplates;

    public:
        ChordRecognizer(){
            chordTemplates["major"] = {0, 4, 7};
            chordTemplates["minor"] = {0, 3, 7};
        }

        set<int> collapseToOctave(const set<int>& pitches){
            set<int> collapsedPitches;
            for (int pitch : pitches){
                int collapsedPitch = pitch % 12;
                collapsedPitches.insert(collapsedPitch);
            }
            return collapsedPitches;
        }

        set<int> inversionOfChord(const set<int>& pitches, int candidateRoot){            
            set<int> invertedPitches;
            for (int pitch : pitches){
                int invertedPitch = (pitch - candidateRoot + 12) % 12;
                invertedPitches.insert(invertedPitch);
            }
            return invertedPitches;
        }

        int getBassInterval(const string& chordType, int inversionType) const {    
            return *std::next(chordTemplates.at(chordType).begin(), inversionType);
        }
        
        int findAnchorMidi(int bassClass, const set<int>& selectedPitches) {
            for (int pitch : selectedPitches) {                
                if (pitch % 12 == bassClass) {
                    return pitch;
                }
            }            
            return 60 + bassClass; 
        }

        set<int> getPhysicalNotes(const ChordData& chord){
            int bassInterval = getBassInterval(chord.chordType, chord.inversionType);
            const set<int>& degrees = chordTemplates.at(chord.chordType);
            int fantomRoot = chord.lowestPitch - bassInterval;
            set<int> physicalNotes;
            int i = 0;
            
            for (int degree : degrees) {                
                int octaveOffset = (i < chord.inversionType) ? 12 : 0;
                physicalNotes.insert(fantomRoot + degree + octaveOffset);
                ++i;
            }
            return physicalNotes;
        }
        
        ChordData defineChord(const set<int>& pitches){
            if (pitches.empty()){
                return {-1, -1, -1, ""};
            }
            int rootNote = -1;
            set<int> chordTemplate;
            int lowestPitch;
            lowestPitch = *pitches.begin();
            int inversionType = 0;
            string chordType = "";
            bool found = false;

            set<int> collapsedPitches = collapseToOctave(pitches);            
            for (int candidateRoot : collapsedPitches){
                set<int> pitchesForComparison = inversionOfChord(collapsedPitches, candidateRoot);                
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

        set<int> buildChord(const ChordCommand& cmdChord, const set<int>& selectedPitches) {            
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
};


class PianoKeyboard
{
    private:

    public:
        void clearHighlights(){
            cout << "\n--- [ НОВЫЙ КАДР ОБНОВЛЕНИЯ ] ---\n" << endl;
        }

        void highlightNotes(const std::set<int>& pitches){
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

        void highlightChords(const std::vector<ChordData>& chords){
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
};

class AppController
{
    private:
        PianoKeyboard* myPiano;
        State currentState;
        ChordRecognizer chordRecognizer;
    public:
        AppController(PianoKeyboard* p){
            myPiano = p;
        }

        void updateAll(){
            ChordData chord = chordRecognizer.defineChord(currentState.getPitches());
            currentState.clearSelectedChords();
            if (chord.rootNote != -1){
                currentState.addChord(chord);
            }
            myPiano->clearHighlights();
            myPiano->highlightNotes(currentState.getPitches());
            myPiano->highlightChords(currentState.getChords());
        }
        
        void press (int clickedPitch){
            currentState.addPitch(clickedPitch);
            updateAll();
        }

        void press (const ChordCommand& chord){
            set<int> notesToAdd = chordRecognizer.buildChord(chord, currentState.getPitches());
            for (int note : notesToAdd){
                currentState.addPitch(note);
            }
            updateAll();
        }

        void release (int clickedPitch){
            currentState.removePitch(clickedPitch);
            updateAll();
        }

        void release (const ChordCommand& chord){
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
};

int main()
{
    SetConsoleOutputCP(CP_UTF8);
    PianoKeyboard myPiano;
    AppController controller(&myPiano);

    std::cout << "\n=== ТЕСТ 1: Определение аккорда по 3 нотам (До мажор) ===" << std::endl;
    controller.press(60); // До
    controller.press(64); // Ми
    controller.press(67); // Соль 
    // Ожидаем: горят 60, 64, 67. Аккорд: major, Тоника: 0, Бас: 60.

    std::cout << "\n=== ТЕСТ 2: Забывание аккорда при отжатии одной ноты ===" << std::endl;
    controller.release(64); 
    // Ожидаем: горят 60, 67. Аккордов нет.

    std::cout << "\n=== ТЕСТ 3: Очистка клавиатуры ===" << std::endl;
    controller.release(60);
    controller.release(67);
    // Ожидаем: ничего не нажато.

    std::cout << "\n=== ТЕСТ 4: Построение аккорда в пустой октаве (Ре минор) ===" << std::endl;
    ChordCommand dMinor{2, 0, "minor"}; // Тоника Ре (2), Основной вид (0)
    controller.press(dMinor);
    // Ожидаем: горят 62, 65, 69. Аккорд: minor, Тоника: 2, Бас: 62.

    std::cout << "\n=== ТЕСТ 5: Удаление всего аккорда одной командой ===" << std::endl;
    controller.release(dMinor);
    // Ожидаем: ничего не нажато (все 3 ноты стерты).

    std::cout << "\n=== ТЕСТ 6: Умное построение аккорда с привязкой и обращением ===" << std::endl;
    controller.press(52); // Пользователь зажал ноту Ми (64)
    
    // Команда: построй До мажор, но в ПЕРВОМ обращении (Ми-Соль-До)
    ChordCommand cMajor1stInv{0, 1, "major"}; 
    controller.press(cMajor1stInv);
    // Ожидаем: программа найдет уже зажатую Ми (64) как бас, достроит 67 и 72. 
    // На экране: горят 64, 67, 72. Аккорд: major, Тоника: 0, Бас: 64.

    return 0;
}