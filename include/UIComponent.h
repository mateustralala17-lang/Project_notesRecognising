#pragma once

class UIComponent{
    public:
        virtual ~UIComponent() = default;
        virtual void draw() = 0;
};