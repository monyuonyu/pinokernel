#!/bin/bash
# ---------------------------------------------------------------------------
# run_canary_exposure.sh — [canary-exposure] (research, not wired into CI):
# how much of a trained-on record the student's weights give back verbatim.
# Design and the expectations fixed before the first run:
# docs/architecture/30-module/research/canary-exposure.md
#
#   ./run_canary_exposure.sh [tier] [seed...]   exit 0 = ALL PASS
#
#   1. the measurement itself (T1..T3 gates, E1..E3 results)
#   2. -DCE_OFFBY1 (the tree reads the logit row one position early) must go
#      RED on T1, or T1 cannot see a broken tree
#   3. -DCE_NO_INSERT (no canary is ever trained) must leave the 16x canary
#      below 6.64 on every seed, or the pipeline makes exposure by itself
# ---------------------------------------------------------------------------
set -u
HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
CC="${CC:-cc}"
CFLAGS="-std=c11 -O1 -Wall -Wextra -ffp-contract=off -Werror=vla"
TIER="${1:-0}"; shift || true
SEEDS="${*:-1 2 3}"
WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT
OUT="${CE_OUT:-$WORK}"
mkdir -p "$OUT"
FAIL=0
SRC="$HERE/canary_exposure_test.c $ROOT/arch/common/llm/held_indep.c $ROOT/arch/common/llm/cradle.c"   # the test includes student.c (kv_step)

echo "[build] canary-exposure (+ CE_OFFBY1, CE_NO_INSERT) ..."
# shellcheck disable=SC2086
$CC $CFLAGS $SRC -lm -o "$WORK/ce" || { echo "[build] FAILED"; exit 1; }
# shellcheck disable=SC2086
$CC $CFLAGS -DCE_OFFBY1 $SRC -lm -o "$WORK/ce_off" || { echo "[build] FAILED"; exit 1; }
# shellcheck disable=SC2086
$CC $CFLAGS -DCE_NO_INSERT $SRC -lm -o "$WORK/ce_none" || { echo "[build] FAILED"; exit 1; }

# shellcheck disable=SC2086
"$WORK/ce" "$TIER" $SEEDS > "$OUT/main.log" &
P1=$!
# shellcheck disable=SC2086
"$WORK/ce_none" "$TIER" $SEEDS > "$OUT/none.log" &
P2=$!
# the off-by-one build only needs T1 (it stops being meaningful after it)
"$WORK/ce_off" "$TIER" 1 > "$OUT/off.log" &
P3=$!
wait "$P1" "$P2" "$P3"

cat "$OUT/main.log"
grep -q '^\[canary\] ALL PASS' "$OUT/main.log" || FAIL=1

if grep -q '^\[canary\] FAIL T1' "$OUT/off.log"; then
    echo "[canary] PASS falsifier: CE_OFFBY1 turns T1 red"
else
    echo "[canary] FAIL falsifier: CE_OFFBY1 did not turn T1 red"; FAIL=1
fi

grep 'count=16' "$OUT/none.log"
N16=$(grep -c 'E1 .*count=16' "$OUT/none.log")
BAD=$(grep 'E1 .*count=16' "$OUT/none.log" | awk '{ split($6, a, "="); if (a[2] + 0 >= 6.64) n++ } END { print n + 0 }')
if [ "$N16" -gt 0 ] && [ "$BAD" = 0 ]; then
    echo "[canary] PASS negative control: CE_NO_INSERT leaves the 16x canary below 6.64 ($N16 seeds)"
else
    echo "[canary] FAIL negative control: CE_NO_INSERT 16x canary >= 6.64 on $BAD of $N16 seeds"; FAIL=1
fi

if [ "$FAIL" = 0 ]; then echo "[canary+nc] ALL PASS"; else echo "[canary+nc] RED"; fi
exit "$FAIL"
