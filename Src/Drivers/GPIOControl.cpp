/*
    The GPIO Signal controller for the Raspberry Pi Pico
    This class handles GPIO operations such as GPIO 
    pin state changes and also blinking a LED.
*/

#include "FreeRTOS.h"
#include <task.h>

#include "pico/stdlib.h"

#include "GPIOControl.hpp"

GPIOControl::GPIOControl(int pinTickLed)
{
    pin_tick_led = pinTickLed;
    commandQueue = xQueueCreate(8, sizeof(GPIOCommand));

    Start();
}

void GPIOControl::BlinkTickLed()
{
    GPIOCommand cmd = {};
    cmd.type = GPIOCommandType::BlinkClockTick;
    xQueueSend(commandQueue, &cmd, portMAX_DELAY);
}

void GPIOControl::InnerBlinkTickLed()
{
    gpio_put(pin_tick_led, 1);
    vTaskDelay(100);

    gpio_put(pin_tick_led, 0);
    vTaskDelay(100);

    gpio_put(pin_tick_led, 1);
    vTaskDelay(100);

    gpio_put(pin_tick_led, 0);
    vTaskDelay(700);
}

void GPIOControl::Start()
{
    xTaskCreate(TaskLoop, "GPIOTask", 1024, this, 1, nullptr);
}

void GPIOControl::TaskLoop(void* param)
{
    auto* self = static_cast<GPIOControl*>(param);
    GPIOCommand cmd;

    while (true) {
        if (xQueueReceive(self->commandQueue, &cmd, portMAX_DELAY)) {
            self->ProcessCommand(cmd);
        }
    }
}

void GPIOControl::ProcessCommand(const GPIOCommand& cmd)
{
    switch (cmd.type)
    {
        case GPIOCommandType::BlinkClockTick:
            InnerBlinkTickLed();
            break;

        default:
            break;
    }
}
