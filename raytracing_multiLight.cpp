#include "raytracing_lib.hpp"

#define GEOMETRY_NUM 6

int main()
{
    // ログファイル初期化
    if (initLogFile("raytracing_multiLight.txt") == 1)
        return -1;

    // ビットマップデータ
    BitMapData bitmap(512, 512, 3);
    if (bitmap.allocation() == -1)
        return -1;

    // 描画オブジェクト
    Shape *geometry[GEOMETRY_NUM];

    // 球
    geometry[0] = new Sphere(Vector3(3, 0, 25), 1.f);
    geometry[1] = new Sphere(Vector3(2, 0, 20), 1.f);
    geometry[2] = new Sphere(Vector3(1, 0, 15), 1.f);
    geometry[3] = new Sphere(Vector3(0, 0, 10), 1.f);
    geometry[4] = new Sphere(Vector3(-1, 0, 5), 1.f);

    // 平面
    geometry[5] = new Plane(Vector3(0, 1, 0), Vector3(0, -1, 0));

    // マテリアルセット
    geometry[0]->material.diffuse = FColor(0.69f, 0.f, 0.69f);
    geometry[1]->material.diffuse = FColor(0.f, 0.69f, 0.69f);
    geometry[2]->material.diffuse = FColor(0.f, 0.f, 0.69f);
    geometry[3]->material.diffuse = FColor(0.f, 0.69f, 0.f);
    geometry[4]->material.diffuse = FColor(0.69f, 0.f, 0.f);

    // 視点の位置を決める
    Camera camera;
    camera.position = Vector3(0, 0, -5);

    // 点光源の位置を決める
    PointLight* pointLight = new PointLight(Vector3(-5, 5, -5), FColor(1.f, 1.f, 1.f));
    // pointLight.position = Vector3(-5, 5, -5);
    // pointLight.intensity = FColor(1.f, 1.f, 1.f);
    // recordLine("%f,%f,%f\n",pointLight->position.x,pointLight->position.y,pointLight->position.z);

    // 視線方向で最も近い物体を探し，
    // その物体との交点位置とその点での法線ベクトルを求める
    for (int y = 0; y < bitmap.height; y++)
    {
        for (int x = 0; x < bitmap.width; x++)
        {
            // レイを生成
            Ray ray = createRay(camera, x, y, bitmap.width, bitmap.height);

            // 全物体との交差判定
            IntersectionResult *intersectionResult =
                intersectionWithAll(geometry, GEOMETRY_NUM, &ray);

            // レイと物体に交点あり
            if (intersectionResult->intersectionPoint != nullptr)
            {
                // Color color = phongShading(
                //     *intersectionResult->intersectionPoint, ray,
                //     pointLight, intersectionResult->shape->material);
                // Material material = intersectionResult->shape->material;
                // Lighting lighting = pointLight.lightingAt(intersectionResult->intersectionPoint->position);
                // recordLine("point = (%4.2f, %4.2f, %4.2f)\n",
                //            point->position.x, point->position.y, point->position.z);
                Vector3 answer =
                (pointLight->position - intersectionResult->intersectionPoint->position).normalize();
                recordLine("answer direction = (%4.2f, %4.2f, %4.2f)\n",
                           answer.x, answer.y, answer.z);
                Color color = phongShading(
                    *intersectionResult->intersectionPoint, ray,
                    pointLight->lightingAt(intersectionResult->intersectionPoint->position),
                    intersectionResult->shape->material);
                drawDot(&bitmap, x, y, color);
            }
            else
            {
                drawDot(&bitmap, x, y, Color(100, 149, 237));
            }
        }
    }

    recordLine("演算子の個数%ld\n", operationCount);

    // PNGに変換してファイル保存
    if (pngFileEncodeWrite(&bitmap, "raytracing_multiLight.png") == -1)
    {
        freeBitmapData(&bitmap);
        return -1;
    }

    freeBitmapData(&bitmap);

    finalLogFile();

    return 0;
}