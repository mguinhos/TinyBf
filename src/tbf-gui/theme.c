#include "tbf/tbf-gui/theme.h"

#define TBF_GUI_RGB(hex) (Color) { ((hex) >> 16) & 0xff, ((hex) >> 8) & 0xff, (hex) & 0xff, 255 }

/* Esquemas baseline do Material Design 3 (cor semente #6750A4). */

static const TbfGui_Theme tbf_gui_theme_light = {
    .primary = TBF_GUI_RGB(0x6750a4),
    .on_primary = TBF_GUI_RGB(0xffffff),
    .primary_container = TBF_GUI_RGB(0xeaddff),
    .on_primary_container = TBF_GUI_RGB(0x21005d),
    .secondary_container = TBF_GUI_RGB(0xe8def8),
    .on_secondary_container = TBF_GUI_RGB(0x1d192b),
    .tertiary = TBF_GUI_RGB(0x7d5260),
    .error = TBF_GUI_RGB(0xb3261e),
    .error_container = TBF_GUI_RGB(0xf9dedc),
    .on_error_container = TBF_GUI_RGB(0x410e0b),

    .surface = TBF_GUI_RGB(0xfef7ff),
    .on_surface = TBF_GUI_RGB(0x1d1b20),
    .on_surface_variant = TBF_GUI_RGB(0x49454f),
    .surface_container_low = TBF_GUI_RGB(0xf7f2fa),
    .surface_container = TBF_GUI_RGB(0xf3edf7),
    .surface_container_high = TBF_GUI_RGB(0xece6f0),
    .surface_container_highest = TBF_GUI_RGB(0xe6e0e9),
    .outline = TBF_GUI_RGB(0x79747e),
    .outline_variant = TBF_GUI_RGB(0xcac4d0),

    .success = TBF_GUI_RGB(0x386a20),
    .warning = TBF_GUI_RGB(0x8b5000),

    .code_arith = TBF_GUI_RGB(0x386a20),
    .code_move = TBF_GUI_RGB(0x0061a4),
    .code_loop = TBF_GUI_RGB(0x6750a4),
    .code_io = TBF_GUI_RGB(0x8b5000),
};

static const TbfGui_Theme tbf_gui_theme_dark = {
    .primary = TBF_GUI_RGB(0xd0bcff),
    .on_primary = TBF_GUI_RGB(0x381e72),
    .primary_container = TBF_GUI_RGB(0x4f378b),
    .on_primary_container = TBF_GUI_RGB(0xeaddff),
    .secondary_container = TBF_GUI_RGB(0x4a4458),
    .on_secondary_container = TBF_GUI_RGB(0xe8def8),
    .tertiary = TBF_GUI_RGB(0xefb8c8),
    .error = TBF_GUI_RGB(0xf2b8b5),
    .error_container = TBF_GUI_RGB(0x8c1d18),
    .on_error_container = TBF_GUI_RGB(0xf9dedc),

    .surface = TBF_GUI_RGB(0x141218),
    .on_surface = TBF_GUI_RGB(0xe6e0e9),
    .on_surface_variant = TBF_GUI_RGB(0xcac4d0),
    .surface_container_low = TBF_GUI_RGB(0x1d1b20),
    .surface_container = TBF_GUI_RGB(0x211f26),
    .surface_container_high = TBF_GUI_RGB(0x2b2930),
    .surface_container_highest = TBF_GUI_RGB(0x36343b),
    .outline = TBF_GUI_RGB(0x938f99),
    .outline_variant = TBF_GUI_RGB(0x49454f),

    .success = TBF_GUI_RGB(0x9cd67d),
    .warning = TBF_GUI_RGB(0xffb870),

    .code_arith = TBF_GUI_RGB(0x9cd67d),
    .code_move = TBF_GUI_RGB(0x9ecaff),
    .code_loop = TBF_GUI_RGB(0xd0bcff),
    .code_io = TBF_GUI_RGB(0xffb870),
};

const TbfGui_Theme* tbf_gui_theme_get(bool dark)
{
    return dark ? &tbf_gui_theme_dark : &tbf_gui_theme_light;
}
