 ;----------------------------------------------------------------------
 ;    micro T-Kernel 2.0 Software Package
 ;
 ;    Copyright (C) 2006-2014 by Ken Sakamura.
 ;    This software is distributed under the T-License 2.0.
 ;----------------------------------------------------------------------
 ;
 ;    Released by T-Engine Forum(http://www.t-engine.org/) at 2014/09/01.
 ;
 ;----------------------------------------------------------------------

 ;  @(#) icrt0.S
 ;
 ;    Start up module
 ;


;	.code 16
;	.syntax unified
;	.thumb

;	.text
;	.align 2
;	.thumb_func
	.public _startup_common
_startup_common:

	; .bss initialization
#if USE_NOINIT
;	ldr     r2, =__noinit_end	; dst address
#else
;	ldr     r2, =__bss_start	; dst address
#endif
;	ldr     r3, =__bss_end
;	subs    r3, r3, r2		; r3 := data_size
;	beq     bss_done		; if __bss_start == __bss_end
;	movs	r0, #0			; r0 := 0

bss_loop:
;	str	r0, [r2], #4
;	subs    r3, r3, #4
;	bgt     bss_loop		; if data_size > 0

bss_done:

#if USE_IMALLOC
;	ldr     r5, =SYSTEMAREA_TOP
;	cmp	r2, r5			; _end or RAM_TOP
;	it	hi
;	movhi	r5, r2			; Either of High addresses
;	ldr	r0, =_knl_lowmem_top
;	str	r5, [r0]		; knl_lowmem_top = _end or RAM_TOP
;	ldr     r5, =SYSTEMAREA_END - (EXC_STACK_SIZE + TMP_STACK_SIZE)
;	ldr	r0, =_knl_lowmem_limit
;	str	r5, [r0]		; knl_lowmem_limit = RAM_END
#endif

#if USE_TMONITOR
	call	!!_sio_init		; initialize serial IO
#endif

	; Configure exception priorities
;	ldr	r5, =SCB_AIRCR		; Application interrupt and reset control register
;	ldr	r6, [r5]xsc fg 
;	ldr	r7, =0x0000F8FF
;	ands	r6, r6, r7
;	ldr	r7, =0x05FA0300		; PRIGRP:SUBPRI = 4 : 4
;	orr	r6, r6, r7
;	str	r6, [r5]

;	ldr	r5, =SCB_SHPR2
;	movs	r6, #0			; SVC pri = 0
;	str	r6, [r5]

;	ldr	r5, =SCB_SHPR3
;	ldr	r6, =0x10F00000		; Pendsv pri = 0xF, sistick pri = 0x1
;	str	r6, [r5]

;	ldr	r5, =NVIC_IPR_FORCE_DISPATCH
;	ldr	r6, [r5]
;	movt	r6, #0xc030		; force dispatch pri = 0xC, IRQ #94 pri = 0x3
;	str	r6, [r5]

	; Enable software interrupt
;	ldr	r0, =INTNO_FORCE_DISPATCH
	call	!!_EnableInt

	; Allow user to access SCB_STIR
;	ldr	r5, =SCB_CCR
;	ldr	r6, [r5]
;	orrs	r6, r6, #0x02
;	str	r6, [r5]

kernel_start:
	call	!!_main

l_end:
;	b	.

