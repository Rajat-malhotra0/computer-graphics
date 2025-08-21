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

point *dda(int x1, int y1, int x2, int y2, int *total_points)
{
    int iterations, count;
    float slope, dx, dy, x_increment, y_increment;
    float xi = x1, yi = y1;

    dx = x2 - x1;
    dy = y2 - y1;

    if (fabsf(dx) > fabsf(dy))
    {
        iterations = fabsf(dx);
    }
    else
    {
        iterations = fabsf(dy);
    }

    *total_points = iterations + 1;

    x_increment = dx / (float)iterations;
    y_increment = dy / (float)iterations;

    count = 0;
    point *points = (point *)malloc(*total_points * sizeof(point));

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
    return dda(x1, y1, x2, y2, total_points);
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