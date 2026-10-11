#!/bin/bash
# ---------------------------------------------------------------------------
# run_held_indep.sh — [held-indep] (ROADMAP 0-12): does the held-out set share
# bytes with the train windows? tests/llm/held_indep_test.c over the real
# canonical cradle lesson (cradle.c) with the production split, plus three
# instrument checks (a repeated block is seen, random bytes are clean, a too
# short corpus wraps). Design: docs/architecture/30-module/held-independence.md
#
#   ./run_held_indep.sh        exit 0 = ALL PASS
#
# 2026-10-12: I1 (the canonical lesson) is RED by design of the lesson today
# (its held tail repeats the train filler); not wired into CI until fixed.
# ---------------------------------------------------------------------------
set -u
HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
CC="${CC:-cc}"
CFLAGS="-std=c11 -O1 -Wall -Wextra -ffp-contract=off -Werror=vla"
WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT

echo "[build] held-indep cert (held_indep.c + cradle.c + student.c) ..."
$CC $CFLAGS "$HERE/held_indep_test.c" "$ROOT/arch/common/llm/held_indep.c" \
    "$ROOT/arch/common/llm/cradle.c" "$ROOT/arch/common/llm/student.c" -o "$WORK/hi" \
    || { echo "[build] FAILED"; exit 1; }
"$WORK/hi"
