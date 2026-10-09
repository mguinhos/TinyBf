#ifndef TBF_GUI_THEME_H
#define TBF_GUI_THEME_H

#include <stdbool.h>

#include "raylib.h"

typedef struct TbfGui_Theme {
    Color primary;
    Color on_primary;
    Color primary_container;
    Color on_primary_container;
    Color secondary_container;
    Color on_secondary_container;
    Color tertiary;
    Color error;
    Color error_container;
    Color on_error_container;

    Color surface;
    Color on_surface;
    Color on_surface_variant;
    Color surface_container_low;
    Color surface_container;
    Color surface_container_high;
    Color surface_container_highest;
    Color outline;
    Color outline_variant;

    Color success;
    Color warning;

    Color code_arith;
    Color code_move;
    Color code_loop;
    Color code_io;
} TbfGui_Theme;

const TbfGui_Theme* tbf_gui_theme_get(bool dark);

#endif
