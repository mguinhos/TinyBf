#include "tbf/tbf-gui/views.h"

static const int tbf_gui_toolbar_rates[] = { 1, 5, 20, 100, 1000, 10000, 100000, 1000000, 0 };
static const char* tbf_gui_toolbar_labels[] = { "1/s", "5/s", "20/s", "100/s", "1k/s", "10k/s", "100k/s", "1M/s", "Máximo" };

#define TBF_GUI_TOOLBAR_SPEEDS ((int) (sizeof(tbf_gui_toolbar_rates) / sizeof(tbf_gui_toolbar_rates[0])))

void tbf_gui_toolbar_init(TbfGuiToolbar* self)
{
    self->speed = TBF_GUI_TOOLBAR_SPEEDS - 1;
}

void tbf_gui_toolbar_slower(TbfGuiToolbar* self)
{
    if (self->speed > 0) {
        self->speed--;
    }
}

void tbf_gui_toolbar_faster(TbfGuiToolbar* self)
{
    if (self->speed < TBF_GUI_TOOLBAR_SPEEDS - 1) {
        self->speed++;
    }
}

int tbf_gui_toolbar_rate(const TbfGuiToolbar* self)
{
    return tbf_gui_toolbar_rates[self->speed];
}

static void tbf_gui_toolbar_draw_status(const TbfGuiFonts* fonts, const TbfGuiDebugger* debugger, float x, float y)
{
    const char* status = "Sem programa";
    Color color = TBF_GUI_COLOR_DIM;

    switch (debugger->state) {
    case TBF_GUI_DEBUGGER_PAUSED:
        status = "Pausado";
        color = TBF_GUI_COLOR_HIGHLIGHT;
        break;

    case TBF_GUI_DEBUGGER_RUNNING:
        status = "Executando";
        color = TBF_GUI_COLOR_SUCCESS;
        break;

    case TBF_GUI_DEBUGGER_WAITING_INPUT:
        status = "Aguardando entrada";
        color = TBF_GUI_COLOR_ACCENT;
        break;

    case TBF_GUI_DEBUGGER_HALTED:
        status = "Finalizado";
        break;

    case TBF_GUI_DEBUGGER_FAILED:
        status = TextFormat("Erro: %s", debugger->error);
        color = TBF_GUI_COLOR_BREAKPOINT;
        break;

    default:
        break;
    }

    DrawCircle(x, y + 16, 6, color);
    tbf_gui_text(fonts->regular, status, x + 14, y + 7, TBF_GUI_COLOR_TEXT);
}

static void tbf_gui_toolbar_draw_info(const TbfGuiFonts* fonts, const TbfGuiDebugger* debugger, Rectangle rect)
{
    if (debugger->vm == NULL) {
        return;
    }

    const char* info = TextFormat("passos %llu   ip %u   tp %u", debugger->steps, debugger->vm->ip, debugger->vm->tp);
    Vector2 size = MeasureTextEx(fonts->small, info, fonts->small.baseSize, 0);

    tbf_gui_text(fonts->small, info, rect.x + rect.width - size.x, rect.y + 9, TBF_GUI_COLOR_DIM);
}

TbfGuiAction tbf_gui_toolbar_draw(const TbfGuiToolbar* self, const TbfGuiFonts* fonts, const TbfGuiDebugger* debugger, Rectangle rect)
{
    TbfGuiAction action = TBF_GUI_ACTION_NONE;
    float x = rect.x;
    float y = rect.y;

    const char* run_label = tbf_gui_debugger_is_running(debugger) ? "Pausar (F5)" : "Executar (F5)";

    if (tbf_gui_button(fonts, (Rectangle) { x, y, 140, 32 }, run_label, tbf_gui_debugger_can_run(debugger))) {
        action = TBF_GUI_ACTION_TOGGLE_RUN;
    }

    if (tbf_gui_button(fonts, (Rectangle) { x + 148, y, 130, 32 }, "Passo (F10)", debugger->state == TBF_GUI_DEBUGGER_PAUSED)) {
        action = TBF_GUI_ACTION_STEP;
    }

    if (tbf_gui_button(fonts, (Rectangle) { x + 286, y, 150, 32 }, "Reiniciar (F2)", debugger->loaded)) {
        action = TBF_GUI_ACTION_RESET;
    }

    tbf_gui_text(fonts->small, "Velocidade", x + 456, y + 9, TBF_GUI_COLOR_DIM);

    if (tbf_gui_button(fonts, (Rectangle) { x + 540, y, 32, 32 }, "-", self->speed > 0)) {
        action = TBF_GUI_ACTION_SLOWER;
    }

    const char* label = tbf_gui_toolbar_labels[self->speed];
    Vector2 size = MeasureTextEx(fonts->regular, label, fonts->regular.baseSize, 0);

    tbf_gui_text(fonts->regular, label, x + 576 + (90 - size.x) / 2, y + 7, TBF_GUI_COLOR_TEXT);

    if (tbf_gui_button(fonts, (Rectangle) { x + 670, y, 32, 32 }, "+", self->speed < TBF_GUI_TOOLBAR_SPEEDS - 1)) {
        action = TBF_GUI_ACTION_FASTER;
    }

    tbf_gui_toolbar_draw_status(fonts, debugger, x + 730, y);
    tbf_gui_toolbar_draw_info(fonts, debugger, rect);

    return action;
}
