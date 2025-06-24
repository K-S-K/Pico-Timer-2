#pragma once

#include "FreeRTOS.h"
#include "queue.h"

#include "../Clock/Clock.hpp"
#include "../Display/IDisplay.hpp"
#include "../Drivers/RotaryEncoder.hpp"

#include "../Pages/PageForDate.hpp"

#include "MenuEvent.h"
#include "MenuItem.hpp"

enum class MenuState {
    MainScreen,
    MenuScreen,
    EditScreen
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
    MenuItem *currentItem = nullptr;
    int currentEditValue = 0;

    Clock* clock = nullptr;
    IDisplay* display = nullptr;
    IPage *pageForDate = nullptr;

    MenuItem menuItems[static_cast<int>(MenuItemType::Count)] = {
        MenuItem(MenuItemType::Date, "Date"),
        MenuItem(MenuItemType::Time, "Time"),
        MenuItem(MenuItemType::Alarm, "Alarm"),
        MenuItem(MenuItemType::Relay, "Relay"),
        MenuItem(MenuItemType::System, "System"),
        MenuItem(MenuItemType::Exit, "Exit")
    };

    bool flag = false;
    int counter = 0;
};
