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

Display::Display(HD44780* lcd)
{
    physicalDisplay = lcd;
    commandQueue = xQueueCreate(8, sizeof(DisplayCommand));

    // Add custom symbos for ringing bell (in form of solid bell) 
    // and not ringing bell  (in form of frame of bell)
    uint8_t bellCharOn[8] =
    {
        0b00011,
        0b00111,
        0b11111,
        0b11111,
        0b11111,
        0b00111,
        0b00011,
        0b00000
    };
    uint8_t bellCharOff[8] =
    {
        0b00011,
        0b00101,
        0b11001,
        0b10001,
        0b11001,
        0b00101,
        0b00011,
        0b00000
    };
    uint8_t degreeChar[8] =
    {
        0b00110,
        0b01001,
        0b01001,
        0b00110,
        0b00000,
        0b00000,
        0b00000,
        0b00000
    };
    uint8_t clockChar[8] =
    {
        0b00000,
        0b01110,
        0b10101,
        0b10111,
        0b10001,
        0b01110,
        0b00000,
        0b00000
    };
    uint8_t thermoChar[8] =
    {
        0b00100,
        0b01010,
        0b01010,
        0b01110,
        0b01110,
        0b11111,
        0b11111,
        0b01110
    };
    uint8_t bellChar[8] =
    {
        0x4,0xe,0xe,0xe,0x1f,0x0,0x4
    };
    uint8_t relayCharOpen[8] =
    {
        0b01100,
        0b01100,
        0b00110,
        0b00011,
        0b00001,
        0b01100,
        0b01100,
        0b00000,
    };
    uint8_t relayCharClosed[8] =
    {
        0b01110,
        0b01110,
        0b00100,
        0b00100,
        0b00100,
        0b01110,
        0b01110,
        0b00000
    };

    physicalDisplay->CreateCustomCharacter(0, bellCharOn);
    physicalDisplay->CreateCustomCharacter(1, bellCharOff);
    physicalDisplay->CreateCustomCharacter(2, degreeChar);
    physicalDisplay->CreateCustomCharacter(3, clockChar);
    physicalDisplay->CreateCustomCharacter(4, thermoChar);
    physicalDisplay->CreateCustomCharacter(5, bellChar);
    physicalDisplay->CreateCustomCharacter(6, relayCharOpen);
    physicalDisplay->CreateCustomCharacter(7, relayCharClosed);
}

void Display::Start()
{
    xTaskCreate(TaskLoop, "DisplayTask", 1024, this, 1, nullptr);
}

void Display::Clear()
{
    DisplayCommand cmd = {};
    cmd.type = DisplayCommandType::Clear;
    xQueueSend(commandQueue, &cmd, portMAX_DELAY);
}

void Display::SetBacklight(bool on)
{
    DisplayCommand cmd = {};
    cmd.type = DisplayCommandType::SetBacklight;
    cmd.backlight.on = on;
    xQueueSend(commandQueue, &cmd, portMAX_DELAY);
}

void Display::PrintLine(int row, int col, const char* text)
{
    DisplayCommand cmd = {};
    cmd.type = DisplayCommandType::PrintLine;
    cmd.showText.row = row;
    cmd.showText.col = col;
    strncpy(cmd.showText.text, text, sizeof(cmd.showText.text) - 1);
    cmd.showText.text[sizeof(cmd.showText.text) - 1] = '\0'; // null-terminate
    xQueueSend(commandQueue, &cmd, portMAX_DELAY);
}

void Display::PrintCustomCharacter(uint8_t row, uint8_t col, uint8_t location)
{
    DisplayCommand cmd = {};
    cmd.type = DisplayCommandType::PrintSymbol;
    cmd.symbol.row = row;
    cmd.symbol.col = col;
    cmd.symbol.location = location;
    xQueueSend(commandQueue, &cmd, portMAX_DELAY);
}

void Display::TaskLoop(void* param)
{
    auto* self = static_cast<Display*>(param);
    DisplayCommand cmd;

    while (true)
    {
        if (xQueueReceive(self->commandQueue, &cmd, portMAX_DELAY))
        {
            self->ProcessCommand(cmd);
        }
    }
}

void Display::ProcessCommand(const DisplayCommand& cmd)
{
    switch (cmd.type) {
        case DisplayCommandType::Clear:
            physicalDisplay->Clear();
            break;

        case DisplayCommandType::SetBacklight:
            physicalDisplay->SetBacklight(cmd.backlight.on);
            break;

        case DisplayCommandType::PrintSymbol:
            physicalDisplay->PrintCustomCharacter(
                cmd.symbol.row, cmd.symbol.col, cmd.symbol.location);
            break;

        case DisplayCommandType::PrintLine:
            physicalDisplay->SetCursor(cmd.showText.row, cmd.showText.col);
            physicalDisplay->PrintString(cmd.showText.text);
            break;
    }
}
