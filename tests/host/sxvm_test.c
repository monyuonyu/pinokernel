/*
 * sxvm_test.c — [sxvm] (research, not wired into CI): stage 1 of
 * docs/architecture/30-module/research/safe-exec.md, the expectations fixed
 * there before this file was written.
 *
 *   V1  random programs (biased so that some pass): every program the
 *       checker accepts runs in the interpreter without a trap (no access
 *       outside the area, no more steps than the checker's bound, no unwritten
 *       register, pc never leaves the program). At least 10% must pass, and
 *       some accepted programs must use loops and memory (not a vacuous run).
 *   K   hand-written programs: the ones that must pass pass with the right
 *       r0, the ones that must be refused are refused with the right code.
 *
 *   stage 2: SX_CALL (pub/log) in V1 and K; the selfc demo units as
 *       per-event handlers (K demo-checksum, K gate-v1);
 *   X   (sxvm_test exh <len>) every suffix of length 1..len over a small
 *       table after a fixed prelude, each run on 3 memory fillings.
 *
 *   sxvm_test <count> <seed>   prints SUMMARY, exit 0 = no trap and K holds.
 *   sxvm_test exh <len>        prints EXH, exit 0 = no trap.
 *   Built with -DSXVM_BREAK_* the same run must report traps (run_sxvm.sh).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sxvm.h"

static uint64_t rs;
static uint32_t rnd(void)
{
    rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17;
    return (uint32_t)(rs >> 11);
}
static int rr(int lo, int hi) { return lo + (int)(rnd() % (uint32_t)(hi - lo + 1)); }

static sxvm_insn I(int op, int d, int s, int imm)
{
    sxvm_insn x = { (uint8_t)op, (uint8_t)d, (uint8_t)s, 0, imm };
    return x;
}

/* the host side of the helpers: records what was published, and checks the
 * bound on its own too (the VM's check is what is under test) */
typedef struct { uint8_t last[SXVM_MEM]; uint32_t len, npub, nlog, logv, over; } host_t;
static uint32_t h_pub(void *c, const uint8_t *b, uint32_t len)
{
    host_t *h = c;
    if (len > SXVM_MEM) { h->over++; return 0; }
    memcpy(h->last, b, len); h->len = len; h->npub++;
    return len;
}
static void h_log(void *c, uint32_t v) { host_t *h = c; h->nlog++; h->logv = v; }
static host_t host;
static const sxvm_env env = { &host, h_pub, h_log };

/* a random program: most registers written first, then mixed ops */
static int gen_mode = 1;    /* 2 (stage 4b): registers read from the area first */
static int gen(sxvm_insn *p)
{
    int n = 0, len = rr(6, 40);
    if (gen_mode == 2) {
        p[n++] = I(SX_MOVI, 0, 0, 0);
        for (int r = 1; r < SXVM_NREG; r++)
            p[n++] = rr(0, 9) < 3 ? I(SX_MOVI, r, 0, rr(0, 70)) : I(SX_LDB, r, 0, rr(0, 63));
    } else
    for (int r = 0; r < SXVM_NREG; r++)
        if (rr(0, 9) < 9) {
            int big = rr(0, 15) == 0;
            p[n++] = I(SX_MOVI, r, 0, big ? (int)(0xFFFFFF00u + (uint32_t)rr(0, 255)) : rr(0, 70));
        }
    while (n < len - 1) {
        int c = rr(0, 99), d = rr(0, 7), s = rr(0, 7);
        if (c < 8)       p[n++] = I(SX_LOOP, rr(1, 6), 0, rr(1, 300));
        else if (c < 22) p[n++] = I(SX_LDB, d, s, rr(-8, 70));
        else if (c < 30) p[n++] = I(SX_STB, d, s, rr(-8, 70));
        else if (c < 42) p[n++] = I(SX_ANDI, d, 0, rr(0, 1) ? (1 << rr(0, 6)) - 1 : rr(0, 80));
        else if (c < 54) p[n++] = I(SX_ADDI, d, 0, rr(-12, 12));
        else if (c < 60) p[n++] = I(SX_JLTI, d, rr(0, 70), rr(-3, 5));
        else if (c < 64) p[n++] = I(SX_JEQI, d, rr(0, 70), rr(-3, 5));
        else if (c < 67) p[n++] = I(SX_JLT, d, s, rr(-3, 5));
        else if (c < 69) p[n++] = I(SX_JA, 0, 0, rr(-3, 5));
        else if (c < 73) p[n++] = I(SX_MOVI, d, 0, rr(0, 70));
        else if (c < 77) p[n++] = I(SX_MOV, d, s, 0);
        else if (c < 81) p[n++] = I(SX_SHRI, d, 0, rr(0, 8));
        else if (c < 84) p[n++] = I(SX_SHLI, d, 0, rr(0, 4));
        else if (c < 88) p[n++] = I(SX_CALL, 0, 0, rr(1, 2));
        else {
            static const int ops[] = { SX_ADD, SX_SUB, SX_MUL, SX_AND, SX_OR, SX_XOR, SX_DIV };
            p[n++] = I(ops[rr(0, 6)], d, s, 0);
        }
    }
    p[n++] = I(SX_EXIT, 0, 0, 0);
    return n;
}

static int k_fail;
static void k_expect(const char *name, const sxvm_insn *p, int n, long want_check, int want_trap, uint32_t want_r0)
{
    long b = sxvm_check(p, n);
    uint8_t mem[SXVM_MEM];
    for (int i = 0; i < SXVM_MEM; i++) mem[i] = (uint8_t)(i * 3);
    uint32_t r0 = 0;
    long steps = 0;
    int ok;
    if (want_check < 0) ok = b == want_check;
    else {
        int t = sxvm_run(p, n, mem, b, &env, &r0, &steps);
        ok = b >= 0 && t == want_trap && (t != SXVM_T_OK || r0 == want_r0);
    }
    printf("[sxvm] K %-22s check=%ld r0=%u steps=%ld %s\n", name, b, r0, steps, ok ? "ok" : "FAIL");
    if (!ok) k_fail++;
}

static void known(void)
{
    /* sum of mem[0..63] with a guarded index: passes, r0 = sum of i*3 */
    sxvm_insn a[] = { I(SX_MOVI, 0, 0, 0), I(SX_MOVI, 1, 0, 0),
                      I(SX_LOOP, 4, 0, 64),
                      I(SX_ANDI, 1, 0, 63), I(SX_LDB, 2, 1, 0), I(SX_ADD, 0, 2, 0), I(SX_ADDI, 1, 0, 1),
                      I(SX_EXIT, 0, 0, 0) };
    uint32_t sum = 0;
    for (int i = 0; i < 64; i++) sum += (uint8_t)(i * 3);
    k_expect("sum-masked", a, 8, 0, SXVM_T_OK, sum);
    /* the same without the mask: the index grows, refused */
    sxvm_insn b[] = { I(SX_MOVI, 0, 0, 0), I(SX_MOVI, 1, 0, 0),
                      I(SX_LOOP, 3, 0, 64),
                      I(SX_LDB, 2, 1, 0), I(SX_ADD, 0, 2, 0), I(SX_ADDI, 1, 0, 1),
                      I(SX_EXIT, 0, 0, 0) };
#ifdef SXVM_IND
    /* stage 5b: 64 passes from 0 by 1 read 0..63 only; this case was the
     * head's over-refusal (the comment above), the induction rule accepts it */
    k_expect("sum-unmasked", b, 7, 0, SXVM_T_OK, sum);
#else
    k_expect("sum-unmasked", b, 7, SXVM_E_MEM, 0, 0);
#endif
    /* stage 5b: the index also doubled in the body, not an induction variable
     * (written for the INDMULTI breaker after V1 did not see it, as fixed) */
    sxvm_insn iw[] = { I(SX_MOVI, 0, 0, 0), I(SX_MOVI, 1, 0, 0),
                       I(SX_LOOP, 3, 0, 10),
                       I(SX_LDB, 2, 1, 0), I(SX_ADDI, 1, 0, 1), I(SX_ADD, 1, 1, 0),
                       I(SX_EXIT, 0, 0, 0) };
    k_expect("ind-two-writes", iw, 7, SXVM_E_MEM, 0, 0);
    /* stage 5b: 65 passes from 0 by 1, the last reads mem[64] (for INDOFF) */
    sxvm_insn ie[] = { I(SX_MOVI, 0, 0, 0), I(SX_MOVI, 1, 0, 0),
                       I(SX_LOOP, 3, 0, 65),
                       I(SX_LDB, 2, 1, 0), I(SX_ADD, 0, 2, 0), I(SX_ADDI, 1, 0, 1),
                       I(SX_EXIT, 0, 0, 0) };
    k_expect("ind-edge", ie, 7, SXVM_E_MEM, 0, 0);
    /* stage 5e: from 63 down by 1, 64 passes read 63..0 (refused before 5e) */
    sxvm_insn nc[] = { I(SX_MOVI, 0, 0, 0), I(SX_MOVI, 1, 0, 63),
                       I(SX_LOOP, 3, 0, 64),
                       I(SX_LDB, 2, 1, 0), I(SX_ADD, 0, 2, 0), I(SX_ADDI, 1, 0, -1),
                       I(SX_EXIT, 0, 0, 0) };
#ifdef SXVM_INDNEG
    k_expect("neg-count", nc, 7, 0, SXVM_T_OK, sum);
#else
    k_expect("neg-count", nc, 7, SXVM_E_MEM, 0, 0);
#endif
    /* stage 5e: 65 passes, the last wraps to 2^32-1 (for INDNEGOFF, INDNEGNOB) */
    sxvm_insn ne[] = { I(SX_MOVI, 0, 0, 0), I(SX_MOVI, 1, 0, 63),
                       I(SX_LOOP, 3, 0, 65),
                       I(SX_LDB, 2, 1, 0), I(SX_ADD, 0, 2, 0), I(SX_ADDI, 1, 0, -1),
                       I(SX_EXIT, 0, 0, 0) };
    k_expect("neg-edge", ne, 7, SXVM_E_MEM, 0, 0);
    /* a compare guard instead of a mask: passes */
    sxvm_insn c[] = { I(SX_MOVI, 0, 0, 0), I(SX_LDB, 1, 0, 5),
                      I(SX_JLTI, 1, 64, 1), I(SX_EXIT, 0, 0, 0),
                      I(SX_LDB, 0, 1, 0), I(SX_EXIT, 0, 0, 0) };
    k_expect("guard-jlti", c, 6, 0, SXVM_T_OK, (uint8_t)(15 * 3));
    sxvm_insn d[] = { I(SX_MOVI, 0, 0, 1), I(SX_JA, 0, 0, -2), I(SX_EXIT, 0, 0, 0) };
    k_expect("backward-jump", d, 3, SXVM_E_JUMP, 0, 0);
    sxvm_insn e[] = { I(SX_ADDI, 3, 0, 1), I(SX_MOV, 0, 3, 0), I(SX_EXIT, 0, 0, 0) };
    k_expect("unwritten", e, 3, SXVM_E_UNINIT, 0, 0);
    sxvm_insn f[] = { I(SX_MOVI, 0, 0, 0), I(SX_LOOP, 2, 0, 3), I(SX_LOOP, 1, 0, 3), I(SX_ADDI, 0, 0, 1), I(SX_EXIT, 0, 0, 0) };
    k_expect("nested-loop", f, 5, SXVM_E_LOOP, 0, 0);
    sxvm_insn g[] = { I(SX_MOVI, 0, 0, 63), I(SX_STB, 0, 0, 1), I(SX_EXIT, 0, 0, 0) };
    k_expect("edge+1", g, 3, SXVM_E_MEM, 0, 0);
    sxvm_insn h[] = { I(SX_MOVI, 0, 0, 7) };
    k_expect("falls-off", h, 1, SXVM_E_FALL, 0, 0);

    /* stage 2 */
    sxvm_insn cl[] = { I(SX_MOVI, 1, 0, 5), I(SX_MOVI, 0, 0, 0),
                       I(SX_LOOP, 2, 0, 3), I(SX_CALL, 0, 0, SXVM_CALL_LOG), I(SX_MOVI, 0, 0, 0),
                       I(SX_EXIT, 0, 0, 0) };
    k_expect("call-clobbers-in-loop", cl, 6, SXVM_E_UNINIT, 0, 0);
    sxvm_insn pl[] = { I(SX_MOVI, 1, 0, 60), I(SX_MOVI, 2, 0, 5), I(SX_CALL, 0, 0, SXVM_CALL_PUB), I(SX_EXIT, 0, 0, 0) };
    k_expect("pub-past-end", pl, 4, SXVM_E_MEM, 0, 0);
    sxvm_insn bc[] = { I(SX_MOVI, 1, 0, 0), I(SX_MOVI, 2, 0, 1), I(SX_CALL, 0, 0, 3), I(SX_EXIT, 0, 0, 0) };
    k_expect("unknown-helper", bc, 4, SXVM_E_OP, 0, 0);
}

/* 2c: the selfc demo units, rewritten as per-event handlers */
static void demo_units(void)
{
    static const char msg[] = "I was compiled at runtime inside the kernel";
    uint32_t want = 0;
    for (const char *q = msg; *q; q++) want = want * 31u + (uint32_t)(unsigned char)*q;
    sxvm_insn ck[] = {
        I(SX_MOVI, 0, 0, 0), I(SX_MOVI, 1, 0, 0), I(SX_MOVI, 3, 0, 0), I(SX_MOVI, 4, 0, 31),
        I(SX_LOOP, 9, 0, 64),
        I(SX_JEQI, 3, 1, 8),            /* done: end this pass        */
        I(SX_ANDI, 1, 0, 63),
        I(SX_LDB, 2, 1, 0),
        I(SX_JEQI, 2, 0, 3),            /* the NUL: mark done         */
        I(SX_MUL, 0, 4, 0),
        I(SX_ADD, 0, 2, 0),
        I(SX_JA, 0, 0, 1),
        I(SX_MOVI, 3, 0, 1),
        I(SX_ADDI, 1, 0, 1),
        I(SX_EXIT, 0, 0, 0) };
    int n = (int)(sizeof ck / sizeof ck[0]);
    uint8_t mem[SXVM_MEM];
    memset(mem, 0xA5, sizeof mem);      /* garbage after the NUL must not count */
    memcpy(mem, msg, sizeof msg);
    long b = sxvm_check(ck, n), steps = 0;
    uint32_t r0 = 0;
    int t = b >= 0 ? sxvm_run(ck, n, mem, b, &env, &r0, &steps) : -1;
    int ok = b >= 0 && t == SXVM_T_OK && r0 == want;
    printf("[sxvm] K %-22s check=%ld trap=%d r0=%u want=%u steps=%ld %s\n", "demo-checksum", b, t, r0, want, steps, ok ? "ok" : "FAIL");
    if (!ok) k_fail++;

    sxvm_insn gv[] = {
        I(SX_MOVI, 3, 0, 0),
        I(SX_MOVI, 4, 0, 'A'), I(SX_STB, 3, 4, 0),
        I(SX_MOVI, 4, 0, ':'), I(SX_STB, 3, 4, 1),
        I(SX_MOVI, 4, 0, '1'), I(SX_STB, 3, 4, 2),
        I(SX_MOVI, 1, 0, 0), I(SX_MOVI, 2, 0, 3), I(SX_CALL, 0, 0, SXVM_CALL_PUB),
        I(SX_EXIT, 0, 0, 0) };
    n = (int)(sizeof gv / sizeof gv[0]);
    memset(mem, 0, sizeof mem);
    memset(&host, 0, sizeof host);
    b = sxvm_check(gv, n);
    t = b >= 0 ? sxvm_run(gv, n, mem, b, &env, &r0, &steps) : -1;
    ok = b >= 0 && t == SXVM_T_OK && host.npub == 1 && host.len == 3 && !memcmp(host.last, "A:1", 3) && r0 == 3;
    printf("[sxvm] K %-22s check=%ld trap=%d pub=%u len=%u r0=%u %s\n", "gate-v1", b, t, host.npub, host.len, r0, ok ? "ok" : "FAIL");
    if (!ok) k_fail++;
}

/* X: every suffix over a small table after a fixed prelude */
static sxvm_insn tab[64];
static int ntab;
static void tab_build(void)
{
    static const int mv[] = { 0, 63, 64 }, ai[] = { 1, -4 }, an[] = { 7, 63 };
    for (int d = 0; d < 3; d++) {
        for (int k = 0; k < 3; k++) tab[ntab++] = I(SX_MOVI, d, 0, mv[k]);
        for (int k = 0; k < 2; k++) tab[ntab++] = I(SX_ADDI, d, 0, ai[k]);
        for (int k = 0; k < 2; k++) tab[ntab++] = I(SX_ANDI, d, 0, an[k]);
        tab[ntab++] = I(SX_STB, d, 0, 0);
        tab[ntab++] = I(SX_LDB, 0, d, 0);
        tab[ntab++] = I(SX_LDB, 0, d, 60);
    }
    tab[ntab++] = I(SX_LDB, 0, 0, 64);
    tab[ntab++] = I(SX_ADD, 0, 1, 0);
    tab[ntab++] = I(SX_ADD, 1, 2, 0);
    tab[ntab++] = I(SX_SUB, 1, 0, 0);
    tab[ntab++] = I(SX_MUL, 1, 1, 0);
    tab[ntab++] = I(SX_SHLI, 1, 0, 4);
    tab[ntab++] = I(SX_JLTI, 1, 64, 1);
    tab[ntab++] = I(SX_JA, 0, 0, 1);
    tab[ntab++] = I(SX_JA, 0, 0, -1);
    tab[ntab++] = I(SX_LOOP, 1, 0, 100);
    tab[ntab++] = I(SX_LOOP, 2, 0, 100);
    tab[ntab++] = I(SX_CALL, 0, 0, SXVM_CALL_PUB);
    tab[ntab++] = I(SX_CALL, 0, 0, SXVM_CALL_LOG);
}

static int exhaustive(int maxlen)
{
    sxvm_insn p[16];
    long total = 0, acc = 0, ntrap = 0, trap[8] = { 0 };
    int shown = 0;
    tab_build();
    for (int len = 1; len <= maxlen; len++) {
        long combos = 1;
        for (int k = 0; k < len; k++) combos *= ntab;
        for (long c = 0; c < combos; c++) {
            int n = 0;
            p[n++] = I(SX_MOVI, 0, 0, 0);
            p[n++] = I(SX_LDB, 1, 0, 0);
            p[n++] = I(SX_MOVI, 2, 0, (int)0xFFFFFFF0u);
            long v = c;
            for (int k = 0; k < len; k++) { p[n++] = tab[v % ntab]; v /= ntab; }
            p[n++] = I(SX_EXIT, 0, 0, 0);
            total++;
            long b = sxvm_check(p, n);
            if (b < 0) continue;
            acc++;
            for (int f = 0; f < 3; f++) {
                uint8_t mem[SXVM_MEM];
                for (int i = 0; i < SXVM_MEM; i++) mem[i] = f == 0 ? 0 : f == 1 ? 0xFF : (uint8_t)(i * 37 + 11);
                uint32_t r0; long steps;
                int t = sxvm_run(p, n, mem, b, &env, &r0, &steps);
                if (t != SXVM_T_OK) {
                    trap[t & 7]++; ntrap++;
                    if (shown++ < 3) {
                        printf("[sxvm] EXH-TRAP fill=%d trap=%d bound=%ld steps=%ld:", f, t, b, steps);
                        for (int i = 3; i < n - 1; i++) printf(" %d/%d/%d/%d", p[i].op, p[i].d, p[i].s, p[i].imm);
                        printf("\n");
                    }
                    break;
                }
            }
        }
    }
    printf("[sxvm] EXH table=%d maxlen=%d programs=%ld accepted=%ld traps=%ld mem=%ld steps=%ld pc=%ld op=%ld uninit=%ld host_over=%u\n",
           ntab, maxlen, total, acc, ntrap, trap[SXVM_T_MEM], trap[SXVM_T_STEPS], trap[SXVM_T_PC], trap[SXVM_T_OP], trap[SXVM_T_UNINIT], host.over);
    return ntrap == 0 ? 0 : 1;
}

/* stage 4 (sxvm_z3.py): the same programs as main(), the fillings from a
 * separate stream, so every build (head or breaker) dumps the same list.
 *   dump <count> <seed> <every> [gen]: gen 2 = stage 4b's generator.
 *   "A it bound trap n op/d/s/imm..." for each
 *   accepted program (trap: one random filling), "R it code n ..." for every
 *   <every>-th program refused for a value reason (unwritten, memory, falls off).
 *   replay: stdin lines "n insn*4.. 64 bytes k pubvals.." -> "trap steps check"
 *   (max steps = the check's bound, or 1000000 when refused). */
static uint32_t rp_v[64]; static int rp_n, rp_i; static unsigned rp_over;
static uint32_t rp_pub(void *c, const uint8_t *b, uint32_t len)
{
    (void)c; (void)b;
    if (len > SXVM_MEM) rp_over++;
    return rp_i < rp_n ? rp_v[rp_i++] : 0;
}
static void rp_log(void *c, uint32_t v) { (void)c; (void)v; }

static int dump(long count, uint32_t seed, long every)
{
    static sxvm_insn p[SXVM_MAXINSN];
    uint64_t fs = 0x9E3779B97F4A7C15ULL ^ seed;
    rs = 0x5A5A1234ULL ^ ((uint64_t)seed << 32) ^ seed;
    for (long it = 0; it < count; it++) {
        int n = gen(p);
        uint8_t mem[SXVM_MEM];
        for (int i = 0; i < SXVM_MEM; i++) { fs ^= fs << 13; fs ^= fs >> 7; fs ^= fs << 17; mem[i] = (uint8_t)(fs >> 23); }
        long b = sxvm_check(p, n);
        if (b >= 0) {
            uint32_t r0; long steps;
            memset(&host, 0, sizeof host);
            int t = sxvm_run(p, n, mem, b, &env, &r0, &steps);
            printf("A %ld %ld %d %d", it, b, t, n);
        } else if ((b == SXVM_E_UNINIT || b == SXVM_E_MEM || b == SXVM_E_FALL) && it % every == 0) {
            printf("R %ld %ld %d", it, b, n);
        } else continue;
        for (int i = 0; i < n; i++) printf(" %d/%d/%d/%d", p[i].op, p[i].d, p[i].s, p[i].imm);
        printf("\n");
    }
    return 0;
}

static int replay(void)
{
    static sxvm_insn p[SXVM_MAXINSN];
    const sxvm_env renv = { 0, rp_pub, rp_log };
    int n;
    while (scanf("%d", &n) == 1) {
        if (n < 1 || n > SXVM_MAXINSN) return 2;
        for (int i = 0; i < n; i++) {
            int op, d, s, imm;
            if (scanf("%d %d %d %d", &op, &d, &s, &imm) != 4) return 2;
            p[i] = I(op, d, s, imm);
        }
        uint8_t mem[SXVM_MEM];
        for (int i = 0; i < SXVM_MEM; i++) { int v; if (scanf("%d", &v) != 1) return 2; mem[i] = (uint8_t)v; }
        if (scanf("%d", &rp_n) != 1 || rp_n < 0 || rp_n > 64) return 2;
        for (int i = 0; i < rp_n; i++) if (scanf("%u", &rp_v[i]) != 1) return 2;
        rp_i = 0;
        long b = sxvm_check(p, n), steps = 0;
        uint32_t r0 = 0;
        int t = sxvm_run(p, n, mem, b >= 0 ? b : 1000000, &renv, &r0, &steps);
        printf("%d %ld %ld\n", t, steps, b);
    }
    printf("over %u\n", rp_over);
    return 0;
}

/* stage 5: useful per-event handlers, each in a plain and a masked form.
 * mem[63] = L, mem[0..L) the data. LOOP 63 over i = 0, 1, ...; i >= L does
 * nothing. The forms differ in one instruction: ANDI x,63 (masked) or
 * ADDI x,0 (plain, same jump offsets). */
#define UM(m, r) ((m) ? I(SX_ANDI, (r), 0, 63) : I(SX_ADDI, (r), 0, 0))
static int useful_prog(int h, int m, sxvm_insn *q)
{
    int n = 0, bl = h == 3 ? 7 : h == 4 ? 11 : 9;
    q[n++] = I(SX_MOVI, 0, 0, 0); q[n++] = I(SX_MOVI, 1, 0, 0);
    q[n++] = I(SX_MOVI, 5, 0, 0); q[n++] = I(SX_LDB, 3, 5, 63);
    q[n++] = I(SX_LOOP, bl, 0, 63);
    q[n++] = I(SX_JLT, 1, 3, 1); q[n++] = I(SX_JA, 0, 0, bl - 2);
    q[n++] = I(SX_MOV, 5, 1, 0); q[n++] = UM(m, 5); q[n++] = I(SX_LDB, 2, 5, 0);
    switch (h) {
    case 1: q[n++] = I(SX_JEQI, 2, ' ', 1); q[n++] = I(SX_JA, 0, 0, 1); q[n++] = I(SX_ADDI, 0, 0, 1); break;
    case 2: q[n++] = I(SX_JLT, 0, 2, 1); q[n++] = I(SX_JA, 0, 0, 1); q[n++] = I(SX_MOV, 0, 2, 0); break;
    case 3: q[n++] = I(SX_ADD, 0, 2, 0); break;
    case 4: q[n++] = I(SX_JLTI, 2, 'a', 3); q[n++] = I(SX_JLTI, 2, 'z' + 1, 1); q[n++] = I(SX_JA, 0, 0, 1);
            q[n++] = I(SX_ADDI, 2, 0, -32); q[n++] = I(SX_STB, 5, 2, 0); break;
    case 5: q[n++] = I(SX_JEQI, 2, ':', 1); q[n++] = I(SX_JA, 0, 0, 1); q[n++] = I(SX_MOV, 3, 1, 0); break;
    }
    q[n++] = I(SX_ADDI, 1, 0, 1);
    if (h == 4) { q[n++] = I(SX_MOVI, 1, 0, 0); q[n++] = I(SX_MOV, 2, 3, 0); q[n++] = UM(m, 2);
                  q[n++] = I(SX_CALL, 0, 0, SXVM_CALL_PUB); }
    if (h == 5) q[n++] = I(SX_MOV, 0, 3, 0);
    q[n++] = I(SX_EXIT, 0, 0, 0);
    return n;
}
static void useful_input(uint8_t *mem, int lmax)
{
    static const uint8_t mix[] = { ' ', ':', 'a', 'q', 'z', 'A', '`', '{', 0, 255 };
    for (int k = 0; k < SXVM_MEM; k++) mem[k] = rnd() % 3 ? mix[rnd() % sizeof mix] : (uint8_t)rnd();
    mem[63] = (uint8_t)rr(0, lmax);
}
/* the C reference: r0 (H4: also the expected published bytes in out) */
static uint32_t useful_ref(int h, const uint8_t *mem, uint8_t *out)
{
    int L = mem[63];
    uint32_t v = 0;
    for (int i = 0; i < L && i < 63; i++) {     /* LOOP 63: i stops at 62 */
        uint8_t c = mem[i];
        if (h == 1 && c == ' ') v++;
        if (h == 2 && c > v) v = c;
        if (h == 3) v += c;
        if (h == 4) out[i] = c >= 'a' && c <= 'z' ? (uint8_t)(c - 32) : c;
        if (h == 5 && c == ':') return (uint32_t)i;
    }
    return h == 4 || h == 5 ? (uint32_t)L : v;
}
static int useful(int count)
{
    static sxvm_insn q[64];
    uint8_t mem[SXVM_MEM], m0[SXVM_MEM], want[SXVM_MEM];
    rs = 0xC0FFEE15ULL;
    for (int h = 1; h <= 5; h++) {
        for (int m = 1; m >= 0; m--) {
            int n = useful_prog(h, m, q);
            long b = sxvm_check(q, n);
            long agree = 0, run = 0, trap = 0, steps;
            uint32_t r0;
            for (int it = 0; it < count; it++) {
                /* masked & accepted: F1 (L 0..63, with its bound); plain: F3
                 * (unchecked, L 0..255, a large bound, the run-time checks) */
                useful_input(mem, m ? 63 : (it & 1 ? 255 : 63));
                memcpy(m0, mem, sizeof mem);
                memset(&host, 0, sizeof host);
                if (m && b < 0) break;
                int t = sxvm_run(q, n, mem, m ? b : 100000, &env, &r0, &steps);
                run++;
                if (t != SXVM_T_OK || host.over) { trap++; continue; }
                uint32_t w = useful_ref(h, m0, want);
                int ok = r0 == w;
                if (h == 4) ok = ok && host.npub == 1 && host.len == w && !memcmp(host.last, want, w);
                agree += ok;
            }
            printf("[sxvm] USE H%d %-6s check=%ld run=%ld trap=%ld agree=%ld\n", h, m ? "masked" : "plain", b, run, trap, agree);
        }
    }
    return 0;
}

int main(int argc, char **argv)
{
    long count = argc > 1 ? atol(argv[1]) : 200000;
    uint32_t seed = argc > 2 ? (uint32_t)atoi(argv[2]) : 1;
    static sxvm_insn p[SXVM_MAXINSN];
    long acc = 0, acc_loop = 0, acc_mem = 0, acc_call = 0, trap[8] = { 0 }, ntrap = 0, reject[16] = { 0 };
    int shown = 0;

    setvbuf(stdout, NULL, _IOLBF, 0);
    if (argc > 5) gen_mode = atoi(argv[5]);
    if (argc > 4 && !strcmp(argv[1], "dump")) return dump(atol(argv[2]), (uint32_t)atoi(argv[3]), atol(argv[4]));
    if (argc > 1 && !strcmp(argv[1], "replay")) return replay();
    if (argc > 1 && !strcmp(argv[1], "useful")) return useful(argc > 2 ? atoi(argv[2]) : 1000);
    if (argc > 1 && !strcmp(argv[1], "exh")) return exhaustive(argc > 2 ? atoi(argv[2]) : 3);
    known();
    demo_units();
    rs = 0x5A5A1234ULL ^ ((uint64_t)seed << 32) ^ seed;
    for (long it = 0; it < count; it++) {
        int n = gen(p);
        long b = sxvm_check(p, n);
        if (b < 0) { reject[-b & 15]++; continue; }
        acc++;
        int hl = 0, hm = 0, hc = 0;
        for (int i = 0; i < n; i++) {
            if (p[i].op == SX_LOOP) hl = 1;
            if (p[i].op == SX_LDB || p[i].op == SX_STB) hm = 1;
            if (p[i].op == SX_CALL) hc = 1;
        }
        acc_loop += hl; acc_mem += hm; acc_call += hc;
        uint8_t mem[SXVM_MEM];
        for (int i = 0; i < SXVM_MEM; i++) mem[i] = (uint8_t)rnd();
        uint32_t r0;
        long steps;
        int t = sxvm_run(p, n, mem, b, &env, &r0, &steps);
        if (t != SXVM_T_OK) {
            trap[t & 7]++; ntrap++;
            if (shown++ < 3) {
                printf("[sxvm] TRAP seed=%u it=%ld trap=%d bound=%ld steps=%ld n=%d:", seed, it, t, b, steps, n);
                for (int i = 0; i < n; i++) printf(" %d/%d/%d/%d", p[i].op, p[i].d, p[i].s, p[i].imm);
                printf("\n");
            }
        }
    }
    printf("[sxvm] rejected seed=%u len=%ld op=%ld jump=%ld uninit=%ld mem=%ld loop=%ld fall=%ld imm=%ld\n", seed,
           reject[1], reject[2], reject[3], reject[4], reject[5], reject[6], reject[7], reject[8]);
    printf("[sxvm] SUMMARY seed=%u programs=%ld accepted=%ld (%.1f%%) with_loop=%ld with_mem=%ld with_call=%ld traps=%ld mem=%ld steps=%ld pc=%ld op=%ld uninit=%ld K_fail=%d host_over=%u\n",
           seed, count, acc, 100.0 * acc / (count ? count : 1), acc_loop, acc_mem, acc_call, ntrap,
           trap[SXVM_T_MEM], trap[SXVM_T_STEPS], trap[SXVM_T_PC], trap[SXVM_T_OP], trap[SXVM_T_UNINIT], k_fail, host.over);
    return (ntrap == 0 && k_fail == 0 && acc * 10 >= count && acc_loop > 0 && acc_mem > 0) ? 0 : 1;
}
