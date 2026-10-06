#!/bin/bash
# ---------------------------------------------------------------------------
# run_nosw_bare.sh — no-switch regions on bare metal (R-NOSW-BARE,
#   arch/common/include/pk_nosw.h; gap-ledger EXIT-DISPATCH-SHARED-STATE).
#
# Bare x86 and AArch64 switch at the outermost interrupt exit (RNG0), so a
# task reading a kdds topic slot could be switched out half-way through the
# copy. tk_conform.c's [nosw-bare] (built with -DPK_TKCONF -DPK_TKCONF_NOSW)
# reads a LATEST_ONLY topic while a higher task publishes 60 values, and
# counts torn reads. Per arch, two arms:
#   new  regions on                  -> "[nosw-bare] PASS" (0 torn)
#   off  -DPK_NOSW_OFF (regions gone) -> "[nosw-bare] FAIL" (the NC; 25-35
#        torn reads in 2026-10-07's boots)
#
#   ./tests/run_nosw_bare.sh            (both arches)
#   ARCHES=x86 ./tests/run_nosw_bare.sh
# ---------------------------------------------------------------------------
set -u
HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/.." && pwd)"
ARCHES="${ARCHES:-x86 aarch64}"
BOOT_TIMEOUT="${NOSW_BOOT_TIMEOUT:-90}"
WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT

rc_all=0
arm() {  # $1 = arch, $2 = new|off
    local d="$WORK/$1-$2" log="$WORK/$1-$2.serial" cf line v want
    cf="-DPK_TKCONF -DPK_TKCONF_NOSW"
    [ "$2" = off ] && cf="$cf -DPK_NOSW_OFF"
    mkdir -p "$d"
    tar -C "$ROOT" --exclude=./.git -cf - . | tar -C "$d" -xf -
    : > "$log"
    if [ "$1" = x86 ]; then
        if ! ( cd "$d/boot/x86" && make clean && make EXTRA_CFLAGS="$cf" bootloader.bin ) > "$WORK/$1-$2.build" 2>&1; then
            tail -20 "$WORK/$1-$2.build"; echo "[result] $1 $2 build FAILED"; rc_all=1; return
        fi
        ( cd "$d/boot/x86" && timeout "$BOOT_TIMEOUT" qemu-system-x86_64 -m 256 \
            -kernel bootloader.bin -serial file:"$log" -cpu qemu64 -display none \
            -no-reboot ) > "$WORK/$1-$2.qemu" 2>&1
    else
        if ! ( cd "$d/boot/aarch64" && make clean && make EXTRA_CFLAGS="$cf" ) > "$WORK/$1-$2.build" 2>&1; then
            tail -20 "$WORK/$1-$2.build"; echo "[result] $1 $2 build FAILED"; rc_all=1; return
        fi
        ( cd "$d/boot/aarch64" && timeout -k 3 "$BOOT_TIMEOUT" qemu-system-aarch64 \
            -M virt,gic-version=2 -cpu cortex-a53 -m 256M -serial stdio -display none \
            -no-reboot -nic none -kernel kernel.elf ) > "$log" 2>&1 < /dev/null
    fi
    line=$(tr -d '\r' < "$log" | grep -a -m1 '^\[nosw-bare\]')
    echo "  $1 $2: ${line:-(no [nosw-bare] line)}"
    case "$line" in
        "[nosw-bare] PASS"*) v=PASS ;;
        "[nosw-bare] FAIL"*) v=FAIL ;;
        *) v=NONE ;;
    esac
    want=PASS; [ "$2" = off ] && want=FAIL
    if [ "$v" = "$want" ]; then echo "[result] $1 $2 $v (expected)"
    else echo "[result] $1 $2 $v (UNEXPECTED, want $want)"; rc_all=1; fi
}

for a in $ARCHES; do
    arm "$a" new
    arm "$a" off
done
if [ "$rc_all" -eq 0 ]; then echo "[nosw-bare] PASS"; else echo "[nosw-bare] FAIL"; fi
exit "$rc_all"
