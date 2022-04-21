#include <memory.h>
#include <stdio.h>
#include "myPng.hpp"
#include "mymath.hpp"

#define ZBUFFER_MAX 1
#define ZBUFFER_MIN 0
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

//
struct Shape
{
    virtual IntersectionPoint *isIntersectionRay(Ray *ray) = 0;
};

// 球
struct Sphere : public Shape
{
    Sphere(Vector3 c, float r) : center(c), radius(r) {}
    Sphere() {}
    ~Sphere() {}
    Vector3 center; // 中心座標
    float radius;   // 半径
    IntersectionPoint *isIntersectionRay(Ray *ray) override;
};

// 平面
struct Plane : public Shape
{
    Plane(Vector3 n, Vector3 p) : normal(n), position(p) {}
    ~Plane() {}
    Vector3 normal;   // 法線
    Vector3 position; // 平面が通る点
    IntersectionPoint *isIntersectionRay(Ray *ray) override;
    // 法線計算
    static Vector3 calcNormal(Vector3 p1, Vector3 p2, Vector3 p3);
};

// カメラ
struct Camera
{
    Vector3 position; // 視点
    float far;        // 最遠距離
    float near;       // 最近距離
};

// 点光源
struct PointLight
{
    Vector3 position; // 光源位置
};

// Zバッファ
struct ZBuffer
{
    unsigned char *zbuff;
    unsigned int width;
    unsigned int height;
    ZBuffer(unsigned int w, unsigned int h) : width(w), height(h)
    {
        // バッファ確保
        zbuff = (unsigned char *)malloc(sizeof(unsigned char) * w * h);
        // 1で初期化
        memset((void *)zbuff, ZBUFFER_MAX, sizeof(unsigned char) * w * h);
    }
};

// Zバッファを考慮した描画
void drawDotWithZBuffer(
    BitMapData *bitmap, unsigned int x, unsigned int y, Color color,
    ZBuffer *zbuffer, unsigned char z);

// 視点からスクリーン座標へのRayを生成
Ray createRay(Camera camera, float x, float y, float width, float height);

// スクリーン座標からワールド座標へ変換
Vector3 screenToWorld(float x, float y, unsigned int width, unsigned int height);
