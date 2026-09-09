#include <SDL2/SDL.h>
#include <stdint.h>
#include <time.h>

#include "app.h"
#include "generator.h"
#include "maze.h"
#include "menu.h"
#include "solver.h"
#include "visualize.h"

#define MAZE_FILE "assets/maze.txt"
#define DEFAULT_GENERATED_WIDTH 91
#define DEFAULT_GENERATED_HEIGHT 91

static int dimensions_can_be_generated(int width, int height) {
    return width >= 3 && height >= 3 && width <= MAX_COLS &&
           height <= MAX_ROWS && width % 2 != 0 && height % 2 != 0 &&
           (width != 3 || height != 3);
}

static uint32_t next_generation_seed(void) {
    static uint32_t sequence = 0;
    uint64_t counter = SDL_GetPerformanceCounter();

    ++sequence;
    return (uint32_t)time(NULL) ^ (uint32_t)counter ^
           (uint32_t)(counter >> 32) ^ sequence;
}

static int regenerate_maze(int width, int height) {
    return generate_maze_file(MAZE_FILE, width, height,
                              next_generation_seed()) &&
           load_maze_from_file(MAZE_FILE);
}

static VisualizationResult solve_and_visualize(AppContext* app,
                                               UiLanguage language,
                                               int speed_level) {
    int start_x;
    int start_y;

    if (!find_maze_marker('S', &start_x, &start_y) ||
        !solve_maze_bfs(maze, start_x, start_y)) {
        return VISUALIZATION_ERROR;
    }

    return visualization_play_maze(app, maze, path_x, path_y, path_len,
                                   language, speed_level);
}

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;
    AppContext app = {0};
    int maze_is_ready = load_maze_from_file(MAZE_FILE);
    int running = 1;
    int exit_code = 0;
    int speed_level = 3;
    UiLanguage language = UI_LANGUAGE_CHINESE;
    UiStatus status = maze_is_ready ? UI_STATUS_READY :
                                      UI_STATUS_MAZE_LOAD_FAILED;

    if (!app_init(&app)) {
        return 1;
    }

    while (running) {
        MenuAction action = menu_run(&app, status, &language);

        switch (action) {
        case MENU_START:
        {
            VisualizationResult visualization_result;
            if (!maze_is_ready) {
                status = UI_STATUS_MAZE_LOAD_FAILED;
                break;
            }

            do {
                visualization_result = solve_and_visualize(
                    &app, language, speed_level);
            } while (visualization_result == VISUALIZATION_REPLAY);

            switch (visualization_result) {
            case VISUALIZATION_FINISHED:
                status = UI_STATUS_BACK_TO_MENU;
                break;
            case VISUALIZATION_CANCELLED:
                status = UI_STATUS_BACK_TO_MENU;
                break;
            case VISUALIZATION_QUIT:
                running = 0;
                break;
            case VISUALIZATION_ERROR:
            default:
                status = UI_STATUS_NO_PATH_FOUND;
                break;
            }
            break;
        }

        case MENU_NEW_MAZE:
        {
            int width = maze_is_ready && dimensions_can_be_generated(COLS, ROWS) ?
                            COLS : DEFAULT_GENERATED_WIDTH;
            int height = maze_is_ready && dimensions_can_be_generated(COLS, ROWS) ?
                             ROWS : DEFAULT_GENERATED_HEIGHT;
            MazeSizeResult size_result = menu_prompt_maze_size(
                &app, width, height, &width, &height, language);

            if (size_result == MAZE_SIZE_CONFIRMED) {
                maze_is_ready = regenerate_maze(width, height);
                status = maze_is_ready ? UI_STATUS_NEW_MAZE_GENERATED :
                                         UI_STATUS_MAZE_GENERATION_FAILED;
            } else if (size_result == MAZE_SIZE_CANCELLED) {
                status = UI_STATUS_GENERATION_CANCELLED;
            } else if (size_result == MAZE_SIZE_QUIT) {
                running = 0;
            } else {
                exit_code = 1;
                running = 0;
            }
            break;
        }

        case MENU_LANGUAGE:
        {
            LanguageSelectionResult language_result =
                menu_prompt_language(&app, &language);
            if (language_result == LANGUAGE_SELECTION_QUIT) {
                running = 0;
            } else if (language_result == LANGUAGE_SELECTION_ERROR) {
                exit_code = 1;
                running = 0;
            }
            break;
        }

        case MENU_SETTINGS:
        {
            SettingsResult settings_result = menu_prompt_settings(
                &app, &speed_level, language);
            if (settings_result == SETTINGS_QUIT) {
                running = 0;
            } else if (settings_result == SETTINGS_ERROR) {
                exit_code = 1;
                running = 0;
            }
            break;
        }

        case MENU_QUIT:
            running = 0;
            break;

        case MENU_ERROR:
        default:
            exit_code = 1;
            running = 0;
            break;
        }
    }

    app_shutdown(&app);
    return exit_code;
}
