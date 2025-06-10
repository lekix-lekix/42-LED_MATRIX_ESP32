#include "../include/include.h"

void add_space(char *str)
{
    int i = 0;

    while (str[i])
        i++;
    str[i] = ' ';
    str[i + 1] = '\0';
}

int32_t read_distance_ms(int sensor_read, int sensor_trig)
{
    int32_t distance = -1;
    // Envoi du trigger
    gpio_set_level(sensor_trig, 1);
    esp_rom_delay_us(10);
    gpio_set_level(sensor_trig, 0);

    // Attente front montant
    int64_t t0 = esp_timer_get_time();
    while (gpio_get_level(sensor_read) == 0) {
        if (esp_timer_get_time() - t0 > 30000)
            break;
    }
    int64_t start = esp_timer_get_time();

    // Attente front descendant
    while (gpio_get_level(sensor_read) == 1) {
        if (esp_timer_get_time() - start > 30000)
            break;
    }
    int64_t end = esp_timer_get_time();

    // if (distance != -1)
    distance = (end - start) * 0.0343f;
    // char str[10];
    // bzero(str, 10);
    // itoa(distance, str, 10);
    // // printf("str = %s\n", str);
    // uart_write_bytes(UART_NUM_0, str, 10);
    return (distance);
}

void update_average_distance(sensor_data_t *data)
{
    int  sample_sens_1;
    int  sample_sens_2;
    // char sample1[11];
    char sample1[6];
    char sample2[6];
    char final_str[13];

    sample_sens_1 = read_distance_ms(SENSOR_READ_1, SENSOR_TRIG_1);
    sample_sens_2 = read_distance_ms(SENSOR_READ_2, SENSOR_TRIG_2);
    
    pthread_mutex_lock(data->dist_lock);
    data->dist_sensor_1 = sample_sens_1;
    data->dist_sensor_2 = sample_sens_2;
    pthread_mutex_unlock(data->dist_lock);

    // printf("sample 1 = %d\n", sample_sens_1);
    // printf("sample 2 = %d\n", sample_sens_2);

    bzero(sample1, 6);
    bzero(sample2, 6);
    bzero(final_str, 12);
    sprintf(sample1, "%d", sample_sens_1);
    add_space(sample1);
    sprintf(sample2, "%d", sample_sens_2);
    // itoa(sample_sens_1, sample1, 6);
    // itoa(sample_sens_2, sample2, 6);
    // printf("sample 1 str = %s\n", sample1);
    strncpy(final_str, sample1, 5);
    strcat(final_str, sample2);
    final_str[12] = '\0';   
    // printf("final_str = %s\n", final_str);
    uart_write_bytes(UART_NUM_0, final_str, 13);
    // printf("int sample = %d\n", sample_sens_1);
    // printf("sample str = %s\n", sample1);
    // uart_write_bytes(UART_NUM_0, final_str, 23);
}

void get_sensor_1_values(sensor_data_t *data, float *curr_distance)
{
    pthread_mutex_lock(data->dist_lock);
    float dist_1 = clamp(data->dist_sensor_1, 0.0f, 100.0f);
    // float dist_2 = clamp(data->dist_sensor_1, 0.0f, 100.0f);
    if (*curr_distance == -1)
    {
        *curr_distance = dist_1;
        pthread_mutex_unlock(data->dist_lock);
        return ;
    }
    if (dist_1 == *curr_distance)
    {
        pthread_mutex_unlock(data->dist_lock);
        return ;
    }
    data->sens_1_last_value = *curr_distance;
    data->sens_1_next_value = dist_1;
    data->sens_1_interp = 0.0f;
    *curr_distance = dist_1;
    pthread_mutex_unlock(data->dist_lock);
}

void get_sensor_2_values(sensor_data_t *data, float *curr_distance)
{
    pthread_mutex_lock(data->dist_lock);
    // float dist_2 = clamp(data->dist_sensor_2, 0.0f, 5.0f);
    float dist_2 = clamp(data->dist_sensor_2, 0.0f, 100.0f);
    if (*curr_distance == -1)
    {
        *curr_distance = dist_2;
        pthread_mutex_unlock(data->dist_lock);
        return ;
    }
    if (dist_2 == *curr_distance)
    {
        pthread_mutex_unlock(data->dist_lock);
        return ;
    }
    data->sens_1_last_value = *curr_distance;
    data->sens_1_next_value = dist_2;
    data->sens_1_interp = 0.0f;
    *curr_distance = dist_2;
    pthread_mutex_unlock(data->dist_lock);
}