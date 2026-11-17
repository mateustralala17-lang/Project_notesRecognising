#include "../include/IntervalMatrix.h"
#include "../include/State.h"
#include <sstream>

std::string IntervalMatrix::getKey() const
{
    return "intervals";
}

std::string IntervalMatrix::serialize(const State& state) const
{
    std::ostringstream oss;
    oss << "[";
    bool first = true;
    for (const IntervalData& interval : state.getIntervals())
    {
        if (!first)
        {
            oss << ",";
        }
        oss << "{\"lowestPitch\":" << interval.lowestPitch
            << ",\"distance\":" << interval.distance
            << ",\"intervalType\":\"" << interval.intervalType << "\"}";
        first = false;
    }
    oss << "]";
    return oss.str();
}

std::string IntervalMatrix::getInitData() const
{
    const auto& templates = intervalRecognizer.getTemplates();

    std::ostringstream oss;
    oss << "{\"availableIntervals\":[";
    bool first = true;
    for (const auto& entry : templates)
    {
        if (!first)
        {
            oss << ",";
        }
        oss << "{\"distance\":" << entry.first
            << ",\"intervalType\":\"" << entry.second << "\"}";
        first = false;
    }
    oss << "]}";
    return oss.str();
}

