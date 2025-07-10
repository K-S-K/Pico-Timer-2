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
    config.timeBeg = {2025, 1, 1, 7, 0, 0};
    config.CalcAlarmTimeEnd();
}

void Alarm::ProcessCurrentTime(const DateTime& time)
{
    if (config.enabled)
    {
        bool isAlarmNow = IsAlarmTime(time);

        if (isAlarmNow && !state.ringing)
        {
            state.ringing = true;
            AlarmEvent evt{AlarmEventType::AlarmOn, state, config};
            xQueueSend(outQueue, &evt, 0);
        }
        else if (!isAlarmNow && state.ringing)
        {
            state.ringing = false;
            AlarmEvent evt{AlarmEventType::AlarmOff, state, config};
            xQueueSend(outQueue, &evt, 0);
        }
    }
}


QueueHandle_t Alarm::GetEventQueue() const { return outQueue; }

bool Alarm::IsAlarmTime(const DateTime& currentTime) const 
{
    return
        config.timeBeg.hour <= currentTime.hour && currentTime.hour <= config.timeEnd.hour &&
        config.timeBeg.minute <= currentTime.minute && currentTime.minute <= config.timeEnd.minute &&
        config.timeBeg.second <= currentTime.second && currentTime.second < config.timeEnd.second;
}
