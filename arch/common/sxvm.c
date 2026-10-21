/*
 *  sxvm.c — the checker and the interpreter of the small checked runtime
 *  (sxvm.h, docs/architecture/30-module/research/safe-exec.md).
 *
 *  The checker is one forward pass: jumps go forward only, so visiting the
 *  instructions in order sees every incoming path before the instruction
 *  itself. Each register carries "written?" and an unsigned range [lo, hi].
 *  A loop body is analysed once from a widened state: every register the
 *  body writes is given the full range at the body's entry (it may have any
 *  value on a later pass), so one pass covers every pass.
 *
 *  Breakers (research only, each must turn the fuzz in tests/host/sxvm_test.c
 *  red): -DSXVM_BREAK_EDGE (the area's end one byte loose), -DSXVM_BREAK_BACKJ
 *  (backward jumps accepted), -DSXVM_BREAK_WIDEN (no widening at a loop
 *  entry), -DSXVM_BREAK_ADDOVF (additions tracked without the overflow test),
 *  -DSXVM_BREAK_PUBLEN (stage 2: pub checked on its start only, not its length),
 *  -DSXVM_BREAK_CLOBBER (a call in a loop body not seen at the loop entry;
 *  added after stage 2 was fixed, not one of the fixed breakers).
 */
#include "sxvm.h"

#define FULL 0xFFFFFFFFu

typedef struct {
    uint8_t reached, init;            /* init: bit r = register r written */
    uint32_t lo[SXVM_NREG], hi[SXVM_NREG];
} sx_state;

static sx_state st[SXVM_MAXINSN + 1];   /* not reentrant: one checker at a time */

static void st_join(sx_state *a, const sx_state *b)
{
    if (!b->reached) return;
    if (!a->reached) { *a = *b; return; }
    a->init &= b->init;
    for (int r = 0; r < SXVM_NREG; r++) {
        if (b->lo[r] < a->lo[r]) a->lo[r] = b->lo[r];
        if (b->hi[r] > a->hi[r]) a->hi[r] = b->hi[r];
    }
}

static int writes_reg(uint8_t op) { return op >= SX_MOVI && op <= SX_LDB; }
#define CLOBBER 0x3Eu                       /* r1..r5 are unwritten after a call */

static int reads_d(uint8_t op)
{
    return (op >= SX_ADD && op <= SX_SHRI) || op == SX_STB || op == SX_JEQI || op == SX_JLTI || op == SX_JLT;
}

static int reads_s(uint8_t op)
{
    return (op >= SX_MOV && op <= SX_DIV) || op == SX_LDB || op == SX_STB || op == SX_JLT;
}

static int is_jump(uint8_t op) { return op == SX_JA || op == SX_JEQI || op == SX_JLTI || op == SX_JLT; }

static uint32_t fill_bits(uint32_t m)
{
    uint32_t f = 0;
    while (f < m) f = (f << 1) | 1u;
    return f;
}

/* the byte access [r + imm] stays inside the area for every value in range */
static int mem_ok(const sx_state *s, int r, int32_t imm)
{
    int64_t lo = (int64_t)s->lo[r] + imm, hi = (int64_t)s->hi[r] + imm;
#ifdef SXVM_BREAK_EDGE
    return lo >= 0 && hi <= SXVM_MEM;
#else
    return lo >= 0 && hi <= SXVM_MEM - 1;
#endif
}

static void set_range(sx_state *s, int r, uint64_t lo, uint64_t hi)
{
    if (hi > FULL) { lo = 0; hi = FULL; }
    s->lo[r] = (uint32_t)lo; s->hi[r] = (uint32_t)hi;
    s->init |= (uint8_t)(1u << r);
}

static void add_range(sx_state *s, int d, uint64_t lo, uint64_t hi, int64_t k_lo, int64_t k_hi)
{
    int64_t nlo = (int64_t)lo + k_lo, nhi = (int64_t)hi + k_hi;
#ifdef SXVM_BREAK_ADDOVF
    uint32_t wlo = (uint32_t)nlo, whi = (uint32_t)nhi;     /* the wrapped ends, as if nothing wrapped */
    if (wlo > whi) wlo = 0;
    s->lo[d] = wlo; s->hi[d] = whi; s->init |= (uint8_t)(1u << d);
#else
    if (nlo < 0 || nhi > (int64_t)FULL) set_range(s, d, 0, FULL);
    else set_range(s, d, (uint64_t)nlo, (uint64_t)nhi);
#endif
}

long sxvm_check(const sxvm_insn *p, int n)
{
    if (n <= 0 || n > SXVM_MAXINSN) return SXVM_E_LEN;
    int lb0 = -1, lb1 = -1;                 /* the loop body being scanned, if any */
    long bound = 0;
    static int body_of[SXVM_MAXINSN];       /* index of the LOOP owning i, or -1 */

    /* 1. shape: ops, registers, loops, jump targets */
    for (int i = 0; i < n; i++) body_of[i] = -1;
    for (int i = 0; i < n; i++) {
        const sxvm_insn *x = &p[i];
        if (x->op < SX_MOVI || x->op >= SX_NOP_LAST) return SXVM_E_OP;
        if (x->op != SX_LOOP && x->d >= SXVM_NREG) return SXVM_E_OP;
        if (reads_s(x->op) && x->s >= SXVM_NREG) return SXVM_E_OP;
        if ((x->op == SX_SHLI || x->op == SX_SHRI) && (x->imm < 0 || x->imm > 31)) return SXVM_E_IMM;
        if (i > lb1) lb0 = lb1 = -1;
        if (x->op == SX_LOOP) {
            if (lb0 >= 0) return SXVM_E_LOOP;                       /* nested */
            if (x->d < 1 || x->imm < 1 || x->imm > SXVM_LOOPMAX) return SXVM_E_LOOP;
            if (i + x->d >= n) return SXVM_E_LOOP;                  /* body past the end */
            lb0 = i + 1; lb1 = i + x->d;
            for (int k = lb0; k <= lb1; k++) body_of[k] = i;
            bound += 1 + (long)x->imm * x->d;
        } else if (lb0 < 0) bound += 1;
    }
    for (int i = 0; i < n; i++) {
        const sxvm_insn *x = &p[i];
        if (!is_jump(x->op)) continue;
        long t = (long)i + 1 + x->imm;
#ifdef SXVM_BREAK_BACKJ
        if (t < 0 || t >= n) return SXVM_E_JUMP;
#else
        if (x->imm < 0 || t >= n) return SXVM_E_JUMP;
#endif
        int L = body_of[i];
        if (L >= 0) {                                               /* inside: stay in, or end the pass */
            if (t > L + p[L].d + 1) return SXVM_E_JUMP;
            if (t < L + 1) return SXVM_E_JUMP;
        } else if (t < n && body_of[t] >= 0) return SXVM_E_JUMP;   /* never jump into a body */
    }

    /* 2. ranges, one forward pass */
    for (int i = 0; i <= n; i++) st[i].reached = 0;
    st[0].reached = 1; st[0].init = 0;
    for (int r = 0; r < SXVM_NREG; r++) { st[0].lo[r] = 0; st[0].hi[r] = FULL; }

    for (int i = 0; i < n; i++) {
        if (!st[i].reached) continue;
        const sxvm_insn *x = &p[i];
        sx_state s = st[i];
        int d = x->d, sr = x->s;
        if (reads_d(x->op) && !(s.init & (1u << d))) return SXVM_E_UNINIT;
        if (reads_s(x->op) && !(s.init & (1u << sr))) return SXVM_E_UNINIT;
        if (x->op == SX_EXIT && !(s.init & 1u)) return SXVM_E_UNINIT;
        if (x->op == SX_CALL) {
            if (x->imm != SXVM_CALL_PUB && x->imm != SXVM_CALL_LOG) return SXVM_E_OP;
            if (!(s.init & 2u) || (x->imm == SXVM_CALL_PUB && !(s.init & 4u))) return SXVM_E_UNINIT;
        }
        uint64_t dl = s.lo[d & 7], dh = s.hi[d & 7], sl = s.lo[sr & 7], sh = s.hi[sr & 7];
        int next = i + 1, fall = 1;

        switch (x->op) {
        case SX_MOVI: set_range(&s, d, (uint32_t)x->imm, (uint32_t)x->imm); break;
        case SX_MOV:  set_range(&s, d, sl, sh); break;
        case SX_ADD:  add_range(&s, d, dl, dh, (int64_t)sl, (int64_t)sh); break;
        case SX_ADDI: add_range(&s, d, dl, dh, x->imm, x->imm); break;
        case SX_SUB:
            if (dl >= sh) set_range(&s, d, dl - sh, dh - sl); else set_range(&s, d, 0, FULL);
            break;
        case SX_MUL:  set_range(&s, d, dl * sl, dh * sh); break;     /* both < 2^32: no 64-bit overflow */
        case SX_AND:  set_range(&s, d, 0, dh < sh ? dh : sh); break;
        case SX_ANDI: set_range(&s, d, 0, dh < (uint32_t)x->imm ? dh : (uint32_t)x->imm); break;
        case SX_OR: case SX_XOR: set_range(&s, d, 0, fill_bits((uint32_t)(dh > sh ? dh : sh))); break;
        case SX_DIV:  set_range(&s, d, 0, dh); break;
        case SX_SHLI: set_range(&s, d, dl << x->imm, dh << x->imm); break;
        case SX_SHRI: set_range(&s, d, dl >> x->imm, dh >> x->imm); break;
        case SX_LDB:
            if (!mem_ok(&s, sr, x->imm)) return SXVM_E_MEM;
            set_range(&s, d, 0, 255);
            break;
        case SX_STB:
            if (!mem_ok(&s, d, x->imm)) return SXVM_E_MEM;
            break;
        case SX_JA:
            fall = 0; next = i + 1 + x->imm;
            break;
        case SX_JEQI: case SX_JLTI: case SX_JLT: {
            sx_state tk = s, fl = s;
            int t = i + 1 + x->imm, tk_ok = 1, fl_ok = 1;
            uint32_t c = x->s;
            if (x->op == SX_JEQI) {
                if (c < dl || c > dh) tk_ok = 0; else { tk.lo[d] = tk.hi[d] = c; }
                if (dl == c && dh == c) fl_ok = 0;
            } else if (x->op == SX_JLTI) {
                if (dl < c) { if (tk.hi[d] > c - 1) tk.hi[d] = c - 1; } else tk_ok = 0;
                if (dh >= c) { if (fl.lo[d] < c) fl.lo[d] = c; } else fl_ok = 0;
            } else if (d == sr) {
                tk_ok = 0;                                          /* x < x never */
            } else {
                if (dl < sh) {
                    if (tk.hi[d] > sh - 1) tk.hi[d] = (uint32_t)(sh - 1);
                    if (tk.lo[sr] < dl + 1) tk.lo[sr] = (uint32_t)(dl + 1);
                } else tk_ok = 0;
                if (dh >= sl) {
                    if (fl.lo[d] < sl) fl.lo[d] = (uint32_t)sl;
                    if (fl.hi[sr] > dh) fl.hi[sr] = (uint32_t)dh;
                } else fl_ok = 0;
            }
            if (tk_ok) { tk.reached = 1; if (t >= n) return SXVM_E_FALL; if (t > i) st_join(&st[t], &tk); }
            if (!fl_ok) fall = 0; else s = fl;
            break;
        }
        case SX_LOOP: {
#ifndef SXVM_BREAK_WIDEN
            for (int k = i + 1; k <= i + d; k++) {
                if (writes_reg(p[k].op)) { s.lo[p[k].d] = 0; s.hi[p[k].d] = FULL; }
#ifndef SXVM_BREAK_CLOBBER
                if (p[k].op == SX_CALL) {           /* a later pass may start after a call */
                    s.lo[0] = 0; s.hi[0] = FULL;
                    s.init &= (uint8_t)~CLOBBER;
                }
#endif
            }
            /* a register written only inside stays "unwritten" at the entry:
             * the first pass reads it unwritten, so that is already sound */
#endif
            break;
        }
        case SX_EXIT: fall = 0; break;
        case SX_CALL:
#ifdef SXVM_BREAK_PUBLEN
            if (x->imm == SXVM_CALL_PUB && s.hi[1] > SXVM_MEM - 1) return SXVM_E_MEM;
#else
            if (x->imm == SXVM_CALL_PUB && (uint64_t)s.hi[1] + s.hi[2] > SXVM_MEM) return SXVM_E_MEM;
#endif
            s.init &= (uint8_t)~CLOBBER;
            set_range(&s, 0, 0, FULL);
            break;
        default: return SXVM_E_OP;
        }
        if (x->op == SX_JA) {
            if (next >= n) return SXVM_E_FALL;
            if (next > i) { s.reached = 1; st_join(&st[next], &s); }
            continue;
        }
        if (fall) {
            if (next >= n) return SXVM_E_FALL;
            s.reached = 1;
            st_join(&st[next], &s);
        }
    }
    return bound;
}

int sxvm_run(const sxvm_insn *p, int n, uint8_t mem[SXVM_MEM], long max_steps,
             const sxvm_env *env, uint32_t *r0, long *steps)
{
    uint32_t r[SXVM_NREG] = { 0 };
    unsigned init = 0;
    int pc = 0, lb0 = -1, lb1 = -1;
    long rem = 0, k = 0;
    int trap = SXVM_T_OK;

    for (;;) {
        if (lb0 >= 0) {
            if (pc == lb1 + 1) {                    /* one pass of the body is over */
                if (--rem > 0) pc = lb0; else lb0 = lb1 = -1;
            } else if (pc < lb0 || pc > lb1 + 1) lb0 = lb1 = -1;
        }
        if (pc < 0 || pc >= n) { trap = SXVM_T_PC; break; }
        if (k >= max_steps) { trap = SXVM_T_STEPS; break; }
        k++;
        const sxvm_insn *x = &p[pc];
        uint8_t op = x->op, d = x->d, s = x->s;
        if (op < SX_MOVI || op >= SX_NOP_LAST || (op != SX_LOOP && d >= SXVM_NREG) || (reads_s(op) && s >= SXVM_NREG)) {
            trap = SXVM_T_OP; break;
        }
        if ((reads_d(op) && !(init & (1u << d))) || (reads_s(op) && !(init & (1u << s))) ||
            (op == SX_EXIT && !(init & 1u))) { trap = SXVM_T_UNINIT; break; }
        int64_t a;
        int next = pc + 1;
        switch (op) {
        case SX_MOVI: r[d] = (uint32_t)x->imm; break;
        case SX_MOV:  r[d] = r[s]; break;
        case SX_ADD:  r[d] += r[s]; break;
        case SX_SUB:  r[d] -= r[s]; break;
        case SX_MUL:  r[d] *= r[s]; break;
        case SX_AND:  r[d] &= r[s]; break;
        case SX_OR:   r[d] |= r[s]; break;
        case SX_XOR:  r[d] ^= r[s]; break;
        case SX_DIV:  r[d] = r[s] ? r[d] / r[s] : 0; break;
        case SX_ADDI: r[d] += (uint32_t)x->imm; break;
        case SX_ANDI: r[d] &= (uint32_t)x->imm; break;
        case SX_SHLI: case SX_SHRI:
            if (x->imm < 0 || x->imm > 31) { trap = SXVM_T_OP; goto out; }
            r[d] = op == SX_SHLI ? r[d] << x->imm : r[d] >> x->imm;
            break;
        case SX_LDB:
            a = (int64_t)r[s] + x->imm;
            if (a < 0 || a >= SXVM_MEM) { trap = SXVM_T_MEM; goto out; }
            r[d] = mem[a];
            break;
        case SX_STB:
            a = (int64_t)r[d] + x->imm;
            if (a < 0 || a >= SXVM_MEM) { trap = SXVM_T_MEM; goto out; }
            mem[a] = (uint8_t)r[s];
            break;
        case SX_JA:   next = pc + 1 + x->imm; break;
        case SX_JEQI: if (r[d] == s) next = pc + 1 + x->imm; break;
        case SX_JLTI: if (r[d] < s) next = pc + 1 + x->imm; break;
        case SX_JLT:  if (r[d] < r[s]) next = pc + 1 + x->imm; break;
        case SX_LOOP:
            if (lb0 >= 0 || d < 1 || x->imm < 1) { trap = SXVM_T_OP; goto out; }
            lb0 = pc + 1; lb1 = pc + d; rem = x->imm;
            break;
        case SX_EXIT:
            if (r0) *r0 = r[0];
            goto out;
        case SX_CALL:
            if (!(init & 2u)) { trap = SXVM_T_UNINIT; goto out; }
            if (x->imm == SXVM_CALL_PUB) {
                if (!(init & 4u)) { trap = SXVM_T_UNINIT; goto out; }
                if ((uint64_t)r[1] + r[2] > SXVM_MEM) { trap = SXVM_T_MEM; goto out; }
                r[0] = env && env->pub ? env->pub(env->ctx, mem + r[1], r[2]) : 0;
            } else if (x->imm == SXVM_CALL_LOG) {
                if (env && env->log) env->log(env->ctx, r[1]);
                r[0] = 0;
            } else { trap = SXVM_T_OP; goto out; }
            init = (init & ~CLOBBER) | 1u;
            break;
        default: trap = SXVM_T_OP; goto out;
        }
        if (writes_reg(op)) init |= 1u << d;
        pc = next;
    }
out:
    if (steps) *steps = k;
    return trap;
}
