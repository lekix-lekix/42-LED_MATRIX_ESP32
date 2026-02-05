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

void setup_sound_sensor()
{
    gpio_config_t sensor_trig_config = {
        .intr_type = GPIO_INTR_DISABLE,                  // pas d'interruption pour l’instant
        .pin_bit_mask = (1ULL << SOUND_SENSOR),          // sélection du GPIO
        .mode = GPIO_MODE_INPUT,                         // 🔴 mode INPUT (important)
        .pull_down_en = GPIO_PULLDOWN_DISABLE,           // selon câblage
        .pull_up_en = GPIO_PULLUP_DISABLE                // selon câblage
    };
    gpio_config(&sensor_trig_config);
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
    REG_WRITE(GPIO_ENABLE1_REG, 1 << 0);

    t_rmt module;
    setup_rmt_module(32, &module);  
    uart_init();
    setup_sensor(SENSOR_READ_1, SENSOR_TRIG_1);
    setup_sensor(SENSOR_READ_2, SENSOR_TRIG_2);
    setup_sound_sensor();
    gpio_install_isr_service(0);
    setup_echo_1_interrupt();
    setup_echo_2_interrupt();
    setup_sound_interrupt();
    start_radial(&module);
}