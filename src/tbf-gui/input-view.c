#include "tbf/tbf-gui/views.h"

void tbf_gui_input_view_draw(const TbfGuiFonts* fonts, const TbfGuiInput* input, bool waiting, Rectangle rect)
{
    tbf_gui_panel(fonts, rect, "ENTRADA", waiting ? TBF_GUI_COLOR_ACCENT : TBF_GUI_COLOR_BORDER);

    float x = rect.x + TBF_GUI_PADDING + 80;
    float y = rect.y + (rect.height - TBF_GUI_LINE_HEIGHT) / 2;

    if (tbf_gui_input_is_empty(input)) {
        const char* hint = waiting
            ? "Digite algo: o programa está esperando um ','"
            : "Digite para enfileirar entrada (Enter = \\n, Backspace apaga)";

        tbf_gui_text(fonts->small, hint, x, y + 3, waiting ? TBF_GUI_COLOR_ACCENT : TBF_GUI_COLOR_DIM);
        return;
    }

    float max_x = rect.x + rect.width - TBF_GUI_PADDING - 2 * fonts->char_width;

    for (int i = 0; i < input->length && x < max_x; i++) {
        TbfByte c = tbf_gui_input_peek(input, i);

        if (c == '\n') {
            tbf_gui_text(fonts->regular, "\\n", x, y, TBF_GUI_COLOR_DIM);
            x += 2 * fonts->char_width;
        } else {
            int codepoint = (c >= 0x20 && c < 0x7f) ? c : '?';

            tbf_gui_char(fonts->regular, codepoint, x, y, TBF_GUI_COLOR_TEXT);
            x += fonts->char_width;
        }
    }
}
