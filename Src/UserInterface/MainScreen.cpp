#include <cstdio>

#include "MainScreen.hpp"


void MainScreen::TaskLoop(void* param)
{
    auto* self = static_cast<MainScreen*>(param);
    MainScreenCommand cmd = {};

    while (true)
    {
        if (xQueueReceive(self->commandQueue, &cmd, portMAX_DELAY))
        {
            self->ProcessCommand(cmd);
        }
    }
}

void MainScreen::ProcessCommand(const MainScreenCommand& cmd)
{
    switch (cmd.type)
    {
        case MainScreenCommandType::Clear:
        display->Clear();
        break;

        case MainScreenCommandType::Render:
        inner_Render();
        break;

        case MainScreenCommandType::SetClockTime:
        clockTime.CopyFrom(cmd.clockTime);
        if (cmd.render) {
                inner_Render();
        }
        break;

        case MainScreenCommandType::SetTemperature:
        temperature = cmd.temperature;
        if (cmd.render) {
                inner_Render();
        }
        break;

        case MainScreenCommandType::SetRelayConfig:
        relayConfig.CopyDateFrom(cmd.relayConfig);
        if (cmd.render) {
                inner_Render();
        }
        break;

        case MainScreenCommandType::SetRelayState:
        relayState.CopyFrom(cmd.relayState);
        if (cmd.render) {
                inner_Render();
        }
        break;

        case MainScreenCommandType::SetAlarmConfig:
        alarmConfig.CopyDateFrom(cmd.alarmConfig);
        if (cmd.render) {
                inner_Render();
        }
        break;

        case MainScreenCommandType::SetAlarmState:
        alarmState.CopyFrom(cmd.alarmState);
        if (cmd.render) {
                inner_Render();
        }
        break;

        default:
        // Handle unknown command type
        break;
    }
}

void MainScreen::inner_Render()
{
    //// Display the main screen with clock, temperature, relay, and alarm information


    //// //// //// //// //// //// //// //// ////
    //// Display Current Date and Time  // ////
    char lineClockTime[21];
    char lineClockDate[21];

    // Draw the Clock symbol
    display->PrintCustomCharacter(0, 0, 0x03);

    // Format the time and date strings
    snprintf(lineClockTime, sizeof(lineClockTime), "%04d.%02d.%02d",
            clockTime.year, clockTime.month, clockTime.day);
    snprintf(lineClockDate, sizeof(lineClockDate), "%02d:%02d:%02d",
            clockTime.hour, clockTime.minute, clockTime.second);

    display->PrintLine(0, 1, lineClockTime);
    display->PrintLine(0, 12, lineClockDate);


    //// //// //// //// //// //// ////
    //// Display Temperature  // ////
    char lineTemperature[21];

    // Draw the Thermometer symbol
    display->PrintCustomCharacter(1, 0, 0x04);

    // Format the temperature reading
    snprintf(lineTemperature, sizeof(lineTemperature), "Temperature: %.1f C", temperature);

    display->PrintLine(1, 1, lineTemperature);

    // Print the degree symbol
    display->PrintCustomCharacter(1, 18, 0x02); // Print degree symbol


    //// //// //// //// //// //// ////
    //// Display Relay Status  // ////
    char lineForRelay[21];

    // Display relay status
    display->PrintCustomCharacter(2, 0, relayState.ringing ? 0x07 : 0x06);

    // Format the Relay status string
    snprintf(lineForRelay, sizeof(lineForRelay), "Relay: %02d:%02d-%02d:%02d",
            relayConfig.timeBeg.hour, relayConfig.timeBeg.minute, 
            relayConfig.timeEnd.hour, relayConfig.timeEnd.minute);

    display->PrintLine(2, 1, lineForRelay);


    //// //// //// //// //// //// ////
    //// Display Alarm Status  // ////
    char lineForAlarm[21];

    // Draw the bell symbol at the start of the line
    display->PrintCustomCharacter(3, 0, alarmConfig.enabled && alarmState.ringing ? 0x00 : 0x01);

    // Format the alarm information
    snprintf(lineForAlarm, sizeof(lineForAlarm), "%02d sec at %02d:%02d %s", 
            alarmConfig.duration, 
            alarmConfig.timeBeg.hour, 
            alarmConfig.timeBeg.minute, 
            alarmConfig.enabled ? "On" : "Off");

    display->PrintLine(3, 1, lineForAlarm);
}
