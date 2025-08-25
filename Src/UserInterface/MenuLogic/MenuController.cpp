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

MenuController::MenuController(Clock* clock, Alarm* alarm, Relay* relay, MenuScreen* menuScreen, IDisplay* display, MenuContent* menuContent)
    : clock(clock), alarm(alarm), relay(relay), menuScreen(menuScreen), display(display), menuContent(menuContent)
    {

        // Initialize the menu screen
        menuScreen->SetHeader("Menu");
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
                SetCurrentItem(&menuContent->menuItems[static_cast<int>(MenuItemType::Exit)]);
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
                if (menuContent->currentItem->IsTypeOf(MenuItemType::Exit))
                {
                    // If the user pressed the Exit button, we return to the main screen
                    menuState = MenuState::MainScreen;

                    // Clear the display to avoid showing the 
                    // rest elements of the menu after exiting it
                    display->Clear();
                }
                else
                {
                    if(menuContent->currentItem->IsTypeOf(MenuItemType::Date)){
                        IPage *page = menuContent->currentItem->GetPage();
                        if(page != nullptr)
                        {
                            DateTime value;
                            clock->GetCurrentTime(value);
                            (static_cast<PageForDate*>(page))->SetCurrentDate(value);
                            page->PrepareDisplay();
                            page->Render();
                        }
                    }

                    else if(menuContent->currentItem->IsTypeOf(MenuItemType::Time)){
                        IPage *page = menuContent->currentItem->GetPage();
                        if(page != nullptr)
                        {
                            DateTime value;
                            clock->GetCurrentTime(value);
                            (static_cast<PageForTime*>(page))->SetCurrentTime(value);
                            page->PrepareDisplay();
                            page->Render();
                        }
                        DateTime value;
                        clock->GetCurrentTime(value);
                        (static_cast<PageForTime*>(page))->SetCurrentTime(value);
                        page->PrepareDisplay();
                        page->Render();
                    }

                    else if(menuContent->currentItem->IsTypeOf(MenuItemType::AlarmTime)){
                        IPage *page = menuContent->currentItem->GetPage();
                        if(page != nullptr)
                        {
                            AlarmConfig alarmConfig;
                            alarm->GetAlarmConfig(alarmConfig);
                            (static_cast<PageForTime*>(page))->SetCurrentTime(alarmConfig.timeBeg);
                            page->PrepareDisplay();
                            page->Render();
                        }
                    }

                    else if(menuContent->currentItem->IsTypeOf(MenuItemType::AlarmConfig)){
                        IPage *page = menuContent->currentItem->GetPage();
                        if(page != nullptr)
                        {
                            AlarmConfig alarmConfig;
                            alarm->GetAlarmConfig(alarmConfig);
                            (static_cast<PageForAlrm*>(page))->GetCurrentState(alarmConfig.duration, alarmConfig.enabled);
                            page->PrepareDisplay();
                            page->Render();
                        }
                    }

                    else if(menuContent->currentItem->IsTypeOf(MenuItemType::Relay)){
                        IPage *page = menuContent->currentItem->GetPage();
                        if(page != nullptr)
                        {
                            RelayConfig relayConfig;
                            relay->GetRelayConfig(relayConfig);
                            (static_cast<PageForRelay*>(page))->SetRelayTimes(relayConfig.timeBeg, relayConfig.timeEnd);
                            page->PrepareDisplay();
                            page->Render();
                       }
                    }
                    
                    // Add other page types here as needed

                    menuState = MenuState::EditScreen;
                }
            }
            break;

        case MenuState::EditScreen:
            {
                IPage *page = menuContent->currentItem->GetPage();
                if(page != nullptr)
                {
                    if(menuContent->currentItem->IsTypeOf(MenuItemType::Date))
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
                                    ((PageForDate*)(page))->GetCurrentDate(editorValue);
                                    clockValue.CopyDateFrom(editorValue);

                                    // Apply the changes to the clock
                                    clock->SetCurrentTime(clockValue);
                            }
                            menuState = MenuState::MenuScreen;
                            display->Clear();
                        }
                    }

                    else if(menuContent->currentItem->IsTypeOf(MenuItemType::Time))
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
                            menuState = MenuState::MenuScreen;
                            display->Clear();
                        }
                    }

                    else if(menuContent->currentItem->IsTypeOf(MenuItemType::AlarmTime))
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
                            menuState = MenuState::MenuScreen;
                            display->Clear();
                        }
                    }

                    else if(menuContent->currentItem->IsTypeOf(MenuItemType::AlarmConfig))
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
                            menuState = MenuState::MenuScreen;
                            display->Clear();
                        }
                    }

                    else if(menuContent->currentItem->IsTypeOf(MenuItemType::Relay))
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
            menuScreen->Render();
            break;
        }

        case MenuState::EditScreen: {
            {
                IPage *page = menuContent->currentItem->GetPage();
                if(page != nullptr){
                    page->Render();
                }
            }
            break;
        }
    }
}
