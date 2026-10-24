#!/bin/bash
# tests/host/run_stop_outgoing.sh — ROADMAP 0-14 S2 stage 2f: a stopped node
# sends nothing of its mind either (docs/architecture/30-module/
# swarm-stop.md, "2f"). Stages 2a-2e stopped what comes IN (teach mouths, the
# sleep, Path W); a stopped node still re-drove its own last teach, answered
# region wants and published its mind/have summary every tick, so a peer that
# joined after the stop still learned from it.
#
# Expectations fixed before the code (two hosted nodes over a relay; B = id 2
# is taught and stopped BEFORE A = id 1 starts, so A can only learn sky from B):
#   O0 B is stopped (status "stopped 1 local 1") after `mind teach sky blue`
#   O1 A starts and links (its mind_net_task is up) while B is stopped
#   O2 for 40 s after that, A learns nothing of sky: no "remote teach
#      arrived: "sky"" and A's `mind provenance` has no "sky"
#   O3 after B's `stop resume`, A receives "remote teach arrived:
#      "sky"->"blue"" within 90 s (the stop held the fact back, it did not
#      drop it)
#   O4 (audit-75: the test saw the re-drive mouth only) right after O1 A runs
#      `mind ask sky` and publishes a want ("published mind/want") inside the
#      window: a stopped B that answered it would teach A (O2)
#   O5 inside the window A logs no "mind/have from node" line (B is its only
#      peer; the log numbers nodes from 0, so B is "node 1") (a stopped B
#      publishes no have summary). B is taught a second fact (ink fresh) that A
#      never asks about: A hints only keys it does not already want, so the ask
#      of O4 alone would hide a have summary about sky (first measurement)
#   O6 (audit-77: a B that did not gate its want stayed green) right after O1
#      the stopped B runs `mind ask snow` (a key word nobody was taught); inside the
#      window B logs no "published mind/want" and A no "mind/want from node"
#   O7 after B's `stop resume`, B logs "published mind/want" within 60 s (the
#      ask was held back, not lost; O6 could see it)
# Falsifier: -DSWARM_STOP_NO_OUT_OBEY (the outgoing tick does not look at the
# stop) must turn O2 red. One mouth at a time: -DSWARM_STOP_ANSWER_UNGATED
# (B answers wants while stopped) must turn O2 red, -DSWARM_STOP_HAVE_UNGATED
# (B publishes mind/have while stopped) must turn O5 red,
# -DSWARM_STOP_WANT_UNGATED (B publishes its own wants while stopped) must
# turn O6 red (a local measurement, not in CI).
# Output: "[stop-out] <check> PASS|FAIL", then ALL PASS (exit 0) or RED (1).
# Arg: "nc" builds the falsifier and expects O2 red (exit 0 when so);
# "answer", "have" and "want" build the one-mouth falsifiers and expect
# O2 / O5 / O6 red.
# Rebuilds boot/linux* with EXTRA_CFLAGS and cleans it afterwards.
# Env: SOU_PORT (default 7495).
set -u
HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
case "$(uname -m)" in
    aarch64|arm64) BOOT="$ROOT/boot/linux" ;;
    *)             BOOT="$ROOT/boot/linux_x86_64" ;;
esac
MODE="${1:-plain}"
CF=""
[ "$MODE" = nc ] && CF="-DSWARM_STOP_NO_OUT_OBEY"
[ "$MODE" = answer ] && CF="-DSWARM_STOP_ANSWER_UNGATED"
[ "$MODE" = have ] && CF="-DSWARM_STOP_HAVE_UNGATED"
[ "$MODE" = want ] && CF="-DSWARM_STOP_WANT_UNGATED"
make -C "$BOOT" clean >/dev/null 2>&1
make -C "$BOOT" EXTRA_CFLAGS="$CF" > /tmp/stop_out.build.log 2>&1 \
    || { echo "[stop-out] build FAIL"; tail -20 /tmp/stop_out.build.log; exit 1; }
[ -x "$ROOT/relay/relay" ] || make -C "$ROOT/relay" >/dev/null || exit 1
export PKERNEL_RELAY_KEY=0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef
unset PKERNEL_RELAY_HOST PKERNEL_RELAY_PORT PKERNEL_LAN
PORT="${SOU_PORT:-7495}"
WORK="$(mktemp -d /tmp/sou.XXXXXX)"
REDS=""; RELAY=0; PA=0; PB=0
res() { echo "[stop-out] $1 $2"; [ "$2" = FAIL ] && REDS="$REDS $1"; return 0; }
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
env PKERNEL_NODE_ID=2 PKERNEL_AUTONET=1 PKERNEL_PFS_DIR="$WORK/db" PKERNEL_RELAY="127.0.0.1:$PORT" \
    "$BOOT/p-kernel" < "$WORK/fb" > "$B" 2>&1 &
PB=$!; exec 8<>"$WORK/fb"
wait_for "$B" 'mind_net_task up' 90 || echo "[stop-out] note: B mind_net_task not seen"
printf 'mind teach sky blue\n' >&8
wait_for "$B" 'published mind/teach' 60 || echo "[stop-out] note: B sky not published"
n=0; printf 'mind teach ink fresh\n' >&8
while [ "$(grep -ac 'published mind/teach' "$B")" -lt 2 ] && [ "$n" -lt 120 ]; do sleep 0.5; n=$((n + 1)); done
[ "$n" -lt 120 ] || echo "[stop-out] note: B ink not published"
printf 'stop now\n' >&8
if wait_for "$B" 'stopped 1 local 1' 30; then res O0-stopped PASS; else res O0-stopped FAIL; fi
env PKERNEL_NODE_ID=1 PKERNEL_AUTONET=1 PKERNEL_PFS_DIR="$WORK/da" PKERNEL_RELAY="127.0.0.1:$PORT" \
    "$BOOT/p-kernel" < "$WORK/fa" > "$A" 2>&1 &
PA=$!; exec 7<>"$WORK/fa"
if wait_for "$A" 'mind_net_task up' 90; then res O1-a-up PASS; else res O1-a-up FAIL; fi
printf 'mind ask sky\n' >&7
printf 'mind ask snow\n' >&8
sleep 40
printf 'mind provenance\n' >&7
sleep 3
if grep -aqE 'remote teach arrived: "sky"' "$A" || grep -aqE '^\[mind\]   "sky" -> ' "$A"; then
    res O2-nothing-out FAIL
else
    res O2-nothing-out PASS
fi
if grep -aq 'published mind/want' "$A"; then res O4-a-wants-sky PASS; else res O4-a-wants-sky FAIL; fi
if grep -aq 'mind/have from node' "$A"; then res O5-no-have-out FAIL; else res O5-no-have-out PASS; fi
if grep -aq 'published mind/want' "$B" || grep -aq 'mind/want from node' "$A"; then
    res O6-no-want-out FAIL
else
    res O6-no-want-out PASS
fi
printf 'stop resume\n' >&8
if wait_for "$A" 'remote teach arrived: "sky"->"blue"' 90; then res O3-after-resume PASS; else res O3-after-resume FAIL; fi
if wait_for "$B" 'published mind/want' 60; then res O7-want-after-resume PASS; else res O7-want-after-resume FAIL; fi
cp "$A" /tmp/stop_out.A.log 2>/dev/null; cp "$B" /tmp/stop_out.B.log 2>/dev/null
want=""
[ "$MODE" = nc ] && want=O2-nothing-out
[ "$MODE" = answer ] && want=O2-nothing-out
[ "$MODE" = have ] && want=O5-no-have-out
[ "$MODE" = want ] && want=O6-no-want-out
if [ -n "$want" ]; then
    case "$REDS" in
        *"$want"*) echo "[stop-out] NC RED as expected ($MODE):$REDS"; exit 0 ;;
        *) echo "[stop-out] NC FAIL ($MODE): the falsifier stayed green ($REDS)"; exit 1 ;;
    esac
fi
if [ -z "$REDS" ]; then echo "[stop-out] ALL PASS"; exit 0; fi
echo "[stop-out] RED:$REDS"; exit 1
