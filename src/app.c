#include <SDL2/SDL.h>
#include <stdio.h>

#include "app.h"

static SDL_Texture* load_texture(AppContext* app, const char* path) {
    SDL_Surface* surface = SDL_LoadBMP(path);
    if (!surface) {
        fprintf(stderr, "加载 BMP 失败: %s\n", SDL_GetError());
        return NULL;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(app->renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) {
        fprintf(stderr, "创建纹理失败: %s\n", SDL_GetError());
    }
    return texture;
}

void app_shutdown(AppContext* app) {
    if (!app) {
        return;
    }

    SDL_DestroyTexture(app->mouse_texture);
    SDL_DestroyTexture(app->cheese_texture);
    SDL_DestroyRenderer(app->renderer);
    SDL_DestroyWindow(app->window);

    if (app->sdl_initialized) {
        SDL_Quit();
    }
    *app = (AppContext){0};
}

int app_init(AppContext* app) {
    if (!app || app->sdl_initialized || app->window || app->renderer) {
        fprintf(stderr, "无效或已经初始化的应用上下文\n");
        return 0;
    }

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL 初始化失败: %s\n", SDL_GetError());
        return 0;
    }
    app->sdl_initialized = 1;

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
    app->window = SDL_CreateWindow(
        "Maze Solver", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        APP_INITIAL_WIDTH, APP_INITIAL_HEIGHT,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    if (!app->window) {
        fprintf(stderr, "窗口创建失败: %s\n", SDL_GetError());
        app_shutdown(app);
        return 0;
    }
    SDL_SetWindowMinimumSize(app->window, 400, 300);

    app->renderer = SDL_CreateRenderer(app->window, -1,
                                       SDL_RENDERER_ACCELERATED);
    if (!app->renderer) {
        app->renderer = SDL_CreateRenderer(app->window, -1,
                                           SDL_RENDERER_SOFTWARE);
    }
    if (!app->renderer) {
        fprintf(stderr, "渲染器创建失败: %s\n", SDL_GetError());
        app_shutdown(app);
        return 0;
    }

    app->mouse_texture = load_texture(app, "assets/mouse.bmp");
    app->cheese_texture = load_texture(app, "assets/cheese.bmp");
    if (!app->mouse_texture || !app->cheese_texture) {
        app_shutdown(app);
        return 0;
    }

    SDL_SetRenderDrawBlendMode(app->renderer, SDL_BLENDMODE_BLEND);
    return 1;
}

int app_set_logical_size(AppContext* app, int width, int height) {
    if (!app || !app->renderer || width <= 0 || height <= 0) {
        return 0;
    }
    if (SDL_RenderSetLogicalSize(app->renderer, width, height) != 0) {
        fprintf(stderr, "设置逻辑画布失败: %s\n", SDL_GetError());
        return 0;
    }
    return 1;
}
