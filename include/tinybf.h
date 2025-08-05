#ifndef TINYBF_H
#define TINYBF_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef uint8_t tinybf_byte;
typedef uint16_t tinybf_word;
typedef uint32_t tinybf_dword;

typedef tinybf_byte tinybf_cell;
typedef tinybf_word tinybf_addr;
typedef tinybf_dword tinybf_daddr;

#define TINYBF_TAPE_SIZE    0x10000
#define TINYBF_STACK_SIZE   0x1000

#define TINYBF_TAPE_MAXINDEX TINYBF_TAPE_SIZE -1
#define TINYBF_STACK_MAXINDEX TINYBF_STACK_SIZE -1


typedef struct TinyBf {
    bool running;

    tinybf_addr skip_depth;

    tinybf_addr tp;
    tinybf_addr sp;
    tinybf_daddr ip;

    tinybf_cell tape[TINYBF_TAPE_SIZE];
    tinybf_addr stack[TINYBF_STACK_SIZE];

    size_t program_size;
    tinybf_byte* program;
}
TinyBf;

TinyBf* tinybf_create(tinybf_byte* program, size_t program_size);
TinyBf* tinybf_step(TinyBf* tinybf);

#endif