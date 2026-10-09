#ifndef TBF_GUI_LAYOUT_H
#define TBF_GUI_LAYOUT_H

#include "raylib.h"

/* Box: espaçamento interno e alinhamento de um conteúdo dentro de uma área. */

typedef struct TbfGui_Insets {
    float top;
    float right;
    float bottom;
    float left;
} TbfGui_Insets;

typedef enum TbfGui_Align {
    TBF_GUI_ALIGN_START,
    TBF_GUI_ALIGN_CENTER,
    TBF_GUI_ALIGN_END,
} TbfGui_Align;

TbfGui_Insets tbf_gui_insets_all(float value);
TbfGui_Insets tbf_gui_insets_xy(float x, float y);

Rectangle tbf_gui_box(Rectangle rect, TbfGui_Insets padding);
Rectangle tbf_gui_box_align(Rectangle rect, float width, float height, TbfGui_Align horizontal, TbfGui_Align vertical);

/* Stack: empilha itens em uma direção, com espaçamento entre eles. */

typedef enum TbfGui_StackDirection {
    TBF_GUI_STACK_HORIZONTAL,
    TBF_GUI_STACK_VERTICAL,
    TBF_GUI_STACK_HORIZONTAL_REVERSE,
    TBF_GUI_STACK_VERTICAL_REVERSE,
} TbfGui_StackDirection;

typedef struct TbfGui_Stack {
    Rectangle rect;
    TbfGui_StackDirection direction;
    float spacing;
    float used;
} TbfGui_Stack;

typedef struct TbfGui_StackItem {
    float size;
    float flex;
} TbfGui_StackItem;

#define TBF_GUI_FIXED(px)   ((TbfGui_StackItem) { (px), 0 })
#define TBF_GUI_FLEX(grow)  ((TbfGui_StackItem) { 0, (grow) })

TbfGui_Stack tbf_gui_stack_begin(Rectangle rect, TbfGui_StackDirection direction, float spacing);
Rectangle tbf_gui_stack_next(TbfGui_Stack* self, float size);
void tbf_gui_stack_skip(TbfGui_Stack* self, float size);
Rectangle tbf_gui_stack_rest(const TbfGui_Stack* self);

void tbf_gui_stack(Rectangle rect, TbfGui_StackDirection direction, float spacing, const TbfGui_StackItem* items, int count, Rectangle* out);

/* Grid: contêiner de colunas no estilo do MUI Grid, com itens que ocupam um span de colunas e quebram linha. */

#define TBF_GUI_GRID_COLUMNS 12

typedef struct TbfGui_Grid {
    Rectangle rect;
    int columns;
    float spacing;
    float row_height;
    int column;
    int row;
} TbfGui_Grid;

TbfGui_Grid tbf_gui_grid_begin(Rectangle rect, int columns, float spacing, float row_height);
Rectangle tbf_gui_grid_item(TbfGui_Grid* self, int span);

#endif
