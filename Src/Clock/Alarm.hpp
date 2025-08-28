/*
  * Alarm Class
*/

#pragma once

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include <stdint.h>
#include "DateTime.h"

#include "IDateTimeIO.hpp"

struct AlarmState {
    bool ringing = false; // True if the alarm is currently ringing

    void CopyFrom(const AlarmState& other) {
        ringing = other.ringing;
    }
};

struct AlarmConfig {
    DateTime timeBeg; // Start time of the alarm
    DateTime timeEnd; // End time of the alarm
    int duration = 10; // Duration of the alarm in seconds
    bool enabled = false; // True if the alarm is set and active

    void CalcAlarmTimeEnd()
    {
        // Calculate the end time of the alarm
        timeEnd.CopyFrom(timeBeg);
        timeEnd.AddSeconds(duration);
    }

    void CopyTimeFrom(const AlarmConfig& other) {
        timeBeg.CopyTimeFrom(other.timeBeg);
        duration = other.duration;
        enabled = other.enabled;
        CalcAlarmTimeEnd();
    }
};

enum class AlarmEventType {
    AlarmOn,
    AlarmOff,
    Reconfigured,
};

struct AlarmEvent {
    AlarmEventType type;
    AlarmState state;   // Current state of the alarm
    AlarmConfig config; // Current configuration of the alarm
};

class Alarm : public IDateTimeIO {
public:
    Alarm(int qLength);

    void ProcessCurrentTime(const DateTime& time);

    void SetTime(const DateTime& newTime)
    {
        AlarmConfig alarmConfig;
        GetAlarmConfig(alarmConfig);
        alarmConfig.timeBeg.CopyTimeFrom(newTime);
        SetAlarmConfig(alarmConfig);
    }

    void GetTime(DateTime& outTime)
    {
        outTime.hour = config.timeBeg.hour;
        outTime.minute = config.timeBeg.minute;
    }

    void SetAlarmConfig(const AlarmConfig& newConfig) {
        config.CopyTimeFrom(newConfig);
        AlarmEvent evt{AlarmEventType::Reconfigured, state, config};
        xQueueSend(outQueue, &evt, 0);
    }

    void GetAlarmConfig(AlarmConfig& outConfig) const {
        outConfig.CopyTimeFrom(config);
    }

    void GetAlarmState(AlarmState& outState) const {
        outState.CopyFrom(state);
    }

    QueueHandle_t GetEventQueue() const;

private:
    static void TaskLoop(void* param);

    bool IsAlarmTime(const DateTime& currentTime) const;

    AlarmState state;
    AlarmConfig config;

    QueueHandle_t outQueue;
};
