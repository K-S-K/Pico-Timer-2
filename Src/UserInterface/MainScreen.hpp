#pragma once

#include "../Clock/Clock.hpp"
#include "../Clock/Relay.hpp"
#include "../Clock/Alarm.hpp"

#include "../Display/IDisplay.hpp"

class MainScreen
{
public:
    MainScreen(IDisplay* display)
        : display(display)
    {
    }

    void Render();

    void Clear() {
        display->Clear();
    }

    void SetClockTime(const DateTime& time, bool render = false) {
        clockTime.CopyFrom(time);
        if (render) {
            Render();
        }
    }

    void SetTemperature(float temp, bool render = false) {
        temperature = temp;
        if (render) {
            Render();
        }
    }

    void SetRelayConfig(const RelayConfig& config, bool render = false) {
        relayConfig.CopyDateFrom(config);
        if (render) {
            Render();
        }
    }

    void SetRelayState(const RelayState& state, bool render = false) {
        relayState.CopyFrom(state);
        if (render) {
            Render();
        }
    }

    void SetAlarmConfig(const AlarmConfig& config, bool render = false) {
        alarmConfig.CopyDateFrom(config);
        if (render) {
            Render();
        }
    }

    void SetAlarmState(const AlarmState& state, bool render = false) {
        alarmState.CopyFrom(state);
        if (render) {
            Render();
        }
    }

private:
    DateTime clockTime;
    float temperature = 0.0f;
    RelayConfig relayConfig;
    AlarmConfig alarmConfig;
    RelayState relayState;
    AlarmState alarmState;

private:
    IDisplay* display;
};
