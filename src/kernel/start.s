.h8300h
.section .text
.global _start
.type _start,@function

_start:
	mov.l	#_k_stack, sp
	jsr		@_main
