#include "raylib.h"

#include "gui/app.h"
#include "gui/panels.h"
#include "gui/widgets.h"

#define WINDOW_WIDTH    1280
#define WINDOW_HEIGHT   810

static void handle_dropped_files(App* app)
{
    if (!IsFileDropped()) {
        return;
    }

    FilePathList files = LoadDroppedFiles();

    if (files.count > 0) {
        app_load(app, files.paths[0]);
    }

    UnloadDroppedFiles(files);
}

static void handle_keyboard(App* app)
{
    int codepoint;

    while ((codepoint = GetCharPressed()) != 0) {
        int length = 0;
        const char* bytes = CodepointToUTF8(codepoint, &length);

        for (int i = 0; i < length; i++) {
            app_input_push(app, (tinybf_byte) bytes[i]);
        }
    }

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER))
        app_input_push(app, '\n');

    if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE))
        app_input_pop_last(app);

    if (IsKeyPressed(KEY_F5))
        app_toggle_run(app);

    if (IsKeyPressed(KEY_F10) || IsKeyPressedRepeat(KEY_F10))
        app_single_step(app);

    if (IsKeyPressed(KEY_F2))
        app_reset(app);
}

static void draw(App* app)
{
    const float right_x = 460;
    const float right_width = WINDOW_WIDTH - right_x - 10;
    const float terminal_height = UI_HEADER_HEIGHT + TERM_ROWS * UI_LINE_HEIGHT + UI_PADDING;

    BeginDrawing();
    ClearBackground(COLOR_BG);

    panel_toolbar(app, (Rectangle){ 10, 10, WINDOW_WIDTH - 20, 32 });
    panel_code(app, (Rectangle){ 10, 52, 440, 748 });
    panel_terminal(app, (Rectangle){ right_x, 52, right_width, terminal_height });
    panel_tape(app, (Rectangle){ right_x, 552, right_width, 182 });
    panel_input(app, (Rectangle){ right_x, 744, right_width, 56 });

    EndDrawing();
}

int main(int argc, char* argv[])
{
    static App app;

    app_init(&app);

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "TinyBf");
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);

    ui_load_fonts();

    if (argc > 1) {
        app_load(&app, argv[1]);
    }

    while (!WindowShouldClose()) {
        handle_dropped_files(&app);
        handle_keyboard(&app);

        app_update(&app, GetFrameTime());

        draw(&app);
    }

    app_free(&app);
    ui_unload_fonts();

    CloseWindow();

    return 0;
}
