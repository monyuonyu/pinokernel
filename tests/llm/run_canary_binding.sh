#!/bin/bash
# ---------------------------------------------------------------------------
# run_canary_binding.sh — [canary-binding] (research, not wired into CI):
# stage 4 of docs/architecture/30-module/research/canary-exposure.md.
# 32 people with their own codes: does the student rank a person's own code
# above the other 31 under that person's name (P), or only the template?
#
#   ./run_canary_binding.sh [tier] [seed...]   exit 0 = the gate holds
#
#   gate: -DCB_SHUFFLE (the person -> code pairing re-drawn every round) must
#   give median P in [12, 21] on every seed, or P measures something other
#   than the binding. The unshuffled run's P is the result, not a gate.
# ---------------------------------------------------------------------------
set -u
HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
CC="${CC:-cc}"
CFLAGS="-std=c11 -O1 -Wall -Wextra -Wno-unused-function -ffp-contract=off -Werror=vla"
TIER="${1:-0}"; shift || true
SEEDS="${*:-1 2 3}"
WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT
OUT="${CB_OUT:-$WORK}"
mkdir -p "$OUT"
SRC="$HERE/canary_binding_test.c $ROOT/arch/common/llm/held_indep.c $ROOT/arch/common/llm/cradle.c"

echo "[build] canary-binding (+ CB_SHUFFLE) ..."
# shellcheck disable=SC2086
$CC $CFLAGS $SRC -lm -o "$WORK/cb" || { echo "[build] FAILED"; exit 1; }
# shellcheck disable=SC2086
$CC $CFLAGS -DCB_SHUFFLE $SRC -lm -o "$WORK/cb_shuf" || { echo "[build] FAILED"; exit 1; }

# shellcheck disable=SC2086
"$WORK/cb" "$TIER" $SEEDS > "$OUT/main.log" &
P1=$!
# shellcheck disable=SC2086
"$WORK/cb_shuf" "$TIER" $SEEDS > "$OUT/shuf.log" &
P2=$!
wait "$P1" "$P2"

grep SUMMARY "$OUT/main.log"
grep SUMMARY "$OUT/shuf.log" | sed 's/^\[bind\]/[bind] (shuffle)/'
N=$(grep -c SUMMARY "$OUT/shuf.log")
BAD=$(grep SUMMARY "$OUT/shuf.log" | awk '{ split($4, a, "="); v = a[2] + 0; if (v < 12 || v > 21) n++ } END { print n + 0 }')
if [ "$N" -gt 0 ] && [ "$BAD" = 0 ]; then
    echo "[bind] PASS negative control: CB_SHUFFLE median P in [12, 21] ($N seeds)"; exit 0
fi
echo "[bind] FAIL negative control: CB_SHUFFLE median P outside [12, 21] on $BAD of $N seeds"
exit 1
