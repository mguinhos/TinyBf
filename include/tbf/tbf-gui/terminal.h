#ifndef TBF_GUI_TERMINAL_H
#define TBF_GUI_TERMINAL_H

#define TBF_GUI_TERMINAL_COLS   80
#define TBF_GUI_TERMINAL_ROWS   25
#define TBF_GUI_TERMINAL_PARAMS 8

typedef enum TbfGui_EscState {
    TBF_GUI_ESC_NONE,
    TBF_GUI_ESC_START,
    TBF_GUI_ESC_CSI,
} TbfGui_EscState;

typedef struct TbfGui_Terminal {
    unsigned char cells[TBF_GUI_TERMINAL_ROWS][TBF_GUI_TERMINAL_COLS];
    int cx;
    int cy;

    TbfGui_EscState esc;
    int params[TBF_GUI_TERMINAL_PARAMS];
    int param_count;
} TbfGui_Terminal;

void tbf_gui_terminal_clear(TbfGui_Terminal* self);
void tbf_gui_terminal_put(TbfGui_Terminal* self, unsigned char c);

#endif
