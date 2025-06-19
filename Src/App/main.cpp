#include "FreeRTOS.h"
#include "pico/stdlib.h"
#include <task.h>
#include <cstdio>

#include "../Clock/Clock.hpp"
#include "../Drivers/HD44780.hpp"
#include "../Drivers/RotaryEncoder.hpp"


struct UiTaskContext {
    QueueHandle_t encoderQueue;
    HD44780* lcd;
};

struct ClockTaskContext {
    QueueHandle_t clockQueue;
    HD44780* lcd;
};

static void UserInterfaceTask(void *param) {
    UiTaskContext* uiCtx = static_cast<UiTaskContext*>(param);
    QueueHandle_t q = uiCtx->encoderQueue;
    HD44780* lcd = uiCtx->lcd;

    EncoderEvent evt;
    bool flag = false;
    int counter = 0;

    char buffer[32];

    lcd->clear();
    lcd->setCursor(0, 1);
    lcd->print("Conut: 0  Flag: OFF");

    while (true) {
        if (xQueueReceive(q, &evt, portMAX_DELAY)) {
            switch (evt.type) {
                case EncoderEventType::RotatedR:
                    counter++;
                    break;
                case EncoderEventType::RotatedL:
                    counter--;
                    break;
                case EncoderEventType::Pressed:
                    flag = !flag;
                    break;
            }

            snprintf(buffer, sizeof(buffer), "Conut: %2d Flag: %3s", counter, flag ? "ON" : "OFF");
            lcd->clear();
            lcd->setCursor(0, 1);
            lcd->print(buffer);

            printf("%s\n", buffer); // Optional debug
        }
    }
}

void ClockDisplayTask(void* param) {
    ClockTaskContext* uiCtx = static_cast<ClockTaskContext*>(param);
    QueueHandle_t q = uiCtx->clockQueue;
    HD44780* lcd = uiCtx->lcd;

    char line1[32];
    char line2[32];
    bool alarmIsOn = false;

    while (true) {
        ClockEvent evt;
        if (xQueueReceive(q, &evt, portMAX_DELAY)) {
            switch (evt.type) {
                case ClockEventType::Tick:
                    snprintf(line1, sizeof(line1), "%04d-%02d-%02d %02d:%02d:%02d",
                             evt.currentTime.year, evt.currentTime.month, evt.currentTime.day,
                             evt.currentTime.hour, evt.currentTime.minute, evt.currentTime.second);
                    lcd->setCursor(0, 2);
                    lcd->print(line1);

                    if (alarmIsOn) {
                        snprintf(line2, sizeof(line2), "Alarm Bell is ON");
                    } else {
                        snprintf(line2, sizeof(line2), "Alarm Bell is OFF");
                    }
                    lcd->setCursor(0, 3);
                    lcd->print(line2);
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
  lcd.init();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.setBacklight(true);
  lcd.print("Hello from Pico");

  // xTaskCreate(lcd_task, "LCD", 256, nullptr, 1, nullptr);

  static RotaryEncoder encoder(14, 15, 13);
  encoder.Init();

  // Create Clock instance
  static Clock clock(4); // static so it persists

  // Optional: initialize time and alarm
  clock.SetCurrentTime({2025, 6, 19, 11, 59, 50});
  clock.SetAlarmTime({0, 0, 0, 12, 0, 0});  // Alarm at 12:00
  clock.SetAlarmDuty(true);


  static UiTaskContext uiCtx = {
    .encoderQueue = encoder.GetEventQueue(),
    .lcd = &lcd
  };

  static ClockTaskContext clockCtx = {
    .clockQueue = clock.GetEventQueue(),
    .lcd = &lcd
  };

  xTaskCreate(UserInterfaceTask, "UserInterface", 512, &uiCtx, 1, nullptr);

  // Start Clock UI display task
  xTaskCreate(ClockDisplayTask, "ClockDisplay", 1024, &clockCtx, 1, nullptr);

  clock.Start();

  /* Start the tasks and timer running. */
  vTaskStartScheduler();
}
