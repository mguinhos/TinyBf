#include <math.h>
#include <stddef.h>

#include "tbf/tbf-gui/widgets.h"

#define TBF_GUI_FIRST_CODEPOINT 0x20
#define TBF_GUI_CODEPOINT_COUNT (0x100 - TBF_GUI_FIRST_CODEPOINT)

#define TBF_GUI_STATE_HOVER     0.08f
#define TBF_GUI_DISABLED_BG     0.12f
#define TBF_GUI_DISABLED_FG     0.38f

static const int tbf_gui_icon_codepoints[] = {
    TBF_GUI_ICON_PLAY,
    TBF_GUI_ICON_PAUSE,
    TBF_GUI_ICON_STEP,
    TBF_GUI_ICON_SKIP,
    TBF_GUI_ICON_RESET,
    TBF_GUI_ICON_ADD,
    TBF_GUI_ICON_REMOVE,
    TBF_GUI_ICON_LIGHT,
    TBF_GUI_ICON_DARK,
    TBF_GUI_ICON_FULLSCREEN,
    TBF_GUI_ICON_FULLSCREEN_EXIT,
    TBF_GUI_ICON_COPY,
};

#define TBF_GUI_ICON_COUNT ((int) (sizeof(tbf_gui_icon_codepoints) / sizeof(tbf_gui_icon_codepoints[0])))

static Vector2 tbf_gui_vector2_to_raylib(TbfMath_Vector2 v)
{
    return (Vector2) { v.x, v.y };
}

static TbfMath_Vector2 tbf_gui_vector2_from_raylib(Vector2 v)
{
    return tbf_math_vector2(v.x, v.y);
}

static Font tbf_gui_font_load(const char* name, int size, const int* codepoints, int count)
{
    const char* path = TextFormat("%s%s%s", GetApplicationDirectory(), TBF_GUI_FONT_DIR, name);

    if (!FileExists(path)) {
        if (codepoints != NULL || !FileExists(TBF_GUI_FONT_FALLBACK)) {
            return GetFontDefault();
        }

        path = TBF_GUI_FONT_FALLBACK;
    }

    int latin[TBF_GUI_CODEPOINT_COUNT];

    if (codepoints == NULL) {
        for (int i = 0; i < TBF_GUI_CODEPOINT_COUNT; i++) {
            latin[i] = TBF_GUI_FIRST_CODEPOINT + i;
        }

        codepoints = latin;
        count = TBF_GUI_CODEPOINT_COUNT;
    }

    Font font = LoadFontEx(path, size, (int*) codepoints, count);

    SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);

    return font;
}

static void tbf_gui_font_unload(Font font)
{
    if (font.texture.id != GetFontDefault().texture.id) {
        UnloadFont(font);
    }
}

void tbf_gui_fonts_load(TbfGui_Fonts* self)
{
    self->title = tbf_gui_font_load("Roboto-Regular.ttf", 22, NULL, 0);
    self->label = tbf_gui_font_load("Roboto-Medium.ttf", 14, NULL, 0);
    self->body = tbf_gui_font_load("Roboto-Regular.ttf", 14, NULL, 0);
    self->small = tbf_gui_font_load("Roboto-Regular.ttf", 12, NULL, 0);
    self->mono = tbf_gui_font_load("RobotoMono-Regular.ttf", 16, NULL, 0);
    self->mono_small = tbf_gui_font_load("RobotoMono-Regular.ttf", 11, NULL, 0);
    self->mono_large = tbf_gui_font_load("RobotoMono-Regular.ttf", 24, NULL, 0);
    self->icons = tbf_gui_font_load("MaterialIcons-Regular.ttf", 24, tbf_gui_icon_codepoints, TBF_GUI_ICON_COUNT);
    self->icons_small = tbf_gui_font_load("MaterialIcons-Regular.ttf", 18, tbf_gui_icon_codepoints, TBF_GUI_ICON_COUNT);
    self->char_width = tbf_gui_text_size(self->mono, "M").x;
    self->char_width_large = tbf_gui_text_size(self->mono_large, "M").x;
}

void tbf_gui_fonts_unload(TbfGui_Fonts* self)
{
    tbf_gui_font_unload(self->title);
    tbf_gui_font_unload(self->label);
    tbf_gui_font_unload(self->body);
    tbf_gui_font_unload(self->small);
    tbf_gui_font_unload(self->mono);
    tbf_gui_font_unload(self->mono_small);
    tbf_gui_font_unload(self->mono_large);
    tbf_gui_font_unload(self->icons);
    tbf_gui_font_unload(self->icons_small);
}

/* A raylib cria um cursor novo a cada SetMouseCursor, então só aplicamos quando muda. */
static int tbf_gui_cursor_requested = MOUSE_CURSOR_DEFAULT;
static int tbf_gui_cursor_applied = MOUSE_CURSOR_DEFAULT;

void tbf_gui_cursor_begin(void)
{
    tbf_gui_cursor_requested = MOUSE_CURSOR_DEFAULT;
}

void tbf_gui_cursor_request(int cursor)
{
    tbf_gui_cursor_requested = cursor;
}

void tbf_gui_cursor_apply(void)
{
    if (tbf_gui_cursor_requested != tbf_gui_cursor_applied) {
        SetMouseCursor(tbf_gui_cursor_requested);
        tbf_gui_cursor_applied = tbf_gui_cursor_requested;
    }
}

TbfMath_Vector2 tbf_gui_mouse_position(void)
{
    return tbf_gui_vector2_from_raylib(GetMousePosition());
}

bool tbf_gui_mouse_over(Rectangle rect)
{
    return CheckCollisionPointRec(GetMousePosition(), rect);
}

TbfMath_Vector2 tbf_gui_text_size(Font font, const char* text)
{
    return tbf_gui_vector2_from_raylib(MeasureTextEx(font, text, font.baseSize, 0));
}

void tbf_gui_text(Font font, const char* text, float x, float y, Color color)
{
    TbfMath_Vector2 position = tbf_math_vector2(roundf(x), roundf(y));

    DrawTextEx(font, text, tbf_gui_vector2_to_raylib(position), font.baseSize, 0, color);
}

void tbf_gui_char(Font font, int codepoint, float x, float y, Color color)
{
    TbfMath_Vector2 position = tbf_math_vector2(roundf(x), roundf(y));

    DrawTextCodepoint(font, codepoint, tbf_gui_vector2_to_raylib(position), font.baseSize, color);
}

void tbf_gui_circle(TbfMath_Vector2 center, float radius, Color color)
{
    DrawCircleV(tbf_gui_vector2_to_raylib(center), radius, color);
}

void tbf_gui_icon(Font font, int icon, TbfMath_Vector2 center, Color color)
{
    if (font.texture.id == GetFontDefault().texture.id) {
        return;
    }

    tbf_gui_char(font, icon, center.x - font.baseSize / 2.0f, center.y - font.baseSize / 2.0f, color);
}

static float tbf_gui_roundness(Rectangle rect, float radius)
{
    float side = fminf(rect.width, rect.height);

    return side > 0 ? fminf(1.0f, 2 * radius / side) : 0;
}

void tbf_gui_rounded(Rectangle rect, float radius, Color color)
{
    DrawRectangleRounded(rect, tbf_gui_roundness(rect, radius), 12, color);
}

void tbf_gui_rounded_lines(Rectangle rect, float radius, float thickness, Color color)
{
    DrawRectangleRoundedLinesEx(rect, tbf_gui_roundness(rect, radius), 12, thickness, color);
}

static bool tbf_gui_state_layer(const TbfGui_Style* style, Rectangle rect, float radius, Color content, bool enabled, bool centered)
{
    bool hover = enabled && tbf_gui_mouse_over(rect);
    bool clicked = hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

    if (hover) {
        tbf_gui_rounded(rect, radius, Fade(content, TBF_GUI_STATE_HOVER));
    }

    if (clicked) {
        tbf_gui_ripples_press(style->ripples, rect, tbf_gui_mouse_position(), centered, content);
    }

    tbf_gui_ripples_draw(style->ripples, rect, rect, radius);

    return clicked;
}

bool tbf_gui_button(const TbfGui_Style* style, Rectangle rect, TbfGui_ButtonKind kind, int icon, const char* label, bool enabled)
{
    const TbfGui_Theme* theme = style->theme;
    const float radius = rect.height / 2;

    Color bg = BLANK;
    Color fg = theme->primary;

    switch (kind) {
    case TBF_GUI_BUTTON_FILLED:
        bg = theme->primary;
        fg = theme->on_primary;
        break;

    case TBF_GUI_BUTTON_TONAL:
        bg = theme->secondary_container;
        fg = theme->on_secondary_container;
        break;

    case TBF_GUI_BUTTON_OUTLINED:
        break;
    }

    if (!enabled) {
        bg = kind == TBF_GUI_BUTTON_OUTLINED ? BLANK : Fade(theme->on_surface, TBF_GUI_DISABLED_BG);
        fg = Fade(theme->on_surface, TBF_GUI_DISABLED_FG);
    }

    tbf_gui_rounded(rect, radius, bg);

    if (kind == TBF_GUI_BUTTON_OUTLINED) {
        Color border = enabled ? theme->outline : Fade(theme->on_surface, TBF_GUI_DISABLED_BG);

        tbf_gui_rounded_lines(rect, radius, 1, border);
    }

    bool clicked = tbf_gui_state_layer(style, rect, radius, fg, enabled, false);

    const Font font = style->fonts.label;
    const float icon_size = style->fonts.icons_small.baseSize;
    const float gap = icon != TBF_GUI_ICON_NONE ? 8 : 0;

    TbfMath_Vector2 size = tbf_gui_text_size(font, label);
    float width = size.x + (icon != TBF_GUI_ICON_NONE ? icon_size + gap : 0);
    float x = rect.x + (rect.width - width) / 2;
    float center_y = rect.y + rect.height / 2;

    if (icon != TBF_GUI_ICON_NONE) {
        tbf_gui_icon(style->fonts.icons_small, icon, tbf_math_vector2(x + icon_size / 2, center_y), fg);
        x += icon_size + gap;
    }

    tbf_gui_text(font, label, x, center_y - size.y / 2, fg);

    return clicked;
}

bool tbf_gui_icon_button(const TbfGui_Style* style, Rectangle rect, int icon, bool enabled)
{
    const TbfGui_Theme* theme = style->theme;
    Color fg = enabled ? theme->on_surface_variant : Fade(theme->on_surface, TBF_GUI_DISABLED_FG);

    bool clicked = tbf_gui_state_layer(style, rect, rect.height / 2, fg, enabled, true);

    tbf_gui_icon(style->fonts.icons, icon, tbf_math_vector2(rect.x + rect.width / 2, rect.y + rect.height / 2), fg);

    return clicked;
}

bool tbf_gui_switch(const TbfGui_Style* style, Rectangle rect, bool on, int icon_on, int icon_off)
{
    const TbfGui_Theme* theme = style->theme;
    const float thumb = 24;
    const float inset = (rect.height - thumb) / 2;

    TbfMath_Vector2 center = tbf_math_vector2(
        on ? rect.x + rect.width - inset - thumb / 2 : rect.x + inset + thumb / 2,
        rect.y + rect.height / 2
    );

    if (on) {
        tbf_gui_rounded(rect, rect.height / 2, theme->primary);
    } else {
        tbf_gui_rounded(rect, rect.height / 2, theme->surface_container_highest);
        tbf_gui_rounded_lines(rect, rect.height / 2, 2, theme->outline);
    }

    Color layer = on ? theme->primary : theme->on_surface;
    Rectangle halo = { center.x - 20, center.y - 20, 40, 40 };
    bool hover = tbf_gui_mouse_over(rect);
    bool clicked = hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

    if (hover) {
        tbf_gui_circle(center, 20, Fade(layer, TBF_GUI_STATE_HOVER));
    }

    if (clicked) {
        tbf_gui_ripples_press(style->ripples, rect, center, true, layer);
    }

    tbf_gui_ripples_draw(style->ripples, rect, halo, 20);

    tbf_gui_circle(center, thumb / 2, on ? theme->on_primary : theme->outline);
    tbf_gui_icon(
        style->fonts.icons_small,
        on ? icon_on : icon_off,
        center,
        on ? theme->on_primary_container : theme->surface_container_highest
    );

    return clicked;
}

void tbf_gui_card(const TbfGui_Style* style, Rectangle rect, const char* title, const char* subtitle)
{
    const TbfGui_Theme* theme = style->theme;
    float x = rect.x + TBF_GUI_CARD_PADDING;
    float y = rect.y + TBF_GUI_CARD_PADDING;

    tbf_gui_rounded(rect, TBF_GUI_CARD_RADIUS, theme->surface_container);
    tbf_gui_text(style->fonts.label, title, x, y, theme->on_surface);

    if (subtitle != NULL) {
        x += tbf_gui_text_size(style->fonts.label, title).x + 8;
        tbf_gui_text(style->fonts.body, subtitle, x, y, theme->on_surface_variant);
    }
}

void tbf_gui_text_field(const TbfGui_Style* style, Rectangle rect, const char* label, bool focused)
{
    const TbfGui_Theme* theme = style->theme;
    const float radius = 4;

    Color color = focused ? theme->primary : theme->outline;
    TbfMath_Vector2 size = tbf_gui_text_size(style->fonts.small, label);

    tbf_gui_rounded_lines(rect, radius, focused ? 2 : 1, color);

    DrawRectangleRec((Rectangle) { rect.x + 12, rect.y - 2, size.x + 8, 4 }, theme->surface);
    tbf_gui_text(style->fonts.small, label, rect.x + 16, rect.y - size.y / 2, focused ? theme->primary : theme->on_surface_variant);
}
