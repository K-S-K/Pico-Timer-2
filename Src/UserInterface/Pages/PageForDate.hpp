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
        currentValue.CopyFrom(valueIn);

        int i = 0;
        elements[i++] = new InputElement(display, 3, 0, InputElementType::Cancel);
        elements[i++] = new InputElement(display, row + 1, col + 5, InputElementType::Data, &PageForDate::AlterYearThunk, this);
        elements[i++] = new InputElement(display, row + 1, col + 8, InputElementType::Data, &PageForDate::AlterMonthThunk, this);
        elements[i++] = new InputElement(display, row + 1, col + 11, InputElementType::Data, &PageForDate::AlterDayThunk, this);
        elements[i++] = new InputElement(display, 3, 0, InputElementType::Apply);

        MaxStopItemIndex = i - 1;
    }

    EventProcessingResult ProcessMenuEvent(MenuEvent event)
    {
        if(isEditing)
        {
            elements[CurrentElementIndex]->ProcessUserInput(event);
        }
        else
        {
            elements[CurrentElementIndex]->Render(InputElementMode::Bypass); // Deactivate current item
            switch (event) {
                case MenuEvent::MoveFwd:
                    if(CurrentElementIndex >= MaxStopItemIndex) {
                        CurrentElementIndex = MaxStopItemIndex;
                        return EventProcessingResult::Continue;
                    }
                    
                    CurrentElementIndex++;
                    break;

                case MenuEvent::MoveBack:
                    if(CurrentElementIndex <= 0) {
                        CurrentElementIndex = 0;
                        return EventProcessingResult::Continue;
                    }
                    
                    CurrentElementIndex--;
                    break;

                case MenuEvent::PushButton:
                    if(elements[CurrentElementIndex]->type == InputElementType::Cancel)
                    {
                        // If Cancel is pressed, exit editing mode without saving
                        return EventProcessingResult::Cancel;
                    }
                    else if(elements[CurrentElementIndex]->type == InputElementType::Apply)
                    {
                        // If Apply is pressed, save the current currentValue and exit editing mode
                        return EventProcessingResult::Apply;
                    }
                    else
                    {
                        // Switch to editing mode
                        isEditing = true;
                    }
                    break;
            }
        }
        
        Render();

        return EventProcessingResult::Continue;
    }

    void Render()
    {
        char buffer[32];
        snprintf(buffer, sizeof(buffer), ": %04d.%02d.%02d", currentValue.year, currentValue.month, currentValue.day);
        display->ShowText(row, col, buffer);

        // Render the current item in the appropriate mode
        elements[CurrentElementIndex]->Render(isEditing ? 
            InputElementMode::Modify :
            InputElementMode::Select
        );
    }

    void GetCurrentTime(DateTime& outTime)
    {
        outTime.CopyFrom(currentValue);
    }

    static void AlterYearThunk(void* ctx, MenuEvent event) {
        static_cast<PageForDate*>(ctx)->AlterYear(event);
    }

    static void AlterMonthThunk(void* ctx, MenuEvent event) {
        static_cast<PageForDate*>(ctx)->AlterMonth(event);
    }

    static void AlterDayThunk(void* ctx, MenuEvent event) {
        static_cast<PageForDate*>(ctx)->AlterDay(event);
    }

    
    private:
    IDisplay* display;
    int row;
    int col;

    void AlterDay(MenuEvent event)
    {
        switch (event) {
            case MenuEvent::MoveFwd:
                currentValue.incrementDay();
                break;
            case MenuEvent::MoveBack:
                currentValue.DecrementDay();
                break;
            case MenuEvent::PushButton:
                // Save the current currentValue and exit editing mode
                isEditing = false;
                break;
        }
    }

    void AlterMonth(MenuEvent event)
    {
        switch (event) {
            case MenuEvent::MoveFwd:
                currentValue.incrementMonth();
                break;
            case MenuEvent::MoveBack:
                currentValue.DecrementMonth();
                break;
            case MenuEvent::PushButton:
                // Save the current currentValue and exit editing mode
                isEditing = false;
                break;
        }
    }

    void AlterYear(MenuEvent event)
    {
        switch (event) {
            case MenuEvent::MoveFwd:
                currentValue.incrementYear();
                break;
            case MenuEvent::MoveBack:
                currentValue.DecrementYear();
                break;
            case MenuEvent::PushButton:
                // Save the current currentValue and exit editing mode
                isEditing = false;
                break;
        }
    }

    DateTime currentValue;
    InputElement *elements[5] = {nullptr, nullptr, nullptr, nullptr, nullptr};
    int CurrentElementIndex = 0;
    int MaxStopItemIndex = 0;
    bool isEditing = false; // Flag to indicate if we are in editing mode
};
