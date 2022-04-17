#include <math.h>
#include <malloc.h>
#include <memory.h>

#include "myPng.h"

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
    bitmap.pixelsData = (unsigned char*)malloc(
        sizeof(unsigned char) * 
        bitmap.width * bitmap.height * bitmap.channel);
    if (bitmap.pixelsData == NULL)
    {
        printf("malloc error\n");
        return -1;
    }

    // ビットマップデータの背景を白にする
    memset(bitmap.pixelsData, 0xFF, 
        bitmap.width * bitmap.height * bitmap.channel);

    // (10,20)に点を描画
    drawDot(bitmap, 10, 20, Color(0x00, 0xff, 0xff));

    return 0;
}