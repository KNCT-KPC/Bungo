# SAT/SMTソルバは無理

* 無理だわこれ
  * 少なくとも、一時ファイルを書き出すような方法だと
* 4x4なら解ける
* 以下、挫折までの記録

# MaxSchultz == z3 == danke
Guten Morgen! わたし、折部やすな！たぶん高校生！

## 説明

* [z3](https://github.com/Z3Prover/z3)を用いる

## 準備

### z3のインストール

    $ git clone https://github.com/Z3Prover/z3.git
    $ cd z3
    $ python src/mk_make.py
    $ cd build;
    $ make -j hoge
    $ sudo mkdir /usr/lib/python3.4/site-packages/__pycache__
    $ sudo make install

## ルールの構成

* マスの定義
  * MilkTと同様
* ブロックの定義
  * 「ある石を使うか否か」の変数を用意した
  * それを`==`で`onlyone`のように
  * いけるか分からない
  * たぶんこれはいけない
* ブロックの配置
  * 下記参照

## ブロックの配置について考えた
考えただけ

    0 1 ......... 1の周囲は0
    0 -1 2 ...... 2の周囲は0
    0 -1 -2 3 ... 3の周囲は0
    ----
    1 2 ......... 2の周囲は1
    1 -2 3 ...... 3の周囲は1
    ----
    2 3 ......... 2の周囲は2

使われることはなかった

