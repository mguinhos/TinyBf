#include "raylib.h"

#include "tbf/tbf-gui/app.h"

void tbf_gui_app_init(TbfGui_App* self)
{
    self->dark = true;
    self->style.theme = tbf_gui_theme_get(self->dark);
    self->style.ripples = &self->ripples;
    self->style.skeleton = &self->skeleton;

    tbf_gui_fonts_load(&self->style.fonts);
    tbf_gui_ripples_init(&self->ripples);
    tbf_gui_skeleton_init(&self->skeleton);
    tbf_gui_terminal_clear(&self->terminal);
    tbf_gui_input_clear(&self->input);
    tbf_gui_toolbar_init(&self->toolbar);
    tbf_gui_code_view_init(&self->code_view);
    tbf_gui_output_view_init(&self->output_view);

    tbf_gui_debugger_init(&self->debugger, &self->terminal, &self->input);
    tbf_gui_debugger_set_rate(&self->debugger, tbf_gui_toolbar_rate(&self->toolbar));
}

void tbf_gui_app_free(TbfGui_App* self)
{
    tbf_gui_debugger_free(&self->debugger);
    tbf_gui_code_view_free(&self->code_view);
    tbf_gui_skeleton_free(&self->skeleton);
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

    case TBF_GUI_ACTION_TOGGLE_OUTPUT_FULLSCREEN:
        tbf_gui_output_view_toggle_fullscreen(&self->output_view);
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

#define TBF_GUI_APP_MARGIN          16
#define TBF_GUI_APP_GAP             12
#define TBF_GUI_APP_BAR_HEIGHT      64
#define TBF_GUI_APP_TOOLBAR_HEIGHT  40
#define TBF_GUI_APP_INPUT_HEIGHT    56

static Rectangle tbf_gui_app_window(void)
{
    return (Rectangle) { 0, 0, TBF_GUI_WINDOW_WIDTH, TBF_GUI_WINDOW_HEIGHT };
}

static TbfGui_Action tbf_gui_app_draw_fullscreen(TbfGui_App* self, bool waiting)
{
    Rectangle rect = tbf_gui_box(tbf_gui_app_window(), tbf_gui_insets_all(TBF_GUI_APP_MARGIN));

    return tbf_gui_output_view_draw(&self->output_view, &self->style, &self->terminal, waiting, !self->debugger.loaded, rect);
}

static TbfGui_Action tbf_gui_app_draw_side(TbfGui_App* self, bool waiting, Rectangle rect)
{
    const float output_height = TBF_GUI_CARD_HEADER + TBF_GUI_TERMINAL_ROWS * TBF_GUI_LINE_HEIGHT + TBF_GUI_APP_GAP;
    const TbfGui_StackItem items[] = {
        TBF_GUI_FIXED(output_height),
        TBF_GUI_FLEX(1),
        TBF_GUI_FIXED(TBF_GUI_APP_INPUT_HEIGHT),
    };

    Rectangle slots[3];

    tbf_gui_stack(rect, TBF_GUI_STACK_VERTICAL, TBF_GUI_APP_GAP, items, 3, slots);

    TbfGui_Action action = tbf_gui_output_view_draw(&self->output_view, &self->style, &self->terminal, waiting, !self->debugger.loaded, slots[0]);

    tbf_gui_tape_view_draw(&self->style, self->debugger.vm, slots[1]);
    tbf_gui_input_view_draw(&self->style, &self->input, waiting, slots[2]);

    return action;
}

static void tbf_gui_app_draw_workspace(TbfGui_App* self, bool waiting, TbfGui_Action actions[3])
{
    const TbfGui_Style* style = &self->style;
    TbfGui_Stack root = tbf_gui_stack_begin(tbf_gui_app_window(), TBF_GUI_STACK_VERTICAL, 0);

    Rectangle bar = tbf_gui_stack_next(&root, TBF_GUI_APP_BAR_HEIGHT);
    Rectangle body = tbf_gui_box(tbf_gui_stack_rest(&root), (TbfGui_Insets) { 8, TBF_GUI_APP_MARGIN, TBF_GUI_APP_MARGIN, TBF_GUI_APP_MARGIN });

    const TbfGui_StackItem items[] = { TBF_GUI_FIXED(TBF_GUI_APP_TOOLBAR_HEIGHT), TBF_GUI_FLEX(1) };
    Rectangle rows[2];

    tbf_gui_stack(body, TBF_GUI_STACK_VERTICAL, TBF_GUI_APP_MARGIN, items, 2, rows);

    TbfGui_Grid grid = tbf_gui_grid_begin(rows[1], TBF_GUI_GRID_COLUMNS, TBF_GUI_APP_GAP, rows[1].height);
    Rectangle code = tbf_gui_grid_item(&grid, 4);
    Rectangle side = tbf_gui_grid_item(&grid, 8);

    actions[0] = tbf_gui_app_bar_draw(style, &self->debugger, self->dark, bar);
    actions[1] = tbf_gui_toolbar_draw(&self->toolbar, style, &self->debugger, rows[0]);

    tbf_gui_code_view_draw(&self->code_view, style, &self->debugger, code);
    actions[2] = tbf_gui_app_draw_side(self, waiting, side);
}

void tbf_gui_app_draw(TbfGui_App* self)
{
    TbfGui_Action actions[3] = { TBF_GUI_ACTION_NONE, TBF_GUI_ACTION_NONE, TBF_GUI_ACTION_NONE };
    bool waiting = self->debugger.state == TBF_GUI_DEBUGGER_WAITING_INPUT;

    BeginDrawing();
    ClearBackground(self->style.theme->surface);
    tbf_gui_cursor_begin();

    if (self->output_view.fullscreen) {
        actions[0] = tbf_gui_app_draw_fullscreen(self, waiting);
    } else {
        tbf_gui_app_draw_workspace(self, waiting, actions);
    }

    EndDrawing();
    tbf_gui_cursor_apply();

    for (int i = 0; i < 3; i++) {
        tbf_gui_app_dispatch(self, actions[i]);
    }
}
