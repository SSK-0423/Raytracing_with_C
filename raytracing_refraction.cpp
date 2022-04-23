#include "raytracing_lib.hpp"

#define GEOMETRY_NUM 7

int main()
{
    // ログファイル初期化
    if (initLogFile("log.txt") == 1)
        return -1;

    // ビットマップデータ
    BitMapData bitmap(512, 512, 3);
    if (bitmap.allocation() == -1)
        return -1;

    // 描画オブジェクト
    Shape *geometry[GEOMETRY_NUM];

    // 球(完全鏡面反射 )
    geometry[0] = new Sphere(Vector3(-0.4, -0.65, 3), 0.35f);
    geometry[0]->material =
        Material(FColor(0.f, 0.f, 0.f), FColor(0.f, 0.f, 0.f), FColor(0.f, 0.f, 0.f),0.f);
    geometry[0]->material.useReflection = true;
    geometry[0]->material.reflection = FColor(1.f, 1.f, 1.f);

    // 球(屈折)
    geometry[1] = new Sphere(Vector3(0.5, -0.65, 2), 0.35f);
    geometry[1]->material =
        Material(FColor(0.f, 0.f, 0.f), FColor(0.f, 0.f, 0.f), FColor(0.f, 0.f, 0.f),0.f);
    geometry[1]->material.useRefraction = true;
    geometry[1]->material.reflection = FColor(1.f, 1.f, 1.f);
    geometry[1]->material.refractionIndex = 1.51;

    // 平面
    geometry[2] = new Plane(Vector3(0, 1, 0), Vector3(0, -1, 0)); // 白い床
    geometry[3] = new Plane(Vector3(0, -1, 0), Vector3(0, 1, 0)); // 白い天井
    geometry[4] = new Plane(Vector3(1, 0, 0), Vector3(-1, 0, 0)); // 赤い壁
    geometry[5] = new Plane(Vector3(-1, 0, 0), Vector3(1, 0, 0)); // 緑の壁
    geometry[6] = new Plane(Vector3(0, 0, -1), Vector3(0, 0, 5)); // 白い壁

    // マテリアルセット
    geometry[2]->material.diffuse = FColor(0.7f, 0.7f, 0.7f);
    geometry[3]->material.diffuse = FColor(1.f, 1.f, 1.f);
    geometry[4]->material.diffuse = FColor(1.f, 0, 0);
    geometry[5]->material.diffuse = FColor(0, 0, 1.f);
    geometry[6]->material.diffuse = geometry[2]->material.diffuse;

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
    if (pngFileEncodeWrite(&bitmap, "raytracing_refraction.png") == -1)
    {
        freeBitmapData(&bitmap);
        return -1;
    }

    freeBitmapData(&bitmap);

    for (auto o : geometry)
    {
        delete o;
    }

    finalLogFile();

    return 0;
}