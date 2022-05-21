#include "raytracing_lib.hpp"

#define GEOMETRY_NUM 50
#define LIGHT_NUM 3
#define EVALUATE_NUM 10
#define SCALE 1024

int main(int argc, char **argv)
{
#ifdef MPI
    // MPI初期化
    MPI_Status status;
    MPI_Init(&argc, &argv);

    evaluateTime.Init();
    evaluateTime.totalTime = MPI_Wtime();
#endif

    // ログファイル初期化
    if (initLogFile("log.txt") == 1)
        return -1;

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
    // geometry[1]->material.diffuse = FColor(0.4f, 1.f, 0.4f);
    geometry[1]->material.useReflection = true;
    geometry[1]->material.reflection = FColor(1.f, 1.f, 1.f);
    // geometry[1]->material.useRefraction = true;
    // geometry[1]->material.refractionIndex = 1.51;
    geometry[3] = new Sphere(Vector3(-0.7, -0.65, 15), 0.35f);
    geometry[3]->material.diffuse = FColor(0.f, 0.5f, 1.f);

    geometry[4] = new Sphere(Vector3(-0.1, -0.65, 20), 0.35f);
    geometry[4]->material =
        Material(FColor(0.f, 0.f, 0.f), FColor(0.f, 0.f, 0.f), FColor(0.f, 0.f, 0.f), 0.f);
    geometry[4]->material.useReflection = true;
    geometry[4]->material.reflection = FColor(1.f, 1.f, 1.f);

    for (int i = 5; i < GEOMETRY_NUM; i++)
    {
        geometry[i] =
            new Sphere(Vector3(10 * myRand() - 5.f, 2 * myRand() - 0.65f, 40 * myRand()),
                       0.5f * myRand());
        if (myRand() <= 0.1f)
        {
            geometry[i]->material =
                Material(FColor(0.f, 0.f, 0.f), FColor(0.f, 0.f, 0.f), FColor(0.f, 0.f, 0.f));
            geometry[i]->material.useReflection = true;
            geometry[i]->material.reflection = FColor(1.f, 1.f, 1.f);
        }
        else
        {
            geometry[i]->material.diffuse = FColor(myRand(), myRand(), myRand());
            geometry[i]->material.specular = FColor(myRand(), myRand(), myRand());
            geometry[i]->material.ambient = FColor(myRand(), myRand(), myRand());
            geometry[i]->material.shininess = 40 * myRand();
        }
    }

    // 平面
    geometry[2] = new Plane(Vector3(0, 1, 0), Vector3(0, -1, 0)); // 白い床
    // geometry[3] = new Plane(Vector3(0, -1, 0), Vector3(0, 1, 0)); // 白い天井
    // geometry[4] = new Plane(Vector3(1, 0, 0), Vector3(-1, 0, 0)); // 赤い壁
    // geometry[5] = new Plane(Vector3(-1, 0, 0), Vector3(1, 0, 0)); // 青の壁
    // geometry[6] = new Plane(Vector3(0, 0, 0), Vector3(0, 0, 0)); // 白い壁

    // マテリアルセット
    geometry[2]->material.diffuse = FColor(0.7f, 0.7f, 0.7f);
    // geometry[2]->material =
    //     Material(FColor(0.f, 0.f, 0.f), FColor(0.f, 0.f, 0.f), FColor(0.f, 0.f, 0.f), 0.f);
    // geometry[2]->material.useReflection = true;
    // geometry[2]->material.reflection = FColor(1.f, 1.f, 1.f);

    // geometry[3]->material.diffuse = FColor(0.7f, 0.7f, 0.7f);
    // geometry[3]->material =
    //     Material(FColor(0.f, 0.f, 0.f), FColor(0.f, 0.f, 0.f), FColor(0.f, 0.f, 0.f), 0.f);
    // geometry[3]->material.useReflection = true;
    // geometry[3]->material.reflection = FColor(1.f, 1.f, 1.f);

    // geometry[4]->material.diffuse = FColor(1.f, 0.4f, 0.4f);

    // geometry[5]->material.diffuse = FColor(0.4f, 0.4f, 1.f);
    // geometry[5]->material =
    //     Material(FColor(0.f, 0.f, 0.f), FColor(0.f, 0.f, 0.f), FColor(0.f, 0.f, 0.f), 0.f);
    // geometry[5]->material.useReflection = true;
    // geometry[5]->material.reflection = FColor(1.f, 1.f, 1.f);

    // geometry[6]->material.diffuse = FColor(0.7f, 0.7f, 0.7f);
    // geometry[6]->material =
    //     Material(FColor(0.f, 0.f, 0.f), FColor(0.f, 0.f, 0.f), FColor(0.f, 0.f, 0.f), 0.f);
    // geometry[6]->material.useReflection = true;
    // geometry[6]->material.reflection = FColor(1.f, 1.f, 1.f);

    // 視点の位置を決める
    Camera camera;
    camera.position = Vector3(0, 0, -5);

    // 点光源の位置を決める
    PointLight *pointLight = new PointLight();
    pointLight->position = Vector3(0, 1, 2.5);
    pointLight->intensity = FColor(.8f, .8f, .8f);
    PointLight *point2 = new PointLight();
    point2->position = Vector3(5, 0, -5);
    point2->intensity = FColor(1.2, 1.2, 1.2);
    PointLight *point1 = new PointLight();
    point1->position = Vector3(-5, 5, -5);
    point1->intensity = FColor(0.5, 0.5, 0.5);
    DirectionalLight *directional = new DirectionalLight();
    directional->direction = Vector3(2, 0, 1);
    directional->intensity = FColor(1.f, 1.f, 1.f);

    Light *lights[LIGHT_NUM];

    lights[0] = point1;
    lights[1] = directional;
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

#ifdef MPI
    evaluateTime.raytraceTime = MPI_Wtime();
#endif
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
#ifdef MPI
    evaluateTime.raytraceTime = MPI_Wtime() - evaluateTime.raytraceTime;
#endif
    recordLine("演算子の個数%ld\n", operationCount);

#ifdef MPI
    evaluateTime.encodePngTime = MPI_Wtime();
#endif
    // PNGに変換してファイル保存
    if (pngFileEncodeWrite(&bitmap, "raytracing_reflection_sandbox.png") == -1)
    {
        freeBitmapData(&bitmap);
        return -1;
    }
#ifdef MPI
    evaluateTime.encodePngTime = MPI_Wtime() - evaluateTime.encodePngTime;
#endif

    for (auto o : geometry)
    {
        delete o;
    }

    for (auto l : lights)
    {
        delete l;
    }

#ifdef MPI
    evaluateTime.totalTime = MPI_Wtime() - evaluateTime.totalTime;

    printf("総実行時間: %.2f\n", evaluateTime.totalTime);
    printf("レイトレーシング時間: %.2f\n", evaluateTime.raytraceTime);
    // printf("交差判定時間:%.2f\n", evaluateTime.intersectionTime);
    printf("PNG出力時間: %.2f\n", evaluateTime.encodePngTime);
    printf("------------------------------------\n");

    MPI_Finalize();
#endif

    finalLogFile();

    return 0;
}