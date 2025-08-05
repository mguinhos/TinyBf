#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>

#include "tinybf.h"

void panic(char* message)
{
    fprintf(stderr, "panic: %s\n", message);
    exit(1);
}

TinyBf* tinybf_create(tinybf_byte* program, size_t program_size)
{
    TinyBf* tinybf = malloc(sizeof(TinyBf));

    if (tinybf == NULL) {
        return NULL;
    }

    tinybf->running = true;

    tinybf->skip_depth = 0;

    tinybf->tp = 0;
    tinybf->sp = 0;
    tinybf->ip = 0;

    for (tinybf_addr i=0; i < TINYBF_TAPE_MAXINDEX; i++) {
        tinybf->tape[i] = 0;
    }

    tinybf->program = program;
    tinybf->program_size = program_size;
    
    return tinybf;
}

TinyBf* tinybf_step(TinyBf* tinybf)
{
    if (tinybf->ip >= tinybf->program_size) {
        tinybf->running = false;
    }

    if (!tinybf->running) {
        putchar('\n');
            
        return tinybf;
    }

    tinybf_byte opcode = tinybf->program[tinybf->ip++];

    if (tinybf->skip_depth) {
        switch (opcode) {
            case '[':
                tinybf->skip_depth++;
                break;
            
            case ']':
                tinybf->skip_depth--;
                
                break;
    
            default:
                break;
        }
    }
    else {
        switch (opcode) {
            case '+':
                tinybf->tape[tinybf->tp]++;
                tinybf->tape[tinybf->tp] %= 0x100;
                
                break;

            case '-':
                tinybf->tape[tinybf->tp]--;
                tinybf->tape[tinybf->tp] %= 0x100;
                
                break;

            case '>':
                tinybf->tp++;
                tinybf->tp %= TINYBF_TAPE_MAXINDEX;
                break;

            case '<':
                tinybf->tp--;
                tinybf->tp %= TINYBF_TAPE_MAXINDEX;
                break;

            case '[':
                if (tinybf->tape[tinybf->tp]) {
                    if (tinybf->sp >= TINYBF_STACK_MAXINDEX)
                        panic("stack overflow");

                    tinybf->stack[tinybf->sp++] = tinybf->ip -1;
                }
                else {
                    tinybf->skip_depth++;
                }
                break;
            
            case ']':
                if (tinybf->sp == 0)
                    panic("stack undeflow");

                if (tinybf->tape[tinybf->tp]) {
                    tinybf->ip = tinybf->stack[--tinybf->sp];
                }
                else {
                    tinybf->sp--;
                }
                break;

            case '.':
                putc(tinybf->tape[tinybf->tp], stdout);
                fflush(stdout);
                break;

            case ',':
                tinybf->tape[tinybf->tp] = getc(stdin);
                fflush(stdin);
                break;

            default:
                break;
        }
    }

    return tinybf;
}