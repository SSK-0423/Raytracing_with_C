#include <memory.h>
#include <stdio.h>
#include "myPng.hpp"
#include "mymath.hpp"

// レイ
struct Ray
{
    Vector3 startPoint; // レイの始点
    Vector3 direction;  // 方向ベクトル
};

// 交点
struct IntersectionPoint
{
    Vector3 position; // 交点の位置
    Vector3 normal;   // 交点における法線
};

// 球
struct Sphere
{
    Vector3 center; // 中心座標
    float radius;   // 半径
    bool isIntersectionRay(Ray *ray, IntersectionPoint *intersectionPoint);
};

// 平面
struct Plane
{
    Vector3 normal;   // 法線
    Vector3 position; // 平面が通る点
    bool isIntersectionRay(Ray *ray, IntersectionPoint *intersectionPoint);
};

// カメラ
struct Camera
{
    Vector3 position; // 視点
};

// 点光源
struct PointLight
{
    Vector3 position; // 光源位置
};

// 視点からスクリーン座標へのRayを生成
Ray createRay(Camera camera, float x, float y, float width, float height);

// スクリーン座標からワールド座標へ変換
Vector3 screenToWorld(float x, float y, unsigned int width, unsigned int height);
