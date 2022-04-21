#include "raytracing_lib.hpp"
#include "log.hpp"

// スクリーン座標からワールド座標へ変換
Vector3 screenToWorld(
    float x, float y, unsigned int width, unsigned int height)
{
    float lx = 2 * x / (width - 1) - 1.f;
    float ly = -2 * y / (height - 1) + 1.f;

    return Vector3(lx, ly, 0);
}

// 視点からスクリーン座標へのRayを生成
Ray createRay(Camera camera, float x, float y, float width, float height)
{
    Ray ray;
    ray.startPoint = camera.position;
    // 視点位置から点(x,y)に向かう半直線
    ray.direction = screenToWorld(x, y, width, height) - ray.startPoint;

    return ray;
}

void drawDotWithZBuffer(
    BitMapData *bitmap, unsigned int x, unsigned int y, Color color,
    ZBuffer *zbuffer, unsigned char z)
{
    unsigned char *zbuf = zbuffer->zbuff + y * zbuffer->width + x;

    // Zバッファの最大値より大きいものはZバッファの最大値とする
    if (z > ZBUFFER_MAX)
    {
        z = ZBUFFER_MAX;
    }
    // 既存のZバッファより小さければ描画
    if (*zbuf >= z)
    {
        drawDot(bitmap, x, y, color); // 描画
        *zbuf = z;                    // Zバッファ更新
    }
}

IntersectionPoint *Sphere::isIntersectionRay(Ray *ray)
{
    // 判別式 d = b^2 - 4 * a * c

    // |d|^2
    float a = ray->direction.dot(ray->direction);
    // 2{d・(s - Pc)}
    float b = 2 * ray->direction.dot(ray->startPoint - center);
    // |s - Pc|^2 - r^2
    Vector3 tmp = ray->startPoint - center; // |s - Pc|^2
    float c = tmp.dot(tmp) - myPow(radius, 2);

    recordLine("center = (%4.2f,%4.2f,%4.2f)\n", center.x, center.y, center.z);

    // 判別式計算
    float d = calcDiscriminant(a, b, c);

    // 交点なし
    if (d < 0)
    {
        return nullptr;
    }
    // 1つ以上の交点
    else
    {
        float t1 = calcQuadraticFormula(a, b, c, FIRST_SOLUTION);
        float t2 = calcQuadraticFormula(a, b, c, SECOND_SOLUTION);
        // recordLine("t1:%lf, t2:%lf\n", t1, t2);
        if (t1 > 0 || t2 > 0)
        {
            // 交点は値が小さい方を採用
            IntersectionPoint *point = new IntersectionPoint();
            point->position =
                (t1 < t2) ? ray->startPoint + t1 * ray->direction
                          : ray->startPoint + t2 * ray->direction;
            point->normal = (point->position - center).normalize();
            return point;
        }
        else
        {
            return nullptr;
        }
    }
}

IntersectionPoint *Plane::isIntersectionRay(Ray *ray)
{
    float dn = ray->direction.dot(normal);
    // recordLine("(d・n) = %f\n",dn);
    // 分母0は交点なし
    if (dn == 0)
        return nullptr;

    // recordLine("s = (%4.2f,%4.2f,%4.2f)\n",
    //            ray->startPoint.x, ray->startPoint.y, ray->startPoint.z);
    // recordLine("n = (%4.2f,%4.2f,%4.2f)\n", normal.x, normal.y, normal.z);
    // recordLine("(s・n) = %f\n", ray->startPoint.dot(normal));
    float t = (position - ray->startPoint).dot(normal) / dn;
    // 交点あり
    if (t > 0)
    {
        IntersectionPoint *point = new IntersectionPoint();
        point->position = ray->startPoint + t * ray->direction;
        point->normal = normal;
        return point;
    }
    // 交点なし
    else
    {
        return nullptr;
    }
}

Vector3 Plane::calcNormal(Vector3 p1, Vector3 p2, Vector3 p3)
{
    Vector3 ab = p2 - p1;
    Vector3 ac = p3 - p1;
    return ab.cross(ac).normalize();
}