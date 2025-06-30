/*
    SystemThermo - A class to read the system temperature
    * This class reads the system temperature using the FreeRTOS task management.
    * It provides a method to get the last read temperature in Celsius degrees.
    * The temperature is read periodically and stored in a queue for external access.
    * The class uses a tolerance value (epsilon) to determine significant temperature changes.
    * The temperature reading is done in a separate task to avoid blocking the main application.
    
    * The class is designed to be used in a FreeRTOS environment.
*/

#include <math.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

#include "SystemThermo.hpp"

SystemThermo::SystemThermo(float epsilon, int32_t measurementInterval, int qLength)
    : epsilon(epsilon), measurementInterval(measurementInterval) {


    // Initialize ADC and temperature sensor input
    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_select_input(4);  // Channel 4 is the internal temperature sensor

    // Create the queue for temperature events
    queue = xQueueCreate(qLength, sizeof(TemperatureEvent));
}

// Create the main loop task
void SystemThermo::Start() {
    xTaskCreate(TaskLoop, "Sys.Thermo.Task", 1024, this, 1, nullptr);
}

// Delete the main loop task
void SystemThermo::Stop() {
    vTaskDelete(nullptr); 
}

void SystemThermo::TaskLoop(void* param) {
    SystemThermo* self = static_cast<SystemThermo*>(param);
    while (true) {
        self->ReadTemperature();
        vTaskDelay(pdMS_TO_TICKS(self->measurementInterval));
    }
}

// Read the system temperature
void SystemThermo::ReadTemperature() {

    // Read the raw ADC value from the temperature sensor
    uint16_t raw = adc_read();
    float voltage = raw * 3.3f / 4095.0f;
    float currentTemperature = 27.0f - (voltage - 0.706f) / 0.001721f;

    // Check if the temperature change is significant
    if (abs(currentTemperature - lastReadenTemperature) < epsilon) {
        // If the change is not significant, do not update
        return;
    }

    // Update the last read temperature
    lastReadenTemperature = currentTemperature;

    // Send the new temperature event to the queue
    TemperatureEvent event = { .temperatureC = lastReadenTemperature };
    xQueueSend(queue, &event, 0);
}
