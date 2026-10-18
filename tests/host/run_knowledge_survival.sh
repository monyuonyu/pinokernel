#!/bin/bash
# tests/host/run_knowledge_survival.sh — ROADMAP 1-8, knowledge survival rate
# (small version). Design and the expectations S1..S4, fixed before the first
# run: docs/architecture/30-module/knowledge-survival.md.
#
#   N hosted nodes on one relay (one region). Nodes 1..K each teach one fact.
#   Wait for the facts to spread, then kill -9 round(F*N) nodes chosen by SEED
#   (teachers included). 20 s later ask every survivor every key.
#
# Env: KS_N (8), KS_F (0.5), KS_SEED (1), KS_PORT (7491), KS_LOGDIR,
#      KS_NC=1 = the binary was built with -DKS_NO_REMOTE_TEACH (receivers drop
#      remote teaches): the expectation flips to S4 (mean <= R1) and, v3,
#      S4M / V4M (mean / held_mean <= R1M, the R1 baseline as a mean rate).
#      Plain build, v3: V5 (held_mean >= R3M).
#      The script does not build with the flag itself; build the tree first:
#        make -C boot/linux_x86_64 EXTRA_CFLAGS=-DKS_NO_REMOTE_TEACH
# Output: "[ks] f=.. any=.. mean=.. R1=.. R3=.. spread=.. guessed=.." and
#   "[ks] S<n> PASS|FAIL ...", then "[ks] ALL PASS" (exit 0) or "[ks] RED: .." (exit 1).
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
N="${KS_N:-8}"; F="${KS_F:-0.5}"; SEED="${KS_SEED:-1}"; PORT="${KS_PORT:-7491}"; NC="${KS_NC:-0}"
KEYS=(sea stone leaf ink); VALS=(north sweet loud fresh); K=${#KEYS[@]}
WORK="$(mktemp -d /tmp/ks.XXXXXX)"
LOGDIR="${KS_LOGDIR:-$WORK/logs}"; mkdir -p "$LOGDIR"
declare -A PID
RH=0; REDS=""

note() { echo "[$(date '+%H:%M:%S')] $*"; }
res()  { echo "[ks] $1 $2: $3"; [ "$2" = FAIL ] && REDS="$REDS $1"; return 0; }
cleanup() {
    for i in $(seq 1 "$N"); do
        [ "${PID[$i]:-0}" != 0 ] && kill -9 "${PID[$i]}" 2>/dev/null
        eval "exec $((i + 2))>&-" 2>/dev/null
    done
    [ "$RH" != 0 ] && kill -9 "$RH" 2>/dev/null
    wait 2>/dev/null
    rm -rf "$WORK"/d[0-9]* "$WORK"/f[0-9]*
}
trap cleanup EXIT

start_node() {  # <i>
    local i="$1" fifo="$WORK/f$1"
    rm -f "$fifo"; mkfifo "$fifo"; mkdir -p "$WORK/d$i"
    env PKERNEL_NODE_ID="$i" PKERNEL_AUTONET=1 PKERNEL_PFS_DIR="$WORK/d$i" PKERNEL_RELAY="127.0.0.1:$PORT" \
        "$BOOT/p-kernel" < "$fifo" > "$LOGDIR/node$i.log" 2>&1 &
    PID[$i]=$!
    eval "exec $((i + 2))<>\"$fifo\""
}
send() { local i="$1"; shift; printf '%s\n' "$*" >&$((i + 2)); }
wait_for() {  # <file> <ERE> <secs>
    local n=0
    while [ "$n" -lt "$(( $3 * 2 ))" ]; do
        grep -aqE "$2" "$1" 2>/dev/null && return 0
        sleep 0.5; n=$((n + 1))
    done
    return 1
}
holds() {  # <node> <key index>: 0 if the node taught it or logged its arrival
    local i="$1" j="$2"
    [ "$i" = "$((j + 1))" ] && return 0
    grep -aq "remote teach arrived: \"${KEYS[$j]}\"->\"${VALS[$j]}\"" "$LOGDIR/node$i.log"
}
PROV=""
prov() {  # <node>: sets PROV to this node's `mind provenance` key lines ("k|v" per line)
    local i="$1" lg="$LOGDIR/node$1.log" pre lines n=0
    lines=$(wc -l < "$lg"); pre=$(grep -ac '^\[mind\] provenance: ' "$lg")
    send "$i" "mind provenance"
    while [ "$(grep -ac '^\[mind\] provenance: ' "$lg")" -le "$pre" ] && [ "$n" -lt 60 ]; do sleep 0.5; n=$((n + 1)); done
    PROV=$(tail -n +"$((lines + 1))" "$lg" | sed -nE 's/^\[mind\]   "([a-z]+)" -> "([a-z]+)": .*/\1|\2/p')
}
held_n() {  # <PROV text> <key> <val>: 1 if the node's record holds key -> val
    printf '%s\n' "$1" | grep -qx "$2|$3" && echo 1 || echo 0
}
ANS=""; GUESS=0
ask() {  # <node> <key>: sets ANS and GUESS (1 = "key not in the live queue")
    local i="$1" k="$2" lg="$LOGDIR/node$1.log" pre n=0
    pre=$(grep -ac "ask \"$k\" ->" "$lg")
    send "$i" "mind ask $k"
    while [ "$(grep -ac "ask \"$k\" ->" "$lg")" -le "$pre" ] && [ "$n" -lt 60 ]; do sleep 0.5; n=$((n + 1)); done
    ANS=$(grep -a "ask \"$k\" ->" "$lg" | tail -1 | grep -aoE -- '-> "[a-z]+"' | tr -d '">-' | tr -d ' ')
    sleep 0.5
    GUESS=0
    grep -a -A8 "ask \"$k\" ->" "$lg" | tail -9 | grep -aq 'not in the live queue' && GUESS=1
}

echo "==========================================================="
echo " [ks] knowledge survival: N=$N F=$F SEED=$SEED NC=$NC  logs: $LOGDIR"
echo "==========================================================="
"$ROOT/relay/relay" -p "$PORT" > "$LOGDIR/relay.log" 2>&1 &
RH=$!
sleep 1
for i in $(seq 1 "$N"); do start_node "$i"; done
for i in $(seq 1 "$N"); do wait_for "$LOGDIR/node$i.log" 'mind_net_task up' 120 || note "node$i: mind_net_task not seen"; done
# one region of N: poll `region` on node 1
t=0; pre=$(grep -ac "size=$N" "$LOGDIR/node1.log")
while [ "$t" -lt 180 ]; do
    send 1 "region"; sleep 2; t=$((t + 2))
    [ "$(grep -ac "size=$N" "$LOGDIR/node1.log")" -gt "$pre" ] && break
done
[ "$t" -lt 180 ] && note "region size=$N after ${t}s" || note "region size=$N not seen in 180 s (going on)"

# ---- teach ------------------------------------------------------------------
for j in $(seq 0 $((K - 1))); do send $((j + 1)) "mind teach ${KEYS[$j]} ${VALS[$j]}"; done
for j in $(seq 0 $((K - 1))); do wait_for "$LOGDIR/node$((j + 1)).log" 'published mind/teach' 120 || note "node$((j + 1)) never published"; done
for i in $(seq 1 "$N"); do send "$i" "mind wait 90"; done
# spread: wait until every node holds every fact, at most 200 s
t=0
while [ "$t" -lt 200 ]; do
    h=0
    for i in $(seq 1 "$N"); do for j in $(seq 0 $((K - 1))); do holds "$i" "$j" && h=$((h + 1)); done; done
    [ "$h" = "$((N * K))" ] && break
    sleep 5; t=$((t + 5))
done
SPREAD=$h
note "spread before the kill: $SPREAD / $((N * K)) after ${t}s"
sleep 5
# ask everyone BEFORE the kill too (v1.1): a fact every node holds but no node
# can recall is a recall failure, not a loss (first run: ink->fresh reached all
# 8 nodes, all answered "near", the teacher too)
pre_any=0; pre_good=0
for j in $(seq 0 $((K - 1))); do
    got=0; c=0; seen=""
    for i in $(seq 1 "$N"); do
        ask "$i" "${KEYS[$j]}"; seen="$seen ${ANS:-none}"
        [ "$ANS" = "${VALS[$j]}" ] && [ "$GUESS" = 0 ] && { c=$((c + 1)); got=1; }
    done
    pre_good=$((pre_good + c)); pre_any=$((pre_any + got))
    note "before the kill: ${KEYS[$j]} recalled by $c/$N (answers:$seen)"
done

# ---- kill -------------------------------------------------------------------
KILL=$(python3 -c 'import random,sys; n,f,s=int(sys.argv[1]),float(sys.argv[2]),int(sys.argv[3]); r=random.Random(s); print(" ".join(map(str,sorted(r.sample(range(1,n+1),round(f*n))))))' "$N" "$F" "$SEED")
note "kill -9: $KILL"
for i in $KILL; do kill -9 "${PID[$i]}"; PID[$i]=0; done
ALIVE=""; for i in $(seq 1 "$N"); do [ "${PID[$i]}" != 0 ] && ALIVE="$ALIVE $i"; done
sleep 20

# ---- held records (v2, primary): `mind provenance` is read-only --------------
hany=0; hgood=0
declare -A HP
for i in $ALIVE; do prov "$i"; HP[$i]="$PROV"; note "node$i holds: $(echo $PROV)"; done
for j in $(seq 0 $((K - 1))); do
    got=0
    for i in $ALIVE; do
        [ "$(held_n "${HP[$i]}" "${KEYS[$j]}" "${VALS[$j]}")" = 1 ] && { hgood=$((hgood + 1)); got=1; }
    done
    hany=$((hany + got))
done

# ---- ask --------------------------------------------------------------------
any=0; good=0; asked=0; guessed=0; unheld=0
for j in $(seq 0 $((K - 1))); do
    k="${KEYS[$j]}"; got=0
    for i in $ALIVE; do
        ask "$i" "$k"; asked=$((asked + 1))
        if [ "$ANS" = "${VALS[$j]}" ]; then
            if [ "$GUESS" = 1 ]; then guessed=$((guessed + 1)); else good=$((good + 1)); got=1; fi
            # v3: a right answer from a node whose provenance has no record of it
            if [ "$GUESS" = 0 ] && [ "$(held_n "${HP[$i]}" "$k" "${VALS[$j]}")" != 1 ]; then
                unheld=$((unheld + 1)); note "node$i answered $k right with no record (provenance: $(echo ${HP[$i]}))"
            fi
        fi
        note "node$i ask $k -> ${ANS:-<none>} (want ${VALS[$j]}, guess=$GUESS)"
    done
    any=$((any + got))
done

# ---- baselines on the same kill set -------------------------------------------
# R1/R3 are "any" rates; R1M/R3M (v3) are the same baselines as "mean" rates
# (holder copies alive / (survivors x K)), the right partner for mean / held_mean
read -r R1 R3 R1M R3M < <(python3 -c '
import sys
n, k = int(sys.argv[1]), int(sys.argv[2]); dead = set(map(int, sys.argv[3].split()))
alive = lambda i: i not in dead
na = sum(alive(i) for i in range(1, n + 1))
r1 = sum(alive(j + 1) for j in range(k)) / k
r3 = sum(any(alive((j + d) % n + 1) for d in range(3)) for j in range(k)) / k
r1m = sum(alive(j + 1) for j in range(k)) / (na * k) if na else 0
r3m = sum(alive((j + d) % n + 1) for j in range(k) for d in range(3)) / (na * k) if na else 0
print("%.3f %.3f %.3f %.3f" % (r1, r3, r1m, r3m))' "$N" "$K" "$KILL")
ANYF=$(python3 -c 'import sys; print("%.3f" % (int(sys.argv[1]) / int(sys.argv[2])))' "$any" "$K")
MEANF=$(python3 -c 'import sys; a=int(sys.argv[2]); print("%.3f" % (int(sys.argv[1]) / a if a else 0))' "$good" "$asked")
SPF=$(python3 -c 'import sys; print("%.3f" % (int(sys.argv[1]) / int(sys.argv[2])))' "$SPREAD" "$((N * K))")
echo "[ks] f=$F N=$N seed=$SEED nc=$NC killed=[$KILL] any=$ANYF mean=$MEANF R1=$R1 R3=$R3 spread=$SPF guessed=$guessed/$asked"
echo "[ks] before the kill: any=$pre_any/$K recalled=$pre_good/$((N * K)) (not a verdict: separates recall failures from losses)"
HANYF=$(python3 -c 'import sys; print("%.3f" % (int(sys.argv[1]) / int(sys.argv[2])))' "$hany" "$K")
NALIVE=$(echo $ALIVE | wc -w)
HMEANF=$(python3 -c 'import sys; d=int(sys.argv[2]); print("%.3f" % (int(sys.argv[1]) / d if d else 0))' "$hgood" "$((NALIVE * K))")
echo "[ks] held records (v2): held_any=$HANYF held_mean=$HMEANF R1=$R1 R3=$R3"
echo "[ks] mean baselines (v3): R1M=$R1M R3M=$R3M; right answers with no record: $unheld/$good"

ge() { python3 -c 'import sys; sys.exit(0 if float(sys.argv[1]) >= float(sys.argv[2]) - 1e-9 else 1)' "$1" "$2"; }
if [ "$NC" = 1 ]; then
    ge "$R1" "$MEANF" && res S4 PASS "broken (no remote teach): mean $MEANF <= R1 $R1" || res S4 FAIL "broken: mean $MEANF > R1 $R1"
    ge "$R1" "$HANYF" && res V4 PASS "broken: held_any $HANYF <= R1 $R1" || res V4 FAIL "broken: held_any $HANYF > R1 $R1"
    ge "$R1M" "$MEANF" && res S4M PASS "broken: mean $MEANF <= R1M $R1M" || res S4M FAIL "broken: mean $MEANF > R1M $R1M"
    ge "$R1M" "$HMEANF" && res V4M PASS "broken: held_mean $HMEANF <= R1M $R1M" || res V4M FAIL "broken: held_mean $HMEANF > R1M $R1M"
else
    ge "$HMEANF" "$R3M" && res V5 PASS "held_mean $HMEANF >= R3M $R3M" || res V5 FAIL "held_mean $HMEANF < R3M $R3M"
    ge "$HANYF" "$R3" && res V2 PASS "held_any $HANYF >= R3 $R3" || res V2 FAIL "held_any $HANYF < R3 $R3"
    ge "$HMEANF" "$R1" && res V3 PASS "held_mean $HMEANF >= R1 $R1" || res V3 FAIL "held_mean $HMEANF < R1 $R1"
    ge "$SPF" 0.9   && res S1 PASS "spread $SPF >= 0.9"        || res S1 FAIL "spread $SPF < 0.9"
    ge "$ANYF" "$R3" && res S2 PASS "any $ANYF >= R3 $R3"       || res S2 FAIL "any $ANYF < R3 $R3"
    ge "$MEANF" "$R1" && res S3 PASS "mean $MEANF >= R1 $R1"    || res S3 FAIL "mean $MEANF < R1 $R1"
fi
cr=""; for i in $ALIVE; do kill -0 "${PID[$i]}" 2>/dev/null || cr="$cr node$i"; done
[ -z "$cr" ] || res ALIVE FAIL "survivors died:$cr"
if [ -z "$REDS" ]; then echo "[ks] ALL PASS"; exit 0; fi
echo "[ks] RED:$REDS"
exit 1
