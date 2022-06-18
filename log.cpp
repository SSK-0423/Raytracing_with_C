#include "log.hpp"
#include <stdarg.h>

static FILE *logFile;

int initLogFile(const char *filename, const char* option = "W")
{
    logFile = fopen(filename, option);
    if (logFile == NULL)
    {
        printf("ログファイル%sをオープンできませんでした\n", filename);
        return -1;
    }

    return 0;
}

void recordLine(const char *format, ...)
{
    va_list va;
    va_start(va, format);
    vfprintf(logFile, format, va);
    va_end(va);
}

int finalLogFile()
{
    int result = fclose(logFile);
    if (result == EOF)
    {
        printf("ログファイルのクローズに失敗しました\n");
        return -1;
    }

    return 0;
}