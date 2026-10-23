#!/bin/bash
# ---------------------------------------------------------------------------
# run_selfc_save_run.sh — `selfc save <name>` then `selfc run <name>` on ONE
# node must germinate the unit (hosted Linux x86_64, needs libtcc).
#
# Why: since selfc-ring3 v1 (725cafa7) the unit source is read from the p-fs
# ref "unit/<name>" (selfc_head_seq, selfc_unit_id, selfc_run_germ), but
# `selfc save` wrote the ref "<name>". So save -> run never connected, not
# even on the same node ("has no version in p-fs yet"), the signed manifest
# was made for seq 0, and samples/11_distributed/run_selfc_propagate.sh was
# red (found 2026-10-24, BACKLOG D5-e).
#
# Checks (lines, not exit codes):
#   save-ok    "[selfc] demo source ... saved as p-fs object"
#   head-seq   no "has no version in p-fs yet" after the save
#   compiled   "compiled at runtime inside the kernel"
#   signed     "signed unit manifest 'unitsig/<name>' published"
# Without libtcc the kernel prints its stub and the test says SKIP (rc 0).
# ---------------------------------------------------------------------------
set -u
HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
BOOT="$ROOT/boot/linux_x86_64"
[ -x "$BOOT/p-kernel" ] || make -C "$BOOT" >/dev/null || exit 1
WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT

{ sleep 4; echo "selfc save genome.c"; sleep 3; echo "selfc run genome.c"; sleep 6;
  echo "exit"; } |
  timeout 60 env PKERNEL_NODE_ID=1 PKERNEL_PFS_DIR="$WORK" "$BOOT/p-kernel" > "$WORK/n.log" 2>&1
L="$WORK/n.log"
tr -d '\r' < "$L" | grep -a '\[selfc' | sed 's/^/  | /'

if grep -aq 'no libtcc' "$L"; then
    echo "[selfc-save-run] SKIP (this build has no libtcc)"; exit 0
fi
fail=0
res() { echo "  $2  $1"; [ "$2" = PASS ] || fail=1; }
grep -aq 'saved as p-fs object' "$L" && res save-ok PASS || res save-ok FAIL
! grep -aq 'has no version in p-fs yet' "$L" && res head-seq PASS || res head-seq FAIL
grep -aq 'compiled at runtime inside the kernel' "$L" && res compiled PASS || res compiled FAIL
grep -aq "signed unit manifest 'unitsig/genome.c' published" "$L" && res signed PASS || res signed FAIL
if [ "$fail" = 0 ]; then echo "[selfc-save-run] ALL PASS"; else echo "[selfc-save-run] FAIL"; fi
exit "$fail"
