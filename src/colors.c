#include "../include/include.h"

int	create_trgb(int t, int r, int g, int b)
{
	return (t << 24 | r << 16 | g << 8 | b);
}

int	get_t(int trgb)
{
	return ((trgb >> 24) & 0xFF);
}

int	get_r(int trgb)
{
	return ((trgb >> 16) & 0xFF);
}

int	get_g(int trgb)
{
	return ((trgb >> 8) & 0xFF);
}

int	get_b(int trgb)
{
	return (trgb & 0xFF);
}

void	palette_one(int *colors)
{
	colors[0] = create_trgb(0, 250, 190, 11);
	colors[1] = create_trgb(0, 251, 86, 7);
	colors[2] = create_trgb(0, 255, 0, 110);
	colors[3] = create_trgb(0, 131, 56, 236);
	colors[4] = create_trgb(0, 58, 134, 255);
}

void	palette_two(int *colors)
{
	colors[0] = create_trgb(0, 155, 93, 229);
	colors[1] = create_trgb(0, 241, 91, 181);
	colors[2] = create_trgb(0, 254, 228, 64);
	colors[3] = create_trgb(0, 0, 187, 249);
	colors[4] = create_trgb(0, 0, 245, 212);
}

void	palette_three(int *colors)
{
	colors[0] = create_trgb(0, 157, 2, 8);
	colors[1] = create_trgb(0, 220, 47, 2);
	colors[2] = create_trgb(0, 254, 228, 64);
	colors[3] = create_trgb(0, 232, 93, 4);
	colors[4] = create_trgb(0, 255, 186, 8);
}

void	palette_four(int *colors)
{
	colors[0] = create_trgb(0, 33, 5, 53);
	colors[1] = create_trgb(0, 67, 13, 75);
	colors[2] = create_trgb(0, 123, 51, 125);
	colors[3] = create_trgb(0, 200, 116, 178);
	colors[4] = create_trgb(0, 245, 213, 224);
}

int	interpolate_color(int color_a, int color_b, float t)
{
	int	r;
	int	g;
	int	b;

	r = get_r(color_a) * (1 - t) + get_r(color_b) * t;
	g = get_g(color_a) * (1 - t) + get_g(color_b) * t;
	b = get_b(color_a) * (1 - t) + get_b(color_b) * t;
	return (create_trgb(0, r, g, b));
}

float	normalize_value(float value, float min, float max)
{
    return ((value - min) / (max - min));
}

int	get_color_gradient(float iter, int *colors, int nb_colors)
{
    if (nb_colors == 0)
    {
        iter = 1 - fabs(0.5 - iter) * 2;
        return (interpolate_color(colors[0], colors[1], iter));
    }
    float color_seg;
    int   color;

    nb_colors += 2;
    if (nb_colors > 5)
        nb_colors = 5;
    color_seg = (float)1 / (float)nb_colors;
    if (iter >= color_seg * (nb_colors - 1))
        return (interpolate_color(colors[nb_colors - 1], colors[0], normalize_value(iter, color_seg * (nb_colors - 1), 1)));
    for (int i = 0; i < nb_colors; i++)
    {
        if (iter >= color_seg * i && iter < color_seg * (i + 1))
        {
            color = interpolate_color(colors[i], colors[i + 1], normalize_value(iter, color_seg * i, color_seg * (float)(i + 1)));
            return (color);
        }
    }
    return (0);
}

int wheel(__uint8_t pos) 
{
    __uint8_t r;
    __uint8_t g;
    __uint8_t b;

    pos = 255 - pos;
    if (pos < 85)
    {
        r = 255 - pos * 3;
        g = 0;
        b = pos * 3;
    }
    else if (pos < 170) 
    {
        pos = pos - 85;
        r = 0;
        g = pos * 3;
        b = 255 - pos * 3;
    }
    else
    {
        pos = pos - 170;
        r = pos * 3;
        g = 255 - pos * 3;
        b = 0;
    }
    return (r << 16 | g << 8 | b);
}
