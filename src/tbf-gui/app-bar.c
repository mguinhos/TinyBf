#include "tbf/tbf-gui/views.h"

#define TBF_GUI_APP_BAR_SWITCH_WIDTH    52
#define TBF_GUI_APP_BAR_SWITCH_HEIGHT   32

static void tbf_gui_app_bar_draw_info(const TbfGui_Style* style, const TbfGui_Debugger* debugger, float right, float center_y)
{
    if (debugger->vm == NULL) {
        return;
    }

    const char* info = TextFormat("passos %llu    ip %u    tp %u", debugger->steps, debugger->vm->ip, debugger->vm->tp);
    TbfMath_Vector2 size = tbf_gui_text_size(style->fonts.body, info);

    tbf_gui_text(style->fonts.body, info, right - size.x, center_y - size.y / 2, style->theme->on_surface_variant);
}

TbfGui_Action tbf_gui_app_bar_draw(const TbfGui_Style* style, const TbfGui_Debugger* debugger, bool dark, Rectangle rect)
{
    const TbfGui_Theme* theme = style->theme;
    float center_y = rect.y + rect.height / 2;

    Rectangle toggle = {
        rect.x + rect.width - TBF_GUI_CARD_PADDING - TBF_GUI_APP_BAR_SWITCH_WIDTH,
        center_y - TBF_GUI_APP_BAR_SWITCH_HEIGHT / 2,
        TBF_GUI_APP_BAR_SWITCH_WIDTH,
        TBF_GUI_APP_BAR_SWITCH_HEIGHT,
    };

    TbfMath_Vector2 title = tbf_gui_text_size(style->fonts.title, "TinyBf");

    tbf_gui_text(style->fonts.title, "TinyBf", rect.x + TBF_GUI_CARD_PADDING, center_y - title.y / 2, theme->on_surface);

    const char* label = dark ? "Modo escuro" : "Modo claro";
    TbfMath_Vector2 size = tbf_gui_text_size(style->fonts.body, label);
    float label_x = toggle.x - 12 - size.x;

    tbf_gui_text(style->fonts.body, label, label_x, center_y - size.y / 2, theme->on_surface);
    tbf_gui_app_bar_draw_info(style, debugger, label_x - 32, center_y);

    if (tbf_gui_switch(style, toggle, dark, TBF_GUI_ICON_DARK, TBF_GUI_ICON_LIGHT)) {
        return TBF_GUI_ACTION_TOGGLE_THEME;
    }

    return TBF_GUI_ACTION_NONE;
}
