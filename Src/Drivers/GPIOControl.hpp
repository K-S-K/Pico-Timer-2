/*
    The GPIO Signal controller for the Raspberry Pi Pico
    This class handles GPIO operations such as GPIO 
    pin state changes and also blinking a LED.
*/

#pragma once

#include <stdint.h>
#include "queue.h"

enum class GPIOCommandType {
    SetAlarmOn,
    SetAlarmOff,
    BlinkClockTick,
};

struct GPIOCommand {
    GPIOCommandType type;
};

class GPIOControl
{
public:
    GPIOControl(int pinTickLed);
    void BlinkTickLed();

private:
    void ProcessCommand(const GPIOCommand& cmd);
    static void TaskLoop(void* param);
    void InnerBlinkTickLed();
    void Start();

private:
    uint8_t pin_tick_led;
    QueueHandle_t commandQueue;
};
