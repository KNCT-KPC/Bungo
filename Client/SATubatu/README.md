# SATubatu
*「ドーモ、ミナ＝サン、SATubatuです」*―ネオオタノシケで自己紹介をするSATubatu！なんたることか！サツバツ！
その自己紹介は、ネオクシロシにまで響き渡り、キタオオドーリのシャッタードーリをホテルに変えた！ナムサン！

## 説明

* GlueMiniSATを用いたソルバ
* Linux環境でのみ動作検証済み
  * ファイルの読み書きだけで、特別なことはしていないので、Windowsでも動くと思うが…。

## GlueMiniSat

    $ # wget `https://sites.google.com/a/nabelab.org/glueminisat/のzip`
	$ unzip glueminisat-2.2.8.zip
	$ mv glueminisat-2.2.8/ /usr/local/src/
	$ cd /usr/local/src/glueminisat-2.2.8/
	$ ./build.sh
	$ ln -s /usr/local/src/glueminisat-2.2.8/binary/glueminisat-simp /usr/local/bin/glueminisat

## 参考URL

