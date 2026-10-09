#include "raylib.h"

#include "tbf/tbf-gui/app.h"

void tbf_gui_app_init(TbfGuiApp* self)
{
    tbf_gui_fonts_load(&self->fonts);
    tbf_gui_terminal_clear(&self->terminal);
    tbf_gui_input_clear(&self->input);
    tbf_gui_toolbar_init(&self->toolbar);
    tbf_gui_code_view_init(&self->code_view);

    tbf_gui_debugger_init(&self->debugger, &self->terminal, &self->input);
    tbf_gui_debugger_set_rate(&self->debugger, tbf_gui_toolbar_rate(&self->toolbar));
}

void tbf_gui_app_free(TbfGuiApp* self)
{
    tbf_gui_debugger_free(&self->debugger);
    tbf_gui_code_view_free(&self->code_view);
    tbf_gui_fonts_unload(&self->fonts);
}

void tbf_gui_app_load(TbfGuiApp* self, const char* path)
{
    bool loaded = tbf_gui_debugger_load(&self->debugger, path);

    tbf_gui_code_view_load(&self->code_view, loaded ? &self->debugger.program : NULL);
}

void tbf_gui_app_dispatch(TbfGuiApp* self, TbfGuiAction action)
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

    default:
        break;
    }
}

static void tbf_gui_app_poll_files(TbfGuiApp* self)
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

static void tbf_gui_app_poll_text(TbfGuiApp* self)
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

static void tbf_gui_app_poll_shortcuts(TbfGuiApp* self)
{
    if (IsKeyPressed(KEY_F5)) {
        tbf_gui_app_dispatch(self, TBF_GUI_ACTION_TOGGLE_RUN);
    }

    if (IsKeyPressed(KEY_F10) || IsKeyPressedRepeat(KEY_F10)) {
        tbf_gui_app_dispatch(self, TBF_GUI_ACTION_STEP);
    }

    if (IsKeyPressed(KEY_F2)) {
        tbf_gui_app_dispatch(self, TBF_GUI_ACTION_RESET);
    }
}

void tbf_gui_app_poll(TbfGuiApp* self)
{
    tbf_gui_app_poll_files(self);
    tbf_gui_app_poll_text(self);
    tbf_gui_app_poll_shortcuts(self);
}

void tbf_gui_app_update(TbfGuiApp* self)
{
    tbf_gui_debugger_update(&self->debugger, GetFrameTime());
}

void tbf_gui_app_draw(TbfGuiApp* self)
{
    const float right_x = 460;
    const float right_width = TBF_GUI_WINDOW_WIDTH - right_x - 10;
    const float output_height = TBF_GUI_HEADER_HEIGHT + TBF_GUI_TERMINAL_ROWS * TBF_GUI_LINE_HEIGHT + TBF_GUI_PADDING;

    bool waiting = self->debugger.state == TBF_GUI_DEBUGGER_WAITING_INPUT;

    BeginDrawing();
    ClearBackground(TBF_GUI_COLOR_BG);

    TbfGuiAction action = tbf_gui_toolbar_draw(
        &self->toolbar,
        &self->fonts,
        &self->debugger,
        (Rectangle) { 10, 10, TBF_GUI_WINDOW_WIDTH - 20, 32 }
    );

    tbf_gui_code_view_draw(&self->code_view, &self->fonts, &self->debugger, (Rectangle) { 10, 52, 440, 748 });
    tbf_gui_output_view_draw(&self->fonts, &self->terminal, waiting, (Rectangle) { right_x, 52, right_width, output_height });
    tbf_gui_tape_view_draw(&self->fonts, self->debugger.vm, (Rectangle) { right_x, 552, right_width, 182 });
    tbf_gui_input_view_draw(&self->fonts, &self->input, waiting, (Rectangle) { right_x, 744, right_width, 56 });

    EndDrawing();

    tbf_gui_app_dispatch(self, action);
}
