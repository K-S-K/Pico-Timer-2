/*
    The GPIO Signal controller for the Raspberry Pi Pico
    This class handles GPIO operations such as GPIO 
    pin state changes and also blinking a LED.
*/

#include "FreeRTOS.h"
#include <task.h>

#include "pico/stdlib.h"

#include "GPIOControl.hpp"

GPIOControl::GPIOControl(int pinTickLed, int pinAlrmLed)
{
    pin_tick_led = pinTickLed;
    pin_alrm_led = pinAlrmLed;

    PrepareGPIO(pin_alrm_led);

    commandQueue = xQueueCreate(8, sizeof(GPIOCommand));

    Start();
}

void GPIOControl::AlarmOn()
{
    GPIOCommand cmd= {};
    cmd.type = GPIOCommandType::SetAlarmOn;
    xQueueSend(commandQueue, &cmd, portMAX_DELAY);
}

void GPIOControl::AlarmOff()
{
    GPIOCommand cmd= {};
    cmd.type = GPIOCommandType::SetAlarmOff;
    xQueueSend(commandQueue, &cmd, portMAX_DELAY);
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

void GPIOControl::PrepareGPIO(int pin, int initialState)
{
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_OUT);
    gpio_pull_up(pin);
    gpio_put(pin, initialState);
}

void GPIOControl::ProcessCommand(const GPIOCommand& cmd)
{
    switch (cmd.type)
    {
        case GPIOCommandType::SetAlarmOn:
            gpio_put(pin_alrm_led, 1);
            break;

        case GPIOCommandType::SetAlarmOff:
            gpio_put(pin_alrm_led, 0);
            break;

        case GPIOCommandType::BlinkClockTick:
            InnerBlinkTickLed();
            break;

        default:
            break;
    }
}
