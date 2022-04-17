#include <memory.h>
#include <stdio.h>
#include "myPng.hpp"

// 3次元ベクトル
struct Vector3
{
    float x, y, z;

    Vector3() {}
    Vector3(float x, float y, float z)
        : x(x), y(y), z(z) {}

    Vector3 operator -(Vector3 vec){
        Vector3 ret(vec.x - x, vec.y - y, vec.z - z);
        return ret;
    }
};

// 球
struct Sphere
{
    Vector3 center; // 中心座標
    float radius;   // 半径
};

// レイ
struct Ray
{
    Vector3 startPoint; // レイの始点
    Vector3 direction;  // 方向ベクトル
};

// カメラ
struct Camera
{
    Vector3 position; // 視点
};
