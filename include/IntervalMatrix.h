#pragma once
#include "UIComponent.h"
#include "IntervalRecognizer.h"

class IntervalMatrix : public UIComponent
{
    private:
        IntervalRecognizer intervalRecognizer;

    public:
        IntervalMatrix() {};
        std::string getKey() const override;
        std::string serialize(const State& state) const override;
        std::string getInitData() const override;
};

