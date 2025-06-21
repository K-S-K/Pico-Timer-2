#include "string.h"
#include <cstdio>

#include "MenuController.hpp"
#include "../Display/Display.hpp"
#include "../Drivers/RotaryEncoder.hpp"


// Labels used for display
static constexpr const char* MenuItemLabels[] = {
    "Time",
    "Alarm",
    "Relay",
    "System",
    "Exit"
};

MenuController::MenuController(IDisplay* display)
    : display(display) {}

void MenuController::ProcessEvent(MenuEvent event) {
    DebugEventInput(event, 3, 0);
    // ProcessMenu(event);
    // Render();
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
            break; // Handled in the state machine
    }

    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%3s: %2d", flag ? "IN" : "OUT", counter);
    display->ShowText(row, col, buffer);
}

void MenuController::ProcessMenu(MenuEvent event) {
    switch (state) {
        case MenuState::MainScreen:
            break;

        case MenuState::MenuScreen:
            break;

        case MenuState::EditScreen:
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

    switch (state) {
        case MenuState::MainScreen: {
            break;
        }

        case MenuState::MenuScreen: {
            break;
        }

        case MenuState::EditScreen: {
            break;
        }
    }
}
