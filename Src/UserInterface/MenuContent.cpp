

#include "MenuContent.hpp"

MenuContent::MenuContent(void) {

    count = static_cast<int>(MenuItemType::Count);

        // Initialize menu items
    menuItems = new MenuItem[7]
    {
        MenuItem(0, MenuItemType::Date, "Clock Date", "Set Clock Date"),
        MenuItem(1, MenuItemType::Time, "Clock Time", "Set Clock Time"),
        MenuItem(2, MenuItemType::AlarmTime, "Alarm Time", "Set Alarm Time"),
        MenuItem(3, MenuItemType::AlarmConfig, "Alarm Config", "Configure Alarm"),
        MenuItem(4, MenuItemType::Relay, "Relay", "Set Relay Time"),
        MenuItem(5, MenuItemType::System, "System", "Configure System"),
        MenuItem(6, MenuItemType::Exit, "Exit", "Exit Menu")
    };
}

MenuContent::~MenuContent() {
    delete[] menuItems;
}
