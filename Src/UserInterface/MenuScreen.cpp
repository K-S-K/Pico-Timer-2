#include <cstdio>

#include "MenuScreen.hpp"

void MenuScreen::TaskLoop(void* param)
{
    auto* self = static_cast<MenuScreen*>(param);
    MenuScreenCommand cmd = {};

    while (true)
    {
        if (xQueueReceive(self->commandQueue, &cmd, portMAX_DELAY))
        {
            self->ProcessCommand(cmd);
        }
    }
}

void MenuScreen::ProcessCommand(const MenuScreenCommand& cmd)
{
    switch (cmd.type)
    {
        case MenuScreenCommandType::Clear:
            display->Clear();
            break;

        case MenuScreenCommandType::Render:
            // Render the menu screen
            break;

        case MenuScreenCommandType::SetItems:
            // Set menu items
            break;

        case MenuScreenCommandType::SetHeader:
            // Set header text
            break;

        case MenuScreenCommandType::SetCurrentItem:
            // Set current item index
            break;

        default:
            // Handle unknown command type
            break;
    }
}
