/*
  * Rotary Encoder Driver for Raspberry Pi Pico
  * This implementation uses FreeRTOS for task management and event handling.
  * It captures rotation and button press events from a rotary encoder.
*/

#include "RotaryEncoder.hpp"

RotaryEncoder::RotaryEncoder(uint gpioL, uint gpioR, uint gpioBtn)
    : pinL(gpioL), pinR(gpioR), pinBtn(gpioBtn),
      lastL(true), lastR(true), lastBtn(true), eventQueue(nullptr),
      lastRotationTime(get_absolute_time()),
      lastButtonTime(get_absolute_time()) {}

void RotaryEncoder::Init() {
    gpio_init(pinL); gpio_set_dir(pinL, GPIO_IN); gpio_pull_up(pinL);
    gpio_init(pinR); gpio_set_dir(pinR, GPIO_IN); gpio_pull_up(pinR);
    gpio_init(pinBtn); gpio_set_dir(pinBtn, GPIO_IN); gpio_pull_up(pinBtn);

    eventQueue = xQueueCreate(10, sizeof(EncoderEvent));
    xTaskCreate(EncoderTask, "EncoderTask", 512, this, 1, nullptr);
}

QueueHandle_t RotaryEncoder::GetEventQueue() const {
    return eventQueue;
}

void RotaryEncoder::EncoderTask(void *param) {
    RotaryEncoder *self = static_cast<RotaryEncoder*>(param);
    while (true) {
        self->ProcessInput();
        vTaskDelay(pdMS_TO_TICKS(2));
    }
}

void RotaryEncoder::ProcessInput() {
    bool spinL = gpio_get(pinL) == 0;
    bool spinR = gpio_get(pinR) == 0;
    bool btn = gpio_get(pinBtn) == 0;

    // Rotation debounce
    if (spinL != lastL || spinR != lastR) {
        if (lastL && !spinL) {
            absolute_time_t now = get_absolute_time();
            if (absolute_time_diff_us(lastRotationTime, now) > ROTATION_DEBOUNCE_US) {
                EncoderEvent evt;
                evt.type = spinR ? 
                    EncoderEventType::RotatedL : 
                    EncoderEventType::RotatedR;
                xQueueSend(eventQueue, &evt, 0);
                lastRotationTime = now;
            }
        }
        lastL = spinL;
        lastR = spinR;
    }

    // Button debounce
    if (!lastBtn && btn) {
        absolute_time_t now = get_absolute_time();
        if (absolute_time_diff_us(lastButtonTime, now) > BUTTON_DEBOUNCE_US) {
            EncoderEvent evt{EncoderEventType::Pressed};
            xQueueSend(eventQueue, &evt, 0);
            lastButtonTime = now;
        }
    }
    lastBtn = btn;
}
