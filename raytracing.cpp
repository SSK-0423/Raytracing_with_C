#include "raytracing.hpp"

// 点の描画
// 点の描画
void drawDot(
    BitMapData *bitmap, unsigned int x, unsigned int y, Color color)
{
    // 指定座標のピクセル取得
    unsigned char *pixel = bitmap->pixelsData +
                           y * bitmap->width * bitmap->channel +
                           x * bitmap->channel;
    // 色変更
    if (bitmap->channel == COLOR_RGB)
    {
        pixel[0] = color.r;
        pixel[1] = color.g;
        pixel[2] = color.b;
    }
    else if (bitmap->channel == COLOR_RGBA)
    {
        pixel[0] = color.r;
        pixel[1] = color.g;
        pixel[2] = color.b;
        pixel[3] = color.a;
    }
}

bool isIntersectingRay(Ray *ray, Sphere *sphere, Vector3 *intersectionPoint)
{
    // 判別式 d = b^2 - 4 * a * c

    // |d|^2
    float a = ray->direction.dot(ray->direction);
    // 2{d・(s - Pc)}
    float b = 2 * ray->direction.dot(ray->startPoint - sphere->center);
    // |s - Pc|^2 - r^2
    Vector3 tmp = ray->startPoint - sphere->center; // |s - Pc|^2
    float c = tmp.dot(tmp) - sphere->radius * sphere->radius;

    // 判別式計算
    float d = calcDiscriminant(a, b, c);

    // 交点なし
    if (d < 0)
    {
        return false;
    }
    // 1つ以上の交点
    else
    {
        float t1 = calcQuadraticFormula(a, b, c, FIRST_SOLUTION);
        float t2 = calcQuadraticFormula(a, b, c, SECOND_SOLUTION);
        if (t1 > 0 || t2 > 0)
        {
            // 交点は値が小さい方を採用して計算
            if (intersectionPoint != nullptr)
                *intersectionPoint = (t1 < t2) ? ray->startPoint + t1 * ray->direction : ray->startPoint + t2 * ray->direction;

            return true;
        }
        else
            return false;
    }
}

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
    ray.direction = screenToWorld(x, y, width, height) - ray.startPoint;

    return ray;
}

int main()
{
    // ビットマップデータ
    BitMapData bitmap(512, 512, 3);
    if (bitmap.allocation() == -1)
    {
        return -1;
    }

    // 球
    Sphere sphere;
    sphere.center = Vector3(0, 0, 5);
    sphere.radius = 1.f;

    // 視点の位置を決める
    Camera camera;
    camera.position = Vector3(0, 0, -5);

    // 視線方向で最も近い物体を探し，
    // その物体との交点位置とその点での法線ベクトルを求める
    for (int y = 0; y < bitmap.height; y++)
    {
        for (int x = 0; x < bitmap.width; x++)
        {
            // レイを生成
            Ray ray = createRay(camera, x, y, bitmap.width, bitmap.height);

            // レイと球が交差するか判定+交点があれば計算
            if (isIntersectingRay(&ray, &sphere, nullptr))
            {
                drawDot(&bitmap, x, y, Color(0xff, 0x00, 0x00));
            }
            else
                drawDot(&bitmap, x, y, Color(0x00, 0xff, 0x8f));
        }
    }

    // PNGに変換してファイル保存
    if (pngFileEncodeWrite(&bitmap, "raytracing.png") == -1)
    {
        freeBitmapData(&bitmap);
        return -1;
    }

    freeBitmapData(&bitmap);

    return 0;
}