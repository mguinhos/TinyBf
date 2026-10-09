#include <stdio.h>

#include "tbf/tbf.h"

static void tbf_io_stdio_output(void* context, TbfCell value)
{
    (void) context;

    putc(value, stdout);
    fflush(stdout);
}

static TbfCell tbf_io_stdio_input(void* context)
{
    (void) context;

    return getc(stdin);
}

TbfIo tbf_io_stdio(void)
{
    return (TbfIo) {
        .output = tbf_io_stdio_output,
        .input = tbf_io_stdio_input,
        .context = NULL,
    };
}
