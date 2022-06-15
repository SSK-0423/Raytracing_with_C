#include "raytracing_lib.hpp"

#define GEOMETRY_NUM 7
#define LIGHT_NUM 1
#define EVALUATE_NUM 10
#define SCALE 512

// 全ノードの処理結果の集計結果を格納する
volatile FColor resultRadiance[SCALE][SCALE];
// 各ノードの処理結果を格納する
FColor myRadiance[SCALE][SCALE];

int main(int argc, char **argv)
{
#ifdef MPI

    // 自分のノード番号
    int myrank;
    // ノード数
    int nodeNum;

    // MPI初期化
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nodeNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

    printf("ノード数: %d\n", nodeNum);
    printf("自分のランク %d\n", myrank);

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
    geometry[1]->material.diffuse = FColor(0.4f, 1.f, 0.4f);

    // 平面
    geometry[2] = new Plane(Vector3(0, 1, 0), Vector3(0, -1, 0)); // 白い床
    geometry[3] = new Plane(Vector3(0, -1, 0), Vector3(0, 1, 0)); // 白い天井
    geometry[4] = new Plane(Vector3(1, 0, 0), Vector3(-1, 0, 0)); // 赤い壁
    geometry[5] = new Plane(Vector3(-1, 0, 0), Vector3(1, 0, 0)); // 青の壁
    geometry[6] = new Plane(Vector3(0, 0, -1), Vector3(0, 0, 5)); // 白い壁

    // マテリアルセット
    geometry[2]->material.diffuse = FColor(0.7f, 0.7f, 0.7f);
    geometry[3]->material.diffuse = FColor(0.7f, 0.7f, 0.7f);
    geometry[4]->material.diffuse = FColor(1.f, 0.4f, 0.4f);
    geometry[5]->material.diffuse = FColor(0.4f, 0.4f, 1.f);
    geometry[6]->material.diffuse = FColor(0.7f, 0.7f, 0.7f);

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

            int start = scene.samplingNum / nodeNum * myrank;
            int end = start + scene.samplingNum / nodeNum;
            if (x == 0 && y == 0)
                printf("rank = %d start = %d end = %d\n", myrank, start, end);
            for (int s = start; s < end; s++)
            {
                float u = (float(x) + myRand());
                float v = (float(y) + myRand());
                // レイを生成
                Ray ray = createRay(camera, u, v, bitmap.width, bitmap.height);
                luminance = luminance + RayTrace(&scene, &ray);
            }
            myRadiance[y][x] = luminance;
        }
    }

#ifdef MPI
    MPI_Reduce(
        &myRadiance,
        (void *)&resultRadiance,
        SCALE * SCALE * 3,
        MPI_FLOAT,
        MPI_SUM,
        0,
        MPI_COMM_WORLD);

    evaluateTime.raytraceTime = MPI_Wtime() - evaluateTime.raytraceTime;

    // 書き込み
    if (myrank == 0)
    {
        for (int y = 0; y < bitmap.height; y++)
        {
            for (int x = 0; x < bitmap.width; x++)
            {
                Color color;
                color.r = resultRadiance[y][x].r / (float)scene.samplingNum * 0xff;
                color.g = resultRadiance[y][x].g / (float)scene.samplingNum * 0xff;
                color.b = resultRadiance[y][x].b / (float)scene.samplingNum * 0xff;
                drawDot(&bitmap, x, y, color);
                // recordLine("rank = 0 : resultRadiance[%d][%d] = (%4.2f,%4.2f,%4.2f)\n", y, x, resultRadiance[y][x].r, resultRadiance[y][x].g, resultRadiance[y][x].b);
            }
        }
    }

#endif

    recordLine("演算子の個数%ld\n", operationCount);

#ifdef MPI
    evaluateTime.encodePngTime = MPI_Wtime();
#endif

    // PNGに変換してファイル保存
    if (pngFileEncodeWrite(&bitmap, "result.png") == -1)
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

/*
    並列化実装手順
    1. ノード数取得
    2. サンプリングループの回数をサンプリング数/ノード数
    3. myColorに値代入
    4. MPI_Reduceで平均値計算
    5. 書き込み ←ここも並列化すると良い

                    color.r = myRadiance[y][x].r / (float)(scene.samplingNum / 4) * 0xff;
                color.g = myRadiance[y][x].g / (float)(scene.samplingNum / 4) * 0xff;
                color.b = myRadiance[y][x].b / (float)(scene.samplingNum / 4) * 0xff;
*/