# 賢い合体 第6段: 親の学習率と init の腕の学習率を分ける（設計）

2026-10-24（設計。新しい種はまだ走らせていない）。[brain-merge-stable.md](brain-merge-stable.md) の続き。

## 1. 分かっていること

- 第5段は親の学習率と B_LR（init の腕＝祖先から蒸留する腕の学習率。`#define B_LR LR`）を同時に 0.02→3e-3 にした。監査-73 の切り分け（種 2・3、`pk-scratch/a73/sep-summary.txt`）: 親 3e-3・B_LR 0.02 で S1・S2 とも緑
- **ただしその S1 の緑は init の腕の手柄ではない**。種 2 の S1 は pick の腕も init の腕も best_step=0、VAL 1.6518・TEST 1.0795 が両方の腕で全桁同じ。`st_merge_distill` は VAL が守りの pick より良くならなければ pick を返す（student.h）＝0.02 の init の腕は一度も pick を下回らなかった。種 3 の S1 も init の VAL が pick の出発と同じ 1.7722。S2 では init の腕も下がった（種 2: 2.6537→2.0023）が pick（1.9140）に届かない
- 第5段の頭（B_LR 3e-3）では 10 場面すべてで VAL が init を選び、S1 の 2 種で TEST は pick の方が良かった
- 今の出力は選ばれた腕の歩みしか出さない。init の腕は [sel] の行の init= の 1 つの値だけ

## 2. 決めたこと

- 親の学習率は 3e-3（常駐の脳の値）のまま。B_LR を独立の定数にし、`-DD5C_B_LR=<値>` で替えられる形にする（既定は第5段と同じ 3e-3＝既定の出力は変わらない）
- **B_LR は判定の種で選ばない**。候補は 3e-3（親と同じ）・0.02（第3段b を決めたときの値）・1e-3（pick の腕の蒸留の D_LR）の 3 つ。選びの種 6・7・8・9 で 3 つを回し、start=sel の門の緑の行が最も多い候補を選ぶ（同点は小さい学習率）。選びの数字も記録に残す
- 両方の腕の歩みを 1 行ずつ出す: `[walk <場面> pick|init] best_step=<n> val <出発> -> <返した子> fell_back=<0|1>`（試験だけ、g_ss から）
- SEL の NC を作り直す: SEL_INVERT は [d5c-sel-val] の行で判定する（2 つの VAL が違う場面では必ず赤になるはず）。SEL_PICK・SEL_INIT は「無理に選ばせた腕が VAL で負けた場面」があるときだけ噛める。無ければ「噛めない（片方がいつも勝つ）」と出し、噛んだとは数えない

## 3. 合否（コードより先に固定）

- 判定の種 10・11・12・13（まだ一度も走らせていない）で、選んだ B_LR の plain が [d5c-parents-ok] と start=sel の全部の門（s1-gain・s2-both・floor・determ・sel-val）で緑なら PASS。1 行でも赤なら赤
- NC: SEL_INVERT が判定の種のどれかの [d5c-sel-val] で赤。PARENT_LR_OLD が [d5c-parents-ok] で赤。SEL_PICK・SEL_INIT は噛めたかどうかを書く（噛めなければ「選ぶ規則の要否は示せない」）
- 種 0xC0FFEE・2〜5 は第5段で結果を見たので、選びにも判定にも使わない（出すだけ）

## 4. 入れる場所

試験だけ（student_distill_merge_test.c の B_LR と [walk] の行、run_st_distill_sel.sh の種・候補・NC の判定）。student.c は変えない＝crown は動かない。CI の外。

## 5. 見込み（外れてよい。線は動かさない）

0.02 は S1 で init が pick に戻り、選びは pick ばかりになって SEL_PICK は噛めない見込み。3e-3 は第5段と同じく S1 で VAL と TEST が食い違いうる。1e-3 は init の腕が祖先から遠くへ行けず S2 で弱い見込み。どれが選ばれても、判定の種で赤なら赤と書く。
