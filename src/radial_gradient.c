#include "../include/include.h"

float branches = -2.0f;
float pixellization = -27; // 2.90f; // 1,
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

int   pixel_anim_return = 0;
int   pixel_transition = 0;
int   pixel_anim_finished = 0;
int   pixel_transition_exit = 0;

int   branch_anim_return = 0;
int   branch_transition = 0;
int   branch_anim_finished = 0;
int   branch_transition_exit = 0;

int   leds[15][20];
int   frame = 0;
int   cell_state[G_HEIGHT][G_WIDTH] = {0};

volatile bool bruit_detecte = false;
volatile int64_t bruit_timestamp = 0;

float ring_radius;
int   ring_active;
int   ring_start_frame;

void reset_cell_state()
{
    for (int i = 0; i < G_WIDTH; i++)
    {
        for (int j = 0; j < G_HEIGHT; j++)
            cell_state[i][j] = 0;
    }
}

float calculate_radient(t_cell *center, float max_distance, float *distance, int x, int y)
{
    t_cell cell;

    cell.x = x;
    cell.y = y;

    float angle = atan2(cell.y - center->y, cell.x - center->x);
    *distance = get_norm_distance(&cell, center, max_distance);

    float form = -8.0f + 2.0f * sin(frame * 0.02f); // pulsation douce

    float r = *distance * (form + (pixellization * sin(branches * angle)));
    float raw_color = (r / max_distance + (float)frame * anim_speed);
    return (fmodf(fabsf(raw_color), 1.0f));
}

void radial_gradient()
{
    t_cell center = {9, 7};
    float max_distance = 9;
    int   colors[5];
    float color;
    float distance;

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

    float ring_radius = 0.0f; // avance de 0 à max_distance
    float ring_thickness = 0.3f; // plus petit = plus fin
    static int ring_color_idx = 0;
    int ring_color = colors[ring_color_idx];

    if (ring_active)
    {
        float ring_speed = 0.04f;
        float t = clamp((frame - ring_start_frame) * ring_speed / max_distance, 0.0f, 1.0f);

        // Ease-out uniquement (ex : t * (2 - t))
        ring_radius = max_distance * (t * (2.0f - t));

        if (ring_radius > 1.0f) {
            ring_active = 0;
            ring_color_idx++;
            if (ring_color_idx == 5)
                ring_color_idx = 0;
        }
    }

    for (int i = 0; i < G_WIDTH; i++)
    {
        for (int j = 0; j < G_HEIGHT; j++)
        {
            color = calculate_radient(&center, max_distance, &distance, i, j);
            if (ring_active && fabsf(distance - ring_radius) < ring_thickness)
                leds[j][i] = ring_color;
            else
                leds[j][i] = get_color_gradient(color, colors, 5);
        }
    }
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
            reset_cell_state(); 
            nb_colors = (nb_colors + 1) % 5;     // ajoute la nouvelle couleur
            next_color = (next_color + 1) % 5;
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

void send_data_leds(uint8_t led_data[900], t_rmt *module)
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
    ESP_ERROR_CHECK(rmt_transmit(module->tx_channel, module->encoder, led_data, sizeof(uint8_t) * 900, &module->tx_config));
    ESP_ERROR_CHECK(rmt_tx_wait_all_done(module->tx_channel, -1));
}

void set_pixellization()
{
    static bool in_transition = false;
    static float interp = 0.0f;
    static bool going_down = true;  // true : -2 -> -3, false : -3 -> -2

    if (pixel_transition && !in_transition) {
        in_transition = true;
        interp = 0.0f;
        // Toggle direction si besoin
        going_down = !going_down;
    }

    if (in_transition) {
        interp += 0.02f;
        if (interp >= 1.0f) {
            interp = 1.0f;
            pixel_transition = 0;
            pixel_anim_finished = 1;
            in_transition = false;
            pixel_transition_exit = true;
        }
        // Lerp dans la bonne direction
        if (going_down)
            pixellization = lerp(-27.0f, 27.0f, interp);
        else
            pixellization = lerp(27.0f, -27.0f, interp);
    }
}

void set_branches()
{
    static bool in_transition = false;
    static float interp = 0.0f;
    static bool going_down = true;  // true : -2 -> -3, false : -3 -> -2

    if (branch_transition && !in_transition) {
        in_transition = true;
        interp = 0.0f;
        // Toggle direction si besoin
        going_down = !going_down;
    }

    if (in_transition) {
        interp += 0.02f;
        if (interp >= 1.0f) {
            interp = 1.0f;
            branch_transition = 0;
            branch_anim_finished = 1;
            in_transition = false;
            branch_transition_exit = true;
        }
        // Lerp dans la bonne direction
        if (going_down)
            branches = lerp(-2.0f, -3.0f, interp);
        else
            branches = lerp(-3.0f, -2.0f, interp);
    }
}

int radial_loop(t_rmt *module, sensor_data_t *sensor_data)
{
    static float        distance = -1.0f;
    
    __uint8_t led_data[300 * 3];

    set_pixellization();
    set_branches();
    radial_gradient();
    send_data_leds(led_data, module);
    frame++;
    return (0);
}

void distance_thread_routine(void *data)
{
    sensor_data_t *sensor_data = (sensor_data_t *)data;
    while (1)
    {
        update_average_distance_sens1(sensor_data);
        vTaskDelay(5 / portTICK_PERIOD_MS);
        update_average_distance_sens2(sensor_data);
        vTaskDelay(5 / portTICK_PERIOD_MS);
    }
}

int start_radial(t_rmt *module)
{
    SemaphoreHandle_t    avg_mutex;
    SemaphoreHandle_t interp_mutex;
    sensor_data_t   sensor_data;

    avg_mutex = xSemaphoreCreateMutex();
    interp_mutex = xSemaphoreCreateMutex();
    if (!avg_mutex || !interp_mutex)
        return (-1);
    sensor_data.dist_sensor_1 = 0;
    sensor_data.dist_sensor_2 = 0;
    sensor_data.sens_1_last_value = 0;
    sensor_data.sens_1_next_value = 0;
    sensor_data.sens_1_interp = 0;
    sensor_data.dist_lock = avg_mutex;
    sensor_data.interp_lock = interp_mutex;
    void *data = &sensor_data;
    xTaskCreatePinnedToCore(distance_thread_routine, "Capteur", 2048, data, 1, NULL, 0);
    while (1)
        radial_loop(module, &sensor_data);
    return (0);
}