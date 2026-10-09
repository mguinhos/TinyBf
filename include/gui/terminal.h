#ifndef TINYBF_GUI_TERMINAL_H
#define TINYBF_GUI_TERMINAL_H

#define TERM_COLS       80
#define TERM_ROWS       25
#define TERM_PARAMS     8

typedef enum EscState {
    ESC_NONE,
    ESC_START,
    ESC_CSI,
}
EscState;

typedef struct Terminal {
    unsigned char cells[TERM_ROWS][TERM_COLS];
    int cx;
    int cy;

    EscState esc;
    int params[TERM_PARAMS];
    int param_count;
}
Terminal;

void terminal_clear(Terminal* term);
void terminal_put(Terminal* term, unsigned char c);

#endif
