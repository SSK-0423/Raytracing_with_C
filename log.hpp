/* デバッグ用ログファイル出力 */
#include <stdio.h>

// ログファイル生成
int initLogFile(const char*,const char*);

// ログファイルに記録
void recordLine(const char* format,...);

// ログファイルを閉じる
int finalLogFile();