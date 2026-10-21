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
 *   sxvm_test <count> <seed>   prints SUMMARY, exit 0 = no trap and K holds.
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

/* a random program: most registers written first, then mixed ops */
static int gen(sxvm_insn *p)
{
    int n = 0, len = rr(6, 40);
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
        int t = sxvm_run(p, n, mem, b, &r0, &steps);
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
    k_expect("sum-unmasked", b, 7, SXVM_E_MEM, 0, 0);
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
}

int main(int argc, char **argv)
{
    long count = argc > 1 ? atol(argv[1]) : 200000;
    uint32_t seed = argc > 2 ? (uint32_t)atoi(argv[2]) : 1;
    static sxvm_insn p[SXVM_MAXINSN];
    long acc = 0, acc_loop = 0, acc_mem = 0, trap[8] = { 0 }, ntrap = 0, reject[16] = { 0 };
    int shown = 0;

    setvbuf(stdout, NULL, _IOLBF, 0);
    known();
    rs = 0x5A5A1234ULL ^ ((uint64_t)seed << 32) ^ seed;
    for (long it = 0; it < count; it++) {
        int n = gen(p);
        long b = sxvm_check(p, n);
        if (b < 0) { reject[-b & 15]++; continue; }
        acc++;
        int hl = 0, hm = 0;
        for (int i = 0; i < n; i++) { if (p[i].op == SX_LOOP) hl = 1; if (p[i].op == SX_LDB || p[i].op == SX_STB) hm = 1; }
        acc_loop += hl; acc_mem += hm;
        uint8_t mem[SXVM_MEM];
        for (int i = 0; i < SXVM_MEM; i++) mem[i] = (uint8_t)rnd();
        uint32_t r0;
        long steps;
        int t = sxvm_run(p, n, mem, b, &r0, &steps);
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
    printf("[sxvm] SUMMARY seed=%u programs=%ld accepted=%ld (%.1f%%) with_loop=%ld with_mem=%ld traps=%ld mem=%ld steps=%ld pc=%ld op=%ld uninit=%ld K_fail=%d\n",
           seed, count, acc, 100.0 * acc / (count ? count : 1), acc_loop, acc_mem, ntrap,
           trap[SXVM_T_MEM], trap[SXVM_T_STEPS], trap[SXVM_T_PC], trap[SXVM_T_OP], trap[SXVM_T_UNINIT], k_fail);
    return (ntrap == 0 && k_fail == 0 && acc * 10 >= count && acc_loop > 0 && acc_mem > 0) ? 0 : 1;
}
