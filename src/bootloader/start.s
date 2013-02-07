	.h8300h
	.section .text
	.global _start
_start:
	mov.l	#_b_stack, sp
	jsr		@_init
