/*
 *----------------------------------------------------------------------
 *    micro T-Kernel 3.0  p-kernel AArch64 ベアメタルポート
 *
 *    This software is distributed under the T-License 2.2.
 *----------------------------------------------------------------------
 */

/*
 *	cpu_cntl.c
 *	CPU 制御（AArch64 ベアメタルポート）
 *
 *	CPU 依存グローバル変数の実体と、offset.h のビルド時検証を
 *	提供します。ディスパッチャ本体は arch/aarch64/cpu_support.S。
 */

#include <sys/machine.h>
#ifdef AARCH64_VIRT

#include "kernel.h"
#include "offset.h"

#include <stddef.h>	/* offsetof */

/*
 * タスク独立部（割込みハンドラ実行中）ネストカウンタ
 *	cpu_support.S の knl_timer_handler_startup が増減します。
 */
EXPORT W knl_taskindp = 0;

/*
 * 割込みの出口での遅延ディスパッチの判定（RNG0 の AArch64 版、2026-09-29。
 * gap-ledger RNG0-AARCH64-NO-IRQ-EXIT-DISPATCH）
 *	arch/aarch64/cpu_support.S の _vec_el1_irq が EOI の後に呼び、1 なら
 *	knl_dispatch で切り替える（SMP 自己試験と同じ切替えの道と
 *	.Lirq_resume_tramp）。条件は x86 の knl_irq_exit_dispatch と同じ:
 *	いちばん外側の割込み（knl_taskindp == 0）、ディスパッチ禁止でない、
 *	タスクを割り込んだ（ctxtsk != NULL。ディスパッチャの idle の wfe 中は
 *	ctxtsk が NULL）、切り替えが要る。
 *	SMP 自己試験のビルドは smp_irq_need_resched を使うので、ここは入れない
 *	（そのビルドの .text を1バイトも変えないため）。
 */
#ifndef SMP_SELFTEST
#ifndef PK_NOSW_OFF
/*
 * 切り替えない区間（pk_nosw.h。R-NOSW-BARE、2026-10-07）。x86_pc/cpu_cntl.c
 * と同じ: 深さはタスクごと、いちばん外側を出るとき飛ばした切り替えをする、
 * タスク独立部では数えない、knl_setup_context で 0 に戻す。
 * -DPK_NOSW_OFF: 区間なし（陰性コントロール）。
 */
LOCAL UH knl_nosw_depth[CFN_MAX_TSKID];

EXPORT void pk_nosw_enter( void )
{
	BEGIN_DISABLE_INTERRUPT;
	if ( knl_taskindp == 0 && knl_ctxtsk != NULL ) {
		knl_nosw_depth[INDEX_TSK(knl_ctxtsk->tskid)]++;
	}
	END_DISABLE_INTERRUPT;
}

EXPORT void pk_nosw_leave( void )
{
	BOOL out = FALSE;

	BEGIN_DISABLE_INTERRUPT;
	if ( knl_taskindp == 0 && knl_ctxtsk != NULL ) {
		UH *d = &knl_nosw_depth[INDEX_TSK(knl_ctxtsk->tskid)];
		if ( *d != 0 && --*d == 0 ) {
			out = TRUE;
		}
	}
	END_DISABLE_INTERRUPT;
	if ( out ) {
		BEGIN_CRITICAL_SECTION;		/* 飛ばした切り替えがあればここで */
		END_CRITICAL_SECTION;
	}
}

EXPORT void knl_nosw_forget( ID tskid )
{
	knl_nosw_depth[INDEX_TSK(tskid)] = 0;
}
#define NOSW_HELD()	( knl_nosw_depth[INDEX_TSK(knl_ctxtsk->tskid)] != 0 )
#else
#define NOSW_HELD()	0
#endif

EXPORT INT knl_irq_exit_need_dispatch( void )
{
	return ( knl_taskindp == 0 && !knl_dispatch_disabled
	      && knl_ctxtsk != NULL && knl_ctxtsk != knl_schedtsk
	      && !NOSW_HELD() ) ? 1 : 0;
}
#endif

/*
 * offset.h（アセンブラ用オフセット定数）と実際の TCB レイアウトの
 * ビルド時照合。
 */
_Static_assert( offsetof(TCB, task)    == TCB_task,
		"offset.h の TCB_task が TCB 実レイアウトと不一致" );
_Static_assert( offsetof(TCB, tskctxb) == TCB_tskctxb,
		"offset.h の TCB_tskctxb が TCB 実レイアウトと不一致" );
_Static_assert( offsetof(CTXB, ssp)    == CTXB_ssp,
		"offset.h の CTXB_ssp が CTXB 実レイアウトと不一致" );

#endif /* AARCH64_VIRT */
