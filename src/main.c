#include "rmt_module.h"
#include "driver/gpio.h"
#include "conway.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "esp_timer.h"
#include "../include/include.h"

#define BUFF_SIZE 1024

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
    rmt->config.bit0.level1 = 0;
    rmt->config.bit1.level0 = 1;
    rmt->config.bit1.level1 = 0;

    rmt->config.bit0.duration0 = 14;
    rmt->config.bit0.duration1 = 32;
    rmt->config.bit1.duration0 = 28;
    rmt->config.bit1.duration1 = 24;

    rmt->config.flags.msb_first = true;

    rmt->tx_config.loop_count = 0;
    rmt->tx_config.flags.eot_level = 0;  // ligne basse à la fin

    ESP_ERROR_CHECK(rmt_new_bytes_encoder(&rmt->config, &rmt->encoder));
}

void setup_sensor(int sensor_read, int sensor_trig)
{
    gpio_config_t sensor_trig_config;
    gpio_config_t sensor_read_config;

    sensor_trig_config.intr_type = GPIO_INTR_DISABLE;
    sensor_trig_config.pin_bit_mask = (1ULL << sensor_trig);
    sensor_trig_config.mode = GPIO_MODE_OUTPUT;
    sensor_trig_config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    sensor_trig_config.pull_up_en = GPIO_PULLUP_DISABLE;

    sensor_read_config.intr_type = GPIO_INTR_DISABLE;
    sensor_read_config.pin_bit_mask = (1ULL << sensor_read);
    sensor_read_config.mode = GPIO_MODE_INPUT;
    sensor_read_config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    sensor_read_config.pull_up_en = GPIO_PULLUP_DISABLE;

    gpio_config(&sensor_trig_config);
    gpio_config(&sensor_read_config);
}

int32_t read_distance_ms(int sensor_read, int sensor_trig)
{
    int32_t distance = -1;
    // Envoi du trigger
    gpio_set_level(sensor_trig, 1);
    esp_rom_delay_us(10);
    gpio_set_level(sensor_trig, 0);

    // Attente front montant
    int64_t t0 = esp_timer_get_time();
    while (gpio_get_level(sensor_read) == 0) {
        // if (esp_timer_get_time() - t0 > 30000)
            // break;
    }
    int64_t start = esp_timer_get_time();

    // Attente front descendant
    while (gpio_get_level(sensor_read) == 1) {
        // if (esp_timer_get_time() - start > 30000)
            // break;
    }
    int64_t end = esp_timer_get_time();

    // if (distance != -1)
    distance = (end - start) * 0.0343f;
    // char str[10];
    // bzero(str, 10);
    // itoa(distance, str, 10);
    // // printf("str = %s\n", str);
    // uart_write_bytes(UART_NUM_0, str, 10);
    return (distance);
}

void uart_init()
{
    uart_config_t uart_config;

    uart_config.baud_rate = 115200;
    uart_config.data_bits = UART_DATA_8_BITS;
    uart_config.parity = UART_PARITY_DISABLE;
    uart_config.stop_bits = UART_STOP_BITS_1;
    uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;

    uart_param_config(UART_NUM_0, &uart_config);
    uart_driver_install(UART_NUM_0, BUFF_SIZE * 2, 0, 0, NULL, 0);
}

int ft_strlen(char *str)
{
    int i = 0;
    while (str[i])
        i++;
    return (i);
}

void app_main()
{
    // Connecter la bonne pin au module RMT
    REG_WRITE(GPIO_FUNC0_OUT_SEL_CFG_REG, RMT_SIG_OUT0_IDX);
    // Activer la pin en output
    REG_WRITE(GPIO_ENABLE_REG, 1 << 19);

    t_rmt module;
    setup_rmt_module(19, &module);  
    uart_init();
    setup_sensor(SENSOR_READ_1, SENSOR_TRIG_1);
    setup_sensor(SENSOR_READ_2, SENSOR_TRIG_2);
    start_radial(&module);
}