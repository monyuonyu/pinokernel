#!/bin/bash
# tests/host/run_ed25519_ubsan.sh — ED-SHL: arch/common/ed25519.c must run
# clean under UBSan (car25519 and modL shifted negative values left, undefined
# in C11 6.5.7p4; now an unsigned shift, the same .text on both bare targets).
# Builds tests/host/ed25519_ubsan.c with -O1 -fsanitize=undefined
# -fno-sanitize-recover=all and checks: exit 0, self_test=0, roundtrip_bad=0,
# and the signature digest pinned from master 697758bf (built without UBSan),
# so the change did not move a single signature.
# Falsifier "nc": the same harness on a copy of ed25519.c with the two shifts
# put back to signed (c<<16, carry << 8) must stop with "left shift of
# negative value".
# Output: "[ed25519-ubsan] ... ALL PASS" (exit 0) or RED (1); nc prints
# "NC RED as expected" (exit 0) or NC FAIL (1).
set -u
HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
MODE="${1:-plain}"
WANT_DIGEST=f43d66285f28eca9c3c9c7268124fa07
W="$(mktemp -d /tmp/edubsan.XXXXXX)"
trap 'rm -rf "$W"' EXIT
SRC="$ROOT/arch/common/ed25519.c"
if [ "$MODE" = nc ]; then
    python3 -c 'import sys
s = open(sys.argv[1]).read()
for a, b in (("(i64)((u64)c<<16)", "c<<16"), ("(i64)((u64)carry << 8)", "carry << 8")):
    assert s.count(a) == 1, a
    s = s.replace(a, b)
open(sys.argv[2], "w").write(s)' "$SRC" "$W/ed25519.c" || { echo "[ed25519-ubsan] nc patch FAIL"; exit 1; }
    SRC="$W/ed25519.c"
fi
gcc -O1 -fsanitize=undefined -fno-sanitize-recover=all -I"$ROOT/arch/common/include" \
    "$HERE/ed25519_ubsan.c" "$SRC" -o "$W/t" > "$W/build.log" 2>&1 \
    || { echo "[ed25519-ubsan] build FAIL"; cat "$W/build.log"; exit 1; }
"$W/t" > "$W/out" 2>&1; rc=$?
cat "$W/out"
if [ "$MODE" = nc ]; then
    if [ "$rc" != 0 ] && grep -q 'left shift of negative value' "$W/out"; then
        echo "[ed25519-ubsan] NC RED as expected"; exit 0; fi
    echo "[ed25519-ubsan] NC FAIL: the signed shifts ran clean (rc=$rc)"; exit 1
fi
if [ "$rc" = 0 ] && grep -q "self_test=0 roundtrip_bad=0 digest=$WANT_DIGEST\$" "$W/out"; then
    echo "[ed25519-ubsan] ALL PASS"; exit 0; fi
echo "[ed25519-ubsan] RED (rc=$rc, want digest $WANT_DIGEST)"; exit 1
