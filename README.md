# Bungo

## システム概要

* 3台のPCを使うことを前提
* 1台のPCを「サーバ」とする
  * これが運営サーバと通信
* 3台のうち、1台以上に「クライアント」を設置
  * クライアントは、「サーバ」と通信
  * PCの状況(CPU使用率など)に応じて、1台に複数のクライアントも設置
* クライアント・サーバ間の問題・回答送受信は、独自フォーマットにて行う
  * 余計な改行を省く
  * (C/C++を用いる)クライアントでパースがしやすいように
* クライアント・サーバ間は、競技開始前に接続しておく
  * 3-Way Handshakeの確立をうんぬん
  * クライアントは、サーバから応答があるまでブロック
  * サーバは、競技開始と同時にクライアントに問題を送信
  * 回答が来て、それがベストだったら運営サーバに報告

## ディレクトリ構成
このリポジトリのディレクトリ構成について示す。
~~~~
.
├── OfficialServer/
│   ├── Akane/
│   │   └── (模擬公式サーバ)
│   ├── Board/
│   │   └── (敷地情報を扱うユーザコントロール AkaneとHikariで利用)
│   └── Problem/
│        └── (問題情報を扱うクラス Akaneで利用)
├── KpcServer/
│   ├── Hikari/
│   │   └── (3台のPCを束ねるアレ)
│   └── NewProblem/
│        └── (OfficialServer/Problemの亜種 Hikariで利用)
├── Client/
│   └── (Hikariと通信して問題を解くソルバ)
└── Problem/
    └── (擬似問題)
~~~~

