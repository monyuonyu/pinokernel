#!/bin/bash
# ---------------------------------------------------------------------------
# run_sxvm_z3.sh — [sxvm] stage 4 of docs/architecture/30-module/research/
# safe-exec.md (research, not wired into CI): Z3 over every scratch filling
# and pub answer, each "trap" replayed in the C interpreter.
#
#   SX_PY=<python with z3> ./run_sxvm_z3.sh <outdir> [seed...]
#   prints the Z1-Z5 and ZB lines (sxvm_z3_sum.py), exit 0 = all as fixed.
#   SX_GEN=2: stage 4b's input-driven generator (default 1 = stage 1's).
#   SX_FIRST (2000) programs per seed for the head; every breaker's extra
#   accepted programs in full (BACKJ: the first SX_BACKJ_FIRST, 1000).
# ---------------------------------------------------------------------------
set -u
HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
OUT="$1"; shift
SEEDS="${*:-1 2 3}"
PY="${SX_PY:-python3}"
FIRST="${SX_FIRST:-2000}"
BFIRST="${SX_BACKJ_FIRST:-1000}"
ARMS="head EDGE BACKJ WIDEN ADDOVF PUBLEN CLOBBER"
mkdir -p "$OUT/bin"
for a in $ARMS; do
    extra=""; [ "$a" != head ] && extra="-DSXVM_BREAK_$a"
    # shellcheck disable=SC2086
    cc -std=c11 -O2 -Wall -Wextra -Werror $extra -I"$ROOT/arch/common/include" \
        "$HERE/sxvm_test.c" "$ROOT/arch/common/sxvm.c" -o "$OUT/bin/sx_$a" || { echo "[sxvm-z3] build FAILED $a"; exit 1; }
done
z3run() {   # <arm> <dump> <out> <args...> ; then replay with that arm's build
    local arm="$1" dump="$2" o="$3"; shift 3
    "$PY" -I "$HERE/sxvm_z3.py" "$dump" "$o" "$@" || { echo "[sxvm-z3] z3 FAILED $o"; exit 1; }
    "$OUT/bin/sx_$arm" replay < "$o.replay" > "$o.rep" || { echo "[sxvm-z3] replay FAILED $o"; exit 1; }
}
for s in $SEEDS; do
    for a in $ARMS; do "$OUT/bin/sx_$a" dump 200000 "$s" 10 "${SX_GEN:-1}" > "$OUT/d-$a-s$s.txt"; done
    z3run head "$OUT/d-head-s$s.txt" "$OUT/z-headA-s$s" --first "$FIRST"
    z3run head "$OUT/d-head-s$s.txt" "$OUT/z-headR-s$s" --kind R --first "$FIRST"
    for a in EDGE BACKJ WIDEN ADDOVF PUBLEN CLOBBER; do
        only=$(awk 'NR==FNR { if ($1=="A") h[$2]=1; next } $1=="A" && !($2 in h) { print $2 }' \
               "$OUT/d-head-s$s.txt" "$OUT/d-$a-s$s.txt" | { [ "$a" = BACKJ ] && head -n "$BFIRST" || cat; } | paste -sd,)
        echo "$only" > "$OUT/extra-$a-s$s.txt"
        z3run "$a" "$OUT/d-$a-s$s.txt" "$OUT/z-$a-s$s" --only "$only"
    done
    z3run EDGE "$OUT/d-EDGE-s$s.txt" "$OUT/z-EDGE-zbmem-s$s" --only "$(cat "$OUT/extra-EDGE-s$s.txt")" --zb mem
    z3run CLOBBER "$OUT/d-CLOBBER-s$s.txt" "$OUT/z-CLOBBER-zbuninit-s$s" --only "$(cat "$OUT/extra-CLOBBER-s$s.txt")" --zb uninit
    echo "[sxvm-z3] seed $s done $(date +%T)"
done
"$PY" -I "$HERE/sxvm_z3_sum.py" "$OUT" $SEEDS
