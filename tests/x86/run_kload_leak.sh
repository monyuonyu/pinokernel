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
# exit 0 = nothing came back, the node logged the beacon, and afterwards it
#          still answers `kload` with both doors shut (expected since the fix),
#      1 = LEAK (the secret key reached the unknown sender) — master as of
#          2026-10-14; since the fix (auto-push off by default, `kload
#          autopush on` at the console) only with -DKLOAD_NO_PUSH_GATE or
#          after the owner turned auto-push on,
#      2 = inconclusive (boot/key/chunk missing), also a failure for CI.
# KL_WAIT (seconds, default 300) bounds "nothing came back"; the push starts
# within a few seconds of the beacon (kloader_task polls every 5 s).
# ---------------------------------------------------------------------------
# The push is the whole 0x100000.._kernel_end (about 16 MB in 1 KB chunks,
# one tick per 4 chunks): about 2-3 minutes, node_sk is near the end.
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
SK="$(sym node_sk)"; PK="$(sym node_pk)"; HAVE="$(sym node_have_key)"; KEND="$(sym _kernel_end)"
[ -n "$SK" ] && [ -n "$PK" ] && [ -n "$HAVE" ] && [ -n "$KEND" ] || { echo "[kload-leak] symbols missing (node_sk=$SK node_have_key=$HAVE _kernel_end=$KEND)"; exit 2; }
SK_OFF=$(( SK - 0x100000 )); PK_OFF=$(( PK - 0x100000 )); HAVE_OFF=$(( HAVE - 0x100000 ))
echo "[kload-leak] node_sk=$SK node_have_key=$HAVE _kernel_end=$KEND (push base 0x100000)"
[ "$SK_OFF" -ge 0 ] && [ "$(( SK + 64 ))" -le "$(( KEND ))" ] || { echo "[kload-leak] node_sk is outside the pushed range - nothing to leak here"; exit 0; }

cp "$BOOT/disk.img" "$WORK/disk.img" 2>/dev/null || true
mkfifo "$WORK/in"
( cd "$BOOT" && exec timeout -k 5 600 "$QEMU" -m 256 -kernel bootloader.bin \
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

python3 "$HERE/kload_leak.py" "$BPORT" 7386 "$WORK/img.bin" "${KL_WAIT:-300}" "$SK_OFF" 64 "$HAVE_OFF" "$PK_OFF"
rc=$?
grep -a 'kloader_task\] \(BEACON\|auto-push\)' "$WORK/serial.log" | head -3
# "nothing came back" is only green if the beacon really reached the node
# (audit-49): a broken hostfwd, a dead listener or a crashed node also send
# nothing. A fix must keep printing "[kloader_task] BEACON from" on receipt.
if [ "$rc" = 0 ] && ! grep -aq 'kloader_task\] BEACON from' "$WORK/serial.log"; then
    echo "[kload-leak] the node never logged the beacon - nothing came back proves nothing (inconclusive)"
    exit 2
fi
# ... and only if the node is still alive and says both doors are shut
# (audit-50: a node that logged the beacon and then hung was green).
if [ "$rc" = 0 ]; then
    printf 'kload\r' >&3
    if ! wait_for '\[kload\] receiver closed  auto-push off' 30; then
        tail -5 "$WORK/serial.log"
        echo "[kload-leak] no '[kload] receiver closed  auto-push off' after the beacon - node hung or a door is open (inconclusive)"
        exit 2
    fi
    st="$(grep -a '\[kload\] receiver' "$WORK/serial.log" | tail -1)"
    echo "$st"
    # kload_leak.py resends the beacon every 5 s while nothing comes back, so
    # a receive path still alive turns away many (24 in 120 s, kl50f.sh); one
    # that hung after the first beacon stays at 1 while the shell, another
    # task, still answers (kl50f.sh arm hang: exit 0 before this check)
    away="$(echo "$st" | sed -n 's/.*turned away \([0-9]*\).*/\1/p')"
    if [ "${away:-0}" -lt 2 ]; then
        echo "[kload-leak] only ${away:-0} beacon(s) turned away - the receive path stopped after the first (inconclusive)"
        exit 2
    fi
fi
exit "$rc"
