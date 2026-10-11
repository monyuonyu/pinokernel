#!/bin/bash
# ---------------------------------------------------------------------------
# run_fp_bare.sh — FP/SIMD registers across task switches on bare-metal
#   AArch64 (QEMU virt), ROADMAP 0-11. arch/aarch64/fp_bare_test.c is linked
#   only when EXTRA_CFLAGS has -DPK_FPBARE; usermain() runs it after the banner
#   and prints "[fpb]" lines. The DEFAULT build does not contain it (crown
#   unchanged). Hosted twin: `tkconf fp` (tests/host/run_nosw.sh).
#
#   F1/F2  the IRQ-exit switch must give a task back its FP registers
#   F3/F4  a voluntary switch must keep the callee-saved d8-d15
# Exit 0 only when the plain build prints [fpb] ALL PASS.
# ---------------------------------------------------------------------------
set -u
HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
QEMU="${QEMU:-qemu-system-aarch64}"
BOOT_TIMEOUT="${FPB_BOOT_TIMEOUT:-120}"
command -v "$QEMU" >/dev/null 2>&1 || { echo "[fp-bare] FAIL: $QEMU not found"; exit 1; }
WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT
d="$WORK/t"
mkdir -p "$d"
tar -C "$ROOT" --exclude=./.git -cf - . | tar -C "$d" -xf -
( cd "$d/boot/aarch64" && make clean ) > "$WORK/build.log" 2>&1
if ! ( cd "$d/boot/aarch64" && make EXTRA_CFLAGS="-DPK_FPBARE ${FPB_EXTRA:-}" ) >> "$WORK/build.log" 2>&1; then
    tail -20 "$WORK/build.log"; echo "[fp-bare] FAIL (build)"; exit 1
fi
( cd "$d/boot/aarch64" && timeout -k 3 "$BOOT_TIMEOUT" "$QEMU" -M virt,gic-version=2 \
    -cpu cortex-a53 -m 256M -serial stdio -display none -no-reboot -nic none \
    -kernel kernel.elf ) > "$WORK/uart.log" 2>&1 < /dev/null
tr -d '\r' < "$WORK/uart.log" | grep -a '^\[fpb\]'
if tr -d '\r' < "$WORK/uart.log" | grep -aq '^\[fpb\] ALL PASS'; then
    echo "[fp-bare] GREEN"; exit 0
fi
echo "[fp-bare] RED"; exit 1
