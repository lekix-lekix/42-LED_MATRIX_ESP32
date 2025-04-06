#include "rmt_module.h"
#include "conway.h"

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

void app_main()
{
    // Connecter la bonne pin au module RMT
    REG_WRITE(GPIO_FUNC0_OUT_SEL_CFG_REG, RMT_SIG_OUT0_IDX);
    // Activer la pin en output
    REG_WRITE(GPIO_ENABLE_REG, 1 << 18);
    
    t_rmt module;
    setup_rmt_module(18, &module);

    launch_conway_simulation(&module);

    while (1) {};
}