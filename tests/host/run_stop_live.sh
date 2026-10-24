#!/bin/bash
# tests/host/run_stop_live.sh — ROADMAP 0-14 S2 [live]: one signed command
# typed into one node stops the whole swarm; a command whose signer is not in
# a node's stop table stops nobody there (docs/architecture/30-module/
# swarm-stop.md, "[live]"). Until now a signed command was accepted only at
# the console it was pasted into; nothing carried it to the peers.
#
# Expectations fixed before the code (three hosted nodes over a relay, A = 1,
# B = 2, C = 3). Every node's table has K1 (seed 90); only A's also has K2
# (seed 91). The commands come from the test build (-DSWARM_STOP_TEST_SIGN).
#   L0 the three nodes are up (mind_net_task up)
#   L1 A applies a K2 stop (epoch 5): A is stopped; 30 s later B and C are not
#      ("stopped 0") and each logged the refusal ("[stop] network command
#      refused")
#   L2 A applies a K1 stop (epoch 7): within 60 s B and C log "[stop] network
#      command accepted" and their status is "stopped 1 local 0 signed 1"
#   L4 (audit-75: sending the command once kept L0-L3 green) after L2, a
#      fourth node D (K1 in its table) starts: within 60 s of its key it logs
#      "[stop] network command accepted" and is "stopped 1 local 0 signed 1
#      epoch 7" - only the re-drive can reach a node that was not up yet
#   L5 (audit-77: the write after a network command was outside every test)
#      after L4, B is killed (-9) and started again on its own dir with no
#      network: it logs "[stop] restored" and its first status is "stopped 1
#      local 0 signed 1 epoch 7 keys 1" - the stop it took from the network
#      survives a restart. Then B starts again on the network for L3
#   L3 A applies a K1 resume (epoch 8): within 60 s B, C and D are "stopped 0"
#   L6 (audit-77/78: restoring the re-drive command after a restart was
#      outside every test) after L3, A, C, D and B are killed (-9); B starts
#      again on the network on its own dir, then a new node E (K1 in its
#      table) starts: within 60 s of its key E logs "[stop] network command
#      accepted" and is "stopped 0 local 0 signed 0 epoch 8" - B is the only
#      node up, so only the command B restored from swarm-stop.st can reach E
# Falsifiers: -DSWARM_STOP_NO_FORWARD (an accepted command is not sent on)
# must turn L2 red; -DSWARM_STOP_NO_AUTH (no stop-table check) turns L1 red,
# -DSWARM_STOP_NET_NO_PERSIST (a network command is not written) turns L5
# red, -DSWARM_STOP_NO_RESTORE_FWD (the restored node does not re-drive)
# turns L6 red (local measurements, not in CI).
# Output: "[stop-live] <check> PASS|FAIL", then ALL PASS (exit 0) or RED (1).
# Arg: "nc" builds -DSWARM_STOP_NO_FORWARD and expects L2 red; "noauth"
# builds -DSWARM_STOP_NO_AUTH and expects L1 red; "netnp" builds
# -DSWARM_STOP_NET_NO_PERSIST and expects L5 red; "late" builds
# -DSWARM_STOP_SEND_ONCE (an accepted command is sent once, never again) and
# expects L4 red with L2 green (exit 0 when so); "nofwd" builds
# -DSWARM_STOP_NO_RESTORE_FWD and expects L6 red with L3 green.
# Rebuilds boot/linux* with EXTRA_CFLAGS and cleans it afterwards.
# Env: SLV_PORT (default 7491).
set -u
HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
case "$(uname -m)" in
    aarch64|arm64) BOOT="$ROOT/boot/linux" ;;
    *)             BOOT="$ROOT/boot/linux_x86_64" ;;
esac
MODE="${1:-plain}"
CF="-DSWARM_STOP_TEST_SIGN"
[ "$MODE" = nc ] && CF="$CF -DSWARM_STOP_NO_FORWARD"
[ "$MODE" = noauth ] && CF="$CF -DSWARM_STOP_NO_AUTH"
[ "$MODE" = late ] && CF="$CF -DSWARM_STOP_SEND_ONCE"
[ "$MODE" = netnp ] && CF="$CF -DSWARM_STOP_NET_NO_PERSIST"
[ "$MODE" = nofwd ] && CF="$CF -DSWARM_STOP_NO_RESTORE_FWD"
make -C "$BOOT" clean >/dev/null 2>&1
make -C "$BOOT" EXTRA_CFLAGS="$CF" > /tmp/stop_live.build.log 2>&1 \
    || { echo "[stop-live] build FAIL"; tail -20 /tmp/stop_live.build.log; exit 1; }
[ -x "$ROOT/relay/relay" ] || make -C "$ROOT/relay" >/dev/null || exit 1
export PKERNEL_RELAY_KEY=0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef
unset PKERNEL_RELAY_HOST PKERNEL_RELAY_PORT PKERNEL_LAN
PORT="${SLV_PORT:-7491}"
WORK="$(mktemp -d /tmp/slv.XXXXXX)"
REDS=""; RELAY=0; PA=0; PB=0; PC=0; PD=0; PE=0
res() { echo "[stop-live] $1 $2"; [ "$2" = FAIL ] && REDS="$REDS $1"; return 0; }
cleanup() {
    for p in "$PA" "$PB" "$PC" "$PD" "$PE" "$RELAY"; do [ "$p" != 0 ] && kill -9 "$p" 2>/dev/null; done
    exec 5>&- 6>&- 7>&- 8>&- 9>&- 2>/dev/null; wait 2>/dev/null
    make -C "$BOOT" clean >/dev/null 2>&1
    rm -rf "$WORK"
}
trap cleanup EXIT
wait_for() {  # <file> <ERE> <secs>
    local n=0
    while [ "$n" -lt "$(( $3 * 2 ))" ]; do
        grep -aqE "$2" "$1" 2>/dev/null && return 0
        sleep 0.5; n=$((n + 1))
    done
    return 1
}
# the signed commands (one throwaway node, no network)
mkdir -p "$WORK/sign"
printf 'stop sign 1 5 91\nstop sign 1 7 90\nstop sign 2 8 90\nexit\n' \
    | timeout 120 env PKERNEL_NODE_ID=9 PKERNEL_PFS_DIR="$WORK/sign" "$BOOT/p-kernel" 2>&1 | tr -d '\r' > "$WORK/sign.log"
K2=$(grep -a '^\[stop\] pk ' "$WORK/sign.log" | sed -n 1p | awk '{print $3}')
K1=$(grep -a '^\[stop\] pk ' "$WORK/sign.log" | sed -n 2p | awk '{print $3}')
C2S=$(grep -a '^\[stop\] cmd ' "$WORK/sign.log" | sed -n 1p | awk '{print $3}')
C1S=$(grep -a '^\[stop\] cmd ' "$WORK/sign.log" | sed -n 2p | awk '{print $3}')
C1R=$(grep -a '^\[stop\] cmd ' "$WORK/sign.log" | sed -n 3p | awk '{print $3}')
if [ "${#K1}" != 64 ] || [ "${#K2}" != 64 ] || [ "${#C2S}" != 216 ] || [ "${#C1S}" != 216 ] || [ "${#C1R}" != 216 ]; then
    echo "[stop-live] RED: could not make the signed commands"; exit 1
fi
paste_cmd() {  # <fd> <216 hex>
    printf 'stop in %s\n' "${2:0:72}" >&"$1"; sleep 0.3
    printf 'stop in %s\n' "${2:72:72}" >&"$1"; sleep 0.3
    printf 'stop in %s\n' "${2:144:72}" >&"$1"; sleep 0.3
    printf 'stop apply\n' >&"$1"
}
"$ROOT/relay/relay" -p "$PORT" > "$WORK/relay.log" 2>&1 &
RELAY=$!
sleep 1
A="$WORK/a.log"; B="$WORK/b.log"; C="$WORK/c.log"
mkfifo "$WORK/fa" "$WORK/fb" "$WORK/fc" "$WORK/fd" "$WORK/fe"; mkdir -p "$WORK/da" "$WORK/db" "$WORK/dc" "$WORK/dd" "$WORK/de"
start() {  # <id> <fifo> <log> <dir>
    env PKERNEL_NODE_ID="$1" PKERNEL_AUTONET=1 PKERNEL_PFS_DIR="$4" PKERNEL_RELAY="127.0.0.1:$PORT" \
        "$BOOT/p-kernel" < "$2" > "$3" 2>&1 &
}
start 1 "$WORK/fa" "$A" "$WORK/da"; PA=$!; exec 7<>"$WORK/fa"
start 2 "$WORK/fb" "$B" "$WORK/db"; PB=$!; exec 8<>"$WORK/fb"
start 3 "$WORK/fc" "$C" "$WORK/dc"; PC=$!; exec 9<>"$WORK/fc"
up=1
for f in "$A" "$B" "$C"; do wait_for "$f" 'mind_net_task up' 120 || up=0; done
[ "$up" = 1 ] && res L0-up PASS || res L0-up FAIL
printf 'stop key %s\nstop key %s\n' "$K1" "$K2" >&7
printf 'stop key %s\n' "$K1" >&8
printf 'stop key %s\n' "$K1" >&9
sleep 2
# L1: K2 stop on A
paste_cmd 7 "$C2S"
wait_for "$A" 'stopped 1 local 0 signed 1 epoch 5' 20 || echo "[stop-live] note: A did not take the K2 stop"
sleep 30
printf 'stop\n' >&8; printf 'stop\n' >&9; sleep 2
st() { tr -d '\r' < "$1" | grep -a '\[stop\] stopped ' | tail -1; }
if st "$A" | grep -q 'stopped 1 local 0 signed 1 epoch 5' &&
   st "$B" | grep -q 'stopped 0 ' && st "$C" | grep -q 'stopped 0 ' &&
   grep -aq '\[stop\] network command refused' "$B" && grep -aq '\[stop\] network command refused' "$C"; then
    res L1-unknown-key-stops-nobody PASS
else
    res L1-unknown-key-stops-nobody FAIL
fi
# L2: K1 stop on A
paste_cmd 7 "$C1S"
okb=0; okc=0
wait_for "$B" '\[stop\] network command accepted' 60 && okb=1
wait_for "$C" '\[stop\] network command accepted' 60 && okc=1
printf 'stop\n' >&8; printf 'stop\n' >&9; sleep 2
if [ "$okb" = 1 ] && [ "$okc" = 1 ] &&
   st "$B" | grep -q 'stopped 1 local 0 signed 1 epoch 7' && st "$C" | grep -q 'stopped 1 local 0 signed 1 epoch 7'; then
    res L2-one-command-stops-all PASS
else
    res L2-one-command-stops-all FAIL
fi
# L4: a late joiner D gets the stop from the re-drive
D="$WORK/d.log"
start 4 "$WORK/fd" "$D" "$WORK/dd"; PD=$!; exec 6<>"$WORK/fd"
okd=0
if wait_for "$D" 'mind_net_task up' 120; then
    printf 'stop key %s\n' "$K1" >&6
    wait_for "$D" '\[stop\] network command accepted' 60 && okd=1
fi
printf 'stop\n' >&6; sleep 2
if [ "$okd" = 1 ] && st "$D" | grep -q 'stopped 1 local 0 signed 1 epoch 7'; then
    res L4-late-joiner-stops PASS
else
    res L4-late-joiner-stops FAIL
fi
# L5: B's network-taken stop survives a restart (offline, its own dir)
kill -9 "$PB" 2>/dev/null; wait "$PB" 2>/dev/null; PB=0
printf 'stop\nexit\n' | timeout 60 env PKERNEL_NODE_ID=2 PKERNEL_PFS_DIR="$WORK/db" \
    "$BOOT/p-kernel" > "$WORK/b2.log" 2>&1
if grep -aq '^\[stop\] restored' "$WORK/b2.log" &&
   [ "$(tr -d '\r' < "$WORK/b2.log" | grep -a '^\[stop\] stopped ' | head -1)" = "[stop] stopped 1 local 0 signed 1 epoch 7 keys 1" ]; then
    res L5-net-stop-survives-restart PASS
else
    res L5-net-stop-survives-restart FAIL
fi
B="$WORK/b3.log"
start 2 "$WORK/fb" "$B" "$WORK/db"; PB=$!
wait_for "$B" 'mind_net_task up' 120 || echo "[stop-live] note: B did not come back up"
# L3: K1 resume on A
paste_cmd 7 "$C1R"
wait_for "$B" 'epoch 8' 60 || true
wait_for "$C" 'epoch 8' 60 || true
wait_for "$D" 'epoch 8' 60 || true
sleep 3
printf 'stop\n' >&8; printf 'stop\n' >&9; printf 'stop\n' >&6; sleep 2
if st "$B" | grep -q 'stopped 0 local 0 signed 0 epoch 8' && st "$C" | grep -q 'stopped 0 local 0 signed 0 epoch 8' &&
   st "$D" | grep -q 'stopped 0 local 0 signed 0 epoch 8'; then
    res L3-resume-all PASS
else
    res L3-resume-all FAIL
fi
echo "[stop-live] last status A: $(st "$A") | B: $(st "$B") | C: $(st "$C") | D: $(st "$D")"
cp "$B" /tmp/stop_live.B.log 2>/dev/null
# L6: B alone, restarted, re-drives the command it restored to a new node E
for p in "$PA" "$PC" "$PD" "$PB"; do [ "$p" != 0 ] && kill -9 "$p" 2>/dev/null; done
wait "$PA" "$PC" "$PD" "$PB" 2>/dev/null; PA=0; PC=0; PD=0; PB=0
B="$WORK/b4.log"
start 2 "$WORK/fb" "$B" "$WORK/db"; PB=$!
oke=0
if wait_for "$B" 'mind_net_task up' 120 && grep -aq '^\[stop\] restored' "$B"; then
    E="$WORK/e.log"
    start 5 "$WORK/fe" "$E" "$WORK/de"; PE=$!; exec 5<>"$WORK/fe"
    if wait_for "$E" 'mind_net_task up' 120; then
        printf 'stop key %s\n' "$K1" >&5
        wait_for "$E" '\[stop\] network command accepted' 60 && oke=1
    fi
    printf 'stop\n' >&5; sleep 2
fi
if [ "$oke" = 1 ] && st "$E" | grep -q 'stopped 0 local 0 signed 0 epoch 8'; then
    res L6-restored-node-redrives PASS
else
    res L6-restored-node-redrives FAIL
fi
echo "[stop-live] L6 status B: $(st "$B") | E: $(st "${E:-/dev/null}")"
case "$MODE" in
    nc)
        case "$REDS" in
            *L2-one-command-stops-all*) echo "[stop-live] NC RED as expected:$REDS"; exit 0 ;;
            *) echo "[stop-live] NC FAIL: the falsifier stayed green ($REDS)"; exit 1 ;;
        esac ;;
    late)
        case "$REDS" in
            *L2-one-command-stops-all*) echo "[stop-live] LATE FAIL: L2 went red too ($REDS)"; exit 1 ;;
            *L4-late-joiner-stops*) echo "[stop-live] LATE RED as expected:$REDS"; exit 0 ;;
            *) echo "[stop-live] LATE FAIL: the falsifier stayed green ($REDS)"; exit 1 ;;
        esac ;;
    nofwd)
        case "$REDS" in
            *L3-resume-all*) echo "[stop-live] NOFWD FAIL: L3 went red too ($REDS)"; exit 1 ;;
            *L6-restored-node-redrives*) echo "[stop-live] NOFWD RED as expected:$REDS"; exit 0 ;;
            *) echo "[stop-live] NOFWD FAIL: stayed green ($REDS)"; exit 1 ;;
        esac ;;
    netnp)
        case "$REDS" in
            *L5-net-stop-survives-restart*) echo "[stop-live] NETNP RED as expected:$REDS"; exit 0 ;;
            *) echo "[stop-live] NETNP FAIL: stayed green ($REDS)"; exit 1 ;;
        esac ;;
    noauth)
        case "$REDS" in
            *L1-unknown-key-stops-nobody*) echo "[stop-live] NOAUTH RED as expected:$REDS"; exit 0 ;;
            *) echo "[stop-live] NOAUTH FAIL: stayed green ($REDS)"; exit 1 ;;
        esac ;;
esac
if [ -z "$REDS" ]; then echo "[stop-live] ALL PASS"; exit 0; fi
echo "[stop-live] RED:$REDS"; exit 1
