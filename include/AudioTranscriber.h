#pragma once
#include <string>
#include <vector>
#include "DataTypes.h"

/// @brief Invokes an external Python transcription script and parses MIDI note events.
class AudioTranscriber
{
private:
    std::string commandPrefix; ///< E.g. "python scripts/basicPitch.py"

public:
    explicit AudioTranscriber(std::string cmdPrefix);

    /// @brief Transcribes an audio file into a sorted sequence of NoteEvent structs.
    std::vector<NoteEvent> transcribe(const std::string& audioFilePath) const;
};