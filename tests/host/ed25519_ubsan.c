/* tests/host/ed25519_ubsan.c — run by run_ed25519_ubsan.sh under
 * -fsanitize=undefined: the RFC 8032 KATs (ed25519_self_test) and 300 random
 * sign/verify round trips (fixed xorshift seed) with a one-byte-flip reject.
 * Prints a digest of all signatures so the output can be compared across
 * builds. Exit 0 only when every check passed. */
#include <stdio.h>
#include <string.h>
#include "ed25519.h"

static unsigned long long s = 0x9e3779b97f4a7c15ULL;
static unsigned char rb(void) { s ^= s << 13; s ^= s >> 7; s ^= s << 17; return (unsigned char)s; }

int main(void)
{
    int st = ed25519_self_test();
    unsigned char acc[64] = {0};
    int bad = 0;
    for (int n = 0; n < 300; n++) {
        unsigned char seed[32], pk[32], sk[64], msg[96], sig[64], h[64];
        for (int i = 0; i < 32; i++) seed[i] = rb();
        int len = rb() % 96;
        for (int i = 0; i < len; i++) msg[i] = rb();
        ed25519_keypair_from_seed(seed, pk, sk);
        ed25519_sign(sig, msg, (size_t)len, sk);
        if (ed25519_verify(sig, msg, (size_t)len, pk) != 1) bad++;
        sig[n % 64] ^= 1;
        if (ed25519_verify(sig, msg, (size_t)len, pk) != 0) bad++;
        sig[n % 64] ^= 1;
        for (int i = 0; i < 64; i++) acc[i] ^= (unsigned char)(sig[i] + n);
        ed25519_sha512(acc, 64, h);
        memcpy(acc, h, 64);
    }
    printf("[ed25519-ubsan] self_test=%d roundtrip_bad=%d digest=", st, bad);
    for (int i = 0; i < 16; i++) printf("%02x", acc[i]);
    printf("\n");
    return st != 0 || bad != 0;
}
