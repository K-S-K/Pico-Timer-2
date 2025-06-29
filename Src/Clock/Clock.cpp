/*
  * Clock Class
    * This class manages the system clock, including current time, alarm functionality,
    * and periodic ticking.
    * It uses FreeRTOS for task management and event handling.
    * The clock ticks every second and can be paused or resumed.
    * It also supports setting and getting the current time and alarm time.
    * The clock emits events to a queue for external handling.
    * The clock can be used in applications that require timekeeping and alarm functionality.
    * It is designed to be thread-safe and can be used in a FreeRTOS environment
*/

#include "pico/stdlib.h"

#include "Clock.hpp"

Clock::Clock(int qLength){
    // Create clock event queue
    outQueue = xQueueCreate(qLength, sizeof(ClockEvent));

    // init default time (example)
    currentTime = {2025, 1, 1, 12, 0, 0};
}

void Clock::Start() {
    xTaskCreate(TaskLoop, "ClockTickTask", 1024, this, 1, nullptr);
}

void Clock::TaskLoop(void* param) {
    Clock* self = static_cast<Clock*>(param);
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        if (self->running) {
            self->Tick();
        }
    }
}

void Clock::Tick() {
    // === Time Incrementation ===
    currentTime.IncrementSeconds();

    // === Normal Tick Event ===
    ClockEvent evt{ClockEventType::Tick, currentTime};
    xQueueSend(outQueue, &evt, 0);
}

void Clock::Pause() { running = false; }
void Clock::Resume() { running = true; }

void Clock::SetCurrentTime(const DateTime& newTime) { currentTime = newTime; }
void Clock::GetCurrentTime(DateTime& outTime) { outTime.CopyFrom(currentTime); }
QueueHandle_t Clock::GetEventQueue() const { return outQueue; }
