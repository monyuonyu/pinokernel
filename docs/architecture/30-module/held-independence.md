# 評価の物差しの独立（ROADMAP 0-12）
<!-- vmodel rung: PARTIAL -->
<!-- vmodel certs: [held-indep] `run_held_indep.sh` -->

2026-10-12。held（確かめに使う保持データ）が、学習に使うデータと重ならないことを機械で確かめる。

## いま（測った）

- 赤ちゃん（student）の held は**位置だけ**で決まる。1つのコーパスを `seqlen` の窓に切り、前の 3/4 の窓で学び、残りの窓で held の損失を測る（`student_shell.c` の `heldout_loss`・`sleep_rounds`、`cradle.c` の `ct_heldout_loss`）。バイトを読むのは `(off+i) % n`。中身を比べるものは無かった。分け方の式（`total*3/4`、下限の補正つき）は `llm/` の中だけで11か所に手で写してある（student_shell.c 8・cradle.c 3）。
- `[held-indep]`（`tests/llm/run_held_indep.sh`、`arch/common/llm/held_indep.c`）: held の窓の中の 16 バイトの並び（k-gram）が、学習の窓の中にもあるかを数える。窓が末尾を越えて頭に戻る（wrap）数も数える。
- **正規の教材（`cradle_compose_canon`、[cradle-teach] の cert と生きた先生が送るバイト）は 170 個のうち 108 個が学習側と同じ**（2026-10-12、feat/held-indep 3e3370d0）。held の尾が学習側と同じ詰め物（`CT_FILLER`）だから。`run_cradle_live.sh` の頭の注も「held の多くは詰め物の丸暗記」と書いている。
- 教材は 1279 バイトで、本番の分け方（`n/seqlen` から）では学習 29 窓・held 10 窓、cert は 1280 から 30 窓・10 窓で計算している。**本番の held の最初の窓（928〜959）は cert では学習の窓**で、学習の文が入っている。`run_cradle_live.sh` の注の「train_end 960」は生きた側では 928。
- 道具の確かめ: 同じ塊の繰り返しは 170/170、でたらめなバイトは 0、短すぎるコーパス（n=100, seqlen=64）は wrap 1。

## 直し方（1段ずつ）

1. **（作った、e492a0b4、監査待ち）** 教材の held の尾を学習側に無い文（`CT_HELD_FILLER`）にし、長さをちょうど 1280 にした。測った（/build/hi2、2026-10-12）: I1 は 108/170→0/170、詰め物に戻すと 97/170 で赤。[cradle-teach] は前後とも PASS で、判定に使う試しの文の低下は全桁同じ 2.8609。**ただし held 全体の損失は、直す前 5.4979→2.8935（詰め物の丸暗記で下がって見えた）、直した後 5.5248→7.2894＝学習のあと、見たことの無い文ではでたらめ（ln256=5.545）より悪くなる**。試しの文の低下は、学んだ文と共有する短い句（「the zorblax 」「the blue fox」、どちらも 12 バイト）から来ている可能性がある（未確認）。**cradle_live（canon_probe は held 全体の平均）を直した教材で1回測った（/build/hi3、edbc59e2）: 赤**。教材なし（fixture だけ）の対照 2.8686、教えた腕 4.3823（でたらめな教材の腕 5.7357。監査-43 で腕の名を直した）、先生の死後 4.3609＝教えると、見たことの無い held の文ではむしろ悪くなる。直す前の緑は held の詰め物の丸暗記に支えられていた（直す前の版は今回は測っていない。D5-j の記録では緑）。監査-41（/build/a41c）が直す前の教材で緑（教えた腕 1.5813・対照 2.7727）、直した教材で赤（上と全桁一致）を測った。README 3か国語・Pages・run_cradle_live.sh の注は「重みは変わり先生の死後も残るが、知らない文では悪くなる（赤）」に直した。判定の線は動かしていない。
2. 分け方を1つの関数にまとめ（11か所の写しをやめる）、held が wrap するときは測らずに「測れない」と出す。**前半（作った、feat/held-split d054b9cf、監査待ち）**: `held_indep.h` の `st_held_split`（同じ式と下限、wrap する held の窓の数も返す）に11か所を置き換えた。`tests/llm/check_held_split.sh` は llm/ に 3/4 の手書きがあると赤（master は11行で赤）。tests/llm の中の14行の写しは読まない（限界）。**後半（作った、feat/held-wrap f77cc6a8、監査待ち）**: `st_held_measurable`（コーパスがあり、held の窓が wrap しない＝3窓以上）。wrap に届くのは2か所だけ: CT-2 の生きた cert（予算 1280 以上の文は 0 バイトに敷かれ、`corpus[x % 0]` で落ちていた＝I7、直す前は SIGFPE）は UNMEASURABLE で 1 を返す、`student` の seqlen 引数は3窓未満なら学習せずに断る（短い ring でしか起きず、試験は無い＝限界）。seqlen が固定の残り9か所は、cert の固定の長さ（約 1280）か、ring（4 窓以上＝CRADLE_MIN_LIVE、`_Static_assert` で固定）か fixture の上で測るので wrap しない。
3. `student` の表示と cert に `[held-indep] shared=…` を出し、CI に BLOCKING で入れる。
4. 残る: `TEACHER_FIXTURE`（自然文）の重なり、教材が入れ替わったときに前の教材で学んだ文が次の held に入ること、held の損失で重みを選ぶ（EMA・保存の間引き）と held はもう独立ではないこと。量子化の試験の `CORPUS` も同じ句を繰り返している。
   **前半（作った、feat/held-indep-4、監査待ち、2026-10-14）**: fixture は `teacher_fixture.h` に移した（バイトは同じ）。**I8: 正規の教材の held と、教材が届く前に学ぶ fixture の学習部分（0..3072）の重なりは最初から 0/170＝緑**（k=16。k=10 以上で 0、k=8 で 3/250、k=6 で 27/270＝短い語の重なりは残る）。入れ替わりは「この脳が起動から学んだバイトの記録」`st_seen`（16 バイトの並びのハッシュのブルームフィルタ、131072 ビット・3か所）で見る。多めに数えることはあるが少なく数えない。DMN の各回の始めと `student` の学習で、その回の学習部分を足す（同じ教材は1回だけ）。`student` に `[held-indep] seen=X/Y` の2行目。試験は空の関数で赤を見てから（I10 0/0）: I10 教材 A→B で B の held が A の学習部分のとき、B だけの検査は 0/170・記録は 170/170／I11 正規の教材は 0／I12 でたらめの held の誤検出 0/170（65536 ビットでは 2/170 で線ちょうどだったので倍にした）。本物の student_shell.c での `[held-seen]` S1〜S4（記録を止める `-DSEEN_DISABLE` で S2・S3 赤）。**限界**: 記録は起動からで、復元した重みの過去は知らない（表示で断る）。dlb の蒸留・`student merge` の学習は記録しない。held の損失で重みを選ぶことと量子化の `CORPUS` は残り。

## 確かめ方

- I1（正規の教材）が直す前は赤（108/170）・直した後は緑。
- 陰性コントロール: 道具そのものは I2（全部同じ）で赤くなること。教材の直しを戻すと I1 が赤に戻ること。
- 記録（4）: I9（seen を held そのものにすると全部、でたらめなら 0）、`-DSEEN_DISABLE` で `[held-seen]` が赤くならなければ run_held_indep.sh が赤。
