/*
 * pk_text_begin.c — the FIRST object on the hosted x86_64 link line.
 *
 * The hosted build is -static, so glibc's code sits in the same .text as
 * p-kernel's and __executable_start..__etext cannot tell them apart (audit-20:
 * the exit dispatch switched inside malloc 86-138 times a run). GNU ld lays
 * out `*(.text .text.*)` in link-line order, so a label in the first object's
 * .text and one in the last (pk_text_end.c) bracket p-kernel's own code; the
 * archives (-lc -lpthread -ltcc) come after the end label. preempt.c checks
 * this at boot (malloc outside, its own code inside) and keeps the old
 * behaviour if the layout ever says otherwise.
 */
__asm__(
    "    .text\n"
    "    .globl pk_text_begin\n"
    "pk_text_begin:\n"
);
