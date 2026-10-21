/*
 *  sxvm.h — a small checked runtime (research, docs/architecture/30-module/
 *  research/safe-exec.md, ROADMAP "AI OS" candidate (1)). Not linked into
 *  any kernel build yet.
 *
 *  A program is a list of 8-byte instructions over 8 registers (32 bit) and
 *  a 64-byte scratch area. sxvm_check() walks every path before anything
 *  runs: no read of an unwritten register, every byte access inside the
 *  scratch area, jumps forward only, loops only with a constant count and
 *  not nested. It returns the step bound. sxvm_run() checks again at run
 *  time (it does not trust the checker) and reports a trap instead of
 *  touching anything outside the scratch area.
 *
 *  No libc: the same file builds hosted and bare.
 */
#ifndef SXVM_H
#define SXVM_H

#include <stdint.h>

#define SXVM_NREG      8
#define SXVM_MEM       64
#define SXVM_MAXINSN   256
#define SXVM_LOOPMAX   1024

enum {
    SX_MOVI = 1,   /* d = imm                              */
    SX_MOV,        /* d = s                                */
    SX_ADD,        /* d += s      (wraps)                  */
    SX_SUB,        /* d -= s      (wraps)                  */
    SX_MUL,        /* d *= s      (wraps)                  */
    SX_AND,        /* d &= s                               */
    SX_OR,         /* d |= s                               */
    SX_XOR,        /* d ^= s                               */
    SX_DIV,        /* d /= s      (s == 0 gives 0)         */
    SX_ADDI,       /* d += imm    (wraps)                  */
    SX_ANDI,       /* d &= imm                             */
    SX_SHLI,       /* d <<= imm   (imm 0..31)              */
    SX_SHRI,       /* d >>= imm   (imm 0..31)              */
    SX_LDB,        /* d = mem[s + imm]                     */
    SX_STB,        /* mem[d + imm] = s & 0xff              */
    SX_JA,         /* pc += 1 + imm   (imm >= 0)           */
    SX_JEQI,       /* if d == (u32)src8: pc += 1 + imm     */
    SX_JLTI,       /* if d <  (u32)src8: pc += 1 + imm     */
    SX_JLT,        /* if d <  s:         pc += 1 + imm     */
    SX_LOOP,       /* run the next d instructions imm times */
    SX_EXIT,       /* return r0                            */
    SX_CALL,       /* r0 = helper imm (r1..); r1-r5 unwritten after */
    SX_NOP_LAST
};

typedef struct {
    uint8_t op, d, s, pad;
    int32_t imm;
} sxvm_insn;

/* the only ways out (stage 2): helpers chosen by number, like eBPF's */
#define SXVM_CALL_PUB  1      /* pub(mem[r1 .. r1+r2)) -> r0       */
#define SXVM_CALL_LOG  2      /* log(r1) -> r0 = 0                  */
typedef struct {
    void *ctx;
    uint32_t (*pub)(void *ctx, const uint8_t *b, uint32_t len);
    void (*log)(void *ctx, uint32_t v);
} sxvm_env;

/* sxvm_check results (>= 0 is the step bound) */
#define SXVM_E_LEN     (-1)   /* empty or longer than SXVM_MAXINSN  */
#define SXVM_E_OP      (-2)   /* unknown op or bad register         */
#define SXVM_E_JUMP    (-3)   /* backward, out of range, into a loop */
#define SXVM_E_UNINIT  (-4)   /* reads a register never written     */
#define SXVM_E_MEM     (-5)   /* a byte access may leave the area   */
#define SXVM_E_LOOP    (-6)   /* bad count/length, nested, crossing */
#define SXVM_E_FALL    (-7)   /* may run off the end without EXIT   */
#define SXVM_E_IMM     (-8)   /* shift amount out of 0..31          */

long sxvm_check(const sxvm_insn *p, int n);

/* sxvm_run traps (0 = returned through EXIT) */
#define SXVM_T_OK      0
#define SXVM_T_MEM     1      /* would access outside the area      */
#define SXVM_T_STEPS   2      /* more steps than the bound          */
#define SXVM_T_PC      3      /* pc left the program                */
#define SXVM_T_OP      4      /* unknown op / register / shift      */
#define SXVM_T_UNINIT  5      /* read a register never written      */

int sxvm_run(const sxvm_insn *p, int n, uint8_t mem[SXVM_MEM], long max_steps,
             const sxvm_env *env, uint32_t *r0, long *steps);

#endif
