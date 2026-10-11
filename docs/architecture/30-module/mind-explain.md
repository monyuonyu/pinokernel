# 心を説明するコマンド（ROADMAP 1-4、設計メモ）
<!-- vmodel rung: CLOSED -->
<!-- vmodel certs: [mind-explain] [mind-prov] `run_mind_explain.sh` `run_mind_provenance.sh` `ump-x86_64` -->

2026-10-04。外部レビュー §67「AI の記憶を人間が監査できるインターフェース」。最初の1つ `mind explain <鍵>` を作る。

## 何を出すか
- current: 今の答え（engram の値）。持っていなければ「none held」（教わっていない、または追い出した。重みは答えうるので `mind ask` を案内する）
- learned: 誰から（別のノードなら番号と名前、ここで教えたなら「taught here」）、prov が残っているか、ローカルの seq
- consolidated: PENDING／RETAINED と睡眠の回数、salience
- revision: 1-3a の履歴（`mc_hist`）のうち、この鍵の行。あわせて残っている数と落ちた数（「revised:」は `mind teach` の書き換えの行がすでに使っているので避けた）

## 決めたこと
- **読むだけ。** `mind ask` は投票（乱数を進める）・salience の加算（LM-13）・want の加算（LM-14）・DMN の起動・galaxy と内受容のイベントを起こす。監査のための読みが「何を忘れるか」を変えてはいけないので、explain はどれもしない。`m_quiesce` で睡眠の回の途中を読まないことだけはする
- **hosted だけ**（1-3a と同じ。`mind` はベアメタルのシェルに無く、crown は動かない）
- レビューの例の「current generation」は今は出さない。世代の定義（ROADMAP 1-5 の「今の版」）がまだ無いため

## `mind provenance`（2026-10-06）
- 持っている事実を全部、鍵ごとに1行: 今の値、教えた者（explain の learned: と同じ文。共通の `m_teacher_puts`）、ローカルの seq、PENDING／RETAINED。最後に教えた者の種類ごとの数
- 読むだけ（explain と同じ決まり）。`mind history`（学び・引き寄せ・改訂・忘却の時系列）は、出来事の記録そのものがまだ無いので作っていない（改訂だけなら `mind conflicts`）
- 試験 `tests/host/run_mind_provenance.sh`（5項目）。陰性コントロール `-DMIND_PROVENANCE_TOUCHES` で prov-readonly が赤。「別のノードから」の行は1台では出ない（試していない）

## どう測るか
- `tests/host/run_mind_explain.sh`（1台、網なし、CI の UMP x86_64）。8項目。読むだけの確かめは「explain を3回したあとの ask が salience=2」
- 陰性コントロール: `-DMIND_EXPLAIN_TOUCHES`（explain が salience を加算する）で explain-readonly が赤
