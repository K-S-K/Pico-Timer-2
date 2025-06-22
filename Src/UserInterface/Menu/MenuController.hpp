#pragma once

#include "FreeRTOS.h"
#include "queue.h"

#include "../Clock/Clock.hpp"
#include "../Display/IDisplay.hpp"
#include "../Drivers/RotaryEncoder.hpp"

#include "MenuEvent.h"
#include "../Pages/PageForDate.hpp"

enum class MenuState {
    MainScreen,
    MenuScreen,
    EditScreen
};

enum class MenuItem {
    Date,
    Time,
    Alarm,
    Relay,
    System,
    Exit,
    Count
};


class MenuController {
public:
    MenuController(Clock* clock, IDisplay* display);
    void ProcessEvent(MenuEvent event);
    MenuState GetMenuState() const { return menuState; }

private:
    void DebugEventInput(MenuEvent event, int row, int col);
    void ProcessMenuEvent(MenuEvent event);
    void Render();

    MenuState menuState = MenuState::MainScreen;
    MenuItem currentItem = MenuItem::Exit;
    int currentEditValue = 0;

    Clock* clock = nullptr;
    IDisplay* display = nullptr;
    PageForDate *pageForDate = nullptr;

    bool flag = false;
    int counter = 0;
};
