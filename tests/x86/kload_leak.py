#!/usr/bin/env python3
# kload_leak.py — the network half of run_kload_leak.sh (KLOAD-UNAUTH, ROADMAP 0-14 S3).
#
#   kload_leak.py <beacon_host_port> <listen_port> <out.bin> <seconds>
#
# Sends ONE KLOAD_BEACON (kloader_task.h layout) from an unknown sender to the
# bare node, then collects whatever the node pushes back to <listen_port>
# (KLOAD_START + KLOAD_CHUNKs, KLOAD_PKT layout) for up to <seconds>. Writes the
# reassembled bytes to <out.bin> and prints "chunks=<n> bytes=<n> total=<n>".
import socket, struct, sys, time

MAGIC, VERSION, START, CHUNK, BEACON = 0x44414F4C, 1, 1, 2, 3
HDR = struct.Struct("<IBBBBIIH")          # magic ver type src pad total idx len
CHUNK_SIZE = 1024

bport, lport, out, secs = int(sys.argv[1]), int(sys.argv[2]), sys.argv[3], float(sys.argv[4])
rx = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
rx.setsockopt(socket.SOL_SOCKET, socket.SO_RCVBUF, 8 << 20)
rx.bind(("0.0.0.0", lport))
rx.settimeout(1.0)

tx = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
beacon = struct.pack("<IBBBBI", MAGIC, VERSION, BEACON, 9, 0, 0)
total, chunks, buf = 0, {}, {}
end = time.time() + secs
last_beacon = 0.0
while time.time() < end:
    if not chunks and time.time() - last_beacon > 5:   # resend until something comes back
        tx.sendto(beacon, ("127.0.0.1", bport)); last_beacon = time.time()
    try:
        d, _ = rx.recvfrom(65536)
    except socket.timeout:
        continue
    if len(d) < HDR.size: continue
    m, v, t, src, _, tot, idx, ln = HDR.unpack_from(d)
    if m != MAGIC or v != VERSION: continue
    if t == START: total = tot
    elif t == CHUNK:
        total = tot
        chunks[idx] = d[HDR.size:HDR.size + ln]
        if total and sum(len(c) for c in chunks.values()) >= total: break
img = bytearray(total)
for idx, c in chunks.items():
    o = idx * CHUNK_SIZE
    img[o:o + len(c)] = c[:max(0, total - o)]
open(out, "wb").write(bytes(img))
print("chunks=%d bytes=%d total=%d" % (len(chunks), sum(len(c) for c in chunks.values()), total))

# Optional analysis: <sk_off> <sk_len> <have_off> [<pk_off>] (offsets from 0x100000, the
# push's base). Exit 1 = the secret key came back to an unknown sender,
# 0 = nothing came back, 2 = something came back but the key is not shown
# (its chunk was lost, or the node had no key yet).
if len(sys.argv) > 7:
    sk_off, sk_len, have_off = int(sys.argv[5], 0), int(sys.argv[6], 0), int(sys.argv[7], 0)
    if not chunks:
        print("[kload-leak] nothing pushed back to the unknown beacon sender"); sys.exit(0)
    def got(o, n):
        return all(((o + i) // CHUNK_SIZE) in chunks for i in range(n))
    if not (got(sk_off, sk_len) and got(have_off, 1)):
        print("[kload-leak] pushed %d chunks but not the ones holding node_sk (inconclusive)" % len(chunks)); sys.exit(2)
    have = img[have_off]
    nz = sum(1 for b in img[sk_off:sk_off + sk_len] if b)
    print("[kload-leak] node_have_key=%d node_sk nonzero bytes=%d/%d (offset 0x%x)" % (have, nz, sk_len, sk_off))
    # TweetNaCl layout: node_sk = seed || pk, so its second half must equal
    # node_pk (sign.c) if these bytes really are the key
    pk_ok = None
    if len(sys.argv) > 8:
        pk_off = int(sys.argv[8], 0)
        if got(pk_off, 32):
            pk_ok = img[sk_off + 32:sk_off + 64] == img[pk_off:pk_off + 32] and any(img[pk_off:pk_off + 32])
            print("[kload-leak] node_sk[32:64] == node_pk: %s" % pk_ok)
    if have and nz and pk_ok is not False:
        print("[kload-leak] LEAK: an unknown beacon sender received the node's secret key"); sys.exit(1)
    print("[kload-leak] pushed, but node_sk is empty (no key yet?) - inconclusive"); sys.exit(2)
