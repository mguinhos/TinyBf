#ifndef TBF_H
#define TBF_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define TBF_TAPE_SIZE   0x10000
#define TBF_STACK_SIZE  0x1000

typedef uint8_t TbfByte;
typedef uint16_t TbfWord;
typedef uint32_t TbfDword;

typedef TbfByte TbfCell;
typedef TbfWord TbfAddr;
typedef TbfDword TbfDaddr;

typedef enum TbfStatus {
    TBF_STATUS_OK,
    TBF_STATUS_HALTED,
    TBF_STATUS_STACK_OVERFLOW,
    TBF_STATUS_STACK_UNDERFLOW,
} TbfStatus;

typedef enum TbfProgramStatus {
    TBF_PROGRAM_OK,
    TBF_PROGRAM_UNREADABLE,
    TBF_PROGRAM_UNMATCHED_OPEN,
    TBF_PROGRAM_UNMATCHED_CLOSE,
    TBF_PROGRAM_TOO_DEEP,
} TbfProgramStatus;

typedef struct TbfIo {
    void (*output)(void* context, TbfCell value);
    TbfCell (*input)(void* context);
    void* context;
} TbfIo;

typedef struct TbfProgram {
    TbfByte* code;
    size_t size;
    size_t error_position;
} TbfProgram;

typedef struct TbfVm {
    bool running;

    TbfDaddr skip_depth;

    TbfAddr tp;
    TbfAddr sp;
    TbfDaddr ip;

    TbfCell tape[TBF_TAPE_SIZE];
    TbfDaddr stack[TBF_STACK_SIZE];

    const TbfProgram* program;
    TbfIo io;
} TbfVm;

TbfIo tbf_io_stdio(void);

bool tbf_opcode_is_valid(TbfByte opcode);

TbfProgramStatus tbf_program_load(TbfProgram* self, const char* path);
TbfProgramStatus tbf_program_validate(TbfProgram* self);
void tbf_program_free(TbfProgram* self);
const char* tbf_program_status_message(TbfProgramStatus status);

TbfVm* tbf_vm_create(const TbfProgram* program, TbfIo io);
void tbf_vm_destroy(TbfVm* self);
TbfStatus tbf_vm_step(TbfVm* self);
TbfStatus tbf_vm_run(TbfVm* self);
bool tbf_vm_awaits_input(const TbfVm* self);
const char* tbf_status_message(TbfStatus status);

#endif
