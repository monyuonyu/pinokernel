#!/bin/bash
# ---------------------------------------------------------------------------
# run_kload_leak.sh — [kload-leak] (gap-ledger KLOAD-UNAUTH, ROADMAP 0-14 S3)
#
# Does a bare x86 node push its live RAM to anyone who sends one BEACON?
# kloader_beacon_rx (arch/common/kloader_task.c) queues the sender's IP with
# no check of who it is, and kloader_push_ip sends 0x100000.._kernel_end over
# plain UDP. On x86 that range holds .data/.bss, so sign.c's node_sk (the
# node's Ed25519 secret key) goes out too.
#
#   1. build boot/x86 (default kernel), read node_sk / node_have_key from
#      bootloader.bin (the ELF QEMU boots; see run_killchurn.sh on why)
#   2. boot QEMU as distributed node 0 (MAC 52:54:00:00:00:01 -> IP 10.1.0.1,
#      the only mode that starts the kloader; slirp on 10.1.0.0/24, hostfwd
#      udp <port> -> 10.1.0.1:7387), `sign test` so the node has a key
#   3. kload_leak.py sends ONE beacon from an unknown sender and collects the
#      push on udp 7386 (the guest sends to 10.1.0.100 = slirp = this host)
#
# exit 0 = nothing came back (expected once KLOAD checks the sender),
#      1 = LEAK (the secret key reached the unknown sender) — expected on
#          master as of 2026-10-14, the test is written before the fix,
#      2 = inconclusive (boot/key/chunk missing), also a failure for CI.
# ---------------------------------------------------------------------------
set -u
HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
BOOT="$ROOT/boot/x86"
QEMU="${QEMU:-qemu-system-x86_64}"
NM="${NM:-nm}"
BPORT="${KL_BEACON_PORT:-17387}"
WORK="$(mktemp -d)"
QPID=""
trap '[ -n "$QPID" ] && kill "$QPID" 2>/dev/null; exec 3>&- 2>/dev/null; [ -n "${KL_KEEP:-}" ] && cp "$WORK/serial.log" "$KL_KEEP" 2>/dev/null; rm -rf "$WORK"' EXIT

echo "[build] boot/x86 (default kernel) ..."
( cd "$BOOT" && make all disk ) > "$WORK/build.log" 2>&1 || { tail -20 "$WORK/build.log"; echo "[kload-leak] build FAILED"; exit 2; }

sym() { "$NM" "$BOOT/bootloader.bin" | awk -v s="$1" '$3==s {print "0x"$1}'; }
SK="$(sym node_sk)"; HAVE="$(sym node_have_key)"; KEND="$(sym _kernel_end)"
[ -n "$SK" ] && [ -n "$HAVE" ] && [ -n "$KEND" ] || { echo "[kload-leak] symbols missing (node_sk=$SK node_have_key=$HAVE _kernel_end=$KEND)"; exit 2; }
SK_OFF=$(( SK - 0x100000 )); HAVE_OFF=$(( HAVE - 0x100000 ))
echo "[kload-leak] node_sk=$SK node_have_key=$HAVE _kernel_end=$KEND (push base 0x100000)"
[ "$SK_OFF" -ge 0 ] && [ "$(( SK + 64 ))" -le "$(( KEND ))" ] || { echo "[kload-leak] node_sk is outside the pushed range - nothing to leak here"; exit 0; }

cp "$BOOT/disk.img" "$WORK/disk.img" 2>/dev/null || true
mkfifo "$WORK/in"
( cd "$BOOT" && exec timeout -k 5 300 "$QEMU" -m 256 -kernel bootloader.bin \
    -serial stdio -display none -cpu qemu64 \
    -netdev "user,id=n0,net=10.1.0.0/24,host=10.1.0.100,dhcpstart=10.1.0.50,hostfwd=udp:127.0.0.1:$BPORT-10.1.0.1:7387" \
    -device rtl8139,netdev=n0,mac=52:54:00:00:00:01 \
    -drive file="$WORK/disk.img",format=raw,if=ide,index=0 ) < "$WORK/in" > "$WORK/serial.log" 2>&1 &
QPID=$!
exec 3> "$WORK/in"

wait_for() {   # <pattern> <seconds>
    local n=0
    while ! grep -aq "$1" "$WORK/serial.log"; do
        sleep 1; n=$((n + 1))
        [ "$n" -ge "$2" ] && return 1
    done
    return 0
}
wait_for "BEACON listener ready" 120 || { tail -20 "$WORK/serial.log"; echo "[kload-leak] the node did not start the kloader"; exit 2; }
sleep 3
printf 'sign test\r' >&3
wait_for "node keygen entropy" 120 || { tail -20 "$WORK/serial.log"; echo "[kload-leak] no key (sign test did not run)"; exit 2; }

python3 "$HERE/kload_leak.py" "$BPORT" 7386 "$WORK/img.bin" 90 "$SK_OFF" 64 "$HAVE_OFF"
rc=$?
grep -a 'kloader_task\] \(BEACON\|auto-push\)' "$WORK/serial.log" | head -3
exit "$rc"
