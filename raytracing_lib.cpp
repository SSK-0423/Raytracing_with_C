#include "raytracing_lib.hpp"

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
    // 分母0は交点なし
    if (dn == 0)
        return nullptr;

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

// 物体表面の光源の性質を使ってその点での色を決定する(シェーディング)
Color phongShading(
    IntersectionPoint intersectionPoint, Ray ray, PointLight pointLight)
{
    // 法線ベクトル
    Vector3 normal = intersectionPoint.normal;

    // 入射ベクトル計算(光が当たる点からみた光源の位置であることに注意)
    Vector3 incident =
        (pointLight.position - intersectionPoint.position).normalize();

    // 正反射ベクトル計算 r = 2(n・l)n - l
    Vector3 specularReflection = 2 * normal.dot(incident) * normal - incident;

    // 光源強度
    float Ii = 1.f;

    // ディフューズ(拡散反射光)
    float kd = 0.69; // 拡散反射係数
    float diffuse = Ii * kd * normal.dot(incident);
    if (diffuse < 0)
        diffuse = 0;

    // スペキュラー
    float ks = 0.3; // 鏡面反射係数
    float a = 8;    // 光尺度

    // 鏡面反射係数 * 光源強度 * 視線逆ベクトル・入射光の正反射ベクトル
    Vector3 inverseEyeDir = ((-1) * ray.direction).normalize();
    float specular = ks * Ii * myPow(inverseEyeDir.dot(specularReflection), a);
    // 視線逆ベクトルと正反射ベクトルの内積もしくは，
    // 物体面の法線ベクトルと入射ベクトルの内積が負数の場合，
    // 鏡面反射は「0」になる
    if (inverseEyeDir.dot(specularReflection) < 0 || normal.dot(incident) < 0)
        specular = 0;

    // 環境光
    float Ia = 0.1f;  // 環境光の強度
    float ka = 0.01f; // 環境光反射係数
    float ambient = Ia * ka;

    float I = diffuse + specular + ambient;

    Color color;
    color.r = I * 0x00;
    color.g = I * 0xff;
    color.b = I * 0x80;

    return color;
}

Color phongShading(
    IntersectionPoint intersectionPoint, Ray ray, PointLight pointLight, Material material)
{
    // 法線ベクトル
    Vector3 normal = intersectionPoint.normal;

    // 入射ベクトル計算(光が当たる点からみた光源の位置であることに注意)
    Vector3 incident =
        (pointLight.position - intersectionPoint.position).normalize();

    // 正反射ベクトル計算 r = 2(n・l)n - l
    Vector3 specularReflection = 2 * normal.dot(incident) * normal - incident;

    // 光源強度
    float Ii = 1.f;

    // ディフューズ(拡散反射光)
    FColor diffuse;
    diffuse.r = Ii * material.diffuse.r * normal.dot(incident);
    diffuse.g = Ii * material.diffuse.g * normal.dot(incident);
    diffuse.b = Ii * material.diffuse.b * normal.dot(incident);

    // スペキュラー
    // 鏡面反射係数 * 光源強度 * 視線逆ベクトル・入射光の正反射ベクトル
    Vector3 inverseEyeDir = ((-1) * ray.direction).normalize();
    // (cos)^aを計算
    float cos_a = myPow(inverseEyeDir.dot(specularReflection), material.shininess);

    FColor specular;
    specular.r = Ii * material.specular.r * cos_a;
    specular.g = Ii * material.specular.g * cos_a;
    specular.b = Ii * material.specular.b * cos_a;

    // 視線逆ベクトルと正反射ベクトルの内積もしくは，
    // 物体面の法線ベクトルと入射ベクトルの内積が負数の場合，
    // 鏡面反射は「0」になる
    if (inverseEyeDir.dot(specularReflection) < 0 || normal.dot(incident) < 0)
        specular = FColor(0.f, 0.f, 0.f);

    // 環境光
    float Ia = 0.1f; // 環境光の強度
    FColor ambient;
    ambient.r = material.ambient.r * Ia;
    ambient.g = material.ambient.g * Ia;
    ambient.b = material.ambient.b * Ia;

    FColor I = diffuse + specular + ambient;
    I.normalize(); // 必ず実行する

    Color color;
    color.r = I.r * 0xff;
    color.g = I.g * 0xff;
    color.b = I.b * 0xff;

    return color;
}

IntersectionPoint *intersectionWithAll(Shape *geometry, Ray *ray)
{
    float minDistance = 10e6;                       // レイの視点との最小距離
    IntersectionPoint *intersectionPoint = nullptr; // レイの始点との最近交点
    size_t drawIdx = 0;                             // 描画するオブジェクトのインデックス

    // 全オブジェクトの交点を調べ，レイの視点に最も近い交点を決定する
    for (size_t idx = 0; idx < GEOMETRY_NUM; idx++)
    {
        // レイと球が交差するか判定+交点があれば計算
        IntersectionPoint *point = geometry[idx].isIntersectionRay(ray);

        // 交点ないならスキップ
        if (point == nullptr)
            continue;

        // レイの始点から交点への距離計算
        float distance = (point->position - ray->startPoint).magnitude();

        // 最小距離なら描画点に指定
        if (distance < minDistance)
        {
            // 最小距離更新
            minDistance = distance;

            // 描画対象オブジェクトのインデックス更新
            drawIdx = idx;

            // 先に交点が代入されていたらメモリ解放する
            if (intersectionPoint != nullptr)
                delete intersectionPoint;
            // 交点のメモリを確保してpointの中身をコピーする
            intersectionPoint = new IntersectionPoint();
            memmove(intersectionPoint, point, sizeof(IntersectionPoint));
        }
    }

    return intersectionPoint;
}

void RayTrace(BitMapData *bitmap, Shape *geometry, Camera *camera, PointLight *pointLight)
{
}