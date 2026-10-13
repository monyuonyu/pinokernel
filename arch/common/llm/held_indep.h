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

/* 1 when a held number on this corpus is a measurement: there is a corpus
 * and no held window wraps into the train bytes (n >= 3 windows). When 0,
 * callers say "unmeasurable" instead of printing a loss (0-12 step 2). */
static inline int st_held_measurable(int n, int seqlen)
{
    if (n < 1 || seqlen < 1) return 0;
    return st_held_wrapped(st_held_split(n, seqlen), n, seqlen) == 0;
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

/* The same held windows against bytes the model trained on BEFORE this corpus
 * (0-12 step 4): a node trains the fixture until a lesson arrives, and a new
 * lesson replaces the old one in the ring. `seen` is a plain byte span (no
 * wrap); only k-grams wholly inside it count. r->shared counts held k-grams
 * found in `seen`; r->wrapped as in st_held_check. Returns 0 when nothing is
 * shared and nothing wraps, 1 when something is, -1 on bad arguments. */
int st_held_check_seen(const uint8_t *corpus, int n, int seqlen, int trainw,
                       int heldw, const uint8_t *seen, int seen_n, int k,
                       st_held_report *r);

/* What the brain has trained on, as a Bloom filter of k-gram hashes (0-12
 * step 4). A lesson replaces the ring, so after A then B the bytes of A are
 * gone but the weights learned them; checking B's held only against B's train
 * windows misses that. The student adds the train bytes of every corpus it
 * trains and checks held windows against the whole record. A Bloom filter
 * can say "seen" for a k-gram it never saw (over-counts, never under-counts);
 * at 5000 k-grams (fixture + one lesson) in 131072 bits with 3 probes that is
 * about 0.1% (65536 bits gave 2 of 170 random held k-grams, too close to the
 * cert's line). The record lives in memory: it starts empty at boot, so
 * weights restored from disk carry training it does not know about (the
 * student says "since boot"). */
#define ST_SEEN_BITS  131072
typedef struct {
    uint8_t  bits[ST_SEEN_BITS / 8];
    uint32_t added;     /* k-grams added (with repeats), for the fill estimate */
} st_seen;

void st_seen_clear(st_seen *s);
/* add every k-gram wholly inside span[0..n) */
void st_seen_add(st_seen *s, const uint8_t *span, int n, int k);
/* like st_held_check_seen, with the record in place of a byte span */
int  st_held_check_record(const uint8_t *corpus, int n, int seqlen, int trainw,
                          int heldw, const st_seen *s, int k, st_held_report *r);

#endif /* PK_HELD_INDEP_H */
