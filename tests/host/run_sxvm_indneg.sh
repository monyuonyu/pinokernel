#!/bin/bash
# ---------------------------------------------------------------------------
# run_sxvm_indneg.sh — [sxvm-indneg] stage 5e of docs/architecture/30-module/
# research/safe-exec-log.md (research, not wired into CI): the decreasing
# index (-DSXVM_INDNEG, rules A and B) against IND, with its two breakers.
#
#   SX_PY=<python with z3> ./run_sxvm_indneg.sh <outdir> <ai_progs.c> <ai_t8g.c>
#   K: neg-count accepted only with INDNEG, neg-edge refused (red under both
#   breakers). AI: T8 refused, T8g accepted by INDNEG with every answer right.
#   Pairs (dump, gen 1-2 x seed 1-3): extra over IND, their traps, "lost"
#   (IND accepts, INDNEG refuses: must be 0); INDNEG's extra through Z3.
#   exit 0 = E1-E4 and both K breakers as fixed (the pair breakers are read).
# ---------------------------------------------------------------------------
set -u
HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
OUT="$1"; AI="$2"; T8G="$3"
PY="${SX_PY:-python3}"
ARMS="head IND INDNEG INDNEGOFF INDNEGNOB"
mkdir -p "$OUT/d"
FAIL=0
say() { if [ "$1" = 1 ]; then echo "[sxvm-indneg] $2 PASS"; else echo "[sxvm-indneg] $2 FAIL"; FAIL=1; fi; }
flags() {
    case "$1" in
    head) echo "" ;; IND) echo "-DSXVM_IND" ;; INDNEG) echo "-DSXVM_IND -DSXVM_INDNEG" ;;
    INDNEGOFF) echo "-DSXVM_IND -DSXVM_INDNEG -DSXVM_BREAK_INDNEGOFF" ;;
    INDNEGNOB) echo "-DSXVM_IND -DSXVM_INDNEG -DSXVM_BREAK_INDNEGNOB" ;;
    esac
}
SAN="-O1 -g -fsanitize=address,undefined -fno-sanitize-recover=undefined"
for a in $ARMS; do
    # shellcheck disable=SC2046
    cc -std=c11 $SAN -Wall -Wextra -Werror $(flags $a) -I"$ROOT/arch/common/include" \
        "$HERE/sxvm_test.c" "$ROOT/arch/common/sxvm.c" -o "$OUT/sx_$a" || { echo "[sxvm-indneg] build FAILED $a"; exit 1; }
    # shellcheck disable=SC2046
    cc -std=c11 -O2 -Wall -Wextra -Werror $(flags $a) -I"$ROOT/arch/common/include" \
        "$HERE/sxvm_test.c" "$ROOT/arch/common/sxvm.c" -o "$OUT/o2_$a" || { echo "[sxvm-indneg] build FAILED o2 $a"; exit 1; }
    for f in ai t8g; do
        src="$AI"; [ $f = t8g ] && src="$T8G"
        # shellcheck disable=SC2046
        cc -std=c11 $SAN -Wall -Wextra $(flags $a) -I"$ROOT/arch/common/include" \
            "$HERE/sxvm_ai_test.c" "$src" "$ROOT/arch/common/sxvm.c" -o "$OUT/${f}_$a" 2>/dev/null \
            || { echo "[sxvm-indneg] build FAILED $f $a"; exit 1; }
    done
done
# K (sxvm_test known(); one fuzz program so the run is short)
for a in $ARMS; do
    "$OUT/sx_$a" 1 1 > "$OUT/k-$a.log" 2>&1
    grep -E '^\[sxvm\] K (neg-|sum-unm|ind-)' "$OUT/k-$a.log" | sed "s/^\[sxvm\]/[sxvm-indneg] $a/"
done
kf() { grep -E "^\[sxvm\] K $2 " "$OUT/k-$1.log" | grep -c FAIL; }
for a in head IND INDNEG; do say "$([ "$(grep -c 'K.*FAIL' "$OUT/k-$a.log")" = 0 ] && echo 1)" "K $a every K ok"; done
for a in INDNEGOFF INDNEGNOB; do say "$([ "$(kf $a neg-edge)" = 1 ] && echo 1)" "K breaker $a neg-edge red"; done
# AI sets: the agent's T8 and the checked T8g
for a in $ARMS; do
    "$OUT/ai_$a" 1000 > "$OUT/ai-$a.log"; "$OUT/t8g_$a" 1000 > "$OUT/t8g-$a.log"
done
t8() { awk '$3 == "T8" { print $5, $7, $8 }' "$1"; }
echo "[sxvm-indneg] T8  head/IND/INDNEG: $(t8 "$OUT/ai-head.log") / $(t8 "$OUT/ai-IND.log") / $(t8 "$OUT/ai-INDNEG.log")"
echo "[sxvm-indneg] T8g head/IND/INDNEG: $(t8 "$OUT/t8g-head.log") / $(t8 "$OUT/t8g-IND.log") / $(t8 "$OUT/t8g-INDNEG.log")"
say "$(t8 "$OUT/ai-INDNEG.log" | grep -q '^check=-5 ' && echo 1)" "E2 T8 refused by INDNEG"
say "$(t8 "$OUT/t8g-INDNEG.log" | grep -qE '^check=[0-9]+ trap=0 agree=1000$' && echo 1)" "E1 T8g accepted by INDNEG, 1000/1000"
say "$(t8 "$OUT/t8g-IND.log" | grep -q '^check=-5 ' && echo 1)" "E1 T8g refused by IND"
for f in ai t8g; do
    same=$(diff <(grep -v ' T8 ' "$OUT/$f-IND.log") <(grep -v ' T8 ' "$OUT/$f-INDNEG.log") > /dev/null && echo 1)
    say "$same" "E3 $f T1-T7 INDNEG = IND"
done
"$OUT/sx_IND" useful 1000 > "$OUT/useful-IND.log" 2>&1; "$OUT/sx_INDNEG" useful 1000 > "$OUT/useful-INDNEG.log" 2>&1
say "$(diff "$OUT/useful-IND.log" "$OUT/useful-INDNEG.log" > /dev/null && echo 1)" "E3 useful INDNEG = IND"
# pairs over the same 200000 programs
lost=0; ext=0; extr=0
for g in 1 2; do for s in 1 2 3; do
    for a in IND INDNEG INDNEGOFF INDNEGNOB; do "$OUT/o2_$a" dump 200000 $s 10 $g > "$OUT/d/$a-g$g-s$s.txt"; done
    for a in INDNEG INDNEGOFF INDNEGNOB; do
        line=$(awk 'NR==FNR { if ($1=="A") h[$2]=$3; next } $1=="A" { seen[$2]=1; if (!($2 in h)) { ex++; if ($4!=0) tr++ } else if (h[$2]!=$3) bd++ }
                    END { for (k in h) if (!(k in seen)) lost++; printf "extra=%d extra_trap=%d lost=%d bound_diff=%d", ex, tr, lost, bd }' \
               "$OUT/d/IND-g$g-s$s.txt" "$OUT/d/$a-g$g-s$s.txt")
        echo "[sxvm-indneg] pair g$g s$s $a $line"
        if [ $a = INDNEG ]; then
            eval "$(echo "$line" | tr ' ' '\n' | sed 's/^/p_/')"
            lost=$((lost + p_lost + p_bound_diff)); ext=$((ext + p_extra)); extr=$((extr + p_extra_trap))
            only=$(awk 'NR==FNR { if ($1=="A") h[$2]=1; next } $1=="A" && !($2 in h) { print $2 }' \
                   "$OUT/d/IND-g$g-s$s.txt" "$OUT/d/INDNEG-g$g-s$s.txt" | paste -sd,)
            if [ -n "$only" ]; then
                "$PY" -I "$HERE/sxvm_z3.py" "$OUT/d/INDNEG-g$g-s$s.txt" "$OUT/z-g$g-s$s" --only "$only" || { echo "[sxvm-indneg] z3 FAILED"; exit 1; }
                "$OUT/o2_INDNEG" replay < "$OUT/z-g$g-s$s.replay" > "$OUT/z-g$g-s$s.rep"
                sed "s/^/[sxvm-indneg] z3 g$g s$s /" "$OUT/z-g$g-s$s"
            fi
        fi
    done
done; done
zt=$(cat "$OUT"/z-g*-s*.rep 2>/dev/null | awk '$1 != "over" && $1 != 0' | wc -l)
say "$([ "$lost" = 0 ] && echo 1)" "E3 pairs: IND accepts and INDNEG refuses or bounds differ = $lost"
say "$([ "$extr" = 0 ] && [ "$zt" = 0 ] && echo 1)" "E4 pairs: INDNEG extra=$ext, one-filling traps=$extr, Z3 traps replayed in C=$zt"
exit "$FAIL"
