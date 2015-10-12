# Bungo

## ディレクトリ構成
このリポジトリのディレクトリ構成について示す。
~~~~
.
├── OfficialServer/
│   ├── Akane/
│   │   └── (模擬公式サーバ)
│   ├── Board/
│   │   └── (敷地情報を扱うユーザコントロール AkaneとHikariで利用)
│   ├── Problem/
│   │   └── (問題情報を扱うクラス Akaneで利用)





│   ├── Akane/
│   ├── Akane/



├── README.md
├── bower.json
├── gulpfile.js
├── package.json
├── shared
│   ├── img
│   │   ├── page
│   │   ├── site
│   │   └── sprite
│   ├── jade
│   │   ├── inc
│   │   │   ├── core
│   │   │   │   ├── _base.jade
│   │   │   │   ├── _config.jade
│   │   │   │   └── _mixin.jade
│   │   │   ├── layout
│   │   │   │   ├── _footer.jade
│   │   │   │   └── _header.jade
│   │   │   └── module
│   │   ├── index.jade
│   │   └── setting.json
│   ├── js
│   │   └── src
│   │       └── app.js
│   └── scss
│       ├── core
│       │   └── _mixins.scss
│       ├── layout
│       │   ├── _footer.scss
│       │   ├── _header.scss
│       │   └── _layout.scss
│       ├── module
│       ├── style.scss
└── styleguide
    ├── template
	    │   ├── index.html
		    │   ├── public
			    └── styleguide.md
				~~~~










### OfficialServer

#### Akane
* 模擬公式サーバ

#### Board
* 敷地情報が載るユーザコントロール
* 後述する「Hikari」でも利用

#### Problem
* 問題ファイルのパースを行うクラス
* 後述する「Hikari」でも利用


### KpcServer

#### Hikari

* 「メインマシンのプログラム」


### Client

#### Sample

* サンプル用

#### その他

* いろんな人が作ったソルバ


### Problem

* 練習問題

