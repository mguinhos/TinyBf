#ifndef TINYBF_GUI_WIDGETS_H
#define TINYBF_GUI_WIDGETS_H

#include <stdbool.h>

#include "raylib.h"

#define UI_FONT_PATH        "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf"
#define UI_FONT_SIZE        18
#define UI_FONT_SIZE_SMALL  13
#define UI_LINE_HEIGHT      18

#define UI_PADDING          10
#define UI_HEADER_HEIGHT    30

#define COLOR_BG            (Color){ 24, 25, 30, 255 }
#define COLOR_PANEL         (Color){ 34, 36, 43, 255 }
#define COLOR_BORDER        (Color){ 52, 55, 66, 255 }
#define COLOR_CELL          (Color){ 44, 47, 56, 255 }
#define COLOR_TEXT          (Color){ 220, 223, 228, 255 }
#define COLOR_DIM           (Color){ 120, 125, 140, 255 }
#define COLOR_ACCENT        (Color){ 97, 175, 239, 255 }
#define COLOR_HIGHLIGHT     (Color){ 229, 192, 123, 255 }
#define COLOR_BREAKPOINT    (Color){ 224, 108, 117, 255 }
#define COLOR_SUCCESS       (Color){ 152, 195, 121, 255 }
#define COLOR_OP_ARITH      (Color){ 152, 195, 121, 255 }
#define COLOR_OP_MOVE       (Color){ 97, 175, 239, 255 }
#define COLOR_OP_LOOP       (Color){ 198, 120, 221, 255 }
#define COLOR_OP_IO         (Color){ 229, 192, 123, 255 }

extern Font ui_font;
extern Font ui_font_small;
extern float ui_char_width;

void ui_load_fonts(void);
void ui_unload_fonts(void);

void ui_text(Font font, const char* text, float x, float y, Color color);
void ui_char(Font font, int codepoint, float x, float y, Color color);
bool ui_button(Rectangle rect, const char* label, bool enabled);
void ui_panel(Rectangle rect, const char* title, Color border);

#endif
