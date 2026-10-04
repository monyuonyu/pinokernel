/*
 * pk_text_end.c — the LAST p-kernel object on the hosted x86_64 link line
 * (before the libraries). See pk_text_begin.c.
 */
__asm__(
    "    .text\n"
    "    .globl pk_text_end\n"
    "pk_text_end:\n"
);
