#include "raylib.h"

#include "tbf/tbf-gui/app.h"

void tbf_gui_app_init(TbfGui_App* self)
{
    self->dark = true;
    self->style.theme = tbf_gui_theme_get(self->dark);
    self->style.ripples = &self->ripples;

    tbf_gui_fonts_load(&self->style.fonts);
    tbf_gui_ripples_init(&self->ripples);
    tbf_gui_terminal_clear(&self->terminal);
    tbf_gui_input_clear(&self->input);
    tbf_gui_toolbar_init(&self->toolbar);
    tbf_gui_code_view_init(&self->code_view);

    tbf_gui_debugger_init(&self->debugger, &self->terminal, &self->input);
    tbf_gui_debugger_set_rate(&self->debugger, tbf_gui_toolbar_rate(&self->toolbar));
}

void tbf_gui_app_free(TbfGui_App* self)
{
    tbf_gui_debugger_free(&self->debugger);
    tbf_gui_code_view_free(&self->code_view);
    tbf_gui_ripples_free(&self->ripples);
    tbf_gui_fonts_unload(&self->style.fonts);
}

void tbf_gui_app_load(TbfGui_App* self, const char* path)
{
    bool loaded = tbf_gui_debugger_load(&self->debugger, path);

    tbf_gui_code_view_load(&self->code_view, loaded ? &self->debugger.program : NULL);
}

void tbf_gui_app_dispatch(TbfGui_App* self, TbfGui_Action action)
{
    switch (action) {
    case TBF_GUI_ACTION_TOGGLE_RUN:
        tbf_gui_debugger_toggle_run(&self->debugger);
        tbf_gui_code_view_follow(&self->code_view);
        break;

    case TBF_GUI_ACTION_STEP:
        tbf_gui_debugger_step(&self->debugger);
        tbf_gui_code_view_follow(&self->code_view);
        break;

    case TBF_GUI_ACTION_SKIP:
        tbf_gui_debugger_skip(&self->debugger);
        tbf_gui_code_view_follow(&self->code_view);
        break;

    case TBF_GUI_ACTION_RESET:
        tbf_gui_debugger_reset(&self->debugger);
        tbf_gui_code_view_follow(&self->code_view);
        break;

    case TBF_GUI_ACTION_SLOWER:
        tbf_gui_toolbar_slower(&self->toolbar);
        tbf_gui_debugger_set_rate(&self->debugger, tbf_gui_toolbar_rate(&self->toolbar));
        break;

    case TBF_GUI_ACTION_FASTER:
        tbf_gui_toolbar_faster(&self->toolbar);
        tbf_gui_debugger_set_rate(&self->debugger, tbf_gui_toolbar_rate(&self->toolbar));
        break;

    case TBF_GUI_ACTION_TOGGLE_THEME:
        self->dark = !self->dark;
        self->style.theme = tbf_gui_theme_get(self->dark);
        break;

    default:
        break;
    }
}

static void tbf_gui_app_poll_files(TbfGui_App* self)
{
    if (!IsFileDropped()) {
        return;
    }

    FilePathList files = LoadDroppedFiles();

    if (files.count > 0) {
        tbf_gui_app_load(self, files.paths[0]);
    }

    UnloadDroppedFiles(files);
}

static void tbf_gui_app_poll_text(TbfGui_App* self)
{
    int codepoint;

    while ((codepoint = GetCharPressed()) != 0) {
        int length = 0;
        const char* bytes = CodepointToUTF8(codepoint, &length);

        for (int i = 0; i < length; i++) {
            tbf_gui_input_push(&self->input, (TbfByte) bytes[i]);
        }
    }

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) {
        tbf_gui_input_push(&self->input, '\n');
    }

    if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)) {
        tbf_gui_input_pop_last(&self->input);
    }
}

static void tbf_gui_app_poll_shortcuts(TbfGui_App* self)
{
    if (IsKeyPressed(KEY_F5)) {
        tbf_gui_app_dispatch(self, TBF_GUI_ACTION_TOGGLE_RUN);
    }

    if (IsKeyPressed(KEY_F10) || IsKeyPressedRepeat(KEY_F10)) {
        tbf_gui_app_dispatch(self, TBF_GUI_ACTION_STEP);
    }

    if (IsKeyPressed(KEY_F11)) {
        tbf_gui_app_dispatch(self, TBF_GUI_ACTION_SKIP);
    }

    if (IsKeyPressed(KEY_F2)) {
        tbf_gui_app_dispatch(self, TBF_GUI_ACTION_RESET);
    }
}

void tbf_gui_app_poll(TbfGui_App* self)
{
    tbf_gui_app_poll_files(self);
    tbf_gui_app_poll_text(self);
    tbf_gui_app_poll_shortcuts(self);
}

void tbf_gui_app_update(TbfGui_App* self)
{
    tbf_gui_debugger_update(&self->debugger, GetFrameTime());
    tbf_gui_ripples_update(&self->ripples);
}

void tbf_gui_app_draw(TbfGui_App* self)
{
    const float margin = TBF_GUI_CARD_PADDING;
    const float gap = 12;
    const float top = 128;
    const float right_x = 460;
    const float right_width = TBF_GUI_WINDOW_WIDTH - right_x - margin;
    const float output_height = TBF_GUI_CARD_HEADER + TBF_GUI_TERMINAL_ROWS * TBF_GUI_LINE_HEIGHT + gap;
    const float tape_y = top + output_height + gap;
    const float tape_height = 154;
    const float input_y = tape_y + tape_height + gap;

    const TbfGui_Style* style = &self->style;
    bool waiting = self->debugger.state == TBF_GUI_DEBUGGER_WAITING_INPUT;

    BeginDrawing();
    ClearBackground(style->theme->surface);

    TbfGui_Action bar_action = tbf_gui_app_bar_draw(style, &self->debugger, self->dark, (Rectangle) { 0, 0, TBF_GUI_WINDOW_WIDTH, 64 });
    TbfGui_Action tool_action = tbf_gui_toolbar_draw(&self->toolbar, style, &self->debugger, (Rectangle) { margin, 72, TBF_GUI_WINDOW_WIDTH - 2 * margin, 40 });

    tbf_gui_code_view_draw(&self->code_view, style, &self->debugger, (Rectangle) { margin, top, right_x - margin - gap, TBF_GUI_WINDOW_HEIGHT - top - margin });
    tbf_gui_output_view_draw(style, &self->terminal, waiting, (Rectangle) { right_x, top, right_width, output_height });
    tbf_gui_tape_view_draw(style, self->debugger.vm, (Rectangle) { right_x, tape_y, right_width, tape_height });
    tbf_gui_input_view_draw(style, &self->input, waiting, (Rectangle) { right_x, input_y, right_width, 56 });

    EndDrawing();

    tbf_gui_app_dispatch(self, bar_action);
    tbf_gui_app_dispatch(self, tool_action);
}
