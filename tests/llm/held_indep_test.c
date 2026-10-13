/*
 * held_indep_test.c — [held-indep] (ROADMAP 0-12; arch/common/llm/held_indep.c)
 *
 * Does the held-out set share bytes with what the model trained on?
 *
 *   I1  the canonical cradle lesson (cradle_compose_canon, the bytes of the
 *       [cradle-teach] cert and of the live teacher), production split at
 *       CRADLE_SEQLEN: shared == 0 and wrapped == 0.
 *       Expected RED until the lesson is fixed: its held tail is CT_FILLER,
 *       the same sentence that fills the train region.
 *   Instrument checks (must hold, or I1 means nothing):
 *   I2  held == train (a corpus that is one block repeated): every held
 *       k-gram is shared (the check can see an overlap).
 *   I3  LCG bytes, no repeats: 0 shared, 0 wrapped (no false alarm).
 *   I4  a corpus too short for the split (n=100, seqlen=64 -> the production
 *       clamps give trainw=2, heldw=1): the held window wraps -> wrapped > 0.
 *   I5  st_held_split (held_indep.h, the one production split) is the 3/4
 *       split with its clamps, and its wrapped count equals the byte check's,
 *       for n 1..2048 x seqlen 1..64.
 *   I6  st_held_measurable is exactly "no held window wraps" on that grid.
 *   I7  cradle_live_teach_test with a live text >= the cert budget (it tiled
 *       to 0 bytes) answers UNMEASURABLE, rc 1 (was: a held loss mod 0).
 *   I8  step 4: the canonical lesson's held windows share no k-gram with the
 *       TEACHER_FIXTURE bytes a node trained before the lesson came.
 *   I9  instrument for I8 (st_held_check_seen): seen = the held bytes -> all
 *       shared, seen = random bytes -> none.
 *   I10 the training record (st_seen, a Bloom filter): fixture, lesson A, then
 *       lesson B replaces A and B's held repeats A's train bytes. B against
 *       its own train says 0; the record must count every one (== exact).
 *   I11 the canonical lesson against the record stays 0 (I1 and I8 agree).
 *   I12 random held bytes are rarely "seen" (<= 2 of 170), an empty record
 *       sees nothing.
 *
 * Exit 0 = all PASS. Prints "[held-indep] PASS|FAIL <id> ..." per item.
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "../../arch/common/llm/student.h"
#include "../../arch/common/llm/held_indep.h"
#include "../../arch/common/llm/teacher_fixture.h"

static int fails;

static void item(int ok, const char *id, const char *what, const st_held_report *r)
{
    printf("[held-indep] %s %s  %s  (held k-grams %d, shared %d, wrapped %d)\n",
           ok ? "PASS" : "FAIL", id, what, r->held_kgrams, r->shared, r->wrapped);
    if (!ok) fails++;
}

static char cap[4096];
static int cap_n;
static void capture(const char *s)
{
    int l = (int)strlen(s);
    if (cap_n + l < (int)sizeof cap) { memcpy(cap + cap_n, s, (size_t)l + 1); cap_n += l; }
}

/* the production split: st_held_split (held_indep.h), the one every
 * student_shell.c / cradle.c site takes */
static void split(int n, int seqlen, int *trainw, int *heldw)
{
    st_split sp = st_held_split(n, seqlen);
    *trainw = sp.trainw; *heldw = sp.heldw;
}

int main(void)
{
    static uint8_t buf[CRADLE_RING_BYTES];
    st_held_report r;
    int tw, hw, n;

    /* I1 */
    n = cradle_compose_canon(buf, (int)sizeof buf, NULL);
    if (n <= 0) { printf("[held-indep] FAIL I1 setup: no canonical lesson\n"); return 1; }
    split(n, CRADLE_SEQLEN, &tw, &hw);
    printf("[held-indep] canonical lesson %d B, seqlen %d, train %d win, held %d win, k %d\n",
           n, CRADLE_SEQLEN, tw, hw, ST_HELD_K);
    int rc = st_held_check(buf, n, CRADLE_SEQLEN, tw, hw, ST_HELD_K, &r);
    item(rc == 0, "I1", "canonical cradle lesson: held shares no k-gram with train", &r);

    /* I2: 1280 B = one 256 B block x5 -> held windows equal train windows */
    for (int i = 0; i < 1280; i++) buf[i] = (uint8_t)("abcdefghijklmnopqrstuvwxyz0123456789"[(i % 256) % 36] + (i % 256) / 36);
    split(1280, 32, &tw, &hw);
    rc = st_held_check(buf, 1280, 32, tw, hw, ST_HELD_K, &r);
    item(rc == 1 && r.held_kgrams > 0 && r.shared == r.held_kgrams, "I2",
         "instrument: a repeated block -> every held k-gram is shared", &r);

    /* I3: LCG bytes */
    uint32_t s = 0x5EED1234u;
    for (int i = 0; i < 1280; i++) { s = s * 1664525u + 1013904223u; buf[i] = (uint8_t)(s >> 16); }
    rc = st_held_check(buf, 1280, 32, tw, hw, ST_HELD_K, &r);
    item(rc == 0 && r.held_kgrams > 0, "I3", "instrument: random bytes -> nothing shared, nothing wrapped", &r);

    /* I4: too short -> the clamped split wraps */
    split(100, 64, &tw, &hw);
    rc = st_held_check(buf, 100, 64, tw, hw, ST_HELD_K, &r);
    item(rc == 1 && r.wrapped > 0, "I4", "instrument: n=100 seqlen=64 -> the held window wraps", &r);

    /* I5: st_held_split is the 3/4 split with its clamps, and its wrapped
     * count agrees with the byte check, for every geometry n 1..2048 x
     * seqlen 1..64. The byte check itself runs for seqlen 16/32/48/64 and
     * n <= 400; elsewhere the wrap is counted by st_held_check's rule */
    int bad = 0, cases = 0, wrapcases = 0;
    for (int sl = 1; sl <= 64 && !bad; sl++)
        for (int nn = 1; nn <= 2048 && !bad; nn++) {
            st_split sp = st_held_split(nn, sl);
            int t = nn / sl;
            int etw = t * 3 / 4 < 2 ? 2 : t * 3 / 4;
            int ehw = t - etw < 1 ? 1 : t - etw;
            st_held_report q;
            if (sl < ST_HELD_K || (sl & 15) || nn > 400) {   /* byte check: a small grid (it is slow) */
                q.wrapped = 0;
                for (int w = etw; w < etw + ehw; w++) if (w * sl + sl > nn) q.wrapped++;
            } else (void)st_held_check(buf, nn, sl, etw, ehw, ST_HELD_K, &q);
            int spw = st_held_wrapped(sp, nn, sl);
            cases++; if (spw) wrapcases++;
            if (sp.trainw != etw || sp.heldw != ehw || sp.train_end != etw * sl || spw != q.wrapped) {
                bad = 1;
                printf("[held-indep] I5 mismatch n=%d seqlen=%d: split %d/%d/%d wrap %d, expected %d/%d/%d wrap %d\n",
                       nn, sl, sp.trainw, sp.heldw, sp.train_end, spw, etw, ehw, etw * sl, q.wrapped);
            }
        }
    printf("[held-indep] %s I5  st_held_split = the 3/4 split, wrapped = the byte check  (%d geometries, %d wrap)\n",
           bad ? "FAIL" : "PASS", cases, wrapcases);
    if (bad) fails++;

    /* I6: st_held_measurable is "the split's held windows do not wrap" (and
     * no corpus is unmeasurable), on the same grid */
    bad = 0;
    if (st_held_measurable(0, 32) || st_held_measurable(100, 0)) bad = 1;
    for (int sl = 1; sl <= 64 && !bad; sl++)
        for (int nn = 1; nn <= 2048 && !bad; nn++)
            if (st_held_measurable(nn, sl) != (st_held_wrapped(st_held_split(nn, sl), nn, sl) == 0)) {
                bad = 1;
                printf("[held-indep] I6 mismatch n=%d seqlen=%d\n", nn, sl);
            }
    printf("[held-indep] %s I6  st_held_measurable = no held window wraps\n", bad ? "FAIL" : "PASS");
    if (bad) fails++;

    /* I7: the CT-2 live cert tiles the live text up to the cert budget; a
     * text at least that long tiled to 0 bytes and the held loss read
     * corpus[x % 0]. It must say UNMEASURABLE (rc 1) instead. */
    static uint8_t live[1300];
    for (int i = 0; i < (int)sizeof live; i++) live[i] = (uint8_t)("the quick brown fox. "[i % 21]);
    cap_n = 0; cap[0] = 0;
    float ld = -1.0f, bd = -1.0f;
    int lrc = cradle_live_teach_test(capture, live, (int)sizeof live, &ld, &bd);
    int i7 = lrc == 1 && strstr(cap, "UNMEASURABLE") != NULL;
    printf("[held-indep] %s I7  live cert with a %d B text says UNMEASURABLE (rc %d)\n",
           i7 ? "PASS" : "FAIL", (int)sizeof live, lrc);
    if (!i7) fails++;

    /* I8: before a lesson arrives a node trains the TEACHER_FIXTURE (its train
     * windows at the DMN seqlen); the canonical lesson's held windows must not
     * hold a k-gram from those bytes either */
    {
        const uint8_t *fx = (const uint8_t *)TEACHER_FIXTURE;
        int fxn = (int)sizeof(TEACHER_FIXTURE) - 1;
        int fx_end = st_held_split(fxn, CRADLE_SEQLEN).train_end;
        n = cradle_compose_canon(buf, (int)sizeof buf, NULL);
        split(n, CRADLE_SEQLEN, &tw, &hw);
        rc = st_held_check_seen(buf, n, CRADLE_SEQLEN, tw, hw, fx, fx_end, ST_HELD_K, &r);
        printf("[held-indep] fixture %d B, trained bytes 0..%d\n", fxn, fx_end);
        item(rc == 0, "I8", "canonical lesson held shares no k-gram with the trained fixture", &r);

        /* I9 (instrument for I8): seen = the lesson's own held bytes -> all
         * shared; seen = LCG bytes -> none */
        st_split sp = st_held_split(n, CRADLE_SEQLEN);
        rc = st_held_check_seen(buf, n, CRADLE_SEQLEN, tw, hw, buf + sp.train_end,
                                n - sp.train_end, ST_HELD_K, &r);
        int ok9 = rc == 1 && r.held_kgrams > 0 && r.shared == r.held_kgrams;
        static uint8_t rnd[4096];
        uint32_t s9 = 0xC0FFEEu;
        for (int i = 0; i < (int)sizeof rnd; i++) { s9 = s9 * 1664525u + 1013904223u; rnd[i] = (uint8_t)(s9 >> 16); }
        st_held_report r2;
        int rc2 = st_held_check_seen(buf, n, CRADLE_SEQLEN, tw, hw, rnd, (int)sizeof rnd, ST_HELD_K, &r2);
        ok9 = ok9 && rc2 == 0 && r2.shared == 0;
        item(ok9, "I9", "instrument: seen = the held bytes -> all shared; seen = random -> none", &r);
    }

    /* I10-I12: the training record (st_seen). A node trained the fixture, then
     * lesson A, then lesson B replaced A. B's held tail repeats bytes of A's
     * train region: st_held_check (B against B) cannot see it, the record must. */
    {
        static st_seen rec;
        static uint8_t A[CRADLE_RING_BYTES], Bl[1280], rnd[1280];
        const uint8_t *fx = (const uint8_t *)TEACHER_FIXTURE;
        int fxn = (int)sizeof(TEACHER_FIXTURE) - 1;
        int na = cradle_compose_canon(A, (int)sizeof A, NULL);
        st_split sa = st_held_split(na, CRADLE_SEQLEN);
        st_split sb = st_held_split(1280, CRADLE_SEQLEN);
        uint32_t s10 = 0xB0B0u;
        for (int i = 0; i < sb.train_end; i++) {          /* B's train: lowercase LCG text */
            s10 = s10 * 1664525u + 1013904223u;
            Bl[i] = (uint8_t)("abcdefghijklmnopqrstuvwxyz     "[(s10 >> 16) % 31]);
        }
        memcpy(Bl + sb.train_end, A, (size_t)(1280 - sb.train_end));   /* held = A's train bytes */
        for (int i = 0; i < 1280; i++) { s10 = s10 * 1664525u + 1013904223u; rnd[i] = (uint8_t)(s10 >> 16); }

        st_seen_clear(&rec);
        st_seen_add(&rec, fx, st_held_split(fxn, CRADLE_SEQLEN).train_end, ST_HELD_K);
        st_seen_add(&rec, A, sa.train_end, ST_HELD_K);
        st_seen_add(&rec, Bl, sb.train_end, ST_HELD_K);

        st_held_report own, ex, bl;
        (void)st_held_check(Bl, 1280, CRADLE_SEQLEN, sb.trainw, sb.heldw, ST_HELD_K, &own);
        (void)st_held_check_seen(Bl, 1280, CRADLE_SEQLEN, sb.trainw, sb.heldw, A, sa.train_end, ST_HELD_K, &ex);
        rc = st_held_check_record(Bl, 1280, CRADLE_SEQLEN, sb.trainw, sb.heldw, &rec, ST_HELD_K, &bl);
        printf("[held-indep] I10 lesson B: against its own train %d/%d, against A's train (exact) %d/%d, record %d/%d\n",
               own.shared, own.held_kgrams, ex.shared, ex.held_kgrams, bl.shared, bl.held_kgrams);
        item(rc == 1 && own.shared == 0 && ex.shared == ex.held_kgrams && bl.shared == ex.shared,
             "I10", "lesson swap: the record sees B's held in A's train (B's own check cannot)", &bl);

        /* I11: the canonical lesson against the record of fixture + itself:
         * exactly 0 by the exact checks (I1, I8), the record must agree */
        rc = st_held_check_record(A, na, CRADLE_SEQLEN, sa.trainw, sa.heldw, &rec, ST_HELD_K, &bl);
        item(rc == 0 && bl.held_kgrams > 0 && bl.shared == 0, "I11", "record: the canonical lesson held is still clean", &bl);

        /* I12: random held bytes against the full record: false "seen" stays
         * rare (Bloom over-count), and an empty record sees nothing */
        rc = st_held_check_record(rnd, 1280, CRADLE_SEQLEN, sb.trainw, sb.heldw, &rec, ST_HELD_K, &bl);
        int fp = bl.shared;
        unsigned added = (unsigned)rec.added;
        st_seen_clear(&rec);
        st_held_report e0;
        int rc0 = st_held_check_record(Bl, 1280, CRADLE_SEQLEN, sb.trainw, sb.heldw, &rec, ST_HELD_K, &e0);
        printf("[held-indep] I12 record added %u k-grams before clear; random held: %d/%d false seen; empty record on B: %d\n",
               added, fp, bl.held_kgrams, e0.shared);
        item(bl.held_kgrams > 0 && fp <= 2 && rc0 == 0 && e0.shared == 0, "I12",
             "record: random bytes are rarely 'seen' (<= 2 of 170), an empty record sees nothing", &bl);
    }

    printf("[held-indep] %s\n", fails ? "RED" : "ALL PASS");
    return fails ? 1 : 0;
}
