#ifndef RMT_MODULE_H
# define RMT_MODULE_H

#include "soc/rmt_reg.h"      // Définitions des registres RMT
#include "soc/gpio_reg.h"     // Pour manipuler les registres GPIO
#include "soc/io_mux_reg.h"   // GPIO assignation
#include "soc/gpio_sig_map.h" // Mapping des signaux vers les GPIOs
#include "soc/rmt_struct.h"   // Structure des registres RMT
#include "soc/soc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/rmt_tx.h"
#include <stdio.h>

typedef struct s_rmt
{
    rmt_channel_handle_t tx_channel;
    rmt_encoder_handle_t encoder;
    rmt_bytes_encoder_config_t config;
    rmt_transmit_config_t tx_config;
}   t_rmt;

#endif