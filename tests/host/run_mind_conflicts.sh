#!/bin/bash
# tests/host/run_mind_conflicts.sh — ROADMAP 1-3a, the conflict history
# (docs/architecture/30-module/mind-conflicts.md, global-rules.md rule 2).
#
# When a belief is revised, the superseded value, who taught it and how firmly
# it was held must stay readable (`mind conflicts`, and under `mind ask`).
#   local : one node; mind teach sky blue, then sky green.
#           Expect a record "blue" (taught here) superseded by "green"
#           (taught here), in `mind conflicts` and in `mind ask sky`. (With no
#           networking the node has no id yet, so no "(node N)".)
#   rule  : (1-3b) see the rule arm below — the stamp rule, not arrival order.
#   bump  : (1-3b) see the bump arm at the end — the Lamport clock step.
#   remote: node A (id 1) teaches sky blue; once node B (id 2) has it, B
#           teaches sky green. B keeps "blue" from node 0 (the remote
#           teacher, with its prov) superseded by green "taught here
#           (node 1)"; A, receiving green, keeps its own "blue" ("taught here
#           (node 0)") superseded by "green" from node 1.
# Since 1-3b every record also carries the Lamport stamps of both values.
# Output: "[mind-conflicts] <check> PASS|FAIL", then ALL PASS (exit 0) or
# RED (exit 1). Env: MC_PORT (default 7481; the rule arm uses PORT+1..+3,
# the bump arm PORT+4).
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
PORT="${MC_PORT:-7481}"
WORK="$(mktemp -d /tmp/mconf.XXXXXX)"
REDS=""; RELAY=0; PA=0; PB=0; QA=0; QB=0; QC=0; R1=0; R2=0; RH=0; RS=0; SA=0; SB=0
res() { echo "[mind-conflicts] $1 $2"; [ "$2" = FAIL ] && REDS="$REDS $1"; return 0; }
cleanup() {
    for p in "$PA" "$PB" "$RELAY" "$QA" "$QB" "$QC" "$R1" "$R2" "$RH" "$RS" "$SA" "$SB"; do [ "$p" != 0 ] && kill -9 "$p" 2>/dev/null; done
    exec 3>&- 4>&- 5>&- 6>&- 7>&- 8>&- 9>&- 2>/dev/null; wait 2>/dev/null
    # the node stores are ~176 MB a run; delete them only after the ALL PASS line
    # (a red or a timeout-killed run keeps them, with the logs)
    [ "$GREEN" = 1 ] && rm -rf "$WORK"/dl "$WORK"/da "$WORK"/db "$WORK"/eqa "$WORK"/eqb \
        "$WORK"/eqc "$WORK"/esa "$WORK"/esb
}
GREEN=0
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

# ---- local arm ------------------------------------------------------------
L="$WORK/local.log"; mkdir -p "$WORK/dl"
printf 'mind teach sky blue\nmind teach sky green\nmind conflicts\nmind ask sky\nexit\n' \
    | timeout 180 env PKERNEL_NODE_ID=1 PKERNEL_PFS_DIR="$WORK/dl" "$BOOT/p-kernel" > "$L" 2>&1
chk local-recorded "$L" '^\[mind\] conflict recorded: key [0-9]+ "sky": "blue" \(taught here, stamp 1, seq [0-9]+, (PENDING|RETAINED) [0-9]+/[0-9]+, prov kept\) superseded by "green" taught here, stamp 2 at'
chk local-list     "$L" '^\[mind\] conflict key [0-9]+ "sky": "blue" \(taught here, .*superseded by "green" taught here, stamp 2 at'
chk local-count    "$L" '^\[mind\] conflicts: 1 kept, 0 older dropped'
chk local-ask      "$L" '^\[mind\]   conflict history: key [0-9]+ "sky": "blue"'

# ---- remote arm -----------------------------------------------------------
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
wait_for "$A" 'mind_net_task up' 90 || echo "[mind-conflicts] note: A mind_net_task not seen"
wait_for "$B" 'mind_net_task up' 90 || echo "[mind-conflicts] note: B mind_net_task not seen"
t=0
while [ "$t" -lt 150 ] && ! grep -aq 'size=2' "$A"; do printf 'region\n' >&7; sleep 2; t=$((t + 2)); done
printf 'mind teach sky blue\n' >&7
wait_for "$B" 'remote teach arrived: "sky"->"blue"' 150 || echo "[mind-conflicts] note: blue never reached B"
printf 'mind teach sky green\n' >&8
wait_for "$A" 'remote REVISE key [0-9]+ "sky"' 150 || echo "[mind-conflicts] note: green never reached A"
printf 'mind conflicts\nmind ask sky\n' >&7
printf 'mind conflicts\nmind ask sky\n' >&8
wait_for "$A" 'ask "sky" ->' 60; wait_for "$B" 'ask "sky" ->' 60; sleep 2
chk remote-B-recorded "$B" '^\[mind\] conflict recorded: key [0-9]+ "sky": "blue" \(from node 0, stamp 1, seq [0-9]+, (PENDING|RETAINED) [0-9]+/[0-9]+, prov kept\) superseded by "green" taught here \(node 1\), stamp [0-9]+'
chk remote-A-recorded "$A" '^\[mind\] conflict recorded: key [0-9]+ "sky": "blue" \(taught here \(node 0\), .*prov kept\) superseded by "green" from node 1'
chk remote-A-ask      "$A" '^\[mind\]   conflict history: key [0-9]+ "sky": "blue"'
chk remote-B-ask      "$B" '^\[mind\]   conflict history: key [0-9]+ "sky": "blue"'

# ---- rule arm (1-3b) --------------------------------------------------------
# Concurrent teaches in a partition, then the LOSER arrives LAST at the
# observer C, so last-arrival-wins and the stamp rule give different answers.
#   A (id 1) on "R1,RH": sky blue             -> stamp 1, node 0
#   B (id 2) on "R2"   : fire warm, sky green -> green stamp 2, node 1
#   C (id 3) on "R2,RH": learns green from B (the only one it holds)
# then B is killed and R2, R1 die: A and C meet on RH, and A re-drives blue.
# Rule: C refuses blue (stamp 1 < 2) and pushes green back, so A moves to
# green. Last-arrival-wins: C moves to blue and A never hears green.
# (The first shape, 8d609377, let B stay: A got green from B before its blue
# reached C, so C never saw blue under either rule — not discriminating.)
for p in "$PA" "$PB" "$RELAY"; do kill -9 "$p" 2>/dev/null; done
PA=0; PB=0; RELAY=0
P1=$((PORT + 1)); P2=$((PORT + 2)); PH=$((PORT + 3))
"$ROOT/relay/relay" -p "$P1" > "$WORK/r1.log" 2>&1 & R1=$!
"$ROOT/relay/relay" -p "$P2" > "$WORK/r2.log" 2>&1 & R2=$!
"$ROOT/relay/relay" -p "$PH" > "$WORK/rh.log" 2>&1 & RH=$!
sleep 1
QAL="$WORK/qa.log"; QBL="$WORK/qb.log"; QCL="$WORK/qc.log"
mkfifo "$WORK/qa" "$WORK/qb" "$WORK/qc"; mkdir -p "$WORK/eqa" "$WORK/eqb" "$WORK/eqc"
env PKERNEL_NODE_ID=1 PKERNEL_AUTONET=1 PKERNEL_PFS_DIR="$WORK/eqa" PKERNEL_RELAY="127.0.0.1:$P1,127.0.0.1:$PH" \
    "$BOOT/p-kernel" < "$WORK/qa" > "$QAL" 2>&1 &
QA=$!; exec 5<>"$WORK/qa"
env PKERNEL_NODE_ID=2 PKERNEL_AUTONET=1 PKERNEL_PFS_DIR="$WORK/eqb" PKERNEL_RELAY="127.0.0.1:$P2" \
    "$BOOT/p-kernel" < "$WORK/qb" > "$QBL" 2>&1 &
QB=$!; exec 6<>"$WORK/qb"
env PKERNEL_NODE_ID=3 PKERNEL_AUTONET=1 PKERNEL_PFS_DIR="$WORK/eqc" PKERNEL_RELAY="127.0.0.1:$P2,127.0.0.1:$PH" \
    "$BOOT/p-kernel" < "$WORK/qc" > "$QCL" 2>&1 &
QC=$!; exec 9<>"$WORK/qc"
for f in "$QAL" "$QBL" "$QCL"; do wait_for "$f" 'mind_net_task up' 90 || echo "[mind-conflicts] note: $f mind_net_task not seen"; done
printf 'mind teach sky blue\n' >&5
printf 'mind teach fire warm\n' >&6
wait_for "$QBL" 'published mind/teach k=[0-9]+ v=[0-9]+ seq=1 ' 60 || echo "[mind-conflicts] note: B fire not published"
printf 'mind teach sky green\n' >&6
wait_for "$QAL" 'published mind/teach' 60 || echo "[mind-conflicts] note: A blue not published"
wait_for "$QBL" 'published mind/teach k=[0-9]+ v=[0-9]+ seq=2 ' 60 || echo "[mind-conflicts] note: B green not published at seq 2"
wait_for "$QCL" 'remote teach arrived: "sky"->"green"' 150 || echo "[mind-conflicts] note: green never reached C"
grep -aq 'remote teach arrived: "sky"->"blue"' "$QCL" && echo "[mind-conflicts] note: C saw blue before the heal (partition leaked)"
kill -9 "$QB"; QB=0
kill -9 "$R2"; R2=0
kill -9 "$R1"; R1=0
wait_for "$QCL" 'conflict rule: incoming stamp [0-9]+ node 0|remote REVISE key [0-9]+ "sky": "green"->"blue"' 180 || echo "[mind-conflicts] note: blue never reached C"
wait_for "$QAL" 'remote REVISE key [0-9]+ "sky": "blue"->"green"' 90 || echo "[mind-conflicts] note: A never took green"
sleep 5
printf 'mind conflicts\n' >&5; printf 'mind conflicts\n' >&9
sleep 3
chk rule-C-refused "$QCL" '^\[mind\] conflict recorded: key [0-9]+ "sky": "blue" \(from node 0, stamp 1, refused on arrival, prov kept\) superseded by "green" from node 1, stamp 2'
if grep -aqE 'remote REVISE key [0-9]+ "sky": "green"->"blue"' "$QCL"; then res rule-C-kept-green FAIL; else res rule-C-kept-green PASS; fi
chk rule-A-took-green "$QAL" 'remote REVISE key [0-9]+ "sky": "blue"->"green"'
# A's own record (with its seq): C's refused record heard as mind/hist reads
# "taught here (node 0), stamp 1, refused on arrival" on A and must not count
if grep -aE '^\[mind\] conflict recorded: key [0-9]+ "sky": "blue" \(taught here \(node 0\), stamp 1, seq [0-9]+, .*superseded by "green" from node 1, stamp 2' "$QAL" \
        | grep -avq 'heard as history'; then
    res rule-A-recorded PASS; else res rule-A-recorded FAIL; fi

# ---- bump arm (1-3b, the Lamport step) --------------------------------------
# The rule arm's green wins because B taught twice; it stays green with the
# clock step (mind_net_task: r3_fq_seq up to every fact_seq it receives)
# deleted (audit-18). Here only the step can make B's correction win:
#   A (id 1) alone: fire warm, snow white, sea deep -> stamps 1..3
#   B (id 2) joins; A teaches sky blue               -> stamp 4
#   B, having seen blue (and at most the re-driven sea), teaches sky green.
# With the step B's clock is >= 4 when it sees blue, so green carries >= 5
# and A takes it. Without it B's clock counts only what B learned (<= 2), so
# green carries <= 3, A refuses it and pushes blue back: the correction made
# after seeing the value loses — what the rule promises never happens.
# Both nodes run with PKERNEL_MIND_HAVE=0: with the 1-3c summary B learns
# fire/snow/sea before blue and the arm stops testing the step (its own
# bump-precondition goes red), so the arm keeps the summary off on purpose.
for p in "$QA" "$QC" "$RH"; do kill -9 "$p" 2>/dev/null; done
QA=0; QC=0; RH=0
PS=$((PORT + 4))
"$ROOT/relay/relay" -p "$PS" > "$WORK/rs.log" 2>&1 & RS=$!
sleep 1
SAL="$WORK/sa.log"; SBL="$WORK/sb.log"
mkfifo "$WORK/sa" "$WORK/sb"; mkdir -p "$WORK/esa" "$WORK/esb"
env PKERNEL_MIND_HAVE=0 PKERNEL_NODE_ID=1 PKERNEL_AUTONET=1 PKERNEL_PFS_DIR="$WORK/esa" PKERNEL_RELAY="127.0.0.1:$PS" \
    "$BOOT/p-kernel" < "$WORK/sa" > "$SAL" 2>&1 &
SA=$!; exec 3<>"$WORK/sa"
wait_for "$SAL" 'mind_net_task up' 90 || echo "[mind-conflicts] note: bump A mind_net_task not seen"
printf 'mind teach fire warm\n' >&3
wait_for "$SAL" 'published mind/teach k=[0-9]+ v=[0-9]+ seq=1 ' 60 || echo "[mind-conflicts] note: bump A fire not published"
printf 'mind teach snow white\n' >&3
wait_for "$SAL" 'published mind/teach k=[0-9]+ v=[0-9]+ seq=2 ' 60 || echo "[mind-conflicts] note: bump A snow not published"
printf 'mind teach sea deep\n' >&3
wait_for "$SAL" 'published mind/teach k=[0-9]+ v=[0-9]+ seq=3 ' 60 || echo "[mind-conflicts] note: bump A sea not published"
env PKERNEL_MIND_HAVE=0 PKERNEL_NODE_ID=2 PKERNEL_AUTONET=1 PKERNEL_PFS_DIR="$WORK/esb" PKERNEL_RELAY="127.0.0.1:$PS" \
    "$BOOT/p-kernel" < "$WORK/sb" > "$SBL" 2>&1 &
SB=$!; exec 4<>"$WORK/sb"
wait_for "$SBL" 'mind_net_task up' 90 || echo "[mind-conflicts] note: bump B mind_net_task not seen"
t=0
while [ "$t" -lt 150 ] && ! grep -aq 'size=2' "$SAL"; do printf 'region\n' >&3; sleep 2; t=$((t + 2)); done
printf 'mind teach sky blue\n' >&3
wait_for "$SAL" 'published mind/teach k=[0-9]+ v=[0-9]+ seq=4 ' 60 || echo "[mind-conflicts] note: bump A blue not published at seq 4"
wait_for "$SBL" 'remote teach arrived: "sky"->"blue"' 150 || echo "[mind-conflicts] note: blue never reached bump B"
cp "$SBL" "$WORK/sb-before-green.log"
printf 'mind teach sky green\n' >&4
wait_for "$SAL" 'conflict rule: incoming stamp [0-9]+ node 1' 90 || echo "[mind-conflicts] note: green never reached bump A"
sleep 5
printf 'mind ask sky\n' >&3; printf 'mind ask sky\n' >&4
wait_for "$SAL" 'ask "sky" ->' 60; wait_for "$SBL" 'ask "sky" ->' 60; sleep 2
# precondition: B must not have learned fire/snow from A, or B's own count
# alone could lift green past 4 and the arm would not test the step.
if grep -aqE 'remote teach arrived: "(fire|snow)"' "$WORK/sb-before-green.log"; then
    res bump-precondition FAIL; else res bump-precondition PASS; fi
chk bump-A-rule       "$SAL" '^\[mind\] conflict rule: incoming stamp ([5-9]|[1-9][0-9]+) node 1 beats held stamp 4 node 0'
chk bump-A-took-green "$SAL" 'remote REVISE key [0-9]+ "sky": "blue"->"green"'
if grep -aqE 'remote REVISE key [0-9]+ "sky": "green"->"blue"' "$SBL"; then
    res bump-B-kept-green FAIL; else res bump-B-kept-green PASS; fi
echo "[mind-conflicts] logs: $WORK"
if [ -z "$REDS" ]; then echo "[mind-conflicts] ALL PASS"; GREEN=1; exit 0; fi
echo "[mind-conflicts] RED:$REDS"; exit 1
