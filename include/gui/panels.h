#ifndef TINYBF_GUI_PANELS_H
#define TINYBF_GUI_PANELS_H

#include "raylib.h"

#include "gui/app.h"

void panel_toolbar(App* app, Rectangle rect);
void panel_code(App* app, Rectangle rect);
void panel_terminal(App* app, Rectangle rect);
void panel_tape(App* app, Rectangle rect);
void panel_input(App* app, Rectangle rect);

#endif
