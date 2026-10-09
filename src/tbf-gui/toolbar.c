#include "tbf/tbf-gui/views.h"

static const int tbf_gui_toolbar_rates[] = { 1, 5, 20, 100, 1000, 10000, 100000, 1000000, 0 };
static const char* tbf_gui_toolbar_labels[] = { "1/s", "5/s", "20/s", "100/s", "1k/s", "10k/s", "100k/s", "1M/s", "Máximo" };

#define TBF_GUI_TOOLBAR_SPEEDS ((int) (sizeof(tbf_gui_toolbar_rates) / sizeof(tbf_gui_toolbar_rates[0])))

void tbf_gui_toolbar_init(TbfGui_Toolbar* self)
{
    self->speed = TBF_GUI_TOOLBAR_SPEEDS - 1;
}

void tbf_gui_toolbar_slower(TbfGui_Toolbar* self)
{
    if (self->speed > 0) {
        self->speed--;
    }
}

void tbf_gui_toolbar_faster(TbfGui_Toolbar* self)
{
    if (self->speed < TBF_GUI_TOOLBAR_SPEEDS - 1) {
        self->speed++;
    }
}

int tbf_gui_toolbar_rate(const TbfGui_Toolbar* self)
{
    return tbf_gui_toolbar_rates[self->speed];
}

static void tbf_gui_toolbar_draw_status(const TbfGui_Style* style, const TbfGui_Debugger* debugger, float x, float center_y)
{
    const TbfGui_Theme* theme = style->theme;
    const char* status = "Sem programa";
    Color color = theme->outline;

    switch (debugger->state) {
    case TBF_GUI_DEBUGGER_PAUSED:
        status = "Pausado";
        color = theme->warning;
        break;

    case TBF_GUI_DEBUGGER_RUNNING:
        status = "Executando";
        color = theme->success;
        break;

    case TBF_GUI_DEBUGGER_WAITING_INPUT:
        status = "Aguardando entrada";
        color = theme->primary;
        break;

    case TBF_GUI_DEBUGGER_HALTED:
        status = "Finalizado";
        color = theme->on_surface_variant;
        break;

    case TBF_GUI_DEBUGGER_FAILED:
        status = TextFormat("Erro: %s", debugger->error);
        color = theme->error;
        break;

    default:
        break;
    }

    TbfMath_Vector2 size = tbf_gui_text_size(style->fonts.label, status);
    Rectangle chip = { x, center_y - 16, size.x + 44, 32 };

    tbf_gui_rounded_lines(chip, 8, 1, theme->outline_variant);
    tbf_gui_circle(tbf_math_vector2(chip.x + 18, center_y), 5, color);
    tbf_gui_text(style->fonts.label, status, chip.x + 32, center_y - size.y / 2, theme->on_surface);
}

TbfGui_Action tbf_gui_toolbar_draw(const TbfGui_Toolbar* self, const TbfGui_Style* style, const TbfGui_Debugger* debugger, Rectangle rect)
{
    const TbfGui_Theme* theme = style->theme;
    TbfGui_Action action = TBF_GUI_ACTION_NONE;
    float x = rect.x;
    float y = rect.y;
    float center_y = rect.y + rect.height / 2;

    bool running = tbf_gui_debugger_is_running(debugger);
    int run_icon = running ? TBF_GUI_ICON_PAUSE : TBF_GUI_ICON_PLAY;
    const char* run_label = running ? "Pausar (F5)" : "Executar (F5)";

    if (tbf_gui_button(style, (Rectangle) { x, y, 152, 40 }, TBF_GUI_BUTTON_FILLED, run_icon, run_label, tbf_gui_debugger_can_run(debugger))) {
        action = TBF_GUI_ACTION_TOGGLE_RUN;
    }

    if (tbf_gui_button(style, (Rectangle) { x + 160, y, 136, 40 }, TBF_GUI_BUTTON_TONAL, TBF_GUI_ICON_STEP, "Passo (F10)", debugger->state == TBF_GUI_DEBUGGER_PAUSED)) {
        action = TBF_GUI_ACTION_STEP;
    }

    if (tbf_gui_button(style, (Rectangle) { x + 304, y, 132, 40 }, TBF_GUI_BUTTON_TONAL, TBF_GUI_ICON_SKIP, "Pular (F11)", debugger->state == TBF_GUI_DEBUGGER_PAUSED)) {
        action = TBF_GUI_ACTION_SKIP;
    }

    if (tbf_gui_button(style, (Rectangle) { x + 444, y, 156, 40 }, TBF_GUI_BUTTON_OUTLINED, TBF_GUI_ICON_RESET, "Reiniciar (F2)", debugger->loaded)) {
        action = TBF_GUI_ACTION_RESET;
    }

    TbfMath_Vector2 caption = tbf_gui_text_size(style->fonts.body, "Velocidade");

    tbf_gui_text(style->fonts.body, "Velocidade", x + 632, center_y - caption.y / 2, theme->on_surface_variant);

    if (tbf_gui_icon_button(style, (Rectangle) { x + 712, y, 40, 40 }, TBF_GUI_ICON_REMOVE, self->speed > 0)) {
        action = TBF_GUI_ACTION_SLOWER;
    }

    const char* label = tbf_gui_toolbar_labels[self->speed];
    TbfMath_Vector2 size = tbf_gui_text_size(style->fonts.label, label);

    tbf_gui_text(style->fonts.label, label, x + 756 + (64 - size.x) / 2, center_y - size.y / 2, theme->on_surface);

    if (tbf_gui_icon_button(style, (Rectangle) { x + 824, y, 40, 40 }, TBF_GUI_ICON_ADD, self->speed < TBF_GUI_TOOLBAR_SPEEDS - 1)) {
        action = TBF_GUI_ACTION_FASTER;
    }

    tbf_gui_toolbar_draw_status(style, debugger, x + 896, center_y);

    return action;
}
