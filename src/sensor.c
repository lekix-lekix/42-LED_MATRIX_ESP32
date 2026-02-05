#include "../include/include.h"

// Variables globales pour debug
volatile int64_t echo_1_start = 0;
volatile int64_t echo_1_end = 0;
volatile bool echo_1_received = false;
volatile bool front_montant_1_detecte = false;

volatile int64_t echo_2_start = 0;
volatile int64_t echo_2_end = 0;
volatile bool echo_2_received = false;
volatile bool front_montant_2_detecte = false;

extern bool             bruit_detecte;
extern volatile int64_t bruit_timestamp;
extern int              ring_active;
extern int              ring_start_frame;
extern int              frame;

extern int              pixel_transition;
extern int              pixel_anim_finished;
extern int              pixel_anim_return;

extern int              branch_anim_return;
extern int              branch_transition;
extern int              branch_anim_finished;

extern int              ring_color_idx;

// ISR : routine d’interruption appelée sur front descendant
void IRAM_ATTR sound_isr_handler(void* arg)
{
    int level = gpio_get_level(SOUND_SENSOR);
    int64_t now = esp_timer_get_time();  // microsecondes depuis boot

    if (level == 0) {
        // Le capteur KY-038 déclenche généralement un front descendant sur détection de son
        bruit_detecte = true;
        bruit_timestamp = now;
        ring_active = 1;
        ring_start_frame = frame;
    }
}

void IRAM_ATTR echo_1_isr_handler(void* arg)
{
    int level = gpio_get_level(SENSOR_READ_1);
    int64_t timestamp = esp_timer_get_time();
    
    if (level == 1) {
        // Front montant
        echo_1_start = timestamp;
        front_montant_1_detecte = true;
        echo_1_received = false;
    } else if (level == 0 && front_montant_1_detecte) {
        // Front descendant (seulement si on a eu le front montant)
        echo_1_end = timestamp;
        echo_1_received = true;
    }
}

void IRAM_ATTR echo_2_isr_handler(void* arg)
{
    int level = gpio_get_level(SENSOR_READ_2);
    int64_t timestamp = esp_timer_get_time();
    
    if (level == 1) {
        // Front montant
        echo_2_start = timestamp;
        front_montant_2_detecte = true;
        echo_2_received = false;
    } else if (level == 0 && front_montant_2_detecte) {
        // Front descendant (seulement si on a eu le front montant)
        echo_2_end = timestamp;
        echo_2_received = true;
    }
}

void setup_sound_interrupt()
{
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << SOUND_SENSOR),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE  // Déclenche sur front descendant
    };
    gpio_config(&io_conf);

    gpio_isr_handler_add(SOUND_SENSOR, sound_isr_handler, NULL);
}

void setup_echo_1_interrupt()
{
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << SENSOR_READ_1),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_ANYEDGE
    };
    gpio_config(&io_conf);
    
    gpio_isr_handler_add(SENSOR_READ_1, echo_1_isr_handler, NULL);
}

void setup_echo_2_interrupt()
{
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << SENSOR_READ_2),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_ANYEDGE
    };
    gpio_config(&io_conf);
    
    gpio_isr_handler_add(SENSOR_READ_2, echo_2_isr_handler, NULL);
}

void add_space(char *str)
{
    int i = 0;

    while (str[i])
        i++;
    str[i] = ' ';
    str[i + 1] = '\0';
}

int32_t read_distance_1_interrupt(int sensor_trig)
{
    // Reset des variables
    echo_1_received = false;
    front_montant_1_detecte = false;
    echo_1_start = 0;
    echo_1_end = 0;
    
    // Envoi du trigger
    gpio_set_level(sensor_trig, 1);
    esp_rom_delay_us(10);
    gpio_set_level(sensor_trig, 0);
    
    // Attente de la réponse avec timeout plus long
    int64_t timeout_start = esp_timer_get_time();
    while (!echo_1_received) {
        int64_t elapsed = esp_timer_get_time() - timeout_start;
        
        if (elapsed > 150000) { // 100ms timeout
            if (front_montant_1_detecte) {
            }
            return -1;
        }
        vTaskDelay(1);
    }
    
    // Calcul avec debug
    int64_t duration = echo_1_end - echo_1_start;
    int32_t distance = duration * 0.0343f / 2; 
    
    return distance;
}

int32_t read_distance_2_interrupt(int sensor_trig)
{
    // Reset des variables
    echo_2_received = false;
    front_montant_2_detecte = false;
    echo_2_start = 0;
    echo_2_end = 0;
    
    // Envoi du trigger
    gpio_set_level(sensor_trig, 1);
    esp_rom_delay_us(10);
    gpio_set_level(sensor_trig, 0);
    
    // Attente de la réponse avec timeout plus long
    int64_t timeout_start = esp_timer_get_time();
    while (!echo_2_received) {
        int64_t elapsed = esp_timer_get_time() - timeout_start;
        
        if (elapsed > 150000) { // 100ms timeout
            if (front_montant_2_detecte) {
            }
            return -1;
        }
        vTaskDelay(1);
    }
    
    // Calcul avec debug
    int64_t duration = echo_2_end - echo_2_start;
    int32_t distance = duration * 0.0343f / 2;
    
    return distance;
}

void update_shared_value(sensor_data_t *data, int new_value)
{
    xSemaphoreTake(data->dist_lock, portMAX_DELAY);
    data->dist_sensor_1 = new_value;
    xSemaphoreGive(data->dist_lock);
}

void copy_tab_int(int dst[10], int src[10])
{
    int i = 0;

    while (i < 10)
    {
        dst[i] = src[i];
        i++;
    }
}

void update_tab(int tab[10], int new_value)
{
    int tab_buffer[10];
    int i = 0;

    copy_tab_int(tab_buffer, tab);
    while (i < 9)
    {
        tab[i] = tab_buffer[i];
        i++;
    }
    tab[i] = new_value;
}

float  calc_mean(int tab[10])
{
    int i = -1;
    int total = 0;

    while (++i < 10)
        total += tab[i];
    return ((float)total / (float)10);
}

void update_average_distance_sens1(sensor_data_t *data)
{
    static int  samples[10];
    static int  nb_values = 0;
    static bool already_triggered = false;
    int         sample_sens_1;

    sample_sens_1 = read_distance_1_interrupt(SENSOR_TRIG_1);
    // printf("distance 1 = %d\n", sample_sens_1);

    if (nb_values < 10)
    {
        samples[nb_values] = sample_sens_1;
        update_shared_value(data, sample_sens_1);
        nb_values++;
    }
    else
    {
        update_tab(samples, sample_sens_1);
        float diff = fabs(sample_sens_1 - calc_mean(samples));
        printf("transi valu 1 : %f\n", diff);
        if (diff > 250.0f && !already_triggered) {
            pixel_transition = 1;
            already_triggered = true;  // bloquer les déclenchements suivants
        } else if (pixel_anim_finished && diff < 250) {
            already_triggered = false; // reset seulement si la distance se stabilise
            pixel_anim_return = true;
        }
    }
}

void update_average_distance_sens2(sensor_data_t *data)
{
    static int  samples[10];
    static int  nb_values = 0;
    static bool already_triggered = false;
    int         sample_sens_1;

    sample_sens_1 = read_distance_2_interrupt(SENSOR_TRIG_2);
    printf("distance 2 = %d\n", sample_sens_1);

    if (nb_values < 10)
    {
        samples[nb_values] = sample_sens_1;
        update_shared_value(data, sample_sens_1);
        nb_values++;
    }
    else
    {
        update_tab(samples, sample_sens_1);
        float diff = fabs(sample_sens_1 - calc_mean(samples));
        printf("transi valu 2 : %f\n", diff);
        if (diff > 250.0f && !already_triggered) {
            branch_transition = 1;
            already_triggered = true;  // bloquer les déclenchements suivants
        } else if (branch_anim_finished && diff < 250) {
            already_triggered = false; // reset seulement si la distance se stabilise
            branch_anim_return = true;
        }
    }
}

void get_sensor_1_values(sensor_data_t *data, float *curr_distance)
{
    xSemaphoreTake(data->dist_lock, portMAX_DELAY);
    float dist_1 = clamp(data->dist_sensor_1, 0.0f, 400.0f);
    if (*curr_distance == -1)
    {
        *curr_distance = dist_1;
        xSemaphoreGive(data->dist_lock);
        return ;
    }
    if (dist_1 == *curr_distance)
    {
        xSemaphoreGive(data->dist_lock);
        return ;
    }
    data->sens_1_last_value = *curr_distance;
    data->sens_1_next_value = dist_1;
    data->sens_1_interp = 0.0f;
    *curr_distance = dist_1;
    xSemaphoreGive(data->dist_lock);
}

void get_sensor_2_values(sensor_data_t *data, float *curr_distance)
{
    xSemaphoreTake(data->dist_lock, portMAX_DELAY);
    float dist_2 = clamp(data->dist_sensor_2, 0.0f, 100.0f);
    if (*curr_distance == -1)
    {
        *curr_distance = dist_2;
        xSemaphoreGive(data->dist_lock);
        return ;
    }
    if (dist_2 == *curr_distance)
    {
        xSemaphoreGive(data->dist_lock);
        return ;
    }
    data->sens_1_last_value = *curr_distance;
    data->sens_1_next_value = dist_2;
    data->sens_1_interp = 0.0f;
    *curr_distance = dist_2;
    xSemaphoreGive(data->dist_lock);
}