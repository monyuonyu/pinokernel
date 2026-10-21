#!/bin/bash
# ---------------------------------------------------------------------------
# run_sxvm.sh — [sxvm] (research, not wired into CI): stage 1 of
# docs/architecture/30-module/research/safe-exec.md.
#
#   ./run_sxvm.sh [count] [seed...]   exit 0 = the head holds and every breaker is seen
#
#   head: sxvm_test (ASan+UBSan) exits 0 on every seed = no accepted program
#         traps, >= 10% accepted, the hand-written K cases hold.
#   breakers (the checker broken, the same programs): EDGE, BACKJ, WIDEN,
#         ADDOVF, PUBLEN must each report traps > 0 on every seed.
#   X (stage 2a): `sxvm_test exh $SX_EXH` (default 3) — head 0 traps, every
#         breaker traps > 0.
#   SX_ARMS="head EDGE BACKJ WIDEN ADDOVF PUBLEN" (default) picks the builds.
# ---------------------------------------------------------------------------
set -u
HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
CC="${CC:-cc}"
CFLAGS="-std=c11 -O1 -g -Wall -Wextra -Werror -fsanitize=address,undefined -fno-sanitize-recover=undefined"
COUNT="${1:-200000}"; shift || true
SEEDS="${*:-1 2 3}"
WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT
OUT="${SX_OUT:-$WORK}"
mkdir -p "$OUT"
ARMS="${SX_ARMS:-head EDGE BACKJ WIDEN ADDOVF PUBLEN}"
EXH="${SX_EXH:-3}"

FAIL=0
for a in $ARMS; do
    extra=""; [ "$a" != head ] && extra="-DSXVM_BREAK_$a"
    # shellcheck disable=SC2086
    $CC $CFLAGS $extra -I"$ROOT/arch/common/include" "$HERE/sxvm_test.c" "$ROOT/arch/common/sxvm.c" -o "$WORK/sx_$a" \
        || { echo "[sxvm] build FAILED $a"; exit 1; }
    for s in $SEEDS; do
        "$WORK/sx_$a" "$COUNT" "$s" > "$OUT/$a-s$s.log" 2>&1
        rc=$?
        sum=$(grep 'SUMMARY' "$OUT/$a-s$s.log")
        echo "[sxvm] $a rc=$rc ${sum#\[sxvm\] }"
        if grep -q 'ERROR: AddressSanitizer\|runtime error' "$OUT/$a-s$s.log"; then
            echo "[sxvm] FAIL $a seed $s: sanitizer report"; FAIL=1
        fi
        traps=$(echo "$sum" | sed -n 's/.* traps=\([0-9]*\).*/\1/p')
        if [ "$a" = head ]; then
            [ "$rc" = 0 ] || { echo "[sxvm] FAIL head seed $s"; FAIL=1; }
        else
            [ -n "$traps" ] && [ "$traps" -gt 0 ] || { echo "[sxvm] FAIL breaker $a seed $s: not seen (traps=${traps:-?})"; FAIL=1; }
        fi
    done
    "$WORK/sx_$a" exh "$EXH" > "$OUT/$a-exh$EXH.log" 2>&1
    rc=$?
    sum=$(grep '\] EXH ' "$OUT/$a-exh$EXH.log")
    echo "[sxvm] $a exh rc=$rc ${sum#\[sxvm\] }"
    if grep -q 'ERROR: AddressSanitizer\|runtime error' "$OUT/$a-exh$EXH.log"; then
        echo "[sxvm] FAIL $a exh: sanitizer report"; FAIL=1
    fi
    traps=$(echo "$sum" | sed -n 's/.* traps=\([0-9]*\).*/\1/p')
    if [ "$a" = head ]; then
        [ "$rc" = 0 ] || { echo "[sxvm] FAIL head exh"; FAIL=1; }
    else
        [ -n "$traps" ] && [ "$traps" -gt 0 ] || { echo "[sxvm] FAIL breaker $a exh: not seen"; FAIL=1; }
    fi
done
[ "$FAIL" = 0 ] && echo "[sxvm] ALL PASS (head clean, every breaker seen)"
exit "$FAIL"
