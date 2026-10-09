#include <math.h>
#include <stdlib.h>

#include "tbf/tbf-gui/views.h"

#define TBF_GUI_CODE_VIEW_FEEDBACK  1.5
#define TBF_GUI_CODE_VIEW_SELECTION 0.35f
#define TBF_GUI_CODE_VIEW_LOOP      0.16f

typedef struct TbfGui_CodeLayout {
    Rectangle area;
    int cols;
    int visible_rows;
    int total_rows;
} TbfGui_CodeLayout;

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
    self->pressed_on_cell = false;
    self->copied_at = -TBF_GUI_CODE_VIEW_FEEDBACK;
    tbf_gui_selection_clear(&self->selection);
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

static int tbf_gui_code_view_clamp(int value, int min, int max)
{
    return value < min ? min : value > max ? max : value;
}

static int tbf_gui_code_view_cell_at(const TbfGui_CodeView* self, const TbfGui_Style* style, const TbfGui_CodeLayout* layout, bool* valid)
{
    TbfMath_Vector2 mouse = tbf_gui_mouse_position();
    int col = (int) floorf((mouse.x - layout->area.x) / style->fonts.char_width);
    int row = (int) floorf((mouse.y - layout->area.y) / TBF_GUI_LINE_HEIGHT) + self->scroll;
    long long k = (long long) row * layout->cols + col;

    if (valid != NULL) {
        *valid = col >= 0 && col < layout->cols && row >= 0 && k < (long long) self->size;
    }

    col = tbf_gui_code_view_clamp(col, 0, layout->cols - 1);
    row = tbf_gui_code_view_clamp(row, 0, layout->total_rows - 1);

    return tbf_gui_code_view_clamp(row * layout->cols + col, 0, (int) self->size - 1);
}

static void tbf_gui_code_view_copy(TbfGui_CodeView* self, const TbfGui_Debugger* debugger, bool fallback_all)
{
    int start = 0;
    int end = (int) self->size - 1;

    if (self->selection.active) {
        tbf_gui_selection_range(&self->selection, &start, &end);
    } else if (!fallback_all || self->size == 0) {
        return;
    }

    char* text = malloc((size_t) (end - start + 2));

    if (text == NULL) {
        return;
    }

    for (int k = start; k <= end; k++) {
        text[k - start] = (char) debugger->program.code[self->index[k]];
    }

    text[end - start + 1] = '\0';

    SetClipboardText(text);
    free(text);

    self->copied_at = GetTime();
}

/* Arrastar seleciona; um clique sem arrastar alterna o breakpoint da instrução clicada. */
static void tbf_gui_code_view_interact(TbfGui_CodeView* self, const TbfGui_Style* style, TbfGui_Debugger* debugger, const TbfGui_CodeLayout* layout, Rectangle card)
{
    bool over = tbf_gui_mouse_over(layout->area);
    bool down = IsMouseButtonDown(MOUSE_BUTTON_LEFT);

    if (over || self->selection.selecting) {
        tbf_gui_cursor_request(MOUSE_CURSOR_IBEAM);
    }

    if (over && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        tbf_gui_selection_begin(&self->selection, tbf_gui_code_view_cell_at(self, style, layout, &self->pressed_on_cell));
    }

    if (self->selection.selecting && down) {
        TbfMath_Vector2 mouse = tbf_gui_mouse_position();

        if (mouse.y < layout->area.y) {
            self->scroll--;
            self->follow = false;
        } else if (mouse.y > layout->area.y + layout->area.height) {
            self->scroll++;
            self->follow = false;
        }

        tbf_gui_selection_extend(&self->selection, tbf_gui_code_view_cell_at(self, style, layout, NULL));
    }

    if (self->selection.selecting && !down) {
        if (!self->selection.active && self->pressed_on_cell) {
            tbf_gui_debugger_toggle_breakpoint(debugger, self->index[self->selection.anchor]);
        }

        tbf_gui_selection_end(&self->selection);
    }

    bool ctrl = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);

    if (ctrl && IsKeyPressed(KEY_A) && tbf_gui_mouse_over(card)) {
        tbf_gui_selection_all(&self->selection, (int) self->size);
    }

    if (ctrl && IsKeyPressed(KEY_C)) {
        tbf_gui_code_view_copy(self, debugger, false);
    }
}

static void tbf_gui_code_view_draw_header(TbfGui_CodeView* self, const TbfGui_Style* style, const TbfGui_Debugger* debugger, Rectangle rect)
{
    TbfGui_Stack actions = tbf_gui_card_actions(rect);
    Rectangle copy = tbf_gui_stack_next(&actions, 40);

    if (tbf_gui_icon_button(style, copy, TBF_GUI_ICON_COPY, debugger->loaded)) {
        tbf_gui_code_view_copy(self, debugger, true);
    }

    if (GetTime() - self->copied_at < TBF_GUI_CODE_VIEW_FEEDBACK) {
        const char* label = "Copiado";

        tbf_gui_stack_skip(&actions, 4);
        tbf_gui_label(style->fonts.body, label, tbf_gui_stack_next(&actions, tbf_gui_text_size(style->fonts.body, label).x), TBF_GUI_ALIGN_START, style->theme->primary);
    }
}

static bool tbf_gui_code_view_draw_placeholder(const TbfGui_Style* style, const TbfGui_Debugger* debugger, Rectangle area)
{
    const TbfGui_Theme* theme = style->theme;

    if (debugger->error[0] && !debugger->loaded) {
        tbf_gui_text(style->fonts.label, "Erro ao carregar", area.x, area.y + 4, theme->error);
        tbf_gui_text(style->fonts.body, debugger->error, area.x, area.y + 28, theme->on_surface);
        return true;
    }

    if (!debugger->loaded) {
        tbf_gui_skeleton_text(style, area, TBF_GUI_LINE_HEIGHT + 6, 1);
        return true;
    }

    return false;
}

static void tbf_gui_code_view_draw_scrollbar(const TbfGui_CodeView* self, const TbfGui_Theme* theme, Rectangle rect, const TbfGui_CodeLayout* layout)
{
    if (layout->total_rows <= layout->visible_rows) {
        return;
    }

    Rectangle area = layout->area;
    float bar_height = area.height * layout->visible_rows / layout->total_rows;
    float bar_y = area.y + (area.height - bar_height) * self->scroll / (layout->total_rows - layout->visible_rows);

    tbf_gui_rounded((Rectangle) { rect.x + rect.width - 10, bar_y, 4, bar_height }, 2, theme->outline_variant);
}

static void tbf_gui_code_view_draw_selection(const TbfGui_CodeView* self, const TbfGui_Style* style, const TbfGui_CodeLayout* layout)
{
    const TbfGui_Theme* theme = style->theme;

    TbfGui_SelectionGrid grid = {
        .x = layout->area.x,
        .y = layout->area.y - self->scroll * TBF_GUI_LINE_HEIGHT,
        .char_width = style->fonts.char_width,
        .line_height = TBF_GUI_LINE_HEIGHT,
        .cols = layout->cols,
        .first_row = self->scroll - 1,
        .last_row = self->scroll + layout->visible_rows,
    };

    tbf_gui_selection_draw(&self->selection, &grid, ColorLerp(theme->surface_container, theme->primary, TBF_GUI_CODE_VIEW_SELECTION), theme->surface_container);
}

void tbf_gui_code_view_draw(TbfGui_CodeView* self, const TbfGui_Style* style, TbfGui_Debugger* debugger, Rectangle rect)
{
    const TbfGui_Theme* theme = style->theme;
    const char* subtitle = debugger->path[0] ? GetFileName(debugger->path) : "Arraste um arquivo .bf para a janela";

    tbf_gui_card(style, rect, "Programa", subtitle);
    tbf_gui_code_view_draw_header(self, style, debugger, rect);

    TbfGui_CodeLayout layout = {
        .area = tbf_gui_box(tbf_gui_card_body(rect), (TbfGui_Insets) { 0, 6, 0, 0 }),
    };

    if (tbf_gui_code_view_draw_placeholder(style, debugger, layout.area)) {
        return;
    }

    const TbfVm* vm = debugger->vm;
    const TbfByte* code = debugger->program.code;
    Rectangle area = layout.area;

    layout.cols = (int) (area.width / style->fonts.char_width);
    layout.visible_rows = (int) (area.height / TBF_GUI_LINE_HEIGHT);
    layout.total_rows = (int) ((self->size + layout.cols - 1) / layout.cols);

    int cols = layout.cols;
    int max_scroll = layout.total_rows > layout.visible_rows ? layout.total_rows - layout.visible_rows : 0;

    size_t ip_pos = vm ? tbf_gui_code_view_position(self, vm->ip) : self->size;
    bool show_ip = vm && vm->running && ip_pos < self->size;
    int ip_row = (int) (ip_pos / cols);

    size_t loop_start = 0;
    size_t loop_end = 0;
    bool show_loop = tbf_gui_code_view_loop(self, debugger, &loop_start, &loop_end);

    float wheel = tbf_gui_mouse_over(area) ? GetMouseWheelMove() : 0;

    if (wheel != 0) {
        self->scroll -= (int) (wheel * 3);
        self->follow = false;
    }

    if (self->follow && show_ip && (ip_row < self->scroll || ip_row >= self->scroll + layout.visible_rows)) {
        self->scroll = ip_row - layout.visible_rows / 2;
    }

    if (self->size > 0) {
        tbf_gui_code_view_interact(self, style, debugger, &layout, rect);
    }

    self->scroll = tbf_gui_code_view_clamp(self->scroll, 0, max_scroll);

    BeginScissorMode((int) area.x, (int) area.y, (int) area.width, (int) area.height);

    tbf_gui_code_view_draw_selection(self, style, &layout);

    for (int row = self->scroll; row < self->scroll + layout.visible_rows && row < layout.total_rows; row++) {
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
                DrawRectangleRec(cell, Fade(theme->primary, TBF_GUI_CODE_VIEW_LOOP));
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

    tbf_gui_code_view_draw_scrollbar(self, theme, rect, &layout);
}
