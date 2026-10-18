#!/bin/bash
# tests/host/run_stop_console.sh — ROADMAP 0-14 S2 stage 2a: the console's
# stop commands and the first place that obeys the stop
# (docs/architecture/30-module/swarm-stop.md, "stage 2a").
#
# Expectations fixed before the code:
#   S1 a fresh node: "[stop] stopped 0 local 0 signed 0 epoch 0 keys 0"
#   S2 `stop now` stops it (local 1); `mind teach sky blue` is refused and
#      leaves no "sky" fact; `stop resume` clears it; `mind teach ink fresh`
#      is then taught
#   S3 a signed stop pasted at the console (`stop in <hex>` ..., `stop apply`)
#      is refused while the table is empty (rc -3, not stopped)
#   S4 after `stop key <pk>`, the same command with one hex digit changed is
#      refused (rc -4); the right one stops (signed 1, epoch 5); pasting it
#      again is refused (rc -5)
#   S5 a local `stop resume` does not undo a signed stop (still stopped 1);
#      the signed resume (epoch 6) does
# The signed commands come from a test build (-DSWARM_STOP_TEST_SIGN adds
# `stop sign <op> <epoch> <seed-byte>`); S2..S5 run on the plain build.
# Falsifier: -DSWARM_STOP_NO_OBEY (the shell does not look at the stop) must
# turn S2 red.
# Output: "[stop-console] <check> PASS|FAIL", then ALL PASS (exit 0) or RED.
# Rebuilds boot/linux* with EXTRA_CFLAGS and cleans it afterwards.
set -u
HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
case "$(uname -m)" in
    aarch64|arm64) BOOT="$ROOT/boot/linux" ;;
    *)             BOOT="$ROOT/boot/linux_x86_64" ;;
esac
unset PKERNEL_RELAY PKERNEL_RELAY_HOST PKERNEL_RELAY_PORT PKERNEL_LAN PKERNEL_AUTONET
WORK="$(mktemp -d /tmp/stopc.XXXXXX)"
trap 'rm -rf "$WORK"' EXIT
REDS=""
res() { echo "[stop-console] $1 $2"; [ "$2" = FAIL ] && REDS="$REDS $1"; return 0; }
build() {  # <cflags>
    make -C "$BOOT" clean >/dev/null 2>&1
    make -C "$BOOT" EXTRA_CFLAGS="$1" > "$WORK/build.log" 2>&1
}
run() {  # <script> <log>
    local d; d="$(mktemp -d "$WORK/d.XXXXXX")"
    timeout 180 env PKERNEL_NODE_ID=1 PKERNEL_PFS_DIR="$d" "$BOOT/p-kernel" < "$1" > "$2" 2>&1
}
st() {  # <log> <n>: the n-th status line
    grep -a '^\[stop\] stopped ' "$1" | sed -n "${2}p" | tr -d '\r'
}
rc() {  # <log> <n>: the n-th apply rc
    grep -a '^\[stop\] apply rc ' "$1" | sed -n "${2}p" | tr -d '\r' | awk '{print $4}'
}
chunks() {  # <hex>: "stop in" lines of 64 hex digits
    local h="$1"
    while [ -n "$h" ]; do echo "stop in ${h:0:64}"; h="${h:64}"; done
}

# ---- the signed commands, from the test build ---------------------------------
if build "-DSWARM_STOP_TEST_SIGN"; then
    printf 'stop sign 1 5 90\nstop sign 2 6 90\nexit\n' > "$WORK/sign.in"
    run "$WORK/sign.in" "$WORK/sign.log"
else
    res sign-build FAIL; tail -20 "$WORK/build.log"
fi
PK=$(grep -a '^\[stop\] pk ' "$WORK/sign.log" 2>/dev/null | head -1 | tr -d '\r' | awk '{print $3}')
CS=$(grep -a '^\[stop\] cmd ' "$WORK/sign.log" 2>/dev/null | sed -n 1p | tr -d '\r' | awk '{print $3}')
CR=$(grep -a '^\[stop\] cmd ' "$WORK/sign.log" 2>/dev/null | sed -n 2p | tr -d '\r' | awk '{print $3}')
if [ "${#PK}" = 64 ] && [ "${#CS}" = 216 ] && [ "${#CR}" = 216 ]; then res signed-cmds PASS; else res signed-cmds FAIL; fi
# one hex digit changed inside the signature
c=${CS:200:1}; if [ "$c" = 0 ]; then n=1; else n=0; fi
CT="${CS:0:200}${n}${CS:201}"

script() {
    # every `stop` subcommand but `in` prints a status line; the numbers are
    # the status line each one prints
    {
        echo "stop"                                   # status 1
        echo "stop now"                               # status 2
        echo "stop"                                   # status 3
        echo "mind teach sky blue"
        echo "stop resume"                            # status 4
        echo "stop"                                   # status 5
        echo "mind teach ink fresh"
        echo "mind provenance"
        chunks "$CS"; echo "stop apply"               # rc 1 (empty table), status 6
        echo "stop"                                   # status 7
        echo "stop key $PK"                           # status 8
        chunks "$CT"; echo "stop apply"               # rc 2 (bad signature), status 9
        chunks "$CS"; echo "stop apply"               # rc 3 (ok), status 10
        echo "stop"                                   # status 11
        chunks "$CS"; echo "stop apply"               # rc 4 (replay), status 12
        echo "stop resume"                            # status 13
        echo "stop"                                   # status 14
        chunks "$CR"; echo "stop apply"               # rc 5 (signed resume), status 15
        echo "stop"                                   # status 16
        echo "exit"
    } > "$1"
}

check() {  # <log> <prefix>
    local L="$1" p="$2"
    [ "$(st "$L" 1)" = "[stop] stopped 0 local 0 signed 0 epoch 0 keys 0" ] && res "${p}S1" PASS || res "${p}S1" FAIL
    if [ "$(st "$L" 3)" = "[stop] stopped 1 local 1 signed 0 epoch 0 keys 0" ] &&
       grep -aq '^\[stop\] refused: mind teach' "$L" &&
       ! grep -aqE '^\[mind\]   "sky" -> ' "$L" &&
       [ "$(st "$L" 5)" = "[stop] stopped 0 local 0 signed 0 epoch 0 keys 0" ] &&
       grep -aqE '^\[mind\]   "ink" -> "fresh"' "$L"; then res "${p}S2" PASS; else res "${p}S2" FAIL; fi
    [ "$(rc "$L" 1)" = -3 ] && [ "$(st "$L" 7)" = "[stop] stopped 0 local 0 signed 0 epoch 0 keys 0" ] && res "${p}S3" PASS || res "${p}S3" FAIL
    [ "$(rc "$L" 2)" = -4 ] && [ "$(rc "$L" 3)" = 0 ] && [ "$(rc "$L" 4)" = -5 ] &&
      [ "$(st "$L" 11)" = "[stop] stopped 1 local 0 signed 1 epoch 5 keys 1" ] && res "${p}S4" PASS || res "${p}S4" FAIL
    [ "$(st "$L" 14)" = "[stop] stopped 1 local 0 signed 1 epoch 5 keys 1" ] && [ "$(rc "$L" 5)" = 0 ] &&
      [ "$(st "$L" 16)" = "[stop] stopped 0 local 0 signed 0 epoch 6 keys 1" ] && res "${p}S5" PASS || res "${p}S5" FAIL
}

# ---- the plain build -----------------------------------------------------------
L="$WORK/plain.log"
if build ""; then
    script "$WORK/plain.in"; run "$WORK/plain.in" "$L"; check "$L" ""
else
    res build FAIL; tail -20 "$WORK/build.log"
fi

# ---- falsifier: the shell does not obey ------------------------------------------
N="$WORK/nc.log"
if build "-DSWARM_STOP_NO_OBEY"; then
    script "$WORK/nc.in"; run "$WORK/nc.in" "$N"
    if grep -aqE '^\[mind\]   "sky" -> ' "$N"; then res nc-no-obey PASS; else res nc-no-obey FAIL; fi
else
    res nc-build FAIL
fi
make -C "$BOOT" clean >/dev/null 2>&1

cp "$L" /tmp/stop_console.log 2>/dev/null
echo "[stop-console] log: /tmp/stop_console.log"
if [ -z "$REDS" ]; then echo "[stop-console] ALL PASS"; exit 0; fi
echo "[stop-console] RED:$REDS"; exit 1
