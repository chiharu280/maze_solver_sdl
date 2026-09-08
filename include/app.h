#ifndef APP_H
#define APP_H

#include <SDL2/SDL.h>

#define APP_INITIAL_WIDTH 800
#define APP_INITIAL_HEIGHT 600

typedef struct {
    int sdl_initialized;
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* mouse_texture;
    SDL_Texture* cheese_texture;
} AppContext;

/* Initialize the resizable window and resources shared by every screen. */
int app_init(AppContext* app);

/* Select a logical canvas; SDL scales and letterboxes it with the window. */
int app_set_logical_size(AppContext* app, int width, int height);

/* Release all resources once, when the application exits. */
void app_shutdown(AppContext* app);

#endif
