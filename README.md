# Raytracing_with_Cpp
__並列処理の講義で実装した古典的レイトレーシング__
## 現在の実装状況
- [x] フォンの反射モデル
- [x] シャドウイング
- [x] 完全鏡面反射
- [ ] 屈折
- [ ] フォンの反射モデル⇨拡散反射
## レンダリング例
- raytracing_sample1.cpp  
![raytracing_sample1](https://user-images.githubusercontent.com/83057130/169650604-9a6decba-0733-4633-ac67-71647f2fde8a.png)
- raytracing_sample2.cpp  
![raytracing_sample2](https://user-images.githubusercontent.com/83057130/169650618-2d90c0c9-3c0d-40b7-af03-8a9f899c20ae.png)
## 実行方法
シェルスクリプトを用意したのでそれを使う
```
bash raytracingShell.sh [実行したいレイトレーシングプログラム]
```
### 使用例
raytracing_sample1.cppを実行したいとき
```
bash raytracingShell.sh raytracing_sample1
```
## 注意
PNG画像の出力で __libpng__ を使用しているので導入をお願いします.
## 実装で参考にさせていただいたサイト
https://knzw.tech/raytracing/?page_id=1143 東京電機大学
