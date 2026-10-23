/*
 *  sxvm_unit.c — the acceptance gate for sxvm programs (sxvm_unit.h).
 *
 *  Breakers (research only, each must turn exactly its own case of
 *  tests/host/sxvm_unit_test.c red): -DSXVM_BREAK_NOSIG (signature not
 *  verified), -DSXVM_BREAK_NOTAB (signer table not consulted),
 *  -DSXVM_BREAK_NOSUCC (successor not compared), -DSXVM_BREAK_SIGNEDSAFE
 *  (a signed unit skips the checker), -DSXVM_BREAK_NOBUDGET (bound not
 *  compared with the budget).
 */
#include "sxvm_unit.h"

static int body_len(int n) { return SXVM_UNIT_HEAD + 8 * n; }

static int bytes_eq(const unsigned char *a, const unsigned char *b, int len)
{
    unsigned char d = 0;
    for (int i = 0; i < len; i++) d |= (unsigned char)(a[i] ^ b[i]);
    return d == 0;
}

static void put_insn(unsigned char *b, const sxvm_insn *x)
{
    uint32_t v = (uint32_t)x->imm;
    b[0] = x->op; b[1] = x->d; b[2] = x->s; b[3] = 0;
    b[4] = (unsigned char)v; b[5] = (unsigned char)(v >> 8); b[6] = (unsigned char)(v >> 16); b[7] = (unsigned char)(v >> 24);
}

void sxvm_unit_id(const unsigned char *u, int len, unsigned char id[SXVM_ID_LEN])
{
    unsigned char h[ED25519_SHA512_LEN];
    int bl = len - ED25519_PUBLIC_KEY_LEN - ED25519_SIGNATURE_LEN;
    ed25519_sha512(u, (size_t)(bl > 0 ? bl : 0), h);
    for (int i = 0; i < SXVM_ID_LEN; i++) id[i] = h[i];
}

int sxvm_unit_build(unsigned char *out, int cap, const sxvm_insn *p, int n,
                    const unsigned char prev_id[SXVM_ID_LEN],
                    const unsigned char pk[ED25519_PUBLIC_KEY_LEN],
                    const unsigned char sk[ED25519_SECRET_KEY_LEN])
{
    if (n <= 0 || n > SXVM_MAXINSN || cap < SXVM_UNIT_LEN(n)) return -1;
    unsigned char *b = out;
    b[0] = 'P'; b[1] = 'K'; b[2] = 'S'; b[3] = 'X'; b[4] = SXVM_UNIT_VER;
    b[5] = (unsigned char)n; b[6] = (unsigned char)(n >> 8);
    for (int i = 0; i < SXVM_ID_LEN; i++) b[7 + i] = prev_id[i];
    for (int i = 0; i < n; i++) put_insn(b + SXVM_UNIT_HEAD + 8 * i, &p[i]);
    int bl = body_len(n);
    for (int i = 0; i < ED25519_PUBLIC_KEY_LEN; i++) b[bl + i] = pk[i];
    if (!ed25519_sign(b + bl + ED25519_PUBLIC_KEY_LEN, b, (size_t)(bl + ED25519_PUBLIC_KEY_LEN), sk)) return -1;
    return SXVM_UNIT_LEN(n);
}

int sxvm_accept(const unsigned char *u, int len, const sxvm_keytab *kt,
                const unsigned char prev_id[SXVM_ID_LEN], long budget,
                sxvm_insn *p, int *n, long *bound, unsigned char id[SXVM_ID_LEN], long *why)
{
    /* 1. shape */
    if (len < SXVM_UNIT_LEN(1) || len > SXVM_UNIT_MAX) return SXVM_A_SHAPE;
    if (u[0] != 'P' || u[1] != 'K' || u[2] != 'S' || u[3] != 'X' || u[4] != SXVM_UNIT_VER) return SXVM_A_SHAPE;
    int cnt = u[5] | (u[6] << 8);
    if (cnt < 1 || cnt > SXVM_MAXINSN || len != SXVM_UNIT_LEN(cnt)) return SXVM_A_SHAPE;
    for (int i = 0; i < cnt; i++) if (u[SXVM_UNIT_HEAD + 8 * i + 3] != 0) return SXVM_A_SHAPE;
    int bl = body_len(cnt);
    const unsigned char *pk = u + bl, *sig = u + bl + ED25519_PUBLIC_KEY_LEN;

    /* 2. signer in the table (an empty table refuses everything) */
#ifndef SXVM_BREAK_NOTAB
    int known = 0;
    for (int k = 0; kt && k < kt->nkeys; k++) if (bytes_eq(kt->keys[k], pk, ED25519_PUBLIC_KEY_LEN)) known = 1;
    if (!known) return SXVM_A_SIGNER;
#else
    (void)kt;
#endif
    /* 3. signature over body + signer */
#ifndef SXVM_BREAK_NOSIG
    if (!ed25519_verify(sig, u, (size_t)(bl + ED25519_PUBLIC_KEY_LEN), pk)) return SXVM_A_SIG;
#else
    (void)sig;
#endif
    /* 4. successor */
#ifndef SXVM_BREAK_NOSUCC
    if (!bytes_eq(u + 7, prev_id, SXVM_ID_LEN)) return SXVM_A_SUCC;
#else
    (void)prev_id;
#endif
    /* 5. decode, then the checker — always */
    for (int i = 0; i < cnt; i++) {
        const unsigned char *b = u + SXVM_UNIT_HEAD + 8 * i;
        p[i].op = b[0]; p[i].d = b[1]; p[i].s = b[2]; p[i].pad = 0;
        p[i].imm = (int32_t)((uint32_t)b[4] | ((uint32_t)b[5] << 8) | ((uint32_t)b[6] << 16) | ((uint32_t)b[7] << 24));
    }
    long b;
#ifdef SXVM_BREAK_SIGNEDSAFE
    (void)why;
    b = 0;                                  /* "it is signed, so it is fine" */
    for (int i = 0; i < cnt; i++) b += SXVM_LOOPMAX * 8;
#else
    b = sxvm_check(p, cnt);
    if (b < 0) { if (why) *why = b; return SXVM_A_CHECK; }
#endif
    /* 6. budget */
#ifndef SXVM_BREAK_NOBUDGET
    if (b > budget) return SXVM_A_BUDGET;
#else
    (void)budget;
#endif
    *n = cnt;
    *bound = b;
    if (id) sxvm_unit_id(u, len, id);
    return SXVM_A_OK;
}
