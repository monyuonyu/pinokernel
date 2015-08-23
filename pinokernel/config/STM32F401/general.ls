/* メモリマップ */
MEMORY
{
	rom(rx)      : org = 0x00000000, len = 0x00080000    /* TOP of ALL ROM 512KB                 */
	vectors(rx)  : org = 0x00000000, len = 0x00000194    /* TOP of ROM 404B  .vectors .text area */
	romdata(rx)  : org = 0x00000200, len = 0x0007FE00    /* 512KB - 404B =  523884B   END of ROM */

	ram(rwx)     : org = 0x20000000, len = 0x00018000    /* TOP of ALL RAM 98304B (96KB)         */
	ramdata(rwx) : org = 0x20000000, len = 0x00016000    /* .bss .data area 90112B (88KB)        */
	u_stack(rw)  : org = 0x02016000, len = 0x00001000    /* 4096B (4.0KB) END of RAM             */
	i_stack(rw)  : org = 0x02017000, len = 0x00001000    /* 4096B (4.0KB) END of RAM             */
}

SECTIONS
{
/*
	.vectors : {
		vectors.o(.data)
	} > vectors

	.vectors : {
		relboot.rel(.data)
	} > vectors
*/
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
		_edata = .;
		*(.bss)
		_sbss = .;
	} > ramdata AT> romdata

	.data : {
		_sidata = .;
		*(.data)
		_sdata = .;
	} > ramdata AT> romdata

	.u_stack : {
		_ebss = .;
	} > u_stack

	.i_stack : {
		_i_stack = .;
	} > i_stack
}
