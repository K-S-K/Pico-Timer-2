/*
  * Relay Class
*/

#pragma once

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include <stdint.h>
#include "DateTime.h"

struct RelayState {
    bool ringing = false; // True if the relay is currently ringing

    void CopyFrom(const RelayState& other) {
        ringing = other.ringing;
    }
};

struct RelayConfig {
    DateTime timeBeg; // Start time of the relay
    DateTime timeEnd; // End time of the relay
    bool enabled = false; // True if the relay is set and active

    void CopyDateFrom(const RelayConfig& other) {
        timeBeg.CopyTimeFrom(other.timeBeg);
        timeEnd.CopyTimeFrom(other.timeEnd);
        enabled = other.enabled;
    }
};

enum class RelayEventType {
    RelayOn,
    RelayOff,
    Reconfigured
};

struct RelayEvent {
    RelayEventType type;
    RelayState state;   // Current state of the relay
    RelayConfig config; // Current configuration of the relay
};

class Relay {
public:
    Relay(int qLength);

    void ProcessCurrentTime(const DateTime& time);

    void SetRelayConfig(const RelayConfig& newConfig) {
        config.CopyDateFrom(newConfig);
        RelayEvent evt{RelayEventType::Reconfigured, state, config};
        xQueueSend(outQueue, &evt, 0);
    }

    void GetRelayConfig(RelayConfig& outConfig) const {
        outConfig.CopyDateFrom(config);
    }

    void GetRelayState(RelayState& outState) const {
        outState.CopyFrom(state);
    }

    QueueHandle_t GetEventQueue() const;

private:
    static void TaskLoop(void* param);

    bool IsRelayTime(const DateTime& currentTime) const;

    RelayConfig config;
    RelayState state;

    QueueHandle_t outQueue;
};
