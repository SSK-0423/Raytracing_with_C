#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "png.h"

enum COLORTYPE
{
    COLOR_RGB = 3,
    COLOR_RGBA,
};

// 色
struct Color
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;

    Color(
        unsigned char red, unsigned char green,
        unsigned char blue, unsigned char alpha = 1)
        : r(red), g(green), b(blue), a(alpha) {}
    Color() : r(0), g(0), b(0), a(0) {}
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
    unsigned int width;                  // 幅
    unsigned int height;                 // 高さ
    unsigned char *pixelsData = nullptr; // ピクセルデータ
    unsigned char channel;               // チャネル

    BitMapData() {}
    BitMapData(unsigned int w, unsigned int h, unsigned int ch)
        : width(w), height(h), channel(ch) {}

    // ピクセルデータ確保
    int allocation()
    {
        pixelsData = (unsigned char *)malloc(
            sizeof(unsigned char) * width * height * channel);
        if (pixelsData == NULL)
        {
            printf("malloc error\n");
            return -1;
        }

        return 0; // 成功
    }

    Color getPixelColor(unsigned int x, unsigned int y)
    {
        unsigned char *pixel = pixelsData + y * width * channel + x * channel;
        if (channel == COLOR_RGB)
            return Color(pixel[0], pixel[1], pixel[2]);
        if (channel == COLOR_RGBA)
            return Color(pixel[0], pixel[1], pixel[2], pixel[3]);
        else
            return Color(0, 0, 0);
    }
};

int pngFileReadDecode(BitMapData *, const char *);
int pngFileEncodeWrite(BitMapData *, const char *);
int freeBitmapData(BitMapData *);
void drawDot(
    BitMapData *bitmap, unsigned int x, unsigned int y, Color color);

#define SIGNATURE_NUM 8