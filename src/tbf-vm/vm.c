#include <stdlib.h>

#include "tbf/tbf.h"

TbfVm* tbf_vm_create(const TbfProgram* program, TbfIo io)
{
    TbfVm* self = calloc(1, sizeof(TbfVm));

    if (self == NULL) {
        return NULL;
    }

    self->running = true;
    self->program = program;
    self->io = io;

    return self;
}

void tbf_vm_destroy(TbfVm* self)
{
    free(self);
}

static TbfStatus tbf_vm_halt(TbfVm* self, TbfStatus status)
{
    self->running = false;

    return status;
}

static void tbf_vm_skip(TbfVm* self, TbfByte opcode)
{
    if (opcode == '[') {
        self->skip_depth++;
    } else if (opcode == ']') {
        self->skip_depth--;
    }
}

static TbfStatus tbf_vm_execute(TbfVm* self, TbfByte opcode)
{
    switch (opcode) {
    case '+':
        self->tape[self->tp]++;
        break;

    case '-':
        self->tape[self->tp]--;
        break;

    case '>':
        self->tp++;
        break;

    case '<':
        self->tp--;
        break;

    case '[':
        if (!self->tape[self->tp]) {
            self->skip_depth++;
            break;
        }

        if (self->sp >= TBF_STACK_SIZE) {
            return tbf_vm_halt(self, TBF_STATUS_STACK_OVERFLOW);
        }

        self->stack[self->sp++] = self->ip - 1;
        break;

    case ']':
        if (self->sp == 0) {
            return tbf_vm_halt(self, TBF_STATUS_STACK_UNDERFLOW);
        }

        self->sp--;

        if (self->tape[self->tp]) {
            self->ip = self->stack[self->sp];
        }
        break;

    case '.':
        self->io.output(self->io.context, self->tape[self->tp]);
        break;

    case ',':
        self->tape[self->tp] = self->io.input(self->io.context);
        break;

    default:
        break;
    }

    return TBF_STATUS_OK;
}

TbfStatus tbf_vm_step(TbfVm* self)
{
    if (!self->running) {
        return TBF_STATUS_HALTED;
    }

    if (self->ip >= self->program->size) {
        return tbf_vm_halt(self, TBF_STATUS_HALTED);
    }

    TbfByte opcode = self->program->code[self->ip++];

    if (self->skip_depth) {
        tbf_vm_skip(self, opcode);
        return TBF_STATUS_OK;
    }

    return tbf_vm_execute(self, opcode);
}

TbfStatus tbf_vm_run(TbfVm* self)
{
    TbfStatus status;

    do {
        status = tbf_vm_step(self);
    } while (status == TBF_STATUS_OK);

    return status;
}

bool tbf_vm_awaits_input(const TbfVm* self)
{
    return self->running
        && !self->skip_depth
        && self->ip < self->program->size
        && self->program->code[self->ip] == ',';
}

const char* tbf_status_message(TbfStatus status)
{
    switch (status) {
    case TBF_STATUS_OK:
        return "ok";

    case TBF_STATUS_HALTED:
        return "programa finalizado";

    case TBF_STATUS_STACK_OVERFLOW:
        return "estouro da pilha de loops";

    case TBF_STATUS_STACK_UNDERFLOW:
        return "']' sem '[' correspondente";

    default:
        return "erro desconhecido";
    }
}
