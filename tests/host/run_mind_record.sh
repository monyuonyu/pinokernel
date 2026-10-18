#!/bin/bash
# tests/host/run_mind_record.sh — ROADMAP 1-8 / rule 11 ("the original is the
# body, the weights are the librarian"): a node keeps the VALUE IT WAS GIVEN
# in its record, next to the fast layer's reading of it
# (docs/architecture/30-module/knowledge-survival.md, "v4: the record").
#
# Before this, r3_fq kept only the reading (yhat, a 5-vote frozen read at
# arrival), so a misread fact was the only copy: 1-8 measured receivers whose
# `mind provenance` said ink -> near for a teach of ink -> fresh.
#
# The misread is not reproducible on one node with the plain build (the
# reading of a local teach was right in every order tried), so the test
# build forces it: -DR3_TEST_MISREAD_KEY=15 shifts the arrival reading of
# key 15 (ink) by one value. One node, no networking:
#   teach ink fresh, teach sky blue, provenance, explain ink,
#   teach ink green (a revise), provenance
#   rec-precond  : the forced misread happened (teacher_agree 0 for ink)
#   rec-ink      : provenance "ink" -> "fresh": read "<other>", taught here
#   rec-sky      : provenance "sky" -> "blue": taught here (no read clause
#                  when the reading equals the record)
#   rec-explain  : explain "current: "ink" -> "fresh"" and a "read:" line
#   rec-revise   : after the revise, "ink" -> "green": read "<other>"
# A second node on the same build: teach ink fresh twice
#   rec-reteach  : the second teach says "already believes "fresh"" (the
#                  local re-teach compares the record, not the misread
#                  reading; audit-54 D3 made it a REVISE and stayed green)
# Falsifier: -DR3_RECORD_FROM_READING (the record copies the reading, the old
# behaviour) must turn rec-ink red.
# Output: "[mind-rec] <check> PASS|FAIL", then ALL PASS (exit 0) or RED (exit 1).
# Rebuilds boot/linux* with EXTRA_CFLAGS and cleans it afterwards (the
# run_mind_pause.sh pattern), so a later step rebuilds the plain binary.
set -u
HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
case "$(uname -m)" in
    aarch64|arm64) BOOT="$ROOT/boot/linux" ;;
    *)             BOOT="$ROOT/boot/linux_x86_64" ;;
esac
unset PKERNEL_RELAY PKERNEL_RELAY_HOST PKERNEL_RELAY_PORT PKERNEL_LAN PKERNEL_AUTONET
WORK="$(mktemp -d /tmp/mrec.XXXXXX)"
trap 'rm -rf "$WORK"' EXIT
REDS=""
res() { echo "[mind-rec] $1 $2"; [ "$2" = FAIL ] && REDS="$REDS $1"; return 0; }
chk() {  # <name> <file> <ERE>
    if grep -aqE "$3" "$2"; then res "$1" PASS; else res "$1" FAIL; fi
}
build() {  # <cflags>
    make -C "$BOOT" clean >/dev/null 2>&1
    make -C "$BOOT" EXTRA_CFLAGS="$1" > "$WORK/build.log" 2>&1
}
run() {  # <log>
    local d; d="$(mktemp -d "$WORK/d.XXXXXX")"
    printf 'mind teach ink fresh\nmind teach sky blue\nmind provenance\nmind explain ink\nmind teach ink green\nmind provenance\nexit\n' \
        | timeout 180 env PKERNEL_NODE_ID=1 PKERNEL_PFS_DIR="$d" "$BOOT/p-kernel" > "$1" 2>&1
}
run_reteach() {  # <log>
    local d; d="$(mktemp -d "$WORK/d.XXXXXX")"
    printf 'mind teach ink fresh\nmind teach ink fresh\nmind provenance\nexit\n' \
        | timeout 180 env PKERNEL_NODE_ID=1 PKERNEL_PFS_DIR="$d" "$BOOT/p-kernel" > "$1" 2>&1
}
T='taught here( \(node [0-9]+\))?, prov kept, local seq [0-9]+, (PENDING|RETAINED)'

# ---- the forced misread, plain record ---------------------------------------
L="$WORK/rec.log"
if build "-DR3_TEST_MISREAD_KEY=15"; then
    run "$L"
    chk rec-precond "$L" '^\[mind\]   teacher_agree 0 '
    chk rec-ink     "$L" "^\\[mind\\]   \"ink\" -> \"fresh\": read \"[a-z]+\", $T"
    chk rec-sky     "$L" "^\\[mind\\]   \"sky\" -> \"blue\": $T"
    chk rec-explain "$L" '^\[mind\]   current: "ink" -> "fresh"'
    chk rec-read    "$L" '^\[mind\]   read: "[a-z]+" '
    chk rec-revise  "$L" "^\\[mind\\]   \"ink\" -> \"green\": read \"[a-z]+\", $T"
    R="$WORK/reteach.log"
    run_reteach "$R"
    chk rec-reteach "$R" 'already believes "fresh"'
else
    res build FAIL; cat "$WORK/build.log" | tail -20
fi

# ---- falsifier: the record copies the reading -------------------------------
N="$WORK/nc.log"
if build "-DR3_TEST_MISREAD_KEY=15 -DR3_RECORD_FROM_READING"; then
    run "$N"
    if grep -aqE "^\\[mind\\]   \"ink\" -> \"fresh\": " "$N"; then
        res nc-from-reading FAIL      # the record survived without being kept
    else
        res nc-from-reading PASS      # red as it should be
    fi
else
    res nc-build FAIL
fi
make -C "$BOOT" clean >/dev/null 2>&1

cp "$L" "/tmp/mind_record.log" 2>/dev/null
echo "[mind-rec] log: /tmp/mind_record.log"
if [ -z "$REDS" ]; then echo "[mind-rec] ALL PASS"; exit 0; fi
echo "[mind-rec] RED:$REDS"; exit 1
