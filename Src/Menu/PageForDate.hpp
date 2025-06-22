#pragma once

#include "../Display/IDisplay.hpp"

#include "MenuEvent.h"

enum class MenuStopType {
    Cancel,
    Data,
    Apply,
};

enum class EventProcessingResult {
    Continue,
    Cancel,
    Apply,
};


class StopItem
{
    public:
    StopItem(IDisplay* display, int row, int col, MenuStopType type)
    : display(display), row(row), col(col), type(type) {}

    void Render(bool isActive)
    {
        const char* label = isActive? 
            (type == MenuStopType::Cancel) ? "Cancel" :
            (type == MenuStopType::Apply) ? "Apply" :
            (type == MenuStopType::Data) ? "^" : "?" :  // 251
            (type == MenuStopType::Cancel) ? "      " :
            (type == MenuStopType::Apply) ? "     " :
            (type == MenuStopType::Data) ? " " : "?" ;  // 251

        display->ShowText(3, 0, hintClear); // Clear previous text

        const char* hint = isActive ? 
            (type == MenuStopType::Cancel) ? "Cancel" :
            (type == MenuStopType::Apply) ? "Apply" :
            (type == MenuStopType::Data) ? "Select" : "Modify" : "";

        if(type == MenuStopType::Data) {
            display->ShowText(row, col, label);
        }

        display->ShowText(3, 0, hint);
    }

    MenuStopType type;

    private:
    int row;
    int col;
    IDisplay* display;

    const char* hintSelect = "Select";
    const char* hintModify = "Modify";
    const char* hintClear =  "      ";
};


class PageForDate
{
    public:
    PageForDate(IDisplay* display, int row, int col, DateTime valueIn)
    : display(display), row(row), col(col)
    {
        value.CopyFrom(valueIn);

        int i = 0;
        stopItems[i++] = new StopItem(display, 3, 0, MenuStopType::Cancel);
        stopItems[i++] = new StopItem(display, 3, 0, MenuStopType::Apply);
        stopItems[i++] = new StopItem(display, row + 1, col + 5, MenuStopType::Data);
        stopItems[i++] = new StopItem(display, row + 1, col + 8, MenuStopType::Data);
        stopItems[i++] = new StopItem(display, row + 1, col + 11, MenuStopType::Data);

        MaxStopItemIndex = 4;
    }

    EventProcessingResult ProcessMenuEvent(MenuEvent event)
    {
        stopItems[CurrentStopItemIndex]->Render(false); // Deactivate current item
        switch (event) {
            case MenuEvent::MoveFwd:
                if(CurrentStopItemIndex >= MaxStopItemIndex) {
                    CurrentStopItemIndex = 0; // Loop back to the first item
                }
                else {
                    CurrentStopItemIndex++;
                }
                break;
            case MenuEvent::MoveBack:
            if(CurrentStopItemIndex <= 0) {
                    CurrentStopItemIndex = MaxStopItemIndex; // Loop back to the last item
                }
                else {
                    CurrentStopItemIndex--;
                }
                break;
            case MenuEvent::PushButton:
                // Handle button press if needed
                break;
        }
        stopItems[CurrentStopItemIndex]->Render(true); // Activate the current item
        // switch (event) {
        //     case MenuEvent::MoveFwd:
        //         value.incrementDay();
        //         break;
        //     case MenuEvent::MoveBack:
        //         value.DecrementDay();
        //         break;
        //     case MenuEvent::PushButton:
        //         // Handle button press if needed
        //         break;
        // }
        
        Render();

        return EventProcessingResult::Continue;
    }

    void Render()
    {
        char buffer[32];
        snprintf(buffer, sizeof(buffer), ": %04d.%02d.%02d", value.year, value.month, value.day);
        display->ShowText(row, col, buffer);
    }

    private:
    IDisplay* display;
    int row;
    int col;

    DateTime value;
    StopItem *stopItems[5] = {nullptr, nullptr, nullptr, nullptr, nullptr};
    int CurrentStopItemIndex = 0;
    int MaxStopItemIndex = 0;
};


