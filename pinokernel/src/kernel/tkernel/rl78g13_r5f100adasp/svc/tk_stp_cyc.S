 ;
 ;	micro T-Kernel SVC interface library (mb9af312k)
 ;
 ;	   created from /usr/local/cortex-m3/utkernel_source/include/tk/syscall.h
 ;

#include <machine.h>
#include <sysdef.h>
#include <tkfncd.h>
#include "utk_config.h"

;	.code 16
;	.syntax unified
;	.thumb

#if USE_TRAP || (USE_DBGSPT && USE_HOOK_TRACE)
;	.text
;	.align 2
;	.thumb_func
	.public	_tk_stp_cyc
;	.type	_tk_stp_cyc, %function
_tk_stp_cyc:
;	push	{r4,lr}
;	add	r4, sp, #8
;	ldr	ip, =TFN_STP_CYC
#if USE_TRAP
;	svc	SVC_SYSCALL
#else
;	bl	_knl_call_entry
#endif
;	pop	{r4,lr}
;	bx	lr
#endif

