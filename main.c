#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

typedef struct
{
    float x;
    float y;
} point;

#ifdef __EMSCRIPTEN__
EMSCRIPTEN_KEEPALIVE
#endif
int get_point_size()
{
    return sizeof(point);
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
            return NULL;
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

    // Add safety check for excessive iterations
    if (*total_points > 100000 || *total_points < 1)
    {
        *total_points = 0;
        return NULL;
    }

    // Allocate memory with error checking
    point *points = (point *)malloc(*total_points * sizeof(point));
    if (!points)
    {
        *total_points = 0;
        return NULL;
    }

    // Initialize allocated memory to zero
    for (int i = 0; i < *total_points; i++)
    {
        points[i].x = 0.0f;
        points[i].y = 0.0f;
    }

    x_increment = dx / (float)iterations;
    y_increment = dy / (float)iterations;

    count = 0;
    points[count].x = xi;
    points[count].y = yi;
    count++;

    while (count < *total_points)
    {
        xi += x_increment;
        yi += y_increment;

        points[count].x = xi;
        points[count].y = yi;
        count++;
    }

    return points;
}

#ifdef __EMSCRIPTEN__
EMSCRIPTEN_KEEPALIVE
#endif
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

#ifdef __EMSCRIPTEN__
EMSCRIPTEN_KEEPALIVE
#endif
point *calculate_dda(int x1, int y1, int x2, int y2, int *total_points)
{
    // Add stricter input validation for deployment
    if (abs(x1) > 50000 || abs(y1) > 50000 ||
        abs(x2) > 50000 || abs(y2) > 50000)
    {
        *total_points = 0;
        return NULL;
    }

    // Calculate expected number of points and validate
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int expected_points = (dx > dy ? dx : dy) + 1;

    if (expected_points > 50000)
    {
        *total_points = 0;
        return NULL;
    }

// Debug output for WebAssembly
#ifdef __EMSCRIPTEN__
    printf("WASM Debug: Calculating DDA from (%d,%d) to (%d,%d)\n", x1, y1, x2, y2);
    printf("WASM Debug: Expected points: %d\n", expected_points);
#endif

    point *result = dda(x1, y1, x2, y2, total_points);

#ifdef __EMSCRIPTEN__
    printf("WASM Debug: Result pointer: %p, total_points: %d\n", result, *total_points);
    if (result && *total_points > 0)
    {
        printf("WASM Debug: First point: (%.2f, %.2f)\n", result[0].x, result[0].y);
        printf("WASM Debug: Last point: (%.2f, %.2f)\n", result[*total_points - 1].x, result[*total_points - 1].y);
    }
#endif

    return result;
}
int main()
{
    int x1, y1, x2, y2;
    int total_points;

#ifndef __EMSCRIPTEN__
    printf("DDA Line Drawing Algorithm\n");
    printf("==========================\n");

    printf("Enter starting point (x1, y1): ");
    scanf("%d %d", &x1, &y1);

    printf("Enter ending point (x2, y2): ");
    scanf("%d %d", &x2, &y2);

    // Calculate and display slope
    float slope = calculate_slope(x1, y1, x2, y2);
    printf("\nSlope: %.6f\n", slope);
    if (fabsf(slope) > 999998.0f)
    {
        printf("(Vertical line - infinite slope)\n");
    }

    point *points = dda(x1, y1, x2, y2, &total_points);

    printf("\nLine from (%d, %d) to (%d, %d)\n", x1, y1, x2, y2);
    printf("================================\n");
    printf("| Step |    Xi    |    Yi    |\n");
    printf("|------|----------|----------|\n");

    for (int i = 0; i < total_points; i++)
    {
        printf("| %4d | %8.2f | %8.2f |\n", i, points[i].x, points[i].y);
    }

    printf("================================\n");

    free(points);
#else
    // WASM mode - functions are called from JavaScript
    printf("DDA Algorithm ready for WASM\n");
#endif
    return 0;
}