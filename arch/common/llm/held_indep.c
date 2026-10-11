/*
 * held_indep.c — ROADMAP 0-12, see held_indep.h.
 *
 * Plain and slow on purpose: every held k-gram is compared with every trained
 * k-gram (a 64-bit FNV-1a hash first, then the bytes). For the cert lesson
 * (1280 B, seqlen 32, k 16) that is ~170 x ~510 hash compares; the check runs
 * once per corpus, not per training step. No allocation: train hashes are
 * recomputed for each held k-gram, so the cost is O(H*T*k) time, O(k) space.
 */
#include "held_indep.h"

static uint8_t hi_byte(const uint8_t *c, int n, long off)
{
    return c[off % n];
}

static uint64_t hi_hash(const uint8_t *c, int n, long off, int k)
{
    uint64_t h = 1469598103934665603ULL;
    for (int i = 0; i < k; i++) {
        h ^= hi_byte(c, n, off + i);
        h *= 1099511628211ULL;
    }
    return h;
}

static int hi_same(const uint8_t *c, int n, long a, long b, int k)
{
    for (int i = 0; i < k; i++)
        if (hi_byte(c, n, a + i) != hi_byte(c, n, b + i)) return 0;
    return 1;
}

int st_held_check(const uint8_t *corpus, int n, int seqlen, int trainw,
                  int heldw, int k, st_held_report *r)
{
    st_held_report z = { 0, 0, 0 };
    if (r) *r = z;
    if (!corpus || n < 1 || seqlen < 1 || trainw < 0 || heldw < 0 ||
        k < 1 || k > seqlen)
        return -1;

    for (int hw = trainw; hw < trainw + heldw; hw++) {
        long hbase = (long)hw * seqlen;
        if (hbase + seqlen > n) z.wrapped++;
        for (int hi = 0; hi + k <= seqlen; hi++) {
            long ho = hbase + hi;
            uint64_t hh = hi_hash(corpus, n, ho, k);
            int found = 0;
            for (int tw = 0; tw < trainw && !found; tw++) {
                long tbase = (long)tw * seqlen;
                for (int ti = 0; ti + k <= seqlen; ti++) {
                    long to = tbase + ti;
                    if (hi_hash(corpus, n, to, k) == hh &&
                        hi_same(corpus, n, ho, to, k)) { found = 1; break; }
                }
            }
            z.held_kgrams++;
            if (found) z.shared++;
        }
    }
    if (r) *r = z;
    return (z.shared == 0 && z.wrapped == 0) ? 0 : 1;
}
