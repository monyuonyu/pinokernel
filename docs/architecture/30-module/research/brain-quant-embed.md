# 脳の自己量子化 第4段: embed に bit を回す・同じバイト数で比べる（研究メモ）

2026-10-23（研究の調べる回、実装はしない）。第3段 `brain-quant-gptq-seq.md` の監査-63 で「改善の大半は embed を知った H」と分かった。次の段を決める。

## 1. 分かっていること

- 原文 [1] は embed と出力の層を量子化していない（Practical Speedups の段落「embeddings and the output layer, which are kept in full FP16 precision」）。p-kernel の S では embed と out が行列の重みの約 2 割で、2bit の悪化の主因（第3段 §1: out 0.46〜0.54・embed 0.12〜0.26 bpb）
- out は GPTQ で 0.05〜0.09 倍まで縮む。embed は表引きで GPTQ が触れず RTN のまま＝残った床
- **embed と out は同じ形（V×D＝256×64、行の scale 256 個）**＝片方だけ 4bit にすると、増えるバイト数は 1 バイトも違わない（+4096 B）

## 2. 決めたこと

「embed の丸めを良くする」を、「同じバイト数どうしの比較」で判定する（両方を 1 段で）。bit を回す先が embed で正しいかは、同じ 4096 B を out に回した腕と比べないと言えない。

腕（どれも S、第3段と同じ学習・校正、2bit の残りは seq の GPTQ）:
- **seq**: 今の master（全部 2bit）
- **E4**: embed だけ 4bit の RTN。H は 4bit の embed を通した写しから集める
- **O4**: out だけ 4bit の GPTQ、embed は 2bit の RTN（E4 と同じバイト数）
- 値だけ: embed fp32（原文の形）、embed 2bit の G32（+1024 B）

作るもの: `st_quant_gptq_mix(src, dst, bits, bits_embed, bits_out, …)`（hosted だけ、crown は動かない見込み＝測る）。`st_quant_gptq` は 3 つとも同じ bit の場合と全バイト同じであること。

## 3. 合否（2026-10-23 に固定。種 1〜8、E4・O4 はどの種でもまだ走らせていない）

- **Q0 バイト数**: bytes_q(E4) = bytes_q(O4) = bytes_q(seq) + 4096、ちょうど
- **Q1 同じバイト数**: B1 の bpb の悪化が E4 < O4 − 0.02（第1段の雑音の幅）、かつ写しのずれ KL が E4 < O4。8 種すべて
- **Q2 足す価値**: 悪化が E4 < seq。8 種すべて（比は値だけ）
- G1・G4 は第3段のまま（embed は 4bit の格子の上）。種 1 を 2 回走らせて全バイト同じ
- 陰性コントロール: `-DST_QMIX_SWAP`（E4 の腕が O4 を作る）で Q1 赤／`-DST_QMIX_EMB8`（embed 8bit）で Q0 赤／`-DST_QUANT_BADSCALE` で G4 赤／`-DST_QMIX_EMBH2`（embed は 4bit で持ち、H は 2bit の embed で集める）は値だけ
- 事前の見込み: Q1 は緑（out は GPTQ で既に小さい）。見込みどおりでも、意味は「4 KB あたり何 bpb」の数字の方。赤なら赤のまま書く

## 4. 結果（2026-10-23、監査-66 限界つき PASS）

cert ALL PASS、種 1〜8 の 2bit の悪化（bpb）: seq 0.16〜0.33、**E4 0.006〜0.036**、O4 0.13〜0.29（種 4 は O4 0.225＞seq 0.222）。embed fp32 は 0.011〜0.041（E4 より悪い種が 5/8、説明なし）。NC は狙いどおり赤。EMBH2 の E4 は 0.037〜0.125（種 4 は差なし）。監査で全桁再現、壊し方 OUTIGN（out が 2bit のまま）は G1・G4、EMBNOP（embed を作り直さない）は Q1・Q2 で赤・**G4 は緑**＝Q0 は式で数えるだけ。ログ pk-scratch/qmix-cert-1023.log・a66-out/

## 出典

[1] Frantar ほか, GPTQ, ICLR 2023, arXiv:2210.17323（実験の節の Practical Speedups の段落、2026-10-23 に PDF を pdftotext で確かめた）
