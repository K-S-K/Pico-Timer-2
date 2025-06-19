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

void Display::ShowText(int col, int row, const char* text) {
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
            physicalDisplay->clear();
            break;

        case DisplayCommandType::SetBacklight:
            physicalDisplay->setBacklight(cmd.backlight.on);
            break;

        case DisplayCommandType::ShowText:
            physicalDisplay->setCursor(cmd.showText.col, cmd.showText.row);
            physicalDisplay->print(cmd.showText.text);
            break;
    }
}
