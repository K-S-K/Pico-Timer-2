/*
    SystemThermo - A class to read the system temperature
    * This class reads the system temperature using the FreeRTOS task management.
    * It provides a method to get the last read temperature in Celsius degrees.
    * The temperature is read periodically and stored in a queue for external access.
    * The class uses a tolerance value (epsilon) to determine significant temperature changes.
    * The temperature reading is done in a separate task to avoid blocking the main application.

    * The class is designed to be used in a FreeRTOS environment.
*/

#pragma once

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include <stdint.h>

struct TemperatureEvent {
    float temperatureC;
};

class SystemThermo
{
public:
    SystemThermo(float epsilon = 0.01f, int32_t measurementInterval = 2000, int qLength = 4);

    // Create the main loop task
    void Start();
    
    // Delete the main loop task
    void Stop();   

    // Method to get the last readen 
    // temperature in Celsius degrees
    float GetLastReadenTemperature() const {
        return lastReadenTemperature;
    }

    // Get the event queue for temperature events
    QueueHandle_t GetEventQueue() const {
        return queue;
    }


private:
    static void TaskLoop(void* param);

    // Read the system temperature
    void ReadTemperature();

    // Measurement interval in milliseconds
    int32_t measurementInterval = 2000; 


    // Store the last readen  temperature in Celsius
    float lastReadenTemperature = 0.0f;

    // Tolerance for temperature changes
    float epsilon = 0.01f; 

    // Queue for temperature events
    QueueHandle_t queue;
};
