#ifndef INCLUDE_H
#define INCLUDE_H

#define W_WIDTH 800
#define W_HEIGHT 600
#define G_WIDTH 20
#define G_HEIGHT 15

# define GRID_X 40
# define GRID_Y 40

# define ALIVE_COLOR 0x00FF00
# define DEAD_COLOR 0x000000
# define GRID_COLOR 0x0000FF

// #include "../minilibx-linux/mlx.h"
#include <sys/time.h>
#include <sys/select.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include "rmt_module.h"
#include "driver/gpio.h"
#include "conway.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "esp_timer.h"
#include "esp_pthread.h"
#include "pthread.h"
#include "../include/rmt_module.h"
#include "../include/rmt_module.h"

// #include <fcntl.h>
// #include <termio.h>

#define UP_KEY      65362
#define DOWN_KEY    65364

// #define SERIAL_PORT "/dev/ttyUSB0"  // Change ceci en fonction de ton port série
#define BUFFER_SIZE 256

#define SENSOR_TRIG_1 5
#define SENSOR_READ_1 18
#define SENSOR_TRIG_2 16
#define SENSOR_READ_2 17

typedef struct timeval t_timeval;

// typedef struct s_mlx
// {
// 	void			*mlx_ptr;
// 	void			*win_ptr;
// 	int				width;
// 	int				height;
//     int             uart_fd;
// }					t_mlx;

// typedef struct s_img
// {
// 	void			*img_ptr;
// 	char			*img_addr;
// 	int				bpp;
// 	int				line_len;
// 	int				endian;
// }					t_img;

typedef struct cell
{
    int x;
    int y;
}                   t_cell;

typedef struct color
{
    int r;
    int g;
    int b;
}                   t_color;

typedef struct sensor_data_s
{
    pthread_mutex_t     *dist_lock;
    pthread_mutex_t     *interp_lock;
    float               sens_1_last_value;
    float               sens_1_next_value;
    float               sens_1_interp;
    int                 dist_sensor_1;
    int                 dist_sensor_2; 
    // t_mlx               *window;
    int                 uart_fd;
}   sensor_data_t;


/******* MLX ********/
// t_mlx	start_mlx(int width, int height);
// t_img	*init_img(t_mlx *window);
// int	    *get_pixel_from_img(t_img *img, int x, int y);
// void	img_pix_put(t_img *img, int x, int y, int color);
// void    draw_cell(t_img *img, int x, int y, int color);
// void    draw_grid(t_img *img, int color);
// void    push_img(t_img *img, t_mlx *window);

int    start_radial(t_rmt *module);
// void    linear_loop(t_mlx *window);
// int     conway_loop(t_mlx *param_window);

// int     wheel(__uint8_t pos);

/*********** COLORS ********** */
int	    create_trgb(int t, int r, int g, int b);
void	palette_one(int *colors);
void	palette_two(int *colors);
void	palette_three(int *colors);
void	palette_four(int *colors);
int	    interpolate_color(int color_a, int color_b, float t);
int	    get_color_gradient(float iter, int *colors, int nb_colors);
int	    get_r(int trgb);
int	    get_g(int trgb);
int	    get_b(int trgb);

float	normalize_value(float value, float min, float max);

/************* SENSOR ********* */
int32_t read_distance_ms(int sensor_read, int sensor_trig);
void get_sensor_1_values(sensor_data_t *data, float *curr_distance);
void get_sensor_2_values(sensor_data_t *data, float *curr_distance);
void update_average_distance(sensor_data_t *data);

/************* MATHS *************/
float   square(float nb);
float   get_norm_distance(t_cell *cell, t_cell *center, float max_distance);
float   clamp(float value, float min, float max);
float   lerp(float a, float b, float t);
float   norm_value(float value, float min, float max);

/************** */

#endif