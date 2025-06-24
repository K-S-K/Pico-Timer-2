#include "string.h"
#include <cstdio>

#include "MenuController.hpp"
#include "../Display/Display.hpp"
#include "../Drivers/RotaryEncoder.hpp"

#include "../Pages/IPage.hpp"
#include "../Pages/PageForDate.hpp"
#include "../Pages/PageForTime.hpp"

MenuController::MenuController(Clock* clock, IDisplay* display)
    : clock(clock), display(display) 
    {
        count = static_cast<int>(MenuItemType::Count);
        // Initialize menu items
        menuItems = new MenuItem[6]
        {
            MenuItem(0, MenuItemType::Date, "Date"),
            MenuItem(1, MenuItemType::Time, "Time"),
            MenuItem(2, MenuItemType::Alarm, "Alarm"),
            MenuItem(3, MenuItemType::Relay, "Relay"),
            MenuItem(4, MenuItemType::System, "System"),
            MenuItem(5, MenuItemType::Exit, "Exit")
        };

        // Initialize the current menu item 
        // to the "Exit" item to let user
        // easily exit the menu in case
        // he entered it by mistake
        currentItem = &menuItems[5];
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
                            page = new PageForDate(display, 1, 6, value);
                            currentItem->SetPage(page);
                        }
                        page->Render();
                    }

                    else if(currentItem->IsTypeOf(MenuItemType::Time)){
                        IPage *page = currentItem->GetPage();
                        if(page == nullptr)
                        {
                            DateTime value;
                            clock->GetCurrentTime(value);
                            page = new PageForTime(display, 1, 6, value);
                            currentItem->SetPage(page);
                        }
                        page->Render();
                    }

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
    // If we are in the main screen, we do not render anything
    // This is to avoid flickering and unnecessary updates
    if(menuState == MenuState::MainScreen)
    {
        return;
    }

    char buf[21];
    
    // Fill with spaces
    snprintf(buf, sizeof(buf), "                    ");

    // Clear screen first
    display->ShowText(0, 0, buf);
    display->ShowText(1, 0, buf);

    switch (menuState) {
        case MenuState::MainScreen: {
            // Display the main screen status for the debugging purpose only
            // display->ShowText(0, 0, "Main Screen");
            break;
        }

        case MenuState::MenuScreen: {
            display->ShowText(0, 0, "Menu:");
            display->ShowText(1, 2, currentItem->GetName());
            break;
        }

        case MenuState::EditScreen: {
            display->ShowText(0, 0, "Edit:");
            display->ShowText(1, 2, currentItem->GetName());

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
