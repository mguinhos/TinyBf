#include <math.h>
#include <string.h>

#include "tbf/tbf-gui/skeleton.h"

#define TBF_GUI_SKELETON_PERIOD 1.6
#define TBF_GUI_SKELETON_MARGIN 400.0f

/* Retângulo arredondado (SDF) com uma faixa de brilho diagonal que atravessa a janela. */
static const char* tbf_gui_skeleton_fragment =
    "#version 330\n"
    "in vec2 fragTexCoord;\n"
    "in vec4 fragColor;\n"
    "out vec4 finalColor;\n"
    "uniform vec4 shape;\n"
    "uniform float corner;\n"
    "uniform vec4 base;\n"
    "uniform vec4 highlight;\n"
    "uniform float shimmer;\n"
    "uniform vec2 viewport;\n"
    "void main()\n"
    "{\n"
    "    vec2 p = vec2(gl_FragCoord.x, viewport.y - gl_FragCoord.y) / viewport.x;\n"
    "    vec2 half_size = shape.zw * 0.5;\n"
    "    vec2 q = abs(p - (shape.xy + half_size)) - half_size + corner;\n"
    "    float d = length(max(q, 0.0)) + min(max(q.x, q.y), 0.0) - corner;\n"
    "    float inside = clamp(0.5 - d, 0.0, 1.0);\n"
    "    float band = (p.x + p.y * 0.4 - shimmer) / 160.0;\n"
    "    vec4 color = mix(base, highlight, exp(-band * band));\n"
    "    finalColor = vec4(color.rgb, color.a * inside);\n"
    "}\n";

void tbf_gui_skeleton_init(TbfGui_Skeleton* self)
{
    memset(self, 0, sizeof(*self));

    self->shader = LoadShaderFromMemory(NULL, tbf_gui_skeleton_fragment);
    self->shader_loaded = IsShaderValid(self->shader);

    if (!self->shader_loaded) {
        return;
    }

    self->loc_shape = GetShaderLocation(self->shader, "shape");
    self->loc_corner = GetShaderLocation(self->shader, "corner");
    self->loc_base = GetShaderLocation(self->shader, "base");
    self->loc_highlight = GetShaderLocation(self->shader, "highlight");
    self->loc_shimmer = GetShaderLocation(self->shader, "shimmer");
    self->loc_viewport = GetShaderLocation(self->shader, "viewport");
}

void tbf_gui_skeleton_free(TbfGui_Skeleton* self)
{
    if (self->shader_loaded) {
        UnloadShader(self->shader);
    }

    self->shader_loaded = false;
}

static void tbf_gui_skeleton_color(Color color, float value[4])
{
    value[0] = color.r / 255.0f;
    value[1] = color.g / 255.0f;
    value[2] = color.b / 255.0f;
    value[3] = color.a / 255.0f;
}

void tbf_gui_skeleton_draw(const TbfGui_Skeleton* self, Rectangle rect, float radius, Color base, Color highlight)
{
    double phase = fmod(GetTime(), TBF_GUI_SKELETON_PERIOD) / TBF_GUI_SKELETON_PERIOD;

    if (!self->shader_loaded) {
        float pulse = 0.5f + 0.5f * (float) sin(phase * 2 * PI);
        float roundness = fminf(rect.width, rect.height) > 0 ? fminf(1.0f, 2 * radius / fminf(rect.width, rect.height)) : 0;

        DrawRectangleRounded(rect, roundness, 12, ColorLerp(base, highlight, pulse));
        return;
    }

    float shimmer = (float) (phase * (GetScreenWidth() + 2 * TBF_GUI_SKELETON_MARGIN)) - TBF_GUI_SKELETON_MARGIN;
    float shape_value[4] = { rect.x, rect.y, rect.width, rect.height };
    float base_value[4];
    float highlight_value[4];
    float viewport_value[2] = { (float) GetRenderWidth() / GetScreenWidth(), (float) GetRenderHeight() };

    tbf_gui_skeleton_color(base, base_value);
    tbf_gui_skeleton_color(highlight, highlight_value);

    BeginShaderMode(self->shader);
    SetShaderValue(self->shader, self->loc_shape, shape_value, SHADER_UNIFORM_VEC4);
    SetShaderValue(self->shader, self->loc_corner, &radius, SHADER_UNIFORM_FLOAT);
    SetShaderValue(self->shader, self->loc_base, base_value, SHADER_UNIFORM_VEC4);
    SetShaderValue(self->shader, self->loc_highlight, highlight_value, SHADER_UNIFORM_VEC4);
    SetShaderValue(self->shader, self->loc_shimmer, &shimmer, SHADER_UNIFORM_FLOAT);
    SetShaderValue(self->shader, self->loc_viewport, viewport_value, SHADER_UNIFORM_VEC2);
    DrawRectangleRec(rect, WHITE);
    EndShaderMode();
}
