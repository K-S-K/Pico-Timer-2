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

    void SelectNextItem() {
        currentItem = GetNextItem(currentItem);
    }

    void SelectPrevItem() {
        currentItem = GetPrevItem(currentItem);
    }

    MenuItem* GetPrevItem(MenuItem* item) {
        return &menuItems[item->GetPrevItemIndex(count)];
    }

    MenuItem* GetNextItem(MenuItem* item) {
        return &menuItems[item->GetNextItemIndex(count)];
    }

    MenuState menuState = MenuState::MainScreen;
    MenuItem *currentItem = nullptr;
    int currentEditValue = 0;

    Clock* clock = nullptr;
    IDisplay* display = nullptr;
    IPage *pageForDate = nullptr;

    MenuItem *menuItems;
    int count = 0;
};
