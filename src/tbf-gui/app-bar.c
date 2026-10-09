#include "tbf/tbf-gui/views.h"

#define TBF_GUI_APP_BAR_SWITCH_WIDTH    52
#define TBF_GUI_APP_BAR_SWITCH_HEIGHT   32
#define TBF_GUI_APP_BAR_INFO_SKELETON   200

static void tbf_gui_app_bar_draw_info(const TbfGui_Style* style, const TbfGui_Debugger* debugger, TbfGui_Stack* stack)
{
    if (debugger->vm == NULL) {
        Rectangle slot = tbf_gui_stack_next(stack, TBF_GUI_APP_BAR_INFO_SKELETON);

        tbf_gui_skeleton(style, tbf_gui_box_align(slot, slot.width, 14, TBF_GUI_ALIGN_START, TBF_GUI_ALIGN_CENTER), 7);
        return;
    }

    const char* info = TextFormat("passos %llu    ip %u    tp %u", debugger->steps, debugger->vm->ip, debugger->vm->tp);
    float width = tbf_gui_text_size(style->fonts.body, info).x;

    tbf_gui_label(style->fonts.body, info, tbf_gui_stack_next(stack, width), TBF_GUI_ALIGN_START, style->theme->on_surface_variant);
}

TbfGui_Action tbf_gui_app_bar_draw(const TbfGui_Style* style, const TbfGui_Debugger* debugger, bool dark, Rectangle rect)
{
    const TbfGui_Theme* theme = style->theme;
    const char* label = dark ? "Modo escuro" : "Modo claro";

    Rectangle content = tbf_gui_box(rect, tbf_gui_insets_xy(TBF_GUI_CARD_PADDING, 0));
    TbfGui_Stack trailing = tbf_gui_stack_begin(content, TBF_GUI_STACK_HORIZONTAL_REVERSE, 12);

    Rectangle toggle = tbf_gui_box_align(
        tbf_gui_stack_next(&trailing, TBF_GUI_APP_BAR_SWITCH_WIDTH),
        TBF_GUI_APP_BAR_SWITCH_WIDTH,
        TBF_GUI_APP_BAR_SWITCH_HEIGHT,
        TBF_GUI_ALIGN_START,
        TBF_GUI_ALIGN_CENTER
    );

    tbf_gui_label(style->fonts.body, label, tbf_gui_stack_next(&trailing, tbf_gui_text_size(style->fonts.body, label).x), TBF_GUI_ALIGN_START, theme->on_surface);
    tbf_gui_stack_skip(&trailing, 20);
    tbf_gui_app_bar_draw_info(style, debugger, &trailing);

    tbf_gui_label(style->fonts.title, "TinyBf", tbf_gui_stack_rest(&trailing), TBF_GUI_ALIGN_START, theme->on_surface);

    if (tbf_gui_switch(style, toggle, dark, TBF_GUI_ICON_DARK, TBF_GUI_ICON_LIGHT)) {
        return TBF_GUI_ACTION_TOGGLE_THEME;
    }

    return TBF_GUI_ACTION_NONE;
}
