#include <math.h>

#include "tbf/tbf-gui/selection.h"
#include "tbf/tbf-gui/widgets.h"

#define TBF_GUI_SELECTION_RADIUS 0.25f

void tbf_gui_selection_clear(TbfGui_Selection* self)
{
    self->selecting = false;
    self->active = false;
    self->anchor = 0;
    self->head = 0;
}

void tbf_gui_selection_begin(TbfGui_Selection* self, int index)
{
    self->selecting = true;
    self->active = false;
    self->anchor = index;
    self->head = index;
}

void tbf_gui_selection_extend(TbfGui_Selection* self, int index)
{
    self->head = index;
    self->active = self->head != self->anchor;
}

void tbf_gui_selection_end(TbfGui_Selection* self)
{
    self->selecting = false;
}

void tbf_gui_selection_all(TbfGui_Selection* self, int count)
{
    self->selecting = false;
    self->active = count > 0;
    self->anchor = 0;
    self->head = count > 0 ? count - 1 : 0;
}

void tbf_gui_selection_range(const TbfGui_Selection* self, int* start, int* end)
{
    *start = self->anchor < self->head ? self->anchor : self->head;
    *end = self->anchor < self->head ? self->head : self->anchor;
}

static Rectangle tbf_gui_selection_segment(const TbfGui_SelectionGrid* grid, int start, int end, int row)
{
    int first = row == start / grid->cols ? start % grid->cols : 0;
    int last = row == end / grid->cols ? end % grid->cols : grid->cols - 1;

    return (Rectangle) {
        grid->x + first * grid->char_width,
        grid->y + row * grid->line_height,
        (last - first + 1) * grid->char_width,
        grid->line_height,
    };
}

/* Quina côncava: preenche um quadrado com a cor da seleção e recorta um círculo com a cor do fundo. */
static void tbf_gui_selection_draw_fillet(float x, float y, float dx, float dy, float radius, Color fill, Color background)
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

static void tbf_gui_selection_draw_join(Rectangle above, Rectangle below, float radius, Color fill, Color background)
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
        tbf_gui_selection_draw_fillet(above.x, boundary, -1, -1, radius, fill, background);
    } else if (above.x < below.x) {
        tbf_gui_selection_draw_fillet(below.x, boundary, -1, 1, radius, fill, background);
    }

    if (below_right > above_right) {
        tbf_gui_selection_draw_fillet(above_right, boundary, 1, -1, radius, fill, background);
    } else if (above_right > below_right) {
        tbf_gui_selection_draw_fillet(below_right, boundary, 1, 1, radius, fill, background);
    }
}

void tbf_gui_selection_draw(const TbfGui_Selection* self, const TbfGui_SelectionGrid* grid, Color fill, Color background)
{
    if (!self->active || grid->cols <= 0) {
        return;
    }

    const float radius = grid->line_height * TBF_GUI_SELECTION_RADIUS;

    int start;
    int end;

    tbf_gui_selection_range(self, &start, &end);

    int first_row = (int) fmaxf(start / grid->cols, grid->first_row);
    int last_row = (int) fminf(end / grid->cols, grid->last_row);

    for (int row = first_row; row <= last_row; row++) {
        tbf_gui_rounded(tbf_gui_selection_segment(grid, start, end, row), radius, fill);
    }

    for (int row = first_row; row < last_row; row++) {
        Rectangle above = tbf_gui_selection_segment(grid, start, end, row);
        Rectangle below = tbf_gui_selection_segment(grid, start, end, row + 1);

        tbf_gui_selection_draw_join(above, below, radius, fill, background);
    }
}
