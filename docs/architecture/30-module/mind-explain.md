# 心を説明するコマンド（ROADMAP 1-4、設計メモ）

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

## どう測るか
- `tests/host/run_mind_explain.sh`（1台、網なし、CI の UMP x86_64）。8項目。読むだけの確かめは「explain を3回したあとの ask が salience=2」
- 陰性コントロール: `-DMIND_EXPLAIN_TOUCHES`（explain が salience を加算する）で explain-readonly が赤
