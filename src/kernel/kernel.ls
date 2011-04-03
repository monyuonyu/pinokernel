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

}
