/*
    The Piezo Speaker service for the Raspberry Pi Pico
    This class handles the piezo speaker operations such as playing sounds,
    generating tones, and controlling the speaker state.
    It uses FreeRTOS for task management and event handling.
*/

#include "FreeRTOS.h"
#include <task.h>

#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

#include "PiezoSound.hpp"


PiezoSound::PiezoSound(uint8_t pin) : pin(pin)
{
    queue = xQueueCreate(8, sizeof(SoundCommand));
    xTaskCreate(TaskFunc, "SoundTask", 256, this, 1, nullptr);
}

void PiezoSound::PlayTone(uint frequency, uint duration_ms) {
    gpio_set_function(pin, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(pin);
    uint channel = pwm_gpio_to_channel(pin);

    // Ensure wrap value (TOP) fits in 16-bit range and gives usable frequency
    uint sys_clock = clock_get_hz(clk_sys); // Usually 125 MHz
    uint divider16 = 1; // integer divide base value (divide / 16)
    uint top = sys_clock / frequency;

    if (top > 65535) {
        top = 65535;
    }

    pwm_config cfg = pwm_get_default_config();
    pwm_config_set_clkdiv(&cfg, 1.0f); // No division
    pwm_config_set_wrap(&cfg, top);
    pwm_init(slice, &cfg, true);

    pwm_set_chan_level(slice, channel, top / 2); // 50% duty = square wave

    vTaskDelay(pdMS_TO_TICKS(duration_ms));

    pwm_set_enabled(slice, false);
    gpio_set_function(pin, GPIO_FUNC_SIO);
    gpio_set_dir(pin, GPIO_OUT);
    gpio_put(pin, 0);
}

void PiezoSound::EnqueeCommand(SoundCommand command)
{
    xQueueSend(queue, &command, 0);
}

void PiezoSound::TaskFunc(void* param)
{
    auto* self = static_cast<PiezoSound*>(param);
    SoundCommand cmd;

    while (true) {
        if (xQueueReceive(self->queue, &cmd, portMAX_DELAY)) {
            self->PlaySequence(cmd);
        }
    }
}

void PiezoSound::PlaySequence(SoundCommand command) {
    switch (command) {
        case SoundCommand::MenuBeep:
            PlayTone(1000, 50);
            break;

        case SoundCommand::AlarmStart:
            for (int i = 0; i < 10; ++i) {
                PlayTone(1000, 100);
                vTaskDelay(pdMS_TO_TICKS(100));
                PlayTone(1200, 100);
                vTaskDelay(pdMS_TO_TICKS(100));
            }
            break;

        case SoundCommand::HourlyCuckoo:
            PlayTone(800, 200);
            vTaskDelay(pdMS_TO_TICKS(200));
            PlayTone(600, 200);
            vTaskDelay(pdMS_TO_TICKS(200));
            break;
    }
}