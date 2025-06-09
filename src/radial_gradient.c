#include "../include/include.h"
#include "../include/rmt_module.h"
#include "esp_timer.h"
#include "esp_pthread.h"
#include "pthread.h"

float branches = -2.0f;
float pixellization = 10; // 2.90f; // 1,
float spiral_speed = 0.01f;
float anim_speed = 0.02f; // 0.005f
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

void reset_cell_state()
{
    for (int i = 0; i < G_WIDTH; i++)
    {
        for (int j = 0; j < G_HEIGHT; j++)
            cell_state[i][j] = 0;
    }
}

float calculate_radient(t_cell *center, float form, float max_distance, int i, int j)
{
    t_cell cell = {i, j};

    cell.x = i;
    cell.y = j;
    float angle = atan2(cell.y - center->y, cell.x - center->x);
    float distance = get_norm_distance(&cell, center, max_distance);
    float pulse = sin(distance * 0.1f + frame * 0.01f);
    float r = distance * (form + (pixellization * pulse * sin(branches * angle)));
    // float r = distance * (form + (pixellization * sin(branches * angle)));
    float raw_color = (r / max_distance + (float)frame * anim_speed);
    return (fmodf(fabsf(raw_color), 1.0f));
}

void radial_gradient()
{
    // t_cell cell;
    t_cell center = {9, 7};
    float max_distance = 9;
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
            color = calculate_radient(&center, form, max_distance, i, j);
            // cell.x = i;
            // cell.y = j;
            // float angle = atan2(cell.y - center.y, cell.x - center.x);
            // float distance = get_norm_distance(&cell, &center, max_distance);
            // float pulse = sin(distance * 0.1f + frame * 0.01f);
            // float r = distance * (form + (pixellization * pulse * sin(branches * angle)));
            // // float r = distance * (form + (pixellization * sin(branches * angle)));
            // float raw_color = (r / max_distance + (float)frame * anim_speed);
            // color = fmodf(fabsf(raw_color), 1.0f);

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

void add_endline(char *str)
{
    int i = 0;
    while (str[i])
        i++;
    str[i] = '\n';
    str[i + 1] = '\0';
}

float lerp(float a, float b, float t)
{
    return (a * (1 - t) + b * t);
}

void encode_color_rmt(uint8_t *led, int index, int color)
{
    led[index * 3 + 0] = get_r(color);
    led[index * 3 + 1] = get_g(color);
    led[index * 3 + 2] = get_b(color);
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

void get_sensor_values(sensor_data_t *data, float *curr_distance)
{
    pthread_mutex_lock(data->avg_lock);
    float avg = clamp(data->dist_sensor_1, 0.0f, 100.0f);
    if (*curr_distance == -1)
    {
        *curr_distance = avg;
        pthread_mutex_unlock(data->avg_lock);
        return ;
    }
    if (avg == *curr_distance)
    {
        pthread_mutex_unlock(data->avg_lock);
        return ;
    }
    data->last_value = *curr_distance;
    data->next_value = avg;
    data->interp = 0.0f;
    *curr_distance = avg;
    pthread_mutex_unlock(data->avg_lock);
}

void send_data_leds(uint8_t *led_data, t_rmt *module)
{
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
    }
    ESP_ERROR_CHECK(rmt_transmit(module->tx_channel, module->encoder, led_data, sizeof(led_data), &module->tx_config));
    ESP_ERROR_CHECK(rmt_tx_wait_all_done(module->tx_channel, -1));
}

int radial_loop(t_rmt *module, sensor_data_t *sensor_data)
{
    // float               target_frame_time_ms = 33.333f; // 1000 / 60 (fps)
    // static float     distance_tab[10];
    float               smoothed_distance = -1.0f;
    static float        distance = -1.0f;
    // long int            frame_time;
    // t_timeval           timer;
    // int colors_1[5];
    // int colors_2[5];
    
    __uint8_t led_data[300 * 3];

    // sensor_data_t *sensor_data = (sensor_data_t *)data;
    // palette_one(colors_1);
    // palette_two(colors_2);

    if (smoothed_distance < 0.0f)
        smoothed_distance = distance;
    
    // distance = clamp(distance, 0.0f, 50.0f);

    get_sensor_values(sensor_data, &distance);
    // printf("distance in main = %f\n", distance);
    
    // Lissage EMA
    float alpha = 0.05f;  // rapide mais fluide
    
    pthread_mutex_lock(sensor_data->avg_lock);
    sensor_data->interp += alpha;
    if (sensor_data->interp > 1.0f)
        sensor_data->interp = 1.0f;
    smoothed_distance = lerp(sensor_data->last_value, sensor_data->next_value, sensor_data->interp);
    pthread_mutex_unlock(sensor_data->avg_lock);
    
    distance = smoothed_distance;
    float normalized = normalize_value(smoothed_distance, 0.0f, 50.0f);
    float curved = powf(normalized, 1.2f); // légère courbe pour douceur
    
    pixellization = curved * 12.0f;
        
    // printf("di = %f interp = %f last = %f next = %f pix = %f\n", distance, sensor_data->interp, sensor_data->last_value, sensor_data->next_value, pixellization);
    // printf("pixellization : %f\n", pixellization);

    radial_gradient();
    send_data_leds(led_data, module);
    frame++;
    return (0);
}

void update_average_distance(sensor_data_t *data)
{
    int  sample_sens_1;
    int  sample_sens_2;
    // char sample1[11];
    char sample1[10];
    char sample2[10];
    char final_str[23];

    sample_sens_1 = read_distance_ms(SENSOR_READ_1, SENSOR_TRIG_1);
    sample_sens_2 = read_distance_ms(SENSOR_READ_2, SENSOR_TRIG_2);
    
    pthread_mutex_lock(data->avg_lock);
    data->dist_sensor_1 = sample_sens_1;
    data->dist_sensor_2 = sample_sens_2;
    pthread_mutex_unlock(data->avg_lock);

    // printf("sample 1 = %d\n", sample_sens_1);
    // printf("sample 2 = %d\n", sample_sens_2);

    bzero(sample1, 10);
    bzero(sample2, 10);
    bzero(final_str, 23);
    itoa(sample_sens_1, sample1, 10);
    itoa(sample_sens_2, sample2, 10);
    add_endline(sample1);
    // add_endline(sample2);
    strcat(final_str, sample1);
    strcat(final_str, sample2);
    // printf("int sample = %d\n", sample_sens_1);
    // printf("sample str = %s\n", sample1);
    uart_write_bytes(UART_NUM_0, final_str, 23);
    // printf("final_str = %s\n", final_str);
    // uart_write_bytes(UART_NUM_0, final_str, 23);
}

void distance_thread_routine(void *data)
{
    sensor_data_t *sensor_data = (sensor_data_t *)data;
    while (1)
    {
        update_average_distance(sensor_data);
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}

int start_radial(t_rmt *module)
{
    pthread_t       sensor_thread;
    pthread_mutex_t avg_mutex;
    pthread_mutex_t interp_mutex;
    sensor_data_t   sensor_data;

    if (pthread_mutex_init(&avg_mutex, NULL) == -1 || pthread_mutex_init(&interp_mutex, NULL) == -1)
        return (-1);
    sensor_data.dist_sensor_1 = 0;
    sensor_data.dist_sensor_2 = 0;
    sensor_data.last_value = 0;
    sensor_data.next_value = 0;
    sensor_data.interp = 0;
    sensor_data.avg_lock = &avg_mutex;
    sensor_data.interp_lock = &interp_mutex;
    void *data = &sensor_data;
    if (pthread_create(&sensor_thread, NULL, (void *)distance_thread_routine, data) == -1)
        return (-1);
    while (1)
        radial_loop(module, &sensor_data);
    return (0);
}