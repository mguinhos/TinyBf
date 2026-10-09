#include "tbf/tbf-gui/views.h"

void tbf_gui_output_view_draw(const TbfGuiFonts* fonts, const TbfGuiTerminal* terminal, bool waiting, Rectangle rect)
{
    tbf_gui_panel(fonts, rect, "SAÍDA", TBF_GUI_COLOR_BORDER);

    float x = rect.x + TBF_GUI_PADDING;
    float y = rect.y + TBF_GUI_HEADER_HEIGHT;

    for (int row = 0; row < TBF_GUI_TERMINAL_ROWS; row++) {
        for (int col = 0; col < TBF_GUI_TERMINAL_COLS; col++) {
            unsigned char c = terminal->cells[row][col];

            if (c == ' ') {
                continue;
            }

            int codepoint = (c >= 0x20 && c < 0x7f) ? c : '?';

            tbf_gui_char(fonts->regular, codepoint, x + col * fonts->char_width, y + row * TBF_GUI_LINE_HEIGHT, TBF_GUI_COLOR_TEXT);
        }
    }

    if (waiting && (int) (GetTime() * 2) % 2 == 0) {
        DrawRectangle(
            x + terminal->cx * fonts->char_width,
            y + terminal->cy * TBF_GUI_LINE_HEIGHT,
            fonts->char_width,
            TBF_GUI_LINE_HEIGHT,
            TBF_GUI_COLOR_ACCENT
        );
    }
}
