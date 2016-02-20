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

;	.section .data_vector,"aw"
;	.align 2
;vector_syshdr:
;	.space  (16 * 4), 0
	.public _knl_intvec
_knl_intvec:
;	.space	(48 * 4), 0

 ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 ;;
 ;; Start up routine
 ;;

;	.text
;	.align 2

;	.thumb
;	.thumb_func
	.public _Reset_Handler
;	.type	_Reset_Handler, %function
_Reset_Handler:
	call	!!_init_clock_control

setup_ram_vectors:
	; Load vector table to SRAM
;	ldr     r1,	=__vector_org	;  src address
;	ldr     r2,	=__vector_start	;  dst address
;	ldr     r3,	=__vector_end
;	sub     r3,	r3,	r2	;  r3 := r3 - r2 = vector_size

vector_loop:
;	ldr	r0, [r1], #4
;	str	r0, [r2], #4
;	subs    r3, r3, #4
;	bne     vector_loop		;  if vector_size != 0

vector_done:
	;  Set vector table offset to SRAM
;	ldr     r1, =SCB_VTOR
;	ldr     r2, =__vector_start
;	str     r2, [r1]

data_init:	;  .data
;	ldr     r1, =__data_org		;  src address
;	ldr     r2, =__data_start	;  dst address
;	ldr     r3, =__data_end
;	subs    r3, r3, r2		;  r10 := data_size
;	beq     data_done		;  if __data_start == __data_end

data_loop:
;	ldr	r0, [r1], #4
;	str	r0, [r2], #4
;	subs    r3, r3, #4
;	bgt     data_loop		;  if data_size > 0

data_done:

	br	!!_startup_common
