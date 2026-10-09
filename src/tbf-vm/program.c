#include <stdio.h>
#include <stdlib.h>

#include "tbf/tbf.h"

bool tbf_opcode_is_valid(TbfByte opcode)
{
    switch (opcode) {
    case '+':
    case '-':
    case '<':
    case '>':
    case '[':
    case ']':
    case '.':
    case ',':
        return true;

    default:
        return false;
    }
}

TbfProgramStatus tbf_program_load(TbfProgram* self, const char* path)
{
    self->code = NULL;
    self->size = 0;
    self->error_position = 0;

    FILE* fp = fopen(path, "rb");

    if (fp == NULL) {
        return TBF_PROGRAM_UNREADABLE;
    }

    fseek(fp, 0L, SEEK_END);
    long size = ftell(fp);
    rewind(fp);

    if (size < 0) {
        fclose(fp);
        return TBF_PROGRAM_UNREADABLE;
    }

    self->code = malloc(size > 0 ? (size_t) size : 1);

    if (self->code == NULL) {
        fclose(fp);
        return TBF_PROGRAM_UNREADABLE;
    }

    self->size = fread(self->code, sizeof(TbfByte), (size_t) size, fp);
    fclose(fp);

    return tbf_program_validate(self);
}

TbfProgramStatus tbf_program_validate(TbfProgram* self)
{
    size_t depth = 0;

    for (size_t i = 0; i < self->size; i++) {
        if (self->code[i] == '[') {
            if (depth == 0) {
                self->error_position = i;
            }

            if (++depth >= TBF_STACK_SIZE) {
                self->error_position = i;
                return TBF_PROGRAM_TOO_DEEP;
            }
        } else if (self->code[i] == ']') {
            if (depth == 0) {
                self->error_position = i;
                return TBF_PROGRAM_UNMATCHED_CLOSE;
            }

            depth--;
        }
    }

    if (depth != 0) {
        return TBF_PROGRAM_UNMATCHED_OPEN;
    }

    self->error_position = 0;

    return TBF_PROGRAM_OK;
}

void tbf_program_free(TbfProgram* self)
{
    free(self->code);

    self->code = NULL;
    self->size = 0;
    self->error_position = 0;
}

const char* tbf_program_status_message(TbfProgramStatus status)
{
    switch (status) {
    case TBF_PROGRAM_OK:
        return "programa válido";

    case TBF_PROGRAM_UNREADABLE:
        return "não foi possível ler o arquivo";

    case TBF_PROGRAM_UNMATCHED_OPEN:
        return "'[' sem ']' correspondente";

    case TBF_PROGRAM_UNMATCHED_CLOSE:
        return "']' sem '[' correspondente";

    case TBF_PROGRAM_TOO_DEEP:
        return "aninhamento de '[' excede a pilha";

    default:
        return "erro desconhecido";
    }
}
