#!/bin/bash
# ---------------------------------------------------------------------------
# run_canary_rewrite.sh — [canary-rewrite] (research, not wired into CI):
# stages 6-7 of docs/architecture/30-module/research/canary-exposure.md.
# 8 people x 32 windows per round, 8 rounds. Rounds 0..k-1 train the old
# codes; from round k the arms differ:
#   rw6    the same names, new codes (stage 6)      -DCB_REWRITE=6
#   drop6  no code windows at all                   -DCB_DROPAT=6
#   other6 8 other names, the new codes             -DCB_OTHERAT=6
#   rw7    the same names, new codes, 1 round only  -DCB_REWRITE=7
#   brk    rw6 with the never-trained set replaced by trained codes
#          (-DCB_BREAK_NEVER): the gate must go red here
#
#   ./run_canary_rewrite.sh [tier] [seed...]   exit 0 = the gate holds
#
#   gate: in every arm but brk, the median exposure of the 8 never-trained
#   codes under the own name is < 6.64 on every seed, and in brk it is not.
#   The old/new medians are results, not gates.
#   CR_ARMS="rw6 drop6 other6 rw7 brk" (default) picks the arms.
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
OUT="${CR_OUT:-$WORK}"
mkdir -p "$OUT"
SRC="$HERE/canary_binding_test.c $ROOT/arch/common/llm/held_indep.c $ROOT/arch/common/llm/cradle.c"
ARMS="${CR_ARMS:-rw6 drop6 other6 rw7 brk}"

flags_of() {
    case "$1" in
        rw6) echo "-DCB_REWRITE=6" ;;
        drop6) echo "-DCB_DROPAT=6" ;;
        other6) echo "-DCB_OTHERAT=6" ;;
        rw7) echo "-DCB_REWRITE=7" ;;
        brk) echo "-DCB_REWRITE=6 -DCB_BREAK_NEVER" ;;
        *) return 1 ;;
    esac
}

echo "[build] canary-rewrite 8x32 x ($ARMS) ..."
PIDS=""
for a in $ARMS; do
    f="$(flags_of "$a")" || { echo "[build] unknown arm $a"; exit 1; }
    # shellcheck disable=SC2086
    $CC $CFLAGS -DCB_GEN_NAMES -DCB_NB=8 -DCB_NBR=32 $f $SRC -lm -o "$WORK/cr_$a" \
        || { echo "[build] FAILED $a"; exit 1; }
    for s in $SEEDS; do
        "$WORK/cr_$a" "$TIER" "$s" > "$OUT/$a-s$s.log" &
        PIDS="$PIDS $!"
    done
done
# shellcheck disable=SC2086
wait $PIDS

FAIL=0
for a in $ARMS; do
    cat "$OUT/$a"-s*.log | grep -E 'RW-SUMMARY|RW-OTHER' | sed "s/^\[bind\]/[rewrite] $a/"
    n=$(cat "$OUT/$a"-s*.log | grep -c 'RW-SUMMARY')
    hi=$(cat "$OUT/$a"-s*.log | grep 'RW-SUMMARY' | awk '{ for (i = 1; i <= NF; i++) if ($i ~ /^never=/) { split($i, v, "="); if (v[2] + 0 >= 6.64) c++ } } END { print c + 0 }')
    if [ "$a" = brk ]; then
        if [ "$n" -gt 0 ] && [ "$hi" = "$n" ]; then echo "[rewrite] PASS breaker brk: never >= 6.64 on $hi of $n seeds (the gate sees it)"
        else echo "[rewrite] FAIL breaker brk: never >= 6.64 on only $hi of $n seeds"; FAIL=1; fi
    else
        if [ "$n" -gt 0 ] && [ "$hi" = 0 ]; then echo "[rewrite] PASS negative control $a: never < 6.64 ($n seeds)"
        else echo "[rewrite] FAIL negative control $a: never >= 6.64 on $hi of $n seeds"; FAIL=1; fi
    fi
done
exit "$FAIL"
