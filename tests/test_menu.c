#include <SDL2/SDL.h>
#include <stdio.h>

#include "app.h"
#include "maze.h"
#include "menu.h"
#include "visualize.h"

static int failures = 0;

#define CHECK(condition, message)                  \
    do {                                           \
        if (!(condition)) {                        \
            fprintf(stderr, "FAIL: %s\n", message); \
            ++failures;                            \
        }                                          \
    } while (0)

static void push_key(SDL_Keycode key) {
    SDL_Event event = {0};
    event.type = SDL_KEYDOWN;
    event.key.keysym.sym = key;
    CHECK(SDL_PushEvent(&event) == 1, "应能注入键盘事件");
}

static void push_click(int x, int y) {
    SDL_Event event = {0};
    event.type = SDL_MOUSEBUTTONDOWN;
    event.button.button = SDL_BUTTON_LEFT;
    event.button.x = x;
    event.button.y = y;
    CHECK(SDL_PushEvent(&event) == 1, "应能注入鼠标事件");
}

int main(void) {
    AppContext app = {0};
    char small_maze[MAX_ROWS][MAX_COLS + 1] = {{0}};
    const int path_x[] = {3, 2, 1};
    const int path_y[] = {1, 1, 1};
    int logical_width = 0;
    int logical_height = 0;

    CHECK(app_init(&app), "应用上下文应能初始化");
    if (!app.renderer) {
        app_shutdown(&app);
        return 1;
    }

    CHECK((SDL_GetWindowFlags(app.window) & SDL_WINDOW_RESIZABLE) != 0,
          "窗口必须允许调整尺寸");

    push_click(400, 265);
    CHECK(menu_run(&app, "READY") == MENU_START,
          "点击 START 按钮应开始求解");
    SDL_RenderGetLogicalSize(app.renderer, &logical_width, &logical_height);
    CHECK(logical_width == 800 && logical_height == 600,
          "菜单必须使用固定逻辑画布");

    push_key(SDLK_n);
    CHECK(menu_run(&app, "READY") == MENU_NEW_MAZE,
          "N 键应生成新迷宫");

    COLS = 5;
    ROWS = 3;
    snprintf(small_maze[0], sizeof(small_maze[0]), "#####");
    snprintf(small_maze[1], sizeof(small_maze[1]), "#S E#");
    snprintf(small_maze[2], sizeof(small_maze[2]), "#####");
    push_key(SDLK_ESCAPE);
    CHECK(visualization_play_maze(&app, small_maze, path_x, path_y, 3) ==
              VISUALIZATION_CANCELLED,
          "Esc 键应从动画返回菜单");
    SDL_RenderGetLogicalSize(app.renderer, &logical_width, &logical_height);
    CHECK(logical_width == 40 && logical_height == 24,
          "迷宫画布必须按网格尺寸设置并支持缩放");

    push_key(SDLK_q);
    CHECK(menu_run(&app, "READY") == MENU_QUIT,
          "Q 键应退出游戏");

    app_shutdown(&app);
    CHECK(app.window == NULL && app.renderer == NULL,
          "退出时必须清空 SDL 资源");

    if (failures != 0) {
        fprintf(stderr, "%d menu test(s) failed\n", failures);
        return 1;
    }
    puts("menu tests passed");
    return 0;
}
