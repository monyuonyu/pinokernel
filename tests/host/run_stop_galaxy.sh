#!/bin/bash
# tests/host/run_stop_galaxy.sh — ROADMAP 0-14 S2 stage 2b: galaxy's POST
# /teach obeys the stop (docs/architecture/30-module/swarm-stop.md, "2b").
#
# Expectations fixed before the code (one hosted node, galaxy on a fixed port,
# driven by tests/host/stop_galaxy.py):
#   G0 the page answers GET /manifesto     G1 POST /profile ack=1&mid=<id> -> 200
#   G2 POST /teach k=sky&v=blue -> 200 {"ok":true}
#   G3 after `stop now` at the console: POST /teach k=sea&v=green -> 403
#      {"refused":"stopped",...}
#   G4 after `stop resume`: POST /teach k=ink&v=fresh -> 200
#   G5 `mind provenance` lists sky and ink, not sea
# The signed arm (audit-59's limit; the test build -DSWARM_STOP_TEST_SIGN signs
# a stop at epoch 5 and a resume at epoch 6, as run_stop_console.sh does):
#   G6 after `stop key <pk>` and the pasted signed stop: POST /teach -> 403
#   G7 a local `stop resume` keeps the signed stop: POST /teach -> 403
#   G8 after the pasted signed resume: POST /teach k=fire&v=warm -> 200
#   G9 `mind provenance` lists fire, not leaf
# Falsifier: -DGALAXY_NO_STOP_GATE (the web mouth does not look at the stop)
# must turn G3, G5, G6, G7 and G9 red (run on the signed build).
# Output: "[stop-galaxy] <check> PASS|FAIL", then ALL PASS (exit 0) or RED.
# Rebuilds boot/linux* with EXTRA_CFLAGS and cleans it afterwards.
set -u
HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
case "$(uname -m)" in
    aarch64|arm64) BOOT="$ROOT/boot/linux" ;;
    *)             BOOT="$ROOT/boot/linux_x86_64" ;;
esac
unset PKERNEL_RELAY PKERNEL_RELAY_HOST PKERNEL_RELAY_PORT PKERNEL_LAN PKERNEL_AUTONET PKERNEL_GALAXY
PORT="${STOP_GALAXY_PORT:-17811}"
WORK="$(mktemp -d /tmp/stopg.XXXXXX)"
trap 'rm -rf "$WORK"' EXIT
REDS=""
SIGNED=""

build() {  # <name> <cflags>
    make -C "$BOOT" clean >/dev/null 2>&1
    if ! make -C "$BOOT" EXTRA_CFLAGS="$2" > "$WORK/$1.build" 2>&1; then
        tail -20 "$WORK/$1.build"; echo "[stop-galaxy] $1 build FAIL"; return 1
    fi
}
arm() {  # <name> <cflags>
    build "$1" "$2" || return 1
    mkdir -p "$WORK/$1"
    # shellcheck disable=SC2086
    timeout 300 python3 "$HERE/stop_galaxy.py" "$BOOT/p-kernel" "$WORK/$1" "$PORT" $SIGNED > "$WORK/$1.out" 2>&1
    sed "s/^\[stop-galaxy\] /[stop-galaxy] $1 /" "$WORK/$1.out"
}
need() {  # <arm> <check>...: every check PASS in that arm
    local a="$1"; shift
    for c in "$@"; do
        grep -q "^\[stop-galaxy\] $c PASS" "$WORK/$a.out" || REDS="$REDS $a-$c"
    done
}
is() {  # <arm> <check> <PASS|FAIL>
    grep -q "^\[stop-galaxy\] $2 $3" "$WORK/$1.out"
}

arm plain ""
need plain G0-up G1-consent G2-teach G3-refused G4-teach G5-facts

# ---- the signed commands, from the test build ---------------------------------
if build sign "-DSWARM_STOP_TEST_SIGN"; then
    mkdir -p "$WORK/sign"
    printf 'stop sign 1 5 90\nstop sign 2 6 90\nexit\n' \
        | timeout 180 env PKERNEL_NODE_ID=1 PKERNEL_PFS_DIR="$WORK/sign" "$BOOT/p-kernel" > "$WORK/sign.log" 2>&1
fi
PK=$(grep -a '^\[stop\] pk ' "$WORK/sign.log" 2>/dev/null | head -1 | tr -d '\r' | awk '{print $3}')
CS=$(grep -a '^\[stop\] cmd ' "$WORK/sign.log" 2>/dev/null | sed -n 1p | tr -d '\r' | awk '{print $3}')
CR=$(grep -a '^\[stop\] cmd ' "$WORK/sign.log" 2>/dev/null | sed -n 2p | tr -d '\r' | awk '{print $3}')
if [ "${#PK}" = 64 ] && [ "${#CS}" = 216 ] && [ "${#CR}" = 216 ]; then
    echo "[stop-galaxy] signed-cmds PASS"
else
    echo "[stop-galaxy] signed-cmds FAIL"; REDS="$REDS signed-cmds"
fi
SIGNED="$PK $CS $CR"

arm signed "-DSWARM_STOP_TEST_SIGN"
need signed G0-up G1-consent G2-teach G3-refused G4-teach G5-facts \
    G6-signed-refused G7-local-resume-keeps G8-signed-resume G9-facts

arm nc "-DSWARM_STOP_TEST_SIGN -DGALAXY_NO_STOP_GATE"
if is nc G3-refused FAIL && is nc G5-facts FAIL && is nc G6-signed-refused FAIL &&
   is nc G7-local-resume-keeps FAIL && is nc G9-facts FAIL && is nc G2-teach PASS; then
    echo "[stop-galaxy] nc-no-gate PASS"
else
    echo "[stop-galaxy] nc-no-gate FAIL"; REDS="$REDS nc-no-gate"
fi
make -C "$BOOT" clean >/dev/null 2>&1
cp "$WORK/signed/node.log" /tmp/stop_galaxy.log 2>/dev/null
echo "[stop-galaxy] log: /tmp/stop_galaxy.log"
if [ -z "$REDS" ]; then echo "[stop-galaxy] ALL PASS"; exit 0; fi
echo "[stop-galaxy] RED:$REDS"; exit 1
