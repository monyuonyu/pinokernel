 ;
 ;	micro T-Kernel SVC interface library (mb9af312k)
 ;
 ;	   created from /usr/local/cortex-m3/utkernel_source/include/tk/dbgspt.h
 ;

#include <machine.h>
#include <sysdef.h>
#include <tdfncd.h>
#include "utk_config.h"

;	.code 16
;	.syntax unified
;	.thumb

#if USE_TRAP || (USE_DBGSPT && USE_HOOK_TRACE)
;	.text
;	.align 2
;	.thumb_func
	.public	_td_cal_que
;	.type	_td_cal_que, %function
_td_cal_que:
;	push	{r4,lr}
;	ldr	ip, =TDFN_CAL_QUE
#if USE_TRAP
;	svc	SVC_DEBUG_SUPPORT
#else
;	bl	_knl_call_dbgspt
#endif
;	pop	{r4,lr}
;	bx	lr
#endif

