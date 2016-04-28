 ;
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
 ;

 ;
 ;	@(#)disint.S (libtk/FM3)
 ;
 ;	Enable/disable external interrupt
 ;

#include <machine.h>
#include <sysdef.h>

;	.code 16
;	.syntax unified
;	.thumb
 ;
 ; Disable external interrupt
 ; UINT disint( void )
 ;	Disable interrupt by Rise prioperty to 0.
 ;
;	.text
	.align	2
;	.thumb_func
	.public	_disint
;	.type	_disint, %function
_disint:
;	mrs	r0, basepri
;	ldr	r1, =MAX_EXTINT_PRI
;	msr	basepri, r1
;	bx	lr


 ;
 ; Enable external interrupt (restore to original state)
 ; UINT enaint( UINT intsts )
 ;	Enable interrupt.
 ;
;	.text
	.align	2
;	.thumb_func
	.public	_enaint
;	.type	_enaint, %function
_enaint:
;	msr	basepri, r0
;	bx	lr

 ;
 ; Get the value of Base Priority Mask Register.
 ; UINT get_basepri( void )
 ;
;	.text
	.align	2
;	.thumb_func
	.public	_get_basepri
;	.type	_get_basepri, %function
_get_basepri:
;	mrs	r0, basepri
;	bx	lr

;	.end
