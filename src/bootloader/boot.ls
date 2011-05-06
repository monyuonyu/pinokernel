OUTPUT_ARCH(h8300h)

/* メモリマップ */
MEMORY
{
	rom(rx)		: org = 0x000000, len = 0x080000		/* TOP of ROM 512KB			 			*/
	vectors(rx)	: org = 0x000000, len = 0x000100		/* TOP of ROM 256B			 			*/
	romdata(rx)	: org = 0x000100, len = 0x07ff00		/* 512KB - 256B =  524032B	 END of ROM */

	ram(rwx)		: org = 0xffbf20, len = 0x004000	/* TOP of RAM 16384B (16KB) 	*/
	ramdata(rwx)	: org = 0xffbf20, len = 0x000600	/* TOP of RAM 1536B (1.5KB) 	*/
	bufdata(rwx)	: org = 0xFFC520, len = 0x002800	/* 10240B (10KB)			 	*/
	stack(rw)		: org = 0xffff00, len = 0x000000	/* 4576B (4.5KB) END of RAM 	*/
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
		_stack = .;
	} > stack
}
