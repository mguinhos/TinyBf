#include <string.h>

#include "tbf/tbf-gui/terminal.h"

static void tbf_gui_terminal_newline(TbfGuiTerminal* self)
{
    self->cx = 0;

    if (++self->cy < TBF_GUI_TERMINAL_ROWS) {
        return;
    }

    memmove(self->cells[0], self->cells[1], TBF_GUI_TERMINAL_COLS * (TBF_GUI_TERMINAL_ROWS - 1));
    memset(self->cells[TBF_GUI_TERMINAL_ROWS - 1], ' ', TBF_GUI_TERMINAL_COLS);
    self->cy = TBF_GUI_TERMINAL_ROWS - 1;
}

static int tbf_gui_terminal_param(const TbfGuiTerminal* self, int index, int fallback)
{
    if (index > self->param_count || self->params[index] == 0) {
        return fallback;
    }

    return self->params[index];
}

static void tbf_gui_terminal_clamp(TbfGuiTerminal* self)
{
    if (self->cx < 0) {
        self->cx = 0;
    }

    if (self->cy < 0) {
        self->cy = 0;
    }

    if (self->cx >= TBF_GUI_TERMINAL_COLS) {
        self->cx = TBF_GUI_TERMINAL_COLS - 1;
    }

    if (self->cy >= TBF_GUI_TERMINAL_ROWS) {
        self->cy = TBF_GUI_TERMINAL_ROWS - 1;
    }
}

static void tbf_gui_terminal_erase_display(TbfGuiTerminal* self)
{
    if (self->params[0] == 2 || self->params[0] == 3) {
        memset(self->cells, ' ', sizeof(self->cells));
        return;
    }

    if (self->params[0] != 0) {
        return;
    }

    memset(&self->cells[self->cy][self->cx], ' ', TBF_GUI_TERMINAL_COLS - self->cx);

    for (int row = self->cy + 1; row < TBF_GUI_TERMINAL_ROWS; row++) {
        memset(self->cells[row], ' ', TBF_GUI_TERMINAL_COLS);
    }
}

static void tbf_gui_terminal_erase_line(TbfGuiTerminal* self)
{
    if (self->params[0] == 2) {
        memset(self->cells[self->cy], ' ', TBF_GUI_TERMINAL_COLS);
    } else if (self->params[0] == 0) {
        memset(&self->cells[self->cy][self->cx], ' ', TBF_GUI_TERMINAL_COLS - self->cx);
    }
}

static void tbf_gui_terminal_csi(TbfGuiTerminal* self, unsigned char command)
{
    switch (command) {
    case 'H':
    case 'f':
        self->cy = tbf_gui_terminal_param(self, 0, 1) - 1;
        self->cx = tbf_gui_terminal_param(self, 1, 1) - 1;
        break;

    case 'A':
        self->cy -= tbf_gui_terminal_param(self, 0, 1);
        break;

    case 'B':
        self->cy += tbf_gui_terminal_param(self, 0, 1);
        break;

    case 'C':
        self->cx += tbf_gui_terminal_param(self, 0, 1);
        break;

    case 'D':
        self->cx -= tbf_gui_terminal_param(self, 0, 1);
        break;

    case 'J':
        tbf_gui_terminal_erase_display(self);
        break;

    case 'K':
        tbf_gui_terminal_erase_line(self);
        break;

    default:
        break;
    }

    tbf_gui_terminal_clamp(self);
}

static void tbf_gui_terminal_parse(TbfGuiTerminal* self, unsigned char c)
{
    if (self->esc == TBF_GUI_ESC_START) {
        self->esc = TBF_GUI_ESC_NONE;

        if (c == '[') {
            memset(self->params, 0, sizeof(self->params));
            self->param_count = 0;
            self->esc = TBF_GUI_ESC_CSI;
        }
        return;
    }

    if (c >= '0' && c <= '9') {
        self->params[self->param_count] = self->params[self->param_count] * 10 + (c - '0');
    } else if (c == ';') {
        if (self->param_count < TBF_GUI_TERMINAL_PARAMS - 1) {
            self->param_count++;
        }
    } else if (c >= 0x40 && c <= 0x7e) {
        tbf_gui_terminal_csi(self, c);
        self->esc = TBF_GUI_ESC_NONE;
    }
}

void tbf_gui_terminal_clear(TbfGuiTerminal* self)
{
    memset(self->cells, ' ', sizeof(self->cells));
    self->cx = 0;
    self->cy = 0;
    self->esc = TBF_GUI_ESC_NONE;
}

void tbf_gui_terminal_put(TbfGuiTerminal* self, unsigned char c)
{
    if (self->esc != TBF_GUI_ESC_NONE) {
        tbf_gui_terminal_parse(self, c);
        return;
    }

    switch (c) {
    case 0x1b:
        self->esc = TBF_GUI_ESC_START;
        return;

    case '\n':
        tbf_gui_terminal_newline(self);
        return;

    case '\r':
        self->cx = 0;
        return;

    case '\b':
        if (self->cx > 0) {
            self->cx--;
        }
        return;

    case '\t':
        self->cx = (self->cx + 8) & ~7;

        if (self->cx >= TBF_GUI_TERMINAL_COLS) {
            tbf_gui_terminal_newline(self);
        }
        return;

    default:
        break;
    }

    if (self->cx >= TBF_GUI_TERMINAL_COLS) {
        tbf_gui_terminal_newline(self);
    }

    self->cells[self->cy][self->cx++] = c;
}
