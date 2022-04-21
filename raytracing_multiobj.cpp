#include "raytracing_lib.hpp"
#include "log.hpp"

#define GEOMETRY_NUM 2

// フォンシェーディング
Color phongShading(IntersectionPoint *intersectionPoint, Ray *ray, PointLight *pointLight);

int main()
{
    // ログファイル初期化
    if (initLogFile("raytracing_multiobj.txt") == 1)
        return -1;

    // ビットマップデータ
    BitMapData bitmap(512, 512, 3);
    if (bitmap.allocation() == -1)
        return -1;

    ZBuffer zbuffer(512, 512);

    // 描画オブジェクト
    Shape *geometry[GEOMETRY_NUM];
    // 球
    geometry[0] = new Sphere(Vector3(0, 0, 5), 1.f);
    // 平面
    geometry[1] = new Plane(Vector3(0, 1, 0), Vector3(0, -1, 0));
    // 板
    // geometry[1] = new Plane(Vector3(0, 1, 0), Vector3(0, -1, 0)); // 白床
    // geometry[2] = new Plane(Vector3(0, 0, -1), Vector3(0, 0, 5));   // 白壁
    // geometry[3] = new Plane(Vector3(-1, 0, 0), Vector3(1, 0, 0));   // 白天井
    // geometry[4] = new Plane(Vector3(1, 0, 0), Vector3(-1, 0, 0));  // 赤壁
    // geometry[5] = new Plane(Vector3(-1, 0, 0), Vector3(1, 0, 0));  // 緑壁

    // 視点の位置を決める
    Camera camera;
    camera.position = Vector3(1, 0, -5);

    // 点光源の位置を決める
    PointLight pointLight;
    pointLight.position = Vector3(-5, 5, -5);

    // 視線方向で最も近い物体を探し，
    // その物体との交点位置とその点での法線ベクトルを求める
    for (int y = 0; y < bitmap.height; y++)
    {
        for (int x = 0; x < bitmap.width; x++)
        {
            // レイを生成
            Ray ray = createRay(camera, x, y, bitmap.width, bitmap.height);

            bool isHit = false; // レイがオブジェクトにあたったか

            // 全オブジェクトの交点を調べ，一番最初にぶつかったものを描画する
            for (size_t idx = 0; idx < GEOMETRY_NUM; idx++)
            {
                // レイと球が交差するか判定+交点があれば計算
                IntersectionPoint *intersectionPoint = geometry[idx]->isIntersectionRay(&ray);
                if (intersectionPoint != nullptr)
                {
                    isHit = true;
                    Color color = phongShading(intersectionPoint, &ray, &pointLight);
                    drawDot(&bitmap, x, y, color);
                    break;
                }
            }

            // どのオブジェクトにもヒットしない場合は背景描画
            if (!isHit)
                drawDot(&bitmap, x, y, Color(100, 149, 237));
        }
    }

    recordLine("演算子の個数%ld\n", operationCount);

    // PNGに変換してファイル保存
    if (pngFileEncodeWrite(&bitmap, "raytracing_multiobj.png") == -1)
    {
        freeBitmapData(&bitmap);
        return -1;
    }

    freeBitmapData(&bitmap);

    finalLogFile();

    return 0;
}

// 物体表面の光源の性質を使ってその点での色を決定する(シェーディング)
Color phongShading(
    IntersectionPoint *intersectionPoint, Ray *ray, PointLight *pointLight)
{
    // 法線ベクトル
    Vector3 normal = intersectionPoint->normal;

    // 入射ベクトル計算(光が当たる点からみた光源の位置であることに注意)
    Vector3 incident =
        (pointLight->position - intersectionPoint->position).normalize();

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
    Vector3 inverseEyeDir = ((-1) * ray->direction).normalize();
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