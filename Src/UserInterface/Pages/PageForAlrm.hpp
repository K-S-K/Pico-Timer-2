#pragma once

#include "../Display/IDisplay.hpp"

#include "../Menu/MenuEvent.h"
#include "InputElement.hpp"
#include "EmptyPage.hpp"


class PageForAlrm : public EmptyPage
{
    public:
    PageForAlrm(IDisplay* display, int row, int col, int seconds, bool enabled, const char* headerText)
    : EmptyPage(display, row, col, headerText),
      seconds(seconds), enabled(enabled)
    {
        elements = new InputElement*[3]; // 2 editable fields + Cancel/Apply
        int i = 0;
        elements[i++] = new InputElement(display, 3, 0, InputElementType::Cancel);
        elements[i++] = new InputElement(display, row + 1, col + 11, InputElementType::Data, &PageForAlrm::AlterSecondsThunk, this);
        elements[i++] = new InputElement(display, row + 1, col + 14, InputElementType::Data, &PageForAlrm::SetEnabledThunk, this);
        elements[i++] = new InputElement(display, 3, 0, InputElementType::Apply);

        MaxStopItemIndex = i - 1;
    }

    virtual ~PageForAlrm() {}

    void Render()
    {
        char buffer[32];
        snprintf(buffer, sizeof(buffer), "Duration: %02ds %-3s", seconds, enabled ? "On" : "Off");
        display->ShowText(row, col, buffer);
        
        // Render the cursor and options
        RenderElements();
    }

    void SetCurrentState(int seconds, bool enabled)
    {
        this->seconds = seconds;
        this->enabled = enabled;
    }

    void GetCurrentState(int& secondsOut, bool& enabledOut)
    {
        secondsOut = seconds;
        enabledOut = enabled;
    }

    static void AlterSecondsThunk(void* ctx, MenuEvent event) {
        static_cast<PageForAlrm*>(ctx)->AlterSeconds(event);
    }

    static void SetEnabledThunk(void* ctx, MenuEvent event) {
        static_cast<PageForAlrm*>(ctx)->SetEnabled(event);
    }

    // Alter the seconds value based on the MenuEvent
    // This function is called when the user interacts with the seconds input element
    // It increments or decrements the seconds value, or exits editing mode when the button is pushed
    // Note: The seconds value must be non-negative
    //       and should not exceed a reasonable limit 59 seconds
    void AlterSeconds(MenuEvent event)
    {
        switch (event)
        {
            case MenuEvent::MoveFwd:
                if (seconds < 59) { // Limit to 59 seconds
                    seconds++;
                }
                break;

            case MenuEvent::MoveBack:
                if (seconds > 0) {
                    seconds--;
                }
                break;

            case MenuEvent::PushButton:
                // Save the current state and exit editing mode
                isEditing = false;
                break;
        }
    }

    // Set the enabled state based on the MenuEvent
    // This function is called when the user interacts with the enabled input element
    // It toggles the enabled state or exits editing mode when the button is pushed
    void SetEnabled(MenuEvent event)
    {
        switch (event)
        {
            case MenuEvent::MoveFwd:
                enabled = true;
                break;

            case MenuEvent::MoveBack:
                enabled = false;
                break;

            case MenuEvent::PushButton:
                // Save the current state and exit editing mode
                isEditing = false;
                break;
        }
    }

    private:
    int seconds;
    bool enabled;
};
