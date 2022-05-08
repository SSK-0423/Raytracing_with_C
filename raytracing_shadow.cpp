#include "raytracing_lib.hpp"

#define GEOMETRY_NUM 2
#define LIGHT_NUM 1

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
    geometry[0] = new Sphere(Vector3(0, 0, 5), 1.f);

    // geometry[0] = new Sphere(Vector3(3, 0, 25), 1.f);
    // geometry[1] = new Sphere(Vector3(2, 0, 20), 1.f);
    // geometry[2] = new Sphere(Vector3(1, 0, 15), 1.f);
    // geometry[3] = new Sphere(Vector3(0, 0, 10), 1.f);
    // geometry[4] = new Sphere(Vector3(-1, 0, 5), 1.f);

    // 平面
    geometry[1] = new Plane(Vector3(0, 1, 0), Vector3(0, -1, 0));

    // マテリアルセット
    geometry[0]->material.diffuse = FColor(0.69f, 0.f, 0.69f);
    geometry[0]->material.useReflection = true;
    geometry[0]->material.reflection = FColor(1, 1, 1);
    // geometry[1]->material.diffuse = FColor(0.f, 0.69f, 0.69f);
    // geometry[2]->material.diffuse = FColor(0.f, 0.f, 0.69f);
    // geometry[3]->material.diffuse = FColor(0.f, 0.69f, 0.f);
    // geometry[4]->material.diffuse = FColor(0.69f, 0.f, 0.f);

    // 視点の位置を決める
    Camera camera;
    camera.position = Vector3(0, 0, -5);

    // 点光源の位置を決める
    PointLight *pointLight = new PointLight();
    pointLight->position = Vector3(-5, 5, -5);
    pointLight->intensity = FColor(1.f, 1.f, 1.f);

    Light *lights[LIGHT_NUM];

    lights[0] = pointLight;

    // シーン作成
    Scene scene;
    scene.bitmap = &bitmap;
    scene.camera = &camera;
    scene.geometry = geometry;
    scene.geometryNum = GEOMETRY_NUM;
    scene.backgroundColor = FColor(100.f / 255.f, 149.f / 255.f, 237.f / 255.f);
    scene.light = lights;
    scene.lightNum = LIGHT_NUM;
    scene.ambientIntensity = FColor(0.1, 0.1, 0.1);
    scene.samplingNum = 50;

    // 視線方向で最も近い物体を探し，
    // その物体との交点位置とその点での法線ベクトルを求める
    for (int y = 0; y < bitmap.height; y++)
    {
        for (int x = 0; x < bitmap.width; x++)
        {
            FColor luminance = FColor(0, 0, 0);
            for (int s = 0; s < scene.samplingNum; s++)
            {
                float u = (float(x) + drand48());
                float v = (float(y) + drand48());
                // レイを生成
                Ray ray = createRay(camera, u, v, bitmap.width, bitmap.height);
                luminance = luminance + RayTrace(&scene, &ray);
            }
            Color color;
            color.r = luminance.r / (float)scene.samplingNum * 0xff;
            color.g = luminance.g / (float)scene.samplingNum * 0xff;
            color.b = luminance.b / (float)scene.samplingNum * 0xff;
            drawDot(&bitmap, x, y, color);
        }
    }

    recordLine("演算子の個数%ld\n", operationCount);

    // PNGに変換してファイル保存
    if (pngFileEncodeWrite(&bitmap, "raytracing_shadow.png") == -1)
    {
        freeBitmapData(&bitmap);
        return -1;
    }

    freeBitmapData(&bitmap);

    finalLogFile();

    return 0;
}