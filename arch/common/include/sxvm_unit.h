/*
 *  sxvm_unit.h — the acceptance gate for sxvm programs (research stage 3,
 *  docs/architecture/30-module/research/safe-exec.md, part of ROADMAP 2-8).
 *
 *  wire image:
 *    "PKSX" | ver (1) | n (2, LE) | prev id (32) | n x insn (8: op d s pad imm-LE)
 *    | signer pk (32) | sig (64, over everything before it)
 *  content id = the first 32 bytes of SHA-512 over everything before the
 *  signer pk. A unit names the id of the version it replaces (zeros for the
 *  first one).
 *
 *  sxvm_accept checks, in this order: shape, signer in the table, signature,
 *  successor, the checker (always, signed or not: a signature says who made
 *  it, not that it is safe — global rule 6), step bound <= budget.
 */
#ifndef SXVM_UNIT_H
#define SXVM_UNIT_H

#include "sxvm.h"
#include "ed25519.h"

#define SXVM_UNIT_VER      1
#define SXVM_ID_LEN        32
#define SXVM_UNIT_HEAD     (4 + 1 + 2 + SXVM_ID_LEN)
#define SXVM_UNIT_LEN(n)   (SXVM_UNIT_HEAD + 8 * (n) + ED25519_PUBLIC_KEY_LEN + ED25519_SIGNATURE_LEN)
#define SXVM_UNIT_MAX      SXVM_UNIT_LEN(SXVM_MAXINSN)

#define SXVM_A_OK          0
#define SXVM_A_SHAPE     (-20)   /* magic, version, length, pad       */
#define SXVM_A_SIGNER    (-21)   /* signer not in the table           */
#define SXVM_A_SIG       (-22)   /* signature does not verify         */
#define SXVM_A_SUCC      (-23)   /* not the successor of prev_id      */
#define SXVM_A_CHECK     (-24)   /* the checker refused (see *why)    */
#define SXVM_A_BUDGET    (-25)   /* step bound above the budget       */

typedef struct {
    const unsigned char (*keys)[ED25519_PUBLIC_KEY_LEN];
    int nkeys;
} sxvm_keytab;

/* encode + sign (the author's side). Returns the length written, or < 0. */
int sxvm_unit_build(unsigned char *out, int cap, const sxvm_insn *p, int n,
                    const unsigned char prev_id[SXVM_ID_LEN],
                    const unsigned char pk[ED25519_PUBLIC_KEY_LEN],
                    const unsigned char sk[ED25519_SECRET_KEY_LEN]);

void sxvm_unit_id(const unsigned char *u, int len, unsigned char id[SXVM_ID_LEN]);

/* the receiver's side. On SXVM_A_OK: p/n hold the program, *bound its step
 * bound, id (if not NULL) its content id. *why gets the checker's code on
 * SXVM_A_CHECK. */
int sxvm_accept(const unsigned char *u, int len, const sxvm_keytab *kt,
                const unsigned char prev_id[SXVM_ID_LEN], long budget,
                sxvm_insn *p, int *n, long *bound, unsigned char id[SXVM_ID_LEN], long *why);

#endif
