#ifndef TBF_GUI_VIEWS_H
#define TBF_GUI_VIEWS_H

#include <stdbool.h>
#include <stddef.h>

#include "raylib.h"

#include "tbf/tbf.h"
#include "tbf/tbf-gui/debugger.h"
#include "tbf/tbf-gui/input.h"
#include "tbf/tbf-gui/terminal.h"
#include "tbf/tbf-gui/widgets.h"

typedef enum TbfGui_Action {
    TBF_GUI_ACTION_NONE,
    TBF_GUI_ACTION_TOGGLE_RUN,
    TBF_GUI_ACTION_STEP,
    TBF_GUI_ACTION_SKIP,
    TBF_GUI_ACTION_RESET,
    TBF_GUI_ACTION_SLOWER,
    TBF_GUI_ACTION_FASTER,
    TBF_GUI_ACTION_TOGGLE_THEME,
} TbfGui_Action;

typedef struct TbfGui_Toolbar {
    int speed;
} TbfGui_Toolbar;

typedef struct TbfGui_CodeView {
    size_t* index;
    size_t size;
    int scroll;
    bool follow;
} TbfGui_CodeView;

TbfGui_Action tbf_gui_app_bar_draw(const TbfGui_Style* style, const TbfGui_Debugger* debugger, bool dark, Rectangle rect);

void tbf_gui_toolbar_init(TbfGui_Toolbar* self);
void tbf_gui_toolbar_slower(TbfGui_Toolbar* self);
void tbf_gui_toolbar_faster(TbfGui_Toolbar* self);
int tbf_gui_toolbar_rate(const TbfGui_Toolbar* self);
TbfGui_Action tbf_gui_toolbar_draw(const TbfGui_Toolbar* self, const TbfGui_Style* style, const TbfGui_Debugger* debugger, Rectangle rect);

void tbf_gui_code_view_init(TbfGui_CodeView* self);
void tbf_gui_code_view_load(TbfGui_CodeView* self, const TbfProgram* program);
void tbf_gui_code_view_free(TbfGui_CodeView* self);
void tbf_gui_code_view_follow(TbfGui_CodeView* self);
void tbf_gui_code_view_draw(TbfGui_CodeView* self, const TbfGui_Style* style, TbfGui_Debugger* debugger, Rectangle rect);

void tbf_gui_output_view_draw(const TbfGui_Style* style, const TbfGui_Terminal* terminal, bool waiting, Rectangle rect);
void tbf_gui_tape_view_draw(const TbfGui_Style* style, const TbfVm* vm, Rectangle rect);
void tbf_gui_input_view_draw(const TbfGui_Style* style, const TbfGui_Input* input, bool waiting, Rectangle rect);

#endif
