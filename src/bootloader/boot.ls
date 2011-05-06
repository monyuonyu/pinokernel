OUTPUT_ARCH(h8300h)

/* メモリマップ */
MEMORY
{
	rom(rx)		: org = 0x000000, len = 0x080000		/* TOP of ALL ROM 512KB					*/
	vectors(rx)	: org = 0x000000, len = 0x000100		/* TOP of ROM 256B	.vectors .text area	*/
	romdata(rx)	: org = 0x000100, len = 0x07ff00		/* 512KB - 256B =  524032B	 END of ROM */

	ram(rwx)		: org = 0xffbf20, len = 0x004000	/* TOP of ALLRAM 16384B (16KB) 				*/
	softvec(rwx)	: org = 0xffbf20, len = 0x000010	/* TOP of RAM Softvectors area 16B (4bevter)*/
	ramdata(rwx)	: org = 0xffbf30, len = 0x000600	/*  .bss .data area 1516B (1.5KB)			*/
	bufdata(rwx)	: org = 0xffc520, len = 0x001400	/*  .buf area 5120B (5KB)	 				*/
	pinoc(rwx)		: org = 0xffd920, len = 0x001400	/*  pinoc kernel area 5120B (5KB)	 		*/
	k_stack(rw)		: org = 0xfff610, len = 0x000000	/* 2288B (2.2KB) (572recovery) END of RAM	*/
	b_stack(rw)		: org = 0xffff00, len = 0x000000	/* 2288B (2.2KB) (572recovery) END of RAM	*/
}

SECTIONS
{
	.vectors : {
		vectors.o(.data)
	} > vectors

	.text : {
		_text_start = .;
		*(.text)
	} > romdata

	.rodata : {
		_rodata_start = .;
		*(.strings)
		*(.rodata)
		*(.rodata.*)
		_rodata_end = .;
	} > romdata

	.bss : {
		_bss_start = .;
		*(.bss)
		_bss_end = .;
	} > ramdata AT> romdata

	.data : {
		_data_start = .;
		*(.data)
		_data_end = .;
	} > ramdata AT> romdata

	.buf : {
		_buf_start = .;
	} > bufdata

	.stack : {
		_b_stack = .;
	} > b_stack

	.softvec : {
		_softvec = .;
	} > softvec

}
