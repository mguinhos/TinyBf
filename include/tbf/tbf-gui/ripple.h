#ifndef TBF_GUI_RIPPLE_H
#define TBF_GUI_RIPPLE_H

#include <stdbool.h>

#include "raylib.h"

#include "tbf/tbf-math/tbf-math.h"

#define TBF_GUI_RIPPLE_CAPACITY 16

typedef struct TbfGui_Ripple {
    bool active;
    bool centered;
    Rectangle key;
    TbfMath_Vector2 origin;
    Color color;
    double pressed_at;
    double released_at;
} TbfGui_Ripple;

typedef struct TbfGui_Ripples {
    TbfGui_Ripple items[TBF_GUI_RIPPLE_CAPACITY];
    int next;

    Shader shader;
    bool shader_loaded;
    int loc_shape;
    int loc_corner;
    int loc_center;
    int loc_radius;
    int loc_color;
    int loc_viewport;
} TbfGui_Ripples;

void tbf_gui_ripples_init(TbfGui_Ripples* self);
void tbf_gui_ripples_free(TbfGui_Ripples* self);
void tbf_gui_ripples_update(TbfGui_Ripples* self);
void tbf_gui_ripples_press(TbfGui_Ripples* self, Rectangle key, TbfMath_Vector2 origin, bool centered, Color color);
void tbf_gui_ripples_draw(TbfGui_Ripples* self, Rectangle key, Rectangle shape, float corner);

#endif
