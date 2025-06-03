#include "rmt_module.h"
#include "driver/gpio.h"
#include "conway.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "esp_timer.h"
#include "../include/include.h"

#define SENSOR_TRIG 5
#define SENSOR_READ 18
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

int32_t read_distance_ms()
{
    int64_t start_time = 0;
    int64_t end_time = 0;
    int64_t echo_start = 0;

    // Envoi du trigger (10 µs HIGH)
    gpio_set_level(SENSOR_TRIG, 1);
    esp_rom_delay_us(10);
    gpio_set_level(SENSOR_TRIG, 0);

    start_time = esp_timer_get_time();
    while (gpio_get_level(SENSOR_READ) == 0) 
    {
        if ((esp_timer_get_time() - start_time) > 10000) // 30000 == 5us
            return (-1);
    }
    echo_start = esp_timer_get_time();
    while (gpio_get_level(SENSOR_READ) == 1) 
    {
        if ((esp_timer_get_time() - echo_start) > 10000) // 30000 == 5us
            return (-1);
    }
    end_time = esp_timer_get_time();

    int32_t distance = (end_time - start_time) * 0.0343;
    return (distance);
    // Conversion en cm (vitesse du son = 343 m/s = 0.0343 cm/µs)
    // float distance = (duration * 0.0343) / 2;
    // return distance;
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

// void app_main()
// {
//     // Connecter la bonne pin au module RMT
//     // REG_WRITE(GPIO_FUNC0_OUT_SEL_CFG_REG, RMT_SIG_OUT0_IDX);
//     // Activer la pin en output
//     // REG_WRITE(GPIO_ENABLE_REG, 1 << 18);
    
//     // t_rmt module;
//     // setup_rmt_module(18, &module);

//     // launch_conway_simulation(&module);
    
//     uart_init();
//     setup_sensor();

//     int distance_ms;
//     int64_t start_time;
//     int64_t end_time;
//     char    str[10];

//     while (1) {
//         start_time = esp_timer_get_time();
//         distance_ms = read_distance_ms();
//         // printf("distance measured esp = %lld\n", distance_ms);
//         end_time = esp_timer_get_time();
//         // printf("tmps ecoule : %lld\n", end_time - start_time);
//         itoa(distance_ms, str, 10);
//         // printf("final str = %s\n", str);
//         uart_write_bytes(UART_NUM_0, str, 10);
//         for (int i = 0; i < 10; i++)
//             str[i] = '\0';
//         vTaskDelay(pdMS_TO_TICKS(29));
//     }
// }

void app_main()
{
        // Connecter la bonne pin au module RMT
    REG_WRITE(GPIO_FUNC0_OUT_SEL_CFG_REG, RMT_SIG_OUT0_IDX);
    // Activer la pin en output
    REG_WRITE(GPIO_ENABLE_REG, 1 << 18);

    t_rmt module;
    setup_rmt_module(18, &module);
    radial_loop(&module);
    // uint8_t led_data[300 * 3];
    // int      led_i;
    // for (int i = 0; i < 300; i++)
    // {
    //     led_data[i * 3 + 0] = 0xDC;
    //     led_data[i * 3 + 1] = 0xDC;
    //     led_data[i * 3 + 2] = 0xC8;
    // }
    // ESP_ERROR_CHECK(rmt_transmit(module.tx_channel, module.encoder, led_data, sizeof(led_data), &module.tx_config));
    // ESP_ERROR_CHECK(rmt_tx_wait_all_done(module.tx_channel, -1));
    // while (1) {}
}