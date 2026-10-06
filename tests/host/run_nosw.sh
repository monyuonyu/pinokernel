#!/bin/bash
# ---------------------------------------------------------------------------
# run_nosw.sh — the hosted exit dispatch's no-switch regions and compute pool
#   (gap-ledger EXIT-DISPATCH-SHARED-STATE / RNG0-HOSTED; arch/common/include/
#   pk_nosw.h, arch/linux/x86_64/pool_preempt.c, shell verbs `tkconf nosw` and
#   `tkconf pool`).
#
# On hosted Linux x86_64 the timer may switch tasks at any instruction, so
# shared state is bracketed in per-task no-switch regions. The probes race a
# pri-19 task against a pri-20 runner on each guarded state (kdds slot, p-fs,
# student, ARK, dtr, dlb, ...) and must print ALL PASS.
#
# Arms (each a separate build of the same tree):
#   plain    `tkconf nosw` and `tkconf pool` ALL PASS
#   off      -DPK_NOSW_OFF (regions empty): `tkconf nosw` must be RED on N1
#   slow     -DARK_FAULT_SLOW_GET=200000 (ARK get widened): ALL PASS
#   slowoff  the same with -DPK_NOSW_OFF: must be RED on N9 (unwidened, N9
#            catches the ARK race only 1 time in 2, so CI asks it widened)
#   dropfp   -DPK_PREEMPT_DROP_FP (xmm0-15 zeroed, not restored): `tkconf fp`
#            must be RED on F1 (plain also runs `tkconf fp`: ALL PASS)
# A negative-control arm that stays green fails this script.
#
#   tests/host/run_nosw.sh            (x86_64 host)
# ---------------------------------------------------------------------------
set -u
HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT
rc_all=0

drive() {  # $1 = tree root, $2 = verb -> its output on stdout
    printf '%s\nexit\n' "$2" | PKERNEL_GALAXY=0 timeout 300 "$1/boot/linux_x86_64/p-kernel" 2>&1 \
        | tr -d '\r' | grep -a '^\[\(nosw\|pool\|fp\)\]'
}

arm() {  # $1 = name, $2 = EXTRA_CFLAGS -> tree root on stdout ("" on failure)
    local d="$WORK/$1"
    mkdir -p "$d"
    tar -C "$ROOT" --exclude=./.git -cf - . | tar -C "$d" -xf -
    make -C "$d/boot/linux_x86_64" clean > /dev/null 2>&1
    if ! make -C "$d/boot/linux_x86_64" -j4 EXTRA_CFLAGS="$2" > "$WORK/build-$1.log" 2>&1; then
        tail -20 "$WORK/build-$1.log" >&2; echo ""; return
    fi
    echo "$d"
}

echo "[build] plain"
if ! make -C "$ROOT/boot/linux_x86_64" -j4 > "$WORK/build-plain.log" 2>&1; then
    tail -20 "$WORK/build-plain.log"; echo "[nosw-cert] FAIL (build)"; exit 1
fi
for v in "tkconf nosw" "tkconf pool" "tkconf fp"; do
    drive "$ROOT" "$v" > "$WORK/plain.log"
    cat "$WORK/plain.log"
    if grep -q '^\[\(nosw\|pool\|fp\)\] ALL PASS' "$WORK/plain.log"; then echo "[result] plain $v GREEN (expected)"
    else echo "[result] plain $v RED (UNEXPECTED)"; rc_all=1; fi
    echo ""
done

expect_red() {  # $1 = arm, $2 = EXTRA_CFLAGS, $3 = check id that must FAIL
    echo "[build] $1 ($2)"
    local d; d=$(arm "$1" "$2")
    [ -n "$d" ] || { echo "[result] $1 build FAILED"; rc_all=1; return; }
    drive "$d" "tkconf nosw" > "$WORK/$1.log"
    grep '^\[nosw\] FAIL\|ALL PASS\|RED' "$WORK/$1.log"
    if grep -q "^\[nosw\] FAIL $3 " "$WORK/$1.log"; then echo "[result] $1 RED on $3 (expected)"
    else echo "[result] $1 not red on $3 (UNEXPECTED: the probe missed the empty regions)"; rc_all=1; fi
    echo ""
}
expect_green() {  # $1 = arm, $2 = EXTRA_CFLAGS
    echo "[build] $1 ($2)"
    local d; d=$(arm "$1" "$2")
    [ -n "$d" ] || { echo "[result] $1 build FAILED"; rc_all=1; return; }
    drive "$d" "tkconf nosw" > "$WORK/$1.log"
    grep '^\[nosw\] FAIL\|ALL PASS\|RED' "$WORK/$1.log"
    if grep -q '^\[nosw\] ALL PASS' "$WORK/$1.log"; then echo "[result] $1 GREEN (expected)"
    else echo "[result] $1 RED (UNEXPECTED)"; rc_all=1; fi
    echo ""
}
expect_red off "-DPK_NOSW_OFF" N1
expect_green slow "-DARK_FAULT_SLOW_GET=200000"
expect_red slowoff "-DARK_FAULT_SLOW_GET=200000 -DPK_NOSW_OFF" N9

# FP-PRESERVE: the trampoline must give the task back its FP/SIMD registers.
echo "[build] dropfp (-DPK_PREEMPT_DROP_FP)"
d=$(arm dropfp "-DPK_PREEMPT_DROP_FP")
if [ -z "$d" ]; then echo "[result] dropfp build FAILED"; rc_all=1
else
    drive "$d" "tkconf fp" > "$WORK/dropfp.log"
    grep '^\[fp\] \(FAIL\|PASS\|ALL PASS\|RED\)' "$WORK/dropfp.log"
    if grep -q '^\[fp\] FAIL F1 ' "$WORK/dropfp.log"; then echo "[result] dropfp RED on F1 (expected)"
    else echo "[result] dropfp not red on F1 (UNEXPECTED: lost FP registers went unseen)"; rc_all=1; fi
    echo ""
fi

if [ "$rc_all" -eq 0 ]; then echo "[nosw-cert] PASS"; else echo "[nosw-cert] FAIL"; fi
exit "$rc_all"
