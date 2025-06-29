/*
  * Alarm Class
*/

#include "pico/stdlib.h"
#include "Alarm.hpp"

Alarm::Alarm(int qLength)
{
    // Create alarm event queue
    outQueue = xQueueCreate(qLength, sizeof(AlarmEvent));

    // init default time (example)
    alarmTimeBeg = {2025, 1, 1, 7, 0, 0};
    CalcAlarmTimeEnd();
}

void Alarm::ProcessCurrentTime(const DateTime& time)
{
    if (alarmEnabled)
    {
        bool isAlarmNow = IsAlarmTime(time);

        if (isAlarmNow && !alarmRinging)
        {
            alarmRinging = true;
            AlarmEvent evt{AlarmEventType::AlarmOn, time};
            xQueueSend(outQueue, &evt, 0);
        }
        else if (!isAlarmNow && alarmRinging)
        {
            alarmRinging = false;
            AlarmEvent evt{AlarmEventType::AlarmOff, time};
            xQueueSend(outQueue, &evt, 0);
        }
    }
}

void Alarm::SetAlarmTime(const DateTime& newTime) { alarmTimeBeg = newTime; CalcAlarmTimeEnd();}
void Alarm::SetAlarmLength(int seconds) { alarmTimeSec = seconds; CalcAlarmTimeEnd(); }
void Alarm::SetAlarmDuty(bool enable) { alarmEnabled = enable; }

void Alarm::GetAlarmTime(DateTime& outTime) { outTime = alarmTimeBeg; }
void Alarm::GetAlarmLength(int& outSeconds) { outSeconds = alarmTimeSec; }
void Alarm::GetAlarmDuty(bool& outIsEnabled) { outIsEnabled = alarmEnabled; }
void Alarm::GetAlarmStatus(bool& outIsRinging) { outIsRinging = alarmRinging; }

QueueHandle_t Alarm::GetEventQueue() const { return outQueue; }

bool Alarm::IsAlarmTime(const DateTime& currentTime) const 
{
    return
        alarmTimeBeg.hour <= currentTime.hour && currentTime.hour <= alarmTimeEnd.hour &&
        alarmTimeBeg.minute <= currentTime.minute && currentTime.minute <= alarmTimeEnd.minute &&
        alarmTimeBeg.second <= currentTime.second && currentTime.second < alarmTimeEnd.second;
}

void Alarm::CalcAlarmTimeEnd()
{
    // Calculate the end time of the alarm
    alarmTimeEnd.CopyFrom(alarmTimeBeg);
    alarmTimeEnd.AddSeconds(alarmTimeSec);
}
