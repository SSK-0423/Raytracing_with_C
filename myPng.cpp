#include "myPng.hpp"

int pngFileReadDecode(BitMapData *bitmapData, const char *filename)
{
    FILE *file;
    unsigned int width, height;
    unsigned int readSize;

    png_structp png;
    png_infop info;
    png_bytepp datap; // 読みkんだ画像のピクセルデータ
    png_byte type;
    png_byte signature[8];

    file = fopen(filename, "rb");
    if (file == nullptr)
    {
        printf("%sは開けません\n", filename);
        return -1;
    }

    //
    readSize = fread(signature, 1, SIGNATURE_NUM, file);

    if (png_sig_cmp(signature, 0, SIGNATURE_NUM))
    {
        printf("png_sig_cmp error!\n");
        fclose(file);
        return -1;
    }

    png = png_create_read_struct(
        PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png == NULL)
    {
        printf("png_create_read_struct error!\n");
        fclose(file);
        return -1;
    }

    info = png_create_info_struct(png);
    if (info == NULL)
    {
        printf("png_create_info_struct error!\n");
        png_destroy_read_struct(&png, NULL, NULL);
        fclose(file);
        return -1;
    }

    png_init_io(png, file);
    png_set_sig_bytes(png, readSize);
    png_read_png(
        png, info, PNG_TRANSFORM_PACKING | PNG_TRANSFORM_STRIP_16, NULL);

    // 高さ幅取得
    width = png_get_image_width(png, info);
    height = png_get_image_height(png, info);

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
    printf("width = %d, height = &d, ch = %d\n",
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

    png = png_create_write_struct(
        PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    info = png_create_info_struct(png);

    if (bitmapData->channel == 3)
    {
        type = PNG_COLOR_TYPE_RGB;
    }
    else if (bitmapData->channel == 4)
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
    png_init_io(png, file);
    png_set_IHDR(
        png, info, bitmapData->width, bitmapData->height, 8, type,
        PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT);

    datap = (png_bytepp)png_malloc(png, sizeof(png_bytep) * bitmapData->height);

    png_set_rows(png, info, datap);

    for (int i = 0; i < bitmapData->height; i++)
    {
        datap[i] = (png_bytep)png_malloc(
            png, bitmapData->width * bitmapData->channel);
        memcpy(datap[i], bitmapData->pixelsData + i * bitmapData->width * bitmapData->channel,
               bitmapData->width * bitmapData->channel);
    }

    png_write_png(png,info,PNG_TRANSFORM_IDENTITY,NULL);

    for(int i = 0; i < bitmapData->height; i++){
        png_free(png,datap[i]);
    }
    png_free(png,datap);

    png_destroy_write_struct(&png,&info);
    fclose(file);
    return 0;
}

int freeBitmapData(BitMapData* bitmap){
    if(bitmap->pixelsData != nullptr){
        free(bitmap->pixelsData);
        bitmap->pixelsData = nullptr;
    }
    return 0;
}