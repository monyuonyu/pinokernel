/*
 *  pool_preempt.c — `tkconf pool`: the shared matmul pool under the hosted
 *  exit dispatch (audit-21 found it split; the probe is pk-scratch/aud21-hed-
 *  probe.py, this is its test form).
 *
 *  A runner task (pri 20) drives pk_parallel_rows 150 times with slow rows
 *  while a higher task (pri 19) wakes every 3 ms and drives the same pool 40
 *  times. Every row of every call must come back with that call's value.
 *    [pool] PASS P1  no wrong rows in the low task's calls
 *    [pool] PASS P2  no wrong rows in the high task's calls
 *    [pool] PASS P3  the high task finished its 40 calls
 *    [pool] PASS P4  precondition: the pool really ran in parallel (helper wakes)
 *    [pool] PASS P5  no starvation: high calls started inside the low loop
 *  Negative controls: -DPK_POOL_PREEMPTIBLE (busy not counted) turns P1 and
 *  P2 red; -DPK_POOL_NO_LEAVE_DISPATCH (no switch when the pool is left) and
 *  -DPK_HOSTED_NO_EXIT_DISPATCH (the old behaviour, RNG0) turn P5 red. Needs
 *  >= 2 CPUs (else P4 is red: the pool runs inline and proves nothing).
 */
#include "kernel.h"
#include "../../common/llm/pk_parallel.h"
#include <stdio.h>

#define PP_ROWS   512
#define PP_LCALLS 150
#define PP_HCALLS 40

struct pp_ctx { volatile UW *buf; UW val; UW work; };
static volatile UW pp_L[PP_ROWS], pp_H[PP_ROWS];
static volatile UW pp_slow;
static volatile INT pp_h_runs, pp_h_bad, pp_h_during, pp_l_active;

static void pp_body(void *ctx, size_t i0, size_t i1)
{
    struct pp_ctx *c = ctx;
    for (size_t i = i0; i < i1; i++) {
        for (UW k = 0; k < c->work; k++) pp_slow++;
        c->buf[i] = c->val;
    }
}

static void t_pp_high(INT stacd, void *exinf)
{
    (void)exinf;
    for (INT n = 0; n < stacd; n++) {
        struct pp_ctx c = { pp_H, 7000 + (UW)n, 200 };
        tk_dly_tsk(3);
        for (INT i = 0; i < PP_ROWS; i++) pp_H[i] = 0;
        if (pp_l_active) pp_h_during++;
        pk_parallel_rows(PP_ROWS, pp_body, &c);
        INT bad = 0;
        for (INT i = 0; i < PP_ROWS; i++) if (pp_H[i] != c.val) bad++;
        if (bad) pp_h_bad++;
        pp_h_runs++;
    }
    tk_ext_tsk();
}

static ID pp_mk(void (*fn)(INT, void *), PRI pri)
{
    T_CTSK ct = { .exinf = NULL, .tskatr = TA_HLNG | TA_RNG0,
                  .task = (FP)fn, .itskpri = pri, .stksz = 8192 };
    return tk_cre_tsk(&ct);
}

static void (*pp_out)(const char *);
static INT pp_fail;
static ID pp_caller;
static volatile INT pp_done;

static void pp_check(int ok, const char *id, const char *what, long got)
{
    char b[200];
    snprintf(b, sizeof b, "[pool] %s %s  %s (got %ld)\r\n", ok ? "PASS" : "FAIL", id, what, got);
    pp_out(b);
    if (!ok) pp_fail++;
}

static void t_pp_runner(INT stacd, void *exinf)
{
    (void)stacd; (void)exinf;
    INT lbad = 0;
    unsigned long w0 = pk_parallel_wake_count();
    pp_h_runs = pp_h_bad = pp_h_during = 0;
    ID t = pp_mk(t_pp_high, 19);
    if (t > 0) tk_sta_tsk(t, PP_HCALLS);
    pp_l_active = 1;
    for (INT n = 0; n < PP_LCALLS; n++) {
        struct pp_ctx c = { pp_L, 1000 + (UW)n, 4000 };
        for (INT i = 0; i < PP_ROWS; i++) pp_L[i] = 0;
        pk_parallel_rows(PP_ROWS, pp_body, &c);
        INT bad = 0;
        for (INT i = 0; i < PP_ROWS; i++) if (pp_L[i] != c.val) bad++;
        if (bad) lbad++;
    }
    pp_l_active = 0;
    for (INT n = 0; n < 200 && pp_h_runs < PP_HCALLS; n++) tk_dly_tsk(10);
    unsigned long wakes = pk_parallel_wake_count() - w0;
    pp_check(lbad == 0, "P1", "low task: calls with a wrong row (of 150)", lbad);
    pp_check(pp_h_bad == 0, "P2", "high task: calls with a wrong row", pp_h_bad);
    pp_check(pp_h_runs == PP_HCALLS, "P3", "high task: calls finished (of 40)", pp_h_runs);
    pp_check(wakes > 0, "P4", "precondition: pool helper wakes", (long)wakes);
    pp_check(pp_h_during > 0, "P5", "no starvation: high calls started inside the low loop", pp_h_during);
    if (t > 0) { tk_ter_tsk(t); tk_del_tsk(t); }
    pp_done = 1;
    tk_wup_tsk(pp_caller);
    tk_ext_tsk();
}

INT pool_preempt_run(void (*out)(const char *))
{
    pp_out = out; pp_fail = 0; pp_done = 0;
    out("[pool] the shared matmul pool under task switches (runner pri 20, high pri 19)\r\n");
    pp_caller = tk_get_tid();
    ID run = pp_mk(t_pp_runner, 20);
    if (run <= 0) { out("[pool] FAIL setup: no runner task\r\n"); return 1; }
    tk_can_wup(TSK_SELF);
    tk_sta_tsk(run, 0);
    while (!pp_done && tk_slp_tsk(120000) == E_OK) { }
    if (!pp_done) { out("[pool] FAIL setup: the runner did not finish within 120 s\r\n"); pp_fail++; tk_ter_tsk(run); }
    tk_del_tsk(run);
    out(pp_fail ? "[pool] RED\r\n" : "[pool] ALL PASS\r\n");
    return pp_fail;
}
