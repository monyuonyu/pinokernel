#!/usr/bin/env python3
"""sxvm_z3.py — stage 4 of docs/architecture/30-module/research/safe-exec.md
(research, not wired into CI).

A second checker for sxvm programs, written from the interpreter
(arch/common/sxvm.c sxvm_run), not from sxvm_check: the 64 scratch bytes and
every pub answer are symbols, the paths are merged at each pc (jumps go
forward only), loops are unrolled pass by pass, and Z3 is asked whether some
input reaches a trap (memory, unwritten register, pc off the end, or a
backward jump for the BACKJ breaker). A "trap" answer comes with a model; the
caller replays it in the C interpreter (sxvm_test replay), which decides.

  sxvm_z3.py <dump> <out> [--only it,it,..] [--first N] [--kind A|R]
             [--zb mem|uninit] [--jobs J] [--timeout ms] [--maxunroll U]
  out lines: "<A|R> it code fuzz z3=<safe|trap|unknown|skip> label" and,
  for trap, a replay line in <out>.replay
"""
import argparse
import sys
from multiprocessing import Pool

import z3

MOVI, MOV, ADD, SUB, MUL, AND, OR, XOR, DIV, ADDI, ANDI, SHLI, SHRI, LDB, STB, \
    JA, JEQI, JLTI, JLT, LOOP, EXIT, CALL = range(1, 23)
MEM = 64
CALL_PUB, CALL_LOG = 1, 2
ZB = None


def reads_d(op):
    return ADD <= op <= SHRI or op in (STB, JEQI, JLTI, JLT)


def reads_s(op):
    return MOV <= op <= DIV or op in (LDB, STB, JLT)


def writes_reg(op):
    return MOVI <= op <= LDB


class St:
    __slots__ = ("g", "r", "ini", "mem", "npub")

    def __init__(self, g, r, ini, mem, npub):
        self.g, self.r, self.ini, self.mem, self.npub = g, r, ini, mem, npub

    def copy(self, g=None):
        return St(self.g if g is None else g, list(self.r), list(self.ini), self.mem, self.npub)


def merge(sts):
    if len(sts) == 1:
        return sts[0]
    base = sts[-1].copy()
    g = z3.Or([s.g for s in sts])
    for s in reversed(sts[:-1]):         # the incoming paths are disjoint
        c = s.g
        for i in range(8):
            if not s.r[i].eq(base.r[i]):
                base.r[i] = z3.If(c, s.r[i], base.r[i])
            if not s.ini[i].eq(base.ini[i]):
                base.ini[i] = z3.If(c, s.ini[i], base.ini[i])
        if not s.mem.eq(base.mem):
            base.mem = z3.If(c, s.mem, base.mem)
        if not s.npub.eq(base.npub):
            base.npub = z3.If(c, s.npub, base.npub)
    base.g = z3.simplify(g)
    return base


class Prog:
    def __init__(self, p, pub):
        self.p, self.n, self.pub = p, len(p), pub
        self.bad = []                    # (label, pc, cond)

    def addbad(self, label, pc, cond):
        if ZB == "mem" and label == "mem":
            return
        if ZB == "uninit" and label == "uninit":
            return
        self.bad.append((label, pc, cond))

    def addr(self, r, imm):
        return z3.ZeroExt(32, r) + z3.BitVecVal(imm, 64)

    def step(self, pc, s):
        """one instruction from state s; returns [(target, state)], target None = path ended"""
        op, d, sr, imm = self.p[pc]
        g = s.g
        if reads_d(op):
            self.addbad("uninit", pc, z3.And(g, z3.Not(s.ini[d])))
        if reads_s(op):
            self.addbad("uninit", pc, z3.And(g, z3.Not(s.ini[sr])))
        if op == EXIT:
            self.addbad("uninit", pc, z3.And(g, z3.Not(s.ini[0])))
            return []
        s = s.copy()
        R = s.r
        nxt = pc + 1
        if op == MOVI:
            R[d] = z3.BitVecVal(imm & 0xFFFFFFFF, 32)
        elif op == MOV:
            R[d] = R[sr]
        elif op == ADD:
            R[d] = R[d] + R[sr]
        elif op == SUB:
            R[d] = R[d] - R[sr]
        elif op == MUL:
            R[d] = R[d] * R[sr]
        elif op == AND:
            R[d] = R[d] & R[sr]
        elif op == OR:
            R[d] = R[d] | R[sr]
        elif op == XOR:
            R[d] = R[d] ^ R[sr]
        elif op == DIV:
            R[d] = z3.If(R[sr] == 0, z3.BitVecVal(0, 32), z3.UDiv(R[d], R[sr]))
        elif op == ADDI:
            R[d] = R[d] + z3.BitVecVal(imm & 0xFFFFFFFF, 32)
        elif op == ANDI:
            R[d] = R[d] & z3.BitVecVal(imm & 0xFFFFFFFF, 32)
        elif op == SHLI:
            R[d] = R[d] << imm
        elif op == SHRI:
            R[d] = z3.LShR(R[d], imm)
        elif op == LDB:
            a = self.addr(R[sr], imm)
            self.addbad("mem", pc, z3.And(g, z3.Or(a < 0, a >= MEM)))
            R[d] = z3.ZeroExt(24, z3.Select(s.mem, z3.Extract(5, 0, a)))
        elif op == STB:
            a = self.addr(R[d], imm)
            self.addbad("mem", pc, z3.And(g, z3.Or(a < 0, a >= MEM)))
            s.mem = z3.Store(s.mem, z3.Extract(5, 0, a), z3.Extract(7, 0, R[sr]))
        elif op == JA:
            return [(pc + 1 + imm, s)]
        elif op in (JEQI, JLTI, JLT):
            if op == JEQI:
                c = R[d] == z3.BitVecVal(sr, 32)
            elif op == JLTI:
                c = z3.ULT(R[d], z3.BitVecVal(sr, 32))
            else:
                c = z3.ULT(R[d], R[sr])
            tk = s.copy(z3.And(g, c))
            fl = s.copy(z3.And(g, z3.Not(c)))
            return [(pc + 1 + imm, tk), (pc + 1, fl)]
        elif op == CALL:
            self.addbad("uninit", pc, z3.And(g, z3.Not(s.ini[1])))
            if imm == CALL_PUB:
                self.addbad("uninit", pc, z3.And(g, z3.Not(s.ini[2])))
                self.addbad("mem", pc, z3.And(g, z3.UGT(z3.ZeroExt(32, R[1]) + z3.ZeroExt(32, R[2]),
                                                            z3.BitVecVal(MEM, 64))))
                R[0] = z3.Select(self.pub, s.npub)
                s.npub = s.npub + 1
            elif imm == CALL_LOG:
                R[0] = z3.BitVecVal(0, 32)
            else:
                raise ValueError("helper")
            for i in range(1, 6):
                s.ini[i] = z3.BoolVal(False)
            s.ini[0] = z3.BoolVal(True)
            return [(nxt, s)]
        elif op == LOOP:
            raise ValueError("loop in step")
        else:
            raise ValueError("op %d" % op)
        if writes_reg(op):
            s.ini[d] = z3.BoolVal(True)
        return [(nxt, s)]

    def seg(self, entry, lo, hi):
        """run pcs lo..hi-1 from entry at lo; returns the states arriving at hi"""
        inc = {lo: [entry]}
        out = []
        pc = lo
        while pc < hi:
            sts = inc.pop(pc, None)
            if not sts:
                pc += 1
                continue
            s = merge(sts)
            op, d, _, imm = self.p[pc]
            if op == LOOP:
                if lo != 0:
                    raise ValueError("nested loop")
                e = pc + 1 + d
                cur = s
                for _ in range(imm):
                    outs = self.seg(cur, pc + 1, e)
                    if not outs:
                        cur = None
                        break
                    cur = merge(outs)
                if cur is not None:
                    inc.setdefault(e, []).append(cur)
                pc = e
                continue
            for t, ns in self.step(pc, s):
                if t <= pc:
                    self.addbad("back", pc, ns.g)
                elif t < hi:
                    inc.setdefault(t, []).append(ns)
                elif t == hi and lo != 0:
                    out.append(ns)
                elif lo == 0:
                    self.addbad("pc", pc, ns.g)       # off the end
                else:
                    raise ValueError("jump out of a body")
            pc += 1
        return out


def unrolled(p):
    tot, i = 0, 0
    while i < len(p):
        if p[i][0] == LOOP:
            tot += 1 + p[i][1] * p[i][3]
            i += 1 + p[i][1]
        else:
            tot += 1
            i += 1
    return tot


def analyse(job):
    kind, it, code, fuzz, p, timeout, maxu = job
    if unrolled(p) > maxu:
        return (kind, it, code, fuzz, "skip", "-", None)
    mem = z3.Array("mem", z3.BitVecSort(6), z3.BitVecSort(8))
    pub = z3.Array("pub", z3.BitVecSort(32), z3.BitVecSort(32))
    g = Prog(p, pub)
    s0 = St(z3.BoolVal(True), [z3.BitVecVal(0, 32)] * 8, [z3.BoolVal(False)] * 8, mem, z3.BitVecVal(0, 32))
    try:
        g.seg(s0, 0, len(p))
    except ValueError as e:
        return (kind, it, code, fuzz, "skip", "shape:" + str(e), None)
    if not g.bad:
        return (kind, it, code, fuzz, "safe", "-", None)
    sol = z3.Solver()
    sol.set("timeout", timeout)
    sol.add(z3.Or([c for _, _, c in g.bad]))
    r = sol.check()
    if r == z3.unsat:
        return (kind, it, code, fuzz, "safe", "-", None)
    if r != z3.sat:
        return (kind, it, code, fuzz, "unknown", "-", None)
    m = sol.model()
    label = "?"
    for lb, pc, c in g.bad:
        if z3.is_true(m.eval(c, model_completion=True)):
            label = "%s@%d" % (lb, pc)
            break
    mb = [m.eval(z3.Select(mem, z3.BitVecVal(i, 6)), model_completion=True).as_long() for i in range(MEM)]
    pv = [m.eval(z3.Select(pub, z3.BitVecVal(i, 32)), model_completion=True).as_long() for i in range(64)]
    line = "%d %s %s 64 %s" % (len(p), " ".join("%d %d %d %d" % x for x in p),
                               " ".join(map(str, mb)), " ".join(map(str, pv)))
    return (kind, it, code, fuzz, "trap", label, line)


def main():
    global ZB
    ap = argparse.ArgumentParser()
    ap.add_argument("dump")
    ap.add_argument("out")
    ap.add_argument("--only", default=None)
    ap.add_argument("--first", type=int, default=0)
    ap.add_argument("--kind", default="A")
    ap.add_argument("--zb", default=None)
    ap.add_argument("--jobs", type=int, default=3)
    ap.add_argument("--timeout", type=int, default=20000)
    ap.add_argument("--maxunroll", type=int, default=4000)
    a = ap.parse_args()
    ZB = a.zb
    only = set(int(x) for x in a.only.split(",") if x) if a.only is not None else None
    jobs = []
    for ln in open(a.dump):
        f = ln.split()
        if f[0] != a.kind:
            continue
        it = int(f[1])
        if only is not None and it not in only:
            continue
        if f[0] == "A":
            code, fuzz, n, ins = int(f[2]), int(f[3]), int(f[4]), f[5:]
        else:
            code, fuzz, n, ins = int(f[2]), -1, int(f[3]), f[4:]
        p = [tuple(int(v) for v in x.split("/")) for x in ins]
        assert len(p) == n
        jobs.append((f[0], it, code, fuzz, p, a.timeout, a.maxunroll))
        if a.first and len(jobs) >= a.first:
            break
    with Pool(a.jobs, initializer=_init, initargs=(ZB,)) as pool, \
            open(a.out, "w") as fo, open(a.out + ".replay", "w") as fr:
        for res in pool.imap(analyse, jobs, chunksize=4):
            kind, it, code, fuzz, z, label, line = res
            fo.write("%s %d %d %d z3=%s %s\n" % (kind, it, code, fuzz, z, label))
            if line:
                fr.write(line + "\n")
    return 0


def _init(zb):
    global ZB
    ZB = zb


if __name__ == "__main__":
    sys.exit(main())
