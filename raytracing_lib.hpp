#include <memory.h>
#include <stdio.h>
#include <float.h>
#include "myPng.hpp"
#include "mymath.hpp"
#include "log.hpp"

// 使用しない
#define ZBUFFER_MAX 1
#define ZBUFFER_MIN 0

#define MAX_RECURSIVE_LEVEL 2
static float EPSILON = 1.f / 512.f;

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
    ~IntersectionPoint()
    {
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

    bool useReflection; // 完全鏡面反射を使うかどうか
    FColor reflection;  // 完全鏡面反射係数
    bool useRefraction; //
    float refractionIndex;
    Material(FColor a = FColor(0.01f, 0.01f, 0.01f),
             FColor d = FColor(0.69f, 0.69f, 0.69f),
             FColor s = FColor(0.30f, 0.30f, 0.30f), float shi = 8.f,
             FColor f = FColor(0, 0, 0), bool uf = false,
             bool ur = false, float index = 1.f)
        : ambient(a), diffuse(d), specular(s), shininess(shi), reflection(f), useReflection(uf),
          useRefraction(ur), refractionIndex(index)
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

    virtual ~Shape() {}
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

struct Lighting
{
    float distance;
    FColor intensity;
    Vector3 direction;
};

struct Light
{
    virtual Lighting lightingAt(Vector3 p) = 0;
};

// 点光源
struct PointLight : Light
{
    Vector3 position; // 光源位置
    FColor intensity; // 光源強度
    PointLight() {}
    PointLight(Vector3 p, FColor i)
        : position(p), intensity(i)
    {
    }

    Lighting lightingAt(Vector3 p) override
    {
        Lighting lighting;
        lighting.distance = (position - p).magnitude();
        lighting.direction = (position - p).normalize();
        lighting.intensity = intensity;

        return lighting;
    }
};

// 平行光源
struct DirectionalLight : Light
{
    Vector3 direction; // 平行光源の方向
    FColor intensity;  // 光源強度
    DirectionalLight() {}
    DirectionalLight(Vector3 d, FColor i)
        : direction(d), intensity(i)
    {
    }

    Lighting lightingAt(Vector3 p) override
    {
        Lighting lighting;
        lighting.distance = FLT_MAX;
        lighting.direction = ((-1) * direction).normalize();
        lighting.intensity = intensity;

        return lighting;
    }
};

struct Scene
{
    BitMapData *bitmap;
    Shape **geometry;
    int geometryNum;
    Light **light;
    Camera *camera;
    PointLight *pointLight;
    int lightNum;
    FColor backgroundColor;
    float globalRefractionIndex;
    Scene()
    {
        globalRefractionIndex = 1.000293;
    }
};

// 視点からスクリーン座標へのRayを生成
Ray createRay(Camera camera, float x, float y, float width, float height);

// スクリーン座標からワールド座標へ変換
Vector3 screenToWorld(float x, float y, unsigned int width, unsigned int height);

// フォンシェーディング
Color phongShading(
    IntersectionPoint intersectionPoint, Ray ray, PointLight pointLight);

// フォンシェーディング(バグってます)
Color phongShading(
    IntersectionPoint intersectionPoint, Ray ray, Lighting lighting, Material material);

// フォンシェーディング(マテリアル描画)
FColor phongShading(
    IntersectionPoint intersectionPoint, Ray ray, PointLight pointLight, Material material);

// 交差判定の結果
struct IntersectionResult
{
    IntersectionPoint *intersectionPoint = nullptr;
    Shape *shape = nullptr;
    ~IntersectionResult()
    {
        if (intersectionPoint != nullptr)
            delete intersectionPoint;
        intersectionPoint = nullptr;

        if (shape != nullptr)
            delete shape;
        shape = nullptr;
    }
};

// すべてのオブジェクトと交差判定
IntersectionResult *intersectionWithAll(Shape **geometry, int geometryNum, Ray *ray);

// すべてのオブジェクトと交差判定（シャドウレイ用）
IntersectionResult *intersectionWithAll(
    Shape **geometry, int geometryNum, Ray *ray, float maxDistance, bool exitOnceFound);

// レイトレーシング
FColor RayTrace(Scene *scene, Ray *ray);

// レイトレーシングの再帰呼び出し
FColor RayTraceRecursive(Scene *scene, Ray *ray, unsigned int recursiveLevel);

// 影生成
void shadowing(
    Scene *scene, Ray *ray, IntersectionResult *intersectionResult, FColor *luminance);

// 鏡面反射計算
void reflection(
    Scene *scene, Ray *ray,
    IntersectionResult *intersectionResult, FColor *luminance, unsigned int recursiveLevel);

// 屈折計算
void refraction(
    Scene *scene, Ray *ray,
    IntersectionResult *intersectionResult, FColor *luminance, unsigned int recursiveLevel);