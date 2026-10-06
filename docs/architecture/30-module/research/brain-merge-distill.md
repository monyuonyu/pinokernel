# 賢い合体 第3段: 出力で合体する（蒸留）

2026-10-06。[brain-merge.md](brain-merge.md) の続き。第2段で「揃えずに重みを平均する道」は閉じた（山 0.3〜0.7）。

## 1. 何が効くのか（原文から）

- **FedDF** [1 §3・Alg.1]: 親たちの出力（logit）の平均を先生にし、子を KL(σ(平均 logit) ‖ σ(子)) で学ばせる。子の初期値は親の重みの平均。蒸留のデータはラベル無しで、別の分野のデータや生成器の出力でもよい [1 §3]。Adam 1e-3、検証が 1e3 歩伸びなければ止める [1 §4.1]。出力は重みの並びに依らないので、山を越えられる
- **係数探し（Sakana AI）** [2 §1.1.3・§2.1]: 層ごとの TIES＋DARE の係数を CMA-ES で探す。重みの空間の合体は「同じ親から微調整した子孫」に限る、と本文にある。赤子は初期値から 34% 離れ山がある＝前提を満たさない。層の並べ替え（§2.2）も別々の残差の座標を倍率 W でつなぐだけで、揃っていない赤子には効きにくい
- 先生の生成した文で蒸留する形 [3]

**選んだもの: 蒸留。** 係数探しは共通の祖先がある「同じ心の世代どうし」（Path W・delta-quant）に回す。

## 2. 脳のどこに入れるか

- `st_backward_soft(m, bytes, n, q)`: st_backward の頭の (p − onehot) を (p − q) にした版。st_backward の結果は変えない（ss1/ss3 の札で確かめる）。student.c は hosted だけ＝crown は動かない（実測で）
- `st_merge_distill(into, peer, init, scratch, xfer, nx, val, nv, seqlen, steps, lr, stats)`: (1) 初期値は `st_merge_guarded` の選んだもの（平均が良ければ平均）(2) 先生 = 2親の logit の平均、温度 1 (3) 蒸留の文 xfer で steps 歩 (4) 検証で子が初期値より悪ければ初期値に戻す（第1段の安全網を残す）
- 蒸留の文は2つの腕: **T-real**（学習の窓、参考）と **T-gen**（2親が自分で生成した文。生のデータを渡さない、本命）
- 全体の決まり1: これは「重みの合体」（振る舞いを近づける）。事実の食い違いは解決しない

## 3. どう測るか（期待値は先に固定。探りの数字は d5c3-probe、2回同じ）

探り（cert と同じ赤子・窓）: 先生（logit 平均）は良い方の親より良い。S1（同じデータ・順番違い）試験 A 2.191 / B 2.079 / 先生 1.963。S2（データを半分ずつ）両半分 A 2.445 / B 2.000 / 先生 1.906、B の半分は B 1.743・A 2.826。

- [d5c-s1-gain] S1 の試験で 子 ≤ 良い方の親 − 0.03
- [d5c-s2-both] S2 の両半分で 子 ≤ 良い方の親 − 0.03、かつ各半分で 子 ≤ 2親の中点
- [d5c-floor] 子 ≤ 悪い方の親（どの腕でも）、[d5c-determ] 2回で全桁同じ
- 陰性コントロール: `-DST_DISTILL_ONE_TEACHER`（先生が親 A だけ → s2-both が赤）、`-DST_DISTILL_RAND_XFER`（蒸留の文がでたらめなバイト → gain が赤）
- T-gen が赤ならそのまま書く（T-real だけ緑なら「生のデータを渡せば効く」まで）

## 出典
[1] Lin, Kong, Stich, Jaggi, Ensemble Distillation for Robust Model Fusion in Federated Learning, NeurIPS 2020, arXiv:2006.07242（PDF §3・§4.1）
[2] Akiba, Shing, Tang, Sun, Ha（Sakana AI）, Evolutionary Optimization of Model Merging Recipes, arXiv:2403.13187（PDF §1.1.3・§2）
[3] Kim, Rush, Sequence-Level Knowledge Distillation, EMNLP 2016, arXiv:1606.07947（要旨）
