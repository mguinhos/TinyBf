#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "raylib.h"

#include "tbf/tbf-gui/debugger.h"

#define TBF_GUI_DEBUGGER_FRAME_BUDGET 0.012

static void tbf_gui_debugger_output(void* context, TbfCell value)
{
    TbfGui_Debugger* self = context;

    tbf_gui_terminal_put(self->terminal, value);
}

static TbfCell tbf_gui_debugger_input(void* context)
{
    TbfGui_Debugger* self = context;

    return tbf_gui_input_pop(self->input);
}

static void tbf_gui_debugger_unload(TbfGui_Debugger* self)
{
    tbf_vm_destroy(self->vm);
    tbf_program_free(&self->program);
    free(self->breakpoints);
    free(self->match);

    self->vm = NULL;
    self->breakpoints = NULL;
    self->match = NULL;
    self->loaded = false;
    self->error[0] = '\0';
}

static bool tbf_gui_debugger_pair(TbfGui_Debugger* self)
{
    const TbfProgram* program = &self->program;
    size_t* stack = malloc(sizeof(size_t) * (program->size + 1));
    size_t depth = 0;

    self->match = malloc(sizeof(size_t) * (program->size + 1));

    if (stack == NULL || self->match == NULL) {
        free(stack);
        return false;
    }

    for (size_t i = 0; i < program->size; i++) {
        self->match[i] = i;

        if (program->code[i] == '[') {
            stack[depth++] = i;
        } else if (program->code[i] == ']' && depth > 0) {
            size_t open = stack[--depth];

            self->match[open] = i;
            self->match[i] = open;
        }
    }

    free(stack);

    return true;
}

void tbf_gui_debugger_init(TbfGui_Debugger* self, TbfGui_Terminal* terminal, TbfGui_Input* input)
{
    memset(self, 0, sizeof(*self));

    self->terminal = terminal;
    self->input = input;
}

void tbf_gui_debugger_free(TbfGui_Debugger* self)
{
    tbf_gui_debugger_unload(self);
}

bool tbf_gui_debugger_load(TbfGui_Debugger* self, const char* path)
{
    tbf_gui_debugger_unload(self);
    snprintf(self->path, sizeof(self->path), "%s", path);

    TbfProgramStatus status = tbf_program_load(&self->program, path);
    const char* message = tbf_program_status_message(status);

    if (status == TBF_PROGRAM_UNREADABLE) {
        snprintf(self->error, sizeof(self->error), "%s", message);
    } else if (status != TBF_PROGRAM_OK) {
        snprintf(self->error, sizeof(self->error), "%s (posição %zu)", message, self->program.error_position);
    } else {
        self->breakpoints = calloc(self->program.size + 1, sizeof(bool));
        self->loaded = self->breakpoints != NULL && tbf_gui_debugger_pair(self);
    }

    tbf_gui_debugger_reset(self);

    return self->loaded;
}

void tbf_gui_debugger_reset(TbfGui_Debugger* self)
{
    tbf_vm_destroy(self->vm);
    self->vm = NULL;

    tbf_gui_terminal_clear(self->terminal);
    tbf_gui_input_clear(self->input);

    self->steps = 0;
    self->step_accum = 0;
    self->resume_running = false;
    self->ignore_breakpoint = false;
    self->skipping = false;
    self->state = TBF_GUI_DEBUGGER_EMPTY;

    if (!self->loaded) {
        return;
    }

    TbfIo io = {
        .output = tbf_gui_debugger_output,
        .input = tbf_gui_debugger_input,
        .context = self,
    };

    self->error[0] = '\0';
    self->vm = tbf_vm_create(&self->program, io);

    if (self->vm != NULL) {
        self->state = TBF_GUI_DEBUGGER_PAUSED;
    }
}

static bool tbf_gui_debugger_advance(TbfGui_Debugger* self, bool ignore_breakpoint)
{
    TbfVm* vm = self->vm;

    if (!ignore_breakpoint && vm->ip < self->program.size && self->breakpoints[vm->ip]) {
        self->state = TBF_GUI_DEBUGGER_PAUSED;
        return false;
    }

    if (tbf_vm_awaits_input(vm) && tbf_gui_input_is_empty(self->input)) {
        self->resume_running = self->state == TBF_GUI_DEBUGGER_RUNNING;
        self->state = TBF_GUI_DEBUGGER_WAITING_INPUT;
        return false;
    }

    TbfStatus status = tbf_vm_step(vm);

    if (status == TBF_STATUS_HALTED) {
        self->state = TBF_GUI_DEBUGGER_HALTED;
        return false;
    }

    if (status != TBF_STATUS_OK) {
        snprintf(self->error, sizeof(self->error), "%s", tbf_status_message(status));
        self->state = TBF_GUI_DEBUGGER_FAILED;
        return false;
    }

    self->steps++;

    return true;
}

static long long tbf_gui_debugger_budget(TbfGui_Debugger* self, float frame_time)
{
    if (self->rate <= 0) {
        return LLONG_MAX;
    }

    self->step_accum += self->rate * frame_time;

    if (self->step_accum > self->rate) {
        self->step_accum = self->rate;
    }

    long long count = (long long) self->step_accum;

    self->step_accum -= count;

    return count;
}

static void tbf_gui_debugger_run_frame(TbfGui_Debugger* self, float frame_time)
{
    long long count = self->skipping ? LLONG_MAX : tbf_gui_debugger_budget(self, frame_time);
    double deadline = GetTime() + TBF_GUI_DEBUGGER_FRAME_BUDGET;

    for (long long i = 0; i < count; i++) {
        bool ignore = self->ignore_breakpoint;

        self->ignore_breakpoint = false;

        if (!tbf_gui_debugger_advance(self, ignore)) {
            break;
        }

        if (self->skipping && self->vm->ip > self->skip_end) {
            self->state = TBF_GUI_DEBUGGER_PAUSED;
            break;
        }

        if ((i & 0xfff) == 0xfff && GetTime() > deadline) {
            break;
        }
    }
}

void tbf_gui_debugger_update(TbfGui_Debugger* self, float frame_time)
{
    if (self->state == TBF_GUI_DEBUGGER_WAITING_INPUT && !tbf_gui_input_is_empty(self->input)) {
        if (self->resume_running) {
            self->state = TBF_GUI_DEBUGGER_RUNNING;
        } else {
            self->state = TBF_GUI_DEBUGGER_PAUSED;
            tbf_gui_debugger_advance(self, true);
        }
    }

    if (self->state == TBF_GUI_DEBUGGER_RUNNING) {
        tbf_gui_debugger_run_frame(self, frame_time);
    }

    if (self->state != TBF_GUI_DEBUGGER_RUNNING && self->state != TBF_GUI_DEBUGGER_WAITING_INPUT) {
        self->skipping = false;
    }
}

void tbf_gui_debugger_toggle_run(TbfGui_Debugger* self)
{
    switch (self->state) {
    case TBF_GUI_DEBUGGER_PAUSED:
        self->state = TBF_GUI_DEBUGGER_RUNNING;
        self->ignore_breakpoint = true;
        self->step_accum = 0;
        break;

    case TBF_GUI_DEBUGGER_RUNNING:
        self->state = TBF_GUI_DEBUGGER_PAUSED;
        break;

    case TBF_GUI_DEBUGGER_WAITING_INPUT:
        self->resume_running = !self->resume_running;
        break;

    default:
        break;
    }
}

void tbf_gui_debugger_step(TbfGui_Debugger* self)
{
    if (self->state == TBF_GUI_DEBUGGER_PAUSED) {
        tbf_gui_debugger_advance(self, true);
    }
}

static TbfDaddr tbf_gui_debugger_section_end(const TbfGui_Debugger* self)
{
    const TbfVm* vm = self->vm;
    const TbfProgram* program = &self->program;
    TbfDaddr ip = vm->ip;

    if (vm->sp > 0) {
        return self->match[vm->stack[vm->sp - 1]];
    }

    if (ip < program->size && program->code[ip] == '[') {
        return self->match[ip];
    }

    while (ip < program->size && program->code[ip] != '[') {
        ip++;
    }

    return ip > vm->ip ? ip - 1 : ip;
}

void tbf_gui_debugger_skip(TbfGui_Debugger* self)
{
    if (self->state != TBF_GUI_DEBUGGER_PAUSED) {
        return;
    }

    self->skip_end = tbf_gui_debugger_section_end(self);
    self->skipping = true;
    self->ignore_breakpoint = true;
    self->state = TBF_GUI_DEBUGGER_RUNNING;
}

void tbf_gui_debugger_set_rate(TbfGui_Debugger* self, int steps_per_second)
{
    self->rate = steps_per_second;
    self->step_accum = 0;
}

void tbf_gui_debugger_toggle_breakpoint(TbfGui_Debugger* self, size_t index)
{
    if (self->loaded && index < self->program.size) {
        self->breakpoints[index] = !self->breakpoints[index];
    }
}

bool tbf_gui_debugger_is_running(const TbfGui_Debugger* self)
{
    return self->state == TBF_GUI_DEBUGGER_RUNNING
        || (self->state == TBF_GUI_DEBUGGER_WAITING_INPUT && self->resume_running);
}

bool tbf_gui_debugger_can_run(const TbfGui_Debugger* self)
{
    return self->state == TBF_GUI_DEBUGGER_PAUSED
        || self->state == TBF_GUI_DEBUGGER_RUNNING
        || self->state == TBF_GUI_DEBUGGER_WAITING_INPUT;
}
