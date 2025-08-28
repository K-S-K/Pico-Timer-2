#pragma once

#include "FreeRTOS.h"
#include "queue.h"

#include "../Display/IDisplay.hpp"
#include "../Drivers/RotaryEncoder.hpp"
#include "../UserInterface/MenuScreen.hpp"

#include "MenuEvent.h"
#include "MenuItem.hpp"

enum class MenuState {
    MainScreen,
    MenuScreen,
    EditScreen
};

class MenuController {
public:
    MenuController(MenuScreen* menuScreen, IDisplay* display, MenuContent* menuContent);
    void ProcessEvent(MenuEvent event);
    MenuState GetMenuState() const { return menuState; }

private:
    void DebugEventInput(MenuEvent event, int row, int col);
    void ProcessMenuEvent(MenuEvent event);
    void Render();

    void SetCurrentItem(MenuItem* item) {
        menuContent->SetCurrentItem(item);
    }

    void SelectNextItem() {
        menuContent->SelectNextItem();
    }

    void SelectPrevItem() {
        menuContent->SelectPrevItem();
    }

    MenuState menuState = MenuState::MainScreen;

    IDisplay* display = nullptr;
    MenuScreen* menuScreen = nullptr;
    MenuContent* menuContent = nullptr;
};
