# DeepReplaceSearch
DeepReplaceSearchは、DeepSearchとReplaceSearchの考え方を組み合わせたものです。詳しくはそれぞれのページを御覧ください。

DeepReplaceSearchはランク１の枝のみに対しReplaceSearchを行うため、さらに枝刈りの範囲が広がります。
大事なことは、**DeepSearchとReplaceSearchを同時に実行した場合とは結果が異なる**ということです。

# 総評
十分な時間が存在した場合、DeepReplaceSearchは、ReplaceSearchよりも更に解精度が劣ります。
しかし大会時の制限時間内では、「最も高い評価値を得た状態」を優先的に探し、
「解の改善に繋がらなさそうな場合は枝を打ち切る」このソルバは、高い性能を示しました。
ReplaceSearchと比べて一長一短があるので、併用することで保険をかけました。
