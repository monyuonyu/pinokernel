# CPU の予算と見張り（ROADMAP 0-1 の後半、設計）

2026-09-28。`feat/rng0-exit-dispatch`（割込みの出口での遅延ディスパッチ）の上に作る。

## 何が残っているか
- RNG0 の直しで、**低い（か同じ）優先度**の回り続けるタスクが、時間待ちの高い優先度のタスクを飢えさせることは無くなった
- 残るのは優先度どおりの振る舞いそのもの: **高い優先度**のタスクがカーネルを呼ばずに回ると、それより低い全員が止まる。同じ優先度どうしは SCHED_FIFO なら譲らない（SCHED_RR だけが tick で回す、timer.c 254行）。μT-Kernel の約束としては正しいので、**約束（tk_*）は変えない**
- 今ある予算は selfc の子プロセスの RLIMIT_CPU 30秒だけ（arch/linux/selfc_proc.c）。ベアメタルと hosted のタスクには無い。heal の watchdog は死んだ ELF を起こし直すだけで、走りすぎは見ない

## 作るもの（段階）
1. **見張り（記録だけ）**: 割込みの出口（`knl_irq_exit_dispatch` の隣）で、タイマの割込みのたびに「同じタスクが切り替わらずに走っている tick 数」を数える。切り替わったら0に戻す。予算（既定 2000ms、idle は数えない）を超えたら、そのタスクについて1回だけ `[budget] task <id> ran <ms> ms without a switch` を出す。「切り替わらずに」は dispatch.S の `knl_dispatch_entry`／`knl_dispatch_to_schedtsk` の入口で増える回数が tick の間に動かなかったこと。**振る舞いは変えない**。interocept（内受容）の軸に載せるのは次の一手
2. **ring3 の ELF には罰**: 予算を超えた ring3 のタスクは heal と同じ経路で終わらせて起こし直す（selfc の RLIMIT_CPU と同じ考え方。ring3 は信頼しない側）。ring0 のタスクは記録だけ（カーネルの一部なので止めると系が壊れうる）
3. hosted（Linux）は同じ数え方を tick のスレッドで。数える場所だけ違う

## どう測るか（陰性コントロールつき）
- probe は T18（arch/x86/selftest.c、`tests/x86/run_cpu_budget.sh`）。pri 20 の ring0 タスクがカーネルを呼ばずに約3秒回り、その間 init（それより高い優先度）は寝ている＝回るタスクが切り替わらずに走り続ける形。「高い優先度のタスクが下を止める」形と、見張りから見れば同じ（どちらも切り替えが起きない）。腕と期待:
  - spin: `[budget]` がちょうど1行／yield（10ms ごとに tk_dly_tsk）: 0行／nobudget（見張りを外す、陰性コントロール）: 0行
  - **preempt**: 同じく回るが、init が 10ms ごとに300回起きて割込みの出口で割り込む → 切り替えが起きるので0行。**切り替えの回数を試すのはこの腕**（yield はタスクが寝るたび idle で数え直しになるので、回数の比較を外しても緑のまま＝監査-17 の指摘）
  - noentrycount（preempt から `knl_dispatch_entry` の数えを外す、陰性コントロール）: 1行
- 約束ごとの試験（tk_conform）と killchurn が緑のまま（段階1は振る舞いを変えないので、ここが赤なら見張り自体の誤り）
- crown は x86 で動く（re-bless が要る）。段階1と2は別々に

## Linux 版（hosted）の割込みの出口（2026-10-04、`feat/hosted-exit-dispatch`）
- hosted の「割込み」は SIGALRM。ハンドラの中で `knl_dispatch` に入ると、タスクの外から入ることになる（preempt.c の昔の注記）。そこで**ハンドラは中断した文脈を書き換えるだけ**: 中断した RIP を red zone（128B）の下に積み、RIP を `arch_preempt_tramp` にする。sigreturn のあと、タスク自身のスタック・ふだんのシグナルマスクで飛び先が走り、C の呼び出しで守られないレジスタ（rax〜r11・フラグ・x87/SSE を fxsave64）を退避して `knl_dispatch_entry` を呼び、戻ったら復元して中断した命令へ `ret`
- **中断した場所が p-kernel 自身の .text のときだけ**（`__executable_start`〜`__etext`）。libc の中だと libc のロック（malloc のは再入不可）を持っている恐れがあり、次のタスクがそこで止まる。その場合は今までどおり次の END_CRITICAL_SECTION で切り替わる
- 試験: tk_conform の K9（ディスパッチ許可で、カーネルを呼ばない約150ms の回転の間に、20ms で起きる高い優先度のタスクが走る）。陰性コントロール `-DPK_HOSTED_NO_EXIT_DISPATCH`（K9 だけ赤）
- 限界: AVX の上半分は退避しない（hosted は -mavx なし）。aarch64（Android）は未着手で、台帳の行はそのため開いたまま
- 他の案: ハンドラの中で切り替える（sigaltstack を全タスクで共有しているので、切り替えた先で次のシグナルが同じスタックを上書きする）、タスクごとに pthread（ディスパッチャの意味が変わる）
- **結果（2026-10-04、実装者の回）**: tkconf 111/0 ×2、陰性コントロールは K9 だけ赤（110/1）、CI の hosted の自己試験の束（37コマンド）は 119/119 で master と同じ。途中で束が `handoff stream` で SIGSEGV になった。最初は「スタックのあふれ」、次に「hosted の心は協調的な切替えが前提」と疑ったが、どちらも外れ。切替えの記録を取ると1回の切替えの直後に同じ関数で落ちていて、原因は飛び先の `ret` が red zone の 128B を返していなかったこと（`ret $128` で直った）。**tkconf の公式の試験だけでは捕まらなかった**（最初の版でも 111/0・NC 11種赤）＝監査では束と多ノードの試験も走らせる
- **監査-21 の FAIL と、その先（2026-10-05 23:15 の回）**: 計算プール（pk_parallel_rows）は1つの共有の仕事なので、出してから合流するまでは切り替えない（`pk_parallel_busy`）。抜けるとき、刻みが見送った切替えをその場でする（しないと、プールを続けて呼ぶタスクが高いタスクを飢えさせた＝`tkconf pool` P5 が 0）。**ただしプールは一例にすぎない**: 「カーネルを呼ばない間は割り込まれない」を前提にした共有の状態は、送信の作業領域（pmesh・netstack・net_relay）、kdds の topic の枠、arkfs・pfs、student の作業領域、dtr などに約18か所ある（一覧はこの段落を足したコミットのメッセージ。4か所は原文で確認、ほかは未確認）。**次の段の設計**: 切り替えない区間を**タスクごとの深さ**で持つ（大域の数えだと、区間の中で tk_* で待つタスクがいる間、ほかの全タスクが割り込めなくなる）。`PK_NOSW_ENTER/LEAVE` は hosted x86_64 だけ中身があり（ほかは空＝crown 不変）、LEAVE で深さが 0 になったら見送った切替えをする。包むのは上の各入口（送信・kdds・arkfs/pfs・student の1ステップ・dtr）。試験は `tkconf pool` と同じ形で、kdds の topic を2つのタスクが同時に使う探りを足す。**それまで hosted の出口の切替えは master に入れない**。ベアメタル x86・AArch64 は 09-29／10-01 から同じ前提が崩れている: master に同じ形の探りを入れると kdds の読みが千切れた（x86 24・26、AArch64 32・28／60、台帳 EXIT-DISPATCH-SHARED-STATE）。ベアメタルにも同じ区間を持たせるのは crown が動くので判断待ち。**第1・2段（2026-10-05 23:15 の回）**: kdds の枠・pmesh_send・udp_send・ip_send・ARP/ICMP の返事・中継の送り手を包んだ（`tkconf nosw` 3/3 緑、`-DPK_NOSW_OFF` で N1 赤 3/3＝32〜36／60）。第3段: pfs_put_origin・pfs_get（関数まるごとは `PK_NOSW_FN`。空きの枠を選んでから使用中にするまでの窓が狭いので、試験用に `-DPFS_FAULT_SLOW_PUT` で広げる: 区間ありで失われた枠 0／40 ×2、なしで 4・3）。第4段（10-05）: net_relay_tcp・m_gate の遅延作成。**第5段（2026-10-06 04:20 の回）**: student（公開の入口を全部、学習の1歩は `ST_TRAIN_STEP` で forward〜adam を1区間、生成は1バイトごと）・arkfs（公開の入口21）・dtr（入口と、dtr_task／dtr_infer のカーネル呼び出しの間の計算）・dlb の環・cradle の取り込み・swim の gq・m_pretrain_busy。**落とし穴**: hosted の Makefile は LLM の層を `LLM_CFLAGS`（`-D_LINUX_X86_64_` なし）でコンパイルするので、student.c などの区間は空だった（探りの N6・N14 で見つけた）→ `-DPK_NOSW_HOSTED`。`tkconf nosw` N6〜N15 は区間ありで 2/2 緑、`-DPK_NOSW_OFF` で N6・N7・N12・N14 が 2/2 赤、N9（ark）は 1/2。gossip_learn は区間なし（gl_wsum・gl_st_* は1つのタスクからしか呼ばれない）。**残り**: drpc の待ち表（空きを選ぶ→tk_cre_sem→in_use の間にカーネル呼び出しがあり、RNG0 の前から開いている。直すには順番を変える＝ベアメタルの .text が動く）、DMN の学習の一切れの途中で shell が abort して g_student を替える（1歩ごとの区間では防げない。設計が要る）、ui_api の間引きの数え・frontier の作業領域（害は小、未確認）、r3 の rw・rg・rc（r3_round_busy＋m_gate、未確認）

## 決めたこと・他の案
- 既定の予算 2000ms: 今のベアメタルで一番長い計算は数十 ms（台帳の RNG0 行の調査）。桁で余裕を取る。他の案: 優先度ごとに予算（今は要らない）
- 優先度を自動で下げる案は採らない: tk_chg_pri を勝手に呼ぶと約束（優先度はアプリが決める）を破る
