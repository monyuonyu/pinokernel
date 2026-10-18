# 脳の自己量子化 第2段: GPTQ を B1 と写しのずれで確かめる（研究メモ）

2026-10-18（#34-4）。第1段 `st_quant_gptq` の結果: G1・G2・G4 は種 1〜3 で緑、**G3 は種 2 で赤**（0.558 倍、種 1 0.378・種 3 0.476 倍）。線は動かさず CI の外。RTN は `brain-quantization.md`。

## 1. 何が効くのか（GPTQ の原文 [1] §3・§4・Algorithm 1・§5）

- 丸めるだけ（RTN）は各重みを別々に丸める。GPTQ は層の出力のずれ ‖WX−ŴX‖² を最小にする向きで、**1列丸めるたびに、その誤差を残りのまだ丸めていない列に配って打ち消す**（式(2)）。H = 2XXᵀ は層の入力 X だけで決まり、全部の行で同じ順に丸めてよい
- 実装は Algorithm 1 の形: H⁻¹ = (2XXᵀ+λI)⁻¹ の Cholesky の上三角を先に1回作り、列 j を丸め、誤差 E を [H⁻¹]_jj で割り、右の列を E·H⁻¹_{j,j:} で更新。**λ は H の対角の平均の 1%**（Step 3）
- 格子は「行ごとの min-max」で、比べる RTN は**同じ格子**（§5 Baselines）。前の層を量子化した X だと少し良い（§5）
- 小さいモデル・低いビットほど RTN との差が開く（`brain-quantization.md` の [2 表3]）

## 2. 脳のどこに入れるか

- `student.c`（hosted 専用＝crown は動かない）に `st_quant_gptq(src, dst, bits, calib, n)`。格子は第1段の `st_quant_group` と同じ行ごとの対称 absmax（**scale は補正の前の元の行から決める**）＝RTN との差は誤差の配り方だけ
- 対象は wq/wk/wv/wo・w1/w3/w2・out。X は forward の cache（a_in・attn_o・f_in・e_h・o_in、全部あった）から。embed は表引きで行列の積ではないので RTN のまま。専門家（MoE）の w1/w3/w2 は、その専門家に来た位置だけで H を作る。来た位置が 0 の専門家は RTN（その数はまだ出さない）
- 第1段は校正の X を fp32 の前の層から（簡単な方）。前の層を量子化した X は第2段の比べる腕
- 校正のバイトは fixture の学習部分だけ。**B1 の保持データ・DMN の held は使わない**

## 3. どう測るか（コードより先に固定）

cert `tests/llm/run_st_gptq.sh`（b1_held_test と同じ学習、S、種 3 つ）。RTN と GPTQ を同じ格子で、2bit 行・4bit 行:
- **G1 層の目的**: 校正の X で Σ‖WX−ŴX‖² が GPTQ < RTN（2bit・4bit、種 3 つすべて）。GPTQ が最小にしているもの＝仕組みそのものの確かめ
- **G2 写しのずれ**: B1 の保持データの全位置で、fp32 と量子化版の次のバイトの分布の KL の平均が GPTQ < RTN（2bit、種 3 つ）。正答率や bpb だけでは「誤った輪」も通った（dq-ef、監査-31）ので、ずれを主にする
- **G3 B1 の bpb**: 2bit 行で、fp32 からの悪化が GPTQ ≤ RTN の 2 分の 1（種 3 つ）。4bit は第1段で雑音の幅（±0.02）だったので値を出すだけ
- **G4**: 量子化した成分はすべて格子の上（第1段の (C) と同じ）。校正のバイトと B1 の保持データの 16 バイトの重なり 0
- **陰性コントロール**: `-DST_GPTQ_NOCOMP`（誤差を配らない＝順に丸めるだけ）で G1・G2 赤／`-DST_GPTQ_CALIB_HELD`（B1 の保持データで校正）で G4 赤／`-DST_QUANT_BADSCALE` で G4 赤
- 赤なら赤のまま書く
- **限界（監査-54）**: G1 は量子化に使った H で測る＝H の取り違え（wo を a_in の H で）は G1〜G4 とも緑

## 出典

[1] Frantar, Ashkboos, Hoefler, Alistarh, GPTQ: Accurate Post-Training Quantization for Generative Pre-trained Transformers, ICLR 2023, arXiv:2210.17323（PDF 4〜6 ページを画像で）
