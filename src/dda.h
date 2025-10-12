#ifndef DDA_H
#define DDA_H

#include <emscripten.h>

// Define the point struct, which is used by the DDA function.
typedef struct
{
    float x;
    float y;
} point;

/**
 * @brief Returns the size of the 'point' struct.
 * Exported to JS for introspection.
 */
EMSCRIPTEN_KEEPALIVE
int get_point_size();

/**
 * @brief A diagnostic function to test if a certain amount of memory can be allocated.
 * Exported to JS for debugging.
 * @param num_points The number of points to try allocating memory for.
 * @return 1 on success, 0 on failure.
 */
EMSCRIPTEN_KEEPALIVE
int test_memory_allocation(int num_points);

/**
 * @brief Calculates the slope of a line given two points.
 * Exported to JS.
 * @param x1 The x-coordinate of the starting point.
 * @param y1 The y-coordinate of the starting point.
 * @param x2 The x-coordinate of the ending point.
 * @param y2 The y-coordinate of the ending point.
 * @return The slope of the line. Returns a large sentinel value for vertical lines.
 */
EMSCRIPTEN_KEEPALIVE
float calculate_slope(int x1, int y1, int x2, int y2);

/**
 * @brief The core DDA algorithm implementation.
 * This function calculates the points for a line using the DDA algorithm and
 * populates a pre-allocated output array with the results.
 *
 * @param x1 The x-coordinate of the starting point.
 * @param y1 The y-coordinate of the starting point.
 * @param x2 The x-coordinate of the ending point.
 * @param y2 The y-coordinate of the ending point.
 * @param output_array A pointer to a float array allocated in JS, to be filled with point data (x,y,x,y,...).
 * @param total_points A pointer to an integer where the total number of calculated points will be stored.
 */
EMSCRIPTEN_KEEPALIVE
void get_dda_points_array(int x1, int y1, int x2, int y2, float *output_array, int *total_points);

#endif // DDA_H