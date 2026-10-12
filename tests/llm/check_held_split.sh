#!/bin/bash
# ---------------------------------------------------------------------------
# check_held_split.sh — ROADMAP 0-12: the train/held split is written once.
# st_held_split (arch/common/llm/held_indep.h) is the only place allowed to
# compute "3/4 of the windows train"; student_shell.c and cradle.c had 11 hand
# copies (held-independence.md). RED if a .c / .h under arch/common/llm
# writes the 3/4 again ("* 3 / 4", "*3/4", "3 * x / 4" ...).
#
#   ./check_held_split.sh        exit 0 = OK, 1 = a copy was found
#
# Limits: tests/llm keeps its own copies (14 lines on 2026-10-13, some on
# purpose with other clamps) and is not read. It reads the source text; a split written another way (x - x/4,
# x * 0.75, a shift) is not seen.
# ---------------------------------------------------------------------------
set -u
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
cd "$ROOT" || exit 1
PAT='(\*[[:space:]]*3[[:space:]]*/[[:space:]]*4)|(\b3[[:space:]]*\*[[:space:]]*[A-Za-z_][A-Za-z_0-9.>-]*[[:space:]]*/[[:space:]]*4)'
hits=$(grep -nE "$PAT" arch/common/llm/*.c arch/common/llm/*.h 2>/dev/null |
       grep -v '^arch/common/llm/held_indep\.h:')
if [ -n "$hits" ]; then
    echo "$hits"
    echo "check_held_split: FAIL — a train/held split written by hand; use st_held_split (arch/common/llm/held_indep.h)"
    exit 1
fi
echo "check_held_split: OK — the train/held split is only st_held_split"
