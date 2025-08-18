#pragma once

#include "MenuLogic/MenuItem.hpp"


class MenuContent {
public:
    MenuContent(void);
    ~MenuContent();


    void SetCurrentItem(MenuItem* item) {
        currentItem = item;
    }

    MenuItem* SelectPrevItem() {
        SetCurrentItem(&menuItems[currentItem->GetPrevItemIndex(count)]);
        return currentItem;
    }

    MenuItem* SelectNextItem() {
        SetCurrentItem(&menuItems[currentItem->GetNextItemIndex(count)]);
        return currentItem;
    }

    int count = 0;
    MenuItem *menuItems = nullptr;
    MenuItem *currentItem = nullptr;
};
