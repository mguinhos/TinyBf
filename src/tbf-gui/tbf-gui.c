#include "raylib.h"

#include "tbf/tbf-gui/tbf-gui.h"
#include "tbf/tbf-gui/app.h"

int tbf_gui_run(const char* path)
{
    static TbfGuiApp app;

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
    InitWindow(TBF_GUI_WINDOW_WIDTH, TBF_GUI_WINDOW_HEIGHT, "TinyBf");
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);

    tbf_gui_app_init(&app);

    if (path != NULL) {
        tbf_gui_app_load(&app, path);
    }

    while (!WindowShouldClose()) {
        tbf_gui_app_poll(&app);
        tbf_gui_app_update(&app);
        tbf_gui_app_draw(&app);
    }

    tbf_gui_app_free(&app);
    CloseWindow();

    return 0;
}
