/*
 * held_indep.h — ROADMAP 0-12: is the held-out set independent of what the
 * model trained on? (docs/architecture/30-module/held-independence.md)
 *
 * Every held number in p-kernel is defined by POSITION: the student trains on
 * windows 0..trainw-1 of one corpus and scores windows trainw..trainw+heldw-1
 * of the same corpus (student_shell.c heldout_loss, cradle.c ct_heldout_loss),
 * reading byte (off+i) % n. Nothing compared the bytes. This counts, for the
 * same window geometry, the held k-grams that also occur inside a trained
 * window, and the held windows that run past the end of the corpus (they wrap
 * into the train region).
 */
#ifndef PK_HELD_INDEP_H
#define PK_HELD_INDEP_H

#include <stdint.h>

#define ST_HELD_K  16   /* bytes per k-gram: half a production window (32) */

/* THE train/held split. Of total = n/seqlen windows, the first 3/4 (at least
 * 2) are trained and the rest (at least 1) are held; train_end is the first
 * held byte. student_shell.c and cradle.c take every split from here (they
 * had 11 hand copies); tests/llm/check_held_split.sh is red on a new copy.
 * st_held_wrapped counts the held windows that read past byte n-1 (the
 * student's window() reads mod n, so they score train bytes) — not acted on
 * yet. */
typedef struct { int total, trainw, heldw, train_end; } st_split;

static inline st_split st_held_split(int n, int seqlen)
{
    st_split s;
    s.total  = n / seqlen;
    s.trainw = s.total * 3 / 4; if (s.trainw < 2) s.trainw = 2;
    s.heldw  = s.total - s.trainw; if (s.heldw < 1) s.heldw = 1;
    s.train_end = s.trainw * seqlen;
    return s;
}

static inline int st_held_wrapped(st_split s, int n, int seqlen)
{
    int wrapped = 0;
    for (int w = s.trainw; w < s.trainw + s.heldw; w++)
        if (w * seqlen + seqlen > n) wrapped++;
    return wrapped;
}

typedef struct {
    int held_kgrams;    /* k-grams inside the held windows                    */
    int shared;         /* of those, equal to a k-gram inside a train window  */
    int wrapped;        /* held windows that read past byte n-1 (wrap to 0)   */
} st_held_report;

/* Windows are [w*seqlen, w*seqlen+seqlen), read mod n, exactly as the
 * student's window(). Train: w < trainw. Held: trainw <= w < trainw+heldw.
 * Returns 0 when the held set is independent (shared == 0 and wrapped == 0),
 * 1 when it is not, -1 on bad arguments (k > seqlen, n < 1, ...). */
int st_held_check(const uint8_t *corpus, int n, int seqlen, int trainw,
                  int heldw, int k, st_held_report *r);

#endif /* PK_HELD_INDEP_H */
