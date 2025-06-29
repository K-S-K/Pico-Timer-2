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

#pragma once

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include <stdint.h>
#include "DateTime.h"

enum class ClockEventType {
    Tick,
    AlarmOn,
    AlarmOff
};

struct ClockEvent {
    ClockEventType type;
    DateTime currentTime;
};

class Clock {
public:
    Clock(int qLength = 4);

    void Resume(); // resumes ticking
    void Pause();  // pauses ticking

    void SetCurrentTime(const DateTime& newTime);
    void SetAlarmTime(const DateTime& alarmTime);
    void SetAlarmLength(int seconds);
    void SetAlarmDuty(bool isActive);

    void GetCurrentTime(DateTime& outTime);
    void GetAlarmTime(DateTime& outTime);
    void GetAlarmLength(int& outSeconds);
    void GetAlarmDuty(bool& outIsActive);

    void Start();  // create the task

    QueueHandle_t GetEventQueue() const;

private:
    static void TaskLoop(void* param);
    void Tick();  // the per-second logic

    void CalcAlarmTimeEnd();
    bool IsAlarmTime() const;

    DateTime currentTime;
    DateTime alarmTimeBeg;
    DateTime alarmTimeEnd;
    int alarmTimeSec = 10; // seconds to ring the alarm
    bool alarmRinging = false; // true if the alarm is currently ringing
    bool alarmEnabled = false; // true if the alarm is set and active
    bool running = true; // true if the clock is running (ticking)

    QueueHandle_t outQueue;
    TaskHandle_t taskHandle = nullptr;
};
