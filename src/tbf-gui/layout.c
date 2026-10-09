#include <stdbool.h>

#include "tbf/tbf-gui/layout.h"

TbfGui_Insets tbf_gui_insets_all(float value)
{
    return (TbfGui_Insets) { value, value, value, value };
}

TbfGui_Insets tbf_gui_insets_xy(float x, float y)
{
    return (TbfGui_Insets) { y, x, y, x };
}

Rectangle tbf_gui_box(Rectangle rect, TbfGui_Insets padding)
{
    return (Rectangle) {
        rect.x + padding.left,
        rect.y + padding.top,
        rect.width - padding.left - padding.right,
        rect.height - padding.top - padding.bottom,
    };
}

static float tbf_gui_align_offset(float available, float size, TbfGui_Align align)
{
    switch (align) {
    case TBF_GUI_ALIGN_CENTER:
        return (available - size) / 2;

    case TBF_GUI_ALIGN_END:
        return available - size;

    default:
        return 0;
    }
}

Rectangle tbf_gui_box_align(Rectangle rect, float width, float height, TbfGui_Align horizontal, TbfGui_Align vertical)
{
    return (Rectangle) {
        rect.x + tbf_gui_align_offset(rect.width, width, horizontal),
        rect.y + tbf_gui_align_offset(rect.height, height, vertical),
        width,
        height,
    };
}

static bool tbf_gui_stack_is_horizontal(TbfGui_StackDirection direction)
{
    return direction == TBF_GUI_STACK_HORIZONTAL || direction == TBF_GUI_STACK_HORIZONTAL_REVERSE;
}

static bool tbf_gui_stack_is_reverse(TbfGui_StackDirection direction)
{
    return direction == TBF_GUI_STACK_HORIZONTAL_REVERSE || direction == TBF_GUI_STACK_VERTICAL_REVERSE;
}

static float tbf_gui_stack_length(const TbfGui_Stack* self)
{
    return tbf_gui_stack_is_horizontal(self->direction) ? self->rect.width : self->rect.height;
}

/* Recorta um trecho [offset, offset + size) do eixo principal, respeitando a direção. */
static Rectangle tbf_gui_stack_slice(const TbfGui_Stack* self, float offset, float size)
{
    Rectangle rect = self->rect;
    float length = tbf_gui_stack_length(self);
    float start = tbf_gui_stack_is_reverse(self->direction) ? length - offset - size : offset;

    if (tbf_gui_stack_is_horizontal(self->direction)) {
        return (Rectangle) { rect.x + start, rect.y, size, rect.height };
    }

    return (Rectangle) { rect.x, rect.y + start, rect.width, size };
}

TbfGui_Stack tbf_gui_stack_begin(Rectangle rect, TbfGui_StackDirection direction, float spacing)
{
    return (TbfGui_Stack) { rect, direction, spacing, 0 };
}

Rectangle tbf_gui_stack_next(TbfGui_Stack* self, float size)
{
    Rectangle item = tbf_gui_stack_slice(self, self->used, size);

    self->used += size + self->spacing;

    return item;
}

void tbf_gui_stack_skip(TbfGui_Stack* self, float size)
{
    self->used += size;
}

Rectangle tbf_gui_stack_rest(const TbfGui_Stack* self)
{
    float size = tbf_gui_stack_length(self) - self->used;

    return tbf_gui_stack_slice(self, self->used, size > 0 ? size : 0);
}

void tbf_gui_stack(Rectangle rect, TbfGui_StackDirection direction, float spacing, const TbfGui_StackItem* items, int count, Rectangle* out)
{
    TbfGui_Stack stack = tbf_gui_stack_begin(rect, direction, spacing);
    float fixed = count > 1 ? (count - 1) * spacing : 0;
    float grow = 0;

    for (int i = 0; i < count; i++) {
        fixed += items[i].flex > 0 ? 0 : items[i].size;
        grow += items[i].flex;
    }

    float free_space = tbf_gui_stack_length(&stack) - fixed;

    for (int i = 0; i < count; i++) {
        float size = items[i].flex > 0 && grow > 0 ? free_space * items[i].flex / grow : items[i].size;

        out[i] = tbf_gui_stack_next(&stack, size > 0 ? size : 0);
    }
}

TbfGui_Grid tbf_gui_grid_begin(Rectangle rect, int columns, float spacing, float row_height)
{
    return (TbfGui_Grid) { rect, columns > 0 ? columns : TBF_GUI_GRID_COLUMNS, spacing, row_height, 0, 0 };
}

Rectangle tbf_gui_grid_item(TbfGui_Grid* self, int span)
{
    if (span < 1) {
        span = 1;
    }

    if (span > self->columns) {
        span = self->columns;
    }

    if (self->column + span > self->columns) {
        self->column = 0;
        self->row++;
    }

    float unit = (self->rect.width - (self->columns - 1) * self->spacing) / self->columns;

    Rectangle item = {
        self->rect.x + self->column * (unit + self->spacing),
        self->rect.y + self->row * (self->row_height + self->spacing),
        span * unit + (span - 1) * self->spacing,
        self->row_height,
    };

    self->column += span;

    return item;
}
