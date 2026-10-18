#!/bin/bash
# ---------------------------------------------------------------------------
# run_b1_held.sh — [b1-held] (ROADMAP B1): bits per byte of the student on an
# independent held text (tests/llm/b1_held.txt), tests/llm/b1_held_test.c.
# Design and the expectations fixed before the first run:
# docs/architecture/30-module/held-independence.md, section "B1".
#
#   ./run_b1_held.sh [tier]      exit 0 = ALL PASS (tier 0 = S, the default)
#
#   1. the cert itself (B1..B4), run twice: the bpb line must be identical
#   2. -DB1_LEAK (the held text's first half mixed into training) must go RED
#      on B3, or the independence check has no teeth
#   3. no file under arch/ or boot/ names b1_held (the kernel cannot read the
#      held text, so it cannot select weights with it); the same grep on a
#      planted copy must see the name
# ---------------------------------------------------------------------------
set -u
HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
CC="${CC:-cc}"
CFLAGS="-std=c11 -O1 -Wall -Wextra -ffp-contract=off -Werror=vla"
TIER="${1:-0}"
WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT
FAIL=0
SRC="$HERE/b1_held_test.c $ROOT/arch/common/llm/held_indep.c $ROOT/arch/common/llm/cradle.c $ROOT/arch/common/llm/student.c"

echo "[build] b1-held cert (held_indep.c + cradle.c + student.c) ..."
# shellcheck disable=SC2086
$CC $CFLAGS $SRC -lm -o "$WORK/b1" || { echo "[build] FAILED"; exit 1; }
# shellcheck disable=SC2086
$CC $CFLAGS -DB1_LEAK $SRC -lm -o "$WORK/b1_leak" || { echo "[build] FAILED"; exit 1; }

"$WORK/b1" "$HERE/b1_held.txt" "$TIER" | tee "$WORK/run1.log"
grep -q '^\[b1-held\] ALL PASS' "$WORK/run1.log" || FAIL=1
"$WORK/b1" "$HERE/b1_held.txt" "$TIER" > "$WORK/run2.log"
if [ "$(grep 'bpb before' "$WORK/run1.log")" = "$(grep 'bpb before' "$WORK/run2.log")" ] && grep -q 'bpb before' "$WORK/run1.log"; then
    echo "[b1-held] PASS deterministic (two runs print the same bpb line)"
else
    echo "[b1-held] FAIL two runs differ or print no bpb line"; FAIL=1
fi

"$WORK/b1_leak" "$HERE/b1_held.txt" "$TIER" > "$WORK/leak.log"
grep 'trained:\|bpb before' "$WORK/leak.log"
if grep -q '^\[b1-held\] FAIL B3' "$WORK/leak.log"; then
    echo "[b1-held] PASS negative control: training on the held text turns B3 red"
else
    echo "[b1-held] FAIL negative control: -DB1_LEAK did not turn B3 red"; FAIL=1
fi

names() { grep -rl 'b1_held' "$@" 2>/dev/null; }
if [ -n "$(names "$ROOT/arch" "$ROOT/boot")" ]; then
    echo "[b1-held] FAIL the kernel sources name the held text:"; names "$ROOT/arch" "$ROOT/boot"; FAIL=1
else
    echo "[b1-held] PASS no file under arch/ or boot/ names b1_held"
fi
mkdir -p "$WORK/plant"; echo '#include "b1_held.h"' > "$WORK/plant/x.c"
[ -n "$(names "$WORK/plant")" ] || { echo "[b1-held] FAIL the name grep did not see a planted reference"; FAIL=1; }

if [ "$FAIL" = 0 ]; then echo "[b1-held+nc] ALL PASS"; else echo "[b1-held+nc] RED"; fi
exit "$FAIL"
