#include <stdlib.h>

#include "tbf/tbf-gui/views.h"

static Color tbf_gui_code_view_color(TbfByte opcode)
{
    switch (opcode) {
    case '+':
    case '-':
        return TBF_GUI_COLOR_OP_ARITH;

    case '<':
    case '>':
        return TBF_GUI_COLOR_OP_MOVE;

    case '[':
    case ']':
        return TBF_GUI_COLOR_OP_LOOP;

    default:
        return TBF_GUI_COLOR_OP_IO;
    }
}

static size_t tbf_gui_code_view_position(const TbfGuiCodeView* self, size_t ip)
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

void tbf_gui_code_view_init(TbfGuiCodeView* self)
{
    self->index = NULL;
    self->size = 0;
    self->scroll = 0;
    self->follow = true;
}

void tbf_gui_code_view_load(TbfGuiCodeView* self, const TbfProgram* program)
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

void tbf_gui_code_view_free(TbfGuiCodeView* self)
{
    free(self->index);
    tbf_gui_code_view_init(self);
}

void tbf_gui_code_view_follow(TbfGuiCodeView* self)
{
    self->follow = true;
}

static void tbf_gui_code_view_click(TbfGuiCodeView* self, const TbfGuiFonts* fonts, TbfGuiDebugger* debugger, Rectangle area, int cols)
{
    Vector2 mouse = GetMousePosition();
    int col = (int) ((mouse.x - area.x) / fonts->char_width);
    int row = (int) ((mouse.y - area.y) / TBF_GUI_LINE_HEIGHT) + self->scroll;
    size_t k = (size_t) row * cols + col;

    if (col < cols && k < self->size) {
        tbf_gui_debugger_toggle_breakpoint(debugger, self->index[k]);
    }
}

static bool tbf_gui_code_view_draw_message(const TbfGuiFonts* fonts, const TbfGuiDebugger* debugger, Rectangle area)
{
    if (debugger->error[0] && !debugger->loaded) {
        tbf_gui_text(fonts->regular, "Erro ao carregar:", area.x, area.y + 4, TBF_GUI_COLOR_BREAKPOINT);
        tbf_gui_text(fonts->small, debugger->error, area.x, area.y + 28, TBF_GUI_COLOR_TEXT);
        return true;
    }

    if (!debugger->loaded) {
        tbf_gui_text(fonts->regular, "Arraste um arquivo .bf para a janela", area.x, area.y + 4, TBF_GUI_COLOR_DIM);
        return true;
    }

    return false;
}

static void tbf_gui_code_view_draw_scrollbar(const TbfGuiCodeView* self, Rectangle rect, Rectangle area, int visible_rows, int total_rows)
{
    if (total_rows <= visible_rows) {
        return;
    }

    float bar_height = area.height * visible_rows / total_rows;
    float bar_y = area.y + (area.height - bar_height) * self->scroll / (total_rows - visible_rows);

    DrawRectangleRounded((Rectangle) { rect.x + rect.width - 8, bar_y, 4, bar_height }, 1, 4, TBF_GUI_COLOR_BORDER);
}

void tbf_gui_code_view_draw(TbfGuiCodeView* self, const TbfGuiFonts* fonts, TbfGuiDebugger* debugger, Rectangle rect)
{
    const char* title = debugger->path[0] ? TextFormat("PROGRAMA: %s", GetFileName(debugger->path)) : "PROGRAMA";

    tbf_gui_panel(fonts, rect, title, TBF_GUI_COLOR_BORDER);

    Rectangle area = {
        rect.x + TBF_GUI_PADDING,
        rect.y + TBF_GUI_HEADER_HEIGHT,
        rect.width - 2 * TBF_GUI_PADDING - 6,
        rect.height - TBF_GUI_HEADER_HEIGHT - TBF_GUI_PADDING,
    };

    if (tbf_gui_code_view_draw_message(fonts, debugger, area)) {
        return;
    }

    const TbfVm* vm = debugger->vm;
    const TbfByte* code = debugger->program.code;

    int cols = (int) (area.width / fonts->char_width);
    int visible_rows = (int) (area.height / TBF_GUI_LINE_HEIGHT);
    int total_rows = (int) ((self->size + cols - 1) / cols);
    int max_scroll = total_rows > visible_rows ? total_rows - visible_rows : 0;

    size_t ip_pos = vm ? tbf_gui_code_view_position(self, vm->ip) : self->size;
    bool show_ip = vm && vm->running && ip_pos < self->size;
    int ip_row = (int) (ip_pos / cols);

    bool hover = CheckCollisionPointRec(GetMousePosition(), area);
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
        tbf_gui_code_view_click(self, fonts, debugger, area, cols);
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
                area.x + col * fonts->char_width,
                area.y + (row - self->scroll) * TBF_GUI_LINE_HEIGHT,
                fonts->char_width,
                TBF_GUI_LINE_HEIGHT,
            };
            Color color = tbf_gui_code_view_color(code[index]);

            if (debugger->breakpoints[index]) {
                DrawRectangleRec(cell, Fade(TBF_GUI_COLOR_BREAKPOINT, 0.45f));
            }

            if (show_ip && k == ip_pos) {
                DrawRectangleRec(cell, TBF_GUI_COLOR_HIGHLIGHT);
                color = TBF_GUI_COLOR_BG;
            }

            tbf_gui_char(fonts->regular, code[index], cell.x, cell.y, color);
        }
    }

    EndScissorMode();

    tbf_gui_code_view_draw_scrollbar(self, rect, area, visible_rows, total_rows);
}
