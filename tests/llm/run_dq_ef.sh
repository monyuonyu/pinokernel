#!/bin/bash
# tests/llm/run_dq_ef.sh — research/delta-quant-ef.md (ROADMAP 1-6).
#
# Builds the hosted kernel twice and runs `mind dq`:
#   default        : prints the gates as measured (P0, G1..G4). G4 (Lemma 8,
#                    |err-(1-phi)| <= 1e-3 on every send) must PASS; G1..G3
#                    are reported as they come out (they are the experiment,
#                    not a build check — a red G is a result, not a broken
#                    cert).
#   -DDQ_BADALPHA  : alpha halved -> G4 must be FAIL (the gate has teeth).
# Exit 0 when the layout check passes, G4 is PASS on the default build and
# FAIL on the negative control; 1 otherwise.
set -u
HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
case "$(uname -m)" in
    aarch64|arm64) BOOT="$ROOT/boot/linux" ;;
    *)             BOOT="$ROOT/boot/linux_x86_64" ;;
esac
WORK="$(mktemp -d /tmp/dqef.XXXXXX)"
FAIL=0

run_arm() {  # <label> <extra cflags>
    make -C "$BOOT" clean >/dev/null 2>&1
    if ! make -C "$BOOT" EXTRA_CFLAGS="$2" > "$WORK/$1.build.log" 2>&1; then
        echo "[dq-ef-cert] $1 BUILD FAILED"; grep -a 'error' "$WORK/$1.build.log" | head -5
        FAIL=1; return 1
    fi
    mkdir -p "$WORK/$1.pfs"
    printf 'mind dq\nexit\n' | timeout 900 env PKERNEL_PFS_DIR="$WORK/$1.pfs" \
        "$BOOT/p-kernel" > "$WORK/$1.log" 2>&1
    grep -a '^\[dq-ef' "$WORK/$1.log" | sed "s/^/[$1] /"
}

run_arm default ""
L="$WORK/default.log"
grep -aq '^\[dq-ef\] layout: .* PASS' "$L" || { echo "[dq-ef-cert] layout check not PASS"; FAIL=1; }
grep -aq '^\[dq-ef-g4\] PASS' "$L"        || { echo "[dq-ef-cert] default G4 not PASS"; FAIL=1; }

run_arm badalpha "-DDQ_BADALPHA"
grep -aq '^\[dq-ef-g4\] FAIL' "$WORK/badalpha.log" \
    && echo "[dq-ef-cert] negative control (alpha/2) correctly RED on G4" \
    || { echo "[dq-ef-cert] negative control did NOT go red on G4 — gate is toothless"; FAIL=1; }

make -C "$BOOT" clean >/dev/null 2>&1
echo "[dq-ef-cert] logs: $WORK"
if [ "$FAIL" = 0 ]; then echo "[dq-ef-cert] PASS"; exit 0; fi
echo "[dq-ef-cert] FAIL"; exit 1
