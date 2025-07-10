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
    QueueHandle_t encoderQueue;
    IDisplay* display;
    MenuController* menu;
};

struct ClockTaskContext {
    QueueHandle_t queue;
    GPIOControl gpio;
    MainScreen* mainScreen;
    MenuController* menu;
    SystemThermo* thermo;
    IDisplay* display;
    Relay* relay;
    Alarm* alarm;
    Clock* clock;
};

struct AlarmTaskContext {
    MainScreen mainScreen;
    PiezoSound sound;
    GPIOControl gpio;
    Alarm* alarm;
};

struct RelayTaskContext {
    MainScreen mainScreen;
    PiezoSound sound;
    GPIOControl gpio;
    Relay* relay;
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
    MainScreen* mainScreen = &alarmCtx->mainScreen;
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
            }
        }
    }
}

void RelayTask(void* param) {
    RelayTaskContext* relayCtx = static_cast<RelayTaskContext*>(param);
    MainScreen* mainScreen = &relayCtx->mainScreen;
    GPIOControl* gpio = &relayCtx->gpio;
    Relay* relay = relayCtx->relay;
    PiezoSound* sound = &relayCtx->sound;
    QueueHandle_t q = relay->GetEventQueue();

    while (true) {
        RelayEvent relayEvent;
        if (xQueueReceive(q, &relayEvent, portMAX_DELAY)) {
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
            }
        }
    }
}

void ClockDisplayTask(void* param) {
    ClockTaskContext* uiCtx = static_cast<ClockTaskContext*>(param);
    GPIOControl* gpio = &uiCtx->gpio;
    QueueHandle_t q = uiCtx->queue;
    MainScreen* mainScreen = uiCtx->mainScreen;
    MenuController* menu = uiCtx->menu;
    SystemThermo* thermo = uiCtx->thermo;
    IDisplay* lcd = uiCtx->display;
    Relay* relay = uiCtx->relay;
    Alarm* alarm = uiCtx->alarm;
    Clock* clock = uiCtx->clock;

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
                relay->ProcessCurrentTime(clockEvent.currentTime);

                AlarmConfig alarmConfig;
                alarm->GetAlarmConfig(alarmConfig);

                RelayConfig relayConfig;
                relay->GetRelayConfig(relayConfig);

                mainScreen->SetAlarmConfig(alarmConfig, false);
                mainScreen->SetRelayConfig(relayConfig, false);
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

    static RotaryEncoder encoder(14, 15, 13);
    encoder.Init();

    PiezoSound sound(8);

    // LED pin, Alarm control pin, Relay control pin
    GPIOControl gpio(PICO_DEFAULT_LED_PIN, 6, 9);

    // Create Alarm instance
    static Alarm alarm(4);
    {
        AlarmConfig alarmConfig;
        alarm.GetAlarmConfig(alarmConfig);
        
        // Set default alarm time and duration
        alarmConfig.timeBeg = {0, 0, 0, 12, 0, 0}; // Set default alarm time
        alarmConfig.duration = 10; // Set default alarm length
        alarmConfig.enabled = true; // Enable the alarm by default
        alarm.SetAlarmConfig(alarmConfig);
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
    }

    // Create Clock instance
    static Clock clock(4); // static so it persists

    // Optional: initialize time and alarm
    clock.SetCurrentTime({2025, 6, 19, 11, 59, 55});

    SystemThermo thermo(0.01f, 2000, 4);
    thermo.Start(); // Start the temperature reading task

    MainScreen mainScreen(&display);

    static MenuController menu(&clock, &alarm, &relay, &display);

    static UiTaskContext uiCtx = {
        .encoderQueue = encoder.GetEventQueue(),
        .display = &display,
        .menu = &menu
    };

    AlarmTaskContext alarmCtx = {
        .mainScreen = mainScreen,
        .sound = sound,
        .gpio = gpio,
        .alarm = &alarm,
    };

    RelayTaskContext relayCtx = {
        .mainScreen = mainScreen,
        .sound = sound,
        .gpio = gpio,
        .relay = &relay
    };

    static ClockTaskContext clockCtx = {
        .queue = clock.GetEventQueue(),
        .gpio = gpio,
        .mainScreen = &mainScreen,
        .menu = &menu,
        .thermo = &thermo,
        .display = &display,
        .relay = &relay,
        .alarm = &alarm,
        .clock = &clock
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
