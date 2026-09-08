#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "app.h"
#include "menu.h"

#define MENU_WIDTH 800
#define MENU_HEIGHT 600

typedef struct {
    char character;
    uint8_t rows[7];
} Glyph;

typedef struct {
    SDL_Rect bounds;
    const char* label;
    MenuAction action;
} MenuButton;

static const Glyph FONT[] = {
    {'0', {14, 17, 19, 21, 25, 17, 14}},
    {'1', {4, 12, 4, 4, 4, 4, 14}},
    {'2', {14, 17, 1, 2, 4, 8, 31}},
    {'3', {30, 1, 1, 14, 1, 1, 30}},
    {'4', {2, 6, 10, 18, 31, 2, 2}},
    {'5', {31, 16, 16, 30, 1, 1, 30}},
    {'6', {14, 16, 16, 30, 17, 17, 14}},
    {'7', {31, 1, 2, 4, 8, 8, 8}},
    {'8', {14, 17, 17, 14, 17, 17, 14}},
    {'9', {14, 17, 17, 15, 1, 1, 14}},
    {'A', {14, 17, 17, 31, 17, 17, 17}},
    {'B', {30, 17, 17, 30, 17, 17, 30}},
    {'C', {14, 17, 16, 16, 16, 17, 14}},
    {'D', {30, 17, 17, 17, 17, 17, 30}},
    {'E', {31, 16, 16, 30, 16, 16, 31}},
    {'F', {31, 16, 16, 30, 16, 16, 16}},
    {'G', {14, 17, 16, 23, 17, 17, 15}},
    {'H', {17, 17, 17, 31, 17, 17, 17}},
    {'I', {14, 4, 4, 4, 4, 4, 14}},
    {'J', {7, 2, 2, 2, 2, 18, 12}},
    {'K', {17, 18, 20, 24, 20, 18, 17}},
    {'L', {16, 16, 16, 16, 16, 16, 31}},
    {'M', {17, 27, 21, 21, 17, 17, 17}},
    {'N', {17, 25, 21, 19, 17, 17, 17}},
    {'O', {14, 17, 17, 17, 17, 17, 14}},
    {'P', {30, 17, 17, 30, 16, 16, 16}},
    {'Q', {14, 17, 17, 17, 21, 18, 13}},
    {'R', {30, 17, 17, 30, 20, 18, 17}},
    {'S', {15, 16, 16, 14, 1, 1, 30}},
    {'T', {31, 4, 4, 4, 4, 4, 4}},
    {'U', {17, 17, 17, 17, 17, 17, 14}},
    {'V', {17, 17, 17, 17, 17, 10, 4}},
    {'W', {17, 17, 17, 21, 21, 21, 10}},
    {'X', {17, 17, 10, 4, 10, 17, 17}},
    {'Y', {17, 17, 10, 4, 4, 4, 4}},
    {'Z', {31, 1, 2, 4, 8, 16, 31}}
};

static const uint8_t* find_glyph(char character) {
    for (size_t i = 0; i < sizeof(FONT) / sizeof(FONT[0]); ++i) {
        if (FONT[i].character == character) {
            return FONT[i].rows;
        }
    }
    return NULL;
}

static int text_width(const char* text, int scale) {
    size_t length = strlen(text);
    return length == 0 ? 0 : (int)(length * 6 - 1) * scale;
}

static void draw_text(AppContext* app, const char* text, int center_x, int y,
                      int scale, SDL_Color color) {
    int x = center_x - text_width(text, scale) / 2;
    SDL_SetRenderDrawColor(app->renderer, color.r, color.g, color.b, color.a);

    for (const char* current = text; *current; ++current, x += 6 * scale) {
        const uint8_t* rows = find_glyph(*current);
        if (!rows) {
            continue;
        }
        for (int row = 0; row < 7; ++row) {
            for (int column = 0; column < 5; ++column) {
                if (rows[row] & (1u << (4 - column))) {
                    SDL_Rect pixel = {x + column * scale, y + row * scale,
                                      scale, scale};
                    SDL_RenderFillRect(app->renderer, &pixel);
                }
            }
        }
    }
}

static int point_in_rect(float x, float y, const SDL_Rect* rect) {
    return x >= rect->x && x < rect->x + rect->w &&
           y >= rect->y && y < rect->y + rect->h;
}

static void render_menu(AppContext* app, const MenuButton buttons[],
                        size_t button_count, int hovered,
                        const char* status_message) {
    SDL_SetRenderDrawColor(app->renderer, 12, 18, 32, 255);
    SDL_RenderClear(app->renderer);

    SDL_SetRenderDrawColor(app->renderer, 20, 31, 52, 255);
    for (int x = 0; x < MENU_WIDTH; x += 32) {
        SDL_RenderDrawLine(app->renderer, x, 0, x, MENU_HEIGHT);
    }
    for (int y = 0; y < MENU_HEIGHT; y += 32) {
        SDL_RenderDrawLine(app->renderer, 0, y, MENU_WIDTH, y);
    }

    draw_text(app, "MAZE SOLVER", MENU_WIDTH / 2, 78, 8,
              (SDL_Color){97, 218, 251, 255});
    draw_text(app, "FIND THE SHORTEST PATH", MENU_WIDTH / 2, 162, 3,
              (SDL_Color){148, 163, 184, 255});

    for (size_t i = 0; i < button_count; ++i) {
        SDL_Rect shadow = buttons[i].bounds;
        shadow.x += 5;
        shadow.y += 6;
        SDL_SetRenderDrawColor(app->renderer, 3, 7, 18, 180);
        SDL_RenderFillRect(app->renderer, &shadow);

        if ((int)i == hovered) {
            SDL_SetRenderDrawColor(app->renderer, 37, 99, 235, 255);
        } else {
            SDL_SetRenderDrawColor(app->renderer, 30, 41, 59, 255);
        }
        SDL_RenderFillRect(app->renderer, &buttons[i].bounds);

        SDL_SetRenderDrawColor(app->renderer, 96, 165, 250, 255);
        SDL_RenderDrawRect(app->renderer, &buttons[i].bounds);
        draw_text(app, buttons[i].label, MENU_WIDTH / 2,
                  buttons[i].bounds.y + 20, 4,
                  (SDL_Color){241, 245, 249, 255});
    }

    if (status_message && status_message[0] != '\0') {
        draw_text(app, status_message, MENU_WIDTH / 2, 535, 3,
                  (SDL_Color){134, 239, 172, 255});
    }
    draw_text(app, "RESIZE THE WINDOW FREELY", MENU_WIDTH / 2, 570, 2,
              (SDL_Color){100, 116, 139, 255});

    SDL_RenderPresent(app->renderer);
}

MenuAction menu_run(AppContext* app, const char* status_message) {
    static const MenuButton buttons[] = {
        {{250, 235, 300, 68}, "START", MENU_START},
        {{250, 330, 300, 68}, "NEW MAZE", MENU_NEW_MAZE},
        {{250, 425, 300, 68}, "QUIT", MENU_QUIT}
    };
    const size_t button_count = sizeof(buttons) / sizeof(buttons[0]);

    if (!app || !app->renderer || !app_set_logical_size(app, MENU_WIDTH,
                                                         MENU_HEIGHT)) {
        return MENU_ERROR;
    }
    SDL_SetWindowTitle(app->window, "Maze Solver - Menu");

    int hovered = -1;
    for (;;) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                return MENU_QUIT;
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE ||
                    event.key.keysym.sym == SDLK_q) {
                    return MENU_QUIT;
                }
                if (event.key.keysym.sym == SDLK_RETURN ||
                    event.key.keysym.sym == SDLK_SPACE) {
                    return MENU_START;
                }
                if (event.key.keysym.sym == SDLK_n) {
                    return MENU_NEW_MAZE;
                }
            }
            if (event.type == SDL_MOUSEMOTION) {
                hovered = -1;
                for (size_t i = 0; i < button_count; ++i) {
                    if (point_in_rect((float)event.motion.x,
                                      (float)event.motion.y,
                                      &buttons[i].bounds)) {
                        hovered = (int)i;
                        break;
                    }
                }
            }
            if (event.type == SDL_MOUSEBUTTONDOWN &&
                event.button.button == SDL_BUTTON_LEFT) {
                for (size_t i = 0; i < button_count; ++i) {
                    if (point_in_rect((float)event.button.x,
                                      (float)event.button.y,
                                      &buttons[i].bounds)) {
                        return buttons[i].action;
                    }
                }
            }
        }

        render_menu(app, buttons, button_count, hovered, status_message);
        SDL_Delay(16);
    }
}

static int dimensions_are_valid(int width, int height) {
    return width >= 3 && width <= 99 && height >= 3 && height <= 99 &&
           width % 2 != 0 && height % 2 != 0 &&
           (width != 3 || height != 3);
}

static int parse_dimension(const char* text, int* value) {
    char* end = NULL;
    long parsed = strtol(text, &end, 10);

    if (end == text || *end != '\0' || parsed < 0 || parsed > 999) {
        return 0;
    }
    *value = (int)parsed;
    return 1;
}

static void append_digit(char text[4], char digit) {
    size_t length = strlen(text);

    if (length < 3) {
        text[length] = digit;
        text[length + 1] = '\0';
    }
}

static void delete_digit(char text[4]) {
    size_t length = strlen(text);

    if (length > 0) {
        text[length - 1] = '\0';
    }
}

static void draw_input(AppContext* app, const SDL_Rect* bounds,
                       const char* label, const char* value, int focused) {
    draw_text(app, label, bounds->x + bounds->w / 2, bounds->y - 38, 3,
              (SDL_Color){148, 163, 184, 255});
    SDL_SetRenderDrawColor(app->renderer, 15, 23, 42, 255);
    SDL_RenderFillRect(app->renderer, bounds);
    if (focused) {
        SDL_SetRenderDrawColor(app->renderer, 96, 165, 250, 255);
    } else {
        SDL_SetRenderDrawColor(app->renderer, 71, 85, 105, 255);
    }
    SDL_RenderDrawRect(app->renderer, bounds);
    draw_text(app, value, bounds->x + bounds->w / 2, bounds->y + 16, 5,
              (SDL_Color){241, 245, 249, 255});
}

static void draw_size_prompt(AppContext* app, const char* width_text,
                             const char* height_text, int focused,
                             const char* validation_message) {
    const SDL_Rect width_input = {180, 230, 180, 70};
    const SDL_Rect height_input = {440, 230, 180, 70};
    const SDL_Rect generate_button = {155, 400, 220, 68};
    const SDL_Rect cancel_button = {425, 400, 220, 68};

    SDL_SetRenderDrawColor(app->renderer, 12, 18, 32, 255);
    SDL_RenderClear(app->renderer);
    draw_text(app, "NEW MAZE", MENU_WIDTH / 2, 70, 8,
              (SDL_Color){97, 218, 251, 255});
    draw_text(app, "ODD SIZE FROM 3 TO 99", MENU_WIDTH / 2, 160, 3,
              (SDL_Color){148, 163, 184, 255});

    draw_input(app, &width_input, "WIDTH", width_text, focused == 0);
    draw_input(app, &height_input, "HEIGHT", height_text, focused == 1);

    SDL_SetRenderDrawColor(app->renderer, 37, 99, 235, 255);
    SDL_RenderFillRect(app->renderer, &generate_button);
    SDL_SetRenderDrawColor(app->renderer, 71, 85, 105, 255);
    SDL_RenderFillRect(app->renderer, &cancel_button);
    draw_text(app, "GENERATE", generate_button.x + generate_button.w / 2,
              generate_button.y + 20, 4,
              (SDL_Color){241, 245, 249, 255});
    draw_text(app, "CANCEL", cancel_button.x + cancel_button.w / 2,
              cancel_button.y + 20, 4,
              (SDL_Color){241, 245, 249, 255});

    draw_text(app, validation_message, MENU_WIDTH / 2, 340, 2,
              (SDL_Color){248, 113, 113, 255});
    draw_text(app, "TAB SWITCHES FIELD", MENU_WIDTH / 2, 535, 2,
              (SDL_Color){100, 116, 139, 255});
    SDL_RenderPresent(app->renderer);
}

MazeSizeResult menu_prompt_maze_size(AppContext* app, int initial_width,
                                     int initial_height, int* width,
                                     int* height) {
    const SDL_Rect width_input = {180, 230, 180, 70};
    const SDL_Rect height_input = {440, 230, 180, 70};
    const SDL_Rect generate_button = {155, 400, 220, 68};
    const SDL_Rect cancel_button = {425, 400, 220, 68};
    char width_text[4];
    char height_text[4];
    const char* validation_message = "";
    int focused = 0;
    int replace_on_input = 1;
    MazeSizeResult result = MAZE_SIZE_ERROR;

    if (!app || !app->renderer || !width || !height ||
        !dimensions_are_valid(initial_width, initial_height) ||
        !app_set_logical_size(app, MENU_WIDTH, MENU_HEIGHT)) {
        return MAZE_SIZE_ERROR;
    }
    snprintf(width_text, sizeof(width_text), "%d", initial_width);
    snprintf(height_text, sizeof(height_text), "%d", initial_height);
    SDL_SetWindowTitle(app->window, "Maze Solver - New Maze");
    SDL_StartTextInput();

    while (result == MAZE_SIZE_ERROR) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            int confirm = 0;
            if (event.type == SDL_QUIT) {
                result = MAZE_SIZE_QUIT;
                break;
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    result = MAZE_SIZE_CANCELLED;
                    break;
                }
                if (event.key.keysym.sym == SDLK_TAB) {
                    focused = 1 - focused;
                    replace_on_input = 1;
                } else if (event.key.keysym.sym == SDLK_BACKSPACE) {
                    delete_digit(focused == 0 ? width_text : height_text);
                    replace_on_input = 0;
                } else if (event.key.keysym.sym == SDLK_RETURN ||
                           event.key.keysym.sym == SDLK_KP_ENTER) {
                    confirm = 1;
                }
            }
            if (event.type == SDL_TEXTINPUT) {
                for (const char* current = event.text.text; *current; ++current) {
                    if (*current >= '0' && *current <= '9') {
                        if (replace_on_input) {
                            (focused == 0 ? width_text : height_text)[0] = '\0';
                            replace_on_input = 0;
                        }
                        append_digit(focused == 0 ? width_text : height_text,
                                     *current);
                    }
                }
            }
            if (event.type == SDL_MOUSEBUTTONDOWN &&
                event.button.button == SDL_BUTTON_LEFT) {
                float x = (float)event.button.x;
                float y = (float)event.button.y;
                if (point_in_rect(x, y, &width_input)) {
                    focused = 0;
                    replace_on_input = 1;
                } else if (point_in_rect(x, y, &height_input)) {
                    focused = 1;
                    replace_on_input = 1;
                } else if (point_in_rect(x, y, &generate_button)) {
                    confirm = 1;
                } else if (point_in_rect(x, y, &cancel_button)) {
                    result = MAZE_SIZE_CANCELLED;
                    break;
                }
            }

            if (confirm) {
                int parsed_width;
                int parsed_height;
                if (parse_dimension(width_text, &parsed_width) &&
                    parse_dimension(height_text, &parsed_height) &&
                    dimensions_are_valid(parsed_width, parsed_height)) {
                    *width = parsed_width;
                    *height = parsed_height;
                    result = MAZE_SIZE_CONFIRMED;
                    break;
                }
                validation_message = "ODD 3 TO 99 AND NOT 3 X 3";
            }
        }

        if (result == MAZE_SIZE_ERROR) {
            draw_size_prompt(app, width_text, height_text, focused,
                             validation_message);
            SDL_Delay(16);
        }
    }

    SDL_StopTextInput();
    return result;
}
