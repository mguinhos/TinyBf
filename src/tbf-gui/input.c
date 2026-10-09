#include "tbf/tbf-gui/input.h"

void tbf_gui_input_clear(TbfGuiInput* self)
{
    self->head = 0;
    self->length = 0;
}

void tbf_gui_input_push(TbfGuiInput* self, TbfByte value)
{
    if (self->length >= TBF_GUI_INPUT_SIZE) {
        return;
    }

    self->data[(self->head + self->length) % TBF_GUI_INPUT_SIZE] = value;
    self->length++;
}

TbfByte tbf_gui_input_pop(TbfGuiInput* self)
{
    if (self->length == 0) {
        return 0;
    }

    TbfByte value = self->data[self->head];

    self->head = (self->head + 1) % TBF_GUI_INPUT_SIZE;
    self->length--;

    return value;
}

void tbf_gui_input_pop_last(TbfGuiInput* self)
{
    if (self->length > 0) {
        self->length--;
    }
}

TbfByte tbf_gui_input_peek(const TbfGuiInput* self, int index)
{
    return self->data[(self->head + index) % TBF_GUI_INPUT_SIZE];
}

bool tbf_gui_input_is_empty(const TbfGuiInput* self)
{
    return self->length == 0;
}
