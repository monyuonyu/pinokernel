#!/bin/bash
# ---------------------------------------------------------------------------
# run_st_distill.sh — host cert for the smarter merge, stage 3: merge by
#   distillation (st_merge_distill / st_backward_soft in
#   arch/common/llm/student.c; docs/architecture/30-module/research/
#   brain-merge-distill.md §3).
#
# Builds student_distill_merge_test.c three times and requires:
#   plain                      -> exit 0 (all gates, T-real and T-gen)
#   -DST_DISTILL_ONE_TEACHER   -> exit 1 (teacher = into alone)
#   -DST_DISTILL_RAND_XFER     -> exit 1 (distill on random bytes)
#
#   ./run_st_distill.sh
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

rc_all=0
for v in plain ONE_TEACHER RAND_XFER; do
    def=""
    [ "$v" != plain ] && def="-DST_DISTILL_$v"
    echo "[build] $v $def"
    if ! $CC $CFLAGS $def "$SRC_TEST" "$SRC_STU" -o "$WORK/d_$v"; then
        echo "[build] $v FAILED"; rc_all=1; continue
    fi
    "$WORK/d_$v"
    rc=$?
    if [ "$v" = plain ]; then
        if [ "$rc" -eq 0 ]; then echo "[result] plain GREEN (expected)"
        else echo "[result] plain RED rc=$rc (UNEXPECTED)"; rc_all=1; fi
    else
        if [ "$rc" -eq 1 ]; then echo "[result] $v RED (expected: negative control bites)"
        else echo "[result] $v rc=$rc (UNEXPECTED: negative control did not go red)"; rc_all=1; fi
    fi
    echo ""
done

if [ "$rc_all" -eq 0 ]; then echo "[st-distill] PASS"; else echo "[st-distill] FAIL"; fi
exit "$rc_all"
