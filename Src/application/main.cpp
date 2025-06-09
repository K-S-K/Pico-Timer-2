#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"

void vBlinkTask(void *you_need_this)
{
  for (;;)
  {

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

  /* Start the tasks and timer running. */
  vTaskStartScheduler();
}
