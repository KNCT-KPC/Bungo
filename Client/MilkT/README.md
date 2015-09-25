# MilkT
「Sugar制約ソルバ」を用いたソルバ*―甘い 甘いの ミルクティー―*

## 説明

* 神戸大学による[Sugar](http://bach.istc.kobe-u.ac.jp/sugar/)を用いる
  * SATソルバにはGlueMiniSatを用いる
  * 神戸大学のチームかな？
  * もし本物の神戸大学チームが同じアプローチならまずい
* Linux環境でのみ動作検証済み
  * `popen`と改行コードをどうにかすれば、たぶん動く

## 準備

* Sugarのインストール
* のために、適切なSATソルバとか、JDKとか入れる
* `make install`する
* JDK8で`make`するには、[Makefileの修正](https://gist.github.com/lrks/46dab58e40734a43b43a)が必要

## ルールの構成

### マスの制約

* `(x1-1, y1-1) ~ (x2+1, y2+1)`に対して、`0 ~ N`が入る
* `0 ~ N-1`は普通の石、`N`は障害物
* `(int x_i_j 0 N-1)`とか

### 石の定義

* 各石にアンカーを設定する
* `(int y_i_j_n 0 1)`という、「石nのアンカーが(i,j)に配置すると1となる変数」を用意
* 回転などの操作を考慮しつつ、`(iff (= y_i_j_n 1) (一つだけ真 (and (ある操作についてのアンカーを基準としたブロックの配置)) ... ))`とかする

### 石は0回か、1回使う

* 全体で石nのアンカーは最大1個
* `(<= (+ y_0_0_n y_0_1_n ... y_I_J_n) 1)`

### 石の配置には順序がある

* 「石nが使われている」かつ「`x_i_j`に石mを配置」のとき、「石mの周りに、一つは石nがある」とする
* 「周り」は難しいので、実際は「石mの全てのブロックの4近傍に石nのブロックがある」とする
* ん...なんかおかしいな...
* あれ...

## 参考URL

* [パズルをSugar制約ソルバーで解く](http://bach.istc.kobe-u.ac.jp/sugar/puzzles/)
* [Syntax of Sugar CSP description](http://bach.istc.kobe-u.ac.jp/sugar/package/current/docs/syntax.html)
* SATubatuのREADME.mdに記載されたURLも参考にした

