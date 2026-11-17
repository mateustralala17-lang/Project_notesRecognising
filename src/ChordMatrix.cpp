#include "../include/ChordMatrix.h"
#include "../include/State.h"
#include <sstream>

std::string ChordMatrix::getKey() const
{
    return "chords";
}

std::string ChordMatrix::serialize(const State& state) const
{
    std::ostringstream oss;
    oss << "[";
    bool first = true;
    for (const ChordData& chord : state.getChords())
    {
        if (!first)
        {
            oss << ",";
        }
        oss << "{\"rootNote\":" << chord.rootNote
            << ",\"chordType\":\"" << chord.chordType << "\""
            << ",\"inversionType\":" << chord.inversionType
            << ",\"lowestPitch\":" << chord.lowestPitch << "}";
        first = false;
    }
    oss << "]";
    return oss.str();
}

std::string ChordMatrix::getInitData() const
{
    const auto& templates = chordRecognizer.getTemplates();

    std::ostringstream oss;
    oss << "{\"types\":[";
    bool first = true;
    int maxSize = 0;
    for (const auto& entry : templates)
    {
        if (!first)
        {
            oss << ",";
        }
        oss << "\"" << entry.first << "\"";
        if (static_cast<int>(entry.second.size()) > maxSize)
        {
            maxSize = static_cast<int>(entry.second.size());
        }
        first = false;
    }
    oss << "],\"inversions\":[";
    for (int i = 0; i < maxSize; ++i)
    {
        if (i > 0)
        {
            oss << ",";
        }
        oss << i;
    }
    oss << "]}";
    return oss.str();
}

