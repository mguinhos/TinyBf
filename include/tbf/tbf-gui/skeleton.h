#ifndef TBF_GUI_SKELETON_H
#define TBF_GUI_SKELETON_H

#include <stdbool.h>

#include "raylib.h"

typedef struct TbfGui_Skeleton {
    Shader shader;
    bool shader_loaded;
    int loc_shape;
    int loc_corner;
    int loc_base;
    int loc_highlight;
    int loc_shimmer;
    int loc_viewport;
} TbfGui_Skeleton;

void tbf_gui_skeleton_init(TbfGui_Skeleton* self);
void tbf_gui_skeleton_free(TbfGui_Skeleton* self);
void tbf_gui_skeleton_draw(const TbfGui_Skeleton* self, Rectangle rect, float radius, Color base, Color highlight);

#endif
