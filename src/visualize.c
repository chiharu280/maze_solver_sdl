#include <SDL2/SDL.h>

#include "app.h"
#include "maze.h"
#include "ui.h"
#include "visualize.h"

#define TILE_SIZE 8
#define ANIMATION_DELAY_MS 25
#define DIALOG_WIDTH 800
#define DIALOG_HEIGHT 600

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
            if (maze[y][x] == 'S' && (mouse_x < 0 || mouse_y < 0)) {
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

static int point_in_rect(int x, int y, const SDL_Rect* rect) {
    return x >= rect->x && x < rect->x + rect->w &&
           y >= rect->y && y < rect->y + rect->h;
}

static void render_scaled_maze(AppContext* app,
                               char maze[][MAX_COLS + 1],
                               int revealed[][MAX_COLS],
                               int mouse_x, int mouse_y) {
    int cell_size = DIALOG_WIDTH / COLS;
    int height_cell_size = DIALOG_HEIGHT / ROWS;
    int offset_x;
    int offset_y;

    if (height_cell_size < cell_size) {
        cell_size = height_cell_size;
    }
    offset_x = (DIALOG_WIDTH - COLS * cell_size) / 2;
    offset_y = (DIALOG_HEIGHT - ROWS * cell_size) / 2;
    SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
    SDL_RenderClear(app->renderer);
    for (int y = 0; y < ROWS; ++y) {
        for (int x = 0; x < COLS; ++x) {
            SDL_Rect rect = {
                offset_x + x * cell_size,
                offset_y + y * cell_size,
                cell_size,
                cell_size
            };
            set_cell_color(app, maze[y][x], revealed[y][x]);
            SDL_RenderFillRect(app->renderer, &rect);
            if (maze[y][x] == 'E') {
                SDL_RenderCopy(app->renderer, app->cheese_texture, NULL, &rect);
            }
            if (maze[y][x] == 'S' && (mouse_x < 0 || mouse_y < 0)) {
                SDL_RenderCopy(app->renderer, app->mouse_texture, NULL, &rect);
            }
            if (x == mouse_x && y == mouse_y) {
                SDL_RenderCopy(app->renderer, app->mouse_texture, NULL, &rect);
            }
        }
    }
}

static void render_completion_dialog(AppContext* app,
                                     char maze[][MAX_COLS + 1],
                                     int revealed[][MAX_COLS],
                                     int mouse_x, int mouse_y,
                                     int hovered, UiLanguage language) {
    const SDL_Rect panel = {120, 160, 560, 280};
    const SDL_Rect buttons[] = {{165, 330, 210, 66}, {425, 330, 210, 66}};
    const int chinese = language == UI_LANGUAGE_CHINESE;
    const int french = language == UI_LANGUAGE_FRENCH;
    const int japanese = language == UI_LANGUAGE_JAPANESE;
    const int cjk = chinese || japanese;

    render_scaled_maze(app, maze, revealed, mouse_x, mouse_y);
    SDL_SetRenderDrawColor(app->renderer, 2, 6, 23, 175);
    SDL_RenderFillRect(app->renderer, NULL);
    SDL_SetRenderDrawColor(app->renderer, 15, 23, 42, 250);
    SDL_RenderFillRect(app->renderer, &panel);
    SDL_SetRenderDrawColor(app->renderer, 96, 165, 250, 255);
    SDL_RenderDrawRect(app->renderer, &panel);

    ui_draw_text(app, chinese ? "老鼠已经找到奶酪了" :
                      french ? "LA SOURIS A TROUVÉ LE FROMAGE" :
                      japanese ? "ネズミがチーズを見つけました" :
                               "MOUSE FOUND THE CHEESE",
                 DIALOG_WIDTH / 2, 220, cjk ? 2 : 3,
                 (SDL_Color){241, 245, 249, 255});
    for (int i = 0; i < 2; ++i) {
        if (hovered == i) {
            SDL_SetRenderDrawColor(app->renderer, 37, 99, 235, 255);
        } else {
            SDL_SetRenderDrawColor(app->renderer, 30, 41, 59, 255);
        }
        SDL_RenderFillRect(app->renderer, &buttons[i]);
        SDL_SetRenderDrawColor(app->renderer, 96, 165, 250, 255);
        SDL_RenderDrawRect(app->renderer, &buttons[i]);
    }
    ui_draw_text(app, chinese ? "再来一次" : french ? "REJOUER" :
                      japanese ? "もう一度" : "PLAY AGAIN", 270,
                 cjk ? 346 : 350, cjk ? 2 : 3,
                 (SDL_Color){241, 245, 249, 255});
    ui_draw_text(app, chinese ? "退出" : french ? "QUITTER" :
                      japanese ? "終了" : "EXIT", 530,
                 cjk ? 346 : 350, cjk ? 2 : 3,
                 (SDL_Color){241, 245, 249, 255});
    SDL_RenderPresent(app->renderer);
}

static VisualizationResult show_completion_dialog(
    AppContext* app, char maze[][MAX_COLS + 1], int revealed[][MAX_COLS],
    int mouse_x, int mouse_y, UiLanguage language) {
    const SDL_Rect buttons[] = {{165, 330, 210, 66}, {425, 330, 210, 66}};
    int hovered = -1;

    if (!app_set_logical_size(app, DIALOG_WIDTH, DIALOG_HEIGHT)) {
        return VISUALIZATION_ERROR;
    }
    SDL_SetWindowTitle(app->window, language == UI_LANGUAGE_CHINESE ?
                      "迷宫求解器 - 完成" :
                      language == UI_LANGUAGE_FRENCH ?
                      "Labyrinthe - Terminé" :
                      language == UI_LANGUAGE_JAPANESE ?
                      "迷路ソルバー - 完了" : "Maze Solver - Complete");
    for (;;) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                return VISUALIZATION_QUIT;
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_RETURN ||
                    event.key.keysym.sym == SDLK_KP_ENTER ||
                    event.key.keysym.sym == SDLK_SPACE) {
                    return VISUALIZATION_REPLAY;
                }
                if (event.key.keysym.sym == SDLK_ESCAPE ||
                    event.key.keysym.sym == SDLK_q) {
                    return VISUALIZATION_FINISHED;
                }
            }
            if (event.type == SDL_MOUSEMOTION) {
                hovered = -1;
                for (int i = 0; i < 2; ++i) {
                    if (point_in_rect(event.motion.x, event.motion.y,
                                      &buttons[i])) {
                        hovered = i;
                    }
                }
            }
            if (event.type == SDL_MOUSEBUTTONDOWN &&
                event.button.button == SDL_BUTTON_LEFT) {
                if (point_in_rect(event.button.x, event.button.y,
                                  &buttons[0])) {
                    return VISUALIZATION_REPLAY;
                }
                if (point_in_rect(event.button.x, event.button.y,
                                  &buttons[1])) {
                    return VISUALIZATION_FINISHED;
                }
            }
        }
        render_completion_dialog(app, maze, revealed, mouse_x, mouse_y,
                                 hovered, language);
        SDL_Delay(16);
    }
}

VisualizationResult visualization_play_maze(
    AppContext* app, char maze[][MAX_COLS + 1], const int path_x[],
    const int path_y[], int path_len, UiLanguage language) {
    int revealed[MAX_ROWS][MAX_COLS] = {{0}};

    if (!app || !app->renderer || !app->mouse_texture ||
        !app->cheese_texture || !maze || !path_x || !path_y ||
        !is_valid_path(path_x, path_y, path_len)) {
        return VISUALIZATION_ERROR;
    }
    if (!app_set_logical_size(app, COLS * TILE_SIZE, ROWS * TILE_SIZE)) {
        return VISUALIZATION_ERROR;
    }
    SDL_SetWindowTitle(app->window, language == UI_LANGUAGE_CHINESE ?
                      "迷宫求解器 - 求解中" :
                      language == UI_LANGUAGE_FRENCH ?
                      "Labyrinthe - Résolution" :
                      language == UI_LANGUAGE_JAPANESE ?
                      "迷路ソルバー - 探索中" :
                      "Maze Solver - Solving (Esc: Menu)");

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

    revealed[path_y[0]][path_x[0]] = 1;
    return show_completion_dialog(app, maze, revealed, path_x[0], path_y[0],
                                  language);
}
