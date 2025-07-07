/*
  * IDisplay.hpp - The interface for display abstraction layer.
  * It defines the methods for clearing the display, setting backlight,
  * and showing text on the display.
*/

#pragma once

#include "pico/stdlib.h"

class IDisplay {
public:
    virtual ~IDisplay() = default;
    virtual void Clear() = 0;
    virtual void SetBacklight(bool on) = 0;
    virtual void ShowText(int row, int col, const char* text) = 0;
    virtual void PrintCustomCharacter(uint8_t row, uint8_t col, uint8_t location);
};
