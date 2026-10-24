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
#   L3 A applies a K1 resume (epoch 8): within 60 s B and C are "stopped 0"
# Falsifiers: -DSWARM_STOP_NO_FORWARD (an accepted command is not sent on)
# must turn L2 red; -DSWARM_STOP_NO_AUTH (no stop-table check) turns L1 red
# (a local measurement, not in CI).
# Output: "[stop-live] <check> PASS|FAIL", then ALL PASS (exit 0) or RED (1).
# Arg: "nc" builds -DSWARM_STOP_NO_FORWARD and expects L2 red; "noauth"
# builds -DSWARM_STOP_NO_AUTH and expects L1 red (exit 0 when so).
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
make -C "$BOOT" clean >/dev/null 2>&1
make -C "$BOOT" EXTRA_CFLAGS="$CF" > /tmp/stop_live.build.log 2>&1 \
    || { echo "[stop-live] build FAIL"; tail -20 /tmp/stop_live.build.log; exit 1; }
[ -x "$ROOT/relay/relay" ] || make -C "$ROOT/relay" >/dev/null || exit 1
export PKERNEL_RELAY_KEY=0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef
unset PKERNEL_RELAY_HOST PKERNEL_RELAY_PORT PKERNEL_LAN
PORT="${SLV_PORT:-7491}"
WORK="$(mktemp -d /tmp/slv.XXXXXX)"
REDS=""; RELAY=0; PA=0; PB=0; PC=0
res() { echo "[stop-live] $1 $2"; [ "$2" = FAIL ] && REDS="$REDS $1"; return 0; }
cleanup() {
    for p in "$PA" "$PB" "$PC" "$RELAY"; do [ "$p" != 0 ] && kill -9 "$p" 2>/dev/null; done
    exec 7>&- 8>&- 9>&- 2>/dev/null; wait 2>/dev/null
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
mkfifo "$WORK/fa" "$WORK/fb" "$WORK/fc"; mkdir -p "$WORK/da" "$WORK/db" "$WORK/dc"
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
# L3: K1 resume on A
paste_cmd 7 "$C1R"
wait_for "$B" 'epoch 8' 60 || true
wait_for "$C" 'epoch 8' 60 || true
sleep 3
printf 'stop\n' >&8; printf 'stop\n' >&9; sleep 2
if st "$B" | grep -q 'stopped 0 local 0 signed 0 epoch 8' && st "$C" | grep -q 'stopped 0 local 0 signed 0 epoch 8'; then
    res L3-resume-all PASS
else
    res L3-resume-all FAIL
fi
echo "[stop-live] last status A: $(st "$A") | B: $(st "$B") | C: $(st "$C")"
cp "$B" /tmp/stop_live.B.log 2>/dev/null
case "$MODE" in
    nc)
        case "$REDS" in
            *L2-one-command-stops-all*) echo "[stop-live] NC RED as expected:$REDS"; exit 0 ;;
            *) echo "[stop-live] NC FAIL: the falsifier stayed green ($REDS)"; exit 1 ;;
        esac ;;
    noauth)
        case "$REDS" in
            *L1-unknown-key-stops-nobody*) echo "[stop-live] NOAUTH RED as expected:$REDS"; exit 0 ;;
            *) echo "[stop-live] NOAUTH FAIL: stayed green ($REDS)"; exit 1 ;;
        esac ;;
esac
if [ -z "$REDS" ]; then echo "[stop-live] ALL PASS"; exit 0; fi
echo "[stop-live] RED:$REDS"; exit 1
