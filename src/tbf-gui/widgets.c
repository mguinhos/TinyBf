#include "tbf/tbf-gui/widgets.h"

#define TBF_GUI_FIRST_CODEPOINT 0x20
#define TBF_GUI_CODEPOINT_COUNT (0x100 - TBF_GUI_FIRST_CODEPOINT)

static Font tbf_gui_font_load(int size)
{
    int codepoints[TBF_GUI_CODEPOINT_COUNT];

    for (int i = 0; i < TBF_GUI_CODEPOINT_COUNT; i++) {
        codepoints[i] = TBF_GUI_FIRST_CODEPOINT + i;
    }

    if (!FileExists(TBF_GUI_FONT_PATH)) {
        return GetFontDefault();
    }

    Font font = LoadFontEx(TBF_GUI_FONT_PATH, size, codepoints, TBF_GUI_CODEPOINT_COUNT);

    SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);

    return font;
}

static void tbf_gui_font_unload(Font font)
{
    if (font.texture.id != GetFontDefault().texture.id) {
        UnloadFont(font);
    }
}

void tbf_gui_fonts_load(TbfGuiFonts* self)
{
    self->regular = tbf_gui_font_load(TBF_GUI_FONT_SIZE);
    self->small = tbf_gui_font_load(TBF_GUI_FONT_SIZE_SMALL);
    self->char_width = MeasureTextEx(self->regular, "M", self->regular.baseSize, 0).x;
}

void tbf_gui_fonts_unload(TbfGuiFonts* self)
{
    tbf_gui_font_unload(self->regular);
    tbf_gui_font_unload(self->small);
}

void tbf_gui_text(Font font, const char* text, float x, float y, Color color)
{
    DrawTextEx(font, text, (Vector2) { x, y }, font.baseSize, 0, color);
}

void tbf_gui_char(Font font, int codepoint, float x, float y, Color color)
{
    DrawTextCodepoint(font, codepoint, (Vector2) { x, y }, font.baseSize, color);
}

bool tbf_gui_button(const TbfGuiFonts* fonts, Rectangle rect, const char* label, bool enabled)
{
    bool hover = enabled && CheckCollisionPointRec(GetMousePosition(), rect);
    Color bg = !enabled ? TBF_GUI_COLOR_PANEL : hover ? TBF_GUI_COLOR_BORDER : TBF_GUI_COLOR_CELL;
    Color fg = enabled ? TBF_GUI_COLOR_TEXT : TBF_GUI_COLOR_DIM;
    Vector2 size = MeasureTextEx(fonts->regular, label, fonts->regular.baseSize, 0);

    DrawRectangleRounded(rect, 0.25f, 6, bg);
    DrawRectangleRoundedLinesEx(rect, 0.25f, 6, 1, TBF_GUI_COLOR_BORDER);
    tbf_gui_text(fonts->regular, label, rect.x + (rect.width - size.x) / 2, rect.y + (rect.height - size.y) / 2, fg);

    return hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

void tbf_gui_panel(const TbfGuiFonts* fonts, Rectangle rect, const char* title, Color border)
{
    DrawRectangleRounded(rect, 0.02f, 6, TBF_GUI_COLOR_PANEL);
    DrawRectangleRoundedLinesEx(rect, 0.02f, 6, 1, border);
    tbf_gui_text(fonts->small, title, rect.x + TBF_GUI_PADDING, rect.y + 9, TBF_GUI_COLOR_DIM);
}
