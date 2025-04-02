#include "soc/rmt_reg.h"      // Définitions des registres RMT
#include "soc/dport_reg.h"    // Gestion de l’horloge des périphériques
#include "soc/gpio_reg.h"     // Pour manipuler les registres GPIO
#include "soc/io_mux_reg.h"   // GPIO assignation
#include "soc/gpio_sig_map.h" // Mapping des signaux vers les GPIOs
#include "soc/rmt_struct.h"   // Structure des registres RMT
#include "soc/soc.h"
#include <stdio.h>

#define LED_PIN GPIO_NUM_2  // Modifier selon le GPIO utilisé

void activate_rmt_clock() 
{
    // Activer l'horloge du RMT sans affecter les autres périphériques
    DPORT_SET_PERI_REG_BITS(DPORT_PERIP_CLK_EN_REG, DPORT_RMT_CLK_EN, 1, 9);
    // Désactiver le reset du RMT (sinon il reste bloqué)
    DPORT_SET_PERI_REG_BITS(DPORT_PERIP_RST_EN_REG, DPORT_RMT_RST, 0, 9);
    // Connecter la bonne pin au module RMT
    REG_WRITE(GPIO_FUNC0_OUT_SEL_CFG_REG, RMT_SIG_OUT0_IDX);
    // Activer la pin en output
    REG_WRITE(GPIO_ENABLE_REG, 1 << 18);
    // Prescaler a 2 : 1 tick = 25ns
    REG_SET_FIELD(RMT_CH0CONF0_REG, RMT_DIV_CNT_CH0, 2);
}

void send_0_WS2812()
{
    while (REG_GET_BIT(RMT_INT_RAW_REG, RMT_CH0_TX_END_INT_RAW));
    RMT.data_ch[0] = (34 << 16) | (1 << 15) | (16 << 8) | 1;
    REG_SET_BIT(RMT_CH0CONF0_REG, RMT_MEM_WR_RST_CH0);
    REG_SET_BIT(RMT_CH0CONF1_REG, RMT_TX_START_CH0);
}

void send_1_WS2812()
{
    printf("coucou\n");
    // while (REG_GET_BIT(RMT_INT_RAW_REG, RMT_CH0_TX_END_INT_RAW)) {}
    // REG_SET_BIT(RMT_CH0CONF0_REG, RMT_MEM_WR_RST_CH0);
    REG_SET_BIT(RMT_INT_CLR_REG, RMT_CH0_TX_END_INT_CLR);
    RMT.data_ch[0] = (18 << 16) | (1 << 15) | (32 << 8) | 1;
    REG_SET_BIT(RMT_CH0CONF1_REG, RMT_TX_START_CH0);
}

void send_rmt_data()
{
    for (int i = 0; i < 72; i++)
        send_1_WS2812();
    while (1) {}
}

void app_main() 
{

    // Affichage sur le terminal
    activate_rmt_clock();
    while (1) 
    {
        send_rmt_data();
    }
}