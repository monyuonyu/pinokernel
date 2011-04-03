OUTPUT_ARCH(h8300h)

MEMORY
{
	rom(rx)		: org = 0x000000, len = 0x080000
	vectors(rx)	: org = 0x000000, len = 0x000100
	romdata(rx)	: org = 0x000100, len = 0x07ff00

	ram(rwx)		: org = 0xffbf20, len = 0x004000
	ramdata(rwx)	: org = 0xffbf20, len = 0x000600
	bufdata(rwx)	: org = 0xFFC520, len = 0x002800
	stack(rw)		: org = 0xffff00, len = 0x000000
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
