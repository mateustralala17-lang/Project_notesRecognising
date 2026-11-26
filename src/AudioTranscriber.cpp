#include "../include/AudioTranscriber.h"
#include <iostream>
#include <cstdio>
#include <sstream>
#include <utility>

AudioTranscriber::AudioTranscriber(std::string cmdPrefix)
    : commandPrefix(std::move(cmdPrefix))
{
}

std::vector<NoteEvent> AudioTranscriber::transcribe(const std::string& audioFilePath) const
{
    std::string fullCmd = commandPrefix + " \"" + audioFilePath + "\"";

    FILE* pipe = _popen(fullCmd.c_str(), "r");
    if (!pipe)
    {
        return {};
    }

    std::string output;
    char buffer[256];
    while (std::fgets(buffer, sizeof(buffer), pipe) != nullptr)
    {
        output += buffer;
    }
    _pclose(pipe);

    std::vector<NoteEvent> events;
    std::istringstream iss(output);
    std::string line;

    while (std::getline(iss, line))
    {
        if (line.rfind("NOTE ", 0) == 0)
        {
            std::istringstream lineStream(line.substr(5));
            double start = 0.0;
            double end = 0.0;
            int pitch = 0;
            if (lineStream >> start >> end >> pitch)
            {
                events.push_back({start, end, pitch});
            }
        }
    }

    return events;
}