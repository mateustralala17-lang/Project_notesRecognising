#pragma once
#include <string>

class State;

class UIComponent
{
    public:
        virtual ~UIComponent() = default;
        virtual std::string getKey() const = 0;
        virtual std::string serialize(const State& state) const = 0;
        virtual std::string getInitData() const = 0;
};
