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

    // float pulse = sin(distance * 0.1f + frame * 0.01f);
    // float r = distance * (form + (pixellization * pulse * sin(branches * angle)));
    float r = *distance * (form + (pixellization * sin(branches * angle)));
    float raw_color = (r / max_distance + (float)frame * anim_speed);
    // printf("pixellisation = %f\n", pixellization);
    // printf("raw color = %f\n", fmodf(fabsf(raw_color), 1.0f));
    return (fmodf(fabsf(raw_color), 1.0f));
}

void radial_gradient()
{
    // t_cell cell;
    t_cell center = {9, 7};
    float max_distance = 9;
    int   colors[5];
    float color;
    float distance;

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
    // float form = -10.0f;         // -2, -200

    // float anim_relaunch_radius = clamp((frame - restart_frame) * 0.005f, 0.0f, 1.0f);

    float ring_radius = 0.0f; // avance de 0 à max_distance
    float ring_thickness = 0.3f; // plus petit = plus fin
    static int ring_color_idx = 0;
    int ring_color = colors[ring_color_idx];

    // if (ring_active)
    // {
    //     float ring_speed = 0.08f;  // plus petit = plus lent
    //     // ring_radius = (frame - ring_start_frame) * ring_speed;
    //     float t = clamp((frame - ring_start_frame) * ring_speed / max_distance, 0.0f, 1.0f);
    //     ring_radius = max_distance * (t < 0.5f
    //     ? 2.0f * t * t                    // ease-in
    //     : -1.0f + (4.0f - 2.0f * t) * t); // ease-out

    //     // printf("ring active = %d\n", ring_active);
    //     // printf("ring radius = %f\n", ring_radius);
    //     if (ring_radius > 1) {
    //         ring_active = 0; // fin de l'effet
    //         // ring_radius = 0;
    //         // printf("END\n");
    //         ring_color_idx++;
    //         if (ring_color_idx == 5)
    //             ring_color_idx = 0;
    //     }
    // }

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
            if (ring_active && fabsf(distance - ring_radius) < ring_thickness)
                leds[j][i] = ring_color;
            else
                leds[j][i] = get_color_gradient(color, colors, 5);
            // leds[j][i] = get_color_gradient(color, colors, 5);
            // printf("led data = %d\n", leds[j][i]);
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

// void set_pixellization()
// {
//     static bool in_transition = false;
//     static float interp = 0.0f;

//     // Déclenchement

//     // printf("pixel = %d\n", transition_pixel);
//     if (pixel_transition && !in_transition) {
//         in_transition = true;
//         interp = 0.0f;
//     }

//     // Animation en cours
//     if (in_transition) {
//         interp += 0.01f;  // vitesse de l'animation
//         if (interp >= 1.0f) {
//             interp = 1.0f;
//             in_transition = false;
//             pixel_transition = 0;
//             pixel_anim_finished = 1;
//         }
//         // printf("interp = %f\n", interp);
//         // Animation de -27 à 27
//         pixellization = lerp(-27.0f, 27.0f, interp);
//     }
//     // Sinon : on ne touche pas à pixellization
// }

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


// void set_branches()
// {
//     static bool in_transition = false;
//     static float interp = 0.0f;

//     // Déclenchement

//     // printf("pixel = %d\n", transition_pixel);
//     if (branch_transition && !in_transition) {
//         in_transition = true;
//         interp = 0.0f;
//     }

//     // Animation en cours
//     if (in_transition) {
//         interp += 0.02f;  // vitesse de l'animation
//         if (interp >= 1.0f) {
//             interp = 1.0f;
//             branch_transition = 0;
//             branch_anim_finished = 1;
//             in_transition = false;
//             branch_transition_exit = true;
//         }
//         branches = lerp(-2.0f, -3.0f, interp);
//     }
//     // else if (branch_transition_exit)
//     // {
//     //     interp -= 0.02f;  // vitesse de l'animation
//     //     if (interp <= 0) {
//     //         interp = 1.0f;
//     //         branch_transition_exit = false;
//     //     }
//     //     branches = lerp(-3.0f, -2.0f, interp);
//     // }
//     printf("branches = %f\n", branches);
//     // Sinon : on ne touche pas à pixellization
// }

// void    set_pixellization(sensor_data_t *sensor_data, float *distance)
// {
//     // Lissage EMA
//     float alpha = 0.05f;  // rapide mais fluide
    
//     xSemaphoreTake(sensor_data->dist_lock, portMAX_DELAY);
//     sensor_data->sens_1_interp += alpha;
//     if (sensor_data->sens_1_interp > 1.0f)
//         sensor_data->sens_1_interp = 1.0f;
//     *distance = lerp(sensor_data->sens_1_last_value, sensor_data->sens_1_next_value, sensor_data->sens_1_interp);
//     xSemaphoreGive(sensor_data->dist_lock);

// //     float curved = powf(normalize_value(*distance, 0.0f, 50.0f), 1.2f); // légère courbe pour douceur
// //     pixellization = lerp(27.0f, 3.0f, curved);
// //     printf("pixellisation : %f\n", pixellization);

//         // Définir vos valeurs de pixellisation souhaitées
//     float pixellisation_proche = 27.0f;  // quand distance = 0
//     // float pixellisation_loin = 3.0f;     // quand distance = 400
//     float pixellisation_loin = -20.0f;     // quand distance = 400
    
//     // Clamping de la distance pour être sûr
//     float dist_clamped = fminf(fmaxf(*distance, 0.0f), 400.0f);
    
//     // Normalisation SIMPLE : 0-400 → 0-1
//     float normalized = dist_clamped / 400.0f;
    
//     // Application de la courbe pour la douceur (optionnel)x
//     float curved = powf(normalized, 1.2f);
    
//     // Mapping linéaire direct - FORMULE SIMPLE
//     pixellization = pixellisation_proche + curved * (pixellisation_loin - pixellisation_proche);

//     // if (pixellization == 27)
    
//     // printf("pixellisation = %f\n", pixellization);
//     // Debug
//     // printf("Distance: %.2f, Normalized: %.3f, Curved: %.3f, Pixellisation: %.2f\n", 
//     //        *distance, normalized, curved, pixellization);
// }

// void set_pixellization(sensor_data_t *sensor_data, float *distance)
// {
//     // Lissage EMA
//     float alpha = 0.05f;
//     xSemaphoreTake(sensor_data->dist_lock, portMAX_DELAY);
    
//     sensor_data->sens_1_interp += alpha;
//     if (sensor_data->sens_1_interp > 1.0f)
//         sensor_data->sens_1_interp = 1.0f;
    
//     float raw_distance = lerp(sensor_data->sens_1_last_value, sensor_data->sens_1_next_value, sensor_data->sens_1_interp);  
    
//     // === FILTRAGE DES VALEURS ABERRANTES ===
//     static float previous_distance = 0.0f;
//     static int outlier_count = 0;
//     static float outlier_value = 0.0f;
    
//     const float MAX_JUMP = 50.0f;  // Seuil de variation max acceptable
//     const int OUTLIER_THRESHOLD = 10; // Nombre de mesures cohérentes nécessaires
    
//     float distance_diff = fabsf(raw_distance - previous_distance);
    
//     if (distance_diff > MAX_JUMP && raw_distance > 350.0f) { // Valeur potentiellement aberrante
//         if (outlier_value == 0.0f || fabsf(raw_distance - outlier_value) < 20.0f) {
//             // Première aberrante ou cohérente avec la précédente aberrante
//             outlier_value = raw_distance;
//             outlier_count++;
            
//             if (outlier_count < OUTLIER_THRESHOLD) {
//                 // Pas encore validée, on garde l'ancienne valeur
//                 *distance = previous_distance;
//             } else {
//                 // Validée par cohérence, on l'accepte
//                 *distance = raw_distance;
//                 previous_distance = raw_distance;
//                 outlier_count = 0;
//                 outlier_value = 0.0f;
//             }
//         } else {
//             // Nouvelle aberrante différente, on reset
//             outlier_value = raw_distance;
//             outlier_count = 1;
//             *distance = previous_distance;
//         }
//     } else {
//         // Valeur normale
//         *distance = raw_distance;
//         previous_distance = raw_distance;
//         outlier_count = 0;
//         outlier_value = 0.0f;
//     }
    
//     xSemaphoreGive(sensor_data->dist_lock);
    
//     // Reste du code de pixellisation...
//     float pixellisation_proche = 27.0f;
//     float pixellisation_loin = -20.0f;
    
//     float dist_clamped = fminf(fmaxf(*distance, 0.0f), 400.0f);
//     float normalized = dist_clamped / 400.0f;
//     float curved = powf(normalized, 1.2f);
    
//     pixellization = pixellisation_proche + curved * (pixellisation_loin - pixellisation_proche);
// }

// void set_pixellization(sensor_data_t *sensor_data, float *distance)
// {
//     // Lissage EMA
//     float alpha = 0.06f; // rapide mais fluide
//     xSemaphoreTake(sensor_data->dist_lock, portMAX_DELAY);
//     sensor_data->sens_1_interp += alpha;
//     if (sensor_data->sens_1_interp > 1.0f)
//         sensor_data->sens_1_interp = 1.0f;
//     *distance = lerp(sensor_data->sens_1_last_value, sensor_data->sens_1_next_value, sensor_data->sens_1_interp);
//     xSemaphoreGive(sensor_data->dist_lock);
    
//     // Normalisation corrigée
//     float curved = powf(normalize_value(*distance, 0.0f, 400.0f), 1.2f); // ← 400 au lieu de 50
//     pixellization = lerp(27.0f, -27.0f, curved);
//     // branches = lerp(-2, 2, curved);
//     // printf("pixellisation : %f\n", pixellization);
// }

int radial_loop(t_rmt *module, sensor_data_t *sensor_data)
{
    // float               target_frame_time_ms = 33.333f; // 1000 / 60 (fps)
    // static float     distance_tab[10];
    static float        distance = -1.0f;
    // long int            frame_time;
    // t_timeval           timer;
    // int colors_1[5];
    // int colors_2[5];
    
    __uint8_t led_data[300 * 3];

    // sensor_data_t *sensor_data = (sensor_data_t *)data;
    // palette_one(colors_1);
    // palette_two(colors_2);
    
    // distance = clamp(distance, 0.0f, 50.0f);

    // get_sensor_1_values(sensor_data, &distance);
    set_pixellization();
    set_branches();
    // printf("distance = %f\n", distance);
    // printf("pixellisation = %f\n", pixellization);
    // printf("distance in main = %f\n", distance);
        
    // printf("di = %f interp = %f last = %f next = %f pix = %f\n", distance, sensor_data->interp, sensor_data->last_value, sensor_data->next_value, pixellization);
    // printf("pixellization : %f\n", pixellization);

    radial_gradient();
    // printf("coucou\n");
    // printf("pixesllisation = %f\n", pixellization);
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
    // printf("couocu\n");
    // if (pthread_create(&sensor_thread, NULL, (void *)distance_thread_routine, data) == -1)
        // return (-1);
    while (1)
        radial_loop(module, &sensor_data);
    return (0);
}