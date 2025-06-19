#pragma once

#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include <stdint.h>

#include "../Drivers/HD44780.hpp"

enum class DisplayCommandType {
    Clear,
    SetBacklight,
    ShowText
};

struct DisplayCommand {
    DisplayCommandType type;

    union {
        struct {
            int row;
            int col;
            char text[32]; // adjust to LCD width
        } showText;

        struct {
            bool on;
        } backlight;
    };
};

class Display {
public:
    Display(HD44780* lcd);
    void Start();

    void Clear();
    void SetBacklight(bool on);
    void ShowText(int row, int col, const char* text);

private:
    static void TaskLoop(void* param);
    void ProcessCommand(const DisplayCommand& cmd);

    QueueHandle_t commandQueue;
    HD44780* physicalDisplay; // Assuming HD44780 is a class for the LCD driver
};
