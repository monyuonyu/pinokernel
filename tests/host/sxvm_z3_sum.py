#!/usr/bin/env python3
"""sxvm_z3_sum.py <outdir> <seed...> — the Z1-Z5 and ZB lines of stage 4
(safe-exec.md) from run_sxvm_z3.sh's files. exit 0 = every gate as fixed."""
import sys
from collections import Counter

BREAK = ["EDGE", "WIDEN", "ADDOVF", "PUBLEN", "CLOBBER"]


def load(path):
    """[(kind, it, code, fuzz, z3, label, replay_trap or None)], over count"""
    rows = [ln.split() for ln in open(path)]
    rep = [ln.split() for ln in open(path + ".rep")]
    over = int(rep[-1][1]) if rep and rep[-1][0] == "over" else -1
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


def main():
    d, seeds = sys.argv[1], sys.argv[2:]
    bad = 0

    def gate(name, ok, text):
        nonlocal bad
        print("[sxvm-z3] %-10s %s %s" % (name, "PASS" if ok else "FAIL", text))
        if not ok:
            bad += 1

    allz, allrep_ok, overs = 0, 0, 0
    for s in seeds:
        rows, ov = load("%s/z-headA-s%s" % (d, s))
        overs += ov
        c = Counter(r[4] for r in rows)
        conf = sum(1 for r in rows if r[4] == "trap" and r[6] != 0)
        gate("Z1 s" + s, conf == 0 and c["safe"] >= 0.9 * len(rows),
             "programs=%d safe=%d trap=%d (replayed %d) unknown=%d skip=%d" %
             (len(rows), c["safe"], c["trap"], conf, c["unknown"], c["skip"]))
        for f in ["headA", "headR"] + BREAK + ["BACKJ"]:
            rows, ov = load("%s/z-%s-s%s" % (d, f, s))
            overs += ov
            for r in rows:
                if r[4] == "trap":
                    allz += 1
                    allrep_ok += r[6] != 0
        rows, _ = load("%s/z-headR-s%s" % (d, s))
        c = Counter(r[4] for r in rows)
        bycode = Counter((r[2], r[4]) for r in rows)
        frac = c["safe"] / max(1, len(rows))
        print("[sxvm-z3] Z5 s%s refused=%d safe=%d (%.1f%%) trap=%d (replayed %d) unknown=%d skip=%d by-code %s prediction 5-40%%: %s" %
              (s, len(rows), c["safe"], 100 * frac, c["trap"], sum(1 for r in rows if r[6]),
               c["unknown"], c["skip"], dict(sorted(bycode.items())), "held" if 0.05 <= frac <= 0.40 else "MISSED"))
        for b in BREAK:
            rows, _ = load("%s/z-%s-s%s" % (d, b, s))
            fz = sum(1 for r in rows if r[3] != 0)
            miss = sum(1 for r in rows if r[3] != 0 and not (r[4] == "trap" and r[6]))
            zc = sum(1 for r in rows if r[4] == "trap" and r[6])
            c = Counter(r[4] for r in rows)
            gate("Z3 %s s%s" % (b, s), miss == 0, "extra=%d fuzz=%d z3_missed=%d" % (len(rows), fz, miss))
            strict = b in ("EDGE", "ADDOVF")
            gate("Z4 %s s%s" % (b, s), zc > fz if strict else zc >= fz,
                 "z3_replayed=%d fuzz=%d (%s) unknown=%d skip=%d" % (zc, fz, ">" if strict else ">=", c["unknown"], c["skip"]))
        rows, _ = load("%s/z-BACKJ-s%s" % (d, s))
        c = Counter(r[4] for r in rows)
        lab = Counter(r[5].split("@")[0] for r in rows if r[4] == "trap")
        print("[sxvm-z3] BACKJ s%s extra(first)=%d fuzz=%d z3_trap=%d replayed=%d labels %s unknown=%d skip=%d" %
              (s, len(rows), sum(1 for r in rows if r[3] != 0), c["trap"],
               sum(1 for r in rows if r[4] == "trap" and r[6]), dict(lab), c["unknown"], c["skip"]))
        for f, b in (("EDGE-zbmem", "EDGE"), ("CLOBBER-zbuninit", "CLOBBER")):
            rows, _ = load("%s/z-%s-s%s" % (d, f, s))
            miss = sum(1 for r in rows if r[3] != 0 and not (r[4] == "trap" and r[6]))
            gate("ZB %s s%s" % (f, s), miss > 0, "fuzz=%d z3_missed=%d (must be > 0)" %
                 (sum(1 for r in rows if r[3] != 0), miss))
    gate("Z2", allz > 0 and allrep_ok == allz and overs == 0,
         "z3 traps=%d replayed in C=%d host_over=%d" % (allz, allrep_ok, overs))
    print("[sxvm-z3] %s" % ("ALL PASS" if bad == 0 else "FAIL %d" % bad))
    return 0 if bad == 0 else 1


if __name__ == "__main__":
    sys.exit(main())
