#include "string.h"
#include <cstdio>

#include "MenuController.hpp"
#include "../MenuPages/IPage.hpp"

MenuController::MenuController(MenuScreen* menuScreen)
    : menuScreen(menuScreen)
    {
        menuContent = menuScreen->GetMenuContent();

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
                // Clear the screen before entering the menu
                menuScreen->Clear();

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

                    // Clear the screen to avoid showing the 
                    // rest elements of the menu after exiting it
                    menuScreen->Clear();
                }
                else
                {
                    if(menuContent->currentItem->IsTypeOf(MenuItemType::Date)){
                        IPage *page = menuContent->currentItem->GetPage();
                        if(page != nullptr)
                        {
                            page->UpdatePage();
                        }
                    }

                    else if(menuContent->currentItem->IsTypeOf(MenuItemType::Time)){
                        IPage *page = menuContent->currentItem->GetPage();
                        if(page != nullptr)
                        {
                            page->UpdatePage();
                        }
                    }

                    else if(menuContent->currentItem->IsTypeOf(MenuItemType::AlarmTime)){
                        IPage *page = menuContent->currentItem->GetPage();
                        if(page != nullptr)
                        {
                            page->UpdatePage();
                        }
                    }

                    else if(menuContent->currentItem->IsTypeOf(MenuItemType::AlarmConfig)){
                        IPage *page = menuContent->currentItem->GetPage();
                        if(page != nullptr)
                        {
                            page->UpdatePage();
                        }
                    }

                    else if(menuContent->currentItem->IsTypeOf(MenuItemType::Relay)){
                        IPage *page = menuContent->currentItem->GetPage();
                        if(page != nullptr)
                        {
                            page->UpdatePage();
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
                                page->UpdateData();
                            }
                            menuState = MenuState::MenuScreen;
                            menuScreen->Clear();
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
                                page->UpdateData();
                            }
                            menuState = MenuState::MenuScreen;
                            menuScreen->Clear();
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
                                page->UpdateData();
                            }
                            menuState = MenuState::MenuScreen;
                            menuScreen->Clear();
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
                                page->UpdateData();
                            }
                            menuState = MenuState::MenuScreen;
                            menuScreen->Clear();
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
                                page->UpdateData();
                            }
                            menuState = MenuState::MenuScreen;
                            menuScreen->Clear();
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
