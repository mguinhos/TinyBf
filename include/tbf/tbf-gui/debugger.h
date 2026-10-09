#ifndef TBF_GUI_DEBUGGER_H
#define TBF_GUI_DEBUGGER_H

#include <stdbool.h>

#include "tbf/tbf.h"
#include "tbf/tbf-gui/input.h"
#include "tbf/tbf-gui/terminal.h"

#define TBF_GUI_DEBUGGER_PATH_SIZE  1024
#define TBF_GUI_DEBUGGER_ERROR_SIZE 256

typedef enum TbfGui_DebuggerState {
    TBF_GUI_DEBUGGER_EMPTY,
    TBF_GUI_DEBUGGER_PAUSED,
    TBF_GUI_DEBUGGER_RUNNING,
    TBF_GUI_DEBUGGER_WAITING_INPUT,
    TBF_GUI_DEBUGGER_HALTED,
    TBF_GUI_DEBUGGER_FAILED,
} TbfGui_DebuggerState;

typedef struct TbfGui_Debugger {
    char path[TBF_GUI_DEBUGGER_PATH_SIZE];
    char error[TBF_GUI_DEBUGGER_ERROR_SIZE];

    TbfProgram program;
    bool loaded;
    bool* breakpoints;
    size_t* match;

    TbfVm* vm;
    TbfGui_DebuggerState state;
    bool resume_running;
    bool ignore_breakpoint;
    bool skipping;
    TbfDaddr skip_end;
    unsigned long long steps;

    int rate;
    double step_accum;

    TbfGui_Terminal* terminal;
    TbfGui_Input* input;
} TbfGui_Debugger;

void tbf_gui_debugger_init(TbfGui_Debugger* self, TbfGui_Terminal* terminal, TbfGui_Input* input);
void tbf_gui_debugger_free(TbfGui_Debugger* self);

bool tbf_gui_debugger_load(TbfGui_Debugger* self, const char* path);
void tbf_gui_debugger_reset(TbfGui_Debugger* self);

void tbf_gui_debugger_update(TbfGui_Debugger* self, float frame_time);
void tbf_gui_debugger_toggle_run(TbfGui_Debugger* self);
void tbf_gui_debugger_step(TbfGui_Debugger* self);
void tbf_gui_debugger_skip(TbfGui_Debugger* self);
void tbf_gui_debugger_set_rate(TbfGui_Debugger* self, int steps_per_second);
void tbf_gui_debugger_toggle_breakpoint(TbfGui_Debugger* self, size_t index);

bool tbf_gui_debugger_is_running(const TbfGui_Debugger* self);
bool tbf_gui_debugger_can_run(const TbfGui_Debugger* self);

#endif
