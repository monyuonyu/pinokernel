#!/bin/bash
# tests/host/run_stop_sleep.sh — ROADMAP 0-14 S2 stage 2d: a stopped node
# does not learn in its sleep either (docs/architecture/30-module/
# swarm-stop.md, "2d"). Stages 2a-2c stopped the shell, galaxy and network
# teach mouths; a fact already taught kept being distilled into rw[] by the
# DMN idle work while the node was stopped (measured on master 25ea7712:
# 9 "[dmn] sleep: distilled" rounds inside a 25 s stop).
#
# Expectations fixed before the code (one hosted node, timed console input):
#   (Z0 "the sleep runs before the stop" was fixed here too and dropped
#    after the first runs: on all three trees - test-first, head, nc - the
#    first round starts more than 14 s after the teach, 0 lines before the
#    stop. What Z0 was for - the window covers sleep time and a fact is
#    pending in it - is shown by the test-first tree and the nc build, which
#    distil 11 rounds inside the same window, and by Z3: the fact is still
#    pending after the window, so the stopped node had work to skip.)
#   Z1 `stop now` stops it (status "stopped 1 local 1")
#   Z2 between the status line 2 s after the stop and the one 20 s later:
#      no "[dmn] sleep:" line
#   Z3 after `stop resume` the sleep runs again and "sky" ends RETAINED in
#      `mind provenance` (the stop paused the learning, it did not drop it)
# Falsifier: -DSWARM_STOP_NO_SLEEP_OBEY (the idle work does not look at the
# stop) must turn Z2 red.
# Only the R3 in-context track prints a line per round; the engram replay,
# the student distillation and ga_step share the same gate but are not
# observed here.
# Output: "[stop-sleep] <check> PASS|FAIL", then ALL PASS (exit 0) or RED (1).
# Arg: "nc" builds the falsifier and expects Z2 red (exit 0 when so).
# Rebuilds boot/linux* with EXTRA_CFLAGS and cleans it afterwards.
set -u
HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
case "$(uname -m)" in
    aarch64|arm64) BOOT="$ROOT/boot/linux" ;;
    *)             BOOT="$ROOT/boot/linux_x86_64" ;;
esac
MODE="${1:-plain}"
CF=""
[ "$MODE" = nc ] && CF="-DSWARM_STOP_NO_SLEEP_OBEY"
unset PKERNEL_RELAY PKERNEL_RELAY_HOST PKERNEL_RELAY_PORT PKERNEL_LAN PKERNEL_AUTONET
WORK="$(mktemp -d /tmp/stops.XXXXXX)"
trap 'rm -rf "$WORK"; make -C "$BOOT" clean >/dev/null 2>&1' EXIT
REDS=""
res() { echo "[stop-sleep] $1 $2"; [ "$2" = FAIL ] && REDS="$REDS $1"; return 0; }

make -C "$BOOT" clean >/dev/null 2>&1
if ! make -C "$BOOT" EXTRA_CFLAGS="$CF" > "$WORK/build.log" 2>&1; then
    tail -20 "$WORK/build.log"; echo "[stop-sleep] RED: build"; exit 1
fi

mkdir "$WORK/pfs"
{ sleep 3;  echo "mind teach sky blue"
  sleep 12; echo "stop now"
  sleep 2;  echo "stop"                      # status line A (window start)
  sleep 20; echo "stop"                      # status line B (window end)
  echo "stop resume"                         # status line C (resume prints one)
  echo "stop"
  sleep 40; echo "mind provenance"
  sleep 2;  echo "exit"; } |
  timeout 150 env PKERNEL_NODE_ID=1 PKERNEL_PFS_DIR="$WORK/pfs" "$BOOT/p-kernel" > "$WORK/n.raw" 2>&1
tr -d '\r' < "$WORK/n.raw" > "$WORK/n.log"
L="$WORK/n.log"

# line numbers of the status lines (A = 2nd, B = 3rd, C = 4th; `stop now`
# and `stop resume` print one each)
ln() { grep -an '^\[stop\] stopped \|^p-kernel> \[stop\] stopped ' "$L" | sed -n "${1}p" | cut -d: -f1; }
A=$(ln 2); B=$(ln 3); C=$(ln 4)
sl() { awk -v a="$1" -v b="$2" 'NR > a && NR < b && /\[dmn\] sleep:/' "$L" | grep -c .; }
echo "[stop-sleep] status lines at $A $B $C; sleep lines before A $(sl 0 "${A:-0}"), A..B $(sl "${A:-0}" "${B:-0}"), after C $(sl "${C:-0}" 999999)"

[ -n "$A" ] && sed -n "${A}p" "$L" | grep -q 'stopped 1 local 1' && res Z1-stopped PASS || res Z1-stopped FAIL
[ -n "$A" ] && [ -n "$B" ] && [ "$(sl "$A" "$B")" = 0 ] && res Z2-no-sleep PASS || res Z2-no-sleep FAIL
if [ -n "$C" ] && sed -n "${C}p" "$L" | grep -q 'stopped 0 local 0' &&
   [ "$(sl "$C" 999999)" -ge 1 ] && grep -aqE '"sky" -> "blue".*RETAINED' "$L"; then
    res Z3-after-resume PASS
else
    res Z3-after-resume FAIL
fi
cp "$L" /tmp/stop_sleep.log 2>/dev/null
if [ "$MODE" = nc ]; then
    case "$REDS" in
        *Z2-no-sleep*) echo "[stop-sleep] NC RED as expected:$REDS"; exit 0 ;;
        *) echo "[stop-sleep] NC FAIL: the falsifier stayed green ($REDS)"; exit 1 ;;
    esac
fi
if [ -z "$REDS" ]; then echo "[stop-sleep] ALL PASS"; exit 0; fi
echo "[stop-sleep] RED:$REDS"; exit 1
