#include <cstdio>


#include "MainScreen.hpp"

void MainScreen::Render()
{
    char lineClockTime[21];
    char lineClockDate[21];
    char lineTemperature[21];
    char lineForRelay[21];
    char lineForAlarm[21];

    // Display relay status
    display->PrintCustomCharacter(2, 0, relayState.ringing ? 0x07 : 0x06);

                
    // Draw the bell symbol at the start of the line
    display->PrintCustomCharacter(3, 0, alarmConfig.enabled && alarmState.ringing ? 0x00 : 0x01);

    // Draw the clock and thermo symbols
    display->PrintCustomCharacter(0, 0, 0x03); // Clock
    display->PrintCustomCharacter(1, 0, 0x04); // Therm
    
    // Print the degree symbol
    display->PrintCustomCharacter(1, 18, 0x02); // Print degree symbol


    // Format the time and date strings
    snprintf(lineClockTime, sizeof(lineClockTime), "%04d.%02d.%02d",
            clockTime.year, clockTime.month, clockTime.day);
    snprintf(lineClockDate, sizeof(lineClockDate), "%02d:%02d:%02d",
            clockTime.hour, clockTime.minute, clockTime.second);

    // Format the temperature reading
    snprintf(lineTemperature, sizeof(lineTemperature), "Temperature: %.1f", temperature);

    // Format the Relay status string
    snprintf(lineForRelay, sizeof(lineForRelay), "Relay: %02d:%02d-%02d:%02d",
            relayConfig.timeBeg.hour, relayConfig.timeBeg.minute, 
            relayConfig.timeEnd.hour, relayConfig.timeEnd.minute);

    // Format the alarm information
    snprintf(lineForAlarm, sizeof(lineForAlarm), "%02d sec at %02d:%02d %s", 
            alarmConfig.duration, 
            alarmConfig.timeBeg.hour, 
            alarmConfig.timeBeg.minute, 
            alarmConfig.enabled ? "On" : "Off");

    display->ShowText(0, 1, lineClockTime);
    display->ShowText(0, 12, lineClockDate);

    display->ShowText(1, 1, lineTemperature);
    display->ShowText(1, 19, "C");
    
    display->ShowText(2, 1, lineForRelay);
    
    display->ShowText(3, 1, lineForAlarm);

}
