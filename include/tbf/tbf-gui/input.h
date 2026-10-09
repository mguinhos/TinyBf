#ifndef TBF_GUI_INPUT_H
#define TBF_GUI_INPUT_H

#include <stdbool.h>

#include "tbf/tbf.h"

#define TBF_GUI_INPUT_SIZE 4096

typedef struct TbfGuiInput {
    TbfByte data[TBF_GUI_INPUT_SIZE];
    int head;
    int length;
} TbfGuiInput;

void tbf_gui_input_clear(TbfGuiInput* self);
void tbf_gui_input_push(TbfGuiInput* self, TbfByte value);
TbfByte tbf_gui_input_pop(TbfGuiInput* self);
void tbf_gui_input_pop_last(TbfGuiInput* self);
TbfByte tbf_gui_input_peek(const TbfGuiInput* self, int index);
bool tbf_gui_input_is_empty(const TbfGuiInput* self);

#endif
