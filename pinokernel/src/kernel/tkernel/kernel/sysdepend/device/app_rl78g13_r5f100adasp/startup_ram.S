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

#include "hwconfig.h"
#include "utk_config.h"

#include <machine.h>
#include <tk/asm.h>

;	.code 16
;	.syntax unified
;	.thumb

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

	; Set vector table offset to SRAM
;	ldr     r1, =SCB_VTOR
;	ldr     r2, =__vector_start
;	str     r2, [r1]

	br	!!_startup_common

