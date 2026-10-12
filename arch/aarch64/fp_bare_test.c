/*
 *  fp_bare_test.c — FP/SIMD registers across task switches on bare-metal
 *  AArch64 (ROADMAP 0-11). Linked only into -DPK_FPBARE test builds
 *  (tests/aarch64/run_fp_bare.sh), so the default .text (the crown) is
 *  unchanged. The hosted form is `tkconf fp` (arch/linux/pool_preempt.c).
 *
 *  Two ways a task can lose its FP registers here:
 *   F1/F2  the IRQ-exit switch (RNG0, cpu_support.S _vec_el1_irq): the runner
 *          (pri 20) steps 8 doubles held in registers through chunks of a
 *          bounded linear map while a pri-19 task wakes every 2 ms and does FP
 *          work of its own, 60 times. Then the same number of chunks with
 *          dispatch disabled. The two results must be equal bit for bit.
 *          F2 is the precondition (high rounds landed inside the chunks).
 *   F3/F4  a voluntary switch (knl_dispatch): d8-d15 are callee-saved in the
 *          AArch64 procedure call standard, so a task that holds values in
 *          them across tk_slp_tsk must get them back. The runner pins 8
 *          values in d8-d15, sleeps; a pri-19 task pins other values in
 *          d8-d15 and sleeps too; the runner wakes and compares (30 rounds).
 *          F4 is the precondition (the high task ran in between).
 *   F5/F6  the FPCR rounding mode is per-task state too (AAPCS64: preserved
 *          across calls). The runner runs in round-toward-zero, the high tasks
 *          set round-toward-+inf; the runner's mode must come back after the
 *          IRQ-exit switch (F5, checked after every chunk) and after the
 *          voluntary switch of tk_wup_tsk (F6, 30 rounds).
 */
#ifdef PK_FPBARE
#include "kernel.h"

static void (*fb_out)(const char *);
static INT fb_fail;
static ID fb_caller;
static volatile INT fb_done;

static void fb_dec(long v)
{
    char b[24]; INT i = 23; b[i] = 0;
    unsigned long u = (v < 0) ? (unsigned long)(-v) : (unsigned long)v;
    if (u == 0) b[--i] = '0';
    while (u) { b[--i] = (char)('0' + u % 10); u /= 10; }
    if (v < 0) b[--i] = '-';
    fb_out(&b[i]);
}

static void fb_check(int ok, const char *id, const char *what, long got)
{
    fb_out("[fpb] "); fb_out(ok ? "PASS " : "FAIL "); fb_out(id); fb_out("  ");
    fb_out(what); fb_out(" (got "); fb_dec(got); fb_out(")\r\n");
    if (!ok) fb_fail++;
}

static ID fb_mk(void (*fn)(INT, void *), PRI pri)
{
    T_CTSK ct = { .exinf = NULL, .tskatr = TA_HLNG | TA_RNG0,
                  .task = (FP)fn, .itskpri = pri, .stksz = 8192 };
    return tk_cre_tsk(&ct);
}

/* FPCR rounding mode (RMode, bits 23:22). AAPCS64 treats it as state a call
 * must preserve, so a task's mode must survive its own switches (F5/F6). */
#define FB_RMODE_MASK (3UL << 22)
#define FB_RM_RP      (1UL << 22)   /* toward +inf: the high tasks */
#define FB_RM_RZ      (3UL << 22)   /* toward zero: the runner     */
static unsigned long fb_fpcr(void)
{
    unsigned long v; __asm__ volatile("mrs %0, fpcr" : "=r"(v)); return v;
}
static void fb_set_rmode(unsigned long m)
{
    unsigned long v = (fb_fpcr() & ~FB_RMODE_MASK) | m;
    __asm__ volatile("msr fpcr, %0" :: "r"(v));
}

static int fb_same(double a, double b)
{
    union { double d; unsigned long u; } x, y;
    x.d = a; y.d = b;
    return x.u == y.u;
}

/* ---- F1/F2: the IRQ-exit switch --------------------------------------- */
#define FB_HI    60
#define FB_ITERS 20000
static volatile INT fb_hrun, fb_during, fb_busy;
static volatile double fb_sink;
static void fb_chunk(double s[8])
{
    double a = s[0], b = s[1], c = s[2], d = s[3], e = s[4], f = s[5], g = s[6], h = s[7];
    for (INT i = 0; i < FB_ITERS; i++) {
        double t = a;
        a = a * 0.9999999 + b * 1e-7;  b = b * 0.9999999 + c * 1e-7;
        c = c * 0.9999999 + d * 1e-7;  d = d * 0.9999999 + e * 1e-7;
        e = e * 0.9999999 + f * 1e-7;  f = f * 0.9999999 + g * 1e-7;
        g = g * 0.9999999 + h * 1e-7;  h = h * 0.9999999 + t * 1e-7;
    }
    s[0] = a; s[1] = b; s[2] = c; s[3] = d; s[4] = e; s[5] = f; s[6] = g; s[7] = h;
}
static void t_fb_high(INT stacd, void *exinf)
{
    double x[8] = { -3.5, 2.25, 9.0, -0.125, 7.75, -6.0, 0.5, 4.0 };
    (void)stacd; (void)exinf;
    for (INT n = 0; n < FB_HI; n++) {
        tk_dly_tsk(2);
        fb_set_rmode(FB_RM_RP);      /* F5: a mode unlike the runner's */
        if (fb_busy) fb_during++;
        fb_chunk(x);
        fb_hrun++;
    }
    fb_sink = x[0];
    tk_ext_tsk();
}
static void fb_irq_part(void)
{
    static const double s0[8] = { 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8 };
    double s[8], r[8];
    INT chunks = 0, diff = 0, rbad = 0;
    for (INT i = 0; i < 8; i++) s[i] = s0[i];
    fb_hrun = fb_during = 0;
    unsigned long fpcr0 = fb_fpcr();
    fb_set_rmode(FB_RM_RZ);
    ID t = fb_mk(t_fb_high, 19);
    if (t > 0) tk_sta_tsk(t, 0);
    fb_busy = 1;
    while (fb_hrun < FB_HI && chunks < 1000000) {
        fb_chunk(s); chunks++;
        if ((fb_fpcr() & FB_RMODE_MASK) != FB_RM_RZ) { rbad++; fb_set_rmode(FB_RM_RZ); }
    }
    fb_busy = 0;
    for (INT n = 0; n < 200 && fb_hrun < FB_HI; n++) tk_dly_tsk(10);
    if (t > 0) { tk_ter_tsk(t); tk_del_tsk(t); }
    for (INT i = 0; i < 8; i++) r[i] = s0[i];
    tk_dis_dsp();
    fb_set_rmode(FB_RM_RZ);
    for (INT k = 0; k < chunks; k++) fb_chunk(r);
    tk_ena_dsp();
    __asm__ volatile("msr fpcr, %0" :: "r"(fpcr0));
    for (INT i = 0; i < 8; i++) if (!fb_same(s[i], r[i])) diff++;
    fb_out("[fpb] irq part: "); fb_dec(chunks); fb_out(" chunks, high rounds ");
    fb_dec(fb_hrun); fb_out("\r\n");
    fb_check(diff == 0, "F1", "doubles unlike the run without switches (of 8)", diff);
    fb_check(fb_during > 0, "F2", "precondition: high FP rounds inside the runner's chunks", fb_during);
    fb_check(rbad == 0, "F5", "chunks after which the runner's FPCR rounding mode had changed (IRQ-exit switch)", rbad);
}

/* ---- F3/F4: callee-saved d8-d15 across a voluntary switch ------------- */
#define FB_VROUNDS 30
static volatile INT fb_vhigh;
static void t_fb_vhigh(INT stacd, void *exinf)
{
    (void)stacd; (void)exinf;
    for (;;) {
        register double h8  __asm__("d8")  = -101.0, h9  __asm__("d9")  = -102.0;
        register double h10 __asm__("d10") = -103.0, h11 __asm__("d11") = -104.0;
        register double h12 __asm__("d12") = -105.0, h13 __asm__("d13") = -106.0;
        register double h14 __asm__("d14") = -107.0, h15 __asm__("d15") = -108.0;
        __asm__ volatile("" : "+w"(h8), "+w"(h9), "+w"(h10), "+w"(h11),
                              "+w"(h12), "+w"(h13), "+w"(h14), "+w"(h15));
        fb_vhigh++;
        fb_set_rmode(FB_RM_RP);      /* F6: a mode unlike the runner's */
        tk_slp_tsk(TMO_FEVR);
        __asm__ volatile("" : "+w"(h8), "+w"(h9), "+w"(h10), "+w"(h11),
                              "+w"(h12), "+w"(h13), "+w"(h14), "+w"(h15));
        fb_sink = h8 + h9 + h10 + h11 + h12 + h13 + h14 + h15;
    }
}
static void fb_vol_part(void)
{
    INT bad = 0, rbad = 0;
    fb_vhigh = 0;
    unsigned long fpcr0 = fb_fpcr();
    ID t = fb_mk(t_fb_vhigh, 19);
    if (t > 0) tk_sta_tsk(t, 0);   /* runs at once (pri 19), pins, sleeps */
    for (INT n = 0; n < FB_VROUNDS; n++) {
        register double r8  __asm__("d8")  = 1.0 + n, r9  __asm__("d9")  = 2.0 + n;
        register double r10 __asm__("d10") = 3.0 + n, r11 __asm__("d11") = 4.0 + n;
        register double r12 __asm__("d12") = 5.0 + n, r13 __asm__("d13") = 6.0 + n;
        register double r14 __asm__("d14") = 7.0 + n, r15 __asm__("d15") = 8.0 + n;
        __asm__ volatile("" : "+w"(r8), "+w"(r9), "+w"(r10), "+w"(r11),
                              "+w"(r12), "+w"(r13), "+w"(r14), "+w"(r15));
        fb_set_rmode(FB_RM_RZ);
        if (t > 0) tk_wup_tsk(t);  /* switch to the high task now */
        __asm__ volatile("" : "+w"(r8), "+w"(r9), "+w"(r10), "+w"(r11),
                              "+w"(r12), "+w"(r13), "+w"(r14), "+w"(r15));
        if ((fb_fpcr() & FB_RMODE_MASK) != FB_RM_RZ) rbad++;
        if (!fb_same(r8, 1.0 + n) || !fb_same(r9, 2.0 + n) ||
            !fb_same(r10, 3.0 + n) || !fb_same(r11, 4.0 + n) ||
            !fb_same(r12, 5.0 + n) || !fb_same(r13, 6.0 + n) ||
            !fb_same(r14, 7.0 + n) || !fb_same(r15, 8.0 + n)) bad++;
    }
    if (t > 0) { tk_ter_tsk(t); tk_del_tsk(t); }
    __asm__ volatile("msr fpcr, %0" :: "r"(fpcr0));
    fb_check(bad == 0, "F3", "rounds whose d8-d15 came back changed (of 30)", bad);
    fb_check(fb_vhigh >= FB_VROUNDS, "F4", "precondition: the high task ran between (of 31)", fb_vhigh);
    fb_check(rbad == 0, "F6", "rounds whose FPCR rounding mode came back changed (of 30, voluntary switch)", rbad);
}

static void t_fb_runner(INT stacd, void *exinf)
{
    (void)stacd; (void)exinf;
    fb_vol_part();
    fb_irq_part();
    fb_done = 1;
    tk_wup_tsk(fb_caller);
    tk_ext_tsk();
}

INT fp_bare_run(void (*out)(const char *))
{
    fb_out = out; fb_fail = 0; fb_done = 0;
    out("[fpb] FP/SIMD registers across task switches, bare AArch64 (runner pri 20, high pri 19)\r\n");
    fb_caller = tk_get_tid();
    ID run = fb_mk(t_fb_runner, 20);
    if (run <= 0) { out("[fpb] FAIL setup: no runner task\r\n"); return 1; }
    tk_can_wup(TSK_SELF);
    tk_sta_tsk(run, 0);
    while (!fb_done && tk_slp_tsk(120000) == E_OK) { }
    if (!fb_done) { out("[fpb] FAIL setup: the runner did not finish within 120 s\r\n"); fb_fail++; tk_ter_tsk(run); }
    tk_del_tsk(run);
    out(fb_fail ? "[fpb] RED\r\n" : "[fpb] ALL PASS\r\n");
    return fb_fail;
}
#endif /* PK_FPBARE */
