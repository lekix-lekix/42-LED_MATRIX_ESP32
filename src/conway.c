#include "rmt_module.h"
#include "conway.h"

int check_neighbours(uint8_t tab[15][20], int x_tab, int y_tab)
{
    int x;
    int y;
    int alive = 0;

    for (int i = -1; i <= 1; i++)
    {
        y = (y_tab + i + G_HEIGHT) % G_HEIGHT;
        for (int j = -1; j <= 1; j++)
        {
            x = (x_tab + j + G_WIDTH) % G_WIDTH;
            if (tab[y][x] && !(x == x_tab && y == y_tab))
                alive++;
        }
    }
    return (alive);
}

void check_if_alive(uint8_t tab[15][20], uint8_t next_tab[15][20], int x_tab, int y_tab)
{
    int alive = 0;

    alive = check_neighbours(tab, x_tab, y_tab);
    if (tab[y_tab][x_tab] && (alive < 2 || alive > 3))
        next_tab[y_tab][x_tab] = 0;
    else if (tab[y_tab][x_tab] && (alive == 2 || alive == 3)) 
        next_tab[y_tab][x_tab] = 1;
    else if (!tab[y_tab][x_tab] && alive == 3)
        next_tab[y_tab][x_tab] = 1;
}

void copy_tab(__uint8_t tab[15][20], __uint8_t next_tab[15][20])
{
    for (int y = 0; y < G_HEIGHT; y++)
    {
        for (int x = 0; x < G_WIDTH; x++)
            tab[y][x] = next_tab[y][x];
    }
}

void conway(uint8_t tab[15][20])
{
    uint8_t next_tab[15][20] = {0};
    
    for (int y = 0; y < G_HEIGHT; y++)
    {
        for (int x = 0; x < G_WIDTH; x++)
            check_if_alive(tab, next_tab, x, y);
    }
    copy_tab(tab, next_tab);
}

int launch_conway_simulation(t_rmt *module)
{
    uint8_t conway_tab[15][20] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,0},
                                  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,0},
                                  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,0},
                                  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,0},
                                  {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 ,0},
                                  {0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0 ,0},
                                  {0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 ,0},
                                  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,0},
                                  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0 ,0},
                                  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0 ,0},
                                  {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0 ,0},
                                  {0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,0},
                                  {0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,0},
                                  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,0},
                                  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,0},};
    uint8_t led_data[300 * 3];
    int led_i;

    while (1)
    {
        conway(conway_tab);
        led_i = 0;
        for (int i = 0; i < G_HEIGHT; i++)
        {
            for (int j = 0; j < G_WIDTH; j++)
            {
                if (conway_tab[i][j])
                {
                    led_data[led_i * 3 + 0] = 0xFF; // g
                    led_data[led_i * 3 + 1] = 0x00; // b
                    led_data[led_i * 3 + 2] = 0x00; // r
                }
                else
                {
                    led_data[led_i * 3 + 0] = 0x00;
                    led_data[led_i * 3 + 1] = 0x00;
                    led_data[led_i * 3 + 2] = 0xFF;
                }
                led_i++;
            }
        }
        ESP_ERROR_CHECK(rmt_transmit(module->tx_channel, module->encoder, led_data, sizeof(led_data), &module->tx_config));
        ESP_ERROR_CHECK(rmt_tx_wait_all_done(module->tx_channel, -1));
    }
    return (0);
}