#ifndef UI_H
#define UI_H

#include <SDL2/SDL.h>

#include "app.h"

typedef enum {
    UI_LANGUAGE_ENGLISH = 0,
    UI_LANGUAGE_CHINESE,
    UI_LANGUAGE_FRENCH
} UiLanguage;

typedef enum {
    UI_STATUS_READY = 0,
    UI_STATUS_MAZE_LOAD_FAILED,
    UI_STATUS_NEW_MAZE_GENERATED,
    UI_STATUS_MAZE_GENERATION_FAILED,
    UI_STATUS_GENERATION_CANCELLED,
    UI_STATUS_BACK_TO_MENU,
    UI_STATUS_NO_PATH_FOUND
} UiStatus;

const char* ui_status_text(UiLanguage language, UiStatus status);
int ui_text_width(const char* text, int scale);
void ui_draw_text(AppContext* app, const char* text, int center_x, int y,
                  int scale, SDL_Color color);

#endif
