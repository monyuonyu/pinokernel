/*
 * sxvm_ai_test.c — [sxvm-ai] stage 5c of docs/architecture/30-module/
 * research/safe-exec-log.md (research, not wired into CI): eight handlers
 * written by another agent from sxvm.h only (the file is not in the tree;
 * built with -include <ai_progs.c>). Each is checked, and when accepted run
 * on 1000 inputs (mem[63] = L in 0..63, mem[0..L) data) against the C
 * reference below.
 *
 *   sxvm_ai_test [count]   prints one AI line per task
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sxvm.h"

int ai_T1(sxvm_insn *q); int ai_T2(sxvm_insn *q); int ai_T3(sxvm_insn *q); int ai_T4(sxvm_insn *q);
int ai_T5(sxvm_insn *q); int ai_T6(sxvm_insn *q); int ai_T7(sxvm_insn *q); int ai_T8(sxvm_insn *q);
static int (*const progs[8])(sxvm_insn *) = { ai_T1, ai_T2, ai_T3, ai_T4, ai_T5, ai_T6, ai_T7, ai_T8 };

typedef struct { uint8_t last[SXVM_MEM]; uint32_t len, npub, over; } host_t;
static uint32_t h_pub(void *c, const uint8_t *b, uint32_t len)
{
    host_t *h = c;
    if (len > SXVM_MEM) { h->over++; return 0; }
    memcpy(h->last, b, len); h->len = len; h->npub++;
    return len;
}
static void h_log(void *c, uint32_t v) { (void)c; (void)v; }
static host_t host;
static const sxvm_env env = { &host, h_pub, h_log };

static uint64_t rs = 0xA15EEDULL;
static uint32_t rnd(void) { rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17; return (uint32_t)(rs >> 11); }

/* the reference: r0, and the expected published bytes for T5/T8 */
static uint32_t ref(int t, const uint8_t *m, uint8_t *out)
{
    int L = m[63];
    uint32_t v = 0;
    switch (t) {
    case 1: for (int i = 0; i < L; i++) v += m[i] >= '0' && m[i] <= '9'; return v;
    case 2: v = 255; for (int i = 0; i < L; i++) if (m[i] < v) v = m[i]; return v;
    case 3: for (int i = 0; i < L; i++) v ^= m[i]; return v;
    case 4: for (int i = 0; i + 1 < L; i++) v += m[i] == m[i + 1]; return v;
    case 5: for (int i = 0; i < L; i++) out[i] = m[i] >= 'A' && m[i] <= 'Z' ? (uint8_t)(m[i] + 32) : m[i]; return (uint32_t)L;
    case 6: v = 255; for (int i = 0; i < L; i++) if (m[i] == ' ') v = (uint32_t)i; return v;
    case 7: for (int i = 0; i < L; i += 2) v += m[i]; return v;
    case 8: for (int i = 0; i < L; i++) out[i] = m[L - 1 - i]; return (uint32_t)L;
    }
    return 0;
}

int main(int argc, char **argv)
{
    int count = argc > 1 ? atoi(argv[1]) : 1000;
    static const uint8_t mix[] = { ' ', '0', '5', '9', 'A', 'M', 'Z', 'a', '@', '[', '/', ':', 0, 255 };
    setvbuf(stdout, NULL, _IOLBF, 0);
    for (int t = 1; t <= 8; t++) {
        static sxvm_insn q[SXVM_MAXINSN];
        int n = progs[t - 1](q);
#ifdef AI_STRIP
        /* added after seeing the masked run (not fixed): every ANDI x,63 becomes
         * ADDI x,0 (same offsets), the agent's masks taken away */
        for (int k = 0; k < n; k++) if (q[k].op == SX_ANDI && q[k].imm == 63) { q[k].op = SX_ADDI; q[k].imm = 0; }
#endif
        long b = sxvm_check(q, n), run = 0, trap = 0, agree = 0;
        for (int it = 0; b >= 0 && it < count; it++) {
            uint8_t mem[SXVM_MEM], m0[SXVM_MEM], want[SXVM_MEM];
            for (int k = 0; k < SXVM_MEM; k++) mem[k] = rnd() % 3 ? mix[rnd() % sizeof mix] : (uint8_t)rnd();
            if (rnd() % 4 == 0) for (int k = 1; k < SXVM_MEM; k += 2) mem[k] = mem[k - 1];   /* T4: some runs of equals */
            mem[63] = (uint8_t)(it < 64 ? it : (int)(rnd() % 64));
            memcpy(m0, mem, sizeof mem);
            memset(&host, 0, sizeof host);
            uint32_t r0 = 0; long steps = 0;
            int tr = sxvm_run(q, n, mem, b, &env, &r0, &steps);
            run++;
            if (tr != SXVM_T_OK || host.over) { trap++; continue; }
            uint32_t w = ref(t, m0, want);
            int ok = r0 == w;
            if (t == 5 || t == 8) ok = ok && host.npub == 1 && host.len == w && !memcmp(host.last, want, w);
            agree += ok;
        }
        printf("[sxvm] AI T%d n=%d check=%ld run=%ld trap=%ld agree=%ld\n", t, n, b, run, trap, agree);
    }
    return 0;
}
