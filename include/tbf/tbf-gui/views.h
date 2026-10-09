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

typedef enum TbfGuiAction {
    TBF_GUI_ACTION_NONE,
    TBF_GUI_ACTION_TOGGLE_RUN,
    TBF_GUI_ACTION_STEP,
    TBF_GUI_ACTION_RESET,
    TBF_GUI_ACTION_SLOWER,
    TBF_GUI_ACTION_FASTER,
} TbfGuiAction;

typedef struct TbfGuiToolbar {
    int speed;
} TbfGuiToolbar;

typedef struct TbfGuiCodeView {
    size_t* index;
    size_t size;
    int scroll;
    bool follow;
} TbfGuiCodeView;

void tbf_gui_toolbar_init(TbfGuiToolbar* self);
void tbf_gui_toolbar_slower(TbfGuiToolbar* self);
void tbf_gui_toolbar_faster(TbfGuiToolbar* self);
int tbf_gui_toolbar_rate(const TbfGuiToolbar* self);
TbfGuiAction tbf_gui_toolbar_draw(const TbfGuiToolbar* self, const TbfGuiFonts* fonts, const TbfGuiDebugger* debugger, Rectangle rect);

void tbf_gui_code_view_init(TbfGuiCodeView* self);
void tbf_gui_code_view_load(TbfGuiCodeView* self, const TbfProgram* program);
void tbf_gui_code_view_free(TbfGuiCodeView* self);
void tbf_gui_code_view_follow(TbfGuiCodeView* self);
void tbf_gui_code_view_draw(TbfGuiCodeView* self, const TbfGuiFonts* fonts, TbfGuiDebugger* debugger, Rectangle rect);

void tbf_gui_output_view_draw(const TbfGuiFonts* fonts, const TbfGuiTerminal* terminal, bool waiting, Rectangle rect);
void tbf_gui_tape_view_draw(const TbfGuiFonts* fonts, const TbfVm* vm, Rectangle rect);
void tbf_gui_input_view_draw(const TbfGuiFonts* fonts, const TbfGuiInput* input, bool waiting, Rectangle rect);

#endif
