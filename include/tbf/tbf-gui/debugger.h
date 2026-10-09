#ifndef TBF_GUI_DEBUGGER_H
#define TBF_GUI_DEBUGGER_H

#include <stdbool.h>

#include "tbf/tbf.h"
#include "tbf/tbf-gui/input.h"
#include "tbf/tbf-gui/terminal.h"

#define TBF_GUI_DEBUGGER_PATH_SIZE  1024
#define TBF_GUI_DEBUGGER_ERROR_SIZE 256

typedef enum TbfGuiDebuggerState {
    TBF_GUI_DEBUGGER_EMPTY,
    TBF_GUI_DEBUGGER_PAUSED,
    TBF_GUI_DEBUGGER_RUNNING,
    TBF_GUI_DEBUGGER_WAITING_INPUT,
    TBF_GUI_DEBUGGER_HALTED,
    TBF_GUI_DEBUGGER_FAILED,
} TbfGuiDebuggerState;

typedef struct TbfGuiDebugger {
    char path[TBF_GUI_DEBUGGER_PATH_SIZE];
    char error[TBF_GUI_DEBUGGER_ERROR_SIZE];

    TbfProgram program;
    bool loaded;
    bool* breakpoints;

    TbfVm* vm;
    TbfGuiDebuggerState state;
    bool resume_running;
    bool ignore_breakpoint;
    unsigned long long steps;

    int rate;
    double step_accum;

    TbfGuiTerminal* terminal;
    TbfGuiInput* input;
} TbfGuiDebugger;

void tbf_gui_debugger_init(TbfGuiDebugger* self, TbfGuiTerminal* terminal, TbfGuiInput* input);
void tbf_gui_debugger_free(TbfGuiDebugger* self);

bool tbf_gui_debugger_load(TbfGuiDebugger* self, const char* path);
void tbf_gui_debugger_reset(TbfGuiDebugger* self);

void tbf_gui_debugger_update(TbfGuiDebugger* self, float frame_time);
void tbf_gui_debugger_toggle_run(TbfGuiDebugger* self);
void tbf_gui_debugger_step(TbfGuiDebugger* self);
void tbf_gui_debugger_set_rate(TbfGuiDebugger* self, int steps_per_second);
void tbf_gui_debugger_toggle_breakpoint(TbfGuiDebugger* self, size_t index);

bool tbf_gui_debugger_is_running(const TbfGuiDebugger* self);
bool tbf_gui_debugger_can_run(const TbfGuiDebugger* self);

#endif
