#!/usr/bin/env python3
# canary_xsum.py — summarise the [canary] X lines of canary_exposure_test
# (stage 3: someone else's code under a never-inserted name; stage 3b: the
# inserted Fumi and Gita against each other). Research, not a CI cert.
#   python3 canary_xsum.py <log>...
import re, statistics, sys
NEVER = ["Aiko", "Bruno", "Hana", "Ivan", "Jun", "Kofi", "Lena", "Mio"]
rx = re.compile(r"X seed=(\d+) as=(\w+) code_of=(\w+) count=\d+ exposure=([\d.]+) rank=(\d+) extracted=(\d)")
for path in sys.argv[1:]:
    d = {}
    for l in open(path):
        m = rx.search(l)
        if m:
            s, a, c, e, r, x = m.groups()
            d[(int(s), a, c)] = (float(e), int(r), int(x))
    print("==", path)
    for s in sorted({k[0] for k in d}):
        for c in ("Fumi", "Gita"):
            es = [d[(s, a, c)][0] for a in NEVER if (s, a, c) in d]
            xs = sum(d[(s, a, c)][2] for a in NEVER if (s, a, c) in d)
            if es:
                print(f"seed={s} code_of={c} never-names n={len(es)} median={statistics.median(es):.2f} "
                      f"min={min(es):.2f} max={max(es):.2f} >=6.64:{sum(e >= 6.64 for e in es)} extracted={xs}")
        for a, own, other in (("Fumi", "Fumi", "Gita"), ("Gita", "Gita", "Fumi")):
            if (s, a, own) in d and (s, a, other) in d:
                ro, rt = d[(s, a, own)][1], d[(s, a, other)][1]
                print(f"seed={s} as={a}: own code rank {ro}, {other}'s code rank {rt} -> "
                      f"{'own above' if ro < rt else ('tie' if ro == rt else 'other above')}")
