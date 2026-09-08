#include <SDL2/SDL.h>
#include <stdio.h>

#include "maze.h"
#include "visualize.h"

#define TILE_SIZE 8
#define ANIMATION_DELAY_MS 25

/* All SDL-owned resources are grouped to simplify cleanup on every failure path. */
typedef struct {
    int sdl_initialized;
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* mouse_texture;
    SDL_Texture* cheese_texture;
} VisualizationContext;

static VisualizationContext context = {0};

static SDL_Texture* load_texture(const char* path);

/*
 * Release every resource that may have been allocated during initialization.
 * SDL destroy functions safely accept NULL, so this single cleanup routine can
 * be called after a partial initialization as well as normal application exit.
 */
void visualization_shutdown(void) {
    SDL_DestroyTexture(context.mouse_texture);
    SDL_DestroyTexture(context.cheese_texture);
    SDL_DestroyRenderer(context.renderer);
    SDL_DestroyWindow(context.window);

    if (context.sdl_initialized) {
        SDL_Quit();
    }
    context = (VisualizationContext){0};
}

int visualization_init(int maze_width, int maze_height) {
    if (maze_width <= 0 || maze_height <= 0 ||
        maze_width > MAX_COLS || maze_height > MAX_ROWS) {
        fprintf(stderr, "无效的窗口尺寸\n");
        return 0;
    }
    if (context.sdl_initialized || context.window || context.renderer) {
        fprintf(stderr, "SDL 可视化已经初始化\n");
        return 0;
    }

    /* SDL must be initialized before creating a window or renderer. */
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL 初始化失败: %s\n", SDL_GetError());
        return 0;
    }
    context.sdl_initialized = 1;

    context.window = SDL_CreateWindow("Maze Solver", SDL_WINDOWPOS_CENTERED,
                                      SDL_WINDOWPOS_CENTERED,
                                      maze_width * TILE_SIZE,
                                      maze_height * TILE_SIZE,
                                      SDL_WINDOW_SHOWN);
    if (!context.window) {
        fprintf(stderr, "窗口创建失败: %s\n", SDL_GetError());
        return 0;
    }

    /* Prefer hardware acceleration, then gracefully fall back to software. */
    context.renderer = SDL_CreateRenderer(context.window, -1, SDL_RENDERER_ACCELERATED);
    if (!context.renderer) {
        /* Keep the program usable on systems without hardware acceleration. */
        context.renderer = SDL_CreateRenderer(context.window, -1, SDL_RENDERER_SOFTWARE);
    }
    if (!context.renderer) {
        fprintf(stderr, "渲染器创建失败: %s\n", SDL_GetError());
        return 0;
    }

    /* Load the two required sprites after the renderer has been created. */
    context.mouse_texture = load_texture("assets/mouse.bmp");
    context.cheese_texture = load_texture("assets/cheese.bmp");
    if (!context.mouse_texture || !context.cheese_texture) {
        return 0;
    }

    return 1;
}

static SDL_Texture* load_texture(const char* path) {
    /* SDL_LoadBMP creates a temporary CPU surface; the renderer owns the texture. */
    SDL_Surface* surface = SDL_LoadBMP(path);
    if (!surface) {
        fprintf(stderr, "加载 BMP 失败: %s\n", SDL_GetError());
        return NULL;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(context.renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) {
        fprintf(stderr, "创建纹理失败: %s\n", SDL_GetError());
    }
    return texture;
}

static void set_cell_color(char cell, int is_revealed) {
    /* Revealed route cells are blue, walls black, and remaining floor gray. */
    if (cell == '#') {
        SDL_SetRenderDrawColor(context.renderer, 0, 0, 0, 255);
    } else if (is_revealed && cell != 'S' && cell != 'E') {
        SDL_SetRenderDrawColor(context.renderer, 0, 100, 255, 255);
    } else {
        SDL_SetRenderDrawColor(context.renderer, 220, 220, 220, 255);
    }
}

/*
 * Render one complete animation frame from immutable maze data.
 *
 * `revealed` is a separate overlay rather than a mutation of `maze`, allowing
 * the original map to be reused by later algorithms or render passes.  The
 * moving mouse is rendered after all tiles so it remains visible on top of E.
 */
static void render_maze_frame(char maze[][MAX_COLS + 1],
                              int revealed[][MAX_COLS],
                              int mouse_x, int mouse_y) {
    SDL_SetRenderDrawColor(context.renderer, 255, 255, 255, 255);
    SDL_RenderClear(context.renderer);

    for (int y = 0; y < ROWS; ++y) {
        for (int x = 0; x < COLS; ++x) {
            SDL_Rect rect = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
            set_cell_color(maze[y][x], revealed[y][x]);
            SDL_RenderFillRect(context.renderer, &rect);

            if (maze[y][x] == 'E') {
                SDL_RenderCopy(context.renderer, context.cheese_texture, NULL, &rect);
            }
            if (maze[y][x] == 'S' && (x != mouse_x || y != mouse_y)) {
                SDL_RenderCopy(context.renderer, context.mouse_texture, NULL, &rect);
            }
        }
    }

    /* Draw the moving mouse last so it appears above the destination tile. */
    if (mouse_x >= 0 && mouse_y >= 0) {
        SDL_Rect mouse_rect = {mouse_x * TILE_SIZE, mouse_y * TILE_SIZE,
                               TILE_SIZE, TILE_SIZE};
        SDL_RenderCopy(context.renderer, context.mouse_texture, NULL, &mouse_rect);
    }

    SDL_RenderPresent(context.renderer);
}

/*
 * Keep handling window events while an animation frame is visible.  A plain
 * SDL_Delay would make the close button unresponsive until the delay ends.
 */
static int wait_for_frame_delay(Uint32 delay_ms) {
    Uint32 start = SDL_GetTicks();
    SDL_Event event;

    while (SDL_GetTicks() - start < delay_ms) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                return 0;
            }
        }
        SDL_Delay(1);
    }
    return 1;
}

/*
 * Reveal the route from S to E one step at a time.
 *
 * Solvers store coordinates as E, ..., S, so a reverse loop starts at S.  A
 * cell is marked as revealed only after the mouse has moved beyond it.
 */
static int animate_path(char maze[][MAX_COLS + 1], const int path_x[],
                        const int path_y[], int path_len) {
    int revealed[MAX_ROWS][MAX_COLS] = {{0}};

    for (int i = path_len - 1; i >= 0; --i) {
        if (i < path_len - 1) {
            revealed[path_y[i + 1]][path_x[i + 1]] = 1;
        }

        render_maze_frame(maze, revealed, path_x[i], path_y[i]);
        if (!wait_for_frame_delay(ANIMATION_DELAY_MS)) {
            return 0;
        }
    }

    return 1;
}

/*
 * Validate caller-provided coordinates before using them as array indices.
 * This keeps the public visualization interface safe even when it is called
 * by a future solver implementation rather than the current BFS function.
 */
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

VisualizationResult visualization_play_maze(
    char maze[][MAX_COLS + 1], const int path_x[], const int path_y[],
    int path_len) {
    /* Validate all external data before rendering or grid indexing begins. */
    if (!maze || !path_x || !path_y || !is_valid_path(path_x, path_y, path_len)) {
        return VISUALIZATION_ERROR;
    }
    if (!context.renderer || !context.mouse_texture || !context.cheese_texture) {
        fprintf(stderr, "SDL 可视化尚未初始化\n");
        return VISUALIZATION_ERROR;
    }

    if (!animate_path(maze, path_x, path_y, path_len)) {
        return VISUALIZATION_QUIT;
    }

    return VISUALIZATION_FINISHED;
}

void visualization_wait_for_close(void) {
    if (!context.window) {
        return;
    }

    SDL_Event event;
    while (SDL_WaitEvent(&event) && event.type != SDL_QUIT) {
        /* Ignore non-close events after the animation completes. */
    }
}
