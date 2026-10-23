#!/usr/bin/env python3
"""sxvm_prec_sum.py <outdir> <stage4dir> <seed...> — the P1-P3 and PRECSAME
lines of stage 4c (safe-exec.md) from run_sxvm_prec.sh's files.
exit 0 = every gate as fixed (P3 is summed over the seeds given here; the
memo sums it over both generators by hand)."""
import os
import sys
from collections import Counter


def load(path):
    """[(kind, it, code, fuzz, z3, label, replay_trap or None)], over count"""
    rows = [ln.split() for ln in open(path)]
    rep = [ln.split() for ln in open(path + ".rep")]
    over = int(rep[-1][1]) if rep and rep[-1][0] == "over" else 0
    rep = [r for r in rep if r[0] != "over"]
    out, k = [], 0
    for r in rows:
        z = r[4][3:]
        t = None
        if z == "trap":
            t = int(rep[k][0])
            k += 1
        out.append((r[0], int(r[1]), int(r[2]), int(r[3]), z, r[5], t))
    assert k == len(rep), path
    return out, over


def dump(path):
    """it -> (kind, fuzz trap or None)"""
    out = {}
    for ln in open(path):
        f = ln.split()
        out[int(f[1])] = (f[0], int(f[3]) if f[0] == "A" else None)
    return out


DIV, LOOP, EXIT = 9, 20, 21


def refused(path):
    """it -> [(op, d, s, imm)] for the refused programs (sxvm_z5_kinds.py)"""
    out = {}
    for ln in open(path):
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
    d, s4, seeds = sys.argv[1], sys.argv[2], sys.argv[3:]
    bad = 0

    def gate(name, ok, text):
        nonlocal bad
        print("[sxvm-prec] %-12s %s %s" % (name, "PASS" if ok else "FAIL", text))
        if not ok:
            bad += 1

    p3_all, p3_acc, p3_exitl, p3_exitl_acc = 0, 0, 0, 0
    p3_missing = []
    for s in seeds:
        dp = dump("%s/d-PREC-s%s.txt" % (d, s))
        fz_all = sum(1 for v in dp.values() if v[0] == "A" and v[1] != 0)
        nacc = sum(1 for v in dp.values() if v[0] == "A")
        conf, info, overs = 0, [], 0
        for f in ("PRECA", "PREC"):
            rows, ov = load("%s/z-%s-s%s" % (d, f, s))
            overs += ov
            c = Counter(r[4] for r in rows)
            n = sum(1 for r in rows if r[4] == "trap" and r[6])
            conf += n
            info.append("%s=%d safe=%d trap=%d (replayed %d) unknown=%d skip=%d" %
                        (f, len(rows), c["safe"], c["trap"], n, c["unknown"], c["skip"]))
        gate("P1 s" + s, conf == 0 and fz_all == 0 and overs == 0,
             "accepted=%d fuzz_traps=%d %s host_over=%d" % (nacc, fz_all, " ".join(info), overs))
        ex = [x for x in open("%s/extra-PREC-s%s.txt" % (d, s)).read().strip().split(",") if x]
        gate("P2 s" + s, len(ex) > 0, "extra=%d" % len(ex))
        # P3: the stage 4 head refusals Z3 called safe
        if not os.path.exists("%s/z-headR-s%s.rep" % (s4, s)):
            print("[sxvm-prec] P3 s%s        (stage 4 z-headR not there yet: not counted)" % s)
            rows, hp = [], {}
            p3_missing.append(s)
        else:
            rows, _ = load("%s/z-headR-s%s" % (s4, s))
            hp = refused("%s/d-head-s%s.txt" % (s4, s))
        for r in rows:
            if r[4] != "safe":
                continue
            p3_all += 1
            p3_acc += dp[r[1]][0] == "A"
            if kinds(hp[r[1]]) == {"exitL"}:
                p3_exitl += 1
                p3_exitl_acc += dp[r[1]][0] == "A"
        # PRECSAME
        rows, _ = load("%s/z-PRECSAME-s%s" % (d, s))
        fz = sum(1 for r in rows if r[3] != 0)
        zc = sum(1 for r in rows if r[4] == "trap" and r[6])
        both = sum(1 for r in rows if r[3] != 0 and r[4] == "trap" and r[6])
        gate("PRECSAME s" + s, fz > 0 and both > 0,
             "extra=%d fuzz=%d z3_replayed=%d both=%d" % (len(rows), fz, zc, both))
    print("[sxvm-prec] P3 refused-but-safe(stage4)=%d now accepted=%d (%.0f%%; fixed: >= 40%% over both gens, by hand)" %
          (p3_all, p3_acc, 100.0 * p3_acc / max(1, p3_all)))
    gate("P3 exitL", p3_exitl_acc == 0, "exitL-only=%d accepted=%d (must be 0)" % (p3_exitl, p3_exitl_acc))
    if p3_missing:
        print("[sxvm-prec] P3 incomplete: stage 4 missing for seeds %s" % " ".join(p3_missing))
        bad += 1
    print("[sxvm-prec] %s" % ("ALL PASS" if bad == 0 else "FAIL %d" % bad))
    return 0 if bad == 0 else 1


if __name__ == "__main__":
    sys.exit(main())
