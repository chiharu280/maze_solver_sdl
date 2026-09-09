#ifndef VISUALIZE_H
#define VISUALIZE_H

#include "app.h"
#include "maze.h"
#include "ui.h"

typedef enum {
    VISUALIZATION_ERROR = 0,
    VISUALIZATION_FINISHED,
    VISUALIZATION_REPLAY,
    VISUALIZATION_CANCELLED,
    VISUALIZATION_QUIT
} VisualizationResult;

/*
 * Animate the supplied E-to-S path in reverse, from S to E. The logical maze
 * canvas scales with the resizable window without modifying the source maze.
 */
VisualizationResult visualization_play_maze(
    AppContext* app, char maze[][MAX_COLS + 1], const int path_x[],
    const int path_y[], int path_len, UiLanguage language);

#endif
