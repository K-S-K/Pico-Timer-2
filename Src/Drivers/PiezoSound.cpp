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

#define NOTE_C4  262
#define NOTE_D4  294
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_G5  784


static const MelodyNote cuckooMelody[] = {
    { 784, 300 }, // G5
    {   0, 100 },
    { 784, 300 }, // G5
    {   0, 100 }
};

static const MelodyNote hatikvahEnding[] = {
    { NOTE_E4, 250 },
    { NOTE_G4, 500 },
    { NOTE_F4, 250 },
    { NOTE_F4, 250 },
    { NOTE_E4, 500 },
    { NOTE_E4, 250 },
    { NOTE_E4, 250 },
    { NOTE_F4, 250 },
    { NOTE_F4, 250 },
    { NOTE_F4, 250 },
    { NOTE_G4, 250 },
    { NOTE_C4, 750 }
};

PiezoSound::PiezoSound(uint8_t pin) : pin(pin)
{
    queue = xQueueCreate(8, sizeof(SoundCommand));
    xTaskCreate(TaskFunc, "SoundTask", 256, this, 1, nullptr);
}

void PiezoSound::PlayTone(uint frequency, uint duration_ms) {
    gpio_set_function(pin, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(pin);
    uint channel = pwm_gpio_to_channel(pin);

    uint sys_clock = clock_get_hz(clk_sys); // 125 MHz typically

    // Calculate initial top assuming clkdiv = 1.0
    float clkdiv = 1.0f;
    uint top = sys_clock / frequency;

    // If top is too large, scale clkdiv so top fits in 16-bit range
    if (top > 65535) {
        clkdiv = (float)top / 65535.0f;
        top = 65535;
    }

    pwm_config cfg = pwm_get_default_config();
    pwm_config_set_clkdiv(&cfg, clkdiv);
    pwm_config_set_wrap(&cfg, top);
    pwm_init(slice, &cfg, true);

    pwm_set_chan_level(slice, channel, top / 2); // 50% duty
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
            PlayMelody(cuckooMelody, sizeof(cuckooMelody) / sizeof(MelodyNote));
            break;

        case SoundCommand::Hatikvah:
            PlayMelody(hatikvahEnding, sizeof(hatikvahEnding) / sizeof(MelodyNote));
            break;

        case SoundCommand::Sweep:
            {
                const int start_freq = 200;   // Start of sweep (Hz)
                const int end_freq = 2000;    // End of sweep (Hz)
                const int step = 50;          // Hz step
                const int duration = 50;      // ms per tone

                for (int freq = start_freq; freq <= end_freq; freq += step)
                {
                    PlayTone(freq, duration);
                    vTaskDelay(pdMS_TO_TICKS(10)); // small pause for smoothness
                }
            }
            break;
    }
}

void PiezoSound::PlayMelody(const MelodyNote* notes, size_t length)
{
    for (size_t i = 0; i < length; ++i)
    {
        if (notes[i].frequency > 0)
        {
            PlayTone(notes[i].frequency, notes[i].duration_ms);
        }
        else
        {
            // Pause
            vTaskDelay(pdMS_TO_TICKS(notes[i].duration_ms));
        }

        // Optional: small pause between notes
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}
