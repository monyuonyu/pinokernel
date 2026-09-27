#!/bin/sh
# tests/x86/run_cpu_budget.sh — CPU budget watchdog, stage 1 (log only)
# (docs/architecture/30-module/cpu-budget.md; ROADMAP 0-1, second half).
#
# Builds boot/x86 with the T18 probe (arch/x86/selftest.c, -DPK_CPU_BUDGET_TEST)
# and boots it under QEMU. Three arms:
#   spin      a pri-20 ring0 task spins ~3 s with no kernel call.
#             Expect exactly one "[budget] task <id> ran <ms> ms" line.
#   yield     (-DPK_CPU_BUDGET_YIELD) the task sleeps 10 ms every loop.
#             Expect no "[budget]" line.
#   nobudget  (-DPK_NO_CPU_BUDGET) the spin arm with the watchdog compiled
#             out. Expect no "[budget]" line (negative control).
# Every arm also needs "[T18] DONE" (the probe itself ran to the end).
#
# Usage: ARM=spin|yield|nobudget sh tests/x86/run_cpu_budget.sh
# Exit 0 = the arm behaved as expected, 1 = not (or build/boot failed).
set -eu
HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
BOOT="$ROOT/boot/x86"
ARM="${ARM:-}"
QEMU="${QEMU:-qemu-system-x86_64}"
TMO="${BUDGET_BOOT_TIMEOUT:-25}"
case "$ARM" in
    spin)     DEFS="-DPK_CPU_BUDGET_TEST";                         WANT=1 ;;
    yield)    DEFS="-DPK_CPU_BUDGET_TEST -DPK_CPU_BUDGET_YIELD";   WANT=0 ;;
    nobudget) DEFS="-DPK_CPU_BUDGET_TEST -DPK_NO_CPU_BUDGET";      WANT=0 ;;
    *) echo "[cpu-budget] FATAL: set ARM=spin, yield or nobudget" >&2; exit 1 ;;
esac
SCRATCH="$(mktemp -d "${TMPDIR:-/tmp}/cpubudget.XXXXXX")"
trap 'rm -rf "$SCRATCH"' EXIT
( cd "$BOOT" && make clean ) > "$SCRATCH/build.log" 2>&1 || true
if ! ( cd "$BOOT" && make EXTRA_CFLAGS="$DEFS" bootloader.bin ) >> "$SCRATCH/build.log" 2>&1; then
    tail -40 "$SCRATCH/build.log" >&2
    echo "[cpu-budget] FATAL: build failed" >&2; exit 1
fi
LOG="$SCRATCH/serial.log"; : > "$LOG"
( cd "$BOOT" && timeout "$TMO" "$QEMU" -m 256 -kernel bootloader.bin \
    -serial file:"$LOG" -cpu qemu64 -display none -no-reboot ) > /dev/null 2>&1 || true
N=$(grep -ac '^\[budget\] task' "$LOG" || true)
DONE=$(grep -ac '\[T18\] DONE' "$LOG" || true)
grep -a '\[T18\]\|^\[budget\]' "$LOG" | sed 's/^/[cpu-budget]   /'
echo "[cpu-budget] arm=$ARM budget-lines=$N want=$WANT t18-done=$DONE"
if [ "$DONE" -ge 1 ] && [ "$N" -eq "$WANT" ]; then
    echo "[cpu-budget] PASS: arm=$ARM"; exit 0
fi
echo "[cpu-budget] FAIL: arm=$ARM" >&2
exit 1
