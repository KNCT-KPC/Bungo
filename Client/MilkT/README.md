# MilkT
「Sugar制約ソルバ」を用いたソルバ*―甘い 甘いの ミルクティー―*

## 説明

* 神戸大学による[Sugar](http://bach.istc.kobe-u.ac.jp/sugar/)を用いる
  * SATソルバにはGlueMiniSatを用いる
  * 神戸大学のチームかな？
  * もし本物の神戸大学チームが同じアプローチならまずい
* Linux環境でのみ動作検証済み
  * fork と execlp さえどうにかすれば、他の環境でもたぶん動く

## 準備

* Sugarのインストール
* のために、適切なSATソルバとか、JDKとか入れる
* `make install`する
* JDK8で`make`するには、[Makefileの修正](https://gist.github.com/lrks/46dab58e40734a43b43a)が必要

## ルールの構成

* あとで書く

## 参考URL

* [パズルをSugar制約ソルバーで解く](http://bach.istc.kobe-u.ac.jp/sugar/puzzles/)
* [Syntax of Sugar CSP description](http://bach.istc.kobe-u.ac.jp/sugar/package/current/docs/syntax.html)
* SATubatuのREADME.mdに記載されたURLも参考にした

