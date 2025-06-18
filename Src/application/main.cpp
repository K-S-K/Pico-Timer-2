#include "FreeRTOS.h"
#include "pico/stdlib.h"
#include "task.h"

#include "HD44780.hpp"
#include <cstdio>

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

void lcd_task(void *param) {
  HD44780 lcd(0x27); // This address is common for many I2C LCDs, but it may vary.
  lcd.init();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Hello from Pico");

  lcd.setBacklight(true);

  while (true) {
    lcd.setCursor(0, 1);
    lcd.print("Tick: ");
    char buf[10];
    sprintf(buf, "%lu", xTaskGetTickCount());
    lcd.print(buf);

    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

int main() {
  gpio_init(PICO_DEFAULT_LED_PIN);

  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

  xTaskCreate(vBlinkTask, "Blink Task", 128, NULL, 1, NULL);

  xTaskCreate(lcd_task, "LCD", 256, nullptr, 1, nullptr);

  /* Start the tasks and timer running. */
  vTaskStartScheduler();
}
