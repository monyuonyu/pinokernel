	.file	"main.c"
	.h8300h
	.section .text
	.align 1
	.global _getstring
_getstring:
	mov.l	er6,@-er7
	mov.l	er7,er6
	mov.l	er4,@-er7
	sub.l	#8,er7
	mov.l	er0,@(-12,er6)
	sub.w	r2,r2
	mov.w	r2,@(-8,er6)
	sub.w	r2,r2
	mov.w	r2,@(-6,er6)
	bra	.L2:16
.L5:
	mov.w	@(-8,er6),r2
	exts.l	er2
	mov.l	@(-12,er6),er3
	mov.l	er3,er4
	add.l	er2,er4
	mov.w	#1,r0
	jsr	@_sci_read_byte
	mov.b	r0l,r2l
	extu.w	r2
	mov.w	r2,r0
	jsr	@_tolower
	mov.w	r0,r2
	mov.b	r2l,@er4
	mov.w	@(-8,er6),r2
	exts.l	er2
	mov.l	@(-12,er6),er3
	add.l	er3,er2
	mov.b	@er2,r2l
	mov.b	r2l,r1l
	mov.w	#1,r0
	jsr	@_sci_write
	mov.w	@(-8,er6),r2
	exts.l	er2
	mov.l	@(-12,er6),er3
	add.l	er3,er2
	mov.b	@er2,r2l
	cmp.b	#13,r2l
	bne	.L3
	mov.w	@(-8,er6),r2
	exts.l	er2
	mov.l	@(-12,er6),er3
	add.l	er3,er2
	sub.b	r3l,r3l
	mov.b	r3l,@er2
	bra	.L1
.L3:
	mov.w	@(-8,er6),r2
	adds	#1,er2
	mov.w	r2,@(-8,er6)
	mov.w	@(-6,er6),r2
	adds	#1,er2
	mov.w	r2,@(-6,er6)
.L2:
	mov.w	@(-6,er6),r2
	cmp.w	#29,r2
	ble	.L5:16
.L1:
	adds	#4,er7
	adds	#4,er7
	mov.l	@er7+,er4
	mov.l	@er7+,er6
	rts
	.size	_getstring, .-_getstring
	.section	.rodata
.LC0:
	.string	"57"
.LC1:
	.string	"\n\r"
.LC2:
	.string	"dump"
.LC3:
	.string	"load"
.LC4:
	.string	"devlop"
.LC5:
	.string	"start"
	.section .text
	.align 1
	.global _test_vec
_test_vec:
	mov.l	er6,@-er7
	mov.l	er7,er6
	mov.l	er4,@-er7
	sub.l	#8,er7
	mov.w	r0,@(-6,er6)
	mov.l	er1,@(-12,er6)
	mov.w	@_i___1140:32,r4
	mov.w	#1,r0
	jsr	@_sci_read_byte_intr
	mov.b	r0l,r3l
	mov.w	r4,r2
	exts.l	er2
	mov.b	r3l,@(_buf___1139,er2)
	mov.w	@_i___1140:32,r2
	exts.l	er2
	mov.b	@(_buf___1139,er2),r2l
	mov.b	r2l,r1l
	mov.w	#1,r0
	jsr	@_sci_write
	mov.w	@_i___1140:32,r2
	exts.l	er2
	mov.b	@(_buf___1139,er2),r2l
	cmp.b	#13,r2l
	bne	.L7:16
	mov.l	#___FUNCTION_____1141,er1
	mov.w	#1,r0
	jsr	@_sci_write_str
	mov.b	#58,r1l
	mov.w	#1,r0
	jsr	@_sci_write
	mov.l	#.LC0,er1
	mov.w	#1,r0
	jsr	@_sci_write_str
	mov.l	#.LC1,er1
	mov.w	#1,r0
	jsr	@_sci_write_str
	mov.l	#.LC2,er1
	mov.l	#_buf___1139,er0
	jsr	@_strcmp
	mov.w	r0,r2
	mov.w	r2,r2
	bne	.L7
	mov.l	#.LC3,er1
	mov.l	#_buf___1139,er0
	jsr	@_strcmp
	mov.w	r0,r2
	mov.w	r2,r2
	bne	.L7
	mov.l	#.LC4,er1
	mov.l	#_buf___1139,er0
	jsr	@_strcmp
	mov.w	r0,r2
	mov.w	r2,r2
	bne	.L7
	mov.l	#.LC5,er1
	mov.l	#_buf___1139,er0
	jsr	@_strcmp
.L7:
	mov.w	@_i___1140:32,r2
	adds	#1,er2
	mov.w	r2,@_i___1140:32
	adds	#4,er7
	adds	#4,er7
	mov.l	@er7+,er4
	mov.l	@er7+,er6
	rts
	.size	_test_vec, .-_test_vec
	.section	.rodata
.LC6:
	.string	"108"
.LC7:
	.string	"PINoC Console>_ "
	.section .text
	.align 1
	.global _main
_main:
	mov.l	er6,@-er7
	mov.l	er7,er6
	sub.l	#32,er7
	mov.l	#___FUNCTION_____1144,er1
	mov.w	#1,r0
	jsr	@_sci_write_str
	mov.b	#58,r1l
	mov.w	#1,r0
	jsr	@_sci_write
	mov.l	#.LC6,er1
	mov.w	#1,r0
	jsr	@_sci_write_str
	mov.l	#.LC1,er1
	mov.w	#1,r0
	jsr	@_sci_write_str
	mov.w	#1,r0
	jsr	@_sci_read_intr_enable
; #APP
;# 114 "main.c" 1
	andc.b #0x3f,ccr
;# 0 "" 2
; #NO_APP
	mov.l	#_test_vec,er1
	mov.w	#2,r0
	jsr	@_softvec_setintr
	mov.l	er6,er2
	sub.l	#32,er2
	sub.l	er3,er3
	mov.l	er3,@er2
	adds	#4,er2
	sub.l	er3,er3
	mov.l	er3,@er2
	adds	#4,er2
	sub.l	er3,er3
	mov.l	er3,@er2
	adds	#4,er2
	sub.l	er3,er3
	mov.l	er3,@er2
	adds	#4,er2
	sub.l	er3,er3
	mov.l	er3,@er2
	adds	#4,er2
	sub.l	er3,er3
	mov.l	er3,@er2
	adds	#4,er2
	sub.l	er3,er3
	mov.l	er3,@er2
	adds	#4,er2
	sub.l	er3,er3
	mov.l	er3,@er2
	adds	#4,er2
	mov.l	#.LC7,er1
	mov.w	#1,r0
	jsr	@_sci_write_str
.L9:
; #APP
;# 129 "main.c" 1
	sleep
;# 0 "" 2
; #NO_APP
	bra	.L9
	.size	_main, .-_main
	.align 1
	.global _init
_init:
	mov.l	er6,@-er7
	mov.l	er7,er6
	mov.l	#_bss_end,er3
	mov.l	#_bss_start,er2
	mov.l	er3,er0
	sub.l	er2,er0
	mov.l	er0,er2
	sub.w	r1,r1
	mov.l	#_bss_start,er0
	jsr	@_memset
	mov.l	#_data_end,er3
	mov.l	#_data_start,er2
	mov.l	er3,er0
	sub.l	er2,er0
	mov.l	er0,er2
	mov.l	#_rodata_end,er1
	mov.l	#_data_start,er0
	jsr	@_memcpy
	sub.l	er2,er2
	add.b	#6,r2h
	sub.w	r1,r1
	mov.l	#_buf_start,er0
	jsr	@_memset
; #APP
;# 213 "main.c" 1
	orc.b #0xc0,ccr
;# 0 "" 2
; #NO_APP
	jsr	@_softvec_init
	mov.w	#64,r1
	mov.w	#1,r0
	jsr	@_sci_init
	jsr	@_main
	mov.l	@er7+,er6
	rts
	.size	_init, .-_init
	.section	.rodata
	.type	___FUNCTION_____1144, @object
	.size	___FUNCTION_____1144, 5
___FUNCTION_____1144:
	.string	"main"
	.local	_i___1140
	.comm	_i___1140,2,2
	.local	_buf___1139
	.comm	_buf___1139,32,1
	.type	___FUNCTION_____1141, @object
	.size	___FUNCTION_____1141, 9
___FUNCTION_____1141:
	.string	"test_vec"
	.ident	"GCC: (GNU) 4.5-GNUH8_v10.03"
	.end
