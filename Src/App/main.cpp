#include "FreeRTOS.h"
#include "pico/stdlib.h"
#include <task.h>
#include <cstdio>

#include "../Clock/Clock.hpp"
#include "../Drivers/HD44780.hpp"
#include "../Display/Display.hpp"
#include "../Display/IDisplay.hpp"
#include "../Menu/MenuController.hpp"
#include "../Drivers/RotaryEncoder.hpp"


struct UiTaskContext {
    QueueHandle_t encoderQueue;
    IDisplay* display;
    MenuController* menu;
};

struct ClockTaskContext {
    QueueHandle_t clockQueue;
    IDisplay* display;
};

static void UserInterfaceTask(void *param) {
    UiTaskContext* uiCtx = static_cast<UiTaskContext*>(param);
    QueueHandle_t q = uiCtx->encoderQueue;
    MenuController* menu = uiCtx->menu;
    IDisplay* lcd = uiCtx->display;

    EncoderEvent userEvent;

    while (true) {
        if (xQueueReceive(q, &userEvent, portMAX_DELAY)) {

            // Convert EncoderEvent to MenuEvent
            MenuEvent menuEvt;
            switch (userEvent.type) {
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
    QueueHandle_t q = uiCtx->clockQueue;
    IDisplay* lcd = uiCtx->display;

    char line0[32];
    char line1[32];
    char line2[32];
    bool alarmIsOn = false;

    while (true) {
        ClockEvent userEvent;
        if (xQueueReceive(q, &userEvent, portMAX_DELAY)) {
            switch (userEvent.type) {
                case ClockEventType::Tick:
                    snprintf(line0, sizeof(line0), "%04d.%02d.%02d",
                             userEvent.currentTime.year, userEvent.currentTime.month, userEvent.currentTime.day);
                    snprintf(line1, sizeof(line1), "%02d:%02d:%02d",
                             userEvent.currentTime.hour, userEvent.currentTime.minute, userEvent.currentTime.second);
                    snprintf(line2, sizeof(line2), "Alarm Bell is %3s", alarmIsOn ? "ON" : "OFF");

                    // lcd->ShowText(3, 0, line0);
                    lcd->ShowText(3, 11, line1);
                    // lcd->ShowText(3, 0, line2);
                    break;

                case ClockEventType::AlarmOn:
                    alarmIsOn = true;
                    break;

                case ClockEventType::AlarmOff:
                    alarmIsOn = false;
                    break;
            }
        }
    }
}

void vBlinkTask(void *you_need_this) {
  for (;;) {

    gpio_put(PICO_DEFAULT_LED_PIN, 1);
    vTaskDelay(100);

    gpio_put(PICO_DEFAULT_LED_PIN, 0);
    vTaskDelay(100);

    gpio_put(PICO_DEFAULT_LED_PIN, 1);
    vTaskDelay(100);

    gpio_put(PICO_DEFAULT_LED_PIN, 0);
    vTaskDelay(700);
  }
}

int main() {
  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
  xTaskCreate(vBlinkTask, "Blink Task", 128, NULL, 1, NULL);


  HD44780 lcd(0x27); // This address is common for many I2C LCDs, but it may vary.
  lcd.Init();
  lcd.Clear();

  Display display(&lcd);
  display.Start();

  // display.ShowText(0, 0, "Hello, Pico Timer!");

  static RotaryEncoder encoder(14, 15, 13);
  encoder.Init();

  // Create Clock instance
  static Clock clock(4); // static so it persists

  // Optional: initialize time and alarm
  clock.SetCurrentTime({2025, 6, 19, 11, 59, 50});
  clock.SetAlarmTime({0, 0, 0, 12, 0, 0});  // Alarm at 12:00
  clock.SetAlarmDuty(true);

  static MenuController menu(&display);

  static UiTaskContext uiCtx = {
    .encoderQueue = encoder.GetEventQueue(),
    .display = &display,
    .menu = &menu
  };

  static ClockTaskContext clockCtx = {
    .clockQueue = clock.GetEventQueue(),
    .display = &display
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
