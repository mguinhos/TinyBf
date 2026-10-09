#include <stdio.h>
#include <string.h>

#include "tinybf.c"

void show_usage(void)
{
    fprintf(stderr, "usage:\n\ttinyf <FILENAME>\n");
}

int main_noargs(void)
{
    fprintf(stderr, "error: missing FILE argument\n");
    show_usage();

    return 1;
}

int main_onearg(char* filename)
{

    FILE* fp = fopen(filename, "rb");

    if (fp == NULL) {
        fprintf(stderr, "error: unable to open file '%s', perhaps the file does not exist or is not ready.\n", filename);

        return 1;
    }

    size_t filesize;

    fseek(fp, 0L, SEEK_END);
    filesize = ftell(fp);
    rewind(fp);

    tinybf_byte* filedata = malloc(sizeof(tinybf_byte) * filesize);

    fread(filedata, sizeof(tinybf_byte), filesize, fp);

    fclose(fp);

    TinyBf* bf = tinybf_create(filedata, filesize);

    while (bf->running)  {
        tinybf_step(bf);
    }

    putchar('\n');

    return 0;
}

int main(int argc, char* argv[])
{
    if (argc == 0)
        return main_noargs();
    else if (argc == 1)
        return main_noargs();
    
    return main_onearg(argv[1]);        
}