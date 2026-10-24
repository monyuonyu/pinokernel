# 脳の自己量子化 第5段: 同じバイト数で bit の置き場を自分で選ぶ・写しのずれの門（研究メモ）

2026-10-24（設計、実装はしない）。第4段 [brain-quant-embed.md](brain-quant-embed.md) の続き。

## 1. 仕組み（原文）

HAWQ-V2（Dong ほか, arXiv:1911.03852 §2.3、式 2.11）: 層ごとの bit を、Ω = Σ_i Tr(H_i)/n_i · ‖Q(W_i) − W_i‖² が最小の組で選ぶ（H_i は損失の重みについてのヘッセ行列、Tr は Hutchinson で推定）。原文も「最良の保証は無い」と書いている。student.c にはヘッセ行列とベクトルの積が無いので、**Tr(H)/n の代わりに勾配の 2 乗の平均（経験フィッシャー）**を使う（極小の近くでの近似。これは原文と違う点として書いておく）。

## 2. 決めたこと

S の行列: embed・out 各 16384、注意（wq・wk・wv・wo）2 層で 32768、FFN 98304。どの腕も「全部 2bit（seq）＋ちょうど 4096 B」:
- **E4**: embed 4bit（第4段）
- **E3O3**: embed・out を 3bit
- **A3**: 注意の 4 行列を 3bit
- **AUTO**: ノードが校正の窓（偶数の窓）で勾配の 2 乗を集め、上の 3 つから Ω 最小を選ぶ（held は見ない）

3bit は st_quant_fake と同じ格子（qmax = 2^(b−1)−1、[−qmax−1, qmax]）。st_quant_gptq_mix を bit を行列の種類ごとに持つ形に広げ、bits が {8,4,2} の場合は今と全バイト同じ。

**写しのずれの門**: 量子化した写しを「私の代わり」として受け入れるのは、門の窓（奇数の窓＝校正にも held にも使わない）での KL(fp32‖写し) ≤ **0.05** nats/位置のときだけ。held は報告だけ（held で重みを選ばない、held-independence.md）。KL で見るのは、dq-ef の「誤った輪も正答率では通る」限界のため（最上位の答えが同じでも分布は崩れうる）。

## 3. 合否（2026-10-24 に固定。種 1〜8、E3O3・A3・AUTO・門はどの種でもまだ走らせていない）

- **P0 バイト数**: E4・E3O3・A3 は seq＋4096 B ちょうど
- **P1 自分で選ぶ**: AUTO の B1 bpb の悪化 ≤ 3 つの腕の最小＋0.02。8 種すべて
- **P2 選ぶ物差しの効き**: Ω の順位で最下位の腕は、悪化が AUTO より大きい。8 種のうち 6 以上
- **D1 門**: E4 は受け入れ（第4段の KL 0.013〜0.025 から）、seq は拒む（0.098〜0.233）。8 種すべて。NOOP は KL 0 で受け入れ
- **D2 門と held の一致**: held の KL が 0.05 以下かどうかと門の判定の一致を数える（値だけ。合否にしない）
- 陰性コントロール: `-DST_QALLOC_INVERT`（Ω 最大を選ぶ）で P1 赤／`-DST_QALLOC_FLATFISHER`（勾配の重みを全部 1＝‖ΔW‖² だけ）は値だけ／`-DST_QUANT_BADSCALE` で D1 の E4 が拒まれる（赤）
- 事前の見込み: E4 が最小で AUTO も E4 を選ぶ（第4段で embed の床が主因）。見込みどおりなら P1 は自明に近い＝P2 と INVERT の赤が中身。外れたら外れたまま書く

## 4. 入れる場所

hosted だけ（student.c の量子化と試験 `tests/llm/student_qalloc_test.c`、cert は CI の外）。ベアは student_stub＝crown は動かない見込み、測る。門はまだ `student quant` に配線しない（次の段）。

## 出典

[1] Dong ほか, HAWQ-V2: Hessian Aware trace-Weighted Quantization of Neural Networks, arXiv:1911.03852（§2.3 式 2.11 と直後の段落、2026-10-24 に PDF を pdftotext で確かめた）
