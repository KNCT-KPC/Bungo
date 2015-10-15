﻿# ReplaceSearch
Replaceは「再配置」の意味で、BreadhSearchの改良版となります。ここら辺から、命名がノリで決まっていきます。
このソルバはBreadhSearchをベースにしているので、詳しいことはBreadhSearchのページを御覧ください。

BreadhSearchとの違いは、枝の終端に一定回数以上到達しているのにも関わらず、解の改善がない場合、枝の根本から探索を打ち切ってしまうことです。

# 総評
解の改善が見込めない探索を打ち切ることで、BreadthSearchやDeepSearchのような極端に偏った探索を防げます。
ただし、猛烈な勢いで枝刈りを行うため、DeepSearchよりも解精度は劣ります。
枝刈りをする到達回数を多くすると、解精度が向上しますが、解探索の速度が大きく低下することになります。
そこで、DeepSearchとの併用や、2つの特徴を組み合わせたDeepRepalceSearch、さらに改善手法としてFocusSearchなどの開発を行いました。