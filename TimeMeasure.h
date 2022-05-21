#define MPI

// 時間計測用構造体 
struct ProcessTime
{
    double totalTime = 0.f;
    double raytraceTime = 0.f;
    double encodePngTime = 0.f;
    double shadowingTime = 0.f;
    double intersectionTime = 0.f;

    void Init(){
        totalTime = 0;
        raytraceTime = 0;
        shadowingTime = 0;
        encodePngTime = 0;
        intersectionTime = 0;
    }
};

// 時間計測用のグローバル変数
static ProcessTime evaluateTime;