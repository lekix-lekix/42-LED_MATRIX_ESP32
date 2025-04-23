#include "rmt_module.h"
#include "driver/gpio.h"
#include "conway.h"
#include "esp_timer.h"
#include "esp_log.h"

#define SENSOR_TRIG 5
#define SENSOR_READ 18

void setup_rmt_module(int gpio, t_rmt *rmt)
{
    rmt_tx_channel_config_t tx_chan_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .gpio_num = gpio,
        .mem_block_symbols = 64,
        .resolution_hz =  40 * 1000 * 1000,
        .trans_queue_depth = 1,
    };
    ESP_ERROR_CHECK(rmt_new_tx_channel(&tx_chan_config, &rmt->tx_channel));
    ESP_ERROR_CHECK(rmt_enable(rmt->tx_channel));

    rmt->encoder = NULL;
    rmt->config.bit0.level0 = 1;
    rmt->config.bit0.duration0 = 14;
    rmt->config.bit0.level1 = 0;
    rmt->config.bit0.duration1 = 32;
    rmt->config.bit1.level0 = 1;
    rmt->config.bit1.duration0 = 28;
    rmt->config.bit1.level1 = 0;
    rmt->config.bit1.duration1 = 24;
    rmt->config.flags.msb_first = true;

    rmt->tx_config.loop_count = 0;

    ESP_ERROR_CHECK(rmt_new_bytes_encoder(&rmt->config, &rmt->encoder));
}

void setup_sensor()
{
    gpio_config_t pin_5;
    gpio_config_t pin_18;

    pin_5.intr_type = GPIO_INTR_DISABLE;
    pin_5.pin_bit_mask = (1ULL << 5);
    pin_5.mode = GPIO_MODE_OUTPUT;
    pin_5.pull_down_en = GPIO_PULLDOWN_DISABLE;
    pin_5.pull_up_en = GPIO_PULLUP_DISABLE;

    pin_18.intr_type = GPIO_INTR_DISABLE;
    pin_18.pin_bit_mask = (1ULL << 18);
    pin_18.mode = GPIO_MODE_INPUT;
    pin_18.pull_down_en = GPIO_PULLDOWN_DISABLE;
    pin_18.pull_up_en = GPIO_PULLUP_DISABLE;

    gpio_config(&pin_5);
    gpio_config(&pin_18);
}

float read_distance_cm()
{
    int64_t start_time = 0;
    int64_t end_time = 0;

    // Envoi du trigger (10 µs HIGH)
    gpio_set_level(SENSOR_TRIG, 1);
    esp_rom_delay_us(10);
    gpio_set_level(SENSOR_TRIG, 0);

    // Attente que ECHO passe à 1
    while (gpio_get_level(SENSOR_READ) == 0) {
        start_time = esp_timer_get_time();
    }

    // Attente que ECHO retombe à 0
    while (gpio_get_level(SENSOR_READ) == 1) {
        end_time = esp_timer_get_time();
    }

    // Durée en microsecondes
    int64_t duration = end_time - start_time;

    // Conversion en cm (vitesse du son = 343 m/s = 0.0343 cm/µs)
    float distance = (duration * 0.0343) / 2;

    return distance;
}

void app_main()
{
    // Connecter la bonne pin au module RMT
    // REG_WRITE(GPIO_FUNC0_OUT_SEL_CFG_REG, RMT_SIG_OUT0_IDX);
    // Activer la pin en output
    // REG_WRITE(GPIO_ENABLE_REG, 1 << 18);
    
    // t_rmt module;
    // setup_rmt_module(18, &module);

    // launch_conway_simulation(&module);
    
    setup_sensor();

    while (1) {
        float distance = read_distance_cm();
        // printf("Distance mesurée : %.2f cm\n", distance);
        printf("%f\n", distance);
        vTaskDelay(1);
    }
}