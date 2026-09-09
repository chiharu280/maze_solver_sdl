#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "app.h"
#include "menu.h"
#include "ui.h"

#define MENU_WIDTH 800
#define MENU_HEIGHT 600

typedef struct {
    SDL_Rect bounds;
    const char* label;
    MenuAction action;
} MenuButton;

static int point_in_rect(float x, float y, const SDL_Rect* rect) {
    return x >= rect->x && x < rect->x + rect->w &&
           y >= rect->y && y < rect->y + rect->h;
}

static void set_menu_labels(MenuButton buttons[], UiLanguage language) {
    int chinese = language == UI_LANGUAGE_CHINESE;
    int french = language == UI_LANGUAGE_FRENCH;
    buttons[0].label = chinese ? "开始" : french ? "JOUER" : "START";
    buttons[1].label = chinese ? "新迷宫" : french ? "NOUVEAU" : "NEW MAZE";
    buttons[2].label = chinese ? "语言" : french ? "LANGUE" : "LANGUAGE";
    buttons[3].label = chinese ? "退出" : french ? "QUITTER" : "QUIT";
}

static void render_menu(AppContext* app, const MenuButton buttons[],
                        size_t button_count, int hovered,
                        UiStatus status, UiLanguage language) {
    const int chinese = language == UI_LANGUAGE_CHINESE;
    const int french = language == UI_LANGUAGE_FRENCH;
    const char* title = chinese ? "迷宫求解器" :
                        french ? "LABYRINTHE" : "MAZE SOLVER";
    const char* subtitle = chinese ? "寻找最短路径" :
                           french ? "TROUVER LE PLUS COURT CHEMIN" :
                                    "FIND THE SHORTEST PATH";
    const char* footer = chinese ? "可自由调整窗口" :
                         french ? "REDIMENSIONNEZ LA FENÊTRE" :
                                  "RESIZE THE WINDOW FREELY";
    const char* status_message = ui_status_text(language, status);
    SDL_SetWindowTitle(app->window, chinese ? "迷宫求解器 - 菜单" :
                      french ? "Labyrinthe - Menu" : "Maze Solver - Menu");
    SDL_SetRenderDrawColor(app->renderer, 12, 18, 32, 255);
    SDL_RenderClear(app->renderer);

    SDL_SetRenderDrawColor(app->renderer, 20, 31, 52, 255);
    for (int x = 0; x < MENU_WIDTH; x += 32) {
        SDL_RenderDrawLine(app->renderer, x, 0, x, MENU_HEIGHT);
    }
    for (int y = 0; y < MENU_HEIGHT; y += 32) {
        SDL_RenderDrawLine(app->renderer, 0, y, MENU_WIDTH, y);
    }

    ui_draw_text(app, title, MENU_WIDTH / 2, 62, chinese ? 4 : 8,
                 (SDL_Color){97, 218, 251, 255});
    ui_draw_text(app, subtitle, MENU_WIDTH / 2, 142, chinese ? 2 : 3,
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
        ui_draw_text(app, buttons[i].label, MENU_WIDTH / 2,
                     buttons[i].bounds.y + (chinese ? 14 : 17),
                     chinese ? 2 : 4, (SDL_Color){241, 245, 249, 255});
    }

    if (status_message && status_message[0] != '\0') {
        ui_draw_text(app, status_message, MENU_WIDTH / 2, 526,
                     chinese ? 2 : 3, (SDL_Color){134, 239, 172, 255});
    }
    ui_draw_text(app, footer, MENU_WIDTH / 2, 566, 2,
                 (SDL_Color){100, 116, 139, 255});

    SDL_RenderPresent(app->renderer);
}

MenuAction menu_run(AppContext* app, UiStatus status, UiLanguage* language) {
    MenuButton buttons[] = {
        {{250, 195, 300, 60}, NULL, MENU_START},
        {{250, 275, 300, 60}, NULL, MENU_NEW_MAZE},
        {{250, 355, 300, 60}, NULL, MENU_LANGUAGE},
        {{250, 435, 300, 60}, NULL, MENU_QUIT}
    };
    const size_t button_count = sizeof(buttons) / sizeof(buttons[0]);

    if (!app || !app->renderer || !language ||
        !app_set_logical_size(app, MENU_WIDTH, MENU_HEIGHT)) {
        return MENU_ERROR;
    }
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
                if (event.key.keysym.sym == SDLK_l) {
                    return MENU_LANGUAGE;
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

        set_menu_labels(buttons, *language);
        render_menu(app, buttons, button_count, hovered, status, *language);
        SDL_Delay(16);
    }
}

static void render_language_prompt(AppContext* app, int hovered,
                                   UiLanguage language) {
    const SDL_Rect choices[] = {
        {200, 175, 400, 65}, {200, 265, 400, 65}, {200, 355, 400, 65}
    };
    const SDL_Rect cancel = {280, 465, 240, 60};
    const char* labels[] = {"中文", "ENGLISH", "FRANÇAIS"};
    const UiLanguage choice_languages[] = {
        UI_LANGUAGE_CHINESE, UI_LANGUAGE_ENGLISH, UI_LANGUAGE_FRENCH
    };
    const int chinese = language == UI_LANGUAGE_CHINESE;
    const int french = language == UI_LANGUAGE_FRENCH;
    const char* title = chinese ? "选择语言" :
                        french ? "CHOISIR LA LANGUE" : "SELECT LANGUAGE";
    const char* cancel_label = chinese ? "取消" :
                               french ? "ANNULER" : "CANCEL";

    SDL_SetWindowTitle(app->window, chinese ? "迷宫求解器 - 选择语言" :
                      french ? "Labyrinthe - Choisir la langue" :
                               "Maze Solver - Select Language");
    SDL_SetRenderDrawColor(app->renderer, 12, 18, 32, 255);
    SDL_RenderClear(app->renderer);
    ui_draw_text(app, title, MENU_WIDTH / 2, 68, chinese ? 4 : 7,
                 (SDL_Color){97, 218, 251, 255});

    for (int i = 0; i < 3; ++i) {
        if (hovered == i) {
            SDL_SetRenderDrawColor(app->renderer, 37, 99, 235, 255);
        } else if (language == choice_languages[i]) {
            SDL_SetRenderDrawColor(app->renderer, 22, 78, 99, 255);
        } else {
            SDL_SetRenderDrawColor(app->renderer, 30, 41, 59, 255);
        }
        SDL_RenderFillRect(app->renderer, &choices[i]);
        SDL_SetRenderDrawColor(app->renderer, 96, 165, 250, 255);
        SDL_RenderDrawRect(app->renderer, &choices[i]);
        ui_draw_text(app, labels[i], MENU_WIDTH / 2,
                     choices[i].y + (i == 0 ? 16 : 19), i == 0 ? 2 : 4,
                     (SDL_Color){241, 245, 249, 255});
    }

    SDL_SetRenderDrawColor(app->renderer, hovered == 3 ? 71 : 30,
                           hovered == 3 ? 85 : 41,
                           hovered == 3 ? 105 : 59, 255);
    SDL_RenderFillRect(app->renderer, &cancel);
    SDL_SetRenderDrawColor(app->renderer, 100, 116, 139, 255);
    SDL_RenderDrawRect(app->renderer, &cancel);
    ui_draw_text(app, cancel_label, MENU_WIDTH / 2,
                 cancel.y + (chinese ? 14 : 17), chinese ? 2 : 4,
                 (SDL_Color){241, 245, 249, 255});
    SDL_RenderPresent(app->renderer);
}

LanguageSelectionResult menu_prompt_language(AppContext* app,
                                             UiLanguage* language) {
    const SDL_Rect choices[] = {
        {200, 175, 400, 65}, {200, 265, 400, 65}, {200, 355, 400, 65}
    };
    const SDL_Rect cancel = {280, 465, 240, 60};
    const UiLanguage language_choices[] = {
        UI_LANGUAGE_CHINESE, UI_LANGUAGE_ENGLISH, UI_LANGUAGE_FRENCH
    };
    int hovered = -1;

    if (!app || !app->renderer || !language ||
        !app_set_logical_size(app, MENU_WIDTH, MENU_HEIGHT)) {
        return LANGUAGE_SELECTION_ERROR;
    }
    for (;;) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                return LANGUAGE_SELECTION_QUIT;
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    return LANGUAGE_SELECTION_CANCELLED;
                }
                if (event.key.keysym.sym == SDLK_c) {
                    *language = UI_LANGUAGE_CHINESE;
                    return LANGUAGE_SELECTION_SELECTED;
                }
                if (event.key.keysym.sym == SDLK_e) {
                    *language = UI_LANGUAGE_ENGLISH;
                    return LANGUAGE_SELECTION_SELECTED;
                }
                if (event.key.keysym.sym == SDLK_f) {
                    *language = UI_LANGUAGE_FRENCH;
                    return LANGUAGE_SELECTION_SELECTED;
                }
            }
            if (event.type == SDL_MOUSEMOTION) {
                hovered = -1;
                for (int i = 0; i < 3; ++i) {
                    if (point_in_rect((float)event.motion.x,
                                      (float)event.motion.y, &choices[i])) {
                        hovered = i;
                    }
                }
                if (point_in_rect((float)event.motion.x,
                                  (float)event.motion.y, &cancel)) {
                    hovered = 3;
                }
            }
            if (event.type == SDL_MOUSEBUTTONDOWN &&
                event.button.button == SDL_BUTTON_LEFT) {
                for (int i = 0; i < 3; ++i) {
                    if (point_in_rect((float)event.button.x,
                                      (float)event.button.y, &choices[i])) {
                        *language = language_choices[i];
                        return LANGUAGE_SELECTION_SELECTED;
                    }
                }
                if (point_in_rect((float)event.button.x,
                                  (float)event.button.y, &cancel)) {
                    return LANGUAGE_SELECTION_CANCELLED;
                }
            }
        }
        render_language_prompt(app, hovered, *language);
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
                       const char* label, const char* value, int focused,
                       int chinese) {
    ui_draw_text(app, label, bounds->x + bounds->w / 2, bounds->y - 38,
                 chinese ? 2 : 3, (SDL_Color){148, 163, 184, 255});
    SDL_SetRenderDrawColor(app->renderer, 15, 23, 42, 255);
    SDL_RenderFillRect(app->renderer, bounds);
    if (focused) {
        SDL_SetRenderDrawColor(app->renderer, 96, 165, 250, 255);
    } else {
        SDL_SetRenderDrawColor(app->renderer, 71, 85, 105, 255);
    }
    SDL_RenderDrawRect(app->renderer, bounds);
    ui_draw_text(app, value, bounds->x + bounds->w / 2, bounds->y + 16, 5,
                 (SDL_Color){241, 245, 249, 255});
}

static void draw_size_prompt(AppContext* app, const char* width_text,
                             const char* height_text, int focused,
                             const char* validation_message,
                             UiLanguage language) {
    const SDL_Rect width_input = {180, 230, 180, 70};
    const SDL_Rect height_input = {440, 230, 180, 70};
    const SDL_Rect generate_button = {155, 400, 220, 68};
    const SDL_Rect cancel_button = {425, 400, 220, 68};
    int chinese = language == UI_LANGUAGE_CHINESE;
    int french = language == UI_LANGUAGE_FRENCH;

    SDL_SetRenderDrawColor(app->renderer, 12, 18, 32, 255);
    SDL_RenderClear(app->renderer);
    ui_draw_text(app, chinese ? "新迷宫" :
                      french ? "NOUVEAU LABYRINTHE" : "NEW MAZE",
                 MENU_WIDTH / 2, 70, chinese ? 4 : french ? 5 : 8,
                 (SDL_Color){97, 218, 251, 255});
    ui_draw_text(app, chinese ? "请输入3到99的奇数" :
                      french ? "DIMENSIONS IMPAIRES 3 À 99" :
                               "ODD SIZE FROM 3 TO 99",
                 MENU_WIDTH / 2, 160, chinese ? 2 : 3,
                 (SDL_Color){148, 163, 184, 255});

    draw_input(app, &width_input,
               chinese ? "宽度" : french ? "LARGEUR" : "WIDTH", width_text,
               focused == 0, chinese);
    draw_input(app, &height_input,
               chinese ? "高度" : french ? "HAUTEUR" : "HEIGHT", height_text,
               focused == 1, chinese);

    SDL_SetRenderDrawColor(app->renderer, 37, 99, 235, 255);
    SDL_RenderFillRect(app->renderer, &generate_button);
    SDL_SetRenderDrawColor(app->renderer, 71, 85, 105, 255);
    SDL_RenderFillRect(app->renderer, &cancel_button);
    ui_draw_text(app, chinese ? "生成" : french ? "CRÉER" : "GENERATE",
                 generate_button.x + generate_button.w / 2,
                 generate_button.y + (chinese ? 14 : 20), chinese ? 2 : 4,
                 (SDL_Color){241, 245, 249, 255});
    ui_draw_text(app, chinese ? "取消" : french ? "ANNULER" : "CANCEL",
                 cancel_button.x + cancel_button.w / 2,
                 cancel_button.y + (chinese ? 14 : 20), chinese ? 2 : 4,
                 (SDL_Color){241, 245, 249, 255});

    ui_draw_text(app, validation_message, MENU_WIDTH / 2, 340, 2,
                 (SDL_Color){248, 113, 113, 255});
    ui_draw_text(app, chinese ? "TAB切换输入框" :
                      french ? "TAB CHANGE DE CHAMP" : "TAB SWITCHES FIELD",
                 MENU_WIDTH / 2, 535, 2,
                 (SDL_Color){100, 116, 139, 255});
    SDL_RenderPresent(app->renderer);
}

MazeSizeResult menu_prompt_maze_size(AppContext* app, int initial_width,
                                     int initial_height, int* width,
                                     int* height, UiLanguage language) {
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
    SDL_SetWindowTitle(app->window, language == UI_LANGUAGE_CHINESE ?
                      "迷宫求解器 - 新迷宫" :
                      language == UI_LANGUAGE_FRENCH ?
                      "Labyrinthe - Nouveau" : "Maze Solver - New Maze");
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
                validation_message = language == UI_LANGUAGE_CHINESE ?
                                         "尺寸无效" :
                                     language == UI_LANGUAGE_FRENCH ?
                                         "TAILLE INVALIDE" :
                                         "ODD 3 TO 99 AND NOT 3 X 3";
            }
        }

        if (result == MAZE_SIZE_ERROR) {
            draw_size_prompt(app, width_text, height_text, focused,
                             validation_message, language);
            SDL_Delay(16);
        }
    }

    SDL_StopTextInput();
    return result;
}
