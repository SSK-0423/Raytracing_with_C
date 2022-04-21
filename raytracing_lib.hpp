#include <memory.h>
#include <stdio.h>
#include "myPng.hpp"
#include "mymath.hpp"
#include "log.hpp"

// 使用しない
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
    ~IntersectionPoint(){
        // recordLine("IntersectionPointのデストラクタが呼ばれました\n");
    }
};

// float成分のカラー
struct FColor
{
    float r;
    float g;
    float b;

    FColor()
    {
    }
    FColor(float red, float green, float blue)
        : r(red), g(green), b(blue)
    {
    }
    FColor operator+(FColor color)
    {
        operationCount += 3;
        return FColor(r + color.r, g + color.g, b + color.b);
    }
    // 0〜1に正規化
    void normalize()
    {
        if (r < 0.f)
            r = 0.f;
        if (g < 0.f)
            g = 0.f;
        if (b < 0.f)
            b = 0.f;

        if (r > 1.f)
            r = 1.f;
        if (g > 1.f)
            g = 1.f;
        if (b > 1.f)
            b = 1.f;
    }
};

// マテリアル
struct Material
{
    FColor ambient;  // 環境光反射係数
    FColor diffuse;  // 拡散反射係数
    FColor specular; // 鏡面反射係数
    float shininess; // 光尺度
    Material(FColor a = FColor(0.01f, 0.01f, 0.01f),
             FColor d = FColor(0.69f, 0.69f, 0.69f),
             FColor s = FColor(0.30f, 0.30f, 0.30f), float shi = 8.f)
        : ambient(a), diffuse(d), specular(s), shininess(shi)
    {
    }
};

//
struct Shape
{
    // Rayとの交差判定
    virtual IntersectionPoint *isIntersectionRay(Ray *ray) = 0;
    // マテリアル
    Material material;
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

// フォンシェーディング
Color phongShading(
    IntersectionPoint intersectionPoint, Ray ray, PointLight pointLight);

// フォンシェーディング(マテリアル描画)
Color phongShading(
    IntersectionPoint intersectionPoint, Ray ray, PointLight pointLight, Material material);

// レイトレーシング
void RayTrace(BitMapData* bitmap,Shape* geometry, Camera* camera, PointLight* pointLight);
