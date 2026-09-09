#ifndef MENU_H
#define MENU_H

#include "app.h"
#include "ui.h"

typedef enum {
    MENU_ERROR = 0,
    MENU_START,
    MENU_NEW_MAZE,
    MENU_QUIT
} MenuAction;

typedef enum {
    MAZE_SIZE_ERROR = 0,
    MAZE_SIZE_CONFIRMED,
    MAZE_SIZE_CANCELLED,
    MAZE_SIZE_QUIT
} MazeSizeResult;

/* Display the start screen until the user chooses an action. */
MenuAction menu_run(AppContext* app, UiStatus status, UiLanguage* language);

/* Collect and validate odd maze dimensions before generating a new maze. */
MazeSizeResult menu_prompt_maze_size(AppContext* app, int initial_width,
                                     int initial_height, int* width,
                                     int* height, UiLanguage language);

#endif
