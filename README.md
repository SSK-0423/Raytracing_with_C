# Raytracing_with_Cpp
並列処理の講義で実装した古典的なレイトレーシング  
2022/5/21現在,鏡面反射までの実装

## レンダリング例
![raytracing_sample1](https://user-images.githubusercontent.com/83057130/169647840-14f974e0-d222-4b75-942c-bd98ca2ca6c1.png)
![raytracing_sample2](https://user-images.githubusercontent.com/83057130/169647859-c2052fdf-6c2c-4dbf-8d73-ea1cfe319df7.png)

## 必要なライブラリ
- libpng

## 実行方法
実行を容易にするシェルスクリプトを用意したのでそれを使うといいと思います
```
bash raytracingShell.sh [実行するレイトレーシングプログラム]
例: raytracing_sample1.cppを実行したい
bash raytracingShell.sh raytracing_sample1
```
## 実装で参考にさせていただいたサイト
https://knzw.tech/raytracing/?page_id=1143
