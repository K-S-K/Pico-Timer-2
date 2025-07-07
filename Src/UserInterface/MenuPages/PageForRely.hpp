#pragma once

#include "../Display/IDisplay.hpp"

#include "../MenuLogic/MenuEvent.h"
#include "InputElement.hpp"
#include "EmptyPage.hpp"

class PageForRelay : public EmptyPage
{
    public:
    PageForRelay(IDisplay* display, int row, int col, 
        DateTime tOn, DateTime tOff, const char* headerText)
    : EmptyPage(display, row, col, headerText)
    {
        timeOn.CopyFrom(tOn);
        timeOff.CopyFrom(tOff);

        elements = new InputElement*[6]; // 4 editable fields + Cancel/Apply
        int i = 0;
        elements[i++] = new InputElement(display, 3, 0, InputElementType::Cancel);
        elements[i++] = new InputElement(display, row + 1, col + 1, InputElementType::Data, &PageForRelay::AlterHourThunkOn, this);
        elements[i++] = new InputElement(display, row + 1, col + 4, InputElementType::Data, &PageForRelay::AlterMinuteThunkOn, this);
        elements[i++] = new InputElement(display, row + 1, col + 9, InputElementType::Data, &PageForRelay::AlterHourThunkOff, this);
        elements[i++] = new InputElement(display, row + 1, col + 12, InputElementType::Data, &PageForRelay::AlterMinuteThunkOff, this);
        elements[i++] = new InputElement(display, 3, 0, InputElementType::Apply);

        MaxStopItemIndex = i - 1;
    }

    virtual ~PageForRelay() {}

    void Render()
    {
        char buffer[21];
        snprintf(buffer, sizeof(buffer), "%02d:%02d - %02d:%02d", 
            timeOn.hour, timeOn.minute, timeOff.hour, timeOff.minute);

        display->PrintLine(row, col, buffer);

        // Render the cursor and options
        RenderElements();
    }

    void GetRelayTimes(DateTime& outTimeOn, DateTime& outTimeOff)
    {
        outTimeOn.CopyFrom(timeOn);
        outTimeOff.CopyFrom(timeOff);
    }

    void SetRelayTimes(const DateTime& timeOn, const DateTime& timeOff)
    {
        this->timeOn.CopyFrom(timeOn);
        this->timeOff.CopyFrom(timeOff);
    }

    static void AlterHourThunkOn(void* ctx, MenuEvent event) {
        static_cast<PageForRelay*>(ctx)->AlterHourOn(event);
    }

    static void AlterMinuteThunkOn(void* ctx, MenuEvent event) {
        static_cast<PageForRelay*>(ctx)->AlterMinuteOn(event);
    }

    static void AlterHourThunkOff(void* ctx, MenuEvent event) {
        static_cast<PageForRelay*>(ctx)->AlterHourOff(event);
    }

    static void AlterMinuteThunkOff(void* ctx, MenuEvent event) {
        static_cast<PageForRelay*>(ctx)->AlterMinuteOff(event);
    }

    private:
    void AlterHourOn(MenuEvent event)
    {
        switch (event)
        {
            case MenuEvent::MoveFwd:
                timeOn.IncrementHours(false); // Do not propagate to day
                break;

            case MenuEvent::MoveBack:
                timeOn.DecrementHours(false); // Do not propagate to day
                break;

            case MenuEvent::PushButton:
                // Save the current timeOn and exit editing mode
                isEditing = false;
                break;
        }
    }

    void AlterMinuteOn(MenuEvent event)
    {
        switch (event)
        {
            case MenuEvent::MoveFwd:
                timeOn.IncrementMinutes(false); // Do not propagate to hour
                break;

            case MenuEvent::MoveBack:
                timeOn.DecrementMinutes(false); // Do not propagate to hour
                break;

            case MenuEvent::PushButton:
                // Save the current timeOn and exit editing mode
                isEditing = false;
                break;
        }
    }

    void AlterHourOff(MenuEvent event)
    {
        switch (event)
        {
            case MenuEvent::MoveFwd:
                timeOff.IncrementHours(false); // Do not propagate to day
                break;

            case MenuEvent::MoveBack:
                timeOff.DecrementHours(false); // Do not propagate to day
                break;

            case MenuEvent::PushButton:
                // Save the current timeOff and exit editing mode
                isEditing = false;
                break;
        }
    }

    void AlterMinuteOff(MenuEvent event)
    {
        switch (event)
        {
            case MenuEvent::MoveFwd:
                timeOff.IncrementMinutes(false); // Do not propagate to hour
                break;

            case MenuEvent::MoveBack:
                timeOff.DecrementMinutes(false); // Do not propagate to hour
                break;

            case MenuEvent::PushButton:
                // Save the current timeOff and exit editing mode
                isEditing = false;
                break;
        }
    }


    DateTime timeOn;
    DateTime timeOff;
};
