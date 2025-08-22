#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <emscripten.h>

typedef struct
{
    float x;
    float y;
} point;

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
        // Test write access
        test_ptr[0].x = 1.0f;
        test_ptr[0].y = 1.0f;
        if (num_points > 1)
        {
            test_ptr[num_points - 1].x = 2.0f;
            test_ptr[num_points - 1].y = 2.0f;
        }

        free(test_ptr);

        return 1; // Success
    }
    else
    {
        return 0; // Failure
    }
}

point *dda(int x1, int y1, int x2, int y2, int *total_points)
{
    int iterations, count;
    float dx, dy, x_increment, y_increment;
    float xi = x1, yi = y1;

    dx = x2 - x1;
    dy = y2 - y1;

    // Handle the case where start and end points are the same
    if (dx == 0 && dy == 0)
    {
        *total_points = 1;
        point *points = (point *)malloc(sizeof(point));
        if (!points)
        {
            return NULL;
        }
        points[0].x = x1;
        points[0].y = y1;
        return points;
    }

    if (fabsf(dx) > fabsf(dy))
    {
        iterations = (int)fabsf(dx);
    }
    else
    {
        iterations = (int)fabsf(dy);
    }

    *total_points = iterations + 1;

    // Add stricter safety checks
    if (*total_points > 50000 || *total_points < 1 || iterations < 0)
    {
        *total_points = 0;
        return NULL;
    }

    // Calculate memory requirement
    size_t memory_needed = *total_points * sizeof(point);

    // Use a more conservative allocation approach
    point *points = NULL;

    // For small arrays, try simple malloc first
    if (*total_points <= 1000)
    {
        points = (point *)malloc(memory_needed);
    }
    else
    {
        points = (point *)calloc(*total_points, sizeof(point));
    }

    if (!points)
    {

        // Try the opposite allocation method
        if (*total_points <= 1000)
        {
            points = (point *)calloc(*total_points, sizeof(point));
        }
        else
        {
            points = (point *)malloc(memory_needed);
        }
    }

    if (!points)
    {

        // Try allocating in smaller chunks to test memory availability
        for (int test_size = *total_points; test_size > 0; test_size /= 2)
        {
            size_t test_bytes = test_size * sizeof(point);
            point *test_ptr = (point *)malloc(test_bytes);
            if (test_ptr)
            {
                free(test_ptr);

                // If we can allocate half the size, try allocating the full size again
                if (test_size >= *total_points / 2)
                {
                    points = (point *)malloc(memory_needed);
                    if (points)
                    {
                        break;
                    }
                }
                break;
            }
            else
            {
            }
        }

        if (!points)
        {
            *total_points = 0;
            return NULL;
        }
    }

    // Test write access to allocated memory with bounds checking
    if (points && *total_points > 0)
    {
        // Test writing to first element
        points[0].x = 0.0f;
        points[0].y = 0.0f;

        // Test writing to last element (with bounds check)
        if (*total_points > 1)
        {
            points[*total_points - 1].x = 0.0f;
            points[*total_points - 1].y = 0.0f;
        }

        // Initialize all allocated memory to zero with bounds checking
        for (int i = 0; i < *total_points; i++)
        {
            points[i].x = 0.0f;
            points[i].y = 0.0f;
        }
    }

    // Calculate increments with safer division
    if (iterations > 0)
    {
        x_increment = dx / (float)iterations;
        y_increment = dy / (float)iterations;
    }
    else
    {
        x_increment = 0.0f;
        y_increment = 0.0f;
    }

    count = 0;

    // Bounds check before writing
    if (count < *total_points)
    {
        points[count].x = xi;
        points[count].y = yi;
        count++;
    }

    while (count < *total_points && iterations > 0)
    {
        xi += x_increment;
        yi += y_increment;

        // Bounds check before writing
        if (count < *total_points)
        {
            points[count].x = xi;
            points[count].y = yi;
            count++;
        }
        else
        {
            break;
        }
    }

    // Update total_points to actual count
    *total_points = count;

    return points;
}

EMSCRIPTEN_KEEPALIVE
float calculate_slope(int x1, int y1, int x2, int y2)
{
    float dx = x2 - x1;
    float dy = y2 - y1;

    if (dx == 0)
    {
        // Vertical line - return a large number to indicate infinite slope
        return (dy >= 0) ? 999999.0f : -999999.0f;
    }

    return dy / dx;
}

EMSCRIPTEN_KEEPALIVE
void get_dda_points_array(int x1, int y1, int x2, int y2, float *output_array, int *total_points)
{
    // Add stricter input validation for deployment
    if (abs(x1) > 50000 || abs(y1) > 50000 ||
        abs(x2) > 50000 || abs(y2) > 50000)
    {
        *total_points = 0;
        return;
    }

    // Calculate expected number of points and validate
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int expected_points = (dx > dy ? dx : dy) + 1;

    if (expected_points > 50000)
    {
        *total_points = 0;
        return;
    }

    // Get points using existing DDA function
    point *result = dda(x1, y1, x2, y2, total_points);

    if (result && *total_points > 0)
    {

        // Copy points to output array (x, y, x, y, ...)
        for (int i = 0; i < *total_points; i++)
        {
            output_array[i * 2] = result[i].x;     // x coordinate
            output_array[i * 2 + 1] = result[i].y; // y coordinate
        }

        // Free the temporary points array
        free(result);
    }
    else
    {
        *total_points = 0;
    }
}
int main()
{
    int x1, y1, x2, y2;
    int total_points;

#ifndef __EMSCRIPTEN__

    scanf("%d %d", &x1, &y1);

    scanf("%d %d", &x2, &y2);

    // Calculate and display slope
    float slope = calculate_slope(x1, y1, x2, y2);
    if (fabsf(slope) > 999998.0f)
    {
    }

    point *points = dda(x1, y1, x2, y2, &total_points);

    for (int i = 0; i < total_points; i++)
    {
    }

    free(points);
#else
    // WASM mode - functions are called from JavaScript
#endif
    return 0;
}