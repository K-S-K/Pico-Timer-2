#pragma once

#include "../Display/IDisplay.hpp"

#include "../Menu/MenuEvent.h"
#include "InputElement.hpp"
#include "EmptyPage.hpp"


class PageForTime : public EmptyPage
{
    public:
    PageForTime(IDisplay* display, int row, int col, DateTime valueIn)
    : EmptyPage(display, row, col)
    {
        currentValue.CopyFrom(valueIn);

        elements = new InputElement*[5]; // 3 editable fields + Cancel/Apply
        int i = 0;
        elements[i++] = new InputElement(display, 3, 0, InputElementType::Cancel);
        elements[i++] = new InputElement(display, row + 1, col + 3, InputElementType::Data, &PageForTime::AlterHourThunk, this);
        elements[i++] = new InputElement(display, row + 1, col + 6, InputElementType::Data, &PageForTime::AlterMinuteThunk, this);
        elements[i++] = new InputElement(display, row + 1, col + 9, InputElementType::Data, &PageForTime::AlterSecondThunk, this);
        elements[i++] = new InputElement(display, 3, 0, InputElementType::Apply);

        MaxStopItemIndex = i - 1;
    }

    virtual ~PageForTime() {}

    void Render()
    {
        char buffer[32];
        snprintf(buffer, sizeof(buffer), ": %02d:%02d:%02d", currentValue.hour, currentValue.minute, currentValue.second);
        display->ShowText(row, col, buffer);

        // Render the cursor and options
        RenderElements();
    }

    void GetCurrentTime(DateTime& outTime)
    {
        outTime.CopyFrom(currentValue);
    }

    static void AlterHourThunk(void* ctx, MenuEvent event) {
        static_cast<PageForTime*>(ctx)->AlterHour(event);
    }

    static void AlterMinuteThunk(void* ctx, MenuEvent event) {
        static_cast<PageForTime*>(ctx)->AlterMinute(event);
    }

    static void AlterSecondThunk(void* ctx, MenuEvent event) {
        static_cast<PageForTime*>(ctx)->AlterSecond(event);
    }

    private:
    void AlterHour(MenuEvent event)
    {
        switch (event)
        {
            case MenuEvent::MoveFwd:
                currentValue.IncrementHours(false); // Do not propagate to day
                break;

            case MenuEvent::MoveBack:
                currentValue.DecrementHours(false); // Do not propagate to day
                break;

            case MenuEvent::PushButton:
                // Save the current currentValue and exit editing mode
                isEditing = false;
                break;
        }
    }

    void AlterMinute(MenuEvent event)
    {
        switch (event)
        {
            case MenuEvent::MoveFwd:
                currentValue.IncrementMinutes(false); // Do not propagate to hour
                break;

            case MenuEvent::MoveBack:
                currentValue.DecrementMinutes(false); // Do not propagate to hour
                break;

            case MenuEvent::PushButton:
                // Save the current currentValue and exit editing mode
                isEditing = false;
                break;
        }
    }

    void AlterSecond(MenuEvent event)
    {
        switch (event)
        {
            case MenuEvent::MoveFwd:
                currentValue.IncrementSeconds(false); // Do not propagate to minute/hour
                break;

            case MenuEvent::MoveBack:
                currentValue.DecrementSeconds(false); // Do not propagate to minute/hour
                break;

            case MenuEvent::PushButton:
                // Save the current currentValue and exit editing mode
                isEditing = false;
                break;
        }
    }

    DateTime currentValue;
};
