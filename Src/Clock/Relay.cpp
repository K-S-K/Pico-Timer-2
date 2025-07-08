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
    config.timeBeg = {2025, 1, 1, 7, 0, 0};
    config.timeEnd = {2025, 1, 1, 19, 0, 0};
}

void Relay::ProcessCurrentTime(const DateTime& time)
{
    if (config.enabled)
    {
        bool isRelayNow = IsRelayTime(time);

        if (isRelayNow && !state.ringing)
        {
            state.ringing = true;
            RelayEvent evt{RelayEventType::RelayOn, time};
            xQueueSend(outQueue, &evt, 0);
        }
        else if (!isRelayNow && state.ringing)
        {
            state.ringing = false;
            RelayEvent evt{RelayEventType::RelayOff, time};
            xQueueSend(outQueue, &evt, 0);
        }
    }
}

QueueHandle_t Relay::GetEventQueue() const { return outQueue; }

bool Relay::IsRelayTime(const DateTime& currentTime) const 
{
    return
        config.timeBeg.hour <= currentTime.hour && currentTime.hour <= config.timeEnd.hour &&
        config.timeBeg.minute <= currentTime.minute && currentTime.minute < config.timeEnd.minute ;
}
