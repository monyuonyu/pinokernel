/*
 *  student_ema_test.c — host cert for the SAVED-COPY EMA of the resident baby
 *  (research/lr-schedule.md "where it goes"; arch/common/llm/student_shell.c).
 *
 *  Links the REAL student_shell.c (#include, like student_yield_test.c) and
 *  captures what the DMN persist path writes. Drives T DMN ticks of the
 *  production student_dmn_consolidate() on the TEACHER_FIXTURE.
 *
 *  PKERNEL_STUDENT_EMA unset (the OFF arm, today's behaviour):
 *   [ema-off-identical]   every DMN write is byte-identical to st_save(live)
 *                         at that moment, and no copy is ever allocated.
 *  PKERNEL_STUDENT_EMA=1 (the ON arm):
 *   [ema-live-unchanged]  after every tick the live weights are bit-identical
 *                         to a reference trained by sleep_rounds alone (the
 *                         copy never touches training).
 *   [ema-picks-lower]     every DMN write holds the lower-held of live and the
 *                         copy (its held-out loss == min of the two, <= live).
 *   [ema-helps]           pre-registered from the probe (lr_sched_probe ema
 *                         100, K=8, second half): in the second half of the
 *                         ticks, every write that happened is the copy, and
 *                         at least one happened.
 *  FALSIFIER (-DEMA_FOLD_DISABLE): no copy is ever folded -> live is always
 *  written -> [ema-helps] goes RED while the other ON checks stay green.
 *
 *  Build: -std=c11 -O1 -Wall -Wextra -ffp-contract=off (see run_ema.sh).
 */
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../arch/common/llm/student_shell.c"

/* ---------- kernel-seam stubs (as in student_yield_test.c) ---------- */
static unsigned char *g_cap = NULL;       /* the last DMN write */
static unsigned       g_cap_len = 0;
static unsigned       g_cap_count = 0;
int pfs_dur_active(void) { return 1; }
int pfs_dur_write(const char *f, const void *d, unsigned n)
{
    (void)f;
    unsigned char *b = (unsigned char *)realloc(g_cap, n);
    if (!b) return -1;
    memcpy(b, d, n); g_cap = b; g_cap_len = n; g_cap_count++;
    return 0;
}
int pfs_dur_read (const char *f, void *b, unsigned m) { (void)f;(void)b;(void)m; return -1; }
int st_init_device(st_model *m, uint32_t seed) { return st_init(m, seed); }
int  gguf_open (gguf_file *gf, const char *p) { (void)gf;(void)p; return -1; }
void gguf_close(gguf_file *gf)                { (void)gf; }
int  lm_load   (lm_model *m, const gguf_file *gf) { (void)m;(void)gf; return -1; }
void lm_free   (lm_model *m)                  { (void)m; }
void     ss6_live_install(void *m)   { (void)m; }
void     ss6_live_uninstall(void)    { }
void     ss6_live_set_enabled(int o) { (void)o; }
unsigned ss6_live_req_sent(void)     { return 0; }
unsigned ss6_live_req_served(void)   { return 0; }

static int g_pass = 0, g_fail = 0;
#define CHECK(cond, msg) do { \
    if (cond) { printf("  PASS  %s\n", (msg)); g_pass++; } \
    else      { printf("  FAIL  %s\n", (msg)); g_fail++; } } while (0)

/* [ema-writer-kept] (audit-35): the copy's held-out evaluation is many
 * st_eval_loss calls, and the exit dispatch may run a higher writer of
 * g_student between two of them. Linked with -Wl,--wrap=st_eval_loss, the
 * first evaluation of the copy (m->w == g_ema) runs such a writer once, as the
 * `student` retrain / merge paths do: student_consol_abort(), then a write to
 * g_student.w. The write must stay in live, and the DMN must not then write
 * the copy it evaluated before the abort. */
float __real_st_eval_loss(st_model *m, const uint8_t *b, int n, int *np);
static int    g_wr_armed = 0, g_wr_done = 0;
static float *g_wr_live = NULL;
static float  g_wr_v0 = 0.0f;
float __wrap_st_eval_loss(st_model *m, const uint8_t *b, int n, int *np)
{
    if (g_wr_armed && !g_wr_done && g_ema && m->w == g_ema) {
        g_wr_done = 1;
        g_wr_v0 = g_wr_live[0];
        student_consol_abort();
        g_student.w[0] += 0.5f;
    }
    return __real_st_eval_loss(m, b, n, np);
}

static int writer_arm(void)
{
    printf("[ema] writer arm: a higher writer runs inside the copy's evaluation\n");
    g_have_student = 0; g_consol_active = 0; g_consol_idx = 0;
    if (student_ensure(0) != 0) { printf("[ema] student_ensure OOM\n"); return 2; }
    unsigned wrote_after = 0, copy_saves = 0;
    /* armed from tick 5: the ON arm's copy wins from tick 4 on, so the DMN is
     * about to write the copy when the writer cuts in (the generation check
     * is exercised, not only the evaluation) */
    for (int t = 1; t <= 8 && !g_wr_done; t++) {
        int rc = 0, calls = 0;
        do {
            unsigned before = g_cap_count, esv = g_ema_saves;
            g_wr_live = g_student.w;
            g_wr_armed = t >= 5;
            rc = student_dmn_consolidate(); calls++;
            g_wr_armed = 0;
            if (g_wr_done) { wrote_after = g_cap_count - before; copy_saves = g_ema_saves - esv; }
        } while (rc == 0 && calls < 100000 && !g_wr_done);
    }
    if (!g_wr_done) { printf("[ema] the copy was never evaluated\n"); return 2; }
    float dv = g_wr_live[0] - g_wr_v0;   /* later Adam steps move w[0] by ~1e-2 at most */
    /* a write in that call may be live (the copy lost, or it was dropped): it
     * must then carry the writer's update; the stale copy must not be written */
    float saved0 = g_wr_live[0];
    if (wrote_after) {
        st_model chk;
        if (st_init(&chk, STUDENT_SEED) != ST_OK || st_load(&chk, g_cap, g_cap_len) != ST_OK) return 2;
        saved0 = chk.w[0];
        st_free(&chk);
    }
    printf("[ema] live[0] %.6f -> %.6f (writer added 0.5), DMN writes in that call: %u"
           " (copy writes %u, written w[0] %.6f)\n",
           (double)g_wr_v0, (double)g_wr_live[0], wrote_after, copy_saves, (double)saved0);
    CHECK(g_student.w == g_wr_live && dv > 0.25f && dv < 0.75f,
          "[ema-writer-kept] the writer's update is in live");
    CHECK(copy_saves == 0 && saved0 == g_wr_live[0],
          "[ema-writer-kept] the copy evaluated before the abort is not written (a write is live)");
    printf("[result] %s (%d pass, %d fail)\n", g_fail ? "FAIL" : "PASS", g_pass, g_fail);
    return g_fail ? 1 : 0;
}

int main(int argc, char **argv)
{
    if (argc > 1 && !strcmp(argv[1], "writer")) return writer_arm();
    int T = argc > 1 ? atoi(argv[1]) : 8;
    int on = student_ema_on();
    int corpus_n  = (int)sizeof(TEACHER_FIXTURE) - 1;
    int total_win = corpus_n / ST_DMN_SEQLEN;
    int trainw    = total_win * 3 / 4; if (trainw < 2) trainw = 2;
    int heldw     = total_win - trainw; if (heldw < 1) heldw = 1;
    int train_end = trainw * ST_DMN_SEQLEN;
    printf("[ema] arm=%s ticks=%d trainw=%d passes/tick=%d half-life=%.0f steps\n",
           on ? "ON" : "OFF", T, trainw, ST_DMN_ROUNDS * trainw, (double)ST_EMA_HALF);
#ifdef EMA_FOLD_DISABLE
    printf("[ema] *** built with -DEMA_FOLD_DISABLE (falsifier: no copy) ***\n");
#endif

    st_model ref, tmp;
    if (st_init(&ref, STUDENT_SEED) != ST_OK || st_init(&tmp, STUDENT_SEED) != ST_OK) {
        printf("[ema] st_init OOM\n"); return 2;
    }
    g_have_student = 0; g_consol_active = 0; g_consol_idx = 0;
    if (student_ensure(0) != 0) { printf("[ema] student_ensure OOM\n"); return 2; }
    unsigned base = g_cap_count;          /* student_ensure may have saved the newborn */

    size_t cap = st_blob_size(&g_student);
    unsigned char *live_blob = (unsigned char *)malloc(cap);
    if (!live_blob) return 2;
    int off_ident = 1, live_same = 1, picks = 1, late_writes = 0, late_copy = 0, writes = 0;
    for (int t = 1; t <= T; t++) {
        unsigned before = g_cap_count;
        int rc, calls = 0;
        do { rc = student_dmn_consolidate(); calls++; } while (rc == 0 && calls < 100000);
        sleep_rounds(&ref, ST_DMN_SEQLEN, trainw, ST_DMN_ROUNDS, ST_DMN_LR);
        if (memcmp(ref.w, g_student.w, (size_t)ref.n_params * sizeof(float)) != 0) live_same = 0;

        float lh = heldout_loss(&g_student, ST_DMN_SEQLEN, train_end, heldw);
        float ch = -1.0f;
        if (g_ema_have) {
            float *lw = g_student.w; g_student.w = g_ema;
            ch = heldout_loss(&g_student, ST_DMN_SEQLEN, train_end, heldw);
            g_student.w = lw;
        }
        int wrote = g_cap_count != before;
        float sh = -1.0f; int is_copy = 0;
        if (wrote) {
            writes++;
            long ln = st_save(&g_student, live_blob, cap);
            if (!on && !(ln == (long)g_cap_len && memcmp(live_blob, g_cap, g_cap_len) == 0)) off_ident = 0;
            if (st_load(&tmp, g_cap, g_cap_len) == ST_OK) {
                sh = heldout_loss(&tmp, ST_DMN_SEQLEN, train_end, heldw);
                is_copy = g_ema_have && g_ema && memcmp(tmp.w, g_ema, (size_t)tmp.n_params * sizeof(float)) == 0;
                float want = (g_ema_have && ch < lh) ? ch : lh;
                if (!(sh == want && sh <= lh)) picks = 0;
            } else picks = 0;
            if (t > T / 2) { late_writes++; late_copy += is_copy; }
        }
        printf("[ema] tick=%d live=%.4f copy=%s%.4f wrote=%d saved=%.4f%s\n", t, (double)lh,
               g_ema_have ? "" : "(none) ", (double)ch, wrote, (double)sh, is_copy ? " (copy)" : "");
        fflush(stdout);
    }
    printf("[ema] writes=%d (newborn save excluded: base=%u) ema_saves=%u late_writes=%d late_copy=%d\n",
           writes, base, g_ema_saves, late_writes, late_copy);
    if (!on) {
        CHECK(writes > 0 && off_ident, "[ema-off-identical] every DMN write == st_save(live)");
        CHECK(g_ema == NULL, "[ema-off-identical] no copy allocated");
    } else {
        CHECK(live_same, "[ema-live-unchanged] live == sleep_rounds reference after every tick");
        CHECK(writes > 0 && picks, "[ema-picks-lower] every write is the lower-held of live / copy");
        CHECK(late_writes > 0 && late_copy == late_writes,
              "[ema-helps] every second-half write is the copy (pre-registered)");
    }
    free(live_blob); st_free(&ref); st_free(&tmp); free(g_cap);
    printf("[result] %s (%d pass, %d fail)\n", g_fail ? "FAIL" : "PASS", g_pass, g_fail);
    return g_fail ? 1 : 0;
}
