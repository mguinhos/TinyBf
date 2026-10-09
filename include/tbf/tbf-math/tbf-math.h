#ifndef TBF_MATH_H
#define TBF_MATH_H

typedef struct TbfMath_Vector2 {
    float x;
    float y;
} TbfMath_Vector2;

TbfMath_Vector2 tbf_math_vector2(float x, float y);
TbfMath_Vector2 tbf_math_vector2_add(TbfMath_Vector2 a, TbfMath_Vector2 b);
TbfMath_Vector2 tbf_math_vector2_sub(TbfMath_Vector2 a, TbfMath_Vector2 b);
TbfMath_Vector2 tbf_math_vector2_scale(TbfMath_Vector2 v, float factor);

#endif
