#!/bin/bash
# tests/host/run_stop_net_teach.sh — ROADMAP 0-14 S2 stage 2c: a stopped node
# takes no teaching from the network either (docs/architecture/30-module/
# swarm-stop.md, "2c"). Stage 2a/2b stopped the shell and galaxy mouths; a
# teach gossiped by a peer still landed.
#
# Expectations fixed before the code (two hosted nodes over a relay):
#   N1 B (id 2) is stopped at its console (`stop now`)
#   N2 A (id 1) teaches sea green; after A's re-drives, B's `mind provenance`
#      has no sea
#   N3 B logged the refusal ("remote teach ... refused (stopped)")
#   N4 after B's `stop resume`, A's re-drive lands: B "remote teach arrived:
#      "sea"->"green"" (the refusal did not eat the fact for good)
# Falsifier: -DSWARM_STOP_NO_NET_OBEY (the arrival does not look at the stop)
# must turn N2 and N3 red.
# Output: "[stop-net] <check> PASS|FAIL", then ALL PASS (exit 0) or RED (1).
# Arg: "nc" builds the falsifier and expects N2 N3 red (exit 0 when so).
# Rebuilds boot/linux* with EXTRA_CFLAGS and cleans it afterwards.
# Env: SNT_PORT (default 7499).
set -u
HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
case "$(uname -m)" in
    aarch64|arm64) BOOT="$ROOT/boot/linux" ;;
    *)             BOOT="$ROOT/boot/linux_x86_64" ;;
esac
MODE="${1:-plain}"
CF=""
[ "$MODE" = nc ] && CF="-DSWARM_STOP_NO_NET_OBEY"
make -C "$BOOT" clean >/dev/null 2>&1
make -C "$BOOT" EXTRA_CFLAGS="$CF" > /tmp/stop_net_teach.build.log 2>&1 \
    || { echo "[stop-net] build FAIL"; tail -20 /tmp/stop_net_teach.build.log; exit 1; }
[ -x "$ROOT/relay/relay" ] || make -C "$ROOT/relay" >/dev/null || exit 1
export PKERNEL_RELAY_KEY=0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef
unset PKERNEL_RELAY_HOST PKERNEL_RELAY_PORT PKERNEL_LAN
PORT="${SNT_PORT:-7499}"
WORK="$(mktemp -d /tmp/snt.XXXXXX)"
REDS=""; RELAY=0; PA=0; PB=0
res() { echo "[stop-net] $1 $2"; [ "$2" = FAIL ] && REDS="$REDS $1"; return 0; }
cleanup() {
    for p in "$PA" "$PB" "$RELAY"; do [ "$p" != 0 ] && kill -9 "$p" 2>/dev/null; done
    exec 7>&- 8>&- 2>/dev/null; wait 2>/dev/null
    make -C "$BOOT" clean >/dev/null 2>&1
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
"$ROOT/relay/relay" -p "$PORT" > "$WORK/relay.log" 2>&1 &
RELAY=$!
sleep 1
A="$WORK/a.log"; B="$WORK/b.log"
mkfifo "$WORK/fa" "$WORK/fb"; mkdir -p "$WORK/da" "$WORK/db"
env PKERNEL_NODE_ID=1 PKERNEL_AUTONET=1 PKERNEL_PFS_DIR="$WORK/da" PKERNEL_RELAY="127.0.0.1:$PORT" \
    "$BOOT/p-kernel" < "$WORK/fa" > "$A" 2>&1 &
PA=$!; exec 7<>"$WORK/fa"
env PKERNEL_NODE_ID=2 PKERNEL_AUTONET=1 PKERNEL_PFS_DIR="$WORK/db" PKERNEL_RELAY="127.0.0.1:$PORT" \
    "$BOOT/p-kernel" < "$WORK/fb" > "$B" 2>&1 &
PB=$!; exec 8<>"$WORK/fb"
wait_for "$A" 'mind_net_task up' 90 || echo "[stop-net] note: A mind_net_task not seen"
wait_for "$B" 'mind_net_task up' 90 || echo "[stop-net] note: B mind_net_task not seen"
printf 'stop now\n' >&8
if wait_for "$B" '^\[stop\] stopped 1 local 1' 30; then res N1-stopped PASS; else res N1-stopped FAIL; fi
printf 'mind teach sea green\n' >&7
wait_for "$A" 'published mind/teach k=[0-9]+ v=[0-9]+ seq=1 ' 60 || echo "[stop-net] note: A sea not published"
sleep 15                                  # ~30 re-drives at 500 ms
printf 'mind provenance\n' >&8
sleep 3
if grep -aqE '^\[mind\]   "sea" -> ' "$B" || grep -aqE 'remote teach arrived: "sea"' "$B"; then
    res N2-not-taught FAIL
else
    res N2-not-taught PASS
fi
if grep -aqE 'remote teach from node [0-9]+ refused \(stopped' "$B"; then res N3-logged PASS; else res N3-logged FAIL; fi
printf 'stop resume\n' >&8
if wait_for "$B" 'remote teach arrived: "sea"->"green"' 60; then res N4-after-resume PASS; else res N4-after-resume FAIL; fi
cp "$B" /tmp/stop_net_teach.B.log 2>/dev/null
echo "[stop-net] logs: $WORK"
if [ "$MODE" = nc ]; then
    case "$REDS" in
        *N2-not-taught*N3-logged*) echo "[stop-net] NC RED as expected:$REDS"; exit 0 ;;
        *) echo "[stop-net] NC FAIL: the falsifier stayed green ($REDS)"; exit 1 ;;
    esac
fi
if [ -z "$REDS" ]; then echo "[stop-net] ALL PASS"; exit 0; fi
echo "[stop-net] RED:$REDS"; exit 1
