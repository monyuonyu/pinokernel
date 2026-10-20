#!/bin/bash
# tests/host/run_mind_record_pull.sh — ROADMAP 1-8 / rule 11, the wire half
# of v4 (docs/architecture/30-module/knowledge-survival.md, "v4"): what a node
# SENDS about a fact it holds is its record (the value it was given), not the
# fast layer's reading of it.
#
# v4 gave r3_fq a record (val) next to the reading (yhat), but the pull answer
# still sent yhat: a peer that pulled a misread fact from its holder got the
# misreading as its own record, so the original stayed only on the teacher.
#
# Both nodes run the test build -DR3_TEST_MISREAD_KEY=15 (the arrival reading
# of key 15, ink, is shifted by one value; run_mind_record.sh has the one-node
# half). A (id 1) alone teaches ink fresh, then sky blue, so its re-drive is
# sky. B (id 2) joins afterwards, so ink can only reach B as a pull answer:
# B is asked about ink, wonders, A answers.
#   rp-precond  : A misread ink (teacher_agree 0), else nothing is tested
#   rp-answered : A logged the answer for ink
#   rp-B-fresh  : B "remote teach arrived: "ink"->"fresh"" (the record crossed
#                 the wire, not A's reading)
#   rp-B-record : B's provenance starts "ink" -> "fresh"
# Falsifier: -DR3_PULL_SENDS_READING (the answer sends the reading, the old
# behaviour) must turn rp-B-fresh and rp-B-record red.
# Output: "[mind-rec-pull] <check> PASS|FAIL", then ALL PASS (exit 0) or RED (1).
# Arg: "nc" runs the falsifier build and expects it red (exit 0 when red).
# Rebuilds boot/linux* with EXTRA_CFLAGS and cleans it afterwards.
# Env: MRP_PORT (default 7497).
set -u
HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
case "$(uname -m)" in
    aarch64|arm64) BOOT="$ROOT/boot/linux" ;;
    *)             BOOT="$ROOT/boot/linux_x86_64" ;;
esac
MODE="${1:-plain}"
CF="-DR3_TEST_MISREAD_KEY=15"
[ "$MODE" = nc ] && CF="$CF -DR3_PULL_SENDS_READING"
make -C "$BOOT" clean >/dev/null 2>&1
make -C "$BOOT" EXTRA_CFLAGS="$CF" > /tmp/mind_record_pull.build.log 2>&1 \
    || { echo "[mind-rec-pull] build FAIL"; tail -20 /tmp/mind_record_pull.build.log; exit 1; }
[ -x "$ROOT/relay/relay" ] || make -C "$ROOT/relay" >/dev/null || exit 1
export PKERNEL_RELAY_KEY=0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef
unset PKERNEL_RELAY_HOST PKERNEL_RELAY_PORT PKERNEL_LAN
PORT="${MRP_PORT:-7497}"
WORK="$(mktemp -d /tmp/mrp.XXXXXX)"
REDS=""; RELAY=0; PA=0; PB=0
res() { echo "[mind-rec-pull] $1 $2"; [ "$2" = FAIL ] && REDS="$REDS $1"; return 0; }
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
wait_for "$A" 'mind_net_task up' 90 || echo "[mind-rec-pull] note: A mind_net_task not seen"
printf 'mind teach ink fresh\n' >&7
wait_for "$A" 'published mind/teach k=[0-9]+ v=[0-9]+ seq=1 ' 60 || echo "[mind-rec-pull] note: A ink not published"
printf 'mind teach sky blue\n' >&7
wait_for "$A" 'published mind/teach k=[0-9]+ v=[0-9]+ seq=2 ' 60 || echo "[mind-rec-pull] note: A sky not published"
env PKERNEL_NODE_ID=2 PKERNEL_AUTONET=1 PKERNEL_PFS_DIR="$WORK/db" PKERNEL_RELAY="127.0.0.1:$PORT" \
    "$BOOT/p-kernel" < "$WORK/fb" > "$B" 2>&1 &
PB=$!; exec 8<>"$WORK/fb"
wait_for "$B" 'mind_net_task up' 90 || echo "[mind-rec-pull] note: B mind_net_task not seen"
wait_for "$B" 'remote teach arrived: "sky"->"blue"' 150 || echo "[mind-rec-pull] note: the sky re-drive never reached B"
printf 'mind ask ink\n' >&8
wait_for "$B" 'ask "ink" ->' 60
wait_for "$A" 'answering want key [0-9]+ \("ink"\)' 90 || echo "[mind-rec-pull] note: A never answered"
wait_for "$B" 'remote teach arrived: "ink"->' 60 || echo "[mind-rec-pull] note: ink never reached B"
sleep 2
printf 'mind provenance\n' >&8
wait_for "$B" '^\[mind\]   "ink" -> ' 30
sleep 1
chk rp-precond  "$A" '^\[mind\]   teacher_agree 0 '
chk rp-answered "$A" 'answering want key [0-9]+ \("ink"\) from node 1'
chk rp-B-fresh  "$B" 'remote teach arrived: "ink"->"fresh"'
chk rp-B-record "$B" '^\[mind\]   "ink" -> "fresh"'
grep -aE 'remote teach arrived: "ink"|^\[mind\]   "ink" -> ' "$B" | sed 's/^/[mind-rec-pull]   B: /'
cp "$B" /tmp/mind_record_pull.B.log 2>/dev/null
echo "[mind-rec-pull] logs: $WORK"
if [ "$MODE" = nc ]; then
    case "$REDS" in
        *rp-B-fresh*rp-B-record*) echo "[mind-rec-pull] NC RED as expected:$REDS"; exit 0 ;;
        *) echo "[mind-rec-pull] NC FAIL: the falsifier stayed green ($REDS)"; exit 1 ;;
    esac
fi
if [ -z "$REDS" ]; then echo "[mind-rec-pull] ALL PASS"; exit 0; fi
echo "[mind-rec-pull] RED:$REDS"; exit 1
