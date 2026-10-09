#include "tbf/tbf-gui/views.h"

#define TBF_GUI_TAPE_VIEW_COLS  16
#define TBF_GUI_TAPE_VIEW_ROWS  2
#define TBF_GUI_TAPE_VIEW_GAP   4

static void tbf_gui_tape_view_draw_cell(const TbfGuiFonts* fonts, Rectangle cell, size_t addr, TbfCell value, bool current)
{
    Color bg = current ? TBF_GUI_COLOR_ACCENT : value ? TBF_GUI_COLOR_BORDER : TBF_GUI_COLOR_CELL;
    Color fg = current ? TBF_GUI_COLOR_BG : value ? TBF_GUI_COLOR_TEXT : TBF_GUI_COLOR_DIM;

    DrawRectangleRounded(cell, 0.15f, 4, bg);

    tbf_gui_text(fonts->small, TextFormat("%04zX", addr), cell.x + 4, cell.y + 3, current ? TBF_GUI_COLOR_BG : TBF_GUI_COLOR_DIM);

    const char* text = TextFormat("%d", value);
    Vector2 size = MeasureTextEx(fonts->regular, text, fonts->regular.baseSize, 0);

    tbf_gui_text(fonts->regular, text, cell.x + (cell.width - size.x) / 2, cell.y + (cell.height - size.y) / 2 + 2, fg);

    if (value > 0x20 && value < 0x7f) {
        tbf_gui_char(fonts->small, value, cell.x + cell.width - 12, cell.y + cell.height - 16, fg);
    }
}

void tbf_gui_tape_view_draw(const TbfGuiFonts* fonts, const TbfVm* vm, Rectangle rect)
{
    tbf_gui_panel(fonts, rect, "MEMÓRIA", TBF_GUI_COLOR_BORDER);

    const int per_page = TBF_GUI_TAPE_VIEW_COLS * TBF_GUI_TAPE_VIEW_ROWS;
    const float gap = TBF_GUI_TAPE_VIEW_GAP;

    TbfAddr tp = vm ? vm->tp : 0;
    size_t start = (tp / per_page) * per_page;

    float width = (rect.width - 2 * TBF_GUI_PADDING - (TBF_GUI_TAPE_VIEW_COLS - 1) * gap) / TBF_GUI_TAPE_VIEW_COLS;
    float height = (rect.height - TBF_GUI_HEADER_HEIGHT - TBF_GUI_PADDING - (TBF_GUI_TAPE_VIEW_ROWS - 1) * gap) / TBF_GUI_TAPE_VIEW_ROWS;

    for (int i = 0; i < per_page; i++) {
        size_t addr = start + i;

        Rectangle cell = {
            rect.x + TBF_GUI_PADDING + (i % TBF_GUI_TAPE_VIEW_COLS) * (width + gap),
            rect.y + TBF_GUI_HEADER_HEIGHT + (i / TBF_GUI_TAPE_VIEW_COLS) * (height + gap),
            width,
            height,
        };

        tbf_gui_tape_view_draw_cell(fonts, cell, addr, vm ? vm->tape[addr] : 0, vm && addr == tp);
    }
}
