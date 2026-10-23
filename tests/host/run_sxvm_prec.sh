#!/bin/bash
# ---------------------------------------------------------------------------
# run_sxvm_prec.sh — [sxvm] stage 4c of docs/architecture/30-module/research/
# safe-exec.md (research, not wired into CI): the -DSXVM_PREC checker under
# Z3 (every scratch filling and pub answer), each "trap" replayed in C.
#
#   SX_PY=<python with z3> ./run_sxvm_prec.sh <outdir> <stage4dir> [seed...]
#   <stage4dir>: run_sxvm_z3.sh's output for the same SX_GEN (its z-headR
#   files give P3's set). prints P1-P3 and the PRECSAME line
#   (sxvm_prec_sum.py), exit 0 = all as fixed.
# ---------------------------------------------------------------------------
set -u
HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
OUT="$1"; S4="$2"; shift 2
SEEDS="${*:-1 2 3}"
PY="${SX_PY:-python3}"
FIRST="${SX_FIRST:-2000}"
mkdir -p "$OUT/bin"
build() {   # <arm> <flags...>
    local a="$1"; shift
    cc -std=c11 -O2 -Wall -Wextra -Werror "$@" -I"$ROOT/arch/common/include" \
        "$HERE/sxvm_test.c" "$ROOT/arch/common/sxvm.c" -o "$OUT/bin/sx_$a" || { echo "[sxvm-prec] build FAILED $a"; exit 1; }
}
build head
build PREC -DSXVM_PREC
build PRECSAME -DSXVM_PREC -DSXVM_BREAK_PRECSAME
z3run() {   # <arm> <dump> <out> <args...> ; then replay with that arm's build
    local arm="$1" dump="$2" o="$3"; shift 3
    "$PY" -I "$HERE/sxvm_z3.py" "$dump" "$o" "$@" || { echo "[sxvm-prec] z3 FAILED $o"; exit 1; }
    "$OUT/bin/sx_$arm" replay < "$o.replay" > "$o.rep" || { echo "[sxvm-prec] replay FAILED $o"; exit 1; }
}
extra() {   # accepted in $2 but not in $1
    awk 'NR==FNR { if ($1=="A") h[$2]=1; next } $1=="A" && !($2 in h) { print $2 }' "$1" "$2" | paste -sd,
}
for s in $SEEDS; do
    for a in head PREC PRECSAME; do "$OUT/bin/sx_$a" dump 200000 "$s" 10 "${SX_GEN:-1}" > "$OUT/d-$a-s$s.txt"; done
    z3run PREC "$OUT/d-PREC-s$s.txt" "$OUT/z-PRECA-s$s" --first "$FIRST"
    extra "$OUT/d-head-s$s.txt" "$OUT/d-PREC-s$s.txt" > "$OUT/extra-PREC-s$s.txt"
    extra "$OUT/d-PREC-s$s.txt" "$OUT/d-PRECSAME-s$s.txt" > "$OUT/extra-PRECSAME-s$s.txt"
    for a in PREC PRECSAME; do
        if [ -s "$OUT/extra-$a-s$s.txt" ] && [ "$(cat "$OUT/extra-$a-s$s.txt")" != "" ]; then
            z3run "$a" "$OUT/d-$a-s$s.txt" "$OUT/z-$a-s$s" --only "$(cat "$OUT/extra-$a-s$s.txt")"
        else : > "$OUT/z-$a-s$s"; : > "$OUT/z-$a-s$s.rep"; fi
    done
    echo "[sxvm-prec] seed $s done $(date +%T)"
done
"$PY" -I "$HERE/sxvm_prec_sum.py" "$OUT" "$S4" $SEEDS
