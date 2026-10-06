/*
 * pk_text_begin.c — the FIRST object on the hosted aarch64 link line
 * (boot/linux/Makefile, both p-kernel and libpkernel.so; the Android
 * CMakeLists.txt puts it first too). A label here and one in pk_text_end.c
 * bracket p-kernel's own .text for the exit dispatch (preempt.c); the
 * x86_64 sibling explains why (-static puts glibc in the same .text).
 * preempt.c checks the bracket at boot and keeps the old behaviour if the
 * layout ever says otherwise.
 */
__asm__(
    "    .text\n"
    "    .globl pk_text_begin\n"
    "pk_text_begin:\n"
);
