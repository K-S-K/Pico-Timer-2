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

#pragma once

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include <stdint.h>

#include "IDisplay.hpp"

#include "../Drivers/HD44780.hpp"

enum class DisplayCommandType {
    Clear,
    PrintLine,
    SetBacklight,
};

struct DisplayCommand {
    DisplayCommandType type;

    union {
        struct {
            int row;
            int col;
            char text[21]; // adjust to LCD width
        } showText;

        struct {
            bool on;
        } backlight;
    };
};

class Display : public IDisplay {
public:
    Display(HD44780* lcd);
    void Start();

    void Clear() override;
    void SetBacklight(bool on) override;
    void PrintLine(int row, int col, const char* text) override;
    void PrintCustomCharacter(uint8_t row, uint8_t col, uint8_t location) override;

private:
    static void TaskLoop(void* param);
    void ProcessCommand(const DisplayCommand& cmd);

    QueueHandle_t commandQueue;
    HD44780* physicalDisplay; // Assuming HD44780 is a class for the LCD driver
};
