#pragma once

#include "FreeRTOS.h"
#include "queue.h"

#include "../Display/IDisplay.hpp"
#include "../Drivers/RotaryEncoder.hpp"

enum class MenuState {
    MainScreen,
    MenuScreen,
    EditScreen
};

enum class MenuItem {
    Time,
    Alarm,
    Relay,
    System,
    Exit,
    Count
};

enum class MenuEvent {
    MoveFwd,
    MoveBack,
    PushButton,
};

struct MenuCommand {
    MenuEvent event;
};

class MenuController {
public:
    MenuController(IDisplay* display);
    void ProcessEvent(MenuEvent event);

private:
    void DebugEventInput(MenuEvent event, int row, int col);
    void ProcessMenu(MenuEvent event);
    void Render();

    MenuState state = MenuState::MainScreen;
    MenuItem currentItem = MenuItem::Time;
    int currentEditValue = 0;

    IDisplay* display;

    bool flag = false;
    int counter = 0;
};
