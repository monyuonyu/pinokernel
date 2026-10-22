#!/bin/bash
# ---------------------------------------------------------------------------
# run_st_qmix.sh — [st-qmix] (R-1 stage 4, embed 4 bit against out 4 bit at the
#   same bytes; docs/architecture/30-module/research/brain-quant-embed.md §3).
#   Not in CI (a research cert, as run_st_gptq.sh).
#
#   plain                  -> exit 0 on every seed in SEEDS (I0 Q0 Q1 Q2 G1 G4
#                             D0); E4 and O4 were first run by this cert
#   -DST_QMIX_SWAP         -> Q1 FAIL (the E4 call builds O4)
#   -DST_QMIX_EMB8         -> Q0 FAIL (embed 8 bit: not the same bytes)
#   -DST_QUANT_BADSCALE    -> G4 FAIL (s/4: values off the absmax grid)
#   -DST_QMIX_EMBH2        -> values only (embed held at 4 bit, H collected
#                             through the 2-bit embed)
# A negative control that stays green fails this script. Controls run on the
# first seed only.
#
#   ./run_st_qmix.sh          exit 0 = ALL PASS
# ---------------------------------------------------------------------------
set -u
HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
CC="${CC:-cc}"
CFLAGS="-std=c11 -O1 -Wall -Wextra -ffp-contract=off -Werror=vla"
WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT
SRC="$HERE/student_qmix_test.c $ROOT/arch/common/llm/held_indep.c $ROOT/arch/common/llm/student.c"
SEEDS="${ST_QMIX_SEEDS:-1 2 3 4 5 6 7 8}"
FAIL=0

for v in plain SWAP EMB8 BADSCALE EMBH2; do
    def=""
    case "$v" in
        SWAP|EMB8|EMBH2) def="-DST_QMIX_$v" ;;
        BADSCALE)        def="-DST_QUANT_BADSCALE" ;;
    esac
    # shellcheck disable=SC2086
    $CC $CFLAGS $def $SRC -lm -o "$WORK/q_$v" || { echo "[build] $v FAILED"; FAIL=1; continue; }
done

first=""
for s in $SEEDS; do
    [ -z "$first" ] && first="$s"
    "$WORK/q_plain" "$HERE/b1_held.txt" "$s" | tee "$WORK/plain_$s.log"
    grep -q '^\[st-qmix\] ALL PASS' "$WORK/plain_$s.log" || { echo "[st-qmix] seed $s RED"; FAIL=1; }
done

nc() {  # <variant> <ERE of the gate(s) that must go red>
    "$WORK/q_$1" "$HERE/b1_held.txt" "$first" > "$WORK/nc_$1.log" 2>&1
    grep -a 'bytes\|damage' "$WORK/nc_$1.log"
    if grep -aqE "^\[st-qmix\] FAIL ($2) " "$WORK/nc_$1.log"; then
        echo "[st-qmix] PASS negative control $1 turns $(grep -aoE "^\[st-qmix\] FAIL [A-Z][0-9]" "$WORK/nc_$1.log" | grep -oE '[A-Z][0-9]' | tr '\n' ' ')red"
    else
        echo "[st-qmix] FAIL negative control $1 did not turn $2 red"; FAIL=1
    fi
}
nc SWAP 'Q1'
nc EMB8 'Q0'
nc BADSCALE 'G4'
for s in $SEEDS; do  # values only
    r="$("$WORK/q_EMBH2" "$HERE/b1_held.txt" "$s" | grep -ao 'bpb damage seq [^;]*')"
    r0="$(grep -ao 'bpb damage seq [^;]*' "$WORK/plain_$s.log")"
    echo "[st-qmix] value EMBH2 seed $s: $r (plain: $r0)"
done

if [ "$FAIL" = 0 ]; then echo "[st-qmix] CERT ALL PASS"; exit 0; fi
echo "[st-qmix] CERT RED"; exit 1
