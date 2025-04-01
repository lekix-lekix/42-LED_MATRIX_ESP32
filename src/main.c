#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define LED_PIN GPIO_NUM_2  // Modifier selon le GPIO utilisé

void app_main() {
    // Configurer le GPIO comme une sortie
    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    while (1) {
        gpio_set_level(LED_PIN, 1); // Allumer la LED
        vTaskDelay(1000 / portTICK_PERIOD_MS); // Attendre 1s
        gpio_set_level(LED_PIN, 0); // Éteindre la LED
        vTaskDelay(1000 / portTICK_PERIOD_MS); // Attendre 1s
    }
}