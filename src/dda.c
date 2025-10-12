#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "dda.h"

// This is a private helper function, so it's not declared in the header.
static point *dda_calculate(int x1, int y1, int x2, int y2, int *total_points)
{
    int iterations, count;
    float dx, dy, x_increment, y_increment;
    float xi = x1, yi = y1;

    dx = x2 - x1;
    dy = y2 - y1;

    if (dx == 0 && dy == 0)
    {
        *total_points = 1;
        point *points = (point *)malloc(sizeof(point));
        if (!points) return NULL;
        points[0].x = x1;
        points[0].y = y1;
        return points;
    }

    iterations = (int)fmax(fabsf(dx), fabsf(dy));
    *total_points = iterations + 1;

    if (*total_points > 50000 || *total_points < 1)
    {
        *total_points = 0;
        return NULL;
    }

    point *points = (point *)calloc(*total_points, sizeof(point));
    if (!points)
    {
        *total_points = 0;
        return NULL;
    }

    x_increment = (iterations > 0) ? dx / (float)iterations : 0.0f;
    y_increment = (iterations > 0) ? dy / (float)iterations : 0.0f;

    for (count = 0; count <= iterations; count++)
    {
        if (count < *total_points)
        {
            points[count].x = xi;
            points[count].y = yi;
        }
        xi += x_increment;
        yi += y_increment;
    }

    *total_points = count;
    return points;
}

EMSCRIPTEN_KEEPALIVE
int get_point_size()
{
    return sizeof(point);
}

EMSCRIPTEN_KEEPALIVE
int test_memory_allocation(int num_points)
{
    size_t memory_needed = num_points * sizeof(point);
    point *test_ptr = (point *)malloc(memory_needed);
    if (test_ptr)
    {
        free(test_ptr);
        return 1; // Success
    }
    return 0; // Failure
}

EMSCRIPTEN_KEEPALIVE
float calculate_slope(int x1, int y1, int x2, int y2)
{
    float dx = x2 - x1;
    float dy = y2 - y1;

    if (dx == 0)
    {
        return (dy >= 0) ? 999999.0f : -999999.0f;
    }
    return dy / dx;
}

EMSCRIPTEN_KEEPALIVE
void get_dda_points_array(int x1, int y1, int x2, int y2, float *output_array, int *total_points)
{
    if (abs(x1) > 50000 || abs(y1) > 50000 || abs(x2) > 50000 || abs(y2) > 50000)
    {
        *total_points = 0;
        return;
    }

    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int expected_points = fmax(dx, dy) + 1;

    if (expected_points > 50000)
    {
        *total_points = 0;
        return;
    }

    point *result = dda_calculate(x1, y1, x2, y2, total_points);

    if (result && *total_points > 0)
    {
        for (int i = 0; i < *total_points; i++)
        {
            output_array[i * 2] = result[i].x;
            output_array[i * 2 + 1] = result[i].y;
        }
        free(result);
    }
    else
    {
        *total_points = 0;
    }
}