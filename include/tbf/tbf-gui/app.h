#ifndef TBF_GUI_APP_H
#define TBF_GUI_APP_H

#include "tbf/tbf-gui/debugger.h"
#include "tbf/tbf-gui/input.h"
#include "tbf/tbf-gui/terminal.h"
#include "tbf/tbf-gui/views.h"
#include "tbf/tbf-gui/widgets.h"

#define TBF_GUI_WINDOW_WIDTH    1280
#define TBF_GUI_WINDOW_HEIGHT   810

typedef struct TbfGuiApp {
    TbfGuiFonts fonts;
    TbfGuiTerminal terminal;
    TbfGuiInput input;
    TbfGuiDebugger debugger;
    TbfGuiToolbar toolbar;
    TbfGuiCodeView code_view;
} TbfGuiApp;

void tbf_gui_app_init(TbfGuiApp* self);
void tbf_gui_app_free(TbfGuiApp* self);
void tbf_gui_app_load(TbfGuiApp* self, const char* path);
void tbf_gui_app_dispatch(TbfGuiApp* self, TbfGuiAction action);
void tbf_gui_app_poll(TbfGuiApp* self);
void tbf_gui_app_update(TbfGuiApp* self);
void tbf_gui_app_draw(TbfGuiApp* self);

#endif
