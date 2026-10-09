#ifndef TBF_GUI_SELECTION_H
#define TBF_GUI_SELECTION_H

#include <stdbool.h>

#include "raylib.h"

typedef struct TbfGui_Selection {
    bool selecting;
    bool active;
    int anchor;
    int head;
} TbfGui_Selection;

typedef struct TbfGui_SelectionGrid {
    float x;
    float y;
    float char_width;
    float line_height;
    int cols;
    int first_row;
    int last_row;
} TbfGui_SelectionGrid;

void tbf_gui_selection_clear(TbfGui_Selection* self);
void tbf_gui_selection_begin(TbfGui_Selection* self, int index);
void tbf_gui_selection_extend(TbfGui_Selection* self, int index);
void tbf_gui_selection_end(TbfGui_Selection* self);
void tbf_gui_selection_all(TbfGui_Selection* self, int count);
void tbf_gui_selection_range(const TbfGui_Selection* self, int* start, int* end);
void tbf_gui_selection_draw(const TbfGui_Selection* self, const TbfGui_SelectionGrid* grid, Color fill, Color background);

#endif
