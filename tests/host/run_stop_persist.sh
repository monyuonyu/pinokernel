#!/bin/bash
# tests/host/run_stop_persist.sh — ROADMAP 0-14 S2: the stop survives a
# restart (docs/architecture/30-module/swarm-stop.md, "persist").
#
# One node, one PKERNEL_PFS_DIR, started five times. Expectations fixed
# before the code:
#   R0 run A on an empty dir: "[stop] stopped 0 local 0 signed 0 epoch 0 keys 0"
#      and no "[stop] restored" line
#   R1 run A puts a key, stops locally and applies a signed stop (epoch 5);
#      run B's first status is "stopped 1 local 1 signed 1 epoch 5 keys 1"
#   R2 in run B `mind teach sky blue` is refused and leaves no "sky" fact
#   R3 in run B the same signed stop again is refused as a replay (rc -5)
#   R4 in run B a local resume leaves the signed stop, and the signed resume
#      (epoch 6) is accepted (rc 0) because the key table came back too:
#      "stopped 0 local 0 signed 0 epoch 6 keys 1"
#   R5 run C's first status is "stopped 0 local 0 signed 0 epoch 6 keys 1"
#   R6 one byte of the state file changed: run D says so on a line starting
#      "[stop] state file" and starts stopped here (fail-closed):
#      "stopped 1 local 1 signed 0 epoch 0 keys 0"; `stop resume` then gives
#      "stopped 0 local 0 signed 0 epoch 0 keys 0"
#   R7 run E (after D wrote a good file again): "stopped 0 local 0 signed 0
#      epoch 0 keys 0" and a "[stop] restored" line
# The signed commands come from a test build (-DSWARM_STOP_TEST_SIGN).
# Falsifier: -DSWARM_STOP_NO_PERSIST (nothing written, nothing read) must turn
# R1 red.
# Output: "[stop-persist] <check> PASS|FAIL", then ALL PASS (exit 0) or RED.
# Rebuilds boot/linux* with EXTRA_CFLAGS and cleans it afterwards.
set -u
HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
case "$(uname -m)" in
    aarch64|arm64) BOOT="$ROOT/boot/linux" ;;
    *)             BOOT="$ROOT/boot/linux_x86_64" ;;
esac
unset PKERNEL_RELAY PKERNEL_RELAY_HOST PKERNEL_RELAY_PORT PKERNEL_LAN PKERNEL_AUTONET
WORK="$(mktemp -d /tmp/stopp.XXXXXX)"
trap 'rm -rf "$WORK"' EXIT
REDS=""
res() { echo "[stop-persist] $1 $2"; [ "$2" = FAIL ] && REDS="$REDS $1"; return 0; }
build() {  # <cflags>
    make -C "$BOOT" clean >/dev/null 2>&1
    make -C "$BOOT" EXTRA_CFLAGS="$1" > "$WORK/build.log" 2>&1
}
run() {  # <dir> <script> <log>
    timeout 180 env PKERNEL_NODE_ID=1 PKERNEL_PFS_DIR="$1" "$BOOT/p-kernel" < "$2" > "$3" 2>&1
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
    mkdir -p "$WORK/sd"
    printf 'stop sign 1 5 90\nstop sign 2 6 90\nexit\n' > "$WORK/sign.in"
    run "$WORK/sd" "$WORK/sign.in" "$WORK/sign.log"
else
    res sign-build FAIL; tail -20 "$WORK/build.log"
fi
PK=$(grep -a '^\[stop\] pk ' "$WORK/sign.log" 2>/dev/null | head -1 | tr -d '\r' | awk '{print $3}')
CS=$(grep -a '^\[stop\] cmd ' "$WORK/sign.log" 2>/dev/null | sed -n 1p | tr -d '\r' | awk '{print $3}')
CR=$(grep -a '^\[stop\] cmd ' "$WORK/sign.log" 2>/dev/null | sed -n 2p | tr -d '\r' | awk '{print $3}')
if [ "${#PK}" = 64 ] && [ "${#CS}" = 216 ] && [ "${#CR}" = 216 ]; then res signed-cmds PASS; else res signed-cmds FAIL; fi

{
    echo "stop"                                       # A status 1
    echo "stop key $PK"                               # A status 2
    echo "stop now"                                   # A status 3
    chunks "$CS"; echo "stop apply"                   # A rc 1, status 4
    echo "stop"                                       # A status 5
    echo "exit"
} > "$WORK/a.in"
{
    echo "stop"                                       # B status 1
    echo "mind teach sky blue"
    echo "mind provenance"
    chunks "$CS"; echo "stop apply"                   # B rc 1 (replay), status 2
    echo "stop resume"                                # B status 3
    chunks "$CR"; echo "stop apply"                   # B rc 2, status 4
    echo "stop"                                       # B status 5
    echo "exit"
} > "$WORK/b.in"
printf 'stop\nexit\n' > "$WORK/c.in"
printf 'stop\nstop resume\nstop\nexit\n' > "$WORK/d.in"
printf 'stop\nexit\n' > "$WORK/e.in"

scenario() {  # <prefix>: five runs on one dir
    local p="$1" D="$WORK/$1-dir"
    rm -rf "$D"; mkdir -p "$D"
    run "$D" "$WORK/a.in" "$WORK/$p-a.log"
    run "$D" "$WORK/b.in" "$WORK/$p-b.log"
    run "$D" "$WORK/c.in" "$WORK/$p-c.log"
    if [ -f "$D/swarm-stop.st" ]; then
        python3 -c 'import sys; p=sys.argv[1]; b=bytearray(open(p,"rb").read()); b[14]^=0x01; open(p,"wb").write(b)' "$D/swarm-stop.st"
    fi
    run "$D" "$WORK/d.in" "$WORK/$p-d.log"
    run "$D" "$WORK/e.in" "$WORK/$p-e.log"
}

check() {  # <prefix>
    local p="$1" A="$WORK/$1-a.log" B="$WORK/$1-b.log" C="$WORK/$1-c.log" D="$WORK/$1-d.log" E="$WORK/$1-e.log"
    [ "$(st "$A" 1)" = "[stop] stopped 0 local 0 signed 0 epoch 0 keys 0" ] && ! grep -aq '^\[stop\] restored' "$A" &&
      res "${p}R0" PASS || res "${p}R0" FAIL
    [ "$(rc "$A" 1)" = 0 ] && [ "$(st "$A" 5)" = "[stop] stopped 1 local 1 signed 1 epoch 5 keys 1" ] &&
      [ "$(st "$B" 1)" = "[stop] stopped 1 local 1 signed 1 epoch 5 keys 1" ] && res "${p}R1" PASS || res "${p}R1" FAIL
    grep -aq '^\[stop\] refused: mind teach' "$B" && ! grep -aqE '^\[mind\]   "sky" -> ' "$B" &&
      res "${p}R2" PASS || res "${p}R2" FAIL
    [ "$(rc "$B" 1)" = -5 ] && res "${p}R3" PASS || res "${p}R3" FAIL
    [ "$(st "$B" 3)" = "[stop] stopped 1 local 0 signed 1 epoch 5 keys 1" ] && [ "$(rc "$B" 2)" = 0 ] &&
      [ "$(st "$B" 5)" = "[stop] stopped 0 local 0 signed 0 epoch 6 keys 1" ] && res "${p}R4" PASS || res "${p}R4" FAIL
    [ "$(st "$C" 1)" = "[stop] stopped 0 local 0 signed 0 epoch 6 keys 1" ] && res "${p}R5" PASS || res "${p}R5" FAIL
    grep -aq '^\[stop\] state file' "$D" && [ "$(st "$D" 1)" = "[stop] stopped 1 local 1 signed 0 epoch 0 keys 0" ] &&
      [ "$(st "$D" 3)" = "[stop] stopped 0 local 0 signed 0 epoch 0 keys 0" ] && res "${p}R6" PASS || res "${p}R6" FAIL
    [ "$(st "$E" 1)" = "[stop] stopped 0 local 0 signed 0 epoch 0 keys 0" ] && grep -aq '^\[stop\] restored' "$E" &&
      res "${p}R7" PASS || res "${p}R7" FAIL
}

# ---- the plain build -----------------------------------------------------------
if build ""; then
    scenario plain; check plain
else
    res build FAIL; tail -20 "$WORK/build.log"
fi

# ---- falsifier: nothing is written or read ---------------------------------------
if build "-DSWARM_STOP_NO_PERSIST"; then
    scenario nc
    if [ "$(st "$WORK/nc-b.log" 1)" != "[stop] stopped 1 local 1 signed 1 epoch 5 keys 1" ]; then
        res nc-no-persist PASS; else res nc-no-persist FAIL; fi
else
    res nc-build FAIL
fi
make -C "$BOOT" clean >/dev/null 2>&1

for x in a b c d e; do cp "$WORK/plain-$x.log" "/tmp/stop_persist_$x.log" 2>/dev/null; done
echo "[stop-persist] logs: /tmp/stop_persist_[a-e].log"
if [ -z "$REDS" ]; then echo "[stop-persist] ALL PASS"; exit 0; fi
echo "[stop-persist] RED:$REDS"; exit 1
