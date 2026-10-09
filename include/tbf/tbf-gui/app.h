#ifndef TBF_GUI_APP_H
#define TBF_GUI_APP_H

#include "tbf/tbf-gui/debugger.h"
#include "tbf/tbf-gui/input.h"
#include "tbf/tbf-gui/terminal.h"
#include "tbf/tbf-gui/views.h"
#include "tbf/tbf-gui/widgets.h"

#define TBF_GUI_WINDOW_WIDTH    1280
#define TBF_GUI_WINDOW_HEIGHT   912

typedef struct TbfGui_App {
    TbfGui_Style style;
    TbfGui_Ripples ripples;
    bool dark;
    TbfGui_Terminal terminal;
    TbfGui_Input input;
    TbfGui_Debugger debugger;
    TbfGui_Toolbar toolbar;
    TbfGui_CodeView code_view;
} TbfGui_App;

void tbf_gui_app_init(TbfGui_App* self);
void tbf_gui_app_free(TbfGui_App* self);
void tbf_gui_app_load(TbfGui_App* self, const char* path);
void tbf_gui_app_dispatch(TbfGui_App* self, TbfGui_Action action);
void tbf_gui_app_poll(TbfGui_App* self);
void tbf_gui_app_update(TbfGui_App* self);
void tbf_gui_app_draw(TbfGui_App* self);

#endif
