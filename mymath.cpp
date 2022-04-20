#include "mymath.hpp"

float calcDiscriminant(float a, float b, float c)
{
    return b * b - 4 * a * c;
}

float calcQuadraticFormula(float a, float b, float c, SOLUTION solution)
{
    float ret = 0.f;
    if (solution == FIRST_SOLUTION)
        ret = (-b + sqrt(calcDiscriminant(a, b, c))) / (2.f * a);
    else if(solution == SECOND_SOLUTION)
        ret = (-b - sqrt(calcDiscriminant(a, b, c))) / (2.f * a);

    return ret;
}

// 乗算
Vector3 operator*(float n, Vector3 vec)
{
    return Vector3(n * vec.x, n * vec.y, n * vec.z);
}