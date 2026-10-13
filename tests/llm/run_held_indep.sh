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
# 2026-10-12: I1 (the canonical lesson) was RED at 3e3370d0 (108 of 170 held
# k-grams in train: the held tail repeated the train filler); the next commit
# gives the tail a held-only sentence. CI: llm-engine-certs (BLOCKING), with
# check_held_split.sh (2026-10-13, feat/held-split).
# ---------------------------------------------------------------------------
set -u
HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
CC="${CC:-cc}"
CFLAGS="-std=c11 -O1 -Wall -Wextra -ffp-contract=off -Werror=vla"
WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT
FAIL=0

echo "[build] held-indep cert (held_indep.c + cradle.c + student.c) ..."
$CC $CFLAGS "$HERE/held_indep_test.c" "$ROOT/arch/common/llm/held_indep.c" \
    "$ROOT/arch/common/llm/cradle.c" "$ROOT/arch/common/llm/student.c" -o "$WORK/hi" \
    || { echo "[build] FAILED"; exit 1; }
"$WORK/hi" || FAIL=1

# [held-seen] (0-12 step 4): the record wired into the real student_shell.c.
# -DSEEN_DISABLE (records nothing) must go RED, or the cert has no teeth.
SEEN="$HERE/student_seen_test.c $ROOT/arch/common/llm/held_indep.c $ROOT/arch/common/llm/student.c"
SEEN="$SEEN $ROOT/arch/common/llm/cradle.c $ROOT/arch/common/llm/dlb.c $HERE/held_seen_stubs.c"
echo "[build] held-seen cert (student_shell.c #included) ..."
# shellcheck disable=SC2086
$CC $CFLAGS $SEEN -lm -o "$WORK/seen" || { echo "[build] FAILED"; exit 1; }
# shellcheck disable=SC2086
$CC $CFLAGS -DSEEN_DISABLE $SEEN -lm -o "$WORK/seen_off" || { echo "[build] FAILED"; exit 1; }
"$WORK/seen" || FAIL=1
if "$WORK/seen_off" > "$WORK/seen_off.txt"; then
    echo "[held-seen] FAIL falsifier: -DSEEN_DISABLE stayed green"; FAIL=1
else
    grep -E 'FAIL S[0-9]' "$WORK/seen_off.txt" | sed 's/^/[held-seen] falsifier (expected RED): /'
fi
[ "$FAIL" = 0 ] && echo "[held-indep+seen] ALL PASS" || echo "[held-indep+seen] RED"
exit "$FAIL"
