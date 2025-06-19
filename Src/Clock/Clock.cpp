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
    alarmTime = {2025, 1, 1, 7, 0, 0};
}

void Clock::Start() {
    xTaskCreate(TaskLoop, "ClockTickTask", 1024, this, 1, &taskHandle);
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
    currentTime.second++;
    if (currentTime.second >= 60) {
        currentTime.second = 0;
        currentTime.minute++;
        if (currentTime.minute >= 60) {
            currentTime.minute = 0;
            currentTime.hour++;
            if (currentTime.hour >= 24) {
                currentTime.hour = 0;
                // Advance day
                incrementDay();
            }
        }
    }

    // === Alarm Check ===
    if (alarmActive) {
        bool isAlarmNow =
            currentTime.hour == alarmTime.hour &&
            currentTime.minute == alarmTime.minute;

        if (isAlarmNow && !alarmRinging) {
            alarmRinging = true;
            ClockEvent evt{ClockEventType::AlarmOn, currentTime};
            xQueueSend(outQueue, &evt, 0);
        } else if (!isAlarmNow && alarmRinging) {
            alarmRinging = false;
            ClockEvent evt{ClockEventType::AlarmOff, currentTime};
            xQueueSend(outQueue, &evt, 0);
        }
    }

    // === Normal Tick Event ===
    ClockEvent evt{ClockEventType::Tick, currentTime};
    xQueueSend(outQueue, &evt, 0);
}

bool Clock::isLeapYear(int year) {
    return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
}

int Clock::daysInMonth(int month, int year) {
    switch (month) {
        case 2: return isLeapYear(year) ? 29 : 28;
        case 4: case 6: case 9: case 11: return 30;
        default: return 31;
    }
}

void Clock::incrementDay() {
    currentTime.day++;
    int maxDay = daysInMonth(currentTime.month, currentTime.year);
    if (currentTime.day > maxDay) {
        currentTime.day = 1;
        currentTime.month++;
        if (currentTime.month > 12) {
            currentTime.month = 1;
            currentTime.year++;
        }
    }
}

void Clock::Pause() { running = false; }
void Clock::Resume() { running = true; }

void Clock::SetCurrentTime(const DateTime& newTime) { currentTime = newTime; }
void Clock::SetAlarmTime(const DateTime& newTime) { alarmTime = newTime; }
void Clock::SetAlarmDuty(bool isActive) { alarmActive = isActive; }

void Clock::GetCurrentTime(DateTime& outTime) { outTime = currentTime; }
void Clock::GetAlarmTime(DateTime& outTime) { outTime = alarmTime; }

QueueHandle_t Clock::GetEventQueue() const { return outQueue; }
