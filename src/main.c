#include <stdio.h>
#include <string.h>

#include "tbf/tbf.h"
#include "tbf/tbf-gui/tbf-gui.h"

static void tbf_cli_show_usage(void)
{
    fprintf(stderr, "usage:\n\ttbf <FILENAME>\n\ttbf --gui [FILENAME]\n");
}

static int tbf_cli_run(const char* path)
{
    TbfProgram program;
    TbfProgramStatus program_status = tbf_program_load(&program, path);

    if (program_status == TBF_PROGRAM_UNREADABLE) {
        fprintf(stderr, "error: unable to open file '%s', perhaps the file does not exist or is not ready.\n", path);
        tbf_program_free(&program);
        return 1;
    }

    if (program_status != TBF_PROGRAM_OK) {
        fprintf(stderr, "error: %s (posição %zu)\n", tbf_program_status_message(program_status), program.error_position);
        tbf_program_free(&program);
        return 1;
    }

    TbfVm* vm = tbf_vm_create(&program, tbf_io_stdio());

    if (vm == NULL) {
        fprintf(stderr, "error: out of memory\n");
        tbf_program_free(&program);
        return 1;
    }

    TbfStatus status = tbf_vm_run(vm);

    putchar('\n');

    tbf_vm_destroy(vm);
    tbf_program_free(&program);

    if (status != TBF_STATUS_HALTED) {
        fprintf(stderr, "error: %s\n", tbf_status_message(status));
        return 1;
    }

    return 0;
}

int main(int argc, char* argv[])
{
    if (argc > 1 && strcmp(argv[1], "--gui") == 0) {
        return tbf_gui_run(argc > 2 ? argv[2] : NULL);
    }

    if (argc < 2) {
        fprintf(stderr, "error: missing FILE argument\n");
        tbf_cli_show_usage();
        return 1;
    }

    return tbf_cli_run(argv[1]);
}
