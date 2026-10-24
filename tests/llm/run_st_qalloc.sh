#!/bin/bash
# ---------------------------------------------------------------------------
# run_st_qalloc.sh — [st-qalloc] (R-1 stage 5, the node places the bits at the
#   same bytes, and the copy-drift gate; docs/architecture/30-module/research/
#   brain-quant-alloc.md §3). Not in CI (a research cert, as run_st_qmix.sh).
#
#   plain                    -> exit 0 on every seed in SEEDS (I0 P0 A0 P1 G4
#                               K0 D1), and "value P2 1" on >= 6 of 8 seeds
#   -DST_QALLOC_INVERT       -> P1 FAIL (the node picks the largest omega)
#   -DST_QUANT_BADSCALE      -> D1 FAIL (s/4 clips: E4 refused by the gate)
#   -DST_QALLOC_FLATFISHER   -> values only (every F = 1: the pick by
#                               ||Q(W) - W||^2 alone)
# A negative control that stays green fails this script. Controls run on the
# first seed only. D2 (held KL against the gate) is printed, never gated.
#
#   ./run_st_qalloc.sh          exit 0 = ALL PASS
# ---------------------------------------------------------------------------
set -u
HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
CC="${CC:-cc}"
# ST_QALLOC_DEF: extra defines for every variant (run_st_qalloc_el.sh, stage 6)
CFLAGS="-std=c11 -O1 -Wall -Wextra -ffp-contract=off -Werror=vla ${ST_QALLOC_DEF:-}"
WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT
SRC="$HERE/student_qalloc_test.c $ROOT/arch/common/llm/held_indep.c $ROOT/arch/common/llm/student.c"
SEEDS="${ST_QALLOC_SEEDS:-1 2 3 4 5 6 7 8}"
FAIL=0

for v in plain INVERT BADSCALE FLATFISHER; do
    def=""
    case "$v" in
        INVERT|FLATFISHER) def="-DST_QALLOC_$v" ;;
        BADSCALE)          def="-DST_QUANT_BADSCALE" ;;
    esac
    # shellcheck disable=SC2086
    $CC $CFLAGS $def $SRC -lm -o "$WORK/q_$v" || { echo "[build] $v FAILED"; FAIL=1; continue; }
done

first=""; nseed=0; p2=0
for s in $SEEDS; do
    [ -z "$first" ] && first="$s"
    nseed=$((nseed + 1))
    "$WORK/q_plain" "$HERE/b1_held.txt" "$s" | tee "$WORK/plain_$s.log"
    grep -q '^\[st-qalloc\] ALL PASS' "$WORK/plain_$s.log" || { echo "[st-qalloc] seed $s RED"; FAIL=1; }
    grep -q '^\[st-qalloc\] value P2 1' "$WORK/plain_$s.log" && p2=$((p2 + 1))
done
# P2: 6 of 8 (the same share when SEEDS is shorter, rounded up)
need=$(( (nseed * 6 + 7) / 8 ))
if [ "$p2" -ge "$need" ]; then
    echo "[st-qalloc] PASS P2 the largest omega is worse than AUTO on $p2/$nseed seeds (need $need)"
else
    echo "[st-qalloc] FAIL P2 the largest omega is worse than AUTO on $p2/$nseed seeds (need $need)"; FAIL=1
fi

nc() {  # <variant> <ERE of the gate(s) that must go red>
    "$WORK/q_$1" "$HERE/b1_held.txt" "$first" > "$WORK/nc_$1.log" 2>&1
    grep -a 'omega\|damage\|gate KL' "$WORK/nc_$1.log"
    if grep -aqE "^\[st-qalloc\] FAIL ($2) " "$WORK/nc_$1.log"; then
        echo "[st-qalloc] PASS negative control $1 turns $(grep -aoE "^\[st-qalloc\] FAIL [A-Z][0-9]" "$WORK/nc_$1.log" | grep -oE '[A-Z][0-9]' | tr '\n' ' ')red"
    else
        echo "[st-qalloc] FAIL negative control $1 did not turn $2 red"; FAIL=1
    fi
}
nc INVERT 'P1'
nc BADSCALE 'D1'
"$WORK/q_FLATFISHER" "$HERE/b1_held.txt" "$first" | grep -a 'omega' | sed 's/^/[st-qalloc] value FLATFISHER: /'

if [ "$FAIL" = 0 ]; then echo "[st-qalloc] CERT ALL PASS"; exit 0; fi
echo "[st-qalloc] CERT RED"; exit 1
