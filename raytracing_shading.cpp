#include "raytracing_lib.hpp"
#include "log.hpp"

int main()
{
    // ログファイル初期化
    if (initLogFile("raytracing_shading.txt") == 1)
        return -1;

    // ビットマップデータ
    BitMapData bitmap(512, 512, 3);
    if (bitmap.allocation() == -1)
        return -1;

    // 描画オブジェクト
    // 球
    Sphere sphere = Sphere(Vector3(1, 0, 5), 1.f);

    // 視点の位置を決める
    Camera camera;
    camera.position = Vector3(0, 0, -5);

    // 点光源の位置を決める
    PointLight pointLight;
    pointLight.position = Vector3(-5, 5, -5);

    // 視線方向で最も近い物体を探し，
    // その物体との交点位置とその点での法線ベクトルを求める
    for (int y = 0; y < bitmap.height; y++)
    {
        for (int x = 0; x < bitmap.width; x++)
        {
            // レイを生成
            Ray ray = createRay(camera, x, y, bitmap.width, bitmap.height);
            // recordLine("視点:(%4.2f,%4.2f,%4.2f) 視線:(%4.2f,%4.2f,%4.2f),注視点:(%d,%d)\n",
            //            ray.startPoint.x, ray.startPoint.y, ray.startPoint.z,
            //            ray.direction.x, ray.direction.y, ray.direction.z, x, y);

            // レイと球の交点
            IntersectionPoint *intersectionPoint = sphere.isIntersectionRay(&ray);

            // レイと球が交差するか判定+交点があれば計算
            if (intersectionPoint != nullptr)
            {
                // 物体表面の光源の性質を使ってその点での色を決定する(シェーディング)

                // 球の法線ベクトルを求める
                Vector3 normal = intersectionPoint->normal;

                // 入射ベクトル計算(光が当たる点からみた光源の位置であることに注意)
                Vector3 incident = (pointLight.position - intersectionPoint->position).normalize();

                // 正反射ベクトル計算 r = 2(n・l)n - l
                Vector3 specularReflection = 2 * normal.dot(incident) * normal - incident;

                // 光源強度
                float Ii = 1.f;

                // ディフューズ(拡散反射光)
                float kd = 0.69; // 拡散反射係数
                float diffuse = Ii * kd * normal.dot(incident);
                if (diffuse < 0)
                    diffuse = 0;
                // recordLine("ディフューズ%f\n", diffuse);

                // スペキュラー
                float ks = 0.3; // 鏡面反射係数
                float a = 8;    // 光尺度

                // 鏡面反射係数 * 光源強度 * 視線逆ベクトル・入射光の正反射ベクトル
                Vector3 inverseEyeDir = ((-1) * ray.direction).normalize();
                float specular = ks * Ii * myPow(inverseEyeDir.dot(specularReflection), a);
                // 視線逆ベクトルと正反射ベクトルの内積もしくは，
                // 物体面の法線ベクトルと入射ベクトルの内積が負数の場合，
                // 鏡面反射は「0」になる
                if (inverseEyeDir.dot(specularReflection) < 0 || normal.dot(incident) < 0)
                    specular = 0;

                // 環境光
                float Ia = 0.1f;  // 環境光の強度
                float ka = 0.01f; // 環境光反射係数
                float ambient = Ia * ka;

                float I = diffuse + specular + ambient;

                Color color;
                color.r = I * 0x00;
                color.g = I * 0xff;
                color.b = I * 0x80;

                drawDot(&bitmap, x, y, color);
            }
            else
            {
                drawDot(&bitmap, x, y, Color(100, 149, 237));
            }
        }
    }

    recordLine("演算子の個数%ld\n", operationCount);

    // PNGに変換してファイル保存
    if (pngFileEncodeWrite(&bitmap, "raytracing_shading.png") == -1)
    {
        freeBitmapData(&bitmap);
        return -1;
    }

    freeBitmapData(&bitmap);

    finalLogFile();

    return 0;
}