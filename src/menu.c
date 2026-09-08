#include <SDL2/SDL.h>
#include <stdint.h>
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

static void window_to_logical(AppContext* app, int window_x, int window_y,
                              float* logical_x, float* logical_y) {
    SDL_RenderWindowToLogical(app->renderer, window_x, window_y,
                              logical_x, logical_y);
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

    for (;;) {
        int window_x;
        int window_y;
        float mouse_x;
        float mouse_y;
        int hovered = -1;
        SDL_Event event;

        SDL_GetMouseState(&window_x, &window_y);
        window_to_logical(app, window_x, window_y, &mouse_x, &mouse_y);
        for (size_t i = 0; i < button_count; ++i) {
            if (point_in_rect(mouse_x, mouse_y, &buttons[i].bounds)) {
                hovered = (int)i;
                break;
            }
        }

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
            if (event.type == SDL_MOUSEBUTTONDOWN &&
                event.button.button == SDL_BUTTON_LEFT) {
                float click_x;
                float click_y;
                window_to_logical(app, event.button.x, event.button.y,
                                  &click_x, &click_y);
                for (size_t i = 0; i < button_count; ++i) {
                    if (point_in_rect(click_x, click_y, &buttons[i].bounds)) {
                        return buttons[i].action;
                    }
                }
            }
        }

        render_menu(app, buttons, button_count, hovered, status_message);
        SDL_Delay(16);
    }
}
