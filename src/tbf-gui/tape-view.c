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

    Rectangle inner = tbf_gui_box(cell, (TbfGui_Insets) { 4, 6, 4, 6 });
    TbfGui_Stack stack = tbf_gui_stack_begin(inner, TBF_GUI_STACK_VERTICAL, 0);
    Rectangle header = tbf_gui_stack_next(&stack, style->fonts.mono_small.baseSize);

    tbf_gui_rounded(cell, 8, bg);
    tbf_gui_label(style->fonts.mono_small, TextFormat("%04zX", addr), header, TBF_GUI_ALIGN_START, Fade(fg, 0.7f));

    if (value > 0x20 && value < 0x7f) {
        tbf_gui_label(style->fonts.mono_small, TextFormat("%c", value), header, TBF_GUI_ALIGN_END, fg);
    }

    tbf_gui_label(style->fonts.mono, TextFormat("%d", value), tbf_gui_stack_rest(&stack), TBF_GUI_ALIGN_CENTER, fg);
}

void tbf_gui_tape_view_draw(const TbfGui_Style* style, const TbfVm* vm, Rectangle rect)
{
    const int per_page = TBF_GUI_TAPE_VIEW_COLS * TBF_GUI_TAPE_VIEW_ROWS;

    TbfAddr tp = vm ? vm->tp : 0;
    size_t start = (tp / per_page) * per_page;

    tbf_gui_card(style, rect, "Memória", vm ? TextFormat("%04zX - %04zX", start, start + per_page - 1) : NULL);

    Rectangle body = tbf_gui_card_body(rect);
    float row_height = (body.height - (TBF_GUI_TAPE_VIEW_ROWS - 1) * TBF_GUI_TAPE_VIEW_GAP) / TBF_GUI_TAPE_VIEW_ROWS;
    TbfGui_Grid grid = tbf_gui_grid_begin(body, TBF_GUI_TAPE_VIEW_COLS, TBF_GUI_TAPE_VIEW_GAP, row_height);

    for (int i = 0; i < per_page; i++) {
        size_t addr = start + i;
        Rectangle cell = tbf_gui_grid_item(&grid, 1);

        if (vm == NULL) {
            tbf_gui_skeleton(style, cell, 8);
            continue;
        }

        tbf_gui_tape_view_draw_cell(style, cell, addr, vm->tape[addr], addr == tp);
    }
}
