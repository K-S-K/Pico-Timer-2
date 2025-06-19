#include "FreeRTOS.h"
#include "pico/stdlib.h"
#include <task.h>
#include <cstdio>

#include "../Drivers/HD44780.hpp"
#include "../Drivers/RotaryEncoder.hpp"


struct UiTaskContext {
    QueueHandle_t encoderQueue;
    HD44780* lcd;
};

static void UserInterfaceTask(void *param) {
    UiTaskContext* ctx = static_cast<UiTaskContext*>(param);
    QueueHandle_t q = ctx->encoderQueue;
    HD44780* lcd = ctx->lcd;

    EncoderEvent evt;
    bool flag = false;
    int counter = 0;

    char buffer[32];

    lcd->clear();
    lcd->setCursor(0, 0);
    lcd->print("Cnt: 0  Flag: OFF");

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

            snprintf(buffer, sizeof(buffer), "Cnt: %d  Flag: %s", counter, flag ? "ON " : "OFF");
            lcd->clear();
            lcd->setCursor(0, 1);
            lcd->print(buffer);

            printf("%s\n", buffer); // Optional debug
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

  static UiTaskContext ctx = {
    .encoderQueue = encoder.GetEventQueue(),
    .lcd = &lcd
  };

  // xTaskCreate(EncoderEventHandlerTask, "EncoderHandler", 512, (void*)encoder.GetEventQueue(), 1, nullptr);

  xTaskCreate(UserInterfaceTask, "UserInterface", 512, &ctx, 1, nullptr);

  /* Start the tasks and timer running. */
  vTaskStartScheduler();
}
