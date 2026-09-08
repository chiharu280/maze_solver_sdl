#ifndef VISUALIZE_H
#define VISUALIZE_H

#include "maze.h"

typedef enum {
    VISUALIZATION_ERROR = 0,
    VISUALIZATION_FINISHED,
    VISUALIZATION_QUIT
} VisualizationResult;

/* Initialize the SDL presentation layer once for the application lifetime. */
int visualization_init(int maze_width, int maze_height);

/*
 * Animate the supplied E-to-S path in reverse, from S to E. The renderer uses
 * a separate overlay and never modifies the source maze.
 */
VisualizationResult visualization_play_maze(
    char maze[][MAX_COLS + 1], const int path_x[], const int path_y[],
    int path_len);

/* Keep the final frame visible until the window is closed. */
void visualization_wait_for_close(void);

/* Release every SDL resource when the application is ready to exit. */
void visualization_shutdown(void);

#endif
