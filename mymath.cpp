#include "mymath.hpp"

float calcDiscriminant(float a, float b, float c)
{
    return b * b - 4 * a * c;
}

float calcQuadraticFormula(float a, float b, float c, SOLUTION solution)
{
    float ret = 0.f;
    if (solution == FIRST_SOLUTION)
        ret = (-b + mySqrt(calcDiscriminant(a, b, c))) / (2.f * a);
    else if (solution == SECOND_SOLUTION)
        ret = (-b - mySqrt(calcDiscriminant(a, b, c))) / (2.f * a);

    return ret;
}

float mySqrt(float n)
{
    float xn, xo;
    xn = 100.f;

    // ニュートン法
    while (myAbsf(xn - xo) > 1.0e-5)
    {
        xo = xn;
        xn = (xo + n / xo) / 2;
        operationCount += 4;
    }
    return xn;
}

float myPow(float n, int a)
{
    float ret = 1;
    for (int i = 0; i < a; i++)
        ret *= n;

    operationCount += a;
    return ret;
}

float myAbsf(float n)
{
    if (n >= 0)
    {
        operationCount++;
        return n;
    }
    else
    {
        operationCount += 2;
        return -n;
    }
}

float myRand()
{
    static int x = 10;
    int a = 1103515245;
    int b = 12345;
    int m = __INT_MAX__;
    x = (a * x + b) & m;

    float ret = ((float)x + 1.f) / ((float)m + 2.f);
    return ret / (float)m;
}
// 乗算
Vector3 operator*(float n, Vector3 vec)
{
    operationCount += 3;
    return Vector3(n * vec.x, n * vec.y, n * vec.z);
}