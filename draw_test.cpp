#include <math.h>
#include <malloc.h>
#include <memory.h>

// 色
struct Color
{
    unsigned char r;
    unsigned char g;
    unsigned char b;

    Color(unsigned char red, unsigned char green, unsigned char blue)
        : r(red), g(green), b(blue) {}
};

// ピクセル構造体
struct Pixel
{
    Color color;
    void SetColor(Color newColor) { color = newColor; }
};

// ビットマップデータ
struct BitMapData
{
    unsigned int width;    // 幅
    unsigned int height;   // 高さ
    Pixel *pixelsData;     // ピクセルデータ
    unsigned char channel; // チャネル

    BitMapData(unsigned int w, unsigned int h, unsigned int ch)
        : width(w), height(h), channel(ch) {}
};


// 点の描画
void drawDot(
    BitMapData bitmap, unsigned int x, unsigned int y, Color color)
{
    // 指定座標のピクセル取得
    Pixel *pixel = bitmap.pixelsData + y * bitmap.width + x;

    // 色変更
    pixel->SetColor(color);
}

int main()
{
    // ビットマップデータ初期化
    BitMapData bitmap(1024, 768, 3);

    // ピクセルデータ確保
    bitmap.pixelsData = (Pixel *)malloc(
        sizeof(Pixel) * bitmap.width * bitmap.height);
    if (bitmap.pixelsData == NULL)
    {
        printf("malloc error\n");
        return -1;
    }

    // ビットマップデータの背景を白にする
    memset(bitmap.pixelsData, 0xFF, bitmap.width * bitmap.height);

    // (10,20)に点を描画
    drawDot(bitmap, 10, 20, Color(0x00, 0xff, 0xff));

    return 0;
}