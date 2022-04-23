#include "raytracing_lib.hpp"

#define GEOMETRY_NUM 6
#define LIGHT_NUM 3

int main()
{
    // ログファイル初期化
    if (initLogFile("log.txt") == 1)
        return -1;

    // ビットマップデータ
    BitMapData bitmap(1280, 1280, 3);
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

    Light *lights[LIGHT_NUM];

    // 点光源の位置を決める
    PointLight pointLight;
    pointLight.position = Vector3(-5, 5, -5);
    pointLight.intensity = FColor(1.f, 1.f, 1.f);

    PointLight *point1 = new PointLight();
    point1->position = Vector3(-5, 5, -5);
    point1->intensity = FColor(0.5, 0.5, 0.5);
    PointLight *point2 = new PointLight();
    point2->position = Vector3(5, 0, -5);
    point2->intensity = FColor(0.5, 0.5, 0.5);
    PointLight *point3 = new PointLight();
    point3->position = Vector3(5, 20, -5);
    point3->intensity = FColor(0.5, 0.5, 0.5);

    lights[0] = point1;
    lights[1] = point2;
    lights[2] = point3;

    DirectionalLight *directional = new DirectionalLight();
    directional->direction = Vector3(0, -1, 0);
    directional->intensity = FColor(1, 1, 1);

    // シーン作成
    Scene scene;
    scene.bitmap = &bitmap;
    scene.camera = &camera;
    scene.geometry = geometry;
    scene.geometryNum = GEOMETRY_NUM;
    scene.backgroundColor = FColor(100.f / 255.f, 149.f / 255.f, 237.f / 255.f);
    scene.pointLight = &pointLight;
    scene.light = lights;
    scene.lightNum = LIGHT_NUM;

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
    if (pngFileEncodeWrite(&bitmap, "raytracing_multiLight.png") == -1)
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