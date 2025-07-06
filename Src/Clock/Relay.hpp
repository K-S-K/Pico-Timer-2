/*
  * Relay Class
*/

#pragma once

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include <stdint.h>
#include "DateTime.h"

enum class RelayEventType {
    RelayOn,
    RelayOff,
};

struct RelayEvent {
    RelayEventType type;
    DateTime time;
};

class Relay {
public:
    Relay(int qLength);

    void ProcessCurrentTime(const DateTime& time);

    void SetRelayTimes(const DateTime& timeBeg, const DateTime& timeEnd);
    void GetRelayTimes(DateTime& outTimeBeg, DateTime& outTimeEnd);

    void SetRelayDuty(bool isActive);
    void GetRelayDuty(bool& outIsEnabled);

    void GetRelayStatus(bool& outIsRinging);

    QueueHandle_t GetEventQueue() const;

private:
    static void TaskLoop(void* param);

    bool IsRelayTime(const DateTime& currentTime) const;

    DateTime relayTimeBeg;
    DateTime relayTimeEnd;
    int relayTimeSec = 10; // seconds to ring the relay
    bool relayRinging = false; // true if the relay is currently ringing
    bool relayEnabled = false; // true if the relay is set and active
    bool running = true; // true if the clock is running (ticking)

    QueueHandle_t outQueue;
};
