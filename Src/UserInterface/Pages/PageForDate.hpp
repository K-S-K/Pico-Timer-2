#pragma once

#include "../Display/IDisplay.hpp"

#include "../Menu/MenuEvent.h"
#include "InputElement.hpp"

enum class EventProcessingResult {
    Continue,
    Cancel,
    Apply,
};


class PageForDate
{
    public:
    PageForDate(IDisplay* display, int row, int col, DateTime valueIn)
    : display(display), row(row), col(col)
    {
        value.CopyFrom(valueIn);

        int i = 0;
        stopItems[i++] = new InputElement(display, 3, 0, InputElementType::Cancel);
        stopItems[i++] = new InputElement(display, 3, 0, InputElementType::Apply);
        stopItems[i++] = new InputElement(display, row + 1, col + 5, InputElementType::Data);
        stopItems[i++] = new InputElement(display, row + 1, col + 8, InputElementType::Data);
        stopItems[i++] = new InputElement(display, row + 1, col + 11, InputElementType::Data);

        MaxStopItemIndex = 4;
    }

    EventProcessingResult ProcessMenuEvent(MenuEvent event)
    {
        stopItems[CurrentStopItemIndex]->Render(InputElementMode ::Bypass); // Deactivate current item
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
        stopItems[CurrentStopItemIndex]->Render(InputElementMode ::Select); // Activate the current item
        // TODO: Also render with InputElementMode   ::Modify if needed
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
    InputElement *stopItems[5] = {nullptr, nullptr, nullptr, nullptr, nullptr};
    int CurrentStopItemIndex = 0;
    int MaxStopItemIndex = 0;
};
