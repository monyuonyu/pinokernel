#!/bin/bash
# ---------------------------------------------------------------------------
# run_st_delta.sh — host cert for the one-bit delta, stage 1
#   (st_delta_sign in arch/common/llm/student.c;
#    docs/architecture/30-module/research/delta-quant.md).
#
# Builds student_delta_test.c two ways and requires:
#   plain                 -> exit 0 on EVERY seed in SEEDS
#   -DST_DQ_TEST_ALLPLUS  -> exit 1 (the sign thrown away: [dq-onebit] RED)
# The expectations are in the test's header, written before its first run.
#
#   ./run_st_delta.sh
# ---------------------------------------------------------------------------
set -u
HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
CC="${CC:-cc}"
CFLAGS="-std=c11 -O1 -Wall -Wextra -ffp-contract=off -Werror=vla"
WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT
SRC_TEST="$HERE/student_delta_test.c"
SRC_STU="$ROOT/arch/common/llm/student.c"
SEEDS="${ST_DELTA_SEEDS:-0xC0FFEE 1 12345}"

rc_all=0
for v in plain ALLPLUS; do
    def=""
    [ "$v" != plain ] && def="-DST_DQ_TEST_$v"
    echo "[build] $v $def"
    if ! $CC $CFLAGS $def "$SRC_TEST" "$SRC_STU" -o "$WORK/d_$v" -lm; then
        echo "[st-delta] BUILD FAIL ($v)"; exit 1
    fi
done
for s in $SEEDS; do
    "$WORK/d_plain" "$s"; rc=$?
    echo "[st-delta] plain seed=$s rc=$rc"
    [ "$rc" -eq 0 ] || rc_all=1
done
"$WORK/d_ALLPLUS"; rc=$?
echo "[st-delta] negative control ALLPLUS rc=$rc (expect 1)"
[ "$rc" -eq 1 ] || rc_all=1
if [ "$rc_all" -eq 0 ]; then echo "[st-delta] ALL PASS"; else echo "[st-delta] RED"; fi
exit $rc_all
