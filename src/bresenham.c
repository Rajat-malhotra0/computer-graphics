#include <stdlib.h>
#include <math.h>
#include "bresenham.h"

// Helper function to get the absolute value of an integer
static int int_abs(int n) {
    return (n > 0) ? n : -n;
}

// Helper function to get the sign of an integer
static int int_sign(int n) {
    if (n > 0) return 1;
    if (n < 0) return -1;
    return 0;
}

EMSCRIPTEN_KEEPALIVE
void get_bresenham_points_array(int x1, int y1, int x2, int y2, float *output_array, int *total_points) {
    // Bresenham's line algorithm implementation
    int dx = int_abs(x2 - x1);
    int sx = (x1 < x2) ? 1 : -1;
    int dy = -int_abs(y2 - y1);
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx + dy; // error value e_xy
    int e2; // error value e_xy
    int count = 0;

    // Estimate the number of points to prevent infinite loops and for safety.
    // The number of points in Bresenham's is max(|dx|, |dy|) + 1.
    int max_points = fmax(dx, int_abs(dy)) + 1;
    if (max_points > 50000) { // Safety guard
        *total_points = 0;
        return;
    }

    for (;;) {
        // Store the current point as a float, since that's what our interface uses
        output_array[count * 2] = (float)x1;
        output_array[count * 2 + 1] = (float)y1;
        count++;

        if (count >= max_points) break; // Safety break

        if (x1 == x2 && y1 == y2) break;

        e2 = 2 * err;
        if (e2 >= dy) { // e_xy+e_x > 0
            err += dy;
            x1 += sx;
        }
        if (e2 <= dx) { // e_xy+e_y < 0
            err += dx;
            y1 += sy;
        }
    }

    *total_points = count;
}