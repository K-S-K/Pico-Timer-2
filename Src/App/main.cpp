#include "FreeRTOS.h"
#include "pico/stdlib.h"
#include <task.h>
#include <cstdio>

#include "../Clock/Clock.hpp"
#include "../Clock/Alarm.hpp"
#include "../Clock/Relay.hpp"
#include "../Drivers/HD44780.hpp"
#include "../Display/Display.hpp"
#include "../Display/IDisplay.hpp"
#include "../Drivers/PiezoSound.hpp"
#include "../Drivers/GPIOControl.hpp"
#include "../Drivers/SystemThermo.hpp"
#include "../Drivers/RotaryEncoder.hpp"
#include "../UserInterface/MainScreen.hpp"
#include "../UserInterface/MenuLogic/MenuEvent.h"
#include "../UserInterface/MenuLogic/MenuController.hpp"


struct UiTaskContext {
    QueueHandle_t queue;
    MenuController* menu;
};

struct ClockTaskContext {
    QueueHandle_t queue;
    GPIOControl gpio;
    MainScreen* mainScreen;
    MenuController* menu;
    SystemThermo* thermo;
    Relay* relay;
    Alarm* alarm;
};

struct AlarmTaskContext {
    QueueHandle_t queue;
    MainScreen mainScreen;
    PiezoSound sound;
    GPIOControl gpio;
};

struct RelayTaskContext {
    QueueHandle_t queue;
    MainScreen mainScreen;
    PiezoSound sound;
    GPIOControl gpio;
};

static void UserInterfaceTask(void *param) {
    UiTaskContext* ctx = static_cast<UiTaskContext*>(param);
    QueueHandle_t queue = ctx->queue;
    MenuController* menu = ctx->menu;

    EncoderEvent clockEvent;

    while (true) {
        if (xQueueReceive(queue, &clockEvent, portMAX_DELAY)) {

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
    AlarmTaskContext* ctx = static_cast<AlarmTaskContext*>(param);
    MainScreen* mainScreen = &ctx->mainScreen;
    GPIOControl* gpio = &ctx->gpio;
    PiezoSound* sound = &ctx->sound;
    QueueHandle_t queue = ctx->queue;

    while (true) {
        AlarmEvent alarmEvent;
        if (xQueueReceive(queue, &alarmEvent, portMAX_DELAY)) {
            switch (alarmEvent.type) {
                case AlarmEventType::AlarmOn:
                    gpio->AlarmOn();
                    mainScreen->SetAlarmState(alarmEvent.state, false);  // Later we can add a render flag
                    // sound->PlayAlarmStart(); // Play alarm sound
                    // sound->PlayHourlyCuckoo(); // Play hourly cuckoo sound
                    // sound->PlaySweep(); // Play a sweep sound
                    sound->PlayMenuBeep(); // Play a menu beep sound
                    // sound->PlayHatikvah(); // Play Hatikvah melody
                    break;

                case AlarmEventType::AlarmOff:
                    gpio->AlarmOff();
                    mainScreen->SetAlarmState(alarmEvent.state, false);  // Later we can add a render flag
                    break;

                case AlarmEventType::Reconfigured:
                    // Handle reconfiguration if needed
                    mainScreen->SetAlarmConfig(alarmEvent.config, false); // Update the alarm config
                    break;
            }
        }
    }
}

void RelayTask(void* param) {
    RelayTaskContext* ctx = static_cast<RelayTaskContext*>(param);
    MainScreen* mainScreen = &ctx->mainScreen;
    GPIOControl* gpio = &ctx->gpio;
    PiezoSound* sound = &ctx->sound;
    QueueHandle_t queue = ctx->queue;

    while (true) {
        RelayEvent relayEvent;
        if (xQueueReceive(queue, &relayEvent, portMAX_DELAY)) {
            switch (relayEvent.type) {
                case RelayEventType::RelayOn:
                    gpio->RelayOn();
                    mainScreen->SetRelayState(relayEvent.state, false);  // Later we can add a render flag
                    sound->PlayMenuBeep(); // Play a menu beep sound
                    break;

                case RelayEventType::RelayOff:
                    gpio->RelayOff();
                    mainScreen->SetRelayState(relayEvent.state, false);  // Later we can add a render flag
                    sound->PlayMenuBeep(); // Play a menu beep sound
                    break;

                case RelayEventType::Reconfigured:
                    // Handle reconfiguration if needed
                    mainScreen->SetRelayConfig(relayEvent.config, false); // Update the relay config
                    break;
            }
        }
    }
}

void ClockDisplayTask(void* param) {
    ClockTaskContext* ctx = static_cast<ClockTaskContext*>(param);
    GPIOControl* gpio = &ctx->gpio;
    QueueHandle_t queue = ctx->queue;
    MainScreen* mainScreen = ctx->mainScreen;
    MenuController* menu = ctx->menu;
    SystemThermo* thermo = ctx->thermo;
    Relay* relay = ctx->relay;
    Alarm* alarm = ctx->alarm;

    while (true) {
        ClockEvent clockEvent;
        if (xQueueReceive(queue, &clockEvent, portMAX_DELAY)) {

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
                relay->ProcessCurrentTime(clockEvent.currentTime);

                mainScreen->SetClockTime(clockEvent.currentTime, false);
                mainScreen->SetTemperature(thermo->GetLastReadenTemperature(), false);

                mainScreen->Render();
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
    MainScreen mainScreen(&display);

    RotaryEncoder encoder(14, 15, 13);
    encoder.Init();

    PiezoSound sound(8);

    // LED pin, Alarm control pin, Relay control pin
    GPIOControl gpio(PICO_DEFAULT_LED_PIN, 6, 9);

    // Create Alarm instance
    Alarm alarm(4);
    {
        AlarmConfig alarmConfig;
        alarm.GetAlarmConfig(alarmConfig);
        
        // Set default alarm time and duration
        alarmConfig.timeBeg = {0, 0, 0, 12, 0, 0}; // Set default alarm time
        alarmConfig.duration = 10; // Set default alarm length
        alarmConfig.enabled = true; // Enable the alarm by default
        alarm.SetAlarmConfig(alarmConfig);
        mainScreen.SetAlarmConfig(alarmConfig, false);
    }

    // Create Relay instance
    Relay relay(4);
    {
        RelayConfig relayConfig;
        relay.GetRelayConfig(relayConfig);

        // Set default relay times
        relayConfig.timeBeg = {2025, 1, 1, 12, 0, 0}; // Start at 12:00
        relayConfig.timeEnd = {2025, 1, 1, 12, 1, 0}; // End at 12:01
        relayConfig.enabled = true; // Enable the relay by default
        relay.SetRelayConfig(relayConfig);
        mainScreen.SetRelayConfig(relayConfig, false);
    }

    // Create Clock instance
    Clock clock(4); // static so it persists

    // Optional: initialize time and alarm
    clock.SetCurrentTime({2025, 6, 19, 11, 59, 55});

    SystemThermo thermo(0.01f, 2000, 4);
    thermo.Start(); // Start the temperature reading task

    MenuController menu(&clock, &alarm, &relay, &display);

    static UiTaskContext uiCtx = {
        .queue = encoder.GetEventQueue(),
        .menu = &menu
    };

    AlarmTaskContext alarmCtx = {
        .queue = alarm.GetEventQueue(),
        .mainScreen = mainScreen,
        .sound = sound,
        .gpio = gpio,
    };

    RelayTaskContext relayCtx = {
        .queue = relay.GetEventQueue(),
        .mainScreen = mainScreen,
        .sound = sound,
        .gpio = gpio,
    };

    static ClockTaskContext clockCtx = {
        .queue = clock.GetEventQueue(),
        .gpio = gpio,
        .mainScreen = &mainScreen,
        .menu = &menu,
        .thermo = &thermo,
        .relay = &relay,
        .alarm = &alarm,
    };

    // Start Encoder task
    xTaskCreate(UserInterfaceTask, "UserInterface", 512, &uiCtx, 1, nullptr);

    // Start Clock UI display task
    xTaskCreate(ClockDisplayTask, "ClockDisplay", 1024, &clockCtx, 1, nullptr);

    // Start Alarm task
    xTaskCreate(AlarmTask, "AlarmTask", 512, &alarmCtx, 1, nullptr);

    // Start Relay task
    xTaskCreate(RelayTask, "RelayTask", 512, &relayCtx, 1, nullptr);

    // Start the Clock
    clock.Start();

    /* Start the tasks and timer running. */
    vTaskStartScheduler();
}
