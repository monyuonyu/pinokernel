#!/bin/bash
# ---------------------------------------------------------------------------
# run_st_distill_sel.sh — host cert for the smarter merge, stages 4-6: choose
#   the distillation start by VAL (st_merge_distill_sel in
#   arch/common/llm/student.c; docs/architecture/30-module/research/
#   brain-merge-select.md §3, brain-merge-stable.md §3, brain-merge-blr.md §2-3,
#   gates fixed before code).
#
# Stage 6 splits B_LR (the init arm's distillation LR, -DD5C_B_LR) from the
# parents' LR. Two phases, on seeds no earlier stage looked at:
#   select  each candidate B_LR (3e-3 0.02 1e-3) runs plain on the selection
#           seeds 6-9; the candidate with the most PASS start=sel gate lines
#           is chosen (ties: the smaller LR). The init arm's [walk] lines must
#           differ between candidates on some seed, or the flag did nothing
#           and the cert stops RED here.
#   judge   the chosen B_LR on the judgment seeds 10-13, plain and the NCs.
# Judges LINES, not exit codes:
#   plain       every start=sel gate line ([d5c-parents-ok] included) and
#               [d5c-sel-val] PASS on every judgment seed
#   SEL_INVERT  some [d5c-sel-val] FAIL (bites whenever the two VALs differ)
#   PARENT_LR_OLD  some [d5c-parents-ok] start=sel FAIL
#   SEL_PICK / SEL_INIT  can bite only where the forced start lost on VAL
#               (from the NC's own [sel] line). Reported as "bites" (a
#               s1-gain/s2-both/floor start=sel line FAIL in the NC that is
#               PASS in plain), "does not bite", or "cannot bite"; not
#               counted in the verdict (brain-merge-blr.md §3).
#
#   ./run_st_distill_sel.sh
#   D5C_B_LR_FORCE=0.02 ./run_st_distill_sel.sh      # skip the select phase
#   D5C_SEL_SEEDS="6" D5C_JUDGE_SEEDS="10" ./run_st_distill_sel.sh
#   D5C_SHOW_SEEDS="0xC0FFEE 2 3 4 5"   # also print plain on these, not judged
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
SEL_SEEDS="${D5C_SEL_SEEDS:-6 7 8 9}"
JUDGE_SEEDS="${D5C_JUDGE_SEEDS:-10 11 12 13}"
SHOW_SEEDS="${D5C_SHOW_SEEDS:-}"
CANDS="3e-3 0.02 1e-3"
VARS="plain SEL_INVERT SEL_PICK SEL_INIT PARENT_LR_OLD"

rc_all=0
build() {   # $1 = out name, $2... = extra flags
    local o="$1"; shift
    echo "[build] $o $*"
    if ! $CC $CFLAGS "$@" "$SRC_TEST" "$SRC_STU" -lm -o "$WORK/$o"; then
        echo "[build] $o FAILED"; rc_all=1
    fi
}
gates() {   # $1 = log: the start=sel gate lines and the sel-val lines
    grep -E '^  (PASS|FAIL)  ' "$1" 2>/dev/null | grep -E 'start=sel|\[d5c-sel-val\]'
}

# ---- select ----
if [ -n "${D5C_B_LR_FORCE:-}" ]; then
    CHOSEN="$D5C_B_LR_FORCE"
    echo "[blr-select] skipped: B_LR forced to $CHOSEN"
else
    for c in $CANDS; do build "sel_$c" "-DD5C_B_LR=${c}f"; done
    for s in $SEL_SEEDS; do
        for c in $CANDS; do
            [ -x "$WORK/sel_$c" ] || continue
            D5C_SEED="$s" D5C_ONLY_SEL=1 "$WORK/sel_$c" > "$WORK/sel-$c-$s.log" 2>&1 &
        done
        wait
    done
    CHOSEN=""; best=-1
    for c in $(printf '%s\n' $CANDS | sort -g); do
        np=0; nt=0
        for s in $SEL_SEEDS; do
            echo "=== select B_LR $c seed $s ==="
            grep -E '^  \[(walk|sel) ' "$WORK/sel-$c-$s.log"
            p=$(gates "$WORK/sel-$c-$s.log" | grep -c '^  PASS')
            t=$(gates "$WORK/sel-$c-$s.log" | grep -c .)
            np=$((np + p)); nt=$((nt + t))
        done
        echo "[blr-select] B_LR $c: $np/$nt start=sel gate lines PASS on seeds $SEL_SEEDS"
        if [ "$nt" -gt 0 ] && [ "$np" -gt "$best" ]; then best=$np; CHOSEN=$c; fi
    done
    # the flag must reach the init arm: its walk lines differ somewhere
    eff=0
    for s in $SEL_SEEDS; do
        n=$(for c in $CANDS; do grep -E '^  \[walk [^ ]+ init\] ' "$WORK/sel-$c-$s.log"; done | sort -u | grep -c .)
        k=$(grep -c -E '^  \[walk [^ ]+ init\] ' "$WORK/sel-$(printf '%s\n' $CANDS | head -1)-$s.log")
        if [ "$k" -gt 0 ] && [ "$n" -gt "$k" ]; then eff=1; fi
    done
    if [ "$eff" -ne 1 ] || [ -z "$CHOSEN" ]; then
        echo "[blr-select] RED: no [walk ... init] lines, or they are the same for every B_LR - -DD5C_B_LR did not reach the init arm"
        echo "[st-distill-sel] FAIL"
        exit 1
    fi
    echo "[blr-select] chosen B_LR $CHOSEN ($best PASS lines; ties keep the smaller LR)"
fi

# ---- judge ----
for v in $VARS; do
    f=""; [ "$v" != plain ] && f="-DST_DISTILL_$v"
    build "d_$v" "-DD5C_B_LR=${CHOSEN}f" $f
done
for s in $JUDGE_SEEDS $SHOW_SEEDS; do
    for v in $VARS; do
        [ -x "$WORK/d_$v" ] || continue
        case " $SHOW_SEEDS " in *" $s "*) [ "$v" = plain ] || continue ;; esac
        D5C_SEED="$s" D5C_ONLY_SEL=1 "$WORK/d_$v" > "$WORK/$v-$s.log" 2>&1 &
    done
    wait
done

for s in $SHOW_SEEDS; do
    echo "=== show (not judged) plain seed $s B_LR $CHOSEN ==="
    grep -E '^  \[(T-real|sel|walk|parents) ' "$WORK/plain-$s.log"
    gates "$WORK/plain-$s.log"
done
for s in $JUDGE_SEEDS; do
    echo "=== judge plain seed $s B_LR $CHOSEN ==="
    grep -E '^  \[(T-real|sel|walk|parents) ' "$WORK/plain-$s.log"
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
    for s in $JUDGE_SEEDS; do
        local l
        l=$(gates "$WORK/$1-$s.log" | grep -E "$2")
        printf '%s\n' "$l" | sed "s/^/  [$1 seed $s]/"
        if printf '%s\n' "$l" | grep -q '^  FAIL'; then hit=1; fi
    done
    if [ "$hit" -eq 1 ]; then
        echo "[result] $1 RED on target - negative control bites"
    else
        echo "[result] $1 did NOT go red on target"; rc_all=1
    fi
}
# $1 = variant, $2 = "pick" or "init" (the forced start). Not in the verdict.
nc_forced() {
    local can=0 bit=0
    for s in $JUDGE_SEEDS; do
        local lose
        # [sel S1] VAL pick=X init=Y -> chose ...: the forced start lost if
        # its VAL is the higher one
        lose=$(grep -E '^  \[sel ' "$WORK/$1-$s.log" | awk -v f="$2" '{
            for (i = 1; i <= NF; i++) {
                if ($i ~ /^pick=/) p = substr($i, 6) + 0
                if ($i ~ /^init=/) q = substr($i, 6) + 0
            }
            if ((f == "pick" && p > q) || (f == "init" && q > p)) print $2
        }' | tr -d ']' | tr '\n' ' ')
        echo "  [$1 seed $s] the forced start=$2 lost on VAL in: ${lose:-none}"
        [ -n "$lose" ] && can=1
        local l
        l=$(gates "$WORK/$1-$s.log" | grep -E '\[d5c-(s1-gain|s2-both|floor)\]' | grep '^  FAIL')
        if [ -n "$l" ]; then
            while IFS= read -r x; do
                local g="${x#  FAIL  }"
                if gates "$WORK/plain-$s.log" | grep -qxF "  PASS  $g"; then
                    echo "  [$1 seed $s] FAIL here, PASS in plain: $g"; bit=1
                fi
            done <<< "$l"
        fi
    done
    if [ "$can" -eq 0 ]; then
        echo "[result] $1 cannot bite (the forced start never lost on VAL) - not counted"
    elif [ "$bit" -eq 1 ]; then
        echo "[result] $1 bites (a gate PASS in plain is FAIL here) - recorded, not counted"
    else
        echo "[result] $1 does not bite (it lost on VAL, but no gate turned) - recorded, not counted"
    fi
}
echo ""
echo "[blr] B_LR $CHOSEN, judgment seeds $JUDGE_SEEDS"
nc SEL_INVERT '\[d5c-sel-val\]'
nc PARENT_LR_OLD '\[d5c-parents-ok\]'
nc_forced SEL_PICK pick
nc_forced SEL_INIT init

if [ "$rc_all" -eq 0 ]; then echo "[st-distill-sel] PASS"; else echo "[st-distill-sel] FAIL"; fi
exit "$rc_all"
