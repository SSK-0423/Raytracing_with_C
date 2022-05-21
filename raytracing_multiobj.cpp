#include "raytracing_lib.hpp"

#define GEOMETRY_NUM 6

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
    PointLight pointLight;
    pointLight.position = Vector3(-5, 5, -5);
    pointLight.intensity = FColor(1.f, 1.f, 1.f);
    

    // 視線方向で最も近い物体を探し，
    // その物体との交点位置とその点での法線ベクトルを求める
    for (int y = 0; y < bitmap.height; y++)
    {
        for (int x = 0; x < bitmap.width; x++)
        {
            // レイを生成
            Ray ray = createRay(camera, x, y, bitmap.width, bitmap.height);

            float minDistance = 10e6;                       // レイの視点との最小距離
            IntersectionPoint *intersectionPoint = nullptr; // レイの始点との最近交点
            size_t drawIdx = 0;                             // 描画するオブジェクトのインデックス

            // 全オブジェクトの交点を調べ，レイの視点に最も近い交点を決定する
            for (size_t idx = 0; idx < GEOMETRY_NUM; idx++)
            {
                // レイと球が交差するか判定+交点があれば計算
                IntersectionPoint *point = nullptr;
                point = geometry[idx]->isIntersectionRay(&ray);

                // 交点ないならスキップ
                if (point == nullptr)
                    continue;

                // レイの始点から交点への距離計算
                float distance = (point->position - ray.startPoint).magnitude();

                // 最小距離なら描画点に指定
                if (distance < minDistance)
                {
                    // 最小距離更新
                    minDistance = distance;

                    // 描画対象オブジェクトのインデックス更新
                    drawIdx = idx;

                    // 先に交点が代入されていたらメモリ解放する
                    if (intersectionPoint != nullptr)
                        delete intersectionPoint;
                    // 交点のメモリを確保してpointの中身をコピーする
                    intersectionPoint = new IntersectionPoint();
                    memmove(intersectionPoint, point, sizeof(IntersectionPoint));
                }
            }

            if (intersectionPoint != nullptr)
            {
                // 交点情報表示
                // recordLine("交点座標 = (%4.2f,%4.2f,%4.2f)\n",
                //            intersectionPoint->position.x, intersectionPoint->position.y, intersectionPoint->position.z);
                // recordLine("レイ始点 = (%4.2f,%4.2f,%4.2f)\n", ray.startPoint.x, ray.startPoint.y, ray.startPoint.z);
                // recordLine("レイ方向 = (%4.2f,%4.2f,%4.2f)\n", ray.direction.x, ray.direction.y, ray.direction.z);
                // recordLine("点光源 = (%4.2f,%4.2f,%4.2f)\n", pointLight.position.x, pointLight.position.y, pointLight.position.z);
                FColor phong = phongShading(*intersectionPoint, ray, pointLight, geometry[drawIdx]->material);
                Color color;
                color.r = phong.r * 0xff;
                color.g = phong.g * 0xff;
                color.b = phong.b * 0xff;
                drawDot(&bitmap, x, y, color);

                // drawDot(&bitmap, x, y, Color(0, 255, 0));
            }
            else
            {
                drawDot(&bitmap, x, y, Color(100, 149, 237));
            }
        }
    }

    recordLine("演算子の個数%ld\n", operationCount);

    // PNGに変換してファイル保存
    if (pngFileEncodeWrite(&bitmap, "raytracing_multiobj.png") == -1)
    {
        freeBitmapData(&bitmap);
        return -1;
    }

    freeBitmapData(&bitmap);

    finalLogFile();

    return 0;
}