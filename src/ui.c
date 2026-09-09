#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ui.h"

#define MAX_CJK_GLYPHS 192

typedef struct {
    uint32_t codepoint;
    uint8_t rows[8];
} LatinGlyph;

typedef struct {
    uint32_t codepoint;
    uint16_t rows[16];
} CjkGlyph;

static const LatinGlyph LATIN_FONT[] = {
    {'0', {14, 17, 19, 21, 25, 17, 14}},
    {'1', {4, 12, 4, 4, 4, 4, 14}},
    {'2', {14, 17, 1, 2, 4, 8, 31}},
    {'3', {30, 1, 1, 14, 1, 1, 30}},
    {'4', {2, 6, 10, 18, 31, 2, 2}},
    {'5', {31, 16, 16, 30, 1, 1, 30}},
    {'6', {14, 16, 16, 30, 17, 17, 14}},
    {'7', {31, 1, 2, 4, 8, 8, 8}},
    {'8', {14, 17, 17, 14, 17, 17, 14}},
    {'9', {14, 17, 17, 15, 1, 1, 14}},
    {'A', {14, 17, 17, 31, 17, 17, 17}},
    {'B', {30, 17, 17, 30, 17, 17, 30}},
    {'C', {14, 17, 16, 16, 16, 17, 14}},
    {'D', {30, 17, 17, 17, 17, 17, 30}},
    {'E', {31, 16, 16, 30, 16, 16, 31}},
    {'F', {31, 16, 16, 30, 16, 16, 16}},
    {'G', {14, 17, 16, 23, 17, 17, 15}},
    {'H', {17, 17, 17, 31, 17, 17, 17}},
    {'I', {14, 4, 4, 4, 4, 4, 14}},
    {'J', {7, 2, 2, 2, 2, 18, 12}},
    {'K', {17, 18, 20, 24, 20, 18, 17}},
    {'L', {16, 16, 16, 16, 16, 16, 31}},
    {'M', {17, 27, 21, 21, 17, 17, 17}},
    {'N', {17, 25, 21, 19, 17, 17, 17}},
    {'O', {14, 17, 17, 17, 17, 17, 14}},
    {'P', {30, 17, 17, 30, 16, 16, 16}},
    {'Q', {14, 17, 17, 17, 21, 18, 13}},
    {'R', {30, 17, 17, 30, 20, 18, 17}},
    {'S', {15, 16, 16, 14, 1, 1, 30}},
    {'T', {31, 4, 4, 4, 4, 4, 4}},
    {'U', {17, 17, 17, 17, 17, 17, 14}},
    {'V', {17, 17, 17, 17, 17, 10, 4}},
    {'W', {17, 17, 17, 21, 21, 21, 10}},
    {'X', {17, 17, 10, 4, 10, 17, 17}},
    {'Y', {17, 17, 10, 4, 4, 4, 4}},
    {'Z', {31, 1, 2, 4, 8, 16, 31}},
    {0x00C0u, {4, 2, 14, 17, 31, 17, 17, 0}}, /* À */
    {0x00C7u, {14, 17, 16, 16, 17, 14, 4, 8}}, /* Ç */
    {0x00C9u, {2, 4, 31, 16, 30, 16, 31, 0}},  /* É */
    {0x00CAu, {4, 10, 31, 16, 30, 16, 31, 0}}  /* Ê */
};

static CjkGlyph cjk_font[MAX_CJK_GLYPHS];
static size_t cjk_glyph_count;
static int cjk_font_loaded;

static void load_cjk_font(void) {
    FILE* file;
    char line[80];

    if (cjk_font_loaded) {
        return;
    }
    cjk_font_loaded = 1;
    file = fopen("assets/ui_font.hex", "r");
    if (!file) {
        return;
    }
    while (cjk_glyph_count < MAX_CJK_GLYPHS &&
           fgets(line, sizeof(line), file)) {
        char* separator = strchr(line, ':');
        char* bits;
        CjkGlyph glyph = {0};
        if (!separator || strlen(separator + 1) < 64) {
            continue;
        }
        *separator = '\0';
        glyph.codepoint = (uint32_t)strtoul(line, NULL, 16);
        bits = separator + 1;
        for (int row = 0; row < 16; ++row) {
            char value[5];
            memcpy(value, bits + row * 4, 4);
            value[4] = '\0';
            glyph.rows[row] = (uint16_t)strtoul(value, NULL, 16);
        }
        cjk_font[cjk_glyph_count++] = glyph;
    }
    fclose(file);
}

static const uint8_t* find_latin_glyph(uint32_t codepoint) {
    for (size_t i = 0; i < sizeof(LATIN_FONT) / sizeof(LATIN_FONT[0]); ++i) {
        if (LATIN_FONT[i].codepoint == codepoint) {
            return LATIN_FONT[i].rows;
        }
    }
    return NULL;
}

static const uint16_t* find_cjk_glyph(uint32_t codepoint) {
    load_cjk_font();
    for (size_t i = 0; i < cjk_glyph_count; ++i) {
        if (cjk_font[i].codepoint == codepoint) {
            return cjk_font[i].rows;
        }
    }
    return NULL;
}

static uint32_t next_codepoint(const char** current) {
    const unsigned char* text = (const unsigned char*)*current;
    uint32_t codepoint;
    if (text[0] < 0x80) {
        codepoint = text[0];
        *current += 1;
    } else if ((text[0] & 0xE0) == 0xC0 && text[1]) {
        codepoint = ((uint32_t)(text[0] & 0x1F) << 6) |
                    (uint32_t)(text[1] & 0x3F);
        *current += 2;
    } else if ((text[0] & 0xF0) == 0xE0 && text[1] && text[2]) {
        codepoint = ((uint32_t)(text[0] & 0x0F) << 12) |
                    ((uint32_t)(text[1] & 0x3F) << 6) |
                    (uint32_t)(text[2] & 0x3F);
        *current += 3;
    } else {
        codepoint = '?';
        *current += 1;
    }
    return codepoint;
}

static int glyph_advance(uint32_t codepoint, int scale) {
    return (codepoint < 0x100 ? 6 : 17) * scale;
}

const char* ui_status_text(UiLanguage language, UiStatus status) {
    static const char* const english[] = {
        "READY", "MAZE LOAD FAILED", "NEW MAZE GENERATED",
        "MAZE GENERATION FAILED", "GENERATION CANCELLED", "BACK TO MENU",
        "NO PATH FOUND"
    };
    static const char* const chinese[] = {
        "准备就绪", "迷宫加载失败", "新迷宫已生成", "迷宫生成失败",
        "已取消", "返回菜单", "没有找到路径"
    };
    static const char* const french[] = {
        "PRÊT", "ÉCHEC DU CHARGEMENT", "NOUVEAU LABYRINTHE CRÉÉ",
        "ÉCHEC DE CRÉATION", "CRÉATION ANNULÉE", "RETOUR AU MENU",
        "AUCUN CHEMIN"
    };
    static const char* const japanese[] = {
        "準備完了", "迷路の読み込みに失敗", "新しい迷路を生成しました",
        "迷路の生成に失敗", "生成をキャンセルしました",
        "メニューに戻りました", "経路が見つかりません"
    };
    if (status < UI_STATUS_READY || status > UI_STATUS_NO_PATH_FOUND) {
        status = UI_STATUS_READY;
    }
    if (language == UI_LANGUAGE_CHINESE) {
        return chinese[status];
    }
    if (language == UI_LANGUAGE_FRENCH) {
        return french[status];
    }
    return language == UI_LANGUAGE_JAPANESE ? japanese[status] :
                                               english[status];
}

int ui_text_width(const char* text, int scale) {
    int width = 0;
    const char* current = text;
    if (!text || scale <= 0) {
        return 0;
    }
    while (*current) {
        width += glyph_advance(next_codepoint(&current), scale);
    }
    return width > 0 ? width - scale : 0;
}

void ui_draw_text(AppContext* app, const char* text, int center_x, int y,
                  int scale, SDL_Color color) {
    const char* current = text;
    int x;
    if (!app || !app->renderer || !text || scale <= 0) {
        return;
    }
    x = center_x - ui_text_width(text, scale) / 2;
    SDL_SetRenderDrawColor(app->renderer, color.r, color.g, color.b, color.a);
    while (*current) {
        uint32_t codepoint = next_codepoint(&current);
        if (codepoint < 0x100) {
            const uint8_t* rows = find_latin_glyph(codepoint);
            if (rows) {
                for (int row = 0; row < 8; ++row) {
                    for (int column = 0; column < 5; ++column) {
                        if (rows[row] & (1u << (4 - column))) {
                            SDL_Rect pixel = {x + column * scale,
                                              y + row * scale, scale, scale};
                            SDL_RenderFillRect(app->renderer, &pixel);
                        }
                    }
                }
            }
        } else {
            const uint16_t* rows = find_cjk_glyph(codepoint);
            if (rows) {
                for (int row = 0; row < 16; ++row) {
                    for (int column = 0; column < 16; ++column) {
                        if (rows[row] & (1u << (15 - column))) {
                            SDL_Rect pixel = {x + column * scale,
                                              y + row * scale, scale, scale};
                            SDL_RenderFillRect(app->renderer, &pixel);
                        }
                    }
                }
            }
        }
        x += glyph_advance(codepoint, scale);
    }
}
