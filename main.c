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
    float slope, dx, dy, inverse;
    float xi = x1, yi = y1;

    dx = x2 - x1;
    dy = y2 - y1;

    slope = dy / dx;

    if (fabsf(dx) > fabsf(dy))
    {
        iterations = fabsf(dx);
    }
    else
    {
        iterations = fabsf(dy);
    }

    *total_points = iterations + 1;
    count = 0;
    inverse = 1 / slope;
    point *points = (point *)malloc(*total_points * sizeof(point));

    points[count].x = xi;
    points[count].y = yi;
    count++;

    while (xi < x2 || yi < y2)
    {
        if (fabsf(slope) >= 1 && slope >= 0)
        {
            xi = xi + inverse;
            yi = yi + 1;
        }
        else if (fabsf(slope) >= 1 && slope < 0)
        {
            xi = xi + inverse;
            yi = yi - 1;
        }
        else if (fabsf(slope) < 1 && slope >= 0)
        {
            xi = xi + 1;
            yi = yi + slope;
        }
        else if (fabsf(slope) < 1 && slope < 0)
        {
            xi = xi + 1;
            yi = yi - fabsf(slope);
        }
        points[count].x = xi;
        points[count].y = yi;
        count++;
    }

    return points;
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