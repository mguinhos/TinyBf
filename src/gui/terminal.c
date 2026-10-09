#include <string.h>

#include "gui/terminal.h"

static void terminal_newline(Terminal* term)
{
    term->cx = 0;

    if (++term->cy < TERM_ROWS) {
        return;
    }

    memmove(term->cells[0], term->cells[1], TERM_COLS * (TERM_ROWS - 1));
    memset(term->cells[TERM_ROWS - 1], ' ', TERM_COLS);
    term->cy = TERM_ROWS - 1;
}

static int terminal_param(Terminal* term, int index, int fallback)
{
    if (index > term->param_count || term->params[index] == 0) {
        return fallback;
    }

    return term->params[index];
}

static void terminal_csi(Terminal* term, unsigned char command)
{
    switch (command) {
        case 'H':
        case 'f':
            term->cy = terminal_param(term, 0, 1) - 1;
            term->cx = terminal_param(term, 1, 1) - 1;
            break;

        case 'A':
            term->cy -= terminal_param(term, 0, 1);
            break;

        case 'B':
            term->cy += terminal_param(term, 0, 1);
            break;

        case 'C':
            term->cx += terminal_param(term, 0, 1);
            break;

        case 'D':
            term->cx -= terminal_param(term, 0, 1);
            break;

        case 'J':
            if (term->params[0] == 2 || term->params[0] == 3) {
                memset(term->cells, ' ', sizeof(term->cells));
            }
            else if (term->params[0] == 0) {
                memset(&term->cells[term->cy][term->cx], ' ', TERM_COLS - term->cx);
                for (int row = term->cy + 1; row < TERM_ROWS; row++) {
                    memset(term->cells[row], ' ', TERM_COLS);
                }
            }
            break;

        case 'K':
            if (term->params[0] == 2) {
                memset(term->cells[term->cy], ' ', TERM_COLS);
            }
            else if (term->params[0] == 0) {
                memset(&term->cells[term->cy][term->cx], ' ', TERM_COLS - term->cx);
            }
            break;

        default:
            break;
    }

    if (term->cx < 0) term->cx = 0;
    if (term->cy < 0) term->cy = 0;
    if (term->cx >= TERM_COLS) term->cx = TERM_COLS - 1;
    if (term->cy >= TERM_ROWS) term->cy = TERM_ROWS - 1;
}

void terminal_clear(Terminal* term)
{
    memset(term->cells, ' ', sizeof(term->cells));
    term->cx = 0;
    term->cy = 0;
    term->esc = ESC_NONE;
}

void terminal_put(Terminal* term, unsigned char c)
{
    switch (term->esc) {
        case ESC_START:
            term->esc = ESC_NONE;

            if (c == '[') {
                memset(term->params, 0, sizeof(term->params));
                term->param_count = 0;
                term->esc = ESC_CSI;
            }
            return;

        case ESC_CSI:
            if (c >= '0' && c <= '9') {
                term->params[term->param_count] = term->params[term->param_count] * 10 + (c - '0');
            }
            else if (c == ';') {
                if (term->param_count < TERM_PARAMS - 1)
                    term->param_count++;
            }
            else if (c >= 0x40 && c <= 0x7e) {
                terminal_csi(term, c);
                term->esc = ESC_NONE;
            }
            return;

        default:
            break;
    }

    switch (c) {
        case 0x1b:
            term->esc = ESC_START;
            return;

        case '\n':
            terminal_newline(term);
            return;

        case '\r':
            term->cx = 0;
            return;

        case '\b':
            if (term->cx > 0)
                term->cx--;
            return;

        case '\t':
            term->cx = (term->cx + 8) & ~7;
            if (term->cx >= TERM_COLS)
                terminal_newline(term);
            return;

        default:
            break;
    }

    if (term->cx >= TERM_COLS) {
        terminal_newline(term);
    }

    term->cells[term->cy][term->cx++] = c;
}
