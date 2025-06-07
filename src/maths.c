#include "../include/include.h"

float square(float nb)
{
    return (nb * nb);
}

float get_norm_distance(t_cell *cell, t_cell *center, float max_distance)
{
    float distance_squared = square(cell->x - center->x) + square(cell->y - center->y);
    return (distance_squared / square(max_distance));
}

float clamp(float value, float min, float max)
{
    if (value < min) return min;
    if (value > max) return max;
    return value;
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

