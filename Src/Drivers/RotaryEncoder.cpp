/*
  * Rotary Encoder Driver for Raspberry Pi Pico
  * This implementation uses FreeRTOS for task management and event handling.
  * It captures rotation and button press events from a rotary encoder.
*/

#include "RotaryEncoder.hpp"

RotaryEncoder::RotaryEncoder(uint gpioR, uint gpioL, uint gpioBtn)
    : pinR(gpioR), pinL(gpioL), pinBtn(gpioBtn),
      lastR(true), lastL(true), lastBtn(true), eventQueue(nullptr) {}

void RotaryEncoder::Init() {
    gpio_init(pinR); gpio_set_dir(pinR, GPIO_IN); gpio_pull_up(pinR);
    gpio_init(pinL); gpio_set_dir(pinL, GPIO_IN); gpio_pull_up(pinL);
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
    bool spinR = gpio_get(pinR) == 0;
    bool spinL = gpio_get(pinL) == 0;
    bool btn = gpio_get(pinBtn) == 0;

    if (spinR != lastR || spinL != lastL) {
        if (lastR && !spinR) {
            EncoderEvent evt;
            evt.type = spinL ? 
                EncoderEventType::RotatedL : 
                EncoderEventType::RotatedR;
            xQueueSend(eventQueue, &evt, 0);
        }
        lastR = spinR;
        lastL = spinL;
    }

    if (!lastBtn && btn) {
        EncoderEvent evt{EncoderEventType::Pressed};
        xQueueSend(eventQueue, &evt, 0);
    }

    lastBtn = btn;
}
