#!/usr/bin/env python3
"""sxvm_z5_kinds.py <outdir> <seed...> — stage 4 (safe-exec.md), after the
Z5 numbers were seen: a rough shape count of the programs the head checker
refused for a value reason but Z3 called safe for every input.
  div   : has DIV (the checker's DIV range is coarse, e.g. r1/r1)
  exitL : a LOOP whose body holds EXIT (the 2nd round never comes)
  other : neither
A program can be both div and exitL; it is counted under each."""
import sys
from collections import Counter

DIV, LOOP, EXIT = 9, 20, 21


def prog(dump):
    out = {}
    for ln in open(dump):
        f = ln.split()
        if f[0] == "R":
            out[int(f[1])] = [tuple(map(int, x.split("/"))) for x in f[4:]]
    return out


def kinds(p):
    k = set()
    if any(i[0] == DIV for i in p):
        k.add("div")
    for pc, i in enumerate(p):
        if i[0] == LOOP and any(j[0] == EXIT for j in p[pc + 1:pc + 1 + i[1]]):
            k.add("exitL")
    return k or {"other"}


def main():
    d, seeds = sys.argv[1], sys.argv[2:]
    for s in seeds:
        ps = prog("%s/d-head-s%s.txt" % (d, s))
        c, n = Counter(), 0
        for ln in open("%s/z-headR-s%s" % (d, s)):
            f = ln.split()
            if f[4] == "z3=safe":
                n += 1
                c.update(kinds(ps[int(f[1])]))
        print("[sxvm-z5k] s%s refused-but-safe=%d %s" % (s, n, dict(sorted(c.items()))))


main()
