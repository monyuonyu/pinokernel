# 賢い合体 第3段: 出力で合体する（蒸留）

2026-10-06（§3 の診断・§4 は10-07）。[brain-merge.md](brain-merge.md) の続き。第2段で「揃えずに重みを平均する道」は閉じた。

## 1. 何が効くのか（原文から）

- **FedDF** [1 §3・Alg.1]: 親たちの logit の平均を先生にし、子を KL(σ(平均)‖σ(子)) で学ばせる。蒸留の文はラベル無しでよい（生成器の出力でも）。Adam 1e-3 [1 §4.1]。出力は重みの並びに依らないので山を越えられる
- **係数探し（Sakana AI）** [2 §1.1.3・§2]: 層ごとの TIES＋DARE の係数を CMA-ES で探す。前提は「同じ親から微調整した子孫」。赤子は初期値から 34% 離れ山がある＝満たさない → 同じ心の世代どうし（Path W）に回す
- 先生の生成文で蒸留する形 [3]

## 2. 脳のどこに入れるか

- `st_backward_soft`: st_backward の (p − onehot) を (p − q) にした版。student.c は hosted だけ＝crown は動かない
- `st_merge_distill(..., start, ...)`: 先生 = 2親の logit の平均（温度 1）、蒸留の文 xfer で steps 歩、検証で一番良いものを残す。安全網: 検証で `st_merge_guarded` の選んだもの（第1段）より悪ければそれを返す。出発点は guarded の選んだもの（第3段）か共通の祖先 init（第3段b）
- 蒸留の文: **T-real**（学習の窓、参考）と **T-gen**（2親が生成した文。本命）
- 全体の決まり1: これは「重みの合体」。事実の食い違いは解決しない

## 3. 第3段の結果と監査-25 の診断

ゲート: [d5c-s1-gain] S1（同じデータ・順番違い）の試験で 子 ≤ 良い方の親 − 0.03。[d5c-s2-both] S2（データ半分ずつ）の両半分で同じ、かつ各半分で 子 ≤ 中点。[d5c-floor] 子 ≤ 悪い方。[d5c-determ] 2回で全桁同じ。

第3段（出発点 = guarded、400歩、1e-3）: S2 は T-real・T-gen とも緑、S1 は赤（子 = A のまま）。

診断（監査-25）: A から始めると、先生を B 1体（同じ大きさ＝表せる）にしても子は悪くなる。共通の祖先から始めれば吸える。**障害は「先生が平均であること」ではなく「訓練済みの盆地 A から始めること」**。「小さな子は平均を吸えない」は誤り。ただし親は学習不足（祖先からの通常の学習を伸ばすとさらに下がる）ので、比べる相手に同じ歩数の通常の学習が要る。

## 4. 第3段b（結果を見る前に固定、2026-10-07）

- 出発点 = init。歩数 = 親1体の学習と同じ（ROUNDS×学習の窓）、学習率 = 親と同じ 0.02（Adam）。「親と同じ手順で、目標だけ先生に替える」で、自由な値を残さない。監査-25 が試験を見た 5e-3・2000歩 は使わない
- §3 の4つのゲートを init の腕にもかける。加えて [d5c-b-plain]: T-real で 子 ≤ 同じ祖先から同じ歩数・学習率で正解ラベルを学んだもの（S1 は試験、S2 は両半分）。T-gen は生のデータを使わないので表示だけ
- 陰性コントロール（rc ではなく狙いの行で判定）: ONE_TEACHER（先生 = A だけ）と RAND_XFER（でたらめな文）→ どちらも init の [d5c-s2-both] が赤
- 第3段（guarded から）の S1 の赤は、既知の赤として記録に残す

## 出典
[1] Lin, Kong, Stich, Jaggi, Ensemble Distillation for Robust Model Fusion in Federated Learning, NeurIPS 2020, arXiv:2006.07242
[2] Akiba, Shing, Tang, Sun, Ha（Sakana AI）, Evolutionary Optimization of Model Merging Recipes, arXiv:2403.13187
[3] Kim, Rush, Sequence-Level Knowledge Distillation, EMNLP 2016, arXiv:1606.07947
