# Sample

* Windows / Linux に対応したクライアントサンプル
* `base.c`は使いまわすことを想定
  * `#include "base.c"`とかいう豪快な仕様
  * それがどうしても嫌で、オブジェクトファイルも作りたくなくて、Socketも扱いたくない場合、`base.c`にfdopenの例があるので参考に
* `sample.c`をコピーして、`int solver()`を弄ってくれ

## Linux でのコンパイル

   gcc sample.c

## Windows でのコンパイル

   cl sample.c
   # とか、ゔぃじゅあるすたじおでどうにかなるでしょう？
