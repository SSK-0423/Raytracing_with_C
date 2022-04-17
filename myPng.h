#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "png.h"

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

int pngFileReadDecode(BitMapData*, const char*);
int pngFileEncodeWrite(BitMapData*,const char*);
int freeBitmapData(BitMapData*);

#define SIGNATURE_NUM 8