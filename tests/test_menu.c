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

static void push_text(const char* text) {
    SDL_Event event = {0};
    event.type = SDL_TEXTINPUT;
    snprintf(event.text.text, sizeof(event.text.text), "%s", text);
    CHECK(SDL_PushEvent(&event) == 1, "应能注入文本输入事件");
}

static void test_resized_buttons(AppContext* app) {
    const int sizes[][2] = {{400, 300}, {1200, 600}, {600, 1000}};
    const int button_y[] = {269, 364, 459};
    const MenuAction actions[] = {MENU_START, MENU_NEW_MAZE, MENU_QUIT};
    for (size_t size = 0; size < sizeof(sizes) / sizeof(sizes[0]); ++size) {
        SDL_SetWindowSize(app->window, sizes[size][0], sizes[size][1]);
        SDL_PumpEvents();
        CHECK(app_set_logical_size(app, 800, 600), "设置菜单画布");
        for (int button = 0; button < 3; ++button) {
            /* SDL_PushEvent bypasses native mouse scaling: inject the logical
             * coordinates that SDL delivers after filtering native events. */
            push_click(400, button_y[button]);
            push_key(SDLK_q); /* Fail rather than hang if a click is missed. */
            CHECK(menu_run(app, "READY") == actions[button],
                  "缩放后按钮应接收逻辑坐标点击");
            SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);
        }
    }
}

int main(void) {
    AppContext app = {0};
    char small_maze[MAX_ROWS][MAX_COLS + 1] = {{0}};
    const int path_x[] = {3, 2, 1};
    const int path_y[] = {1, 1, 1};
    int logical_width = 0;
    int logical_height = 0;
    int selected_width = 0;
    int selected_height = 0;

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

    SDL_SetWindowSize(app.window, 1200, 600);
    SDL_PumpEvents();
    int new_maze_x;
    int new_maze_y;
    SDL_RenderLogicalToWindow(app.renderer, 400.0f, 364.0f,
                              &new_maze_x, &new_maze_y);
    CHECK(new_maze_x != 400 || new_maze_y != 364,
          "测试窗口尺寸必须实际改变坐标映射");
    /* SDL delivers real mouse events in renderer logical coordinates. */
    push_click(400, 364);
    CHECK(menu_run(&app, "READY") == MENU_NEW_MAZE,
          "缩放窗口后点击 NEW MAZE 仍应有效");

    test_resized_buttons(&app);

    push_text("50");
    push_key(SDLK_TAB);
    push_text("41");
    push_key(SDLK_RETURN);
    push_click(270, 265);
    push_text("51");
    push_key(SDLK_RETURN);
    CHECK(menu_prompt_maze_size(&app, 31, 21, &selected_width,
                                &selected_height) == MAZE_SIZE_CONFIRMED,
          "应拒绝偶数尺寸并允许修正后确认");
    CHECK(selected_width == 51 && selected_height == 41,
          "尺寸输入结果必须正确");

    push_key(SDLK_ESCAPE);
    CHECK(menu_prompt_maze_size(&app, 31, 21, &selected_width,
                                &selected_height) == MAZE_SIZE_CANCELLED,
          "Esc 应取消尺寸输入");
    CHECK(selected_width == 51 && selected_height == 41,
          "取消不得改变已确认尺寸");

    push_text("3");
    push_key(SDLK_TAB);
    push_text("3");
    push_key(SDLK_RETURN);
    push_key(SDLK_ESCAPE);
    CHECK(menu_prompt_maze_size(&app, 31, 21, &selected_width,
                                &selected_height) == MAZE_SIZE_CANCELLED,
          "3 x 3 必须被拒绝并允许取消");

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
