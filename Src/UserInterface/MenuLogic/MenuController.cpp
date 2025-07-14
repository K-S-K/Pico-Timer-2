#include "string.h"
#include <cstdio>

#include "MenuController.hpp"
#include "../Display/Display.hpp"
#include "../Drivers/RotaryEncoder.hpp"

#include "../MenuPages/IPage.hpp"
#include "../MenuPages/PageForDate.hpp"
#include "../MenuPages/PageForTime.hpp"
#include "../MenuPages/PageForAlrm.hpp"
#include "../MenuPages/PageForRely.hpp"

MenuController::MenuController(Clock* clock, Alarm* alarm, Relay* relay, MenuScreen* menuScreen, IDisplay* display)
    : clock(clock), alarm(alarm), relay(relay), menuScreen(menuScreen), display(display)
    {
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

        // Initialize the current menu item 
        // to the "Exit" item to let user
        // easily exit the menu in case
        // he entered it by mistake
        currentItem = &menuItems[6];
    }

void MenuController::ProcessEvent(MenuEvent event) {
    ProcessMenuEvent(event);
    Render();
}

void MenuController::ProcessMenuEvent(MenuEvent event) {
    switch (menuState) {
        case MenuState::MainScreen:
            // In the main screen, we only handle the push button event to enter the menu
            if (event == MenuEvent::PushButton) {
                // Clear the display before entering the menu
                display->Clear();

                // Switch to the menu mode
                menuState = MenuState::MenuScreen;

                // Set the initial item to the "Cancel" item
                // so if user entered the menu by mistake, he can exit it
                // by pressing the button again
                currentItem = &menuItems[static_cast<int>(MenuItemType::Exit)];
            }
            // Otherwise, we ignore the event in the main screen
            else
            {
                return;
            }
            break;

        case MenuState::MenuScreen:
            if (event == MenuEvent::MoveFwd)
            {
                SelectNextItem();
            }
            else if (event == MenuEvent::MoveBack)
            {
                SelectPrevItem();
            }
            else if (event == MenuEvent::PushButton)
            {
                if (currentItem->IsTypeOf(MenuItemType::Exit))
                {
                    // If the user pressed the Exit button, we return to the main screen
                    menuState = MenuState::MainScreen;

                    // Clear the display to avoid showing the 
                    // rest elements of the menu after exiting it
                    display->Clear();
                }
                else
                {
                    if(currentItem->IsTypeOf(MenuItemType::Date)){
                        IPage *page = currentItem->GetPage();
                        if(page == nullptr)
                        {
                            DateTime value;
                            clock->GetCurrentTime(value);
                            page = new PageForDate(display, 1, 4, value, currentItem->GetHeader());
                            currentItem->SetPage(page);
                        }
                        page->PrepareDisplay();
                        page->Render();
                    }

                    else if(currentItem->IsTypeOf(MenuItemType::Time)){
                        IPage *page = currentItem->GetPage();
                        if(page == nullptr)
                        {
                            DateTime value;
                            clock->GetCurrentTime(value);
                            page = new PageForTime(display, 1, 4, value, currentItem->GetHeader());
                            currentItem->SetPage(page);
                        }
                        page->PrepareDisplay();
                        page->Render();
                    }

                    else if(currentItem->IsTypeOf(MenuItemType::AlarmTime)){
                        IPage *page = currentItem->GetPage();
                        if(page == nullptr)
                        {
                            AlarmConfig alarmConfig;
                            alarm->GetAlarmConfig(alarmConfig);
                            page = new PageForTime(display, 1, 4, alarmConfig.timeBeg, currentItem->GetHeader(), PageForTimeMode::WithoutSeconds);
                            currentItem->SetPage(page);
                        }
                        page->PrepareDisplay();
                        page->Render();
                    }

                    else if(currentItem->IsTypeOf(MenuItemType::AlarmConfig)){
                        IPage *page = currentItem->GetPage();
                        if(page == nullptr)
                        {
                            AlarmConfig alarmConfig;
                            alarm->GetAlarmConfig(alarmConfig);
                            page = new PageForAlrm(display, 1, 2, alarmConfig.duration, alarmConfig.enabled, currentItem->GetHeader());
                            currentItem->SetPage(page);
                        }
                        page->PrepareDisplay();
                        page->Render();
                    }

                    else if(currentItem->IsTypeOf(MenuItemType::Relay)){
                        IPage *page = currentItem->GetPage();
                        if(page == nullptr)
                        {
                            RelayConfig relayConfig;
                            relay->GetRelayConfig(relayConfig);
                            page = new PageForRelay(display, 1, 2, relayConfig.timeBeg, relayConfig.timeEnd, currentItem->GetHeader());
                            currentItem->SetPage(page);
                        }
                        page->PrepareDisplay();
                        page->Render();
                    }
                    
                    // Add other page types here as needed

                    menuState = MenuState::EditScreen;
                }
            }
            break;

        case MenuState::EditScreen:
            {
                IPage *page = currentItem->GetPage();
                if(page != nullptr)
                {
                    if(currentItem->IsTypeOf(MenuItemType::Date))
                    {
                        if(page != nullptr)
                        {
                            EventProcessingResult result = 
                                page->ProcessMenuEvent(event);
                            if(result == EventProcessingResult::Continue)
                            {
                                return; // Continue processing in the page
                            }
                            if(result == EventProcessingResult::Apply)
                            {
                                    DateTime clockValue;
                                    clock->GetCurrentTime(clockValue);
                                    DateTime editorValue;
                                    ((PageForDate*)(page))->GetCurrentTime(editorValue);
                                    clockValue.CopyDateFrom(editorValue);

                                    // Apply the changes to the clock
                                    clock->SetCurrentTime(clockValue);
                            }
                            delete page;
                            page = nullptr;
                            currentItem->SetPage(nullptr);
                            menuState = MenuState::MenuScreen;
                            display->Clear();
                        }
                    }

                    else if(currentItem->IsTypeOf(MenuItemType::Time))
                    {
                        if(page != nullptr)
                        {
                            EventProcessingResult result = 
                                page->ProcessMenuEvent(event);
                            if(result == EventProcessingResult::Continue)
                            {
                                return; // Continue processing in the page
                            }
                            if(result == EventProcessingResult::Apply)
                            {
                                    DateTime clockValue;
                                    clock->GetCurrentTime(clockValue);
                                    DateTime editorValue;
                                    ((PageForTime*)(page))->GetCurrentTime(editorValue);
                                    clockValue.CopyTimeFrom(editorValue);

                                    // Apply the changes to the clock
                                    clock->SetCurrentTime(clockValue);
                            }
                            delete page;
                            page = nullptr;
                            currentItem->SetPage(nullptr);
                            menuState = MenuState::MenuScreen;
                            display->Clear();
                        }
                    }

                    else if(currentItem->IsTypeOf(MenuItemType::AlarmTime))
                    {
                        if(page != nullptr)
                        {
                            EventProcessingResult result = 
                                page->ProcessMenuEvent(event);
                            if(result == EventProcessingResult::Continue)
                            {
                                return; // Continue processing in the page
                            }
                            if(result == EventProcessingResult::Apply)
                            {
                                    AlarmConfig alarmConfig;
                                    alarm->GetAlarmConfig(alarmConfig);
                                    DateTime editorValue;
                                    ((PageForTime*)(page))->GetCurrentTime(editorValue);
                                    alarmConfig.timeBeg.CopyTimeFrom(editorValue);

                                    // Apply the changes to the clock
                                    alarm->SetAlarmConfig(alarmConfig);
                            }
                            delete page;
                            page = nullptr;
                            currentItem->SetPage(nullptr);
                            menuState = MenuState::MenuScreen;
                            display->Clear();
                        }
                    }

                    else if(currentItem->IsTypeOf(MenuItemType::AlarmConfig))
                    {
                        if(page != nullptr)
                        {
                            EventProcessingResult result = 
                                page->ProcessMenuEvent(event);
                            if(result == EventProcessingResult::Continue)
                            {
                                return; // Continue processing in the page
                            }
                            if(result == EventProcessingResult::Apply)
                            {
                                    bool enabled;
                                    int seconds;
                                    AlarmConfig alarmConfig;
                                    alarm->GetAlarmConfig(alarmConfig);
                                    ((PageForAlrm*)(page))->GetCurrentState(seconds, enabled);
                                    alarmConfig.duration = seconds;
                                    alarmConfig.enabled = enabled;
                                    // Apply the changes to the clock
                                    alarm->SetAlarmConfig(alarmConfig);
                            }
                            delete page;
                            page = nullptr;
                            currentItem->SetPage(nullptr);
                            menuState = MenuState::MenuScreen;
                            display->Clear();
                        }
                    }

                    else if(currentItem->IsTypeOf(MenuItemType::Relay))
                    {
                        if(page != nullptr)
                        {
                            EventProcessingResult result = 
                                page->ProcessMenuEvent(event);
                            if(result == EventProcessingResult::Continue)
                            {
                                return; // Continue processing in the page
                            }
                            if(result == EventProcessingResult::Apply)
                            {
                                    RelayConfig relayConfig;
                                    relay->GetRelayConfig(relayConfig);
                                    ((PageForRelay*)(page))->GetRelayTimes(relayConfig.timeBeg, relayConfig.timeEnd);
                                    // Apply the changes to the clock
                                    relay->SetRelayConfig(relayConfig);
                            }
                            delete page;
                            page = nullptr;
                            currentItem->SetPage(nullptr);
                            menuState = MenuState::MenuScreen;
                            display->Clear();
                        }
                    }

                    // Add other page types here as needed
                }

                else if (event == MenuEvent::MoveFwd) {
                } else if (event == MenuEvent::MoveBack) {
                } else if (event == MenuEvent::PushButton) {
                    menuState = MenuState::MainScreen;
                    // Saving logic will go here in the next steps
                }
            }
            break;
    }
}

void MenuController::Render() {

    switch (menuState) {
        case MenuState::MainScreen: {
            break;
        }

        case MenuState::MenuScreen: {
            display->PrintLine(0, 0, "Menu");
            char buffer[21];
            snprintf(buffer, sizeof(buffer), "  -> %-15s", currentItem->GetName());
            display->PrintLine(1, 0, buffer);
            break;
        }

        case MenuState::EditScreen: {
            {
                IPage *page = currentItem->GetPage();
                if(page != nullptr){
                    page->Render();
                }
            }
            break;
        }
    }
}
