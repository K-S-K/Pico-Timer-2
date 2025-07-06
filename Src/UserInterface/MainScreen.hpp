#pragma once

#include "../Display/IDisplay.hpp"

class MainScreen
{
public:
    MainScreen(IDisplay* display)
        : display(display)
    {
    }

    void Render();

    void Clear() {
        display->Clear();
    }

private:
    IDisplay* display;
};