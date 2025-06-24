#pragma once

#include "../Display/IDisplay.hpp"

#include "../Menu/MenuEvent.h"
#include "InputElement.hpp"
#include "IPage.hpp"


class EmptyPage: public IPage
{
public:
EmptyPage(IDisplay* display, int row, int col)
    : display(display), row(row), col(col) {}

    virtual ~EmptyPage()
    {
        for (int i = 0; i < MaxStopItemIndex + 1; ++i) {
            delete elements[i];
        }
        delete[] elements;
    }

    virtual void Render() = 0;

    
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


protected:
    void RenderElements()
    {
        elements[CurrentElementIndex]->Render(isEditing ? 
            InputElementMode::Modify :
            InputElementMode::Select
        );
    }


protected:
    IDisplay* display;
    int row;
    int col;

    InputElement **elements;
    int MaxStopItemIndex = 0;
    bool isEditing = false; // Flag to indicate if we are in editing mode

private:
    int CurrentElementIndex = 0;
};
