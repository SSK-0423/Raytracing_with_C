#include "raytracing_lib.hpp"

#define GEOMETRY_NUM 100
#define LIGHT_NUM 1
#define EVALUATE_NUM 10
#define SCALE 512

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

    // 平面
    geometry[0] = new Plane(Vector3(0, 1, 0), Vector3(0, -1, 0));

    // 球
    for (int i = 1; i < GEOMETRY_NUM; i++)
    {
        geometry[i] = new Sphere(Vector3(75 * myRand(), 2 * myRand(), 30 * myRand()), 0.35f * myRand());
        FColor random = FColor(1.f * myRand(), 1.f * myRand(), 1.f * myRand());
        geometry[i]->material =
            Material(random, random, random, myRand());
        if (100 * myRand() <= 20)
        {
            geometry[i]->material.useReflection = true;
            geometry[i]->material.reflection = FColor(1.f, 1.f, 1.f);
        }
    }

    // 視点の位置を決める
    Camera camera;
    camera.position = Vector3(0, 0, -5);

    // 点光源の位置を決める
    PointLight *pointLight = new PointLight();
    pointLight->position = Vector3(0, 0.9, 2.5);
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
    if (pngFileEncodeWrite(&bitmap, "raytracing_evaluate.png") == -1)
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
    printf("交差判定時間:%.2f\n", evaluateTime.intersectionTime);
    printf("PNG出力時間: %.2f\n", evaluateTime.encodePngTime);

    MPI_Finalize();
#endif

    finalLogFile();

    return 0;
}