OUTPUT_FORMAT(binary)
OUTPUT_ARCH(i386)

SECTIONS {
	. = 0x7e00;
	.text : {
		*(.text)
	}
}
