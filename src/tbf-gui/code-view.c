#include <stdlib.h>

#include "tbf/tbf-gui/views.h"

static Color tbf_gui_code_view_color(const TbfGui_Theme* theme, TbfByte opcode)
{
    switch (opcode) {
    case '+':
    case '-':
        return theme->code_arith;

    case '<':
    case '>':
        return theme->code_move;

    case '[':
    case ']':
        return theme->code_loop;

    default:
        return theme->code_io;
    }
}

static size_t tbf_gui_code_view_position(const TbfGui_CodeView* self, size_t ip)
{
    size_t lo = 0;
    size_t hi = self->size;

    while (lo < hi) {
        size_t mid = (lo + hi) / 2;

        if (self->index[mid] < ip) {
            lo = mid + 1;
        } else {
            hi = mid;
        }
    }

    return lo;
}

static bool tbf_gui_code_view_loop(const TbfGui_CodeView* self, const TbfGui_Debugger* debugger, size_t* start, size_t* end)
{
    const TbfVm* vm = debugger->vm;

    if (vm == NULL || !vm->running || vm->sp == 0) {
        return false;
    }

    TbfDaddr open = vm->stack[vm->sp - 1];

    *start = tbf_gui_code_view_position(self, open);
    *end = tbf_gui_code_view_position(self, debugger->match[open]);

    return *start < self->size;
}

void tbf_gui_code_view_init(TbfGui_CodeView* self)
{
    self->index = NULL;
    self->size = 0;
    self->scroll = 0;
    self->follow = true;
}

void tbf_gui_code_view_load(TbfGui_CodeView* self, const TbfProgram* program)
{
    tbf_gui_code_view_free(self);

    if (program == NULL) {
        return;
    }

    self->index = malloc(sizeof(size_t) * (program->size + 1));

    if (self->index == NULL) {
        return;
    }

    for (size_t i = 0; i < program->size; i++) {
        if (tbf_opcode_is_valid(program->code[i])) {
            self->index[self->size++] = i;
        }
    }
}

void tbf_gui_code_view_free(TbfGui_CodeView* self)
{
    free(self->index);
    tbf_gui_code_view_init(self);
}

void tbf_gui_code_view_follow(TbfGui_CodeView* self)
{
    self->follow = true;
}

static void tbf_gui_code_view_click(TbfGui_CodeView* self, const TbfGui_Style* style, TbfGui_Debugger* debugger, Rectangle area, int cols)
{
    TbfMath_Vector2 mouse = tbf_gui_mouse_position();
    int col = (int) ((mouse.x - area.x) / style->fonts.char_width);
    int row = (int) ((mouse.y - area.y) / TBF_GUI_LINE_HEIGHT) + self->scroll;
    size_t k = (size_t) row * cols + col;

    if (col < cols && k < self->size) {
        tbf_gui_debugger_toggle_breakpoint(debugger, self->index[k]);
    }
}

static bool tbf_gui_code_view_draw_message(const TbfGui_Style* style, const TbfGui_Debugger* debugger, Rectangle area)
{
    const TbfGui_Theme* theme = style->theme;

    if (debugger->error[0] && !debugger->loaded) {
        tbf_gui_text(style->fonts.label, "Erro ao carregar", area.x, area.y + 4, theme->error);
        tbf_gui_text(style->fonts.body, debugger->error, area.x, area.y + 28, theme->on_surface);
        return true;
    }

    if (!debugger->loaded) {
        tbf_gui_text(style->fonts.body, "Arraste um arquivo .bf para a janela", area.x, area.y + 4, theme->on_surface_variant);
        return true;
    }

    return false;
}

static void tbf_gui_code_view_draw_scrollbar(const TbfGui_CodeView* self, const TbfGui_Theme* theme, Rectangle rect, Rectangle area, int visible_rows, int total_rows)
{
    if (total_rows <= visible_rows) {
        return;
    }

    float bar_height = area.height * visible_rows / total_rows;
    float bar_y = area.y + (area.height - bar_height) * self->scroll / (total_rows - visible_rows);

    tbf_gui_rounded((Rectangle) { rect.x + rect.width - 10, bar_y, 4, bar_height }, 2, theme->outline_variant);
}

void tbf_gui_code_view_draw(TbfGui_CodeView* self, const TbfGui_Style* style, TbfGui_Debugger* debugger, Rectangle rect)
{
    const TbfGui_Theme* theme = style->theme;
    const char* subtitle = debugger->path[0] ? GetFileName(debugger->path) : NULL;

    tbf_gui_card(style, rect, "Programa", subtitle);

    Rectangle area = {
        rect.x + TBF_GUI_CARD_PADDING,
        rect.y + TBF_GUI_CARD_HEADER,
        rect.width - 2 * TBF_GUI_CARD_PADDING - 6,
        rect.height - TBF_GUI_CARD_HEADER - TBF_GUI_CARD_PADDING,
    };

    if (tbf_gui_code_view_draw_message(style, debugger, area)) {
        return;
    }

    const TbfVm* vm = debugger->vm;
    const TbfByte* code = debugger->program.code;

    int cols = (int) (area.width / style->fonts.char_width);
    int visible_rows = (int) (area.height / TBF_GUI_LINE_HEIGHT);
    int total_rows = (int) ((self->size + cols - 1) / cols);
    int max_scroll = total_rows > visible_rows ? total_rows - visible_rows : 0;

    size_t ip_pos = vm ? tbf_gui_code_view_position(self, vm->ip) : self->size;
    bool show_ip = vm && vm->running && ip_pos < self->size;
    int ip_row = (int) (ip_pos / cols);

    size_t loop_start = 0;
    size_t loop_end = 0;
    bool show_loop = tbf_gui_code_view_loop(self, debugger, &loop_start, &loop_end);

    bool hover = tbf_gui_mouse_over(area);
    float wheel = hover ? GetMouseWheelMove() : 0;

    if (wheel != 0) {
        self->scroll -= (int) (wheel * 3);
        self->follow = false;
    }

    if (self->follow && show_ip && (ip_row < self->scroll || ip_row >= self->scroll + visible_rows)) {
        self->scroll = ip_row - visible_rows / 2;
    }

    if (self->scroll > max_scroll) {
        self->scroll = max_scroll;
    }

    if (self->scroll < 0) {
        self->scroll = 0;
    }

    if (hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        tbf_gui_code_view_click(self, style, debugger, area, cols);
    }

    BeginScissorMode((int) area.x, (int) area.y, (int) area.width, (int) area.height);

    for (int row = self->scroll; row < self->scroll + visible_rows && row < total_rows; row++) {
        for (int col = 0; col < cols; col++) {
            size_t k = (size_t) row * cols + col;

            if (k >= self->size) {
                break;
            }

            size_t index = self->index[k];
            Rectangle cell = {
                area.x + col * style->fonts.char_width,
                area.y + (row - self->scroll) * TBF_GUI_LINE_HEIGHT,
                style->fonts.char_width,
                TBF_GUI_LINE_HEIGHT,
            };
            Color color = tbf_gui_code_view_color(theme, code[index]);

            if (show_loop && k >= loop_start && k <= loop_end) {
                DrawRectangleRec(cell, Fade(theme->primary, 0.16f));
            }

            if (debugger->breakpoints[index]) {
                tbf_gui_rounded(cell, 3, theme->error_container);
                color = theme->on_error_container;
            }

            if (show_ip && k == ip_pos) {
                tbf_gui_rounded(cell, 3, theme->primary);
                color = theme->on_primary;
            }

            tbf_gui_char(style->fonts.mono, code[index], cell.x, cell.y, color);
        }
    }

    EndScissorMode();

    tbf_gui_code_view_draw_scrollbar(self, theme, rect, area, visible_rows, total_rows);
}
