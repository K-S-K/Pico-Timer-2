/*
    The Piezo Speaker service for the Raspberry Pi Pico
    This class handles the piezo speaker operations such as playing sounds,
    generating tones, and controlling the speaker state.
    It uses FreeRTOS for task management and event handling.
*/

#pragma once

#include <stdint.h>
#include "queue.h"

// class PiezoSound

enum class SoundCommand {
    MenuBeep,
    AlarmStart,
    HourlyCuckoo
};

class PiezoSound {
public:
    PiezoSound(uint8_t pin);

    void PlayMenuBeep() {
        EnqueeCommand(SoundCommand::MenuBeep);
    }
    void PlayAlarmStart() {
        EnqueeCommand(SoundCommand::AlarmStart);
    }
    void PlayHourlyCuckoo() {
        EnqueeCommand(SoundCommand::HourlyCuckoo);
    }

private:
    void EnqueeCommand(SoundCommand command);
    static void TaskFunc(void* param);

    void PlayTone(uint frequency, uint duration_ms);
    void PlaySequence(SoundCommand command);

    QueueHandle_t queue;
    uint8_t pin;
};
