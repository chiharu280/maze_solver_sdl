#ifndef MENU_H
#define MENU_H

#include "app.h"

typedef enum {
    MENU_ERROR = 0,
    MENU_START,
    MENU_NEW_MAZE,
    MENU_QUIT
} MenuAction;

/* Display the start screen until the user chooses an action. */
MenuAction menu_run(AppContext* app, const char* status_message);

#endif
