#include <math.h>
#include <string.h>

#include "tbf/tbf-gui/ripple.h"

#define TBF_GUI_RIPPLE_EXPAND   0.40
#define TBF_GUI_RIPPLE_HOLD     0.15
#define TBF_GUI_RIPPLE_FADE     0.30
#define TBF_GUI_RIPPLE_OPACITY  0.16f

/* Desenha um círculo recortado por um retângulo arredondado (SDF), com bordas suaves. */
static const char* tbf_gui_ripple_fragment =
    "#version 330\n"
    "in vec2 fragTexCoord;\n"
    "in vec4 fragColor;\n"
    "out vec4 finalColor;\n"
    "uniform vec4 shape;\n"
    "uniform float corner;\n"
    "uniform vec2 center;\n"
    "uniform float radius;\n"
    "uniform vec4 color;\n"
    "uniform vec2 viewport;\n"
    "void main()\n"
    "{\n"
    "    vec2 p = vec2(gl_FragCoord.x, viewport.y - gl_FragCoord.y) / viewport.x;\n"
    "    vec2 half_size = shape.zw * 0.5;\n"
    "    vec2 q = abs(p - (shape.xy + half_size)) - half_size + corner;\n"
    "    float d = length(max(q, 0.0)) + min(max(q.x, q.y), 0.0) - corner;\n"
    "    float inside = clamp(0.5 - d, 0.0, 1.0);\n"
    "    float circle = clamp(radius - length(p - center) + 0.5, 0.0, 1.0);\n"
    "    finalColor = vec4(color.rgb, color.a * inside * circle);\n"
    "}\n";

static bool tbf_gui_ripple_same_key(Rectangle a, Rectangle b)
{
    return a.x == b.x && a.y == b.y && a.width == b.width && a.height == b.height;
}

static float tbf_gui_ripple_max_radius(TbfMath_Vector2 origin, Rectangle shape)
{
    float dx = fmaxf(origin.x - shape.x, shape.x + shape.width - origin.x);
    float dy = fmaxf(origin.y - shape.y, shape.y + shape.height - origin.y);

    return sqrtf(dx * dx + dy * dy);
}

static float tbf_gui_ripple_alpha(const TbfGui_Ripple* ripple, double now)
{
    if (ripple->released_at < 0) {
        return TBF_GUI_RIPPLE_OPACITY;
    }

    double fade_from = fmax(ripple->released_at, ripple->pressed_at + TBF_GUI_RIPPLE_HOLD);
    double t = (now - fade_from) / TBF_GUI_RIPPLE_FADE;

    return TBF_GUI_RIPPLE_OPACITY * (float) (1 - fmin(fmax(t, 0), 1));
}

void tbf_gui_ripples_init(TbfGui_Ripples* self)
{
    memset(self, 0, sizeof(*self));

    self->shader = LoadShaderFromMemory(NULL, tbf_gui_ripple_fragment);
    self->shader_loaded = IsShaderValid(self->shader);

    if (!self->shader_loaded) {
        return;
    }

    self->loc_shape = GetShaderLocation(self->shader, "shape");
    self->loc_corner = GetShaderLocation(self->shader, "corner");
    self->loc_center = GetShaderLocation(self->shader, "center");
    self->loc_radius = GetShaderLocation(self->shader, "radius");
    self->loc_color = GetShaderLocation(self->shader, "color");
    self->loc_viewport = GetShaderLocation(self->shader, "viewport");
}

void tbf_gui_ripples_free(TbfGui_Ripples* self)
{
    if (self->shader_loaded) {
        UnloadShader(self->shader);
    }

    self->shader_loaded = false;
}

void tbf_gui_ripples_update(TbfGui_Ripples* self)
{
    double now = GetTime();
    bool down = IsMouseButtonDown(MOUSE_BUTTON_LEFT);

    for (int i = 0; i < TBF_GUI_RIPPLE_CAPACITY; i++) {
        TbfGui_Ripple* ripple = &self->items[i];

        if (!ripple->active) {
            continue;
        }

        if (!down && ripple->released_at < 0) {
            ripple->released_at = now;
        }

        if (ripple->released_at >= 0 && tbf_gui_ripple_alpha(ripple, now) <= 0) {
            ripple->active = false;
        }
    }
}

void tbf_gui_ripples_press(TbfGui_Ripples* self, Rectangle key, TbfMath_Vector2 origin, bool centered, Color color)
{
    TbfGui_Ripple* ripple = &self->items[self->next];

    self->next = (self->next + 1) % TBF_GUI_RIPPLE_CAPACITY;

    ripple->active = true;
    ripple->centered = centered;
    ripple->key = key;
    ripple->origin = origin;
    ripple->color = color;
    ripple->pressed_at = GetTime();
    ripple->released_at = -1;
}

static void tbf_gui_ripple_draw(TbfGui_Ripples* self, const TbfGui_Ripple* ripple, Rectangle shape, float corner, double now)
{
    TbfMath_Vector2 origin = ripple->centered
        ? tbf_math_vector2(shape.x + shape.width / 2, shape.y + shape.height / 2)
        : ripple->origin;

    double t = fmin((now - ripple->pressed_at) / TBF_GUI_RIPPLE_EXPAND, 1);
    float ease = 1 - (float) pow(1 - t, 3);
    float radius = tbf_gui_ripple_max_radius(origin, shape) * (0.1f + 0.9f * ease);
    float alpha = tbf_gui_ripple_alpha(ripple, now);

    float shape_value[4] = { shape.x, shape.y, shape.width, shape.height };
    float center_value[2] = { origin.x, origin.y };
    float color_value[4] = { ripple->color.r / 255.0f, ripple->color.g / 255.0f, ripple->color.b / 255.0f, alpha };
    float viewport_value[2] = { (float) GetRenderWidth() / GetScreenWidth(), (float) GetRenderHeight() };

    BeginShaderMode(self->shader);
    SetShaderValue(self->shader, self->loc_shape, shape_value, SHADER_UNIFORM_VEC4);
    SetShaderValue(self->shader, self->loc_corner, &corner, SHADER_UNIFORM_FLOAT);
    SetShaderValue(self->shader, self->loc_center, center_value, SHADER_UNIFORM_VEC2);
    SetShaderValue(self->shader, self->loc_radius, &radius, SHADER_UNIFORM_FLOAT);
    SetShaderValue(self->shader, self->loc_color, color_value, SHADER_UNIFORM_VEC4);
    SetShaderValue(self->shader, self->loc_viewport, viewport_value, SHADER_UNIFORM_VEC2);
    DrawRectangleRec(shape, WHITE);
    EndShaderMode();
}

void tbf_gui_ripples_draw(TbfGui_Ripples* self, Rectangle key, Rectangle shape, float corner)
{
    if (!self->shader_loaded) {
        return;
    }

    double now = GetTime();

    for (int i = 0; i < TBF_GUI_RIPPLE_CAPACITY; i++) {
        const TbfGui_Ripple* ripple = &self->items[i];

        if (ripple->active && tbf_gui_ripple_same_key(ripple->key, key)) {
            tbf_gui_ripple_draw(self, ripple, shape, corner, now);
        }
    }
}
