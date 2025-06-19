/*
  * Display.cpp - Implementation of the Display class for HD44780 LCD
  * This class handles display commands using FreeRTOS queues.
  * It allows for clearing the display, setting backlight, and showing text.
  * The display commands are processed in a dedicated task loop.
  * The class uses a queue to manage commands asynchronously.
  * The display is assumed to be controlled by an HD44780 driver class.
  * The display commands are processed in a FreeRTOS task, allowing for non-blocking
  * operation and responsiveness in embedded applications.
  * The display can be used in applications that require text output on an HD44780 LCD
  * and can be integrated with other FreeRTOS tasks.
*/

#include <cstring>
#include "Display.hpp"

Display::Display(HD44780* lcd) {
    physicalDisplay = lcd;
    commandQueue = xQueueCreate(8, sizeof(DisplayCommand));
}

void Display::Start() {
    xTaskCreate(TaskLoop, "DisplayTask", 1024, this, 1, nullptr);
}

void Display::Clear() {
    DisplayCommand cmd = {};
    cmd.type = DisplayCommandType::Clear;
    xQueueSend(commandQueue, &cmd, portMAX_DELAY);
}

void Display::SetBacklight(bool on) {
    DisplayCommand cmd = {};
    cmd.type = DisplayCommandType::SetBacklight;
    cmd.backlight.on = on;
    xQueueSend(commandQueue, &cmd, portMAX_DELAY);
}

void Display::ShowText(int row, int col, const char* text) {
    DisplayCommand cmd = {};
    cmd.type = DisplayCommandType::ShowText;
    cmd.showText.row = row;
    cmd.showText.col = col;
    strncpy(cmd.showText.text, text, sizeof(cmd.showText.text) - 1);
    cmd.showText.text[sizeof(cmd.showText.text) - 1] = '\0'; // null-terminate
    xQueueSend(commandQueue, &cmd, portMAX_DELAY);
}

void Display::TaskLoop(void* param) {
    auto* self = static_cast<Display*>(param);
    DisplayCommand cmd;

    while (true) {
        if (xQueueReceive(self->commandQueue, &cmd, portMAX_DELAY)) {
            self->ProcessCommand(cmd);
        }
    }
}

void Display::ProcessCommand(const DisplayCommand& cmd) {
    switch (cmd.type) {
        case DisplayCommandType::Clear:
            physicalDisplay->Clear();
            break;

        case DisplayCommandType::SetBacklight:
            physicalDisplay->SetBacklight(cmd.backlight.on);
            break;

        case DisplayCommandType::ShowText:
            physicalDisplay->SetCursor(cmd.showText.row, cmd.showText.col);
            physicalDisplay->PrintString(cmd.showText.text);
            break;
    }
}
