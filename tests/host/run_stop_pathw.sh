#!/bin/bash
# tests/host/run_stop_pathw.sh — ROADMAP 0-14 S2 stage 2e: a stopped node
# neither sends nor folds weights (Path W, the fleet-DMN merge;
# docs/architecture/30-module/swarm-stop.md, "2e"). Stages 2a-2d stopped the
# teach mouths and the DMN sleep; mind_merge_task kept publishing rw[] and
# folding region peers while the node was stopped (probe on master 25ea7712,
# two nodes over a relay, 140 s: B published rw[] 5 times, A 10 times, each
# with a "fold:" line; no fold completed, the chunks were still in flight).
#
# Expectations fixed before the code (two hosted nodes over a relay, B = id 2):
#   W0 B publishes rw[] before the stop (Path W is live)
#   W1 B `stop now` stops it (status "stopped 1 local 1")
#   W2 between B's status line 2 s after the stop and the one 30 s later:
#      no "[onemind] published rw[]", no "[onemind] FOLD", no "[onemind] fold:"
#   W3 `mind merge` on B inside that window is refused ("[stop] refused: mind
#      merge") and does not fold ("[onemind-merge]" absent in the window)
#   W4 after B's `stop resume`, B publishes rw[] again within 40 s
# Falsifier: -DSWARM_STOP_NO_PATHW_OBEY (neither the task nor the verb looks
# at the stop) must turn W2 and W3 red.
# Output: "[stop-pathw] <check> PASS|FAIL", then ALL PASS (exit 0) or RED (1).
# Arg: "nc" builds the falsifier and expects W2 W3 red (exit 0 when so).
# Rebuilds boot/linux* with EXTRA_CFLAGS and cleans it afterwards.
# Env: SPW_PORT (default 7497).
set -u
HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
case "$(uname -m)" in
    aarch64|arm64) BOOT="$ROOT/boot/linux" ;;
    *)             BOOT="$ROOT/boot/linux_x86_64" ;;
esac
MODE="${1:-plain}"
CF=""
[ "$MODE" = nc ] && CF="-DSWARM_STOP_NO_PATHW_OBEY"
make -C "$BOOT" clean >/dev/null 2>&1
make -C "$BOOT" EXTRA_CFLAGS="$CF" > /tmp/stop_pathw.build.log 2>&1 \
    || { echo "[stop-pathw] build FAIL"; tail -20 /tmp/stop_pathw.build.log; exit 1; }
[ -x "$ROOT/relay/relay" ] || make -C "$ROOT/relay" >/dev/null || exit 1
export PKERNEL_RELAY_KEY=0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef
unset PKERNEL_RELAY_HOST PKERNEL_RELAY_PORT PKERNEL_LAN
PORT="${SPW_PORT:-7497}"
WORK="$(mktemp -d /tmp/spw.XXXXXX)"
REDS=""; RELAY=0; PA=0; PB=0
res() { echo "[stop-pathw] $1 $2"; [ "$2" = FAIL ] && REDS="$REDS $1"; return 0; }
cleanup() {
    for p in "$PA" "$PB" "$RELAY"; do [ "$p" != 0 ] && kill -9 "$p" 2>/dev/null; done
    exec 7>&- 8>&- 2>/dev/null; wait 2>/dev/null
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
wait_for "$A" 'mind_merge_task up' 120 || echo "[stop-pathw] note: A mind_merge_task not seen"
wait_for "$B" 'mind_merge_task up' 120 || echo "[stop-pathw] note: B mind_merge_task not seen"
printf 'mind teach sky blue\n' >&7
printf 'mind teach sea green\n' >&8
if wait_for "$B" '\[onemind\] published rw\[\]' 120; then res W0-pathw-live PASS; else res W0-pathw-live FAIL; fi
printf 'stop now\n' >&8
sleep 2;  printf 'stop\n' >&8                 # status line A (window start)
sleep 5;  printf 'mind merge\n' >&8
sleep 25; printf 'stop\n' >&8                 # status line B (window end)
sleep 1;  printf 'stop resume\n' >&8          # status line C
wait_for "$B" 'stopped 0 local 0' 30 || true
sleep 40
tr -d '\r' < "$B" > "$WORK/b.txt"
L="$WORK/b.txt"
ln() { grep -an '\[stop\] stopped [01] ' "$L" | sed -n "${1}p" | cut -d: -f1; }
S=$(ln 1); WA=$(ln 2); WB=$(ln 3); C=$(ln 4)
cnt() { awk -v a="$1" -v b="$2" -v re="$3" 'NR > a && NR < b && $0 ~ re' "$L" | grep -c .; }
PW='\[onemind\] (published rw|FOLD|fold:)'
echo "[stop-pathw] status lines at $S $WA $WB $C; Path W lines before the stop $(cnt 0 "${S:-0}" "$PW"), in the window $(cnt "${WA:-0}" "${WB:-0}" "$PW"), after the resume $(cnt "${C:-0}" 999999 '\[onemind\] published rw')"
[ -n "$S" ] && sed -n "${S}p" "$L" | grep -q 'stopped 1 local 1' && res W1-stopped PASS || res W1-stopped FAIL
[ -n "$WA" ] && [ -n "$WB" ] && [ "$(cnt "$WA" "$WB" "$PW")" = 0 ] && res W2-no-pathw PASS || res W2-no-pathw FAIL
if [ -n "$WA" ] && [ -n "$WB" ] && [ "$(cnt "$WA" "$WB" 'refused: mind merge')" -ge 1 ] &&
   [ "$(cnt "$WA" "$WB" '\[onemind-merge\]')" = 0 ]; then
    res W3-merge-refused PASS
else
    res W3-merge-refused FAIL
fi
if [ -n "$C" ] && sed -n "${C}p" "$L" | grep -q 'stopped 0 local 0' &&
   [ "$(cnt "$C" 999999 '\[onemind\] published rw')" -ge 1 ]; then
    res W4-after-resume PASS
else
    res W4-after-resume FAIL
fi
cp "$L" /tmp/stop_pathw.B.log 2>/dev/null
if [ "$MODE" = nc ]; then
    case "$REDS" in
        *W2-no-pathw*W3-merge-refused*) echo "[stop-pathw] NC RED as expected:$REDS"; exit 0 ;;
        *) echo "[stop-pathw] NC FAIL: the falsifier stayed green ($REDS)"; exit 1 ;;
    esac
fi
if [ -z "$REDS" ]; then echo "[stop-pathw] ALL PASS"; exit 0; fi
echo "[stop-pathw] RED:$REDS"; exit 1
