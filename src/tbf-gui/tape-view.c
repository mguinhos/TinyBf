#include "tbf/tbf-gui/views.h"

#define TBF_GUI_TAPE_VIEW_COLS  16
#define TBF_GUI_TAPE_VIEW_ROWS  2
#define TBF_GUI_TAPE_VIEW_GAP   6

static void tbf_gui_tape_view_draw_cell(const TbfGui_Style* style, Rectangle cell, size_t addr, TbfCell value, bool current)
{
    const TbfGui_Theme* theme = style->theme;

    Color bg = theme->surface_container_highest;
    Color fg = theme->on_surface_variant;

    if (current) {
        bg = theme->primary;
        fg = theme->on_primary;
    } else if (value) {
        bg = theme->secondary_container;
        fg = theme->on_secondary_container;
    }

    tbf_gui_rounded(cell, 8, bg);
    tbf_gui_text(style->fonts.mono_small, TextFormat("%04zX", addr), cell.x + 6, cell.y + 4, Fade(fg, 0.7f));

    const char* text = TextFormat("%d", value);
    TbfMath_Vector2 size = tbf_gui_text_size(style->fonts.mono, text);

    tbf_gui_text(style->fonts.mono, text, cell.x + (cell.width - size.x) / 2, cell.y + (cell.height - size.y) / 2 + 4, fg);

    if (value > 0x20 && value < 0x7f) {
        tbf_gui_char(style->fonts.mono_small, value, cell.x + cell.width - 12, cell.y + 4, fg);
    }
}

void tbf_gui_tape_view_draw(const TbfGui_Style* style, const TbfVm* vm, Rectangle rect)
{
    const int per_page = TBF_GUI_TAPE_VIEW_COLS * TBF_GUI_TAPE_VIEW_ROWS;
    const float gap = TBF_GUI_TAPE_VIEW_GAP;

    TbfAddr tp = vm ? vm->tp : 0;
    size_t start = (tp / per_page) * per_page;

    tbf_gui_card(style, rect, "Memória", TextFormat("%04zX - %04zX", start, start + per_page - 1));

    float width = (rect.width - 2 * TBF_GUI_CARD_PADDING - (TBF_GUI_TAPE_VIEW_COLS - 1) * gap) / TBF_GUI_TAPE_VIEW_COLS;
    float height = (rect.height - TBF_GUI_CARD_HEADER - TBF_GUI_CARD_PADDING - (TBF_GUI_TAPE_VIEW_ROWS - 1) * gap) / TBF_GUI_TAPE_VIEW_ROWS;

    for (int i = 0; i < per_page; i++) {
        size_t addr = start + i;

        Rectangle cell = {
            rect.x + TBF_GUI_CARD_PADDING + (i % TBF_GUI_TAPE_VIEW_COLS) * (width + gap),
            rect.y + TBF_GUI_CARD_HEADER + (i / TBF_GUI_TAPE_VIEW_COLS) * (height + gap),
            width,
            height,
        };

        tbf_gui_tape_view_draw_cell(style, cell, addr, vm ? vm->tape[addr] : 0, vm && addr == tp);
    }
}
