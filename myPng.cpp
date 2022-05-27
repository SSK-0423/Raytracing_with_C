#include "myPng.hpp"

int pngFileReadDecode(BitMapData *bitmapData, const char *filename)
{
    FILE *file;
    unsigned int width, height;
    unsigned int readSize;

    png_structp png;
    png_infop info;
    png_bytepp datap; // 読みこんだ画像データ
    png_byte type;    // 色の形式
    png_byte signature[8];  // シグネチャ

    // シグネチャの読み込み
    file = fopen(filename, "rb");
    if (file == nullptr)
    {
        printf("%sは開けません\n", filename);
        return -1;
    }
    readSize = fread(signature, 1, SIGNATURE_NUM, file);

    // シグネチャからPNGファイルかどうかを判定
    if (png_sig_cmp(signature, 0, SIGNATURE_NUM))
    {
        printf("png_sig_cmp error!\n");
        fclose(file);
        return -1;
    }

    // png_read構造体を生成
    // 第一引数:libpngのバージョン
    png = png_create_read_struct(
        PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png == NULL)
    {
        printf("png_create_read_struct error!\n");
        fclose(file);
        return -1;
    }

    // png_info構造体を生成
    info = png_create_info_struct(png);
    if (info == NULL)
    {
        printf("png_create_info_struct error!\n");
        png_destroy_read_struct(&png, NULL, NULL);
        fclose(file);
        return -1;
    }

    // PNGファイルとして読み込むファイルの設定
    png_init_io(png, file);
    // シグネチャサイズの設定
    png_set_sig_bytes(png, readSize);
    // PNGファイル読み込み
    /*
        PNG_TRANSFORM_PACKING：輝度値が1, 2, 4 ビットのデータの場合は 8 ビットのデータに変換する
        PNG_TRANSFORM_STRIP_16：輝度値が 16 ビットのデータの場合は 8 ビットのデータに変換する
    */
    png_read_png(
        png, info, PNG_TRANSFORM_PACKING | PNG_TRANSFORM_STRIP_16, NULL);

    // PNGから画像データ情報取得
    width = png_get_image_width(png, info);
    height = png_get_image_height(png, info);
    // PNGをBITMAPにデコードしたデータのアドレスを格納(ここでデコードが行われる)
    // 各行のデータへのアドレスへのポインタ⇨ダブルポインタ使用
    datap = png_get_rows(png, info);
    type = png_get_color_type(png, info);

    /* とりあえずRGBorRGBAだけ対応 */
    if (type != PNG_COLOR_TYPE_RGB && type != PNG_COLOR_TYPE_RGB_ALPHA)
    {
        printf("color type is not RGB or RGBA\n");
        png_destroy_read_struct(&png, &info, NULL);
        fclose(file);
        return -1;
    }

    bitmapData->width = width;
    bitmapData->height = height;
    if (type == PNG_COLOR_TYPE_RGB)
    {
        bitmapData->channel = 3;
    }
    else if (type == PNG_COLOR_TYPE_RGBA)
    {
        bitmapData->channel = 4;
    }
    printf("width = %d, height = %d, ch = %d\n",
           bitmapData->width, bitmapData->height, bitmapData->channel);

    // ピクセルデータのメモリ確保
    bitmapData->pixelsData = (unsigned char *)malloc(
        sizeof(unsigned char) *
        bitmapData->width * bitmapData->height * bitmapData->channel);
    if (bitmapData->pixelsData == nullptr)
    {
        printf("data malloc error\n");
        png_destroy_read_struct(&png, &info, NULL);
        fclose(file);
        return -1;
    }

    // 1行ずつデータコピー
    for (int i = 0; i < bitmapData->height; i++)
    {
        memcpy(
            bitmapData->pixelsData +
                i * bitmapData->width * bitmapData->channel, // 1行
            datap[i], bitmapData->width * bitmapData->channel);
    }

    // 生成した構造体削除
    png_destroy_read_struct(&png, &info, NULL);
    fclose(file);

    return 0;
}

int pngFileEncodeWrite(BitMapData *bitmapData, const char *filename)
{
    FILE *file;

    png_structp png;
    png_infop info;
    png_bytepp datap;
    png_byte type;

    file = fopen(filename, "wb");
    if (file == nullptr)
    {
        printf("%sは開けません\n", filename);
        return -1;
    }

    // png_write構造体生成
    png = png_create_write_struct(
        PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    // pig_info構造体生成
    info = png_create_info_struct(png);

    // 生成するPNGファイルの色情報設定
    if (bitmapData->channel == COLOR_RGB)
    {
        type = PNG_COLOR_TYPE_RGB;
    }
    else if (bitmapData->channel == COLOR_RGBA)
    {
        type = PNG_COLOR_TYPE_RGB_ALPHA;
    }
    else
    {
        printf("channel num is invalid!\n");
        png_destroy_write_struct(&png, &info);
        fclose(file);
        return -1;
    }

    // 書き込み先ファイルの設定
    png_init_io(png, file);

    // PNGのヘッダ設定
    png_set_IHDR(
        png, info, bitmapData->width, bitmapData->height, 8, type,
        PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT);

    // PNGにエンコードするBITMAPデータの設定
    // BITMAPデータ格納先のメモリ確保(行)
    datap = (png_bytepp)png_malloc(png, sizeof(png_bytep) * bitmapData->height);

    png_set_rows(png, info, datap);

    // 各行のメモリにBITMAPデータをコピー
    for (int i = 0; i < bitmapData->height; i++)
    {
        datap[i] = (png_bytep)png_malloc(
            png, bitmapData->width * bitmapData->channel);
        memcpy(datap[i], bitmapData->pixelsData + i * bitmapData->width * bitmapData->channel,
               bitmapData->width * bitmapData->channel);
    }

    // PNGへのエンコードとファイル書き込み
    png_write_png(png, info, PNG_TRANSFORM_IDENTITY, NULL);

    for (int i = 0; i < bitmapData->height; i++)
    {
        png_free(png, datap[i]);
    }
    png_free(png, datap);

    png_destroy_write_struct(&png, &info);
    fclose(file);
    return 0;
}

int freeBitmapData(BitMapData *bitmap)
{
    if (bitmap->pixelsData != nullptr)
    {
        free(bitmap->pixelsData);
        bitmap->pixelsData = nullptr;
    }
    return 0;
}

// 点の描画
void drawDot(
    BitMapData *bitmap, unsigned int x, unsigned int y, Color color)
{
    // 指定座標のピクセル取得
    unsigned char *pixel = bitmap->pixelsData +
                           y * bitmap->width * bitmap->channel +
                           x * bitmap->channel;
    // 色変更
    if (bitmap->channel == COLOR_RGB)
    {
        pixel[0] = color.r;
        pixel[1] = color.g;
        pixel[2] = color.b;
    }
    else if (bitmap->channel == COLOR_RGBA)
    {
        pixel[0] = color.r;
        pixel[1] = color.g;
        pixel[2] = color.b;
        pixel[3] = color.a;
    }
}