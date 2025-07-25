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
            display->PrintLine(0, 0, header);
            if (currentItem != nullptr)
            {
                char buffer[21];
                snprintf(buffer, sizeof(buffer), "  -> %-15s", currentItem->GetName());
                display->PrintLine(1, 0, buffer);
            }
            break;

        case MenuScreenCommandType::SetItems:
            // Set menu items
            items = cmd.items.items;
            currentItem = &items[currentItemIndex];
            break;

        case MenuScreenCommandType::SetHeader:
            // Set header text
            snprintf(header, sizeof(header), "%s", cmd.headerText.text);
            break;

        case MenuScreenCommandType::SetCurrentItem:
            // Set current item index
            currentItemIndex = cmd.item.index;
            currentItem = &items[currentItemIndex];
            break;

        default:
            // Handle unknown command type
            break;
    }
}
