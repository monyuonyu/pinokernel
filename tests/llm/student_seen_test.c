/*
 * student_seen_test.c — [held-seen] (ROADMAP 0-12 step 4; student_shell.c's
 * record of what the baby trained on, held_indep.h st_seen)
 *
 * Drives the REAL student_shell.c (#included, like student_yield_test.c) and
 * the real cradle ring:
 *   S1  before any training the record is empty: lesson B's held is "seen" 0.
 *   S2  a DMN batch starts on lesson A (canonical) and is aborted, then
 *       lesson B replaces A and a batch starts on B. B's held repeats A's
 *       train bytes: B against its own train is 0, the record says 170/170.
 *   S3  the same lesson again does not add twice (the fingerprint).
 *   S4  `student loss` prints the "[held-indep] seen=" line with that count.
 *   S5  with restored weights it also says the held loss chose them
 *       ("selected-by-held") and that the record misses earlier training;
 *       fresh weights print neither.
 * Falsifier: -DSEEN_DISABLE (student_seen_note records nothing) -> S2-S4 RED.
 */
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../arch/common/llm/student_shell.c"

/* kernel seams, as in student_yield_test.c (never reached on this path) */
int pfs_dur_active(void) { return 1; }   /* the DMN only runs with persistence on */
int pfs_dur_write(const char *f, const void *d, unsigned n) { (void)f;(void)d;(void)n; return 0; }
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
/* conscience / frontier link stubs: held_seen_stubs.c (their types differ
 * from student_shell.c's declarations, so they live in their own TU) */

static int fails;
static void check(int ok, const char *id, const char *what)
{
    printf("[held-seen] %s %s  %s\n", ok ? "PASS" : "FAIL", id, what);
    if (!ok) fails++;
}

static char cap[8192];
static int cap_n;
static void capture(const char *s)
{
    int l = (int)strlen(s);
    if (cap_n + l < (int)sizeof cap) { memcpy(cap + cap_n, s, (size_t)l + 1); cap_n += l; }
}

/* start one DMN batch on the current ring, then drop it (the record is
 * written at batch start; the passes themselves do not matter here) */
static void one_batch_start(void)
{
    (void)student_dmn_consolidate();
    student_consol_abort();
}

int main(void)
{
    static uint8_t A[CRADLE_RING_BYTES], B[1280];
    int na = cradle_compose_canon(A, (int)sizeof A, NULL);
    st_split sb = st_held_split(1280, CRADLE_SEQLEN);
    uint32_t s = 0xB0B0u;
    for (int i = 0; i < sb.train_end; i++) {
        s = s * 1664525u + 1013904223u;
        B[i] = (uint8_t)("abcdefghijklmnopqrstuvwxyz     "[(s >> 16) % 31]);
    }
    memcpy(B + sb.train_end, A, (size_t)(1280 - sb.train_end));   /* B's held = A's train bytes */

    if (student_ensure(0) != 0) { printf("[held-seen] FAIL setup: no arena\n"); return 1; }
    cradle_set_enabled(1);
    cradle_lesson_clear();

    st_held_report r;
    st_held_check_record(B, 1280, CRADLE_SEQLEN, sb.trainw, sb.heldw, &g_seen, ST_HELD_K, &r);
    printf("[held-seen] S1 record %u runs, B seen %d/%d\n", (unsigned)g_seen.added, r.shared, r.held_kgrams);
    check(g_seen.added == 0 && r.held_kgrams > 0 && r.shared == 0, "S1", "empty record before training");

    int ia = cradle_lesson_ingest(A, na);
    one_batch_start();                         /* trains A */
    int ib = cradle_lesson_ingest(B, 1280);    /* B replaces A */
    one_batch_start();                         /* trains B */
    unsigned after2 = (unsigned)g_seen.added;
    st_held_report own;
    st_held_check(B, 1280, CRADLE_SEQLEN, sb.trainw, sb.heldw, ST_HELD_K, &own);
    st_held_check_record(B, 1280, CRADLE_SEQLEN, sb.trainw, sb.heldw, &g_seen, ST_HELD_K, &r);
    printf("[held-seen] S2 ingest A %d B %d, record %u runs; B own %d/%d, record %d/%d\n",
           ia, ib, after2, own.shared, own.held_kgrams, r.shared, r.held_kgrams);
    check(ia == na && ib == 1280 && own.shared == 0 && r.shared == r.held_kgrams && r.held_kgrams == 170,
          "S2", "lesson swap: the record sees B's held in A's train");

    one_batch_start();                         /* B again: no new adds */
    printf("[held-seen] S3 record %u -> %u runs after the same lesson again\n", after2, (unsigned)g_seen.added);
    check(after2 > 0 && g_seen.added == after2, "S3", "the same corpus is recorded once");

    cap_n = 0; cap[0] = 0;
    (void)student_shell_cmd("loss", capture);
    const char *ln = strstr(cap, "[held-indep] seen=");
    char want[32];
    snprintf(want, sizeof want, "seen=%d/%d ", r.shared, r.held_kgrams);
    if (ln) { const char *e = strchr(ln, '\r'); printf("[held-seen] S4 %.*s\n", e ? (int)(e - ln) : 120, ln); }
    check(ln && strncmp(ln + 13, want, strlen(want)) == 0, "S4", "`student loss` prints the record's count");
    int fresh_quiet = strstr(cap, "selected-by-held") == NULL;

    /* S5: restored weights were picked by the held loss (the DMN saves on
     * improvement): `student loss` must say the held score is not independent */
    g_loaded_from_disk = 1;
    cap_n = 0; cap[0] = 0;
    (void)student_shell_cmd("loss", capture);
    g_loaded_from_disk = 0;
    int said = strstr(cap, "[held-indep] selected-by-held") != NULL &&
               strstr(cap, "weights RESTORED: earlier training not recorded") != NULL;
    check(fresh_quiet && said, "S5", "restored weights: held loss marked as selected-by-held, fresh: quiet");

    printf("[held-seen] %s\n", fails ? "RED" : "ALL PASS");
    return fails ? 1 : 0;
}
