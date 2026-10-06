#!/bin/bash
# ---------------------------------------------------------------------------
# run_st_distill.sh — host cert for the smarter merge, stage 3: merge by
#   distillation (st_merge_distill / st_backward_soft in
#   arch/common/llm/student.c; docs/architecture/30-module/research/
#   brain-merge-distill.md §3-§4).
#
# Builds student_distill_merge_test.c once plain and once per negative
# control, and judges LINES, not exit codes (audit-25: while plain has a
# known red, every build exits 1, so "NC exits 1" checked nothing):
#   plain  every FAIL line must be a known red (KNOWN_RED below)
#   NC     every line carrying TARGET must be FAIL in the NC build, and
#          PASS in the plain build (else the NC cannot be told apart)
#
#   ./run_st_distill.sh
#   ST_DISTILL_NCS="NOOP" ./run_st_distill.sh   # a no-op "NC": must FAIL
# ---------------------------------------------------------------------------
set -u
HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
CC="${CC:-cc}"
CFLAGS="-std=c11 -O1 -Wall -Wextra -ffp-contract=off -Werror=vla"
WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT

SRC_TEST="$HERE/student_distill_merge_test.c"
SRC_STU="$ROOT/arch/common/llm/student.c"

NCS="${ST_DISTILL_NCS:-ONE_TEACHER RAND_XFER}"
TARGET="${ST_DISTILL_TARGET:-[d5c-s2-both]}"
# stage 3 (start = the guarded pick): S1 stays red, brain-merge-distill.md §3
KNOWN_RED="[d5c-s1-gain]"

rc_all=0
run_one() {   # $1 = name, $2 = -D flag or empty; log in $WORK/$1.log
    echo "[build] $1 $2"
    if ! $CC $CFLAGS $2 "$SRC_TEST" "$SRC_STU" -lm -o "$WORK/d_$1"; then
        echo "[build] $1 FAILED"; return 1
    fi
    "$WORK/d_$1" > "$WORK/$1.log" 2>&1
    echo "  exit $?  $(grep -c '^  PASS' "$WORK/$1.log") PASS / $(grep -c '^  FAIL' "$WORK/$1.log") FAIL"
    return 0
}

if ! run_one plain ""; then rc_all=1; fi
cat "$WORK/plain.log"
nf=0; nk=0
while IFS= read -r line; do
    if printf '%s\n' "$line" | grep -qF "$KNOWN_RED"; then
        echo "[known red] $line"; nk=$((nk + 1))
    else
        echo "[result] plain UNEXPECTED FAIL: $line"; nf=$((nf + 1))
    fi
done < <(grep '^  FAIL' "$WORK/plain.log" 2>/dev/null)
if [ "$nf" -eq 0 ] && grep -q '^  PASS' "$WORK/plain.log" 2>/dev/null; then
    echo "[result] plain GREEN except $nk known red line(s)"
else
    echo "[result] plain RED ($nf unexpected FAIL)"; rc_all=1
fi
plain_t=$(grep -cF "$TARGET" "$WORK/plain.log" 2>/dev/null)
plain_tf=$(grep -F "$TARGET" "$WORK/plain.log" 2>/dev/null | grep -c '^  FAIL')
echo ""

for v in $NCS; do
    if ! run_one "$v" "-DST_DISTILL_$v"; then rc_all=1; continue; fi
    grep -F "$TARGET" "$WORK/$v.log"
    t=$(grep -cF "$TARGET" "$WORK/$v.log")
    tf=$(grep -F "$TARGET" "$WORK/$v.log" | grep -c '^  FAIL')
    if [ "$plain_t" -eq 0 ] || [ "$plain_tf" -ne 0 ]; then
        echo "[result] $v EMPTY: target '$TARGET' is not green in plain ($plain_tf/$plain_t FAIL)"; rc_all=1
    elif [ "$t" -gt 0 ] && [ "$tf" -eq "$t" ]; then
        echo "[result] $v RED on target ($tf/$t lines) - negative control bites"
    else
        echo "[result] $v did NOT go red on target ($tf/$t lines FAIL)"; rc_all=1
    fi
    echo ""
done

if [ "$rc_all" -eq 0 ]; then echo "[st-distill] PASS"; else echo "[st-distill] FAIL"; fi
exit "$rc_all"
