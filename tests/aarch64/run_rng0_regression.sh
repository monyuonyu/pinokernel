#!/bin/sh
# tests/aarch64/run_rng0_regression.sh
#
# gap-ledger RNG0-AARCH64-NO-IRQ-EXIT-DISPATCH: the AArch64 twin of
# tests/x86/run_rng0_regression.sh. A task that never calls the kernel must not
# starve a higher-priority task whose delay ends meanwhile: the switch has to
# happen at the exit of the timer IRQ (the x86 fix of 2026-09-28 does it in
# knl_irq_exit_dispatch; bare AArch64's default build did not, measured
# 2026-09-28 with tk_conform NC-NODDS / K7).
# Builds boot/aarch64 with -DPK_RNG0_REGR_TEST (probe A16 in
# arch/aarch64/usermain.c; the default build does not contain it) and boots it.
#
#   ARM=positive  the spinner never calls the kernel      -> expect GREEN
#   ARM=control   the spinner sleeps 10 ms per turn       -> expect GREEN (probe sanity)
#   ARM=nofix     positive with -DPK_RNG0_NO_EXIT_DISPATCH -> expect RED (negative control)
#
# Verdict line: [rng0-a64] arm=<arm> verdict=<RED|GREEN>. Exit 0 = as expected.
set -eu
HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
BOOT="$ROOT/boot/aarch64"
ARM="${ARM:-}"
QEMU="${QEMU:-qemu-system-aarch64}"
TMO="${RNG0_BOOT_TIMEOUT:-20}"
case "$ARM" in
    positive) DEFS="-DPK_RNG0_REGR_TEST";                            EXPECT=GREEN ;;
    control)  DEFS="-DPK_RNG0_REGR_TEST -DPK_RNG0_REGR_CONTROL";     EXPECT=GREEN ;;
    nofix)    DEFS="-DPK_RNG0_REGR_TEST -DPK_RNG0_NO_EXIT_DISPATCH"; EXPECT=RED ;;
    *) echo "[rng0-a64] FATAL: set ARM=positive, control or nofix" >&2; exit 1 ;;
esac
command -v "$QEMU" >/dev/null 2>&1 || { echo "[rng0-a64] FATAL: $QEMU not found" >&2; exit 1; }
SCRATCH="$(mktemp -d "${TMPDIR:-/tmp}/rng0a64.XXXXXX")"
trap 'rm -rf "$SCRATCH"' EXIT
( cd "$BOOT" && make clean ) > "$SCRATCH/build.log" 2>&1 || true
if ! ( cd "$BOOT" && make EXTRA_CFLAGS="$DEFS" ) >> "$SCRATCH/build.log" 2>&1; then
    tail -40 "$SCRATCH/build.log" >&2
    echo "[rng0-a64] FATAL: build failed" >&2; exit 1
fi
LOG="$SCRATCH/serial.log"
( cd "$BOOT" && timeout -k 3 "$TMO" "$QEMU" -M virt,gic-version=2 \
    -cpu cortex-a53 -m 256M -serial stdio -display none -no-reboot -nic none \
    -kernel kernel.elf ) > "$LOG" 2>&1 < /dev/null || true
grep -a '\[A16\]' "$LOG" | tr -d '\r' | sed 's/^/[rng0-a64]   /' || true
if grep -aqF '[A16] PASS' "$LOG"; then VERDICT=GREEN; else VERDICT=RED; fi
echo "[rng0-a64] arm=$ARM verdict=$VERDICT"
if [ "$VERDICT" = "$EXPECT" ]; then echo "[rng0-a64] PASS: arm=$ARM is $EXPECT as expected."; exit 0; fi
echo "[rng0-a64] FAIL: arm=$ARM is $VERDICT, expected $EXPECT" >&2
exit 1
