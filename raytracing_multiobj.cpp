#include "raytracing_lib.hpp"

#define GEOMETRY_NUM 6

int main()
{
    // ログファイル初期化
    if (initLogFile("raytracing_multiobj.txt") == 1)
        return -1;

    // ビットマップデータ
    BitMapData bitmap(512, 512, 3);
    if (bitmap.allocation() == -1)
        return -1;

    // 描画オブジェクト
    Shape *geometry[GEOMETRY_NUM];
    // 球
    geometry[0] = new Sphere(Vector3(-1, 0, 5), 1.f);
    geometry[1] = new Sphere(Vector3(0, 0, 10), 1.f);
    geometry[2] = new Sphere(Vector3(1, 0, 15), 1.f);
    geometry[3] = new Sphere(Vector3(2, 0, 20), 1.f);
    geometry[4] = new Sphere(Vector3(3, 0, 25), 1.f);

    // 平面
    geometry[5] = new Plane(Vector3(0, 1, 0), Vector3(0, -1, 0));

    // マテリアルセット
    geometry[0]->material.diffuse = FColor(0.69f, 0.f, 0.f);
    geometry[1]->material.diffuse = FColor(0.f, 0.69f, 0.f);
    geometry[2]->material.diffuse = FColor(0.f, 0.f, 0.69f);
    geometry[3]->material.diffuse = FColor(0.f, 0.69f, 0.69f);
    geometry[4]->material.diffuse = FColor(0.69f, 0.f, 0.69f);

    // 視点の位置を決める
    Camera camera;
    camera.position = Vector3(0, 0, -5);

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
                    // Color color = phongShading(intersectionPoint, &ray, &pointLight);
                    Color color = phongShading(
                        *intersectionPoint, ray, pointLight, geometry[idx]->material);
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