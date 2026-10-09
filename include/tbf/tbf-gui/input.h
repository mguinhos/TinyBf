#ifndef TBF_GUI_INPUT_H
#define TBF_GUI_INPUT_H

#include <stdbool.h>

#include "tbf/tbf.h"

#define TBF_GUI_INPUT_SIZE 4096

typedef struct TbfGui_Input {
    TbfByte data[TBF_GUI_INPUT_SIZE];
    int head;
    int length;
} TbfGui_Input;

void tbf_gui_input_clear(TbfGui_Input* self);
void tbf_gui_input_push(TbfGui_Input* self, TbfByte value);
TbfByte tbf_gui_input_pop(TbfGui_Input* self);
void tbf_gui_input_pop_last(TbfGui_Input* self);
TbfByte tbf_gui_input_peek(const TbfGui_Input* self, int index);
bool tbf_gui_input_is_empty(const TbfGui_Input* self);

#endif
