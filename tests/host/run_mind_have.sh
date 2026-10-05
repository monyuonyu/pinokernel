#!/bin/bash
# tests/host/run_mind_have.sh — ROADMAP 1-3c step 2, the HAVE summary
# (docs/architecture/30-module/mind-conflicts.md stage 3).
#
# A (id 1) alone teaches fire warm, snow white, sea deep. Then B (id 2) joins.
# A's re-drive only ever carries its LAST teach (sea), so before 1-3c B could
# get fire and snow only by being asked about them (the pull) — nothing moves
# a fact to a newcomer before a question (MIND-GEN-0-B E1). With mind/have, B
# sees A's summary, wants the keys it never held (level 0) and pulls them.
# Nobody asks B anything.
#   have-fire / have-snow : B learns them from A within 120 s
#   have-hint             : B says why ("never held here -> want")
#   have-not-precious     : the pulled facts do not arrive precious (F-LOCAL:
#                           a remote summary never raises B's curiosity)
# HAVE_OFF=1 runs both nodes with PKERNEL_MIND_HAVE=0 (the negative control:
# have-fire / have-snow / have-hint must go red).
# Output: "[mind-have] <check> PASS|FAIL", then ALL PASS (exit 0) or RED (exit 1).
# Env: MH_PORT (default 7531), HAVE_OFF.
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
PORT="${MH_PORT:-7531}"
HV=1; [ "${HAVE_OFF:-0}" = 1 ] && HV=0
WORK="$(mktemp -d /tmp/mhave.XXXXXX)"
REDS=""; RELAY=0; A=0; B=0
res() { echo "[mind-have] $1 $2"; [ "$2" = FAIL ] && REDS="$REDS $1"; return 0; }
cleanup() {
    for p in "$A" "$B" "$RELAY"; do [ "$p" != 0 ] && kill -9 "$p" 2>/dev/null; done
    exec 3>&- 4>&- 2>/dev/null; wait 2>/dev/null
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

"$ROOT/relay/relay" -p "$PORT" > "$WORK/relay.log" 2>&1 & RELAY=$!
sleep 1
AL="$WORK/a.log"; BL="$WORK/b.log"
mkfifo "$WORK/a" "$WORK/b"; mkdir -p "$WORK/ea" "$WORK/eb"
env PKERNEL_MIND_HAVE=$HV PKERNEL_NODE_ID=1 PKERNEL_AUTONET=1 PKERNEL_PFS_DIR="$WORK/ea" PKERNEL_RELAY="127.0.0.1:$PORT" \
    "$BOOT/p-kernel" < "$WORK/a" > "$AL" 2>&1 &
A=$!; exec 3<>"$WORK/a"
wait_for "$AL" 'mind_net_task up' 90 || echo "[mind-have] note: A mind_net_task not seen"
printf 'mind teach fire warm\n' >&3
wait_for "$AL" 'published mind/teach k=[0-9]+ v=[0-9]+ seq=1 ' 60 || echo "[mind-have] note: A fire not published"
printf 'mind teach snow white\n' >&3
wait_for "$AL" 'published mind/teach k=[0-9]+ v=[0-9]+ seq=2 ' 60 || echo "[mind-have] note: A snow not published"
printf 'mind teach sea deep\n' >&3
wait_for "$AL" 'published mind/teach k=[0-9]+ v=[0-9]+ seq=3 ' 60 || echo "[mind-have] note: A sea not published"
env PKERNEL_MIND_HAVE=$HV PKERNEL_NODE_ID=2 PKERNEL_AUTONET=1 PKERNEL_PFS_DIR="$WORK/eb" PKERNEL_RELAY="127.0.0.1:$PORT" \
    "$BOOT/p-kernel" < "$WORK/b" > "$BL" 2>&1 &
B=$!; exec 4<>"$WORK/b"
wait_for "$BL" 'mind_net_task up' 90 || echo "[mind-have] note: B mind_net_task not seen"
wait_for "$BL" 'remote teach arrived: "sea"' 120 || echo "[mind-have] note: the re-driven sea never reached B"
wait_for "$BL" 'remote teach arrived: "fire"' 120
wait_for "$BL" 'remote teach arrived: "snow"' 60
chk have-fire  "$BL" 'remote teach arrived: "fire"->"warm"'
chk have-snow  "$BL" 'remote teach arrived: "snow"->"white"'
chk have-hint  "$BL" '^\[mind\] mind/have from node 0: key [0-9]+ \("(fire|snow)"\) never held here -> want \(level 0, pull\)'
if grep -aqE 'curiosity answered' "$BL"; then res have-not-precious FAIL; else res have-not-precious PASS; fi
grep -aE 'mind/have|mind/want|answering want|remote teach arrived' "$AL" "$BL" | head -20
echo "[mind-have] logs: $WORK (PKERNEL_MIND_HAVE=$HV)"
if [ -z "$REDS" ]; then echo "[mind-have] ALL PASS"; exit 0; fi
echo "[mind-have] RED:$REDS"; exit 1
