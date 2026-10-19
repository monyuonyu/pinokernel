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
# Falsifier: -DGALAXY_NO_STOP_GATE (the web mouth does not look at the stop)
# must turn G3 and G5 red.
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

arm() {  # <name> <cflags>
    make -C "$BOOT" clean >/dev/null 2>&1
    if ! make -C "$BOOT" EXTRA_CFLAGS="$2" > "$WORK/$1.build" 2>&1; then
        tail -20 "$WORK/$1.build"; echo "[stop-galaxy] $1 build FAIL"; return 1
    fi
    mkdir -p "$WORK/$1"
    timeout 300 python3 "$HERE/stop_galaxy.py" "$BOOT/p-kernel" "$WORK/$1" "$PORT" > "$WORK/$1.out" 2>&1
    sed "s/^\[stop-galaxy\] /[stop-galaxy] $1 /" "$WORK/$1.out"
}

arm plain ""
for c in G0-up G1-consent G2-teach G3-refused G4-teach G5-facts; do
    grep -q "^\[stop-galaxy\] $c PASS" "$WORK/plain.out" || REDS="$REDS $c"
done
arm nc "-DGALAXY_NO_STOP_GATE"
if grep -q '^\[stop-galaxy\] G3-refused FAIL' "$WORK/nc.out" && grep -q '^\[stop-galaxy\] G5-facts FAIL' "$WORK/nc.out" &&
   grep -q '^\[stop-galaxy\] G2-teach PASS' "$WORK/nc.out"; then
    echo "[stop-galaxy] nc-no-gate PASS"
else
    echo "[stop-galaxy] nc-no-gate FAIL"; REDS="$REDS nc-no-gate"
fi
make -C "$BOOT" clean >/dev/null 2>&1
cp "$WORK/plain/node.log" /tmp/stop_galaxy.log 2>/dev/null
echo "[stop-galaxy] log: /tmp/stop_galaxy.log"
if [ -z "$REDS" ]; then echo "[stop-galaxy] ALL PASS"; exit 0; fi
echo "[stop-galaxy] RED:$REDS"; exit 1
