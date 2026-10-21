#!/bin/bash
# ---------------------------------------------------------------------------
# run_canary_crowd.sh — [canary-crowd] (research, not wired into CI):
# stage 5 of docs/architecture/30-module/research/canary-exposure.md.
# 256 canary windows per round split as people x windows (4x64, 8x32, 32x8,
# 64x4, names from syllables): does the student read the name because there
# are many people, or because each person is seen often enough?
#
#   ./run_canary_crowd.sh [tier] [seed...]   exit 0 = the gate holds
#
#   gate: under -DCB_SHUFFLE the P1 fraction is <= 0.5 (4 people) and
#   <= 0.25 (8 or more) for every split and seed. The unshuffled P1 and the
#   UNSEEN lines are results, not gates.
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
SPLITS="4x64 8x32 32x8 64x4"

echo "[build] canary-crowd ($SPLITS, each + CB_SHUFFLE) ..."
PIDS=""
for sp in $SPLITS; do
    nb=${sp%x*}; nbr=${sp#*x}
    for v in main shuf; do
        extra=""; [ "$v" = shuf ] && extra="-DCB_SHUFFLE"
        # shellcheck disable=SC2086
        $CC $CFLAGS -DCB_GEN_NAMES -DCB_NB="$nb" -DCB_NBR="$nbr" $extra $SRC -lm -o "$WORK/cb_${sp}_$v" \
            || { echo "[build] FAILED $sp $v"; exit 1; }
        # shellcheck disable=SC2086
        "$WORK/cb_${sp}_$v" "$TIER" $SEEDS > "$OUT/$sp-$v.log" &
        PIDS="$PIDS $!"
    done
done
# shellcheck disable=SC2086
wait $PIDS

FAIL=0
for sp in $SPLITS; do
    nb=${sp%x*}
    grep 'SUMMARY' "$OUT/$sp-main.log" | sed "s/^\[bind\]/[crowd] $sp/"
    grep 'SUMMARY' "$OUT/$sp-shuf.log" | sed "s/^\[bind\]/[crowd] $sp (shuffle)/"
    lim=0.25; [ "$nb" -le 4 ] && lim=0.5
    n=$(grep -c '^\[bind\] SUMMARY' "$OUT/$sp-shuf.log")
    bad=$(grep '^\[bind\] SUMMARY' "$OUT/$sp-shuf.log" | awk -v lim="$lim" '{ split($5, a, "="); split(a[2], b, "/"); if (b[1] / b[2] > lim) n++ } END { print n + 0 }')
    if [ "$n" -gt 0 ] && [ "$bad" = 0 ]; then
        echo "[crowd] PASS negative control $sp: CB_SHUFFLE P1 fraction <= $lim ($n seeds)"
    else
        echo "[crowd] FAIL negative control $sp: CB_SHUFFLE P1 fraction > $lim on $bad of $n seeds"; FAIL=1
    fi
done
exit "$FAIL"
