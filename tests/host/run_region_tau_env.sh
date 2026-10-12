#!/bin/bash
# ---------------------------------------------------------------------------
# [region-tau-env] — test-only region RTT threshold (2026-10-11, CI-F).
#
# The self-hosted CI runner's 3-node live certs went red because a hosted
# node's measured RTT (relay hop + task wake-ups, EWMA) spreads 10-50 ms on a
# loaded host and crosses REGION_TAU_MS=50: a peer drops out of the region,
# protect caps R to 1 (samples/27, 28), dkva aggregates "1 region peers"
# (samples/13). The product τ stays 50 ms. A HOSTED build reads
#     PKERNEL_REGION_TAU_MS   (>0; unset or 0 = REGION_TAU_MS)
# Bare metal has no env and no setter (crown-neutral).
#
# Two nodes over a local relay, each in its own RTT sim zone with a 120 ms
# penalty, so the peer's RTT is always above the default τ:
#   arm default: no env -> node 1 prints tau=50ms, never size=2, no env line
#   arm env    : PKERNEL_REGION_TAU_MS=400 -> the env line with 400, `region`
#                prints tau=400ms and size=2 with the peer's rtt >= 120 ms
#                (so it is the wider τ that admitted the peer)
# Both arms must first see node 2 discovered by node 1, or the arm is FAIL.
# Falsifier: build with -DREGION_TAU_ENV_IGNORE (the setter does nothing) ->
# the env arm goes red.
#
# Usage: tests/host/run_region_tau_env.sh     exit 0 = both arms as expected
# ---------------------------------------------------------------------------
set -u
HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
case "$(uname -m)" in
    aarch64|arm64) BOOT="$ROOT/boot/linux" ;;
    x86_64|amd64)  BOOT="$ROOT/boot/linux_x86_64" ;;
    *) echo "unsupported host arch"; exit 1 ;;
esac
[ -x "$BOOT/p-kernel" ]    || make -C "$BOOT"       >/dev/null || exit 1
[ -x "$ROOT/relay/relay" ] || make -C "$ROOT/relay" >/dev/null || exit 1

PORT="${REGION_TAU_PORT:-7433}"
LOGDIR="${REGION_TAU_LOGDIR:-$(mktemp -d)}"
export PKERNEL_RELAY_KEY=0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef
export PKERNEL_RELAY_HOST=127.0.0.1
export PKERNEL_RELAY_PORT="$PORT"
export PKERNEL_GALAXY=0
export PKERNEL_RTT_ZONE_SIZE=1 PKERNEL_RTT_ZONE_PENALTY=120

PASS=0 FAIL=0
ok()  { echo "  PASS $1"; PASS=$((PASS+1)); }
bad() { echo "  FAIL $1"; FAIL=$((FAIL+1)); }

RPID="" N1="" N2=""
stop_all() {
    for p in $N1 $N2; do kill "$p" 2>/dev/null; done
    [ -n "$RPID" ] && kill "$RPID" 2>/dev/null
    for p in $N1 $N2 $RPID; do wait "$p" 2>/dev/null; done
    exec 7>&- 8>&- 2>/dev/null
    N1="" N2="" RPID=""
}
trap stop_all EXIT

# run_arm <name> <tau or empty>
run_arm() {
    local name="$1" tau="$2" d="$LOGDIR/$1"
    mkdir -p "$d"; rm -f "$d"/*
    local envs=(PKERNEL_AUTONET=1)
    [ -n "$tau" ] && envs+=(PKERNEL_REGION_TAU_MS="$tau")

    "$ROOT/relay/relay" -p "$PORT" >"$d/relay.log" 2>&1 & RPID=$!
    sleep 1
    mkfifo "$d/in1" "$d/in2"
    env "${envs[@]}" PKERNEL_NODE_ID=1 "$BOOT/p-kernel" <"$d/in1" >"$d/node1.log" 2>&1 & N1=$!
    exec 7>"$d/in1"
    env "${envs[@]}" PKERNEL_NODE_ID=2 "$BOOT/p-kernel" <"$d/in2" >"$d/node2.log" 2>&1 & N2=$!
    exec 8>"$d/in2"

    local t=0
    while ! grep -aq "node 1 discovered\|node 1 -> ALIVE\|node 1 recovered" "$d/node1.log" && [ "$t" -lt 30 ]; do sleep 1; t=$((t+1)); done
    echo "[$name] mesh after ${t}s; polling region for 30 s"
    t=0
    while [ "$t" -lt 15 ]; do
        printf 'region\n' >&7; sleep 2; t=$((t+1))
        [ -n "$tau" ] && grep -aq "size=2" "$d/node1.log" && break
    done
    printf 'exit\n' >&7; printf 'exit\n' >&8
    sleep 1
    stop_all
}

echo "[region-tau-env] build: $BOOT/p-kernel  logs=$LOGDIR"

run_arm default ""
L="$LOGDIR/default/node1.log"
grep -aq "node 1 discovered\|node 1 -> ALIVE\|node 1 recovered" "$L" && ok "default: mesh formed" || bad "default: node 1 never saw node 2"
grep -aq "\[net\] region tau=" "$L" && bad "default: env line printed without env" || ok "default: no env line"
grep -aq "\[region\] .*tau=50ms" "$L" && ok "default: region prints tau=50ms" || bad "default: no 'tau=50ms' region line"
grep -aq "\[region\] .*size=2" "$L" && bad "default: peer admitted although its rtt is above 50 ms" || ok "default: peer stays outside the region (size=1)"

run_arm env 400
L="$LOGDIR/env/node1.log"
grep -aq "node 1 discovered\|node 1 -> ALIVE\|node 1 recovered" "$L" && ok "env: mesh formed" || bad "env: node 1 never saw node 2"
grep -aq "\[net\] region tau=400ms (env" "$L" && ok "env: override read" || bad "env: override line missing or not 400"
grep -aq "\[region\] .*size=2  tau=400ms" "$L" && ok "env: region prints size=2 tau=400ms" || bad "env: no 'size=2  tau=400ms' region line"
R=$(grep -aoE "node1\(rtt=[0-9]+ms\)" "$L" | tail -1 | sed -E 's/.*rtt=([0-9]+)ms.*/\1/')
if [ -n "$R" ] && [ "$R" -ge 120 ]; then ok "env: admitted peer has rtt=${R}ms (> the default 50)"
else bad "env: admitted peer rtt '${R:-none}' is not >= 120 ms"; fi

echo "[region-tau-env] RESULT: $PASS PASS / $FAIL FAIL"
[ "$FAIL" = 0 ] && { echo "[region-tau-env] ALL PASS"; exit 0; }
exit 1
