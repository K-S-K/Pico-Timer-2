#pragma once

#include "../Clock/Clock.hpp"
#include "../Clock/Relay.hpp"
#include "../Clock/Alarm.hpp"

#include "../Display/IDisplay.hpp"

    enum class MainScreenCommandType {
        Clear,
        Render,
        SetClockTime,
        SetTemperature,
        SetRelayConfig,
        SetRelayState,
        SetAlarmConfig,
        SetAlarmState,
    };

struct MainScreenCommand {
    MainScreenCommandType type;
    bool render = false;

    union {
        DateTime clockTime;
        float temperature;
        RelayConfig relayConfig;
        RelayState relayState;
        AlarmConfig alarmConfig;
        AlarmState alarmState;
    };
};

class MainScreen
{
public:
    MainScreen(IDisplay* display)
        : display(display)
    {
        commandQueue = xQueueCreate(4, sizeof(MainScreenCommand));
        xTaskCreate(TaskLoop, "MainScreenTask", 2048, this, tskIDLE_PRIORITY + 1, nullptr);
    }

    void Clear() {
        MainScreenCommand cmd = {};
        cmd.type = MainScreenCommandType::Clear;
        xQueueSend(commandQueue, &cmd, portMAX_DELAY);
    }

    void Render() {
        MainScreenCommand cmd = {};
        cmd.type = MainScreenCommandType::Render;
        xQueueSend(commandQueue, &cmd, portMAX_DELAY);
    }

    void SetClockTime(const DateTime& time, bool render = false) {
        MainScreenCommand cmd = {};
        cmd.type = MainScreenCommandType::SetClockTime;
        cmd.clockTime.CopyFrom(time);
        cmd.render = render;
        xQueueSend(commandQueue, &cmd, portMAX_DELAY);
    }

    void SetTemperature(float temp, bool render = false) {
        MainScreenCommand cmd = {};
        cmd.type = MainScreenCommandType::SetTemperature;
        cmd.temperature = temp;
        cmd.render = render;
        xQueueSend(commandQueue, &cmd, portMAX_DELAY);
    }

    void SetRelayConfig(const RelayConfig& config, bool render = false) {
        MainScreenCommand cmd = {};
        cmd.type = MainScreenCommandType::SetRelayConfig;
        cmd.relayConfig.CopyDateFrom(config);
        cmd.render = render;
        xQueueSend(commandQueue, &cmd, portMAX_DELAY);
    }

    void SetRelayState(const RelayState& state, bool render = false) {
        MainScreenCommand cmd = {};
        cmd.type = MainScreenCommandType::SetRelayState;
        cmd.relayState.CopyFrom(state);
        cmd.render = render;
        xQueueSend(commandQueue, &cmd, portMAX_DELAY);
    }

    void SetAlarmConfig(const AlarmConfig& config, bool render = false) {
        MainScreenCommand cmd = {};
        cmd.type = MainScreenCommandType::SetAlarmConfig;
        cmd.alarmConfig.CopyDateFrom(config);
        cmd.render = render;
        xQueueSend(commandQueue, &cmd, portMAX_DELAY);
    }

    void SetAlarmState(const AlarmState& state, bool render = false) {
        MainScreenCommand cmd = {};
        cmd.type = MainScreenCommandType::SetAlarmState;
        cmd.alarmState.CopyFrom(state);
        cmd.render = render;
        xQueueSend(commandQueue, &cmd, portMAX_DELAY);
    }

    private:
    void inner_Render();
    QueueHandle_t commandQueue;
    static void TaskLoop(void* param);
    void ProcessCommand(const MainScreenCommand& cmd);

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
