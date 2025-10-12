#include "dda.h"

/**
 * @brief The main entry point of the C program.
 *
 * In a standard C application, this function would contain the primary logic.
 * However, when compiling to a WebAssembly module with Emscripten for use as a library,
 * the `main` function is often not strictly necessary, as JavaScript will directly call
 * the exported functions (e.g., `get_dda_points_array`).
 *
 * This `main` function is kept as a standard entry point, which can be useful for
 * native builds, testing, or if the module were to be run in a command-line environment.
 * For our web-based use case, it serves primarily as a formality.
 *
 * @return 0 on successful execution.
 */
int main()
{
    // When compiled for WebAssembly as a library, this function is not actively used.
    // The exported functions in dda.c are called directly from JavaScript.
    // This provides a clean entry point for potential future native builds.
    return 0;
}