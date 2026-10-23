#!/bin/bash
# ---------------------------------------------------------------------------
# run_st_distill_sel.sh — host cert for the smarter merge, stage 4: choose
#   the distillation start by VAL (st_merge_distill_sel in
#   arch/common/llm/student.c; docs/architecture/30-module/research/
#   brain-merge-select.md §3, gates fixed before code).
#
# Builds student_distill_merge_test.c plain and once per negative control,
# runs only the start=sel arm (D5C_ONLY_SEL=1) on each seed, and judges
# LINES, not exit codes:
#   plain       every gate line with start=sel ([d5c-parents-ok] included,
#               stage 5, brain-merge-stable.md §3), and [d5c-sel-val],
#               PASS on every seed
#   SEL_INVERT  some [d5c-s1-gain] or [d5c-s2-both] start=sel line FAIL
#   SEL_PICK    some [d5c-s1-gain] start=sel line FAIL   (S1 needs init)
#   SEL_INIT    some [d5c-s2-both] start=sel line FAIL   (S2 needs pick)
#   (an NC that is red on any seed bites; per-seed lines are printed)
#
#   ./run_st_distill_sel.sh                 # seeds 0xC0FFEE 2 3 4 5
#   D5C_SEEDS="0xC0FFEE" ./run_st_distill_sel.sh
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
SEEDS="${D5C_SEEDS:-0xC0FFEE 2 3 4 5}"
VARS="plain SEL_INVERT SEL_PICK SEL_INIT"

rc_all=0
for v in $VARS; do
    f=""; [ "$v" != plain ] && f="-DST_DISTILL_$v"
    echo "[build] $v $f"
    if ! $CC $CFLAGS $f "$SRC_TEST" "$SRC_STU" -lm -o "$WORK/d_$v"; then
        echo "[build] $v FAILED"; rc_all=1
    fi
done
for s in $SEEDS; do
    for v in $VARS; do
        [ -x "$WORK/d_$v" ] || continue
        D5C_SEED="$s" D5C_ONLY_SEL=1 "$WORK/d_$v" > "$WORK/$v-$s.log" 2>&1 &
    done
done
wait

gates() {   # $1 = log: the start=sel gate lines and the sel-val lines
    grep -E '^  (PASS|FAIL)  ' "$1" 2>/dev/null | grep -E 'start=sel|\[d5c-sel-val\]'
}

for s in $SEEDS; do
    echo "=== plain seed $s ==="
    grep -E '^  \[(T-real|sel) ' "$WORK/plain-$s.log"
    gates "$WORK/plain-$s.log"
    n=$(gates "$WORK/plain-$s.log" | grep -c .)
    nf=$(gates "$WORK/plain-$s.log" | grep -c '^  FAIL')
    if [ "$n" -gt 0 ] && [ "$nf" -eq 0 ]; then
        echo "[result] plain seed $s GREEN ($n lines)"
    else
        echo "[result] plain seed $s RED ($nf/$n FAIL)"; rc_all=1
    fi
done

nc() {   # $1 = variant, $2 = grep -E pattern for the target lines
    local hit=0
    for s in $SEEDS; do
        local l
        l=$(grep -E '^  (PASS|FAIL)  ' "$WORK/$1-$s.log" 2>/dev/null | grep -F 'start=sel' | grep -E "$2")
        printf '%s\n' "$l" | sed "s/^/  [$1 seed $s]/"
        if printf '%s\n' "$l" | grep -q '^  FAIL'; then hit=1; fi
    done
    if [ "$hit" -eq 1 ]; then
        echo "[result] $1 RED on target - negative control bites"
    else
        echo "[result] $1 did NOT go red on target"; rc_all=1
    fi
}
echo ""
nc SEL_INVERT '\[d5c-s1-gain\]|\[d5c-s2-both\]'
nc SEL_PICK   '\[d5c-s1-gain\]'
nc SEL_INIT   '\[d5c-s2-both\]'

if [ "$rc_all" -eq 0 ]; then echo "[st-distill-sel] PASS"; else echo "[st-distill-sel] FAIL"; fi
exit "$rc_all"
