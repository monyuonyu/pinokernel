#!/bin/bash
# ---------------------------------------------------------------------------
# run_st_gptq.sh — [st-gptq] (R-1 stage 2, GPTQ on the student;
#   docs/architecture/30-module/research/brain-quant-gptq.md §3).
#
#   plain                  -> exit 0 on every seed in SEEDS (G1..G4)
#   -DST_GPTQ_NOCOMP       -> G1 or G2 FAIL (rounds column by column, no
#                             error feedback = RTN on the same grid)
#   -DST_GPTQ_CALIB_HELD   -> G4 FAIL (calibrated on the B1 held text)
#   -DST_QUANT_BADSCALE    -> G4 FAIL (s/4: values off the absmax grid)
# A negative control that stays green fails this script. Controls run on the
# first seed only.
#
#   ./run_st_gptq.sh          exit 0 = ALL PASS
# ---------------------------------------------------------------------------
set -u
HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
CC="${CC:-cc}"
CFLAGS="-std=c11 -O1 -Wall -Wextra -ffp-contract=off -Werror=vla"
WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT
SRC="$HERE/student_gptq_test.c $ROOT/arch/common/llm/held_indep.c $ROOT/arch/common/llm/student.c"
SEEDS="${ST_GPTQ_SEEDS:-1 2 3}"
FAIL=0

for v in plain NOCOMP CALIB_HELD BADSCALE; do
    def=""
    case "$v" in
        NOCOMP|CALIB_HELD) def="-DST_GPTQ_$v" ;;
        BADSCALE)          def="-DST_QUANT_BADSCALE" ;;
    esac
    # shellcheck disable=SC2086
    $CC $CFLAGS $def $SRC -lm -o "$WORK/g_$v" || { echo "[build] $v FAILED"; FAIL=1; continue; }
done

first=""
for s in $SEEDS; do
    [ -z "$first" ] && first="$s"
    "$WORK/g_plain" "$HERE/b1_held.txt" "$s" | tee "$WORK/plain_$s.log"
    grep -q '^\[st-gptq\] ALL PASS' "$WORK/plain_$s.log" || { echo "[st-gptq] seed $s RED"; FAIL=1; }
done

nc() {  # <variant> <ERE of the gate(s) that must go red>
    "$WORK/g_$1" "$HERE/b1_held.txt" "$first" > "$WORK/nc_$1.log" 2>&1
    grep -a 'bit:\|calib' "$WORK/nc_$1.log"
    if grep -aqE "^\[st-gptq\] FAIL ($2) " "$WORK/nc_$1.log"; then
        echo "[st-gptq] PASS negative control $1 turns $(grep -aoE "^\[st-gptq\] FAIL G[0-9]" "$WORK/nc_$1.log" | grep -oE 'G[0-9]' | tr '\n' ' ')red"
    else
        echo "[st-gptq] FAIL negative control $1 did not turn $2 red"; FAIL=1
    fi
}
nc NOCOMP 'G1|G2'
nc CALIB_HELD 'G4'
nc BADSCALE 'G4'

if [ "$FAIL" = 0 ]; then echo "[st-gptq] CERT ALL PASS"; exit 0; fi
echo "[st-gptq] CERT RED"; exit 1
