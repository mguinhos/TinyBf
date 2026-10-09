#ifndef TBF_GUI_WIDGETS_H
#define TBF_GUI_WIDGETS_H

#include <stdbool.h>

#include "raylib.h"

#include "tbf/tbf-math/tbf-math.h"
#include "tbf/tbf-gui/ripple.h"
#include "tbf/tbf-gui/theme.h"

#define TBF_GUI_FONT_DIR        "../thirdparty/fonts/"
#define TBF_GUI_FONT_FALLBACK   "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf"

#define TBF_GUI_LINE_HEIGHT     19
#define TBF_GUI_LINE_HEIGHT_LARGE 29
#define TBF_GUI_CARD_PADDING    16
#define TBF_GUI_CARD_HEADER     44
#define TBF_GUI_CARD_RADIUS     12

#define TBF_GUI_ICON_PLAY       0xe037
#define TBF_GUI_ICON_PAUSE      0xe034
#define TBF_GUI_ICON_STEP       0xe044
#define TBF_GUI_ICON_SKIP       0xe01f
#define TBF_GUI_ICON_RESET      0xf053
#define TBF_GUI_ICON_ADD        0xe145
#define TBF_GUI_ICON_REMOVE     0xe15b
#define TBF_GUI_ICON_LIGHT      0xe518
#define TBF_GUI_ICON_DARK       0xe51c
#define TBF_GUI_ICON_FULLSCREEN 0xe5d0
#define TBF_GUI_ICON_FULLSCREEN_EXIT 0xe5d1
#define TBF_GUI_ICON_COPY       0xe14d
#define TBF_GUI_ICON_NONE       0

typedef enum TbfGui_ButtonKind {
    TBF_GUI_BUTTON_FILLED,
    TBF_GUI_BUTTON_TONAL,
    TBF_GUI_BUTTON_OUTLINED,
} TbfGui_ButtonKind;

typedef struct TbfGui_Fonts {
    Font title;
    Font label;
    Font body;
    Font small;
    Font mono;
    Font mono_small;
    Font mono_large;
    Font icons;
    Font icons_small;
    float char_width;
    float char_width_large;
} TbfGui_Fonts;

typedef struct TbfGui_Style {
    TbfGui_Fonts fonts;
    const TbfGui_Theme* theme;
    TbfGui_Ripples* ripples;
} TbfGui_Style;

void tbf_gui_fonts_load(TbfGui_Fonts* self);
void tbf_gui_fonts_unload(TbfGui_Fonts* self);

void tbf_gui_cursor_begin(void);
void tbf_gui_cursor_request(int cursor);
void tbf_gui_cursor_apply(void);

TbfMath_Vector2 tbf_gui_mouse_position(void);
bool tbf_gui_mouse_over(Rectangle rect);

TbfMath_Vector2 tbf_gui_text_size(Font font, const char* text);
void tbf_gui_text(Font font, const char* text, float x, float y, Color color);
void tbf_gui_char(Font font, int codepoint, float x, float y, Color color);
void tbf_gui_icon(Font font, int icon, TbfMath_Vector2 center, Color color);

void tbf_gui_circle(TbfMath_Vector2 center, float radius, Color color);

void tbf_gui_rounded(Rectangle rect, float radius, Color color);
void tbf_gui_rounded_lines(Rectangle rect, float radius, float thickness, Color color);

bool tbf_gui_button(const TbfGui_Style* style, Rectangle rect, TbfGui_ButtonKind kind, int icon, const char* label, bool enabled);
bool tbf_gui_icon_button(const TbfGui_Style* style, Rectangle rect, int icon, bool enabled);
bool tbf_gui_switch(const TbfGui_Style* style, Rectangle rect, bool on, int icon_on, int icon_off);
void tbf_gui_card(const TbfGui_Style* style, Rectangle rect, const char* title, const char* subtitle);
void tbf_gui_text_field(const TbfGui_Style* style, Rectangle rect, const char* label, bool focused);

#endif
