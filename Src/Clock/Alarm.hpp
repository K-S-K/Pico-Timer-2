/*
  * Alarm Class
*/

#pragma once

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include <stdint.h>
#include "DateTime.h"

enum class AlarmEventType {
    AlarmOn,
    AlarmOff,
};

struct AlarmEvent {
    AlarmEventType type;
    DateTime time;
};

class Alarm {
public:
    Alarm(int qLength);

    void ProcessCurrentTime(const DateTime& time);

    void SetAlarmTime(const DateTime& alarmTime);
    void SetAlarmLength(int seconds);
    void SetAlarmDuty(bool isActive);

    void GetAlarmTime(DateTime& outTime);
    void GetAlarmLength(int& outSeconds);
    void GetAlarmDuty(bool& outIsEnabled);
    void GetAlarmStatus(bool& outIsRinging);

    QueueHandle_t GetEventQueue() const;

private:
    static void TaskLoop(void* param);

    void CalcAlarmTimeEnd();
    bool IsAlarmTime(const DateTime& currentTime) const;

    DateTime alarmTimeBeg;
    DateTime alarmTimeEnd;
    int alarmTimeSec = 10; // seconds to ring the alarm
    bool alarmRinging = false; // true if the alarm is currently ringing
    bool alarmEnabled = false; // true if the alarm is set and active
    bool running = true; // true if the clock is running (ticking)

    QueueHandle_t outQueue;
};
