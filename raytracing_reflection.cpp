#include "raytracing_lib.hpp"

#define GEOMETRY_NUM 6

int main()
{
    // ログファイル初期化
    if (initLogFile("raytracing_reflection.txt") == 1)
        return -1;

    // ビットマップデータ
    BitMapData bitmap(512, 512, 3);
    if (bitmap.allocation() == -1)
        return -1;

    // 描画オブジェクト
    Shape *geometry[GEOMETRY_NUM];

    // 球
    geometry[0] = new Sphere(Vector3(-0.25, -0.5, 3), 0.5f);

    // 平面
    geometry[1] = new Plane(Vector3(0, 1, 0), Vector3(0, -1, 0)); // 白い床
    geometry[2] = new Plane(Vector3(0, -1, 0), Vector3(0, 1, 0)); // 白い天井
    geometry[3] = new Plane(Vector3(1, 0, 0), Vector3(-1, 0, 0)); // 赤い壁
    geometry[4] = new Plane(Vector3(-1, 0, 0), Vector3(1, 0, 0)); // 緑の壁
    geometry[5] = new Plane(Vector3(0, 0, -1), Vector3(0, 0, 5)); // 白い壁

    // マテリアルセット
    geometry[1]->material.diffuse = FColor(1.f, 1.f, 1.f);
    geometry[2]->material.diffuse = FColor(1.f, 1.f, 1.f);
    geometry[3]->material.diffuse = FColor(1.f, 0, 0);
    geometry[4]->material.diffuse = FColor(0, 1.f, 0);
    geometry[5]->material.diffuse = FColor(1.f, 1.f, 1.f);

    // 視点の位置を決める
    Camera camera;
    camera.position = Vector3(0, 0, -5);

    // 点光源の位置を決める
    PointLight pointLight;
    pointLight.position = Vector3(0, 0.9, 2.5);
    pointLight.intensity = FColor(1.f, 1.f, 1.f);

    // シーン作成
    Scene scene;
    scene.bitmap = &bitmap;
    scene.camera = &camera;
    scene.geometry = geometry;
    scene.geometryNum = GEOMETRY_NUM;
    scene.gackgroundColor = Color(100, 149, 237);
    scene.pointLight = &pointLight;

    // 視線方向で最も近い物体を探し，
    // その物体との交点位置とその点での法線ベクトルを求める
    for (int y = 0; y < bitmap.height; y++)
    {
        for (int x = 0; x < bitmap.width; x++)
        {
            // レイを生成
            Ray ray = createRay(camera, x, y, bitmap.width, bitmap.height);
            FColor luminance = RayTrace(&scene, &ray);
            Color color;
            color.r = luminance.r * 0xff;
            color.g = luminance.g * 0xff;
            color.b = luminance.b * 0xff;
            drawDot(&bitmap, x, y, color);
        }
    }

    recordLine("演算子の個数%ld\n", operationCount);

    // PNGに変換してファイル保存
    if (pngFileEncodeWrite(&bitmap, "raytracing_reflection.png") == -1)
    {
        freeBitmapData(&bitmap);
        return -1;
    }

    freeBitmapData(&bitmap);

    finalLogFile();

    return 0;
}