#!/bin/bash
# ---------------------------------------------------------------------------
# run_sxvm_useful.sh — [sxvm-useful] stage 5 of docs/architecture/30-module/
# research/safe-exec-log.md (research, not wired into CI): five useful
# handlers, plain vs masked index (one instruction apart).
#
#   ./run_sxvm_useful.sh [count]   exit 0 = F1-F3 and the breaker as fixed
#   F1 head: masked accepted, agree on every input.  F2 head and PREC: plain
#   refused with E_MEM (-5).  F3 plain run unchecked: H1 H2 H3 H5 no trap,
#   H4 traps.  Breaker WIDEN: plain H1 H2 H3 H5 accepted (H4 was fixed as
#   accepted too; it stays refused by the pub length, see the memo).
# ---------------------------------------------------------------------------
set -u
HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
N="${1:-1000}"
WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT
for a in head PREC WIDEN; do
    f=""; [ "$a" = PREC ] && f=-DSXVM_PREC; [ "$a" = WIDEN ] && f=-DSXVM_BREAK_WIDEN
    # shellcheck disable=SC2086
    cc -std=c11 -O1 -g -Wall -Wextra -Werror -fsanitize=address,undefined -fno-sanitize-recover=undefined $f \
        -I"$ROOT/arch/common/include" "$HERE/sxvm_test.c" "$ROOT/arch/common/sxvm.c" -o "$WORK/sx_$a" \
        || { echo "[sxvm-useful] build FAILED $a"; exit 1; }
    "$WORK/sx_$a" useful "$N" > "$WORK/$a.log" 2>&1 || { echo "[sxvm-useful] run FAILED $a"; cat "$WORK/$a.log"; exit 1; }
    sed "s/^\[sxvm\]/[sxvm-useful] $a/" "$WORK/$a.log"
done
FAIL=0
v() { # <arm> <H> <form> -> "check run trap agree"
    awk -v h="H$2" -v f="$3" '$3 == h && $4 == f { for (i = 5; i <= 8; i++) { split($i, x, "="); printf "%s ", x[2] } }' "$WORK/$1.log"
}
say() { if [ "$1" = 1 ]; then echo "[sxvm-useful] $2 PASS"; else echo "[sxvm-useful] $2 FAIL"; FAIL=1; fi; }
for h in 1 2 3 4 5; do
    read -r c r t g <<< "$(v head $h masked)"
    say "$([ "$c" -ge 0 ] && [ "$t" = 0 ] && [ "$g" = "$N" ] && echo 1)" "F1 H$h (check=$c agree=$g/$N)"
    for a in head PREC; do read -r c r t g <<< "$(v $a $h plain)"; say "$([ "$c" = -5 ] && echo 1)" "F2 H$h $a (check=$c)"; done
    read -r c r t g <<< "$(v head $h plain)"
    if [ "$h" = 4 ]; then say "$([ "$t" -gt 0 ] && echo 1)" "F3 H$h traps unchecked ($t/$r)"
    else say "$([ "$t" = 0 ] && echo 1)" "F3 H$h no trap unchecked ($t/$r)"; fi
    read -r c r t g <<< "$(v WIDEN $h plain)"
    if [ "$h" = 4 ]; then echo "[sxvm-useful] WIDEN H4 check=$c (fixed: accepted; refused by the pub length = miss, not a gate)"
    else say "$([ "$c" -ge 0 ] && echo 1)" "WIDEN H$h plain accepted (check=$c)"; fi
done
exit "$FAIL"
