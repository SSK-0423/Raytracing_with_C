/*  数学系処理のヘッダ */

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

// 3次元ベクトル
struct Vector3
{
    float x, y, z;

    Vector3() {}
    Vector3(float x, float y, float z)
        : x(x), y(y), z(z) {}

    Vector3 operator-(Vector3 vec)
    {
        Vector3 ret(vec.x - x, vec.y - y, vec.z - z);
        return ret;
    }

    // 内積
    float dot(Vector3 vec)
    {
        return x * vec.x + y * vec.y + z * vec.z;
    }
};
