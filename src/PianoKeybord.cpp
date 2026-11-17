#include "../include/PianoKeyboard.h"
#include "../include/State.h"
#include <sstream>

std::string PianoKeyboard::getKey() const
{
    return "keyboard";
}

std::string PianoKeyboard::serialize(const State& state) const
{
    std::ostringstream oss;
    oss << "[";
    bool first = true;
    for (int pitch : state.getPitches())
    {
        if (!first)
        {
            oss << ",";
        }
        oss << pitch;
        first = false;
    }
    oss << "]";
    return oss.str();
}

std::string PianoKeyboard::getInitData() const
{
    std::ostringstream oss;
    oss << "{\"minPitch\":" << MIN_PITCH
        << ",\"maxPitch\":" << MAX_PITCH << "}";
    return oss.str();
}
