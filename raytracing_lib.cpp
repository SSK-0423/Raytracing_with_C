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
    color.r = I * 0xff;
    color.g = I * 0xff;
    color.b = I * 0xff;

    return color;
}

FColor phongShading(
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
    diffuse.r = pointLight.intensity.r * material.diffuse.r * normal.dot(incident);
    diffuse.g = pointLight.intensity.g * material.diffuse.g * normal.dot(incident);
    diffuse.b = pointLight.intensity.b * material.diffuse.b * normal.dot(incident);

    // スペキュラー
    // 鏡面反射係数 * 光源強度 * 視線逆ベクトル・入射光の正反射ベクトル
    Vector3 inverseEyeDir = ((-1) * ray.direction).normalize();
    // (cos)^aを計算
    float cos_a = myPow(inverseEyeDir.dot(specularReflection), material.shininess);

    FColor specular;
    specular.r = pointLight.intensity.r * material.specular.r * cos_a;
    specular.g = pointLight.intensity.g * material.specular.g * cos_a;
    specular.b = pointLight.intensity.b * material.specular.b * cos_a;

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

    return I;
}

FColor phongShading(
    IntersectionPoint intersectionPoint, Ray ray, Lighting lighting, Material material)
{
    // 法線ベクトル
    Vector3 normal = intersectionPoint.normal;

    // 入射ベクトル計算(光が当たる点からみた光源の位置であることに注意)
    Vector3 incident = lighting.direction;

    // 正反射ベクトル計算 r = 2(n・l)n - l
    Vector3 specularReflection = 2.f * normal.dot(incident) * normal - incident;

    // ディフューズ(拡散反射光)
    FColor diffuse;
    diffuse.r = lighting.intensity.r * material.diffuse.r * normal.dot(incident);
    diffuse.g = lighting.intensity.g * material.diffuse.g * normal.dot(incident);
    diffuse.b = lighting.intensity.b * material.diffuse.b * normal.dot(incident);
    diffuse.normalize();
    // recordLine("diffuse.r = %f\n", diffuse.r);
    // recordLine("diffuse.g = %f\n", diffuse.g);
    // recordLine("diffuse.b = %f\n", diffuse.b);

    // スペキュラー
    // 鏡面反射係数 * 光源強度 * 視線逆ベクトル・入射光の正反射ベクトル
    Vector3 inverseEyeDir = ((-1.f) * ray.direction).normalize();
    // (cos)^aを計算
    float cos_a = myPow(inverseEyeDir.dot(specularReflection), material.shininess);

    FColor specular;
    specular.r = lighting.intensity.r * material.specular.r * cos_a;
    specular.g = lighting.intensity.g * material.specular.g * cos_a;
    specular.b = lighting.intensity.b * material.specular.b * cos_a;
    specular.normalize();
    // recordLine("specular.r = %f\n", specular.r);
    // recordLine("specular.g = %f\n", specular.g);
    // recordLine("specular.b = %f\n", specular.b);

    // 視線逆ベクトルと正反射ベクトルの内積もしくは，
    // 物体面の法線ベクトルと入射ベクトルの内積が負数の場合，
    // 鏡面反射は「0」になる
    if (inverseEyeDir.dot(specularReflection) < 0 || normal.dot(incident) < 0)
        specular = FColor(0.f, 0.f, 0.f);

    FColor I = diffuse + specular;
    I.normalize(); // 必ず実行する

    return I;
}

// 配列の先頭の要素を指すのが配列名
// 先頭の要素（ポインタ）の位置を指しているのでダブルポインタ
IntersectionResult *intersectionWithAll(Shape **geometry, int geometryNum, Ray *ray)
{
    return intersectionWithAll(geometry, geometryNum, ray, FLT_MAX, false);
}

IntersectionResult *intersectionWithAll(
    Shape **geometry, int geometryNum, Ray *ray, float maxDistance, bool exitOnceFound)
{
    IntersectionResult *result = new IntersectionResult();

    float minDistance = FLT_MAX; // レイの始点との最小距離

    // 全オブジェクトの交点を調べ，レイの始点に最も近い交点を決定する
    for (size_t idx = 0; idx < geometryNum; idx++)
    {
        // 1回交点がみつかったら処理を中止する場合
        if (exitOnceFound && result->intersectionPoint != nullptr)
            break;

        // レイと球が交差するか判定+交点があれば計算
        IntersectionPoint *point = geometry[idx]->isIntersectionRay(ray);

        // 交点ないならスキップ
        if (point == nullptr)
            continue;

        // レイの始点から交点への距離計算
        float distance = (point->position - ray->startPoint).magnitude();

        // 交差とみなす最大距離を超えた場合はスキップ
        if (distance > maxDistance)
            continue;

        // 最小距離なら描画点に指定
        if (distance < minDistance)
        {
            // 最小距離更新
            minDistance = distance;

            // 描画対象オブジェクトを更新
            result->shape = geometry[idx];

            // 先に交点が代入されていたらメモリ解放する
            if (result->intersectionPoint != nullptr)
                delete result->intersectionPoint;

            // 交点のメモリを確保してpointの中身をコピーする
            result->intersectionPoint = new IntersectionPoint();
            memmove(result->intersectionPoint, point, sizeof(IntersectionPoint));
        }
    }

    return result;
}

FColor RayTrace(Scene *scene, Ray *ray)
{
    return RayTraceRecursive(scene, ray, 1);
}

FColor RayTraceRecursive(Scene *scene, Ray *ray, unsigned int recursiveLevel)
{
    // 再起回数の上限に達していたら
    if (recursiveLevel > MAX_RECURSIVE_LEVEL)
    {
        return FColor(FLT_MAX, FLT_MAX, FLT_MAX);
    }

    // 再帰回数の上限以内なら
    else
    {
        // 全物体との交差判定
        IntersectionResult *intersectionResult =
            intersectionWithAll(scene->geometry, scene->geometryNum, ray);

        if (intersectionResult->intersectionPoint == nullptr)
            return scene->backgroundColor;

        // 輝度値
        FColor luminance = FColor(0, 0, 0);

        bool useReflection = intersectionResult->shape->material.useReflection;
        bool useRefraction = intersectionResult->shape->material.useRefraction;

        // シャドウイング
        if (intersectionResult->intersectionPoint != nullptr)
        {
            // 影(0,0,0) or フォンシェーディング
            shadowing(scene, ray, intersectionResult, &luminance);
        }

        // 鏡面反射
        if (useReflection)
        {
            reflection(scene, ray, intersectionResult, &luminance, recursiveLevel);
        }

        // 光の屈折
        if (useRefraction)
        {
            refraction(scene, ray, intersectionResult, &luminance, recursiveLevel);
        }

        return luminance;
    }
}

void shadowing(
    Scene *scene, Ray *ray, IntersectionResult *intersectionResult, FColor *luminance)
{
    // シャドウレイによる交差判定
    IntersectionPoint *intersectionPoint = intersectionResult->intersectionPoint;

    for (size_t idx = 0; idx < scene->lightNum; idx++)
    {
        Lighting lighting = scene->light[idx]->lightingAt(intersectionPoint->position);

        // 入射ベクトル 視点からみた光源
        Vector3 incident = lighting.direction;

        // シャドウレイ
        Ray shadowRay;
        // 交差点を始点とするとその物体自身と交差したと判定されるため，
        // 入射ベクトル(単位ベクトル)側に少しだけずらす
        shadowRay.startPoint = intersectionPoint->position + EPSILON * incident.normalize();
        shadowRay.direction = incident.normalize();

        // 光源までの距離
        float lightDistance = lighting.distance;

        // シャドウレイとオブジェクトとの交差判定
        IntersectionResult *shadowResult =
            intersectionWithAll(
                scene->geometry, scene->geometryNum, &shadowRay, lightDistance, true);

        // 光源との間に交点が存在したら影にする
        if (shadowResult->intersectionPoint == nullptr)
        {
            FColor phong = phongShading(
                *intersectionPoint, *ray, lighting, intersectionResult->shape->material);
            *luminance = *luminance + phong;
            // recordLine("phong.r = %f\n", phong.r);
            // recordLine("phong.g = %f\n", phong.g);
            // recordLine("phong.b = %f\n", phong.b);

            if (idx == scene->lightNum - 1)
            {
                // 最後に環境光成分を加える
                Material material = intersectionResult->shape->material;
                *luminance = *luminance + material.ambient * scene->ambientIntensity;
                // recordLine("material.diffuse.r = %f\n", material.diffuse.r);
                // recordLine("material.diffuse.r = %f\n", material.diffuse.g);
                // recordLine("material.diffuse.r = %f\n", material.diffuse.b);
                // recordLine("====================================\n");
            }
        }
    }
}

// void shadowing(
//     Scene *scene, Ray *ray, IntersectionResult *intersectionResult, FColor *luminance)
// {
//     // シャドウレイによる交差判定
//     IntersectionPoint *intersectionPoint = intersectionResult->intersectionPoint;

//     // 入射ベクトル 視点からみた光源
//     Vector3 incident =
//         (scene->pointLight->position - intersectionPoint->position);

//     // シャドウレイ
//     Ray shadowRay;
//     // 交差点を始点とするとその物体自身と交差したと判定されるため，
//     // 入射ベクトル(単位ベクトル)側に少しだけずらす
//     shadowRay.startPoint = intersectionPoint->position + EPSILON * incident.normalize();
//     shadowRay.direction = incident.normalize();

//     // 光源までの距離
//     float lightDistance = (scene->pointLight->position - shadowRay.startPoint).magnitude();

//     // シャドウレイとオブジェクトとの交差判定
//     IntersectionResult *shadowResult =
//         intersectionWithAll(
//             scene->geometry, scene->geometryNum, &shadowRay, lightDistance, true);

//     // 光源との間に交点が存在したら影にする
//     if (shadowResult->intersectionPoint != nullptr)
//     {
//         luminance->r = 0.f;
//         luminance->g = 0.f;
//         luminance->b = 0.f;
//     }
//     // そうでないならフォンシェーディング
//     else
//     {
//         FColor phong = phongShading(
//             *intersectionResult->intersectionPoint, *ray,
//             *(scene->pointLight), intersectionResult->shape->material);
//         luminance->r = phong.r;
//         luminance->g = phong.g;
//         luminance->b = phong.b;
//     }
// }

void reflection(
    Scene *scene, Ray *ray,
    IntersectionResult *intersectionResult, FColor *luminance, unsigned int recursiveLevel)
{
    // 交点
    IntersectionPoint *intersectionPoint = intersectionResult->intersectionPoint;

    // 前の視線ベクトルの逆ベクトル
    Vector3 inverseRayDirection = (-1) * ray->direction;

    // 交点における法線
    Vector3 normal = intersectionPoint->normal;

    // 視線ベクトルの逆ベクトルと法線ベクトルの内積
    float dot = inverseRayDirection.dot(normal);
    if (dot > 0)
    {
        // 正反射ベクトル
        Vector3 newDirection =
            (2 * dot * normal - inverseRayDirection).normalize();

        // 交点を視点とする新しいレイを作成
        Ray newRay;
        newRay.startPoint =
            intersectionResult->intersectionPoint->position + EPSILON * newDirection;
        newRay.direction = newDirection;

        // 次の反射の輝度を取得
        FColor nextLuminance = RayTraceRecursive(scene, &newRay, recursiveLevel + 1);
        if (nextLuminance.r != FLT_MAX)
        {
            // 完全鏡面反射光計算
            FColor reflection = intersectionResult->shape->material.reflection;
            FColor reflectionLuminance;
            reflectionLuminance.r = reflection.r * nextLuminance.r;
            reflectionLuminance.g = reflection.g * nextLuminance.g;
            reflectionLuminance.b = reflection.b * nextLuminance.b;

            luminance->r += reflectionLuminance.r;
            luminance->g += reflectionLuminance.g;
            luminance->b += reflectionLuminance.b;
        }
    }
}

void refraction(
    Scene *scene, Ray *ray,
    IntersectionResult *intersectionResult, FColor *luminance, unsigned int recursiveLevel)
{
    // 交点
    IntersectionPoint *intersectionPoint = intersectionResult->intersectionPoint;

    // 視線ベクトル
    Vector3 eyeDir = ray->direction.normalize();

    // 視線ベクトルの逆ベクトル
    Vector3 invEyeDir = (-1.f) * eyeDir;

    // 交点における法線
    Vector3 normal = intersectionPoint->normal;

    float refractionIndex_1; // 絶対屈折率1
    float refractionIndex_2; // 絶対屈折率2

    float cos_1;
    float cos_2;

    // 視線ベクトルの逆ベクトルと法線ベクトルの内積
    float dot = invEyeDir.dot(normal);
    if (dot < 0)
    {
        // 物体裏面からの進入
        refractionIndex_1 = intersectionResult->shape->material.refractionIndex;
        refractionIndex_2 = scene->globalRefractionIndex;
        normal = (-1.f) * normal;
        // 内積の計算しなおし
        dot = invEyeDir.dot(normal);
    }
    else
    {
        // 物体表面からの進入
        refractionIndex_1 = scene->globalRefractionIndex;
        refractionIndex_2 = intersectionResult->shape->material.refractionIndex;
    }

    // 絶対屈折率2 / 絶対屈折率1 を計算
    float refractionIndexDiv = refractionIndex_2 / refractionIndex_1;

    // cosθ_1 cosθ_2を計算
    cos_1 = invEyeDir.dot(normal);
    cos_2 = refractionIndex_1 / refractionIndex_2 *
            mySqrt(myPow(refractionIndexDiv, 2) - (1.f - myPow(cos_1, 2)));

    float omega = refractionIndexDiv * cos_2 - cos_1;

    // 正反射方向ベクトル計算
    Vector3 specularReflection = 2.f * invEyeDir.dot(normal) * normal - invEyeDir;
    specularReflection = specularReflection.normalize();

    // 屈折方向ベクトル計算
    Vector3 refractionVec =
        (refractionIndex_1 / refractionIndex_2) * eyeDir -
        (refractionIndex_1 / refractionIndex_2) * omega * normal;
    refractionVec = refractionVec.normalize();

    // 正反射方向のレイを生成
    Ray specularReflectionRay;
    specularReflectionRay.startPoint = intersectionPoint->position + EPSILON * specularReflection;
    specularReflectionRay.direction = specularReflection;

    // 屈折方向のレイを生成
    Ray refractionRay;
    refractionRay.startPoint = intersectionPoint->position + EPSILON * refractionVec;
    refractionRay.direction = refractionVec;

    // 偏光反射率計算
    float polarized_p = (refractionIndexDiv * cos_1 - cos_2) / (refractionIndexDiv * cos_1 + cos_2);
    float polarized_s = (-1.f) * omega / (refractionIndexDiv * cos_2 + cos_1);

    // 完全鏡面反射率/透過率 計算
    float cr = (1.f / 2.f) * (myPow(polarized_p, 2) + myPow(polarized_s, 2));
    float ct = 1.f - cr;

    // 各変数の値を表示
    // recordLine("=============================\n");
    // recordLine("eta1 = %f\n", refractionIndex_1);
    // recordLine("eta2 = %f\n", refractionIndex_2);
    // recordLine("etaR = %f\n", refractionIndexDiv);

    // recordLine("cosθ1 = %f\n", cos_1);
    // recordLine("cosθ2 = %f\n", cos_2);

    // recordLine("Ω = %f\n", omega);

    // recordLine("polarized_P = %f\n", polarized_p);
    // recordLine("polarized_S = %f\n", polarized_s);

    // recordLine("cR = %f\n", cr);
    // recordLine("cT = %f\n", ct);

    // recordLine("=============================\n");

    FColor reflection = intersectionResult->shape->material.reflection;

    // 正反射方向の輝度を計算
    // 次の反射の輝度を取得
    FColor nextLuminance = RayTraceRecursive(scene, &specularReflectionRay, recursiveLevel + 1);

    // recordLine("正反射光の放射輝度\n");
    // recordLine("%d : nextLuminance.r = %f\n", recursiveLevel, nextLuminance.r);
    // recordLine("%d : nextLuminance.g = %f\n", recursiveLevel, nextLuminance.g);
    // recordLine("%d : nextLuminance.b = %f\n", recursiveLevel, nextLuminance.b);
    if (nextLuminance.r != FLT_MAX)
    {
        // 完全鏡面反射輝度(正反射)計算
        FColor reflectionLuminance;
        reflectionLuminance.r = nextLuminance.r;
        reflectionLuminance.g = nextLuminance.g;
        reflectionLuminance.b = nextLuminance.b;

        recordLine("cR = %f\n", cr);

        // 最終放射輝度に加算
        luminance->r += reflection.r * cr * reflectionLuminance.r;
        luminance->g += reflection.g * cr * reflectionLuminance.g;
        luminance->b += reflection.b * cr * reflectionLuminance.b;
    }

    // 屈折光の放射輝度計算
    // 次の反射の輝度を取得
    nextLuminance = RayTraceRecursive(scene, &refractionRay, recursiveLevel + 1);
    // recordLine("屈折光の放射輝度\n");
    // recordLine("%d : nextLuminance.r = %f\n", recursiveLevel, nextLuminance.r);
    // recordLine("%d : nextLuminance.g = %f\n", recursiveLevel, nextLuminance.g);
    // recordLine("%d : nextLuminance.b = %f\n", recursiveLevel, nextLuminance.b);

    if (nextLuminance.r != FLT_MAX)
    {
        // 屈折光の放射輝度計算
        FColor refractionLuminance;
        refractionLuminance.r = nextLuminance.r;
        refractionLuminance.g = nextLuminance.g;
        refractionLuminance.b = nextLuminance.b;

        // recordLine("refractionLuminance.r = %f\n", refractionLuminance.r);
        // recordLine("refractionLuminance.g = %f\n", refractionLuminance.g);
        // recordLine("refractionLuminance.b = %f\n", refractionLuminance.b);

        // 最終放射輝度に加算
        luminance->r += reflection.r * ct * refractionLuminance.r;
        luminance->g += reflection.g * ct * refractionLuminance.g;
        luminance->b += reflection.b * ct * refractionLuminance.b;
    }
}