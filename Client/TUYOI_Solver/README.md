# ソルバまつり
このフォルダには、本戦で使用されたソルバたちが暮らしています。
ただし、保険で動かされていた物もあるので、性能が良いとは限りません。

# 大会当日の様子
## 予行演習
 PC1 : DeepReplace-zero, Replace-zero, Deep-zero, Breadth-zero
 
 PC2 : DeepReplace, Replace, Deep, Breadth
 
 PC3 : （よくわかんないソルバ）
 
## 第１回戦
 PC1 : DeepReplace-zero, Replace-zero, Deep-zero, Breadth-zero
 
 PC2 : DeepReplace, Replace, Deep, Breadth
 
 PC3 : DeepReplace-reverse, Replace-reverse, Deep-reverse, Breadth-reverse（全部バグあり）＋KanaC
 
**<<突然の死！>>** → reverseがクビに。Focus、DeepFocus、Omikujiの開発

## 敗者復活戦
 PC1 : DeepFocus-zero, Focus-zero, Deep-zero, Breadth-zero
 
 PC2 : DeepFocus, Focus, Deep, Breadth
 
 PC3 : Omikuji, Omikuji, Omikuji ＋ KanaC

## 準決勝
 PC1 : DeepFocus-zero, Focus-zero, Omikuji, Omikuji
 
 PC2 : DeepFocus, Focus, Omikuji, Omikuji
 
 PC3 : Omikuji, Omikuji, Omikuji, Omikuji
 
## 決勝
 PC1 : DeepFocus-zero, Focus-zero, Omikuji, Omikuji
 
 PC2 : DeepFocus, Focus, Omikuji, Omikuji
 
 PC3 : Omikuji, Omikuji, Omikuji, Omikuji
 
# Q.なぜ第一回戦で負けたんですか？
A. reverse機能（笑）をつけたのが原因です。

reverse機能は、後ろの糞（ズク）から順番に配置していく機能です。
前からだけでなく、後ろからも探索を行うことで探索範囲を広げようと考えました。

「後ろからでも、番号が連続していれば大丈夫だろ」

**もちろんそんなことは無く**、以下のような配置エラーが発生します。（7はそれよりも若い番号と連続していないのでアウト）

9 9 9

9 7 8

8 8 8


そして、以下のような幸運が重なり突然の死を迎えました。

**① 以上のエラーがHikari側では検出されなかった**

**② エラーが発生した場合、それまでの正しい解が全て無視されるルールだった**

**④ reverse機能をつけたソルバが、通常のソルバよりも良い解を見つけてきた**

**③ エラーが発生した場合でも、Hikariは最良解としてその解答を保存していた**

**⑤ その解答が、reverse機能が有効になっているソルバのものかどうか、確かめることができなかった**

なにより、**reverse機能をつけたのが当日の朝だった**ことが大きいと思います。
そもそもこんなエラー、**やる前から気づけよ**って僕も思います。大会時の僕は思っていなかったみたいです。

# おわりに
大会の結果は**ベスト９**（64チーム中）でした。

「探索の方法」自体は僕の経験則から良いものが作れたと思います。
しかし、探索の順序を決めるための評価指標・評価方法が練り込み不足で、上位と差を付けられてしまったと考えています。

後輩には「ちゃんと十分な期間を設けて開発を行うこと」「大会前日・当日の徹夜はしないこと」「テストはしっかりやりこむこと」などを伝えたいと考えていますが、僕も先輩からそんなことを聞かされた記憶があるので、多分無駄だと思います。
