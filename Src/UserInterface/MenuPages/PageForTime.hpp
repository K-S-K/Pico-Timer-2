#pragma once

#include "../Display/IDisplay.hpp"

#include "../MenuLogic/MenuEvent.h"
#include "InputElement.hpp"
#include "EmptyPage.hpp"

enum class PageForTimeMode {
    WithSeconds,
    WithoutSeconds,
};

class PageForTime : public EmptyPage
{
    public:
    PageForTime(IDisplay* display, int row, int col, DateTime valueIn, const char* headerText, PageForTimeMode mode = PageForTimeMode::WithSeconds)
    : EmptyPage(display, row, col, headerText), mode(mode)
    {
        currentValue.CopyFrom(valueIn);

        int elementsCount = 
            mode == PageForTimeMode::WithSeconds ? 5 : 
            mode == PageForTimeMode::WithoutSeconds ? 4 :
            5; // Default to 5 if mode is not recognized
        elements = new InputElement*[elementsCount]; // 3 editable fields + Cancel/Apply
        int i = 0;
        elements[i++] = new InputElement(display, 3, 0, InputElementType::Cancel);
        elements[i++] = new InputElement(display, row + 1, col + 1, InputElementType::Data, &PageForTime::AlterHourThunk, this);
        elements[i++] = new InputElement(display, row + 1, col + 4, InputElementType::Data, &PageForTime::AlterMinuteThunk, this);
        if (mode == PageForTimeMode::WithSeconds)
        {
            elements[i++] = new InputElement(display, row + 1, col + 7, InputElementType::Data, &PageForTime::AlterSecondThunk, this);
        }
        elements[i++] = new InputElement(display, 3, 0, InputElementType::Apply);

        MaxStopItemIndex = i - 1;
    }

    virtual ~PageForTime() {}

    void Render()
    {
        char buffer[32];
        // Display the current time in the respective format
        // depending on the mode (with or without seconds)
        switch (mode)
        {
            case PageForTimeMode::WithSeconds:
                snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d", currentValue.hour, currentValue.minute, currentValue.second);
                break;

            case PageForTimeMode::WithoutSeconds:
                snprintf(buffer, sizeof(buffer), "%02d:%02d", currentValue.hour, currentValue.minute);
                break;

            default:
                // Handle not implemented mode gracefully
                snprintf(buffer, sizeof(buffer), "! Unknown Mode !");
                break;
        }
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

    PageForTimeMode mode = PageForTimeMode::WithSeconds; // Default mode with seconds

    DateTime currentValue;
};
