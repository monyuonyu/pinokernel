.h8300h
.section .text
.global _start
.type _start,@function

_start:
	mov.l	#_u_stack, sp
	jsr		@_main
