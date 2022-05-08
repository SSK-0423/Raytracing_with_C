/*  数学系処理のヘッダ */

// 演算子の個数を数える
static unsigned long long operationCount = 0;

// 解の公式の解を指定する
enum SOLUTION
{
    FIRST_SOLUTION, // 解の公式の1つ目の解
    SECOND_SOLUTION // 解の公式の2つ目の解
};

// 判別式計算
float calcDiscriminant(float, float, float);

// 解の公式を計算
float calcQuadraticFormula(float, float, float, SOLUTION);

// 平方根
float mySqrt(float);

// べき乗
float myPow(float, int);

// 絶対値
float myAbsf(float);

// [0〜1]の一様乱数生成
float myRand();

// 3次元ベクトル
struct Vector3
{
    float x, y, z;

    Vector3() {}
    Vector3(float x, float y, float z)
        : x(x), y(y), z(z) {}

    // 左オペランドがVector3ではないので，フレンド関数として定義
    // 3個
    friend Vector3 operator*(float n, Vector3 vec);

    // 3個
    Vector3 operator+(Vector3 vec)
    {
        operationCount += 3;
        return Vector3(vec.x + x, vec.y + y, vec.z + z);
    }

    // 3個
    Vector3 operator-(Vector3 vec)
    {
        operationCount += 3;
        Vector3 ret(x - vec.x, y - vec.y, z - vec.z);
        return ret;
    }

    // 内積
    float dot(Vector3 vec)
    {
        operationCount += 5;
        return x * vec.x + y * vec.y + z * vec.z;
    }

    // 外積
    Vector3 cross(Vector3 vec)
    {
        Vector3 ret;
        ret.x = y * vec.z - z * vec.y;
        ret.y = z * vec.x - x * vec.z;
        ret.z = x * vec.y - y * vec.x;

        return ret;
    }

    // 大きさ
    float magnitude()
    {
        return mySqrt(x * x + y * y + z * z);
    }

    // 正規化
    Vector3 normalize()
    {
        float mag = magnitude();
        operationCount += 3;
        return Vector3(x / mag, y / mag, z / mag);
    }
};