#pragma once

#include "MenuLogic/MenuItem.hpp"


class MenuContent {
public:
    MenuContent(void){}
    ~MenuContent(){
        delete[] menuItems;
    }

    void Init(MenuItem *menuItems)
    {
        this->menuItems = menuItems;
        count = static_cast<int>(MenuItemType::Count);

        // Initialize the current menu item 
        // to the "Exit" item to let user
        // easily exit the menu in case
        // he entered it by mistake
        SetCurrentItem(&menuItems[6]);
    }


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
