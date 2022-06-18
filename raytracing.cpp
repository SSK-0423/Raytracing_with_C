#include "raytracing_lib.hpp"
#include <string.h>

#define GEOMETRY_NUM 7
#define LIGHT_NUM 1

/*
    argv[1] : 画像サイズ
    argv[2] : ログファイル名
    argv[3] : 何回目の実行か
*/
int main(int argc, char **argv)
{
    // 自分のノード番号
    int myrank;
    // ノード数
    int nodeNum;
    // 画像サイズ
    unsigned int imageSize = atoi(argv[1]);

    // MPI初期化
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nodeNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

    // 全ノードの処理結果の集計結果を格納する
    FColor *resultRadiance = new FColor[imageSize * imageSize];
    // 各ノードの処理結果を格納する
    FColor *myRadiance = new FColor[imageSize * imageSize];

    evaluateTime.Init();
    evaluateTime.totalTime = MPI_Wtime();

    // ログファイル名の決定
    char fileName[128] = "";
    if (argv[2] == nullptr)
    {
        strcat(fileName, "log.txt");
    }
    else
    {
        strcat(fileName, argv[1]);
        strcat(fileName, argv[2]);
    }

    // ログファイル初期化
    if (initLogFile(fileName, "a") == 1)
        return -1;

    // ビットマップデータ
    BitMapData bitmap(imageSize, imageSize, 3);
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

    evaluateTime.raytraceTime = MPI_Wtime();

    // 視線方向で最も近い物体を探し，
    // その物体との交点位置とその点での法線ベクトルを求める
    for (int y = 0; y < bitmap.height; y++)
    {
        for (int x = 0; x < bitmap.width; x++)
        {
            FColor radiance = FColor(0, 0, 0);
            int start = scene.samplingNum / nodeNum * myrank;
            int end = start + scene.samplingNum / nodeNum;
            for (int s = start; s < end; s++)
            {
                float u = (float(x) + myRand());
                float v = (float(y) + myRand());
                // レイを生成
                Ray ray = createRay(camera, u, v, bitmap.width, bitmap.height);
                radiance = radiance + RayTrace(&scene, &ray);
            }
            myRadiance[y * bitmap.width + x] = radiance;
        }
#ifdef PROGRESS
        if (myrank == 0)
        {
            fprintf(stderr, "\rRendering...");
            fprintf(stderr, "%2.0f%%", (float)(y * SCALE + SCALE) / (float)(SCALE * SCALE) * 100);
        }
#endif
    }
#ifdef PROGRESS
    if (myrank == 0)
    {
        fprintf(stderr, "\rRendering...100%%\n");
    }
#endif

    MPI_Reduce(
        myRadiance,
        resultRadiance,
        bitmap.height * bitmap.width * 3,
        MPI_FLOAT,
        MPI_SUM,
        0,
        MPI_COMM_WORLD);

    evaluateTime.raytraceTime = MPI_Wtime() - evaluateTime.raytraceTime;

    // PNG画像出力
    evaluateTime.encodePngTime = MPI_Wtime();
    
    if (myrank == 0)
    {
        for (int y = 0; y < bitmap.height; y++)
        {
            for (int x = 0; x < bitmap.width; x++)
            {
                Color color;
                color.r = resultRadiance[y * bitmap.height + x].r / (float)scene.samplingNum * 0xff;
                color.g = resultRadiance[y * bitmap.height + x].g / (float)scene.samplingNum * 0xff;
                color.b = resultRadiance[y * bitmap.height + x].b / (float)scene.samplingNum * 0xff;
                drawDot(&bitmap, x, y, color);
            }
        }
    }

    // PNGに変換してファイル保存
    if (myrank == 0 && pngFileEncodeWrite(&bitmap, "result.png") == -1)
    {
        freeBitmapData(&bitmap);
        return -1;
    }

    evaluateTime.encodePngTime = MPI_Wtime() - evaluateTime.encodePngTime;

    for (auto o : geometry)
    {
        delete o;
    }

    for (auto l : lights)
    {
        delete l;
    }

    delete[] myRadiance;
    delete[] resultRadiance;

    evaluateTime.totalTime = MPI_Wtime() - evaluateTime.totalTime;

    if (myrank == 0)
    {
        recordLine("\n");
        recordLine("---------------%d回目---------------\n", atoi(argv[3]));
        recordLine("演算子の個数%ld\n", operationCount);
        recordLine("総実行時間: %.2f\n", evaluateTime.totalTime);
        recordLine("レイトレーシング時間: %.2f\n", evaluateTime.raytraceTime);
        recordLine("PNG出力時間: %.2f\n", evaluateTime.encodePngTime);
        recordLine("------------------------------------\n");
        // printf("交差判定時間:%.2f\n", evaluateTime.intersectionTime);
    }

    MPI_Finalize();

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
*/