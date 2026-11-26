#include "../include/AppController.h"
#include <set>
#include <string>

using namespace std;

void AppController::updateAll()
{
    currentState.clearSelectedChords();
    currentState.clearSelectedIntervals();
    std::set<ChordData> chords =
        chordRecognizer.findAllChords(currentState.getPitches());
    for (const ChordData &chord : chords)
    {
        currentState.addChord(chord);
    }
    std::set<IntervalData> intervals =
        intervalRecognizer.findAllIntervals(currentState.getPitches());
    for (const IntervalData &interval : intervals)
    {
        currentState.addInterval(interval);
    }
}

void AppController::press(int clickedPitch)
{
    currentState.addPitch(clickedPitch);
    updateAll();
}

void AppController::press(const ChordCommand &chord)
{
    set<int> notesToAdd =
        chordRecognizer.buildChord(chord, currentState.getPitches());
    for (int note : notesToAdd)
    {
        currentState.addPitch(note);
    }
    updateAll();
}

void AppController::press(const IntervalCommand &interval)
{
    std::set<int> notesToAdd =
        intervalRecognizer.buildInterval(interval, currentState.getPitches());
    for (int note : notesToAdd)
    {
        currentState.addPitch(note);
    }
    updateAll();
}

void AppController::release(int clickedPitch)
{
    currentState.removePitch(clickedPitch);
    updateAll();
}

void AppController::release(const ChordCommand &chord)
{
    for (const ChordData &activeChord : currentState.getChords())
    {
        if (activeChord.rootNote == chord.rootNote &&
            activeChord.chordType == chord.chordType)
        {
            set<int> notesToRemove = chordRecognizer.getPhysicalNotes(activeChord);
            for (int note : notesToRemove)
            {
                currentState.removePitch(note);
            }
        }
    }
    updateAll();
}

void AppController::release(const IntervalCommand &interval)
{
    for (const IntervalData &activeInterval : currentState.getIntervals())
    {
        if (activeInterval.distance == interval.distance)
        {
            currentState.removePitch(activeInterval.lowestPitch);
            currentState.removePitch(activeInterval.lowestPitch +
                                     activeInterval.distance);
        }
    }
    updateAll();
}

void AppController::releaseAll()
{
    currentState.releaseAll();
    updateAll();
}
