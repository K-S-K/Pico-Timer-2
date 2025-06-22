#include "string.h"
#include <cstdio>

#include "MenuController.hpp"
#include "../Display/Display.hpp"
#include "../Drivers/RotaryEncoder.hpp"


// Labels used for display
static constexpr const char* MenuItemLabels[] = {
    "Date",
    "Time",
    "Alarm",
    "Relay",
    "System",
    "Exit"
};

MenuController::MenuController(Clock* clock, IDisplay* display)
    : clock(clock), display(display) {}

void MenuController::ProcessEvent(MenuEvent event) {
    // DebugEventInput(event, 3, 0);
    ProcessMenuEvent(event);
    Render();
}

void MenuController::DebugEventInput(MenuEvent event, int row, int col)
{
    switch (event) {
        case MenuEvent::MoveFwd:
            counter++;
            break;
        case MenuEvent::MoveBack:
            counter--;
            break;
        case MenuEvent::PushButton:
            flag = !flag;
            break; // Handled in the menuState machine
    }

    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%3s: %2d", flag ? "IN" : "OUT", counter);
    display->ShowText(row, col, buffer);
}

void MenuController::ProcessMenuEvent(MenuEvent event) {
    switch (menuState) {
        case MenuState::MainScreen:
            if (event == MenuEvent::PushButton) {
                menuState = MenuState::MenuScreen;
                currentItem = MenuItem::Exit;
            }
            break;

        case MenuState::MenuScreen:
            if (event == MenuEvent::MoveFwd) {
                currentItem = static_cast<MenuItem>((static_cast<int>(currentItem) + 1) % static_cast<int>(MenuItem::Count));
            } else if (event == MenuEvent::MoveBack) {
                currentItem = static_cast<MenuItem>((static_cast<int>(currentItem) + static_cast<int>(MenuItem::Count) - 1) % static_cast<int>(MenuItem::Count));
            } else if (event == MenuEvent::PushButton) {
                if (currentItem == MenuItem::Exit) {
                    menuState = MenuState::MainScreen;
                } else {
                    menuState = MenuState::EditScreen;
                    if(currentItem == MenuItem::Date){
                        DateTime value;
                        clock->GetCurrentTime(value);
                        pageForDate = new PageForDate(display, 1, 6, value);
                        pageForDate->Render();
                    }
                    currentEditValue = 0;
                }
            }
            break;

        case MenuState::EditScreen:
            if(pageForDate != nullptr){
                EventProcessingResult result = 
                    pageForDate->ProcessMenuEvent(event);
                if(result == EventProcessingResult::Continue){
                    return; // Continue processing in the page
                }
                if(result == EventProcessingResult::Cancel){
                    // TODO: Save the edited data result
                }
                delete pageForDate;
                pageForDate = nullptr;
                menuState = MenuState::MenuScreen;
            }
            else if (event == MenuEvent::MoveFwd) {
                currentEditValue++;
            } else if (event == MenuEvent::MoveBack) {
                currentEditValue--;
            } else if (event == MenuEvent::PushButton) {
                menuState = MenuState::MainScreen;
                // Saving logic will go here in the next steps
            }
            break;
    }
}

void MenuController::Render() {
    char buf[21];
    
    // Fill with spaces
    snprintf(buf, sizeof(buf), "                    ");

    // Clear screen first
    display->ShowText(0, 0, buf);
    display->ShowText(1, 0, buf);

    switch (menuState) {
        case MenuState::MainScreen: {
            display->ShowText(0, 0, "Main Screen");
            break;
        }

        case MenuState::MenuScreen: {
            display->ShowText(0, 0, "Menu:");
            display->ShowText(1, 2, MenuItemLabels[static_cast<int>(currentItem)]);
            break;
        }

        case MenuState::EditScreen: {
            display->ShowText(0, 0, "Edit:");
            display->ShowText(1, 2, MenuItemLabels[static_cast<int>(currentItem)]);

            if(pageForDate != nullptr){
                pageForDate->Render();
            }

            else{
                snprintf(buf, sizeof(buf), "Value: %d", currentEditValue);
                display->ShowText(2, 2, buf);
            }

            break;
        }
    }
}
