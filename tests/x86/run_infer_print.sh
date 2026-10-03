#!/bin/sh
# tests/x86/run_infer_print.sh — the "[ai]  infer: ... (t=<n>C)" line on bare
# x86 (arch/common/pipeline.c). Boots the default image, pushes three sensor
# frames over the serial shell (25, -5 and 38 C) and checks that each infer
# line reads exactly "(t=25C)", "(t=-5C)", "(t=38C)". Before 191f752b the
# digits were passed to tm_putstring without a 0 terminator and the line
# carried stack bytes ("t=-5<junk>C", measured 2026-09-28).
# The boot check alone depends on the stack layout: audit-18 put back only
# one of the two lines (either one) and the three lines still came out clean
# (the next stack byte happened to be 0). So a source check comes first: no
# tm_putstring(&<plain variable>) anywhere in arch/ or boot/ — the address of
# a lone variable is not a 0-terminated string (&buf[i] is not matched).
# Since 2026-10-04 any cast and any blanks/tabs are matched too: audit-19 put
# a line back as tm_putstring((const UB *)&u) and the old pattern, which only
# knew "(UB *)" and spaces, let it through. Not matched (by design): &s.m
# (a char-array member is a valid string), wrappers around tm_putstring.
# The pattern is first checked on fixed samples, so a broken edit of it
# fails here instead of silently matching nothing.
# Exit 0 = no such call and three clean lines, 1 = not (or build/boot failed).
set -eu
HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
PAT='tm_putstring\([[:space:]]*(\([^()]*\)[[:space:]]*)?&[[:space:]]*[A-Za-z_][A-Za-z0-9_]*[[:space:]]*\)'
SELF="$(mktemp "${TMPDIR:-/tmp}/inferpat.XXXXXX")"
printf '%s\n' 'tm_putstring(&u);' 'tm_putstring((UB *)&u);' \
    'tm_putstring((const UB *)&u);' 'tm_putstring(	(UB*)	& u );' > "$SELF"
NHIT=$(grep -cE "$PAT" "$SELF" || true)
printf '%s\n' 'tm_putstring((UB *)&b[i]);' 'tm_putstring((UB *)buf);' \
    'tm_putstring((UB *)"x");' 'tm_putstring((UB *)&s.name);' > "$SELF"
NMISS=$(grep -cE "$PAT" "$SELF" || true)
rm -f "$SELF"
if [ "$NHIT" != 4 ] || [ "$NMISS" != 0 ]; then
    echo "[infer-print] FAIL: pattern self-check (must-match $NHIT/4, must-not-match $NMISS/0)" >&2
    exit 1
fi
SRC=$(grep -rnE "$PAT" --include='*.c' "$ROOT/arch" "$ROOT/boot" || true)
if [ -n "$SRC" ]; then
    echo "$SRC" | sed 's/^/[infer-print]   source: /'
    echo "[infer-print] FAIL: tm_putstring is given the address of a lone variable (not 0-terminated)" >&2
    exit 1
fi
echo "[infer-print] source check: no tm_putstring(&<variable>)"
BOOT="$ROOT/boot/x86"
QEMU="${QEMU:-qemu-system-x86_64}"
WAIT="${INFER_BOOT_WAIT:-30}"
SCRATCH="$(mktemp -d "${TMPDIR:-/tmp}/inferprint.XXXXXX")"
trap 'rm -rf "$SCRATCH"' EXIT
( cd "$BOOT" && make clean ) > "$SCRATCH/build.log" 2>&1 || true
if ! ( cd "$BOOT" && make bootloader.bin ) >> "$SCRATCH/build.log" 2>&1; then
    tail -40 "$SCRATCH/build.log" >&2
    echo "[infer-print] FATAL: build failed" >&2; exit 1
fi
LOG="$SCRATCH/serial.log"
( sleep "$WAIT"; printf 'sensor 25 60 1013 300\r'; sleep 3
  printf 'sensor -5 60 1013 300\r'; sleep 3
  printf 'sensor 38 90 990 10\r'; sleep 6 ) \
  | ( cd "$BOOT" && timeout -k 3 $((WAIT + 20)) "$QEMU" -m 256 -kernel bootloader.bin \
        -serial stdio -cpu qemu64 -display none -no-reboot ) > "$LOG" 2>&1 || true
N=$(grep -ac '\[ai\]  infer:' "$LOG" || true)
ok=0
for t in 25 -5 38; do
    if grep -aE "\[ai\]  infer: [A-Za-z]+ +\(t=$t""C\)" "$LOG" | tr -d '\r' | grep -aqE "\(t=$t""C\)$"; then
        ok=$((ok + 1))
    fi
done
grep -a '\[ai\]  infer:' "$LOG" | tr -d '\r' | cat -v | sed 's/^/[infer-print]   /'
echo "[infer-print] infer-lines=$N clean=$ok/3"
if [ "$ok" -eq 3 ]; then echo "[infer-print] PASS"; exit 0; fi
echo "[infer-print] FAIL" >&2
exit 1
