#include "../include/include.h"
#include "../include/rmt_module.h"
#include "esp_timer.h"
#include "esp_pthread.h"
#include "pthread.h"

// int handle_keys(int key);

float square(float nb)
{
    return (nb * nb);
}

float get_norm_distance(t_cell *cell, t_cell *center, float max_distance)
{
    float distance_squared = square(cell->x - center->x) + square(cell->y - center->y);
    return (distance_squared / square(max_distance));
}

float branches = -2.0f;
float pixellization =  10; // 2.90f; // 1,
float spiral_speed = 0.01f;
float anim_speed = 0.01f; // 0.005f
int   color_mode = 0;
int   nb_colors = 0;
int   next_color = 2;
int   mode = 0;
int   active_palette[5];

int   transition_start_frame = 0;
int   restart_frame = 0;
int   animation_restart = 0;

int   leds[15][20];
int   frame = 0;
int   cell_state[G_HEIGHT][G_WIDTH] = {0};

float clamp(float value, float min, float max)
{
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

void reset_cell_state()
{
    for (int i = 0; i < G_WIDTH; i++)
    {
        for (int j = 0; j < G_HEIGHT; j++)
            cell_state[i][j] = 0;
    }
}

void radial_gradient()
{
    // t_img *img = init_img(window);
    t_cell cell;
    t_cell center = {9, 7};
    float max_distance = 12.5;
    int   colors[5];
    float color;

    // static transition_radius = 0;
    
    // int colors_nb = nb_colors;

    switch (color_mode)
    {
        case 0:
            palette_one(colors);
            break;

        case 1:
            palette_two(colors);
            break;

        case 2:
            palette_three(colors);
            break;

        case 3:
            palette_four(colors);
            break;

        default:
            break;
    }
    float form = -10.0f;         // -2, -200

    // float anim_relaunch_radius = clamp((frame - restart_frame) * 0.005f, 0.0f, 1.0f);

    for (int i = 0; i < G_WIDTH; i++)
    {
        for (int j = 0; j < G_HEIGHT; j++)
        {
            cell.x = i;
            cell.y = j;
            float angle = atan2(cell.y - center.y, cell.x - center.x);
            float distance = get_norm_distance(&cell, &center, max_distance);
            float r = distance * (form + (pixellization * sin(branches * angle)));
            float raw_color = (r / max_distance + (float)frame * anim_speed);
            color = fmodf(fabsf(raw_color), 1.0f);

            // if (transition_start_frame)
            // {
            //     float propagation = clamp((frame - transition_start_frame) * 0.005f, 0.0f, 1.0f);
                
            //     if (distance <= propagation)
            //         cell_state[i][j] = 1;

            //     if (cell_state[i][j] == 1)
            //         draw_cell(img, i, j, colors[next_color]);
            //     else
            //         draw_cell(img, i, j, get_color_gradient(color, colors, nb_colors));
            // }
            // else if (animation_restart)
            // {
            //     // printf("anim restart\n");
            //     if (distance > anim_relaunch_radius)
            //         draw_cell(img, i, j, colors[next_color - 1]);
            //     else
            //         draw_cell(img, i, j, get_color_gradient(color, colors, nb_colors));
            //         // Quand relance anim est finie, on désactive le mode "reprise"
            // if (animation_restart && anim_relaunch_radius >= 1.0f)
            // {
            //     printf("stop anim\n");
            //     animation_restart = 0;
            // }
            // }
            // else
            leds[j][i] = get_color_gradient(color, colors, 5);
            // draw_cell(img, i, j, get_color_gradient(color, colors, 5));
                        // printf("i = %d j = %d\n", i, j);
        }
    }
    // push_img(img, window);
    if (transition_start_frame)
    {
        int all_cells_converted = 1;
        for (int i = 0; i < G_WIDTH; i++)
        {
            for (int j = 0; j < G_HEIGHT; j++)
            {
                if (cell_state[i][j] == 0)
                {
                    all_cells_converted = 0;
                    break;
                }
            }
            if (!all_cells_converted)
                break;
        }
    
        if (all_cells_converted)
        {
            printf("All cells converted\n");
            reset_cell_state(); 
            nb_colors = (nb_colors + 1) % 5;     // ajoute la nouvelle couleur
            next_color = (next_color + 1) % 5;
            printf("nb_colors = %d\n", nb_colors);
            transition_start_frame = 0;
            restart_frame = frame;
            animation_restart = 1;
        }
    }
}

// void radial_gradient_round(t_mlx *window, int frame)
// {
//     t_img *img = init_img(window);
    
//     t_cell cell;
//     t_cell center = {9, 7};
//     float norm_distance;
//     float max_distance = 12.5;
//     float wave;
    
//     for (int i = 0; i < G_WIDTH; i++)
//     {
//         for (int j = 0; j < G_HEIGHT; j++)
//         {
//             cell.x = i;
//             cell.y = j;
//             norm_distance = get_norm_distance(&cell, &center, max_distance);
//             wave = (sin(norm_distance / 1.0f) - (frame / 500.0f) + 1) / 2;
//             draw_cell(img, i, j, wheel(wave * 255));
//         }
//     }
//     push_img(img, window);
// }

// int handle_keys(int key)
// {
//     if (key == 65307)
//         exit(0);
//     else if (key == UP_KEY && mode == 0)
//         branches += 0.1f;
//     else if (key == DOWN_KEY && mode == 0)
//         branches -= 0.1f;
//     else if (key == 119 && mode == 0)
//         pixellization += 0.1f;
//     else if (key == 115 && mode == 0)
//         pixellization -= 0.1f;
//     else if (key == UP_KEY && mode == 2)
//         spiral_speed += 0.01f;
//     else if (key == DOWN_KEY && mode == 2)
//         spiral_speed -= 0.01f;
//     else if (key == 65361)
//         mode = (mode + 1 ) % 3;
//     else if (key == 100)
//         color_mode = (color_mode + 1) % 5;
//     else if (key == 110)
//         transition_start_frame = frame;
//     else if (key == 107)
//         nb_colors = (nb_colors + 1) % 2;
//     else if (key == 61)
//         anim_speed += 0.0001;
//     else if (key == 45 && anim_speed > 0)
//         anim_speed -= 0.0001;
//     // printf("b = %f p = %f\n", branches, pixellization);
//     // printf("anim speed = %f\n", anim_speed);
//     // printf("color mode = %d\n", color_mode);
//     printf("key = %d\n",key);
//     // printf("form = %f\n", *form);
//     return 0;
// }

// Configuration du port série
// int configure_serial_port(int fd) {
//     struct termios options;

//     // Récupérer les paramètres actuels du port série
//     if (tcgetattr(fd, &options) < 0) {
//         perror("Erreur lors de la récupération des paramètres du port série");
//         return -1;
//     }

//     // Configurer les paramètres : 115200 bauds, 8 bits de données, pas de parité, 1 bit d'arrêt
//     cfsetispeed(&options, B115200);  // vitesse de réception
//     cfsetospeed(&options, B115200);  // vitesse d'émission

//     options.c_cflag &= ~PARENB;    // Pas de parité
//     options.c_cflag &= ~CSTOPB;    // 1 bit d'arrêt
//     options.c_cflag &= ~CSIZE;
//     options.c_cflag |= CS8;        // 8 bits de données
//     options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // mode raw
//     options.c_iflag &= ~(IXON | IXOFF | IXANY);         // désactiver le flow control logiciel
//     options.c_oflag &= ~OPOST;                          // mode raw output
//     options.c_cc[VMIN]  = 8;                            // attendre 8 octets minimum
//     options.c_cc[VTIME] = 10;         

//     options.c_cflag |= CREAD | CLOCAL;  // Activer la lecture et ignorer les lignes de contrôle

//     // Appliquer les paramètres configurés
//     if (tcsetattr(fd, TCSANOW, &options) < 0) {
//         perror("Erreur lors de l'application des paramètres du port série");
//         return -1;
//     }
//     return 0;
// }

// long int	get_time_elapsed(t_timeval *starting_time)
// {
// 	t_timeval	current_time;
// 	long int	time_elapsed;

// 	gettimeofday(&current_time, NULL);
// 	time_elapsed = ((current_time.tv_sec - starting_time->tv_sec) * 1000)
// 		+ ((current_time.tv_usec - starting_time->tv_usec) * 0.001);
// 	return (time_elapsed);
// }

// float read_sensor_data(int uart_fd)
// {
//     size_t  bytes_read;
//     char    buffer[BUFFER_SIZE];
//     float   distance; 

//     bytes_read = read(uart_fd, buffer, sizeof(buffer) - 1);
//     if (bytes_read > 0) 
//     {
//         buffer[bytes_read] = '\0';
//         distance = atof(buffer);
//     }
//     printf("buffer = %s bytes read = %ld\n", buffer, bytes_read);
//     return (distance);
// }

void copy_float_tab(float *src, float *dst, int size)
{
    for (int i = 0; i < size; i++)
        dst[i] = src[i];
}

// void get_distance_tab(int uart_fd, float *tab, int size, int fill)
// {
//     float sensor_data = 0;
//     if (fill)
//     {
//         for (int i = 0; i < size; i++)
//         {
//             sensor_data = read_sensor_data(uart_fd);
//             if (sensor_data == -1)
//                 sensor_data = 0;
//             tab[i] = sensor_data / 1000;
//         }
//         return ;
//     }
//     sensor_data = read_sensor_data(uart_fd);
//     // printf("sensor data = %f\n", sensor_data);
//     if (sensor_data == -1)
//         sensor_data = 0;
//     // printf("sensor data = %f\n", sensor_data);
//     for (int i = 0; i < size - 1; i++)
//         tab[i] = tab[i + 1];
//     tab[size - 1] = sensor_data;
//     // for (int i = 0; i < size - 1; i++)
//         // printf("tab[%d] = %f\n", i, tab[i]);
//     printf("=========\n");
// }

// float get_avg_ftab(float *tab, int size)
// {
//     float avg = 0;

//     for (int i = 0; i < size; i++)
//         avg += tab[i];
//     return (avg / size);
// }

float lerp(float a, float b, float t)
{
    return (a * (1 - t) + b * t);
}

float norm_value(float value, float min, float max)
{
    if (max - min == 0.0f)
        return 0.0f;  // pour éviter une division par zéro
    float t = (value - min) / (max - min);
    if (t < 0.0f)
        return 0.0f;
    if (t > 1.0f)
        return 1.0f;
    return t;
}

void encode_color_rmt(uint8_t *led, int index, int color)
{
    led[index * 3 + 0] = get_r(color);
    led[index * 3 + 1] = get_g(color);
    led[index * 3 + 2] = get_b(color);
}

void print_matrix(int matrix[15][20])
{
    for (int i = 0; i < G_HEIGHT; i++)
    {
        for (int j = 0; j < G_WIDTH; j++)
        {
            printf("%d | ", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n\n");
}

long int	get_time_elapsed(t_timeval *starting_time)
{
	t_timeval	current_time;
	long int	time_elapsed;

	gettimeofday(&current_time, NULL);
	time_elapsed = ((current_time.tv_sec - starting_time->tv_sec) * 1000)
		+ ((current_time.tv_usec - starting_time->tv_usec) * 0.001);
	return (time_elapsed);
}

int start_radial(t_rmt *module, sensor_data_t *sensor_data)
{
    // float            target_frame_time_ms = 33.333f; // 1000 / 60 (fps)
    // static float     distance_tab[10];
    // float            distance;
    // uint64_t         start_time;
    // int colors_1[5];
    // int colors_2[5];
    
    __uint8_t led_data[300 * 3];

    // palette_one(colors_1);
    // palette_two(colors_2);
    // if (frame == 0)
    //     get_distance_tab(window->uart_fd, distance_tab, 10, 1);
    // else
    //     get_distance_tab(window->uart_fd, distance_tab, 10, 0);
    // distance = get_avg_ftab(distance_tab, 10);
    // distance = clamp(distance, 0.0f, 100.0f);
    // distance = norm_value(distance, 0, 50.0f);
    // printf("distance = %f\n", distance);
    // for (int i = 0; i < 5; i++)
    //     active_palette[i] = interpolate_color(colors_1[i], colors_2[i], distance);
    // pixellization = lerp(pixellization, distance, 0.2f);
    // printf("branches = %f\n", pixellization);
    // start_time = esp_timer_get_time();
    radial_gradient();
    // frame_time = get_time_elapsed(&timer);
    // if (target_frame_time_ms < frame_time)
    //     usleep((target_frame_time_ms - frame_time) * 1000);
    // t_img *img = init_img(window);
    // for (int i = 0; i < G_HEIGHT; i++)
    // {
    //     for (int j = 0; j < G_WIDTH; j++)
    //     {
    //         if (i % 2 != 0)
    //             draw_cell(img, j, i, leds[i][19 - j]);  // c'est pas degueu d'imprimer cmme ca en mode reel
    //         else
    //             draw_cell(img, j, i, leds[i][j]);
    //     }
    //     // draw_cell(img, j, i, leds[i][j]);
    // }
    int led_i = 0;
    for (int i = 0; i < G_HEIGHT; i++)
    {
        for (int j = 0; j < G_WIDTH; j++)
        {
            if (i % 2 != 0)
                encode_color_rmt(led_data, led_i, leds[i][19 - j]);
            else
                encode_color_rmt(led_data, led_i, leds[i][j]);
            led_i++;
        }
        // draw_cell(img, j, i, leds[i][j]);
    }
    // push_img(img, window);
    frame++;
    // print_matrix(leds);
    ESP_ERROR_CHECK(rmt_tx_wait_all_done(module->tx_channel, -1));
    ESP_ERROR_CHECK(rmt_transmit(module->tx_channel, module->encoder, led_data, sizeof(led_data), &module->tx_config));
    // printf("temps ecoule = %lld\n", esp_timer_get_time() - start_time);
    return (0); 
}

typedef struct sensor_data_s
{
    pthread_mutex_t *avg_lock;
    float           *average;
}   sensor_data_t;

void get_avg_routine(sensor_data_t *sensor_data)
{
    
}

int radial_loop(t_rmt *module)
{
    pthread_t       sensor_thread;
    pthread_mutex_t avg_mutex;
    sensor_data_t   sensor_data;
    float           avg_distance = 0;

    if (pthread_mutex_init(&avg_mutex, NULL) == -1)
        return (-1);
    sensor_data.average = &avg_distance;
    sensor_data.avg_lock = &avg_mutex;
    if (pthread_create(&sensor_thread, NULL, (void *)get_avg_routine, &sensor_data) == -1)
    while (1)
    {
        start_radial(module, &sensor_data);
    }
    return (0);
}