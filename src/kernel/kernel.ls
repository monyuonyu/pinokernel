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
	bufdata(rwx)	: org = 0xffc520, len = 0x002800	/*  .buf area 10240B (10KB)	 				*/
	stack(rw)		: org = 0xffff00, len = 0x000000	/* 4576B (4.5KB) (1144recovery) END of RAM	*/
}

SECTIONS
{

	.text : {
		*(.text)
	} > bufdata

	.rodata : {
		*(.strings)
		*(.rodata)
		*(.rodata.*)
	} > bufdata

	.bss : {
		*(.bss)
	} > bufdata

	.data : {
		*(.data)
	} > bufdata

	.stack : {
		_stack = .;
	} > stack

}
