/*
    * IDisplay.hpp - The interface for display operations.
*/

#pragma once

class IDisplay {
public:
    virtual ~IDisplay() = default;
    virtual void Clear() = 0;
    virtual void SetBacklight(bool on) = 0;
    virtual void ShowText(int row, int col, const char* text) = 0;
};
