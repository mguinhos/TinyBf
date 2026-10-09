#include "gui/widgets.h"

#define UI_FIRST_CODEPOINT  0x20
#define UI_CODEPOINT_COUNT  (0x100 - UI_FIRST_CODEPOINT)

Font ui_font;
Font ui_font_small;
float ui_char_width;

static Font load_font(int size)
{
    int codepoints[UI_CODEPOINT_COUNT];

    for (int i = 0; i < UI_CODEPOINT_COUNT; i++) {
        codepoints[i] = UI_FIRST_CODEPOINT + i;
    }

    if (!FileExists(UI_FONT_PATH)) {
        return GetFontDefault();
    }

    Font font = LoadFontEx(UI_FONT_PATH, size, codepoints, UI_CODEPOINT_COUNT);

    SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);

    return font;
}

static void unload_font(Font font)
{
    if (font.texture.id != GetFontDefault().texture.id) {
        UnloadFont(font);
    }
}

void ui_load_fonts(void)
{
    ui_font = load_font(UI_FONT_SIZE);
    ui_font_small = load_font(UI_FONT_SIZE_SMALL);
    ui_char_width = MeasureTextEx(ui_font, "M", ui_font.baseSize, 0).x;
}

void ui_unload_fonts(void)
{
    unload_font(ui_font);
    unload_font(ui_font_small);
}

void ui_text(Font font, const char* text, float x, float y, Color color)
{
    DrawTextEx(font, text, (Vector2){ x, y }, font.baseSize, 0, color);
}

void ui_char(Font font, int codepoint, float x, float y, Color color)
{
    DrawTextCodepoint(font, codepoint, (Vector2){ x, y }, font.baseSize, color);
}

bool ui_button(Rectangle rect, const char* label, bool enabled)
{
    bool hover = enabled && CheckCollisionPointRec(GetMousePosition(), rect);
    Color bg = !enabled ? COLOR_PANEL : hover ? COLOR_BORDER : COLOR_CELL;
    Vector2 size = MeasureTextEx(ui_font, label, ui_font.baseSize, 0);

    DrawRectangleRounded(rect, 0.25f, 6, bg);
    DrawRectangleRoundedLinesEx(rect, 0.25f, 6, 1, COLOR_BORDER);
    ui_text(ui_font, label, rect.x + (rect.width - size.x) / 2, rect.y + (rect.height - size.y) / 2, enabled ? COLOR_TEXT : COLOR_DIM);

    return hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

void ui_panel(Rectangle rect, const char* title, Color border)
{
    DrawRectangleRounded(rect, 0.02f, 6, COLOR_PANEL);
    DrawRectangleRoundedLinesEx(rect, 0.02f, 6, 1, border);
    ui_text(ui_font_small, title, rect.x + UI_PADDING, rect.y + 9, COLOR_DIM);
}
