#pragma once

#include "MenuLogic/MenuItem.hpp"


class MenuContent {
public:
    MenuContent(void);
    ~MenuContent();


    void SetCurrentItem(MenuItem* item) {
        currentItem = item;
    }


    MenuItem* GetPrevItem(MenuItem* item) {
        return &menuItems[item->GetPrevItemIndex(count)];
    }

    MenuItem* GetNextItem(MenuItem* item) {
        return &menuItems[item->GetNextItemIndex(count)];
    }

    int count = 0;
    MenuItem *menuItems = nullptr;
    MenuItem *currentItem = nullptr;
};
