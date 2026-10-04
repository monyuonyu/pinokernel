/* AArch64 Raspberry Pi 3 linker script
 * Load address: 0x80000 — the BCM2837 firmware drops the AArch64
 * payload here when config.txt sets arm_64bit=1 + kernel=kernel8.img.
 * Stack: grows down from end of image + 64 KB.
 */
ENTRY(_start)
OUTPUT_FORMAT("elf64-littleaarch64")
OUTPUT_ARCH(aarch64)

SECTIONS {
    . = 0x80000;

    .text : {
        *(.text.start)      /* start.S _start must be first */
        *(.text)
        *(.text.*)
    }

    . = ALIGN(2048);        /* EL1 exception vectors need 2KB alignment */

    .rodata : {
        *(.rodata)
        *(.rodata.*)
    }

    . = ALIGN(8);

    .data : {
        *(.data)
        *(.data.*)
    }

    . = ALIGN(8);

    _bss_start = .;
    .bss (NOLOAD) : {
        *(COMMON)
        *(.bss)
        *(.bss.*)
    }
    . = ALIGN(8);
    _bss_end = .;

    /* 64 KB stack */
    . = ALIGN(16);
    _stack_start = .;
    . += 0x10000;
    _stack_top = .;

    /* 16 KB stacks for secondary CPUs 1..3, the same shape as linker.ld
     * (MC-2.0 / MC-2.1). mc2_smp.c references _stack_top_cpu1..3 in every
     * build, so without them the RPi 3 image does not link. */
    . = ALIGN(16);
    _stack_start_cpu1 = .;
    . += 0x4000;
    _stack_top_cpu1 = .;
    . = ALIGN(16);
    _stack_start_cpu2 = .;
    . += 0x4000;
    _stack_top_cpu2 = .;
    . = ALIGN(16);
    _stack_start_cpu3 = .;
    . += 0x4000;
    _stack_top_cpu3 = .;

    . = ALIGN(4096);
    _kernel_end = .;

    /DISCARD/ : {
        *(.comment)
        *(.eh_frame)
        *(.note*)
    }
}
