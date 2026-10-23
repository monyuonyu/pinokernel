#!/bin/bash
# tests/host/run_mind_conflicts_misread.sh — ROADMAP 1-8 v4 / rule 11, the
# conflict half of "send the record, not the reading"
# (docs/architecture/30-module/knowledge-survival.md, audit-61's limit).
#
# feat/ks-record-wire made the push-back (mc_incoming_lost), the lost value
# kept in the history (mc_note) and "do I hold this winner" for mind/hist
# (mhi_holds) use the record r3_fq.val, not the fast layer's reading yhat.
# run_mind_conflicts.sh cannot see the difference: without a misread the two
# are equal (audit-61 put each site back to yhat, E1..E3, and it stayed green).
#
# Here every node runs the test build -DR3_TEST_MISREAD_KEY=0 (the reading of
# key 0, sky, is shifted by one answer at learn and at revise, so val != yhat
# for sky on every node). The rule arm of run_mind_conflicts.sh, then a
# fourth node D that only hears the conflict as mind/hist:
#   A (id 1) on "R1,RH": sky blue             -> stamp 1, node 0
#   B (id 2) on "R2"   : fire warm, sky green -> green stamp 2, node 1
#   C (id 3) on "R2,RH": learns green from B
#   B, R2, R1 die; A and C meet on RH. C refuses blue and pushes green back,
#   A takes green. D (id 4) joins RH, teaches sky green itself (so it holds
#   the winner) and must hear the conflict from A or C as history.
# Checks:
#   mr-precond       : A misread sky (teacher_agree 0), else nothing is tested
#   mr-C-refused     : C keeps blue as "refused on arrival", superseded by green
#   mr-A-took-green  : A "remote REVISE ... "blue"->"green""     (push = record)
#   mr-A-recorded    : A's history: "blue" taught here superseded by green
#                                                                 (lost = record)
#   mr-D-heard       : D "conflict recorded: ... "blue" ... "green" ... heard as
#                      history from node N"            (mhi_holds = record, both
#                                                       on the sender and on D)
# Falsifiers (one site back to the reading each, audit-61's E1..E3):
#   nc-push  -DR3_PUSH_SENDS_READING   -> mr-A-took-green red
#   nc-note  -DR3_NOTE_KEEPS_READING   -> mr-A-recorded red
#   nc-hist  -DR3_HIST_HOLDS_READING   -> mr-D-heard red
# Output: "[mind-conf-mr] <check> PASS|FAIL", then ALL PASS (exit 0) or RED (1).
# Arg: plain (default) | nc-push | nc-note | nc-hist; an nc arm exits 0 when
# its check is red ("NC RED as expected"), 1 when it stayed green.
# Rebuilds boot/linux* with EXTRA_CFLAGS and cleans it afterwards.
# Env: MCM_PORT (default 7541; uses PORT+1..+3).
set -u
HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
case "$(uname -m)" in
    aarch64|arm64) BOOT="$ROOT/boot/linux" ;;
    *)             BOOT="$ROOT/boot/linux_x86_64" ;;
esac
MODE="${1:-plain}"
CF="-DR3_TEST_MISREAD_KEY=0"
case "$MODE" in
    plain)   WANT="" ;;
    nc-push) CF="$CF -DR3_PUSH_SENDS_READING"; WANT=mr-A-took-green ;;
    nc-note) CF="$CF -DR3_NOTE_KEEPS_READING"; WANT=mr-A-recorded ;;
    nc-hist) CF="$CF -DR3_HIST_HOLDS_READING"; WANT=mr-D-heard ;;
    *) echo "[mind-conf-mr] unknown mode $MODE"; exit 2 ;;
esac
make -C "$BOOT" clean >/dev/null 2>&1
make -C "$BOOT" EXTRA_CFLAGS="$CF" > /tmp/mind_conf_mr.build.log 2>&1 \
    || { echo "[mind-conf-mr] build FAIL"; tail -20 /tmp/mind_conf_mr.build.log; exit 1; }
[ -x "$ROOT/relay/relay" ] || make -C "$ROOT/relay" >/dev/null || exit 1
export PKERNEL_RELAY_KEY=0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef
unset PKERNEL_RELAY_HOST PKERNEL_RELAY_PORT PKERNEL_LAN
PORT="${MCM_PORT:-7541}"
WORK="$(mktemp -d /tmp/mconf-mr.XXXXXX)"
REDS=""; R1=0; R2=0; RH=0; QA=0; QB=0; QC=0; QD=0
res() { echo "[mind-conf-mr] $1 $2"; [ "$2" = FAIL ] && REDS="$REDS $1"; return 0; }
cleanup() {
    for p in "$QA" "$QB" "$QC" "$QD" "$R1" "$R2" "$RH"; do [ "$p" != 0 ] && kill -9 "$p" 2>/dev/null; done
    exec 5>&- 6>&- 8>&- 9>&- 2>/dev/null; wait 2>/dev/null
    # the node stores are large; the logs stay in $WORK
    rm -rf "$WORK"/eqa "$WORK"/eqb "$WORK"/eqc "$WORK"/eqd
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

P1=$((PORT + 1)); P2=$((PORT + 2)); PH=$((PORT + 3))
"$ROOT/relay/relay" -p "$P1" > "$WORK/r1.log" 2>&1 & R1=$!
"$ROOT/relay/relay" -p "$P2" > "$WORK/r2.log" 2>&1 & R2=$!
"$ROOT/relay/relay" -p "$PH" > "$WORK/rh.log" 2>&1 & RH=$!
sleep 1
QAL="$WORK/qa.log"; QBL="$WORK/qb.log"; QCL="$WORK/qc.log"; QDL="$WORK/qd.log"
mkfifo "$WORK/qa" "$WORK/qb" "$WORK/qc" "$WORK/qd"
mkdir -p "$WORK/eqa" "$WORK/eqb" "$WORK/eqc" "$WORK/eqd"
env PKERNEL_NODE_ID=1 PKERNEL_AUTONET=1 PKERNEL_PFS_DIR="$WORK/eqa" PKERNEL_RELAY="127.0.0.1:$P1,127.0.0.1:$PH" \
    "$BOOT/p-kernel" < "$WORK/qa" > "$QAL" 2>&1 &
QA=$!; exec 5<>"$WORK/qa"
env PKERNEL_NODE_ID=2 PKERNEL_AUTONET=1 PKERNEL_PFS_DIR="$WORK/eqb" PKERNEL_RELAY="127.0.0.1:$P2" \
    "$BOOT/p-kernel" < "$WORK/qb" > "$QBL" 2>&1 &
QB=$!; exec 6<>"$WORK/qb"
env PKERNEL_NODE_ID=3 PKERNEL_AUTONET=1 PKERNEL_PFS_DIR="$WORK/eqc" PKERNEL_RELAY="127.0.0.1:$P2,127.0.0.1:$PH" \
    "$BOOT/p-kernel" < "$WORK/qc" > "$QCL" 2>&1 &
QC=$!; exec 9<>"$WORK/qc"
for f in "$QAL" "$QBL" "$QCL"; do wait_for "$f" 'mind_net_task up' 90 || echo "[mind-conf-mr] note: $f mind_net_task not seen"; done
printf 'mind teach sky blue\n' >&5
printf 'mind teach fire warm\n' >&6
wait_for "$QBL" 'published mind/teach k=[0-9]+ v=[0-9]+ seq=1 ' 60 || echo "[mind-conf-mr] note: B fire not published"
printf 'mind teach sky green\n' >&6
wait_for "$QAL" 'published mind/teach' 60 || echo "[mind-conf-mr] note: A blue not published"
wait_for "$QBL" 'published mind/teach k=[0-9]+ v=[0-9]+ seq=2 ' 60 || echo "[mind-conf-mr] note: B green not published at seq 2"
wait_for "$QCL" 'remote teach arrived: "sky"->"green"' 150 || echo "[mind-conf-mr] note: green never reached C"
grep -aq 'remote teach arrived: "sky"->"blue"' "$QCL" && echo "[mind-conf-mr] note: C saw blue before the heal (partition leaked)"
kill -9 "$QB"; QB=0
kill -9 "$R2"; R2=0
kill -9 "$R1"; R1=0
wait_for "$QCL" 'conflict rule: incoming stamp [0-9]+ node 0|remote REVISE key [0-9]+ "sky": "green"->' 180 || echo "[mind-conf-mr] note: blue never reached C"
wait_for "$QAL" 'remote REVISE key [0-9]+ "sky": "blue"->' 90 || echo "[mind-conf-mr] note: A never revised sky"

# ---- D: hears the conflict only as mind/hist ---------------------------------
env PKERNEL_NODE_ID=4 PKERNEL_AUTONET=1 PKERNEL_PFS_DIR="$WORK/eqd" PKERNEL_RELAY="127.0.0.1:$PH" \
    "$BOOT/p-kernel" < "$WORK/qd" > "$QDL" 2>&1 &
QD=$!; exec 8<>"$WORK/qd"
wait_for "$QDL" 'mind_net_task up' 90 || echo "[mind-conf-mr] note: D mind_net_task not seen"
printf 'mind teach sky green\n' >&8
wait_for "$QDL" 'heard as history from node' 90 || echo "[mind-conf-mr] note: D heard no history"
sleep 3
printf 'mind conflicts\n' >&5; printf 'mind conflicts\n' >&9; printf 'mind conflicts\n' >&8
sleep 3
chk mr-precond      "$QAL" '^\[mind\]   teacher_agree 0 '
chk mr-C-refused    "$QCL" '^\[mind\] conflict recorded: key 0 "sky": "blue" \(from node 0, stamp 1, refused on arrival, prov kept\) superseded by "green" from node 1, stamp 2'
chk mr-A-took-green "$QAL" 'remote REVISE key 0 "sky": "blue"->"green"'
chk mr-A-recorded   "$QAL" '^\[mind\] conflict recorded: key 0 "sky": "blue" \(taught here \(node 0\), stamp 1, .*superseded by "green" from node 1, stamp 2'
chk mr-D-heard      "$QDL" '^\[mind\] conflict recorded: key 0 "sky": "blue" .*superseded by "green" .*heard as history from node [0-9]+'
grep -aE 'remote REVISE key 0|conflict recorded: key 0' "$QAL" "$QCL" "$QDL" | sed "s|$WORK/||; s/^/[mind-conf-mr]   /"
echo "[mind-conf-mr] logs: $WORK"
if [ "$MODE" != plain ]; then
    case "$REDS" in
        *mr-precond*) echo "[mind-conf-mr] NC FAIL: no misread, $MODE tests nothing ($REDS)"; exit 1 ;;
        *"$WANT"*) echo "[mind-conf-mr] NC RED as expected ($MODE):$REDS"; exit 0 ;;
        *) echo "[mind-conf-mr] NC FAIL: $MODE stayed green on $WANT ($REDS)"; exit 1 ;;
    esac
fi
if [ -z "$REDS" ]; then echo "[mind-conf-mr] ALL PASS"; exit 0; fi
echo "[mind-conf-mr] RED:$REDS"; exit 1
