#ifndef MENU_H
#define MENU_H

#include "app.h"
#include "ui.h"

typedef enum {
    MENU_ERROR = 0,
    MENU_START,
    MENU_NEW_MAZE,
    MENU_LANGUAGE,
    MENU_QUIT
} MenuAction;

typedef enum {
    MAZE_SIZE_ERROR = 0,
    MAZE_SIZE_CONFIRMED,
    MAZE_SIZE_CANCELLED,
    MAZE_SIZE_QUIT
} MazeSizeResult;

typedef enum {
    LANGUAGE_SELECTION_ERROR = 0,
    LANGUAGE_SELECTION_SELECTED,
    LANGUAGE_SELECTION_CANCELLED,
    LANGUAGE_SELECTION_QUIT
} LanguageSelectionResult;

/* Display the start screen until the user chooses an action. */
MenuAction menu_run(AppContext* app, UiStatus status, UiLanguage* language);

/* Display a separate screen for choosing Chinese, English, or French. */
LanguageSelectionResult menu_prompt_language(AppContext* app,
                                             UiLanguage* language);

/* Collect and validate odd maze dimensions before generating a new maze. */
MazeSizeResult menu_prompt_maze_size(AppContext* app, int initial_width,
                                     int initial_height, int* width,
                                     int* height, UiLanguage language);

#endif
