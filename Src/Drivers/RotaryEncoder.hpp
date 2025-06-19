/*
  * Rotary Encoder Driver for Raspberry Pi Pico
  * This implementation uses FreeRTOS for task management and event handling.
  * It captures rotation and button press events from a rotary encoder.
*/

#pragma once

#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

enum class EncoderEventType {
    RotatedR,
    RotatedL,
    Pressed
};

struct EncoderEvent {
    EncoderEventType type;
};

class RotaryEncoder {
public:
    RotaryEncoder(uint gpioR, uint gpioL, uint gpioBtn);
    void Init();
    QueueHandle_t GetEventQueue() const;

private:
    uint pinR, pinL, pinBtn;
    bool lastR, lastL, lastBtn;

    static void EncoderTask(void *param);
    void ProcessInput();

    QueueHandle_t eventQueue;
};
