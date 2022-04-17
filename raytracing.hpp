#include <memory.h>
#include <stdio.h>
#include "myPng.hpp"
#include "mymath.hpp"

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
