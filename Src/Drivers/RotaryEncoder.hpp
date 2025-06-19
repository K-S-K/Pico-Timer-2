/*
  * Rotary Encoder Driver for Raspberry Pi Pico
  * This implementation uses FreeRTOS for task management and event handling.
  * It captures rotation and button press events from a rotary encoder.
*/

#pragma once

#include "pico/stdlib.h"
#include "pico/time.h"
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
    RotaryEncoder(uint gpioL, uint gpioR, uint gpioBtn);
    void Init();
    QueueHandle_t GetEventQueue() const;

private:
    uint pinL, pinR, pinBtn;
    bool lastL, lastR, lastBtn;

    absolute_time_t lastRotationTime;
    absolute_time_t lastButtonTime;
    static constexpr uint ROTATION_DEBOUNCE_US = 5000;
    static constexpr uint BUTTON_DEBOUNCE_US = 10000;

    static void EncoderTask(void *param);
    void ProcessInput();

    QueueHandle_t eventQueue;
};
