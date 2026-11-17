#pragma once
#include "UIComponent.h"

class PianoKeyboard : public UIComponent
{
    public:
        static constexpr int MIN_PITCH = 21;
        static constexpr int MAX_PITCH = 108;

        PianoKeyboard() {};
        std::string getKey() const override;
        std::string serialize(const State& state) const override;
        std::string getInitData() const override;
};
