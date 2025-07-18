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

    ~MenuScreen() {
        vQueueDelete(commandQueue);
    }

    void Clear() {
        MenuScreenCommand cmd = { MenuScreenCommandType::Clear };
        SendCommand(cmd);
    }

    void Render() {
        MenuScreenCommand cmd = { MenuScreenCommandType::Render, true };
        SendCommand(cmd);
    }

    void SetItems(MenuItem* items) {
        MenuScreenCommand cmd = { MenuScreenCommandType::SetItems };
        cmd.items.items = items;
        SendCommand(cmd);
    }

    void SetHeader(const char* text) {
        MenuScreenCommand cmd = { MenuScreenCommandType::SetHeader };
        snprintf(cmd.headerText.text, sizeof(cmd.headerText.text), "%s", text);
        SendCommand(cmd);
    }

    void SetCurrentItem(uint8_t index) {
        MenuScreenCommand cmd = { MenuScreenCommandType::SetCurrentItem };
        cmd.item.index = index;
        SendCommand(cmd);
    }

    private:
    QueueHandle_t commandQueue;
    static void TaskLoop(void* param);
    void ProcessCommand(const MenuScreenCommand& cmd);

    void SendCommand(const MenuScreenCommand& cmd) {
        xQueueSend(commandQueue, &cmd, portMAX_DELAY);
    }

private:
    char header[21] = {0};
    MenuItem* items = nullptr;
    uint8_t currentItemIndex = 0;
    MenuItem* currentItem = nullptr;

private:
    IDisplay* display;
};
