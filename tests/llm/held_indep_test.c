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
 *
 * Exit 0 = all PASS. Prints "[held-indep] PASS|FAIL <id> ..." per item.
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "../../arch/common/llm/student.h"
#include "../../arch/common/llm/held_indep.h"

static int fails;

static void item(int ok, const char *id, const char *what, const st_held_report *r)
{
    printf("[held-indep] %s %s  %s  (held k-grams %d, shared %d, wrapped %d)\n",
           ok ? "PASS" : "FAIL", id, what, r->held_kgrams, r->shared, r->wrapped);
    if (!ok) fails++;
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

    printf("[held-indep] %s\n", fails ? "RED" : "ALL PASS");
    return fails ? 1 : 0;
}
