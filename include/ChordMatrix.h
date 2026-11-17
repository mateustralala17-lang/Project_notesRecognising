#pragma once
#include "UIComponent.h"
#include "ChordRecognizer.h"

class ChordMatrix : public UIComponent
{
    private:
        ChordRecognizer chordRecognizer;

    public:
        ChordMatrix() {};
        std::string getKey() const override;
        std::string serialize(const State& state) const override;
        std::string getInitData() const override;
};

