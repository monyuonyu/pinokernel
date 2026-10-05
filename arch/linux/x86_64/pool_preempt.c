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
#include "kdds.h"
#include "pfs_block.h"
#include "../../common/llm/pk_parallel.h"
#include "../../common/llm/student.h"
#include "arkfs.h"
#include <stdio.h>
#include <string.h>

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
static const char *pp_tag = "[pool]";
static INT pp_fail;
static ID pp_caller;
static volatile INT pp_done;

static void pp_check(int ok, const char *id, const char *what, long got)
{
    char b[200];
    snprintf(b, sizeof b, "%s %s %s  %s (got %ld)\r\n", pp_tag, ok ? "PASS" : "FAIL", id, what, got);
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

/* ---- `tkconf nosw`: a kdds topic slot under task switches (pk_nosw.h) ----
 * The runner (pri 20) reads one LATEST_ONLY topic over and over and checks
 * that all 192 bytes are one value; a higher task (pri 19) wakes every 2 ms
 * and publishes a new value to the same topic, 60 times.
 *   N1  no torn read (a read with two values in it)
 *   N2  the high task finished its 60 publishes
 *   N3  no starvation: publishes landed while the runner was reading
 *   N4  every p-fs block put by two tasks at once reads back whole
 *   N5  no starvation: high puts started during the runner's puts (asked
 *       only with -DPFS_FAULT_SLOW_PUT: unwidened, the puts end too soon)
 *   N6-N8 the student (ns_student below), N9-N11 ARK (ns_ark below)
 * -DPK_NOSW_OFF (empty regions) must turn N1 red, and N4 red when built with
 * -DPFS_FAULT_SLOW_PUT=2000000 too (green with only the latter). */
#define NS_LEN   192                  /* KDDS_DATA_MAX */
#define NS_PUBS  60
static volatile INT ns_pubs, ns_during, ns_reading;

static void t_ns_high(INT stacd, void *exinf)
{
    (void)stacd; (void)exinf;
    UB b[NS_LEN];
    W h = kdds_open_poll_scoped("tkc/nosw", KDDS_QOS_LATEST_ONLY, KDDS_SCOPE_GLOBAL);
    for (INT n = 0; n < NS_PUBS && h >= 0; n++) {
        tk_dly_tsk(2);
        for (INT i = 0; i < NS_LEN; i++) b[i] = (UB)(2 + n % 250);
        if (ns_reading) ns_during++;
        kdds_pub(h, b, NS_LEN);
        ns_pubs++;
    }
    if (h >= 0) kdds_close(h);
    tk_ext_tsk();
}

/* N4/N5: p-fs puts from two tasks. The runner puts 20 blocks while a pri-19
 * task puts 20 more, one every 2 ms; then every block must read back whole.
 * The pick-a-free-slot -> mark-used window is short, so the test builds widen
 * it with -DPFS_FAULT_SLOW_PUT=<spins>; without it N4 proves little (said). */
#define NP_N 20
static U1 np_idh[NP_N][PFS_ID_LEN];
static volatile INT np_hput, np_during, np_putting;
static void np_fill(UB *b, UB who, INT i)
{
    for (INT k = 0; k < 64; k++) b[k] = (UB)(who ^ (i * 7) ^ (k * 13));
    b[0] = who; b[1] = (UB)i;
}
static void t_np_high(INT stacd, void *exinf)
{
    (void)stacd; (void)exinf;
    UB b[64];
    for (INT n = 0; n < NP_N; n++) {
        tk_dly_tsk(2);
        np_fill(b, 0xB0, n);
        if (np_putting) np_during++;
        pfs_put(b, 64, np_idh[n]);
        np_hput++;
    }
    tk_ext_tsk();
}
static void ns_pfs(void)
{
    static U1 idl[NP_N][PFS_ID_LEN];
    UB b[64], r[64];
    INT lost = 0, full = 0;
    np_hput = np_during = 0;
    ID t = pp_mk(t_np_high, 19);
    if (t > 0) tk_sta_tsk(t, 0);
    np_putting = 1;
    for (INT i = 0; i < NP_N; i++) {
        np_fill(b, 0xA0, i);
        if (pfs_put(b, 64, idl[i]) == PFS_E_FULL) full++;
    }
    np_putting = 0;
    for (INT n = 0; n < 100 && np_hput < NP_N; n++) tk_dly_tsk(10);
    for (INT i = 0; i < NP_N; i++) {
        np_fill(b, 0xA0, i);
        if (pfs_get(idl[i], r, 64) != 64 || memcmp(r, b, 64)) lost++;
        np_fill(b, 0xB0, i);
        if (pfs_get(np_idh[i], r, 64) != 64 || memcmp(r, b, 64)) lost++;
    }
    { char m[200];
      snprintf(m, sizeof m, "[nosw] p-fs: %d + %d puts, table full %d times, window widened: %s\r\n",
               NP_N, (int)np_hput, (int)full,
#ifdef PFS_FAULT_SLOW_PUT
               "yes"
#else
               "no"
#endif
               );
      pp_out(m); }
    pp_check(lost == 0, "N4", "p-fs blocks that do not read back whole (of 40)", lost);
#ifdef PFS_FAULT_SLOW_PUT
    pp_check(np_during > 0, "N5", "no starvation: high puts during the runner's puts", np_during);
#else
    /* unwidened, the runner's 20 puts end before the high task first wakes
     * (measured: 0), so N4 above proves little and N5 is not asked */
    pp_out("[nosw] note: N5 needs -DPFS_FAULT_SLOW_PUT (not asked in this build)\r\n");
#endif
    if (t > 0) { tk_ter_tsk(t); tk_del_tsk(t); }
}

/* N6-N8: the student (pk_nosw.h regions in student.c, ST_TRAIN_STEP at the
 * callers). The runner evaluates, then trains, one model while a pri-19 task
 * wakes every 2 ms and runs st_forward on the SAME model (as the chat or a
 * canon probe on g_student does while the DMN trains it), 60 times a phase.
 * st_forward frees and rebuilds m->cache, which the runner's eval / backward
 * read, and shares tw_buf across models.
 *   N6  every eval of the unchanged model returns the bits it did alone
 *   N7  the trained weights equal a twin trained the same number of steps
 *       alone (bit for bit)
 *   N8  no starvation: the high forwards landed while the runner was busy
 * -DPK_NOSW_OFF must turn N6 and N7 red. */
#define NT_N  32
#define NT_HF 60
static st_model nt_m, nt_ref;
static uint8_t nt_a[NT_N], nt_b[NT_N];
static float nt_hl[NT_N * ST_VOCAB], nt_lg[NT_N * ST_VOCAB];
static volatile INT nt_hrun, nt_during, nt_busy;

static void t_nt_high(INT stacd, void *exinf)
{
    (void)stacd; (void)exinf;
    for (INT n = 0; n < NT_HF; n++) {
        tk_dly_tsk(2);
        if (nt_busy) nt_during++;
        st_forward(&nt_m, nt_b, NT_N, nt_hl);
        nt_hrun++;
    }
    tk_ext_tsk();
}

static void nt_phase_end(ID t)
{
    nt_busy = 0;
    for (INT n = 0; n < 200 && nt_hrun < NT_HF; n++) tk_dly_tsk(10);
    if (t > 0) { tk_ter_tsk(t); tk_del_tsk(t); }
}

static void ns_student(void)
{
    for (INT i = 0; i < NT_N; i++) {
        nt_a[i] = (uint8_t)('a' + (i * 7) % 26);
        nt_b[i] = (uint8_t)('0' + (i * 3) % 10);
    }
    if (st_init(&nt_m, 7) != ST_OK || st_init(&nt_ref, 7) != ST_OK) {
        pp_out("[nosw] FAIL setup: st_init\r\n"); pp_fail++; return;
    }
    int np = 0;
    float l0 = st_eval_loss(&nt_m, nt_a, NT_N, &np);
    float l1 = st_eval_loss(&nt_m, nt_a, NT_N, &np);
    float l2 = (st_forward(&nt_m, nt_b, NT_N, nt_hl), st_eval_loss(&nt_m, nt_a, NT_N, &np));
    float lbad = l0;
    INT during = 0, evals = 0, bad = 0, steps = 0, diff = 0;

    nt_hrun = nt_during = 0;
    ID t = pp_mk(t_nt_high, 19);
    if (t > 0) tk_sta_tsk(t, 0);
    nt_busy = 1;
    while (nt_hrun < NT_HF && evals < 200000) {
        float l = st_eval_loss(&nt_m, nt_a, NT_N, &np);
        evals++;
        if (memcmp(&l, &l0, sizeof l)) { if (!bad) lbad = l; bad++; }
    }
    nt_phase_end(t);
    during += nt_during;

    nt_hrun = nt_during = 0;
    t = pp_mk(t_nt_high, 19);
    if (t > 0) tk_sta_tsk(t, 0);
    nt_busy = 1;
    while (nt_hrun < NT_HF && steps < 100000) {
        ST_TRAIN_STEP(&nt_m, nt_a, NT_N, nt_lg, 0.01f);
        steps++;
    }
    nt_phase_end(t);
    during += nt_during;
    for (INT s = 0; s < steps; s++) ST_TRAIN_STEP(&nt_ref, nt_a, NT_N, nt_lg, 0.01f);
    for (INT i = 0; i < nt_m.n_params; i++)
        if (memcmp(&nt_m.w[i], &nt_ref.w[i], sizeof(float))) diff++;

    { char m[200];
      snprintf(m, sizeof m, "[nosw] student: %d evals, %d train steps, %d params\r\n",
               (int)evals, (int)steps, nt_m.n_params);
      pp_out(m);
      UW b0, b1, b2, bb;
      memcpy(&b0, &l0, 4); memcpy(&b1, &l1, 4); memcpy(&b2, &l2, 4); memcpy(&bb, &lbad, 4);
      snprintf(m, sizeof m, "[nosw] student: eval bits alone %08x %08x, after another forward %08x, first changed %08x\r\n",
               (unsigned)b0, (unsigned)b1, (unsigned)b2, (unsigned)bb);
      pp_out(m); }
    pp_check(bad == 0, "N6", "evals of the unchanged model that changed", bad);
    pp_check(diff == 0, "N7", "weights unlike the twin trained alone", diff);
    pp_check(during > 0, "N8", "no starvation: high forwards while the runner was busy", during);
    st_free(&nt_m); st_free(&nt_ref);
}

/* N9-N11: ARK (pk_nosw.h regions in arkfs.c). On a RAM device of its own the
 * runner puts 200 blocks, then gets and checks them over and over, while a
 * pri-19 task wakes every 2 ms and puts 60 more. Every public ARK entry shares
 * g_blkbuf / g_payld / g_secbuf, the log head and the index.
 *   N9   no get returned another block's bytes or failed, no put failed
 *   N10  afterwards every block of both tasks reads back whole
 *   N11  no starvation: high puts landed while the runner was busy
 * Skipped (said) when p-fs runs on ARK (PKERNEL_PFS_BACKEND=ark): the probe
 * would unmount the live log. -DPK_NOSW_OFF must turn N9 or N10 red. */
extern int pfs_ark_configured(void);          /* arch/linux/pfs_ark.c */
#define NA_SEC 1024
#define NA_LEN 256
#define NA_RUN 200
#define NA_HI  60
static U1 na_ram[NA_SEC * ARK_SECTOR];
static U1 na_idr[NA_RUN][ARK_ID_LEN], na_idh[NA_HI][ARK_ID_LEN];
static volatile INT na_hput, na_hbad, na_during, na_busy;

static INT na_read(void *ctx, U4 lba, U4 n, void *buf)
{
    (void)ctx;
    if (lba + n > NA_SEC) return -1;
    memcpy(buf, na_ram + (size_t)lba * ARK_SECTOR, (size_t)n * ARK_SECTOR);
    return 0;
}
static INT na_write(void *ctx, U4 lba, U4 n, const void *buf)
{
    (void)ctx;
    if (lba + n > NA_SEC) return -1;
    memcpy(na_ram + (size_t)lba * ARK_SECTOR, buf, (size_t)n * ARK_SECTOR);
    return 0;
}
static void na_fill(UB *b, UB who, INT i)
{
    for (INT k = 0; k < NA_LEN; k++) b[k] = (UB)(who ^ (i * 7) ^ (k * 13));
    b[0] = who; b[1] = (UB)i; b[2] = (UB)(i >> 8);
}
static void t_na_high(INT stacd, void *exinf)
{
    (void)stacd; (void)exinf;
    UB b[NA_LEN];
    for (INT n = 0; n < NA_HI; n++) {
        tk_dly_tsk(2);
        na_fill(b, 0xB0, n);
        if (na_busy) na_during++;
        if (ark_block_put(b, NA_LEN, na_idh[n]) != ARK_OK) na_hbad++;
        na_hput++;
    }
    tk_ext_tsk();
}
static void ns_ark(void)
{
    if (pfs_ark_configured()) {
        pp_out("[nosw] note: N9-N11 skipped (p-fs runs on ARK here)\r\n");
        return;
    }
    static ARK_BDEV bd = { ARK_SECTOR, NA_SEC, na_read, na_write, 0, 0 };
    if (ark_format(&bd) != ARK_OK || ark_mount(&bd) != ARK_OK) {
        pp_out("[nosw] FAIL setup: ark format / mount\r\n"); pp_fail++; return;
    }
    UB b[NA_LEN], r[NA_LEN];
    INT nr = 0, gets = 0, bad = 0, lost = 0;
    na_hput = na_hbad = na_during = 0;
    ID t = pp_mk(t_na_high, 19);
    if (t > 0) tk_sta_tsk(t, 0);
    na_busy = 1;
    for (INT it = 0; it < 2000000 && na_hput < NA_HI; it++) {
        if (nr < NA_RUN) {
            na_fill(b, 0xA0, nr);
            if (ark_block_put(b, NA_LEN, na_idr[nr]) != ARK_OK) bad++;
            nr++;
        } else {
            INT i = it % NA_RUN;
            na_fill(b, 0xA0, i);
            if (ark_block_get(na_idr[i], r, NA_LEN) != NA_LEN || memcmp(r, b, NA_LEN)) bad++;
            gets++;
        }
    }
    na_busy = 0;
    for (INT n = 0; n < 200 && na_hput < NA_HI; n++) tk_dly_tsk(10);
    if (t > 0) { tk_ter_tsk(t); tk_del_tsk(t); }
    for (INT i = 0; i < nr; i++) {
        na_fill(b, 0xA0, i);
        if (ark_block_get(na_idr[i], r, NA_LEN) != NA_LEN || memcmp(r, b, NA_LEN)) lost++;
    }
    for (INT i = 0; i < na_hput; i++) {
        na_fill(b, 0xB0, i);
        if (ark_block_get(na_idh[i], r, NA_LEN) != NA_LEN || memcmp(r, b, NA_LEN)) lost++;
    }
    ark_unmount();
    { char m[200];
      snprintf(m, sizeof m, "[nosw] ark: %d + %d puts (%d high puts failed), %d gets\r\n",
               (int)nr, (int)na_hput, (int)na_hbad, (int)gets);
      pp_out(m); }
    pp_check(bad == 0 && na_hbad == 0, "N9", "ark gets / puts that failed or crossed", bad + na_hbad);
    pp_check(lost == 0, "N10", "ark blocks that do not read back whole", lost);
    pp_check(na_during > 0, "N11", "no starvation: high puts while the runner was busy", na_during);
}

static void t_ns_runner(INT stacd, void *exinf)
{
    (void)stacd; (void)exinf;
    UB b[NS_LEN];
    INT torn = 0, reads = 0;
    ns_pubs = ns_during = 0;
    W h = kdds_open_poll_scoped("tkc/nosw", KDDS_QOS_LATEST_ONLY, KDDS_SCOPE_GLOBAL);
    if (h < 0) { pp_out("[nosw] FAIL setup: kdds_open\r\n"); pp_fail++; goto done; }
    for (INT i = 0; i < NS_LEN; i++) b[i] = 1;
    kdds_pub(h, b, NS_LEN);
    ID t = pp_mk(t_ns_high, 19);
    if (t > 0) tk_sta_tsk(t, 0);
    ns_reading = 1;
    for (INT r = 0; r < 4000000 && ns_pubs < NS_PUBS; r++) {
        if (kdds_sub(h, b, NS_LEN, 0) != NS_LEN) continue;
        reads++;
        for (INT i = 1; i < NS_LEN; i++) if (b[i] != b[0]) { torn++; break; }
    }
    ns_reading = 0;
    for (INT n = 0; n < 100 && ns_pubs < NS_PUBS; n++) tk_dly_tsk(10);
    { char m[200];
      snprintf(m, sizeof m, "[nosw] %d reads, %d publishes\r\n", (int)reads, (int)ns_pubs);
      pp_out(m); }
    pp_check(torn == 0, "N1", "torn reads of the topic slot", torn);
    pp_check(ns_pubs == NS_PUBS, "N2", "high task: publishes finished (of 60)", ns_pubs);
    pp_check(ns_during > 0, "N3", "no starvation: publishes while the runner was reading", ns_during);
    if (t > 0) { tk_ter_tsk(t); tk_del_tsk(t); }
    kdds_close(h);
    ns_pfs();
    ns_student();
    ns_ark();
done:
    pp_done = 1;
    tk_wup_tsk(pp_caller);
    tk_ext_tsk();
}

INT nosw_run(void (*out)(const char *))
{
    pp_out = out; pp_fail = 0; pp_done = 0; pp_tag = "[nosw]";
    out("[nosw] a kdds topic slot under task switches (runner pri 20, publisher pri 19)\r\n");
    pp_caller = tk_get_tid();
    ID run = pp_mk(t_ns_runner, 20);
    if (run <= 0) { out("[nosw] FAIL setup: no runner task\r\n"); return 1; }
    tk_can_wup(TSK_SELF);
    tk_sta_tsk(run, 0);
    while (!pp_done && tk_slp_tsk(120000) == E_OK) { }
    if (!pp_done) { out("[nosw] FAIL setup: the runner did not finish within 120 s\r\n"); pp_fail++; tk_ter_tsk(run); }
    tk_del_tsk(run);
    out(pp_fail ? "[nosw] RED\r\n" : "[nosw] ALL PASS\r\n");
    return pp_fail;
}

INT pool_preempt_run(void (*out)(const char *))
{
    pp_out = out; pp_fail = 0; pp_done = 0; pp_tag = "[pool]";
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
