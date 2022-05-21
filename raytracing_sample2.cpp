#include "raytracing_lib.hpp"

#define GEOMETRY_NUM 50
#define LIGHT_NUM 3
#define SCALE 512

int main(int argc, char **argv)
{
    // ビットマップデータ
    BitMapData bitmap(SCALE, SCALE, 3);
    if (bitmap.allocation() == -1)
        return -1;

    // 描画オブジェクト
    Shape *geometry[GEOMETRY_NUM];

    // 球
    geometry[0] = new Sphere(Vector3(-0.4, -0.65, 3), 0.35f);
    geometry[0]->material =
        Material(FColor(0.f, 0.f, 0.f), FColor(0.f, 0.f, 0.f), FColor(0.f, 0.f, 0.f), 0.f);
    geometry[0]->material.useReflection = true;
    geometry[0]->material.reflection = FColor(1.f, 1.f, 1.f);

    geometry[1] = new Sphere(Vector3(0.5, -0.65, 2), 0.35f);
    geometry[1]->material =
        Material(FColor(0.f, 0.f, 0.f), FColor(0.f, 0.f, 0.f), FColor(0.f, 0.f, 0.f), 0.f);
    geometry[1]->material.useReflection = true;
    geometry[1]->material.reflection = FColor(1.f, 1.f, 1.f);

    for (int i = 2; i < GEOMETRY_NUM - 1; i++)
    {
        geometry[i] =
            new Sphere(Vector3(10.f * myRand() - 5.f, 2.f * myRand(), 40.f * myRand()),
                       0.5f * myRand());
        geometry[i]->material.diffuse = FColor(myRand(), myRand(), myRand());
        geometry[i]->material.specular = FColor(myRand(), myRand(), myRand());
        geometry[i]->material.ambient = FColor(myRand(), myRand(), myRand());
        geometry[i]->material.shininess = 40 * myRand();
    }

    // 平面
    geometry[GEOMETRY_NUM - 1] = new Plane(Vector3(0, 1, 0), Vector3(0, -1, 0)); // 床

    // マテリアルセット
    geometry[GEOMETRY_NUM - 1]->material.diffuse = FColor(0.7f, 0.7f, 0.7f);

    // 視点の位置を決める
    Camera camera;
    camera.position = Vector3(0, 0, -5);

    // 光源
    // 点光源
    PointLight *point1 = new PointLight();
    point1->position = Vector3(-5, 5, -5);
    point1->intensity = FColor(0.5, 0.5, 0.5);
    PointLight *point2 = new PointLight();
    point2->position = Vector3(5, 0, -5);
    point2->intensity = FColor(1.2, 1.2, 1.2);
    // 平行光源
    DirectionalLight *directional = new DirectionalLight();
    directional->direction = Vector3(2, 0, 1);
    directional->intensity = FColor(1.f, 1.f, 1.f);

    Light *lights[LIGHT_NUM];

    lights[0] = directional;
    lights[1] = point1;
    lights[2] = point2;

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
    scene.samplingNum = 20;

    // 視線方向で最も近い物体を探し，
    // その物体との交点位置とその点での法線ベクトルを求める
    for (int y = 0; y < bitmap.height; y++)
    {
        for (int x = 0; x < bitmap.width; x++)
        {
            FColor luminance = FColor(0, 0, 0);
            for (int s = 0; s < scene.samplingNum; s++)
            {
                float u = (float(x) + myRand());
                float v = (float(y) + myRand());
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

    // PNGに変換してファイル保存
    if (pngFileEncodeWrite(&bitmap, "raytracing_sample2.png") == -1)
    {
        freeBitmapData(&bitmap);
        return -1;
    }

    for (auto o : geometry)
    {
        delete o;
    }

    for (auto l : lights)
    {
        delete l;
    }

    return 0;
}