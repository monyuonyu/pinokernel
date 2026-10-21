#!/bin/bash
# ---------------------------------------------------------------------------
# run_sxvm_ai.sh — [sxvm-ai] stage 5c of docs/architecture/30-module/research/
# safe-exec-log.md (research, not wired into CI).
#
#   ./run_sxvm_ai.sh <ai_progs.c> [count]
#   prints the AI lines for head, PREC and IND, then A1 (no trap among the
#   accepted, every arm) and the accepted counts (A2/A3 read by hand).
#   AI_STRIP=1: every ANDI x,63 replaced by ADDI x,0 (added after the
#   masked run, not fixed before it).
# ---------------------------------------------------------------------------
set -u
HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
AI="$1"; N="${2:-1000}"
WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT
FAIL=0
for a in head PREC IND; do
    f=""; [ "$a" = PREC ] && f=-DSXVM_PREC; [ "$a" = IND ] && f=-DSXVM_IND
    [ -n "${AI_STRIP:-}" ] && f="$f -DAI_STRIP"
    # shellcheck disable=SC2086
    cc -std=c11 -O1 -g -Wall -Wextra -fsanitize=address,undefined -fno-sanitize-recover=undefined $f \
        -I"$ROOT/arch/common/include" "$HERE/sxvm_ai_test.c" "$AI" "$ROOT/arch/common/sxvm.c" -o "$WORK/ai_$a" \
        || { echo "[sxvm-ai] build FAILED $a"; exit 1; }
    "$WORK/ai_$a" "$N" > "$WORK/$a.log" 2>&1 || { echo "[sxvm-ai] run FAILED $a"; cat "$WORK/$a.log"; exit 1; }
    sed "s/^\[sxvm\]/[sxvm-ai] $a/" "$WORK/$a.log"
    acc=$(awk '{ for (i = 1; i <= NF; i++) if ($i ~ /^check=/) { split($i, x, "="); if (x[2] >= 0) c++ } } END { print c + 0 }' "$WORK/$a.log")
    tr=$(awk '{ for (i = 1; i <= NF; i++) if ($i ~ /^trap=/) { split($i, x, "="); c += x[2] } } END { print c + 0 }' "$WORK/$a.log")
    wrong=$(awk '{ for (i = 1; i <= NF; i++) { split($i, x, "="); v[x[1]] = x[2] } if (v["check"] >= 0 && v["agree"] + v["trap"] < v["run"]) c++ } END { print c + 0 }' "$WORK/$a.log")
    echo "[sxvm-ai] $a accepted=$acc/8 traps=$tr wrong_answer_tasks=$wrong"
    if [ "$tr" = 0 ]; then echo "[sxvm-ai] A1 $a PASS"; else echo "[sxvm-ai] A1 $a FAIL"; FAIL=1; fi
done
exit "$FAIL"
