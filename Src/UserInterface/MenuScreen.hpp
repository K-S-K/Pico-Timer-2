#pragma once

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include <stdint.h>

#include "MenuLogic/MenuItem.hpp"
#include "../Display/IDisplay.hpp"

enum class MenuScreenCommandType {
    Clear,
    Render,
    SetItems,
    SetHeader,
    SetCurrentItem,
};

struct MenuScreenCommand {
    MenuScreenCommandType type;
    bool render = false;

    union {
        struct {
            char text[21]; // adjust to LCD width
        } headerText;

        struct {
            MenuItem *items; // Assuming a maximum of 10 menu items
        } items;

        struct {
            uint8_t index; // custom character location
        } item;
    };
};

class MenuScreen
{
public:
    MenuScreen(IDisplay* display)
        : display(display)
    {
        commandQueue = xQueueCreate(4, sizeof(MenuScreenCommand));
        xTaskCreate(TaskLoop, "MenuScreenTask", 2048, this, tskIDLE_PRIORITY + 1, nullptr);
    }

    private:
    QueueHandle_t commandQueue;
    static void TaskLoop(void* param);
    void ProcessCommand(const MenuScreenCommand& cmd);

private:
    IDisplay* display;
};
