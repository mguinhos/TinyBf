#include "tbf/tbf-gui/views.h"

void tbf_gui_output_view_draw(const TbfGui_Style* style, const TbfGui_Terminal* terminal, bool waiting, Rectangle rect)
{
    const TbfGui_Theme* theme = style->theme;
    const float char_width = style->fonts.char_width;

    tbf_gui_card(style, rect, "Saída", NULL);

    float x = rect.x + TBF_GUI_CARD_PADDING;
    float y = rect.y + TBF_GUI_CARD_HEADER;

    for (int row = 0; row < TBF_GUI_TERMINAL_ROWS; row++) {
        for (int col = 0; col < TBF_GUI_TERMINAL_COLS; col++) {
            unsigned char c = terminal->cells[row][col];

            if (c == ' ') {
                continue;
            }

            int codepoint = (c >= 0x20 && c < 0x7f) ? c : '?';

            tbf_gui_char(style->fonts.mono, codepoint, x + col * char_width, y + row * TBF_GUI_LINE_HEIGHT, theme->on_surface);
        }
    }

    if (waiting && (int) (GetTime() * 2) % 2 == 0) {
        DrawRectangle(
            x + terminal->cx * char_width,
            y + terminal->cy * TBF_GUI_LINE_HEIGHT,
            2,
            TBF_GUI_LINE_HEIGHT,
            theme->primary
        );
    }
}
