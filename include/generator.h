#ifndef GENERATOR_H
#define GENERATOR_H

#include <stdint.h>

#include "maze.h"

/*
 * Build a perfect maze using randomized iterative depth-first search.
 * Width and height must be odd, at least 3, and within the maze limits.
 */
int generate_perfect_maze(char output[][MAX_COLS + 1], int width, int height,
                          uint32_t seed);

/* Generate a perfect maze and save it in the format accepted by maze.c. */
int generate_maze_file(const char* filename, int width, int height,
                       uint32_t seed);

#endif
