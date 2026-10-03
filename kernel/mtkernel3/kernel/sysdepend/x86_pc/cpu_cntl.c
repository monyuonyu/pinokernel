/*
 *----------------------------------------------------------------------
 *    micro T-Kernel 3.0  p-kernel x86 ベアメタルポート
 *
 *    This software is distributed under the T-License 2.2.
 *----------------------------------------------------------------------
 */

/*
 *	cpu_cntl.c
 *	CPU 制御（x86 ベアメタルポート）
 *
 *	CPU 依存グローバル変数の実体、offset.h のビルド時検証、
 *	ディスパッチ前の TCB poison チェックを提供します。
 */

#include <sys/machine.h>
#ifdef X86_PC

#include "kernel.h"
#include "../../tkernel/task.h"	/* TSTAT（TS_NONEXIST）の定義 */
#include "offset.h"

#include <stddef.h>	/* offsetof */

/*
 * タスク独立部（割込みハンドラ実行中）ネストカウンタ
 */
EXPORT W knl_taskindp = 0;

/*
 * offset.h（アセンブラ用オフセット定数）と実際の TCB レイアウトの
 * ビルド時照合。ズレはコンパイルエラーとして検出されます。
 */
_Static_assert( offsetof(TCB, task)    == TCB_task,
		"offset.h の TCB_task が TCB 実レイアウトと不一致" );
_Static_assert( offsetof(TCB, tskctxb) == TCB_tskctxb,
		"offset.h の TCB_tskctxb が TCB 実レイアウトと不一致" );
_Static_assert( offsetof(TCB, isstack) == TCB_isstack,
		"offset.h の TCB_isstack が TCB 実レイアウトと不一致" );
_Static_assert( offsetof(CTXB, ssp)    == CTXB_ssp,
		"offset.h の CTXB_ssp が CTXB 実レイアウトと不一致" );

/*
 * 早期出力（boot/x86 のシリアル直叩き。sio_send_frame でも可） */
IMPORT void sio_send_frame(const UB *buf, INT size);

/*
 * ディスパッチ直前の TCB poison チェック（dispatch.S から呼ばれる）
 *	tk_del_tsk で FreeQue に返却された TCB は state == TS_NONEXIST。
 *	kill/heal churn がそのような TCB を knl_schedtsk に残した場合、
 *	そのまま切り替えると再利用済みメモリへ ret して garbage-PC #PF に
 *	なる。ここで決定的・grep 可能な halt に変える。
 */
EXPORT void knl_dispatch_poison_check( TCB *tcb )
{
	static const char msg[] = "\r\n[dispatch] POISON: freed TCB in schedtsk — halt\r\n";

	if ( tcb->state != TS_NONEXIST ) {
		return;			/* 正常 — 何もしない */
	}

	sio_send_frame((const UB *)msg, (INT)sizeof(msg) - 1);
	for ( ;; ) {
		__asm__ volatile ("cli; hlt");
	}
}

/*
 * 割込みの出口での遅延ディスパッチ（RNG0-BUSY-TASK-STALLS-DISPATCH の修正、
 * 2026-09-28。gap-ledger・ROADMAP 0-1）
 *	boot/x86/idt.c の irq_handler の最後（EOI の後、iretq の前）から呼ばれる。
 *	割込みの中の END_CRITICAL_SECTION は、タスク独立部なのでディスパッチ
 *	しない（μT-Kernel の決まりどおり）。そのため、カーネルを呼ばずに回り
 *	続けるタスクがあると、時限待ちが明けた高い優先度のタスクへ永久に
 *	切り替わらなかった。ここで、いちばん外側の割込みからタスクへ戻る直前に
 *	切り替える。ほかの μT-Kernel のポートが割込みの出口（ret_int・PendSV）で
 *	していることと同じ。
 *	割込みフレームは割り込まれたタスクのスタック（ring3 なら TSS.RSP0 の
 *	カーネルスタック）にあるので、knl_dispatch_entry はそれごとタスクの
 *	休止フレームとして残す。再開するとここへ戻り、irq_call32_stub から
 *	iretq で割り込まれた場所へ帰る。
 *	条件: 割込みの入れ子の外側（knl_taskindp == 0）、ディスパッチ禁止でない
 *	（tk_dis_dsp 中や、idle の hlt を含むディスパッチャの中ではない）、
 *	タスクを割り込んだ（ctxtsk != NULL）、切り替えが要る。
 *	割込みゲートなので IF=0 で来る。knl_dispatch_entry は再開時に sti して
 *	戻るので、iretq までの残りを割込み禁止に戻す。
 *	-DPK_RNG0_NO_EXIT_DISPATCH は修正前の振る舞い（試験の陰性コントロール用）。
 */
EXPORT void knl_irq_exit_dispatch( void )
{
#ifndef PK_RNG0_NO_EXIT_DISPATCH
	if ( knl_taskindp == 0 && !knl_dispatch_disabled
	  && knl_ctxtsk != NULL && knl_ctxtsk != knl_schedtsk ) {
		knl_dispatch();
		__asm__ volatile ("cli" ::: "memory");
	}
#endif
}

/*
 * CPU の予算の見張り・段階1（記録だけ。docs/architecture/30-module/cpu-budget.md）
 *	boot/x86/idt.c の irq_handler から、タイマの割込み（IRQ0）のたびに
 *	呼ばれる。同じタスクが切り替わらずに（dispatch.S の knl_dispatch_entry・
 *	knl_dispatch_to_schedtsk の入口で数える knl_dispatch_count が動かずに）
 *	走り続けた時間を数え、予算を超えたら、そのタスクについて1回だけ
 *	記録する。振る舞い（スケジューリング）は変えない。idle（ctxtsk が
 *	NULL）は数えない。-DPK_NO_CPU_BUDGET で外せる（陰性コントロール用）。
 */
EXPORT UW knl_dispatch_count = 0;

#ifndef PK_NO_CPU_BUDGET
#ifndef PK_CPU_BUDGET_MS
#define PK_CPU_BUDGET_MS	2000
#endif
LOCAL TCB	*bud_tcb = NULL;
LOCAL UW	bud_seen = 0;
LOCAL UW	bud_ms = 0;
LOCAL BOOL	bud_warned = FALSE;

LOCAL INT bud_putdec( char *p, UW v )
{
	char	t[10];
	INT	n = 0, i;
	do { t[n++] = (char)('0' + v % 10); v /= 10; } while ( v != 0 && n < 10 );
	for ( i = 0; i < n; i++ ) p[i] = t[n - 1 - i];
	return n;
}

EXPORT void knl_cpu_budget_tick( void )
{
	static const char s1[] = "[budget] task ";
	static const char s2[] = " ran ";
	static const char s3[] = " ms without a switch\r\n";
	char	buf[64];
	INT	n = 0, i;
	TCB	*t = knl_ctxtsk;

	if ( t == NULL || t != bud_tcb || knl_dispatch_count != bud_seen ) {
		bud_tcb = t;
		bud_seen = knl_dispatch_count;
		bud_ms = 0;
		bud_warned = FALSE;
		return;
	}
	bud_ms += CFN_TIMER_PERIOD;
	if ( bud_warned || bud_ms < PK_CPU_BUDGET_MS ) {
		return;
	}
	bud_warned = TRUE;
	for ( i = 0; s1[i] != '\0'; i++ ) buf[n++] = s1[i];
	n += bud_putdec(buf + n, (UW)t->tskid);
	for ( i = 0; s2[i] != '\0'; i++ ) buf[n++] = s2[i];
	n += bud_putdec(buf + n, bud_ms);
	for ( i = 0; s3[i] != '\0'; i++ ) buf[n++] = s3[i];
	sio_send_frame((const UB *)buf, n);
}
#endif /* PK_NO_CPU_BUDGET */

#endif /* X86_PC */
