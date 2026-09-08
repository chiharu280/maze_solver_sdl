#include <SDL2/SDL.h>

#include "app.h"
#include "maze.h"
#include "visualize.h"

#define TILE_SIZE 8
#define ANIMATION_DELAY_MS 25
#define COMPLETION_DELAY_MS 650

typedef enum {
    WAIT_FINISHED,
    WAIT_CANCELLED,
    WAIT_QUIT
} WaitResult;

static void set_cell_color(AppContext* app, char cell, int is_revealed) {
    if (cell == '#') {
        SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
    } else if (is_revealed && cell != 'S' && cell != 'E') {
        SDL_SetRenderDrawColor(app->renderer, 0, 100, 255, 255);
    } else {
        SDL_SetRenderDrawColor(app->renderer, 220, 220, 220, 255);
    }
}

static void render_maze_frame(AppContext* app,
                              char maze[][MAX_COLS + 1],
                              int revealed[][MAX_COLS],
                              int mouse_x, int mouse_y) {
    SDL_SetRenderDrawColor(app->renderer, 255, 255, 255, 255);
    SDL_RenderClear(app->renderer);

    for (int y = 0; y < ROWS; ++y) {
        for (int x = 0; x < COLS; ++x) {
            SDL_Rect rect = {x * TILE_SIZE, y * TILE_SIZE,
                             TILE_SIZE, TILE_SIZE};
            set_cell_color(app, maze[y][x], revealed[y][x]);
            SDL_RenderFillRect(app->renderer, &rect);

            if (maze[y][x] == 'E') {
                SDL_RenderCopy(app->renderer, app->cheese_texture, NULL, &rect);
            }
            if (maze[y][x] == 'S' && (x != mouse_x || y != mouse_y)) {
                SDL_RenderCopy(app->renderer, app->mouse_texture, NULL, &rect);
            }
        }
    }

    if (mouse_x >= 0 && mouse_y >= 0) {
        SDL_Rect mouse_rect = {mouse_x * TILE_SIZE, mouse_y * TILE_SIZE,
                               TILE_SIZE, TILE_SIZE};
        SDL_RenderCopy(app->renderer, app->mouse_texture, NULL, &mouse_rect);
    }

    SDL_RenderPresent(app->renderer);
}

static WaitResult wait_with_events(Uint32 delay_ms) {
    Uint32 start = SDL_GetTicks();
    SDL_Event event;

    while (SDL_GetTicks() - start < delay_ms) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                return WAIT_QUIT;
            }
            if (event.type == SDL_KEYDOWN &&
                event.key.keysym.sym == SDLK_ESCAPE) {
                return WAIT_CANCELLED;
            }
        }
        SDL_Delay(1);
    }
    return WAIT_FINISHED;
}

static int is_valid_path(const int path_x[], const int path_y[], int path_len) {
    if (path_len <= 0 || path_len > MAX_ROWS * MAX_COLS) {
        return 0;
    }

    for (int i = 0; i < path_len; ++i) {
        if (path_x[i] < 0 || path_x[i] >= COLS ||
            path_y[i] < 0 || path_y[i] >= ROWS) {
            return 0;
        }
    }
    return 1;
}

static VisualizationResult map_wait_result(WaitResult result) {
    if (result == WAIT_QUIT) {
        return VISUALIZATION_QUIT;
    }
    if (result == WAIT_CANCELLED) {
        return VISUALIZATION_CANCELLED;
    }
    return VISUALIZATION_FINISHED;
}

VisualizationResult visualization_play_maze(
    AppContext* app, char maze[][MAX_COLS + 1], const int path_x[],
    const int path_y[], int path_len) {
    int revealed[MAX_ROWS][MAX_COLS] = {{0}};

    if (!app || !app->renderer || !app->mouse_texture ||
        !app->cheese_texture || !maze || !path_x || !path_y ||
        !is_valid_path(path_x, path_y, path_len)) {
        return VISUALIZATION_ERROR;
    }
    if (!app_set_logical_size(app, COLS * TILE_SIZE, ROWS * TILE_SIZE)) {
        return VISUALIZATION_ERROR;
    }
    SDL_SetWindowTitle(app->window, "Maze Solver - Solving (Esc: Menu)");

    for (int i = path_len - 1; i >= 0; --i) {
        if (i < path_len - 1) {
            revealed[path_y[i + 1]][path_x[i + 1]] = 1;
        }

        render_maze_frame(app, maze, revealed, path_x[i], path_y[i]);
        WaitResult wait_result = wait_with_events(ANIMATION_DELAY_MS);
        if (wait_result != WAIT_FINISHED) {
            return map_wait_result(wait_result);
        }
    }

    return map_wait_result(wait_with_events(COMPLETION_DELAY_MS));
}
