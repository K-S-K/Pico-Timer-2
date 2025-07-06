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
    SetRelayOn,
    SetRelayOff,
    BlinkClockTick,
};

struct GPIOCommand {
    GPIOCommandType type;
};

class GPIOControl
{
public:
    GPIOControl(int pinTickLed, int pinAlrmCtrl, int pinRelayCtrl);
    void AlarmOn() { EnqueeCommand(GPIOCommandType::SetAlarmOn); }
    void AlarmOff() { EnqueeCommand(GPIOCommandType::SetAlarmOff); }
    void RelayOn() { EnqueeCommand(GPIOCommandType::SetRelayOn); }
    void RelayOff() { EnqueeCommand(GPIOCommandType::SetRelayOff); }
    void BlinkTickLed();

private:
    void PrepareGPIO(int pin, int initialState = 0);
    void EnqueeCommand(GPIOCommandType cmdType);
    void ProcessCommand(const GPIOCommand& cmd);
    static void TaskLoop(void* param);
    void InnerBlinkTickLed();
    void Start();

private:
    uint8_t pin_tick_led;
    uint8_t pin_alrm_ctrl;
    uint8_t pin_relay_ctrl;
    QueueHandle_t commandQueue;
};
