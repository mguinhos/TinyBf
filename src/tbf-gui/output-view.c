#include <math.h>

#include "tbf/tbf-gui/views.h"

#define TBF_GUI_OUTPUT_VIEW_CELLS       (TBF_GUI_TERMINAL_ROWS * TBF_GUI_TERMINAL_COLS)
#define TBF_GUI_OUTPUT_VIEW_TEXT_SIZE   (TBF_GUI_TERMINAL_ROWS * (TBF_GUI_TERMINAL_COLS + 1) + 1)
#define TBF_GUI_OUTPUT_VIEW_FEEDBACK    1.5
#define TBF_GUI_OUTPUT_VIEW_SELECTION   0.35f
#define TBF_GUI_OUTPUT_VIEW_RADIUS      0.25f

typedef struct TbfGui_OutputGrid {
    Font font;
    float x;
    float y;
    float char_width;
    float line_height;
} TbfGui_OutputGrid;

void tbf_gui_output_view_init(TbfGui_OutputView* self)
{
    self->fullscreen = false;
    self->selecting = false;
    self->has_selection = false;
    self->anchor = 0;
    self->head = 0;
    self->copied_at = -TBF_GUI_OUTPUT_VIEW_FEEDBACK;
}

void tbf_gui_output_view_toggle_fullscreen(TbfGui_OutputView* self)
{
    self->fullscreen = !self->fullscreen;
}

static int tbf_gui_output_view_clamp(int value, int min, int max)
{
    return value < min ? min : value > max ? max : value;
}

static unsigned char tbf_gui_output_view_printable(unsigned char c)
{
    return (c >= 0x20 && c < 0x7f) ? c : '?';
}

static TbfGui_OutputGrid tbf_gui_output_view_grid(const TbfGui_OutputView* self, const TbfGui_Style* style, Rectangle rect)
{
    TbfGui_OutputGrid grid = {
        .font = self->fullscreen ? style->fonts.mono_large : style->fonts.mono,
        .char_width = self->fullscreen ? style->fonts.char_width_large : style->fonts.char_width,
        .line_height = self->fullscreen ? TBF_GUI_LINE_HEIGHT_LARGE : TBF_GUI_LINE_HEIGHT,
        .x = rect.x + TBF_GUI_CARD_PADDING,
        .y = rect.y + TBF_GUI_CARD_HEADER,
    };

    if (self->fullscreen) {
        float body_height = rect.height - TBF_GUI_CARD_HEADER - TBF_GUI_CARD_PADDING;

        grid.x = rect.x + (rect.width - TBF_GUI_TERMINAL_COLS * grid.char_width) / 2;
        grid.y += (body_height - TBF_GUI_TERMINAL_ROWS * grid.line_height) / 2;
    }

    return grid;
}

static Rectangle tbf_gui_output_view_grid_rect(const TbfGui_OutputGrid* grid)
{
    return (Rectangle) {
        grid->x,
        grid->y,
        TBF_GUI_TERMINAL_COLS * grid->char_width,
        TBF_GUI_TERMINAL_ROWS * grid->line_height,
    };
}

static int tbf_gui_output_view_cell_at(const TbfGui_OutputGrid* grid, TbfMath_Vector2 point)
{
    int col = tbf_gui_output_view_clamp((int) ((point.x - grid->x) / grid->char_width), 0, TBF_GUI_TERMINAL_COLS - 1);
    int row = tbf_gui_output_view_clamp((int) ((point.y - grid->y) / grid->line_height), 0, TBF_GUI_TERMINAL_ROWS - 1);

    return row * TBF_GUI_TERMINAL_COLS + col;
}

static void tbf_gui_output_view_range(const TbfGui_OutputView* self, int* start, int* end)
{
    *start = self->anchor < self->head ? self->anchor : self->head;
    *end = self->anchor < self->head ? self->head : self->anchor;
}

static void tbf_gui_output_view_copy(TbfGui_OutputView* self, const TbfGui_Terminal* terminal, int start, int end)
{
    static char text[TBF_GUI_OUTPUT_VIEW_TEXT_SIZE];
    int length = 0;

    for (int row = start / TBF_GUI_TERMINAL_COLS; row <= end / TBF_GUI_TERMINAL_COLS; row++) {
        int first = row == start / TBF_GUI_TERMINAL_COLS ? start % TBF_GUI_TERMINAL_COLS : 0;
        int last = row == end / TBF_GUI_TERMINAL_COLS ? end % TBF_GUI_TERMINAL_COLS : TBF_GUI_TERMINAL_COLS - 1;

        while (last >= first && terminal->cells[row][last] == ' ') {
            last--;
        }

        if (row != start / TBF_GUI_TERMINAL_COLS) {
            text[length++] = '\n';
        }

        for (int col = first; col <= last; col++) {
            text[length++] = tbf_gui_output_view_printable(terminal->cells[row][col]);
        }
    }

    while (length > 0 && text[length - 1] == '\n') {
        length--;
    }

    text[length] = '\0';

    SetClipboardText(text);
    self->copied_at = GetTime();
}

static void tbf_gui_output_view_copy_selection(TbfGui_OutputView* self, const TbfGui_Terminal* terminal, bool fallback_all)
{
    int start = 0;
    int end = TBF_GUI_OUTPUT_VIEW_CELLS - 1;

    if (self->has_selection) {
        tbf_gui_output_view_range(self, &start, &end);
    } else if (!fallback_all) {
        return;
    }

    tbf_gui_output_view_copy(self, terminal, start, end);
}

static void tbf_gui_output_view_select(TbfGui_OutputView* self, const TbfGui_OutputGrid* grid, Rectangle card)
{
    Rectangle area = tbf_gui_output_view_grid_rect(grid);
    TbfMath_Vector2 mouse = tbf_gui_mouse_position();
    bool over = tbf_gui_mouse_over(area);

    if (over || self->selecting) {
        tbf_gui_cursor_request(MOUSE_CURSOR_IBEAM);
    }

    if (over && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        self->selecting = true;
        self->has_selection = false;
        self->anchor = tbf_gui_output_view_cell_at(grid, mouse);
        self->head = self->anchor;
    }

    if (self->selecting && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        self->head = tbf_gui_output_view_cell_at(grid, mouse);
        self->has_selection = self->head != self->anchor;
    }

    if (self->selecting && !IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        self->selecting = false;
    }

    bool ctrl = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);

    if (ctrl && IsKeyPressed(KEY_A) && tbf_gui_mouse_over(card)) {
        self->anchor = 0;
        self->head = TBF_GUI_OUTPUT_VIEW_CELLS - 1;
        self->has_selection = true;
    }
}

static Rectangle tbf_gui_output_view_segment(const TbfGui_OutputGrid* grid, int start, int end, int row)
{
    int first = row == start / TBF_GUI_TERMINAL_COLS ? start % TBF_GUI_TERMINAL_COLS : 0;
    int last = row == end / TBF_GUI_TERMINAL_COLS ? end % TBF_GUI_TERMINAL_COLS : TBF_GUI_TERMINAL_COLS - 1;

    return (Rectangle) {
        grid->x + first * grid->char_width,
        grid->y + row * grid->line_height,
        (last - first + 1) * grid->char_width,
        grid->line_height,
    };
}

/* Quina côncava: preenche um quadrado com a cor da seleção e recorta um círculo com a cor do fundo. */
static void tbf_gui_output_view_draw_fillet(float x, float y, float dx, float dy, float radius, Color fill, Color background)
{
    Rectangle square = {
        dx < 0 ? x - radius : x,
        dy < 0 ? y - radius : y,
        radius,
        radius,
    };

    DrawRectangleRec(square, fill);
    tbf_gui_circle(tbf_math_vector2(x + dx * radius, y + dy * radius), radius, background);
}

static void tbf_gui_output_view_draw_join(Rectangle above, Rectangle below, float radius, Color fill, Color background)
{
    float above_right = above.x + above.width;
    float below_right = below.x + below.width;
    float left = fmaxf(above.x, below.x);
    float right = fminf(above_right, below_right);
    float boundary = below.y;

    if (right <= left) {
        return;
    }

    DrawRectangleRec((Rectangle) { left, above.y + above.height / 2, right - left, below.height }, fill);

    if (below.x < above.x) {
        tbf_gui_output_view_draw_fillet(above.x, boundary, -1, -1, radius, fill, background);
    } else if (above.x < below.x) {
        tbf_gui_output_view_draw_fillet(below.x, boundary, -1, 1, radius, fill, background);
    }

    if (below_right > above_right) {
        tbf_gui_output_view_draw_fillet(above_right, boundary, 1, -1, radius, fill, background);
    } else if (above_right > below_right) {
        tbf_gui_output_view_draw_fillet(below_right, boundary, 1, 1, radius, fill, background);
    }
}

static void tbf_gui_output_view_draw_selection(const TbfGui_OutputView* self, const TbfGui_Theme* theme, const TbfGui_OutputGrid* grid)
{
    if (!self->has_selection) {
        return;
    }

    const float radius = grid->line_height * TBF_GUI_OUTPUT_VIEW_RADIUS;
    const Color background = theme->surface_container;
    const Color fill = ColorLerp(background, theme->primary, TBF_GUI_OUTPUT_VIEW_SELECTION);

    int start;
    int end;

    tbf_gui_output_view_range(self, &start, &end);

    int first_row = start / TBF_GUI_TERMINAL_COLS;
    int last_row = end / TBF_GUI_TERMINAL_COLS;

    for (int row = first_row; row <= last_row; row++) {
        tbf_gui_rounded(tbf_gui_output_view_segment(grid, start, end, row), radius, fill);
    }

    for (int row = first_row; row < last_row; row++) {
        Rectangle above = tbf_gui_output_view_segment(grid, start, end, row);
        Rectangle below = tbf_gui_output_view_segment(grid, start, end, row + 1);

        tbf_gui_output_view_draw_join(above, below, radius, fill, background);
    }
}

static void tbf_gui_output_view_draw_cells(const TbfGui_Style* style, const TbfGui_Terminal* terminal, const TbfGui_OutputGrid* grid)
{
    for (int row = 0; row < TBF_GUI_TERMINAL_ROWS; row++) {
        for (int col = 0; col < TBF_GUI_TERMINAL_COLS; col++) {
            unsigned char c = terminal->cells[row][col];

            if (c == ' ') {
                continue;
            }

            tbf_gui_char(
                grid->font,
                tbf_gui_output_view_printable(c),
                grid->x + col * grid->char_width,
                grid->y + row * grid->line_height,
                style->theme->on_surface
            );
        }
    }
}

static TbfGui_Action tbf_gui_output_view_draw_header(TbfGui_OutputView* self, const TbfGui_Style* style, const TbfGui_Terminal* terminal, Rectangle rect)
{
    TbfGui_Action action = TBF_GUI_ACTION_NONE;
    Rectangle fullscreen = { rect.x + rect.width - 8 - 40, rect.y + 2, 40, 40 };
    Rectangle copy = { fullscreen.x - 44, fullscreen.y, 40, 40 };

    if (tbf_gui_icon_button(style, copy, TBF_GUI_ICON_COPY, true)) {
        tbf_gui_output_view_copy_selection(self, terminal, true);
    }

    int icon = self->fullscreen ? TBF_GUI_ICON_FULLSCREEN_EXIT : TBF_GUI_ICON_FULLSCREEN;

    if (tbf_gui_icon_button(style, fullscreen, icon, true)) {
        action = TBF_GUI_ACTION_TOGGLE_OUTPUT_FULLSCREEN;
    }

    if (GetTime() - self->copied_at < TBF_GUI_OUTPUT_VIEW_FEEDBACK) {
        const char* label = "Copiado";
        TbfMath_Vector2 size = tbf_gui_text_size(style->fonts.body, label);

        tbf_gui_text(style->fonts.body, label, copy.x - 8 - size.x, copy.y + (copy.height - size.y) / 2, style->theme->primary);
    }

    return action;
}

TbfGui_Action tbf_gui_output_view_draw(TbfGui_OutputView* self, const TbfGui_Style* style, const TbfGui_Terminal* terminal, bool waiting, Rectangle rect)
{
    const TbfGui_Theme* theme = style->theme;

    tbf_gui_card(style, rect, "Saída", self->fullscreen ? "Esc para sair da tela cheia" : NULL);

    TbfGui_Action action = tbf_gui_output_view_draw_header(self, style, terminal, rect);
    TbfGui_OutputGrid grid = tbf_gui_output_view_grid(self, style, rect);

    tbf_gui_output_view_select(self, &grid, rect);

    bool ctrl = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);

    if (ctrl && IsKeyPressed(KEY_C)) {
        tbf_gui_output_view_copy_selection(self, terminal, false);
    }

    if (self->fullscreen && IsKeyPressed(KEY_ESCAPE)) {
        action = TBF_GUI_ACTION_TOGGLE_OUTPUT_FULLSCREEN;
    }

    tbf_gui_output_view_draw_selection(self, theme, &grid);
    tbf_gui_output_view_draw_cells(style, terminal, &grid);

    if (waiting && (int) (GetTime() * 2) % 2 == 0) {
        DrawRectangle(
            grid.x + terminal->cx * grid.char_width,
            grid.y + terminal->cy * grid.line_height,
            2,
            grid.line_height,
            theme->primary
        );
    }

    return action;
}
