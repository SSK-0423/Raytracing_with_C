#include "mymath.hpp"
#include <math.h>

float calcDiscriminant(float a, float b, float c)
{
    return b * b - 4 * a * c;
}

float calcQuadraticFormula(float a, float b, float c, SOLUTION solution)
{
    float ret = 0.f;
    if (solution == FIRST_SOLUTION)
        ret = (-b + sqrt(calcDiscriminant(a, b, c))) / 2 * a;
    else if(solution == SECOND_SOLUTION)
        ret = (-b - sqrt(calcDiscriminant(a, b, c))) / 2 * a;

    return ret;
}