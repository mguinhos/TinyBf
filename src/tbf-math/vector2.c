#include "tbf/tbf-math/tbf-math.h"

TbfMath_Vector2 tbf_math_vector2(float x, float y)
{
    return (TbfMath_Vector2) { x, y };
}

TbfMath_Vector2 tbf_math_vector2_add(TbfMath_Vector2 a, TbfMath_Vector2 b)
{
    return (TbfMath_Vector2) { a.x + b.x, a.y + b.y };
}

TbfMath_Vector2 tbf_math_vector2_sub(TbfMath_Vector2 a, TbfMath_Vector2 b)
{
    return (TbfMath_Vector2) { a.x - b.x, a.y - b.y };
}

TbfMath_Vector2 tbf_math_vector2_scale(TbfMath_Vector2 v, float factor)
{
    return (TbfMath_Vector2) { v.x * factor, v.y * factor };
}
