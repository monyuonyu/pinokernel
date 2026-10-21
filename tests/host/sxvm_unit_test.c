/*
 * sxvm_unit_test.c — [sxvm-unit] (research, not wired into CI): stage 3 of
 * docs/architecture/30-module/research/safe-exec.md (U1-U9 fixed there
 * before this file was written).
 *
 *   sxvm_unit_test     prints one line per case and "RED <list>" (the cases
 *                      that did not hold), exit 0 = none red.
 *   run_sxvm_unit.sh builds the head and the breakers A1-A5 and requires
 *   each breaker to turn exactly its own cases red.
 */
#include <stdio.h>
#include <string.h>
#include "sxvm_unit.h"

#define I(op, d, s, imm) { (uint8_t)(op), (uint8_t)(d), (uint8_t)(s), 0, (imm) }

static const sxvm_insn good[] = {       /* the sum of the area, index masked (bound 260) */
    I(SX_MOVI, 0, 0, 0), I(SX_MOVI, 1, 0, 0),
    I(SX_LOOP, 4, 0, 64),
    I(SX_ANDI, 1, 0, 63), I(SX_LDB, 2, 1, 0), I(SX_ADD, 0, 2, 0), I(SX_ADDI, 1, 0, 1),
    I(SX_EXIT, 0, 0, 0) };
static const sxvm_insn unsafe[] = {     /* the same without the mask */
    I(SX_MOVI, 0, 0, 0), I(SX_MOVI, 1, 0, 0),
    I(SX_LOOP, 3, 0, 64),
    I(SX_LDB, 2, 1, 0), I(SX_ADD, 0, 2, 0), I(SX_ADDI, 1, 0, 1),
    I(SX_EXIT, 0, 0, 0) };
#define NG ((int)(sizeof good / sizeof good[0]))
#define NU ((int)(sizeof unsafe / sizeof unsafe[0]))
#define BIG 1000000000L

static char red[128];
static void expect(const char *name, int got, int want, int extra_ok)
{
    int ok = got == want && extra_ok;
    printf("[sxvm-unit] %-4s got=%d want=%d %s\n", name, got, want, ok ? "ok" : "RED");
    if (!ok) { strcat(red, " "); strcat(red, name); }
}

int main(void)
{
    unsigned char seedA[32], seedB[32], pkA[32], skA[64], pkB[32], skB[64];
    for (int i = 0; i < 32; i++) { seedA[i] = (unsigned char)(i + 1); seedB[i] = (unsigned char)(0xA0 + i); }
    ed25519_keypair_from_seed(seedA, pkA, skA);
    ed25519_keypair_from_seed(seedB, pkB, skB);
    unsigned char keysA[1][32];
    memcpy(keysA[0], pkA, 32);
    sxvm_keytab tabA = { (const unsigned char (*)[32])keysA, 1 }, empty = { 0, 0 };
    unsigned char zero[SXVM_ID_LEN] = { 0 }, id1[SXVM_ID_LEN], id2[SXVM_ID_LEN], other[SXVM_ID_LEN];
    static unsigned char u[SXVM_UNIT_MAX], v[SXVM_UNIT_MAX];
    sxvm_insn p[SXVM_MAXINSN];
    int n, len, r;
    long bound, why;

    setvbuf(stdout, NULL, _IOLBF, 0);
    if (ed25519_self_test() != 0) { printf("[sxvm-unit] ed25519 self test FAILED\n"); return 2; }

    /* U1: a good unit passes, decodes to the same program, and runs */
    len = sxvm_unit_build(u, sizeof u, good, NG, zero, pkA, skA);
    r = sxvm_accept(u, len, &tabA, zero, BIG, p, &n, &bound, id1, &why);
    int same = r == SXVM_A_OK && n == NG && !memcmp(p, good, sizeof good);
    uint8_t mem[SXVM_MEM];
    for (int i = 0; i < SXVM_MEM; i++) mem[i] = (uint8_t)i;
    uint32_t r0 = 0;
    long steps;
    int t = same ? sxvm_run(p, n, mem, bound, NULL, &r0, &steps) : -1;
    expect("U1", r, SXVM_A_OK, same && bound == 260 && t == SXVM_T_OK && r0 == 2016);
    /* U1b: its successor names it and passes */
    len = sxvm_unit_build(v, sizeof v, good, NG, id1, pkA, skA);
    r = sxvm_accept(v, len, &tabA, id1, BIG, p, &n, &bound, id2, &why);
    expect("U1b", r, SXVM_A_OK, memcmp(id1, id2, SXVM_ID_LEN) != 0);

    /* U2: one byte of an instruction changed (ANDI 63 -> 62, still a safe program) */
    len = sxvm_unit_build(u, sizeof u, good, NG, zero, pkA, skA);
    u[SXVM_UNIT_HEAD + 8 * 3 + 4] ^= 1;
    expect("U2", sxvm_accept(u, len, &tabA, zero, BIG, p, &n, &bound, NULL, &why), SXVM_A_SIG, 1);

    /* U3: a valid signature by a signer not in the table */
    len = sxvm_unit_build(u, sizeof u, good, NG, zero, pkB, skB);
    expect("U3", sxvm_accept(u, len, &tabA, zero, BIG, p, &n, &bound, NULL, &why), SXVM_A_SIGNER, 1);

    /* U4: an empty table refuses everything */
    len = sxvm_unit_build(u, sizeof u, good, NG, zero, pkA, skA);
    expect("U4", sxvm_accept(u, len, &empty, zero, BIG, p, &n, &bound, NULL, &why), SXVM_A_SIGNER, 1);

    /* U5: names a predecessor that is not the current version */
    memset(other, 0x5A, sizeof other);
    len = sxvm_unit_build(u, sizeof u, good, NG, other, pkA, skA);
    expect("U5", sxvm_accept(u, len, &tabA, id1, BIG, p, &n, &bound, NULL, &why), SXVM_A_SUCC, 1);

    /* U6: signed by a known signer, but the checker refuses */
    len = sxvm_unit_build(u, sizeof u, unsafe, NU, zero, pkA, skA);
    why = 0;
    r = sxvm_accept(u, len, &tabA, zero, BIG, p, &n, &bound, NULL, &why);
    expect("U6", r, SXVM_A_CHECK, why == SXVM_E_MEM);

    /* U7: the bound (260) above the budget */
    len = sxvm_unit_build(u, sizeof u, good, NG, zero, pkA, skA);
    expect("U7", sxvm_accept(u, len, &tabA, zero, 259, p, &n, &bound, NULL, &why), SXVM_A_BUDGET, 1);

    /* U8: cut short; and a count that does not match the length */
    len = sxvm_unit_build(u, sizeof u, good, NG, zero, pkA, skA);
    r = sxvm_accept(u, len - 1, &tabA, zero, BIG, p, &n, &bound, NULL, &why);
    u[5] = (unsigned char)(NG - 1);
    int r2 = sxvm_accept(u, len, &tabA, zero, BIG, p, &n, &bound, NULL, &why);
    expect("U8", r, SXVM_A_SHAPE, r2 == SXVM_A_SHAPE);

    /* U9: a non-zero pad byte, signed again so the signature holds */
    len = sxvm_unit_build(u, sizeof u, good, NG, zero, pkA, skA);
    u[SXVM_UNIT_HEAD + 8 * 1 + 3] = 7;
    {
        int bl = SXVM_UNIT_HEAD + 8 * NG;
        ed25519_sign(u + bl + 32, u, (size_t)(bl + 32), skA);
    }
    expect("U9", sxvm_accept(u, len, &tabA, zero, BIG, p, &n, &bound, NULL, &why), SXVM_A_SHAPE, 1);

    printf("[sxvm-unit] RED%s\n", red[0] ? red : " (none)");
    return red[0] ? 1 : 0;
}
