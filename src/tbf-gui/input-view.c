#include "tbf/tbf-gui/views.h"

void tbf_gui_input_view_draw(const TbfGui_Style* style, const TbfGui_Input* input, bool waiting, Rectangle rect)
{
    const TbfGui_Theme* theme = style->theme;
    const float char_width = style->fonts.char_width;

    tbf_gui_text_field(style, rect, "Entrada", waiting);

    float x = rect.x + TBF_GUI_CARD_PADDING;
    float y = rect.y + (rect.height - TBF_GUI_LINE_HEIGHT) / 2;

    if (tbf_gui_input_is_empty(input)) {
        const char* hint = waiting
            ? "Digite algo: o programa está esperando um ','"
            : "Digite para enfileirar entrada (Enter = \\n, Backspace apaga)";
        TbfMath_Vector2 size = tbf_gui_text_size(style->fonts.body, hint);

        tbf_gui_text(style->fonts.body, hint, x, rect.y + (rect.height - size.y) / 2, waiting ? theme->primary : theme->on_surface_variant);
        return;
    }

    float max_x = rect.x + rect.width - TBF_GUI_CARD_PADDING - 2 * char_width;

    for (int i = 0; i < input->length && x < max_x; i++) {
        TbfByte c = tbf_gui_input_peek(input, i);

        if (c == '\n') {
            tbf_gui_text(style->fonts.mono, "\\n", x, y, theme->on_surface_variant);
            x += 2 * char_width;
        } else {
            int codepoint = (c >= 0x20 && c < 0x7f) ? c : '?';

            tbf_gui_char(style->fonts.mono, codepoint, x, y, theme->on_surface);
            x += char_width;
        }
    }
}
