#include <math.h>
#include <malloc.h>
#include <memory.h>

#include "myPng.hpp"

// 点の描画
void drawDot(
    BitMapData* bitmap, unsigned int x, unsigned int y, Color color)
{
    // 指定座標のピクセル取得
    unsigned char* pixel = bitmap->pixelsData +
                          y * bitmap->width * bitmap->channel +
                          x * bitmap->channel;
    // 色変更
    if(bitmap->channel == COLOR_RGB){
        pixel[0] = color.r;
        pixel[1] = color.g;
        pixel[2] = color.b;
    }else if(bitmap->channel == COLOR_RGBA){
        pixel[0] = color.r;
        pixel[1] = color.g;
        pixel[2] = color.b;
        pixel[3] = color.a;
    }
}

int main()
{
    // ビットマップデータ初期化
    BitMapData bitmap(100, 200, 3);

    // ピクセルデータ確保
    bitmap.pixelsData = (unsigned char *)malloc(
        sizeof(unsigned char) *
        bitmap.width * bitmap.height * bitmap.channel);
    if (bitmap.pixelsData == NULL)
    {
        printf("malloc error\n");
        return -1;
    }

    // ビットマップデータの背景を白にする
    memset(bitmap.pixelsData, 0x00,
           bitmap.width * bitmap.height * bitmap.channel);

    // (10,20)に点を描画
    drawDot(&bitmap, 10, 20, Color(0x00, 0xff, 0xff));

    // PNGに変換してファイル保存
    if(pngFileEncodeWrite(&bitmap,"output.png") == -1){
        freeBitmapData(&bitmap);
        return -1;
    }

    freeBitmapData(&bitmap);

    return 0;
}