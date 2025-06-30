#include "FreeRTOS.h"
#include "pico/stdlib.h"
#include <task.h>
#include <cstdio>

#include "../Clock/Clock.hpp"
#include "../Clock/Alarm.hpp"
#include "../Drivers/HD44780.hpp"
#include "../Display/Display.hpp"
#include "../Display/IDisplay.hpp"
#include "../Drivers/PiezoSound.hpp"
#include "../Drivers/GPIOControl.hpp"
#include "../Drivers/RotaryEncoder.hpp"
#include "../UserInterface/Menu/MenuController.hpp"


struct UiTaskContext {
    QueueHandle_t encoderQueue;
    IDisplay* display;
    MenuController* menu;
};

struct ClockTaskContext {
    QueueHandle_t queue;
    GPIOControl gpio;
    MenuController* menu;
    IDisplay* display;
    Alarm* alarm;
    Clock* clock;
};

struct AlarmTaskContext {
    PiezoSound sound;
    GPIOControl gpio;
    Alarm* alarm;
};

static void UserInterfaceTask(void *param) {
    UiTaskContext* uiCtx = static_cast<UiTaskContext*>(param);
    QueueHandle_t q = uiCtx->encoderQueue;
    MenuController* menu = uiCtx->menu;
    IDisplay* lcd = uiCtx->display;

    EncoderEvent clockEvent;

    while (true) {
        if (xQueueReceive(q, &clockEvent, portMAX_DELAY)) {

            // Convert EncoderEvent to MenuEvent
            MenuEvent menuEvt;
            switch (clockEvent.type) {
                case EncoderEventType::RotatedR:
                    menuEvt = MenuEvent::MoveFwd;
                    break;
                case EncoderEventType::RotatedL:
                    menuEvt = MenuEvent::MoveBack;
                    break;
                case EncoderEventType::Pressed:
                    menuEvt = MenuEvent::PushButton;
                    break;
                default:
                    continue; // Ignore unknown events
            }

            // Process the menu event
            menu->ProcessEvent(menuEvt);
        }
    }
}

void AlarmTask(void* param) {
    AlarmTaskContext* alarmCtx = static_cast<AlarmTaskContext*>(param);
    GPIOControl* gpio = &alarmCtx->gpio;
    Alarm* alarm = alarmCtx->alarm;
    PiezoSound* sound = &alarmCtx->sound;
    QueueHandle_t q = alarm->GetEventQueue();

    while (true) {
        AlarmEvent alarmEvent;
        if (xQueueReceive(q, &alarmEvent, portMAX_DELAY)) {
            switch (alarmEvent.type) {
                case AlarmEventType::AlarmOn:
                    gpio->AlarmOn();
                    sound->PlayAlarmStart(); // Play alarm sound
                    // sound->PlayHourlyCuckoo(); // Play hourly cuckoo sound
                    // sound->PlaySweep(); // Play a sweep sound
                    // sound->PlayMenuBeep(); // Play a menu beep sound
                    // sound->PlayHatikvah(); // Play Hatikvah melody
                    break;

                case AlarmEventType::AlarmOff:
                    gpio->AlarmOff();
                    break;
            }
        }
    }
}

void ClockDisplayTask(void* param) {
    ClockTaskContext* uiCtx = static_cast<ClockTaskContext*>(param);
    GPIOControl* gpio = &uiCtx->gpio;
    QueueHandle_t q = uiCtx->queue;
    MenuController* menu = uiCtx->menu;
    IDisplay* lcd = uiCtx->display;
    Alarm* alarm = uiCtx->alarm;
    Clock* clock = uiCtx->clock;

    char line0[32];
    char line1[32];
    // char line2[32];
    char line3[32];
    bool alarmIsOn = false;

    while (true) {
        ClockEvent clockEvent;
        if (xQueueReceive(q, &clockEvent, portMAX_DELAY)) {

            // Check if we are in the main screen of the menu
            if(menu->GetMenuState() != MenuState::MainScreen) {
                // If we are not in the main screen, ignore clock events
                // This allows the menu to take precedence over clock updates
                continue;
            }
            
            if (clockEvent.type != ClockEventType::Tick) {
                // Ignore non-tick events
                continue;
            }

            // Process the clock event
            {
                gpio->BlinkTickLed();
                alarm->ProcessCurrentTime(clockEvent.currentTime);

                int seconds;
                bool enabled;
                DateTime alarmTime;
                alarm->GetAlarmDuty(enabled);
                alarm->GetAlarmLength(seconds);
                alarm->GetAlarmTime(alarmTime);
                alarm->GetAlarmStatus(alarmIsOn);

                snprintf(line0, sizeof(line0), "%04d.%02d.%02d",
                        clockEvent.currentTime.year, clockEvent.currentTime.month, clockEvent.currentTime.day);
                snprintf(line1, sizeof(line1), "%02d:%02d:%02d",
                        clockEvent.currentTime.hour, clockEvent.currentTime.minute, clockEvent.currentTime.second);

                snprintf(line3, sizeof(line3), "%02d sec at %02d:%02d %s", 
                        seconds, alarmTime.hour, alarmTime.minute, 
                        enabled ? "On" : "Off");
                
                // Draw the bell symbol at the start of the line
                lcd->PrintCustomCharacter(3, 0, enabled && alarmIsOn ? 0x00 : 0x01);

                lcd->ShowText(0, 0, line0);
                lcd->ShowText(0, 11, line1);
                // lcd->ShowText(2, 2, line2);
                lcd->ShowText(3, 1, line3);
            }
        }
    }
}

int main() {
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);


    HD44780 lcd(0x27); // This address is common for many I2C LCDs, but it may vary.
    lcd.Init();
    lcd.Clear();

    Display display(&lcd);
    display.Start();

    static RotaryEncoder encoder(14, 15, 13);
    encoder.Init();

    PiezoSound sound(8);

    GPIOControl gpio(PICO_DEFAULT_LED_PIN, 6);

    // Create Alarm instance
    static Alarm alarm(4);
    alarm.SetAlarmTime({0, 0, 0, 12, 0, 0});  // Alarm at 12:00
    alarm.SetAlarmLength(10); // Alarm will ring for 10 seconds
    alarm.SetAlarmDuty(true); // Enable the alarm

    // Create Clock instance
    static Clock clock(4); // static so it persists

    // Optional: initialize time and alarm
    clock.SetCurrentTime({2025, 6, 19, 11, 59, 55});

    static MenuController menu(&clock, &alarm, &display);

    static UiTaskContext uiCtx = {
        .encoderQueue = encoder.GetEventQueue(),
        .display = &display,
        .menu = &menu
    };

    AlarmTaskContext alarmCtx = {
        .sound = sound,
        .gpio = gpio,
        .alarm = &alarm,
    };

    static ClockTaskContext clockCtx = {
        .queue = clock.GetEventQueue(),
        .gpio = gpio,
        .menu = &menu,
        .display = &display,
        .alarm = &alarm,
        .clock = &clock
    };

    // Start Encoder task
    xTaskCreate(UserInterfaceTask, "UserInterface", 512, &uiCtx, 1, nullptr);

    // Start Clock UI display task
    xTaskCreate(ClockDisplayTask, "ClockDisplay", 1024, &clockCtx, 1, nullptr);

    // Start Alarm task
    xTaskCreate(AlarmTask, "AlarmTask", 512, &alarmCtx, 1, nullptr);

    // Start the Clock
    clock.Start();

    /* Start the tasks and timer running. */
    vTaskStartScheduler();
}
