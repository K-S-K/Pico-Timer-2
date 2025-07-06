/*
  * Relay Class
*/

#include "pico/stdlib.h"
#include "Relay.hpp"

Relay::Relay(int qLength)
{
    // Create relay event queue
    outQueue = xQueueCreate(qLength, sizeof(RelayEvent));

    // init default time (example)
    relayTimeBeg = {2025, 1, 1, 7, 0, 0};
    relayTimeEnd = {2025, 1, 1, 19, 0, 0};
}

void Relay::ProcessCurrentTime(const DateTime& time)
{
    if (relayEnabled)
    {
        bool isRelayNow = IsRelayTime(time);

        if (isRelayNow && !relayRinging)
        {
            relayRinging = true;
            RelayEvent evt{RelayEventType::RelayOn, time};
            xQueueSend(outQueue, &evt, 0);
        }
        else if (!isRelayNow && relayRinging)
        {
            relayRinging = false;
            RelayEvent evt{RelayEventType::RelayOff, time};
            xQueueSend(outQueue, &evt, 0);
        }

        relayRinging = isRelayNow; // Update the ringing status
    }
}

void Relay::SetRelayTimes(const DateTime& timeBeg, const DateTime& timeEnd)
{
    relayTimeBeg.CopyFrom(timeBeg);
    relayTimeEnd.CopyFrom(timeEnd);
}
void Relay::GetRelayTimes(DateTime& outTimeBeg, DateTime& outTimeEnd)
{
    outTimeBeg.CopyFrom(relayTimeBeg);
    outTimeEnd.CopyFrom(relayTimeEnd);
}
void Relay::SetRelayDuty(bool enable) { relayEnabled = enable; }
void Relay::GetRelayDuty(bool& outIsEnabled) { outIsEnabled = relayEnabled; }

void Relay::GetRelayStatus(bool& outIsRinging) { outIsRinging = relayRinging; }

QueueHandle_t Relay::GetEventQueue() const { return outQueue; }

bool Relay::IsRelayTime(const DateTime& currentTime) const 
{
    return
        relayTimeBeg.hour <= currentTime.hour && currentTime.hour <= relayTimeEnd.hour &&
        relayTimeBeg.minute <= currentTime.minute && currentTime.minute <= relayTimeEnd.minute ;
}
