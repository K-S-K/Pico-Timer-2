#include "FreeRTOS.h"
#include "pico/stdlib.h"
#include <task.h>
#include <cstdio>

#include "../Clock/Clock.hpp"
#include "../Drivers/HD44780.hpp"
#include "../Display/Display.hpp"
#include "../Display/IDisplay.hpp"
#include "../Drivers/GPIOControl.hpp"
#include "../Drivers/RotaryEncoder.hpp"
#include "../UserInterface/Menu/MenuController.hpp"


struct UiTaskContext {
    QueueHandle_t encoderQueue;
    IDisplay* display;
    MenuController* menu;
};

struct ClockTaskContext {
    QueueHandle_t clockQueue;
    GPIOControl gpioControl;
    MenuController* menu;
    IDisplay* display;
    Clock* clock;
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

void ClockDisplayTask(void* param) {
    ClockTaskContext* uiCtx = static_cast<ClockTaskContext*>(param);
    GPIOControl* gpioControl = &uiCtx->gpioControl;
    QueueHandle_t q = uiCtx->clockQueue;
    MenuController* menu = uiCtx->menu;
    IDisplay* lcd = uiCtx->display;
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

            // Process the clock event
            switch (clockEvent.type) {
                case ClockEventType::Tick:
                    {
                        gpioControl->BlinkTickLed();
                        int seconds;
                        bool enabled;
                        DateTime alarmTime;
                        clock->GetAlarmDuty(enabled);
                        clock->GetAlarmLength(seconds);
                        clock->GetAlarmTime(alarmTime);

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
                    break;

                case ClockEventType::AlarmOn:
                    gpioControl->AlarmOn();
                    alarmIsOn = true;
                    break;

                case ClockEventType::AlarmOff:
                    gpioControl->AlarmOff();
                    alarmIsOn = false;
                    break;
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

  GPIOControl gpioControl(PICO_DEFAULT_LED_PIN, 6);

  static RotaryEncoder encoder(14, 15, 13);
  encoder.Init();

  // Create Clock instance
  static Clock clock(4); // static so it persists

  // Optional: initialize time and alarm
  clock.SetCurrentTime({2025, 6, 19, 11, 59, 50});
  clock.SetAlarmTime({0, 0, 0, 12, 0, 0});  // Alarm at 12:00
  clock.SetAlarmDuty(true);

  static MenuController menu(&clock, &display);

  static UiTaskContext uiCtx = {
    .encoderQueue = encoder.GetEventQueue(),
    .display = &display,
    .menu = &menu
  };

  static ClockTaskContext clockCtx = {
    .clockQueue = clock.GetEventQueue(),
    .gpioControl = gpioControl,
    .menu = &menu,
    .display = &display,
    .clock = &clock
  };

  // Start Encoder task
  xTaskCreate(UserInterfaceTask, "UserInterface", 512, &uiCtx, 1, nullptr);

  // Start Clock UI display task
  xTaskCreate(ClockDisplayTask, "ClockDisplay", 1024, &clockCtx, 1, nullptr);

  // Start the Clock
  clock.Start();

  /* Start the tasks and timer running. */
  vTaskStartScheduler();
}
