#pragma once

#include "../Display/IDisplay.hpp"

#include "../Menu/MenuEvent.h"
#include "InputElement.hpp"
#include "EmptyPage.hpp"


class PageForDate : public EmptyPage
{
    public:
    PageForDate(IDisplay* display, int row, int col, DateTime valueIn, const char* headerText)
    : EmptyPage(display, row, col, headerText)
    {
        currentValue.CopyFrom(valueIn);

        elements = new InputElement*[5]; // 3 editable fields + Cancel/Apply
        int i = 0;
        elements[i++] = new InputElement(display, 3, 0, InputElementType::Cancel);
        elements[i++] = new InputElement(display, row + 1, col + 3, InputElementType::Data, &PageForDate::AlterYearThunk, this);
        elements[i++] = new InputElement(display, row + 1, col + 6, InputElementType::Data, &PageForDate::AlterMonthThunk, this);
        elements[i++] = new InputElement(display, row + 1, col + 9, InputElementType::Data, &PageForDate::AlterDayThunk, this);
        elements[i++] = new InputElement(display, 3, 0, InputElementType::Apply);

        MaxStopItemIndex = i - 1;
    }
    
    virtual ~PageForDate() {}

    void Render()
    {
        char buffer[32];
        snprintf(buffer, sizeof(buffer), "%04d.%02d.%02d", currentValue.year, currentValue.month, currentValue.day);
        display->ShowText(row, col, buffer);

        // Render the cursor and options
        RenderElements();
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
    void AlterDay(MenuEvent event)
    {
        switch (event) {
            case MenuEvent::MoveFwd:
                currentValue.IncrementDays();
                break;
            case MenuEvent::MoveBack:
                currentValue.DecrementDays();
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
                currentValue.IncrementMonths();
                break;
            case MenuEvent::MoveBack:
                currentValue.DecrementMonths();
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
                currentValue.IncrementYears();
                break;
            case MenuEvent::MoveBack:
                currentValue.DecrementYears();
                break;
            case MenuEvent::PushButton:
                // Save the current currentValue and exit editing mode
                isEditing = false;
                break;
        }
    }

    DateTime currentValue;
};
