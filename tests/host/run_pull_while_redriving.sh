#!/bin/bash
# tests/host/run_pull_while_redriving.sh — ROADMAP 1-2b, the second half of
# MIND-GEN-0-B E1: a node that keeps re-driving its OWN last teach must still
# deliver pull answers.
#
# A (id 1) alone teaches fire warm, then sky blue, so its last teach (re-driven
# every mind_net_task tick) is sky. B (id 2) joins afterwards and so only ever
# sees the sky re-drive; B is asked about fire, wonders, and publishes a want.
# A answers on the same LATEST_ONLY mind/teach topic. Before the fix A sent the
# answer and then its own sky re-drive in the same tick, so B only ever saw
# sky: fire never arrived.
#   pwr-B-wondered : precondition — B wanted fire (else the pull is not tested)
#   pwr-A-answered : A logged the answer
#   pwr-B-got-fire : B "remote teach arrived: "fire"->"warm"" within 60 s
# Output: "[pull-redrive] <check> PASS|FAIL", then ALL PASS (exit 0) or RED (1).
# Env: PWR_PORT (default 7491).
set -u
HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
case "$(uname -m)" in
    aarch64|arm64) BOOT="$ROOT/boot/linux" ;;
    *)             BOOT="$ROOT/boot/linux_x86_64" ;;
esac
[ -x "$BOOT/p-kernel" ]    || make -C "$BOOT"       >/dev/null || exit 1
[ -x "$ROOT/relay/relay" ] || make -C "$ROOT/relay" >/dev/null || exit 1
export PKERNEL_RELAY_KEY=0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef
unset PKERNEL_RELAY_HOST PKERNEL_RELAY_PORT PKERNEL_LAN
PORT="${PWR_PORT:-7491}"
WORK="$(mktemp -d /tmp/pwr.XXXXXX)"
REDS=""; RELAY=0; PA=0; PB=0
res() { echo "[pull-redrive] $1 $2"; [ "$2" = FAIL ] && REDS="$REDS $1"; return 0; }
cleanup() {
    for p in "$PA" "$PB" "$RELAY"; do [ "$p" != 0 ] && kill -9 "$p" 2>/dev/null; done
    exec 7>&- 8>&- 2>/dev/null; wait 2>/dev/null
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
chk() {  # <name> <file> <ERE>
    if grep -aqE "$3" "$2"; then res "$1" PASS; else res "$1" FAIL; fi
}
"$ROOT/relay/relay" -p "$PORT" > "$WORK/relay.log" 2>&1 &
RELAY=$!
sleep 1
A="$WORK/a.log"; B="$WORK/b.log"
mkfifo "$WORK/fa" "$WORK/fb"; mkdir -p "$WORK/da" "$WORK/db"
env PKERNEL_NODE_ID=1 PKERNEL_AUTONET=1 PKERNEL_PFS_DIR="$WORK/da" PKERNEL_RELAY="127.0.0.1:$PORT" \
    "$BOOT/p-kernel" < "$WORK/fa" > "$A" 2>&1 &
PA=$!; exec 7<>"$WORK/fa"
wait_for "$A" 'mind_net_task up' 90 || echo "[pull-redrive] note: A mind_net_task not seen"
printf 'mind teach fire warm\n' >&7
wait_for "$A" 'published mind/teach k=[0-9]+ v=[0-9]+ seq=1 ' 60 || echo "[pull-redrive] note: A fire not published"
printf 'mind teach sky blue\n' >&7
wait_for "$A" 'published mind/teach k=[0-9]+ v=[0-9]+ seq=2 ' 60 || echo "[pull-redrive] note: A sky not published"
env PKERNEL_NODE_ID=2 PKERNEL_AUTONET=1 PKERNEL_PFS_DIR="$WORK/db" PKERNEL_RELAY="127.0.0.1:$PORT" \
    "$BOOT/p-kernel" < "$WORK/fb" > "$B" 2>&1 &
PB=$!; exec 8<>"$WORK/fb"
wait_for "$B" 'mind_net_task up' 90 || echo "[pull-redrive] note: B mind_net_task not seen"
wait_for "$B" 'remote teach arrived: "sky"->"blue"' 150 || echo "[pull-redrive] note: the sky re-drive never reached B"
printf 'mind ask fire\n' >&8
wait_for "$B" 'ask "fire" ->' 60
wait_for "$A" 'answering want key [0-9]+ \("fire"\)' 90 || echo "[pull-redrive] note: A never answered"
wait_for "$B" 'remote teach arrived: "fire"->"warm"' 60 || echo "[pull-redrive] note: fire never reached B"
chk pwr-B-wondered "$B" 'wondering about "fire"'
chk pwr-A-answered "$A" 'answering want key [0-9]+ \("fire"\) from node 1'
chk pwr-B-got-fire "$B" 'remote teach arrived: "fire"->"warm"'
echo "[pull-redrive] logs: $WORK"
if [ -z "$REDS" ]; then echo "[pull-redrive] ALL PASS"; exit 0; fi
echo "[pull-redrive] RED:$REDS"; exit 1
