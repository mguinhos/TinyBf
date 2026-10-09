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

static void tbf_gui_toolbar_draw_status(const TbfGui_Style* style, const TbfGui_Debugger* debugger, TbfGui_Stack* stack)
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

    float width = tbf_gui_text_size(style->fonts.label, status).x;
    Rectangle chip = tbf_gui_box_align(tbf_gui_stack_next(stack, width + 44), width + 44, 32, TBF_GUI_ALIGN_START, TBF_GUI_ALIGN_CENTER);
    TbfGui_Stack content = tbf_gui_stack_begin(tbf_gui_box(chip, tbf_gui_insets_xy(13, 0)), TBF_GUI_STACK_HORIZONTAL, 9);
    Rectangle dot = tbf_gui_stack_next(&content, 10);

    tbf_gui_rounded_lines(chip, 8, 1, theme->outline_variant);
    tbf_gui_circle(tbf_math_vector2(dot.x + dot.width / 2, dot.y + dot.height / 2), 5, color);
    tbf_gui_label(style->fonts.label, status, tbf_gui_stack_rest(&content), TBF_GUI_ALIGN_START, theme->on_surface);
}

TbfGui_Action tbf_gui_toolbar_draw(const TbfGui_Toolbar* self, const TbfGui_Style* style, const TbfGui_Debugger* debugger, Rectangle rect)
{
    const TbfGui_Theme* theme = style->theme;
    TbfGui_Action action = TBF_GUI_ACTION_NONE;
    TbfGui_Stack stack = tbf_gui_stack_begin(rect, TBF_GUI_STACK_HORIZONTAL, 8);

    bool paused = debugger->state == TBF_GUI_DEBUGGER_PAUSED;
    bool running = tbf_gui_debugger_is_running(debugger);
    int run_icon = running ? TBF_GUI_ICON_PAUSE : TBF_GUI_ICON_PLAY;
    const char* run_label = running ? "Pausar (F5)" : "Executar (F5)";

    if (tbf_gui_button(style, tbf_gui_stack_next(&stack, 152), TBF_GUI_BUTTON_FILLED, run_icon, run_label, tbf_gui_debugger_can_run(debugger))) {
        action = TBF_GUI_ACTION_TOGGLE_RUN;
    }

    if (tbf_gui_button(style, tbf_gui_stack_next(&stack, 136), TBF_GUI_BUTTON_TONAL, TBF_GUI_ICON_STEP, "Passo (F10)", paused)) {
        action = TBF_GUI_ACTION_STEP;
    }

    if (tbf_gui_button(style, tbf_gui_stack_next(&stack, 132), TBF_GUI_BUTTON_TONAL, TBF_GUI_ICON_SKIP, "Pular (F11)", paused)) {
        action = TBF_GUI_ACTION_SKIP;
    }

    if (tbf_gui_button(style, tbf_gui_stack_next(&stack, 156), TBF_GUI_BUTTON_OUTLINED, TBF_GUI_ICON_RESET, "Reiniciar (F2)", debugger->loaded)) {
        action = TBF_GUI_ACTION_RESET;
    }

    tbf_gui_stack_skip(&stack, 24);

    const char* caption = "Velocidade";

    tbf_gui_label(style->fonts.body, caption, tbf_gui_stack_next(&stack, tbf_gui_text_size(style->fonts.body, caption).x), TBF_GUI_ALIGN_START, theme->on_surface_variant);

    if (tbf_gui_icon_button(style, tbf_gui_stack_next(&stack, 40), TBF_GUI_ICON_REMOVE, self->speed > 0)) {
        action = TBF_GUI_ACTION_SLOWER;
    }

    tbf_gui_label(style->fonts.label, tbf_gui_toolbar_labels[self->speed], tbf_gui_stack_next(&stack, 64), TBF_GUI_ALIGN_CENTER, theme->on_surface);

    if (tbf_gui_icon_button(style, tbf_gui_stack_next(&stack, 40), TBF_GUI_ICON_ADD, self->speed < TBF_GUI_TOOLBAR_SPEEDS - 1)) {
        action = TBF_GUI_ACTION_FASTER;
    }

    tbf_gui_stack_skip(&stack, 24);
    tbf_gui_toolbar_draw_status(style, debugger, &stack);

    return action;
}
