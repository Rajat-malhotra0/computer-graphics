#ifndef BRESENHAM_H
#define BRESENHAM_H

#include "dda.h" // We can reuse the 'point' struct from dda.h
#include <emscripten.h>

/**
 * @brief Calculates the points for a line using Bresenham's integer-based algorithm
 * and populates a pre-allocated output array with the results.
 *
 * @param x1 The x-coordinate of the starting point.
 * @param y1 The y-coordinate of the starting point.
 * @param x2 The x-coordinate of the ending point.
 * @param y2 The y-coordinate of the ending point.
 * @param output_array A pointer to a float array allocated in JS, to be filled with point data (x,y,x,y,...).
 * @param total_points A pointer to an integer where the total number of calculated points will be stored.
 */
EMSCRIPTEN_KEEPALIVE
void get_bresenham_points_array(int x1, int y1, int x2, int y2, float *output_array, int *total_points);

#endif // BRESENHAM_H