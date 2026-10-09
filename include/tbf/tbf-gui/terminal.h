#ifndef TBF_GUI_TERMINAL_H
#define TBF_GUI_TERMINAL_H

#define TBF_GUI_TERMINAL_COLS   80
#define TBF_GUI_TERMINAL_ROWS   25
#define TBF_GUI_TERMINAL_PARAMS 8

typedef enum TbfGuiEscState {
    TBF_GUI_ESC_NONE,
    TBF_GUI_ESC_START,
    TBF_GUI_ESC_CSI,
} TbfGuiEscState;

typedef struct TbfGuiTerminal {
    unsigned char cells[TBF_GUI_TERMINAL_ROWS][TBF_GUI_TERMINAL_COLS];
    int cx;
    int cy;

    TbfGuiEscState esc;
    int params[TBF_GUI_TERMINAL_PARAMS];
    int param_count;
} TbfGuiTerminal;

void tbf_gui_terminal_clear(TbfGuiTerminal* self);
void tbf_gui_terminal_put(TbfGuiTerminal* self, unsigned char c);

#endif
