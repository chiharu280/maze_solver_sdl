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
    int japanese = language == UI_LANGUAGE_JAPANESE;
    buttons[0].label = chinese ? "开始" : french ? "JOUER" :
                       japanese ? "開始" : "START";
    buttons[1].label = chinese ? "新迷宫" : french ? "NOUVEAU" :
                       japanese ? "新しい迷路" : "NEW MAZE";
    buttons[2].label = chinese ? "设置" : french ? "RÉGLAGES" :
                       japanese ? "設定" : "SETTINGS";
    buttons[3].label = chinese ? "语言" : french ? "LANGUE" :
                       japanese ? "言語" : "LANGUAGE";
    buttons[4].label = chinese ? "退出" : french ? "QUITTER" :
                       japanese ? "終了" : "QUIT";
}

static void render_menu(AppContext* app, const MenuButton buttons[],
                        size_t button_count, int hovered,
                        UiStatus status, UiLanguage language) {
    const int chinese = language == UI_LANGUAGE_CHINESE;
    const int french = language == UI_LANGUAGE_FRENCH;
    const int japanese = language == UI_LANGUAGE_JAPANESE;
    const int cjk = chinese || japanese;
    const char* title = chinese ? "迷宫求解器" :
                        french ? "LABYRINTHE" :
                        japanese ? "迷路ソルバー" : "MAZE SOLVER";
    const char* subtitle = chinese ? "寻找最短路径" :
                           french ? "TROUVER LE PLUS COURT CHEMIN" :
                           japanese ? "最短経路を探す" :
                                    "FIND THE SHORTEST PATH";
    const char* footer = chinese ? "可自由调整窗口" :
                         french ? "REDIMENSIONNEZ LA FENÊTRE" :
                         japanese ? "ウィンドウのサイズを自由に変更" :
                                  "RESIZE THE WINDOW FREELY";
    const char* status_message = ui_status_text(language, status);
    SDL_SetWindowTitle(app->window, chinese ? "迷宫求解器 - 菜单" :
                      french ? "Labyrinthe - Menu" :
                      japanese ? "迷路ソルバー - メニュー" :
                                 "Maze Solver - Menu");
    SDL_SetRenderDrawColor(app->renderer, 12, 18, 32, 255);
    SDL_RenderClear(app->renderer);

    SDL_SetRenderDrawColor(app->renderer, 20, 31, 52, 255);
    for (int x = 0; x < MENU_WIDTH; x += 32) {
        SDL_RenderDrawLine(app->renderer, x, 0, x, MENU_HEIGHT);
    }
    for (int y = 0; y < MENU_HEIGHT; y += 32) {
        SDL_RenderDrawLine(app->renderer, 0, y, MENU_WIDTH, y);
    }

    ui_draw_text(app, title, MENU_WIDTH / 2, 62, cjk ? 4 : 8,
                 (SDL_Color){97, 218, 251, 255});
    ui_draw_text(app, subtitle, MENU_WIDTH / 2, 142, cjk ? 2 : 3,
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
                     buttons[i].bounds.y + (cjk ? 14 : 17),
                     cjk ? 2 : 4, (SDL_Color){241, 245, 249, 255});
    }

    if (status_message && status_message[0] != '\0') {
        ui_draw_text(app, status_message, MENU_WIDTH / 2, 526,
                     cjk ? 2 : 3, (SDL_Color){134, 239, 172, 255});
    }
    ui_draw_text(app, footer, MENU_WIDTH / 2, 566, 2,
                 (SDL_Color){100, 116, 139, 255});

    SDL_RenderPresent(app->renderer);
}

MenuAction menu_run(AppContext* app, UiStatus status, UiLanguage* language) {
    MenuButton buttons[] = {
        {{250, 170, 300, 52}, NULL, MENU_START},
        {{250, 235, 300, 52}, NULL, MENU_NEW_MAZE},
        {{250, 300, 300, 52}, NULL, MENU_SETTINGS},
        {{250, 365, 300, 52}, NULL, MENU_LANGUAGE},
        {{250, 430, 300, 52}, NULL, MENU_QUIT}
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
                if (event.key.keysym.sym == SDLK_s) {
                    return MENU_SETTINGS;
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
        {200, 135, 400, 58}, {200, 220, 400, 58},
        {200, 305, 400, 58}, {200, 390, 400, 58}
    };
    const SDL_Rect cancel = {280, 490, 240, 55};
    const char* labels[] = {"中文", "ENGLISH", "FRANÇAIS", "日本語"};
    const UiLanguage choice_languages[] = {
        UI_LANGUAGE_CHINESE, UI_LANGUAGE_ENGLISH, UI_LANGUAGE_FRENCH,
        UI_LANGUAGE_JAPANESE
    };
    const int chinese = language == UI_LANGUAGE_CHINESE;
    const int french = language == UI_LANGUAGE_FRENCH;
    const int japanese = language == UI_LANGUAGE_JAPANESE;
    const int cjk = chinese || japanese;
    const char* title = chinese ? "语言" :
                        french ? "CHOISIR LA LANGUE" :
                        japanese ? "言語" : "SELECT LANGUAGE";
    const char* cancel_label = chinese ? "取消" :
                               french ? "ANNULER" :
                               japanese ? "キャンセル" : "CANCEL";

    SDL_SetWindowTitle(app->window, chinese ? "迷宫求解器 - 选择语言" :
                      french ? "Labyrinthe - Choisir la langue" :
                      japanese ? "迷路ソルバー - 言語選択" :
                               "Maze Solver - Select Language");
    SDL_SetRenderDrawColor(app->renderer, 12, 18, 32, 255);
    SDL_RenderClear(app->renderer);
    ui_draw_text(app, title, MENU_WIDTH / 2, 48, cjk ? 4 : 7,
                 (SDL_Color){97, 218, 251, 255});

    for (int i = 0; i < 4; ++i) {
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
                     choices[i].y + (i == 0 || i == 3 ? 13 : 16),
                     i == 0 || i == 3 ? 2 : 4,
                     (SDL_Color){241, 245, 249, 255});
    }

    SDL_SetRenderDrawColor(app->renderer, hovered == 4 ? 71 : 30,
                           hovered == 4 ? 85 : 41,
                           hovered == 4 ? 105 : 59, 255);
    SDL_RenderFillRect(app->renderer, &cancel);
    SDL_SetRenderDrawColor(app->renderer, 100, 116, 139, 255);
    SDL_RenderDrawRect(app->renderer, &cancel);
    ui_draw_text(app, cancel_label, MENU_WIDTH / 2,
                 cancel.y + (cjk ? 11 : 15), cjk ? 2 : 4,
                 (SDL_Color){241, 245, 249, 255});
    SDL_RenderPresent(app->renderer);
}

LanguageSelectionResult menu_prompt_language(AppContext* app,
                                             UiLanguage* language) {
    const SDL_Rect choices[] = {
        {200, 135, 400, 58}, {200, 220, 400, 58},
        {200, 305, 400, 58}, {200, 390, 400, 58}
    };
    const SDL_Rect cancel = {280, 490, 240, 55};
    const UiLanguage language_choices[] = {
        UI_LANGUAGE_CHINESE, UI_LANGUAGE_ENGLISH, UI_LANGUAGE_FRENCH,
        UI_LANGUAGE_JAPANESE
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
                if (event.key.keysym.sym == SDLK_j) {
                    *language = UI_LANGUAGE_JAPANESE;
                    return LANGUAGE_SELECTION_SELECTED;
                }
            }
            if (event.type == SDL_MOUSEMOTION) {
                hovered = -1;
                for (int i = 0; i < 4; ++i) {
                    if (point_in_rect((float)event.motion.x,
                                      (float)event.motion.y, &choices[i])) {
                        hovered = i;
                    }
                }
                if (point_in_rect((float)event.motion.x,
                                  (float)event.motion.y, &cancel)) {
                    hovered = 4;
                }
            }
            if (event.type == SDL_MOUSEBUTTONDOWN &&
                event.button.button == SDL_BUTTON_LEFT) {
                for (int i = 0; i < 4; ++i) {
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

#define SETTINGS_SLIDER_X 200
#define SETTINGS_SLIDER_Y 285
#define SETTINGS_SLIDER_WIDTH 400
#define SETTINGS_SLIDER_STEP (SETTINGS_SLIDER_WIDTH / 4)

static int speed_level_from_x(int x) {
    int level = (x - SETTINGS_SLIDER_X + SETTINGS_SLIDER_STEP / 2) /
                    SETTINGS_SLIDER_STEP +
                1;
    if (level < 1) {
        return 1;
    }
    return level > 5 ? 5 : level;
}

static void render_settings_prompt(AppContext* app, int speed_level,
                                   int back_hovered, UiLanguage language) {
    const SDL_Rect back_button = {280, 450, 240, 65};
    const int chinese = language == UI_LANGUAGE_CHINESE;
    const int french = language == UI_LANGUAGE_FRENCH;
    const int japanese = language == UI_LANGUAGE_JAPANESE;
    const int cjk = chinese || japanese;
    const char* title = chinese ? "设置" : french ? "RÉGLAGES" :
                        japanese ? "設定" : "SETTINGS";
    const char* speed_label = chinese ? "老鼠速度" :
                              french ? "VITESSE DE LA SOURIS" :
                              japanese ? "ネズミの速度" : "MOUSE SPEED";
    const char* slow_label = chinese ? "慢" : french ? "LENT" :
                             japanese ? "遅い" : "SLOW";
    const char* fast_label = chinese ? "快" : french ? "RAPIDE" :
                             japanese ? "速い" : "FAST";
    const char* back_label = chinese ? "返回" : french ? "RETOUR" :
                             japanese ? "戻る" : "BACK";
    int knob_x = SETTINGS_SLIDER_X + (speed_level - 1) *
                                      SETTINGS_SLIDER_STEP;

    SDL_SetWindowTitle(app->window, chinese ? "迷宫求解器 - 设置" :
                      french ? "Labyrinthe - Réglages" :
                      japanese ? "迷路ソルバー - 設定" :
                                 "Maze Solver - Settings");
    SDL_SetRenderDrawColor(app->renderer, 12, 18, 32, 255);
    SDL_RenderClear(app->renderer);
    ui_draw_text(app, title, MENU_WIDTH / 2, 65, cjk ? 4 : 7,
                 (SDL_Color){97, 218, 251, 255});
    ui_draw_text(app, speed_label, MENU_WIDTH / 2, 175, cjk ? 2 : 3,
                 (SDL_Color){241, 245, 249, 255});

    SDL_SetRenderDrawColor(app->renderer, 51, 65, 85, 255);
    SDL_Rect track = {SETTINGS_SLIDER_X, SETTINGS_SLIDER_Y - 4,
                      SETTINGS_SLIDER_WIDTH, 8};
    SDL_RenderFillRect(app->renderer, &track);
    SDL_SetRenderDrawColor(app->renderer, 37, 99, 235, 255);
    SDL_Rect selected_track = {SETTINGS_SLIDER_X, SETTINGS_SLIDER_Y - 4,
                               knob_x - SETTINGS_SLIDER_X, 8};
    SDL_RenderFillRect(app->renderer, &selected_track);

    for (int level = 1; level <= 5; ++level) {
        int tick_x = SETTINGS_SLIDER_X + (level - 1) * SETTINGS_SLIDER_STEP;
        SDL_Rect tick = {tick_x - 2, SETTINGS_SLIDER_Y - 10, 4, 20};
        char number[2] = {(char)('0' + level), '\0'};
        SDL_SetRenderDrawColor(app->renderer, 148, 163, 184, 255);
        SDL_RenderFillRect(app->renderer, &tick);
        ui_draw_text(app, number, tick_x, 320, 3,
                     (SDL_Color){148, 163, 184, 255});
    }

    SDL_Rect knob = {knob_x - 12, SETTINGS_SLIDER_Y - 16, 24, 32};
    SDL_SetRenderDrawColor(app->renderer, 96, 165, 250, 255);
    SDL_RenderFillRect(app->renderer, &knob);
    SDL_SetRenderDrawColor(app->renderer, 241, 245, 249, 255);
    SDL_RenderDrawRect(app->renderer, &knob);

    ui_draw_text(app, slow_label, SETTINGS_SLIDER_X, 365, cjk ? 2 : 3,
                 (SDL_Color){100, 116, 139, 255});
    ui_draw_text(app, fast_label, SETTINGS_SLIDER_X + SETTINGS_SLIDER_WIDTH,
                 365, cjk ? 2 : 3, (SDL_Color){100, 116, 139, 255});

    SDL_SetRenderDrawColor(app->renderer, back_hovered ? 37 : 30,
                           back_hovered ? 99 : 41,
                           back_hovered ? 235 : 59, 255);
    SDL_RenderFillRect(app->renderer, &back_button);
    SDL_SetRenderDrawColor(app->renderer, 96, 165, 250, 255);
    SDL_RenderDrawRect(app->renderer, &back_button);
    ui_draw_text(app, back_label, MENU_WIDTH / 2,
                 back_button.y + (cjk ? 16 : 19), cjk ? 2 : 4,
                 (SDL_Color){241, 245, 249, 255});
    SDL_RenderPresent(app->renderer);
}

SettingsResult menu_prompt_settings(AppContext* app, int* speed_level,
                                    UiLanguage language) {
    const SDL_Rect slider_hitbox = {
        SETTINGS_SLIDER_X - 20, SETTINGS_SLIDER_Y - 35,
        SETTINGS_SLIDER_WIDTH + 40, 70
    };
    const SDL_Rect back_button = {280, 450, 240, 65};
    int dragging = 0;
    int back_hovered = 0;

    if (!app || !app->renderer || !speed_level || *speed_level < 1 ||
        *speed_level > 5 ||
        !app_set_logical_size(app, MENU_WIDTH, MENU_HEIGHT)) {
        return SETTINGS_ERROR;
    }
    for (;;) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                return SETTINGS_QUIT;
            }
            if (event.type == SDL_KEYDOWN) {
                SDL_Keycode key = event.key.keysym.sym;
                if (key == SDLK_ESCAPE || key == SDLK_RETURN ||
                    key == SDLK_KP_ENTER) {
                    return SETTINGS_DONE;
                }
                if (key == SDLK_LEFT && *speed_level > 1) {
                    --*speed_level;
                } else if (key == SDLK_RIGHT && *speed_level < 5) {
                    ++*speed_level;
                } else if (key >= SDLK_1 && key <= SDLK_5) {
                    *speed_level = (int)(key - SDLK_0);
                }
            }
            if (event.type == SDL_MOUSEBUTTONDOWN &&
                event.button.button == SDL_BUTTON_LEFT) {
                if (point_in_rect((float)event.button.x,
                                  (float)event.button.y, &back_button)) {
                    return SETTINGS_DONE;
                }
                if (point_in_rect((float)event.button.x,
                                  (float)event.button.y, &slider_hitbox)) {
                    dragging = 1;
                    *speed_level = speed_level_from_x(event.button.x);
                }
            }
            if (event.type == SDL_MOUSEMOTION) {
                back_hovered = point_in_rect((float)event.motion.x,
                                             (float)event.motion.y,
                                             &back_button);
                if (dragging) {
                    *speed_level = speed_level_from_x(event.motion.x);
                }
            }
            if (event.type == SDL_MOUSEBUTTONUP &&
                event.button.button == SDL_BUTTON_LEFT) {
                dragging = 0;
            }
        }
        render_settings_prompt(app, *speed_level, back_hovered, language);
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
                       int east_asian) {
    ui_draw_text(app, label, bounds->x + bounds->w / 2, bounds->y - 38,
                 east_asian ? 2 : 3, (SDL_Color){148, 163, 184, 255});
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
    int japanese = language == UI_LANGUAGE_JAPANESE;
    int cjk = chinese || japanese;

    SDL_SetRenderDrawColor(app->renderer, 12, 18, 32, 255);
    SDL_RenderClear(app->renderer);
    ui_draw_text(app, chinese ? "新迷宫" :
                      french ? "NOUVEAU LABYRINTHE" :
                      japanese ? "新しい迷路" : "NEW MAZE",
                 MENU_WIDTH / 2, 70, cjk ? 4 : french ? 5 : 8,
                 (SDL_Color){97, 218, 251, 255});
    ui_draw_text(app, chinese ? "请输入3到99的奇数" :
                      french ? "DIMENSIONS IMPAIRES 3 À 99" :
                      japanese ? "3から99までの奇数" :
                               "ODD SIZE FROM 3 TO 99",
                 MENU_WIDTH / 2, 160, cjk ? 2 : 3,
                 (SDL_Color){148, 163, 184, 255});

    draw_input(app, &width_input,
               chinese ? "宽度" : french ? "LARGEUR" :
               japanese ? "幅" : "WIDTH", width_text, focused == 0, cjk);
    draw_input(app, &height_input,
               chinese ? "高度" : french ? "HAUTEUR" :
               japanese ? "高さ" : "HEIGHT", height_text, focused == 1, cjk);

    SDL_SetRenderDrawColor(app->renderer, 37, 99, 235, 255);
    SDL_RenderFillRect(app->renderer, &generate_button);
    SDL_SetRenderDrawColor(app->renderer, 71, 85, 105, 255);
    SDL_RenderFillRect(app->renderer, &cancel_button);
    ui_draw_text(app, chinese ? "生成" : french ? "CRÉER" :
                      japanese ? "生成" : "GENERATE",
                 generate_button.x + generate_button.w / 2,
                 generate_button.y + (cjk ? 14 : 20), cjk ? 2 : 4,
                 (SDL_Color){241, 245, 249, 255});
    ui_draw_text(app, chinese ? "取消" : french ? "ANNULER" :
                      japanese ? "キャンセル" : "CANCEL",
                 cancel_button.x + cancel_button.w / 2,
                 cancel_button.y + (cjk ? 14 : 20), cjk ? 2 : 4,
                 (SDL_Color){241, 245, 249, 255});

    ui_draw_text(app, validation_message, MENU_WIDTH / 2, 340, 2,
                 (SDL_Color){248, 113, 113, 255});
    ui_draw_text(app, chinese ? "TAB切换输入框" :
                      french ? "TAB CHANGE DE CHAMP" :
                      japanese ? "TABで入力欄を切替" : "TAB SWITCHES FIELD",
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
                      "Labyrinthe - Nouveau" :
                      language == UI_LANGUAGE_JAPANESE ?
                      "迷路ソルバー - 新しい迷路" :
                      "Maze Solver - New Maze");
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
                                     language == UI_LANGUAGE_JAPANESE ?
                                         "無効なサイズ" :
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
