#ifndef TBF_GUI_WIDGETS_H
#define TBF_GUI_WIDGETS_H

#include <stdbool.h>

#include "raylib.h"

#define TBF_GUI_FONT_PATH       "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf"
#define TBF_GUI_FONT_SIZE       18
#define TBF_GUI_FONT_SIZE_SMALL 13
#define TBF_GUI_LINE_HEIGHT     18

#define TBF_GUI_PADDING         10
#define TBF_GUI_HEADER_HEIGHT   30

#define TBF_GUI_COLOR_BG            (Color) { 24, 25, 30, 255 }
#define TBF_GUI_COLOR_PANEL         (Color) { 34, 36, 43, 255 }
#define TBF_GUI_COLOR_BORDER        (Color) { 52, 55, 66, 255 }
#define TBF_GUI_COLOR_CELL          (Color) { 44, 47, 56, 255 }
#define TBF_GUI_COLOR_TEXT          (Color) { 220, 223, 228, 255 }
#define TBF_GUI_COLOR_DIM           (Color) { 120, 125, 140, 255 }
#define TBF_GUI_COLOR_ACCENT        (Color) { 97, 175, 239, 255 }
#define TBF_GUI_COLOR_HIGHLIGHT     (Color) { 229, 192, 123, 255 }
#define TBF_GUI_COLOR_BREAKPOINT    (Color) { 224, 108, 117, 255 }
#define TBF_GUI_COLOR_SUCCESS       (Color) { 152, 195, 121, 255 }
#define TBF_GUI_COLOR_OP_ARITH      (Color) { 152, 195, 121, 255 }
#define TBF_GUI_COLOR_OP_MOVE       (Color) { 97, 175, 239, 255 }
#define TBF_GUI_COLOR_OP_LOOP       (Color) { 198, 120, 221, 255 }
#define TBF_GUI_COLOR_OP_IO         (Color) { 229, 192, 123, 255 }

typedef struct TbfGuiFonts {
    Font regular;
    Font small;
    float char_width;
} TbfGuiFonts;

void tbf_gui_fonts_load(TbfGuiFonts* self);
void tbf_gui_fonts_unload(TbfGuiFonts* self);

void tbf_gui_text(Font font, const char* text, float x, float y, Color color);
void tbf_gui_char(Font font, int codepoint, float x, float y, Color color);
bool tbf_gui_button(const TbfGuiFonts* fonts, Rectangle rect, const char* label, bool enabled);
void tbf_gui_panel(const TbfGuiFonts* fonts, Rectangle rect, const char* title, Color border);

#endif
