#include "gui/panels.h"
#include "gui/widgets.h"

#define TAPE_COLS   16
#define TAPE_ROWS   2

static Color opcode_color(tinybf_byte opcode)
{
    switch (opcode) {
        case '+': case '-': return COLOR_OP_ARITH;
        case '<': case '>': return COLOR_OP_MOVE;
        case '[': case ']': return COLOR_OP_LOOP;
        default:            return COLOR_OP_IO;
    }
}

static int printable(unsigned char c)
{
    return (c >= 0x20 && c < 0x7f) ? c : '?';
}

void panel_toolbar(App* app, Rectangle rect)
{
    float x = rect.x;
    float y = rect.y;

    bool can_run = app->state == STATE_PAUSED || app->state == STATE_RUNNING || app->state == STATE_WAITING_INPUT;

    if (ui_button((Rectangle){ x, y, 140, 32 }, app_is_running(app) ? "Pausar (F5)" : "Executar (F5)", can_run))
        app_toggle_run(app);

    if (ui_button((Rectangle){ x + 148, y, 130, 32 }, "Passo (F10)", app->state == STATE_PAUSED))
        app_single_step(app);

    if (ui_button((Rectangle){ x + 286, y, 150, 32 }, "Reiniciar (F2)", app->state != STATE_EMPTY))
        app_reset(app);

    ui_text(ui_font_small, "Velocidade", x + 456, y + 9, COLOR_DIM);

    if (ui_button((Rectangle){ x + 540, y, 32, 32 }, "-", app->speed > 0))
        app->speed--;

    const char* speed = app_speed_label(app);
    Vector2 speed_size = MeasureTextEx(ui_font, speed, ui_font.baseSize, 0);

    ui_text(ui_font, speed, x + 576 + (90 - speed_size.x) / 2, y + 7, COLOR_TEXT);

    if (ui_button((Rectangle){ x + 670, y, 32, 32 }, "+", app->speed < app_speed_count - 1))
        app->speed++;

    const char* status = "Sem programa";
    Color status_color = COLOR_DIM;

    switch (app->state) {
        case STATE_PAUSED:        status = "Pausado";            status_color = COLOR_HIGHLIGHT; break;
        case STATE_RUNNING:       status = "Executando";         status_color = COLOR_SUCCESS;   break;
        case STATE_WAITING_INPUT: status = "Aguardando entrada"; status_color = COLOR_ACCENT;    break;
        case STATE_HALTED:        status = "Finalizado";         status_color = COLOR_DIM;       break;
        default: break;
    }

    DrawCircle(x + 730, y + 16, 6, status_color);
    ui_text(ui_font, status, x + 744, y + 7, COLOR_TEXT);

    if (app->bf) {
        const char* info = TextFormat("passos %llu   ip %u   tp %u", app->steps, app->bf->ip, app->bf->tp);
        Vector2 info_size = MeasureTextEx(ui_font_small, info, ui_font_small.baseSize, 0);

        ui_text(ui_font_small, info, rect.x + rect.width - info_size.x, y + 9, COLOR_DIM);
    }
}

static void code_handle_click(App* app, Rectangle area, int cols)
{
    Vector2 mouse = GetMousePosition();
    int col = (int) ((mouse.x - area.x) / ui_char_width);
    int row = (int) ((mouse.y - area.y) / UI_LINE_HEIGHT) + app->code_scroll;
    size_t k = (size_t) row * cols + col;

    if (col < cols && k < app->code_size) {
        size_t index = app->code_index[k];

        app->breakpoints[index] = !app->breakpoints[index];
    }
}

void panel_code(App* app, Rectangle rect)
{
    const char* title = app->path[0] ? TextFormat("PROGRAMA: %s", GetFileName(app->path)) : "PROGRAMA";

    ui_panel(rect, title, COLOR_BORDER);

    Rectangle area = {
        rect.x + UI_PADDING,
        rect.y + UI_HEADER_HEIGHT,
        rect.width - 2 * UI_PADDING - 6,
        rect.height - UI_HEADER_HEIGHT - UI_PADDING,
    };

    if (app->error[0]) {
        ui_text(ui_font, "Erro ao carregar:", area.x, area.y + 4, COLOR_BREAKPOINT);
        ui_text(ui_font_small, app->error, area.x, area.y + 28, COLOR_TEXT);
        return;
    }

    if (app->program == NULL) {
        ui_text(ui_font, "Arraste um arquivo .bf para a janela", area.x, area.y + 4, COLOR_DIM);
        return;
    }

    int cols = (int) (area.width / ui_char_width);
    int visible_rows = (int) (area.height / UI_LINE_HEIGHT);
    int total_rows = (int) ((app->code_size + cols - 1) / cols);
    int max_scroll = total_rows > visible_rows ? total_rows - visible_rows : 0;

    size_t ip_pos = app->bf ? app_code_position(app, app->bf->ip) : app->code_size;
    bool show_ip = app->bf && app->bf->running && ip_pos < app->code_size;
    int ip_row = (int) (ip_pos / cols);

    bool hover = CheckCollisionPointRec(GetMousePosition(), area);
    float wheel = hover ? GetMouseWheelMove() : 0;

    if (wheel != 0) {
        app->code_scroll -= (int) (wheel * 3);
        app->code_follow = false;
    }

    if (app->code_follow && show_ip && (ip_row < app->code_scroll || ip_row >= app->code_scroll + visible_rows)) {
        app->code_scroll = ip_row - visible_rows / 2;
    }

    if (app->code_scroll > max_scroll) app->code_scroll = max_scroll;
    if (app->code_scroll < 0) app->code_scroll = 0;

    if (hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        code_handle_click(app, area, cols);
    }

    BeginScissorMode((int) area.x, (int) area.y, (int) area.width, (int) area.height);

    for (int row = app->code_scroll; row < app->code_scroll + visible_rows && row < total_rows; row++) {
        for (int col = 0; col < cols; col++) {
            size_t k = (size_t) row * cols + col;

            if (k >= app->code_size)
                break;

            size_t index = app->code_index[k];
            tinybf_byte opcode = app->program[index];
            Rectangle cell = { area.x + col * ui_char_width, area.y + (row - app->code_scroll) * UI_LINE_HEIGHT, ui_char_width, UI_LINE_HEIGHT };
            Color color = opcode_color(opcode);

            if (app->breakpoints[index]) {
                DrawRectangleRec(cell, Fade(COLOR_BREAKPOINT, 0.45f));
            }

            if (show_ip && k == ip_pos) {
                DrawRectangleRec(cell, COLOR_HIGHLIGHT);
                color = COLOR_BG;
            }

            ui_char(ui_font, opcode, cell.x, cell.y, color);
        }
    }

    EndScissorMode();

    if (total_rows > visible_rows) {
        float bar_height = area.height * visible_rows / total_rows;
        float bar_y = area.y + (area.height - bar_height) * app->code_scroll / max_scroll;

        DrawRectangleRounded((Rectangle){ rect.x + rect.width - 8, bar_y, 4, bar_height }, 1, 4, COLOR_BORDER);
    }
}

void panel_terminal(App* app, Rectangle rect)
{
    ui_panel(rect, "SAÍDA", COLOR_BORDER);

    float x = rect.x + UI_PADDING;
    float y = rect.y + UI_HEADER_HEIGHT;

    for (int row = 0; row < TERM_ROWS; row++) {
        for (int col = 0; col < TERM_COLS; col++) {
            unsigned char c = app->term.cells[row][col];

            if (c != ' ') {
                ui_char(ui_font, printable(c), x + col * ui_char_width, y + row * UI_LINE_HEIGHT, COLOR_TEXT);
            }
        }
    }

    if (app->state == STATE_WAITING_INPUT && (int) (GetTime() * 2) % 2 == 0) {
        DrawRectangle(x + app->term.cx * ui_char_width, y + app->term.cy * UI_LINE_HEIGHT, ui_char_width, UI_LINE_HEIGHT, COLOR_ACCENT);
    }
}

void panel_tape(App* app, Rectangle rect)
{
    ui_panel(rect, "MEMÓRIA", COLOR_BORDER);

    const int per_page = TAPE_COLS * TAPE_ROWS;
    const float gap = 4;

    tinybf_addr tp = app->bf ? app->bf->tp : 0;
    size_t start = (tp / per_page) * per_page;

    float cell_width = (rect.width - 2 * UI_PADDING - (TAPE_COLS - 1) * gap) / TAPE_COLS;
    float cell_height = (rect.height - UI_HEADER_HEIGHT - UI_PADDING - (TAPE_ROWS - 1) * gap) / TAPE_ROWS;

    for (int i = 0; i < per_page; i++) {
        size_t addr = start + i;
        tinybf_cell value = app->bf ? app->bf->tape[addr] : 0;
        bool current = app->bf && addr == tp;

        Rectangle cell = {
            rect.x + UI_PADDING + (i % TAPE_COLS) * (cell_width + gap),
            rect.y + UI_HEADER_HEIGHT + (i / TAPE_COLS) * (cell_height + gap),
            cell_width,
            cell_height,
        };

        Color bg = current ? COLOR_ACCENT : value ? COLOR_BORDER : COLOR_CELL;
        Color fg = current ? COLOR_BG : value ? COLOR_TEXT : COLOR_DIM;

        DrawRectangleRounded(cell, 0.15f, 4, bg);

        ui_text(ui_font_small, TextFormat("%04zX", addr), cell.x + 4, cell.y + 3, current ? COLOR_BG : COLOR_DIM);

        const char* text = TextFormat("%d", value);
        Vector2 size = MeasureTextEx(ui_font, text, ui_font.baseSize, 0);

        ui_text(ui_font, text, cell.x + (cell.width - size.x) / 2, cell.y + (cell.height - size.y) / 2 + 2, fg);

        if (value > 0x20 && value < 0x7f) {
            ui_char(ui_font_small, value, cell.x + cell.width - 12, cell.y + cell.height - 16, fg);
        }
    }
}

void panel_input(App* app, Rectangle rect)
{
    bool waiting = app->state == STATE_WAITING_INPUT;

    ui_panel(rect, "ENTRADA", waiting ? COLOR_ACCENT : COLOR_BORDER);

    float x = rect.x + UI_PADDING + 80;
    float y = rect.y + (rect.height - UI_LINE_HEIGHT) / 2;

    if (app->input_len == 0) {
        const char* hint = waiting
            ? "Digite algo: o programa está esperando um ','"
            : "Digite para enfileirar entrada (Enter = \\n, Backspace apaga)";

        ui_text(ui_font_small, hint, x, y + 3, waiting ? COLOR_ACCENT : COLOR_DIM);
        return;
    }

    float max_x = rect.x + rect.width - UI_PADDING - 2 * ui_char_width;

    for (int i = 0; i < app->input_len && x < max_x; i++) {
        tinybf_byte c = app_input_peek(app, i);

        if (c == '\n') {
            ui_text(ui_font, "\\n", x, y, COLOR_DIM);
            x += 2 * ui_char_width;
        }
        else {
            ui_char(ui_font, printable(c), x, y, COLOR_TEXT);
            x += ui_char_width;
        }
    }
}
