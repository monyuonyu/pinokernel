/*
 *  swarm_stop.c — S2: a human stops the swarm with a signed command
 *  (docs/architecture/30-module/swarm-stop.md, ROADMAP 0-14, global rule 9).
 *
 *  Stage 1 (in-proc): the command format, the stop-key table, the accept rule
 *  and the [swarm-stop] self-test (`tkconf stop` on hosted). Nothing reads
 *  swarm_stop_is_stopped() yet and nothing forwards the command (stage 2).
 *
 *  Rule 5: the stop-key table is filled from the console only; an EMPTY table
 *  refuses every command (fail-closed). Separate from the code-adoption
 *  allowlist (sign_allow_*): a key that may install code is not, by that, a
 *  key that may stop the swarm.
 *
 *  Falsifiers (expected red): -DSWARM_STOP_NO_SIGCHECK (T3),
 *  -DSWARM_STOP_NO_REPLAY (T4), -DSWARM_STOP_NO_AUTH (T2, T5).
 */

#include "swarm_stop.h"
#include "sign.h"

static U1  ss_keys[SWARM_STOP_KEYS_MAX][ED25519_PUBLIC_KEY_LEN];
static INT ss_nkeys   = 0;
static INT ss_stopped = 0;
static U4  ss_epoch   = 0;            /* highest epoch accepted so far */

static INT ss_eq(const U1 *a, const U1 *b, UW n)
{
    UW i; U1 d = 0;
    for (i = 0; i < n; i++) d |= (U1)(a[i] ^ b[i]);
    return d == 0;
}

INT swarm_stop_key_add(const U1 pk[ED25519_PUBLIC_KEY_LEN])
{
    INT i;
    for (i = 0; i < ss_nkeys; i++)
        if (ss_eq(ss_keys[i], pk, ED25519_PUBLIC_KEY_LEN)) return 1;
    if (ss_nkeys >= SWARM_STOP_KEYS_MAX) return 0;
    for (i = 0; i < ED25519_PUBLIC_KEY_LEN; i++) ss_keys[ss_nkeys][i] = pk[i];
    ss_nkeys++;
    return 1;
}

void swarm_stop_key_clear(void) { ss_nkeys = 0; }
INT  swarm_stop_key_count(void) { return ss_nkeys; }
INT  swarm_stop_is_stopped(void) { return ss_stopped; }
void swarm_stop_reset(void) { ss_stopped = 0; ss_epoch = 0; }

void swarm_stop_make(U1 op, U4 epoch, const U1 pk[ED25519_PUBLIC_KEY_LEN],
                     const U1 sk[ED25519_SECRET_KEY_LEN], SWARM_STOP_CMD *out)
{
    static const U1 name[6] = { 'P','K','S','T','O','P' };
    U1 *b = out->b;
    INT i;
    for (i = 0; i < 6; i++) b[i] = name[i];
    b[6] = SWARM_STOP_VER;
    b[7] = op;
    b[8]  = (U1)(epoch);       b[9]  = (U1)(epoch >> 8);
    b[10] = (U1)(epoch >> 16); b[11] = (U1)(epoch >> 24);
    for (i = 0; i < ED25519_PUBLIC_KEY_LEN; i++) b[SWARM_STOP_BODY_LEN + i] = pk[i];
    ed25519_sign(b + SWARM_STOP_BODY_LEN + ED25519_PUBLIC_KEY_LEN,
                 b, SWARM_STOP_BODY_LEN + ED25519_PUBLIC_KEY_LEN, sk);
}

/* accept iff (all): name+version / known op / signer in the stop table /
 * signature over body+signer / epoch above the last accepted one */
INT swarm_stop_apply(const U1 *cmd, UW len)
{
    static const U1 name[6] = { 'P','K','S','T','O','P' };
    const U1 *pk, *sig;
    U4 epoch;
    INT i, known = 0;

    if (len != SWARM_STOP_CMD_LEN) return SWARM_STOP_E_HEAD;
    if (!ss_eq(cmd, name, 6) || cmd[6] != SWARM_STOP_VER) return SWARM_STOP_E_HEAD;
    if (cmd[7] != SWARM_STOP_OP_STOP && cmd[7] != SWARM_STOP_OP_RESUME) return SWARM_STOP_E_OP;
    pk  = cmd + SWARM_STOP_BODY_LEN;
    sig = pk + ED25519_PUBLIC_KEY_LEN;
#ifndef SWARM_STOP_NO_AUTH
    for (i = 0; i < ss_nkeys; i++)
        if (ss_eq(ss_keys[i], pk, ED25519_PUBLIC_KEY_LEN)) { known = 1; break; }
    if (!known) return SWARM_STOP_E_AUTH;
#else
    (void)i; (void)known;
#endif
#ifndef SWARM_STOP_NO_SIGCHECK
    if (!sign_verify(cmd, SWARM_STOP_BODY_LEN + ED25519_PUBLIC_KEY_LEN, sig, pk))
        return SWARM_STOP_E_SIG;
#else
    (void)sig;
#endif
    epoch = (U4)cmd[8] | ((U4)cmd[9] << 8) | ((U4)cmd[10] << 16) | ((U4)cmd[11] << 24);
#ifndef SWARM_STOP_NO_REPLAY
    if (epoch <= ss_epoch) return SWARM_STOP_E_REPLAY;
#endif
    ss_epoch   = epoch;
    ss_stopped = (cmd[7] == SWARM_STOP_OP_STOP);
    return SWARM_STOP_OK;
}

/* ------------------------------------------------------------------ */
/* [swarm-stop] self-test — expectations fixed in swarm-stop.md        */
/* ------------------------------------------------------------------ */
static void (*ss_out)(const char *);
static INT ss_pass, ss_fail;

static void ss_dec(INT v)
{
    char b[12]; INT i = 11; UW u = (UW)(v < 0 ? -v : v);
    b[11] = 0;
    do { b[--i] = (char)('0' + u % 10); u /= 10; } while (u && i > 1);
    if (v < 0) b[--i] = '-';
    ss_out(b + i);
}

static void ss_check(const char *id, const char *what, INT ok, INT got)
{
    ss_out("[swarm-stop] "); ss_out(id); ss_out(ok ? " PASS " : " FAIL ");
    ss_out(what); ss_out(" (rc "); ss_dec(got); ss_out(")\r\n");
    if (ok) ss_pass++; else ss_fail++;
}

static void ss_kp(U1 seed_byte, U1 pk[32], U1 sk[64])
{
    U1 seed[32]; INT i;
    for (i = 0; i < 32; i++) seed[i] = (U1)(seed_byte + i * 29u);
    ed25519_keypair_from_seed(seed, pk, sk);
}

INT swarm_stop_selftest(void (*out)(const char *))
{
    static U1 apk[32], ask[64], epk[32], esk[64];
    static SWARM_STOP_CMD c;
    INT r, keys_were = ss_nkeys, stopped_was = ss_stopped;
    U4 epoch_was = ss_epoch;
    static U1 keys_save[SWARM_STOP_KEYS_MAX][ED25519_PUBLIC_KEY_LEN];
    INT i, j;
    for (i = 0; i < ss_nkeys; i++)
        for (j = 0; j < ED25519_PUBLIC_KEY_LEN; j++) keys_save[i][j] = ss_keys[i][j];

    ss_out = out; ss_pass = ss_fail = 0;
    ss_kp(0x5a, apk, ask);              /* the stop authority          */
    ss_kp(0x77, epk, esk);              /* someone not in the table    */

    /* T5 first: an empty table refuses a well-signed command */
    swarm_stop_key_clear(); swarm_stop_reset();
    swarm_stop_make(SWARM_STOP_OP_STOP, 1, apk, ask, &c);
    r = swarm_stop_apply(c.b, SWARM_STOP_CMD_LEN);
    ss_check("T5", "empty stop table refuses a signed stop", r != SWARM_STOP_OK && !swarm_stop_is_stopped(), r);

    swarm_stop_key_add(apk);
    /* T2 a key not in the table */
    swarm_stop_reset();
    swarm_stop_make(SWARM_STOP_OP_STOP, 1, epk, esk, &c);
    r = swarm_stop_apply(c.b, SWARM_STOP_CMD_LEN);
    ss_check("T2", "stop signed by a key not in the table is refused", r != SWARM_STOP_OK && !swarm_stop_is_stopped(), r);

    /* T3 one byte changed (the epoch, 2 -> 258: still above the last, so
     * only the signature can refuse it) after signing */
    swarm_stop_reset();
    swarm_stop_make(SWARM_STOP_OP_STOP, 2, apk, ask, &c);
    c.b[9] ^= 0x01;
    r = swarm_stop_apply(c.b, SWARM_STOP_CMD_LEN);
    ss_check("T3", "stop with one byte changed is refused", r != SWARM_STOP_OK && !swarm_stop_is_stopped(), r);

    /* T6 wrong name / wrong version (re-signed, so only the head is wrong) */
    swarm_stop_reset();
    swarm_stop_make(SWARM_STOP_OP_STOP, 1, apk, ask, &c);
    c.b[0] = 'X';
    ed25519_sign(c.b + SWARM_STOP_BODY_LEN + 32, c.b, SWARM_STOP_BODY_LEN + 32, ask);
    r = swarm_stop_apply(c.b, SWARM_STOP_CMD_LEN);
    INT t6a = (r != SWARM_STOP_OK && !swarm_stop_is_stopped());
    swarm_stop_make(SWARM_STOP_OP_STOP, 1, apk, ask, &c);
    c.b[6] = SWARM_STOP_VER + 1;
    ed25519_sign(c.b + SWARM_STOP_BODY_LEN + 32, c.b, SWARM_STOP_BODY_LEN + 32, ask);
    r = swarm_stop_apply(c.b, SWARM_STOP_CMD_LEN);
    ss_check("T6", "wrong name or version is refused", t6a && r != SWARM_STOP_OK && !swarm_stop_is_stopped(), r);

    /* short packet */
    r = swarm_stop_apply(c.b, SWARM_STOP_CMD_LEN - 1);
    ss_check("T6b", "a short command is refused", r != SWARM_STOP_OK && !swarm_stop_is_stopped(), r);

    /* T1 the authority's stop works */
    swarm_stop_reset();
    swarm_stop_make(SWARM_STOP_OP_STOP, 5, apk, ask, &c);
    r = swarm_stop_apply(c.b, SWARM_STOP_CMD_LEN);
    ss_check("T1", "a stop signed by the table's key stops the node", r == SWARM_STOP_OK && swarm_stop_is_stopped(), r);

    /* T4 the same command again, and an older epoch, after a resume */
    swarm_stop_make(SWARM_STOP_OP_RESUME, 6, apk, ask, &c);
    swarm_stop_apply(c.b, SWARM_STOP_CMD_LEN);
    swarm_stop_make(SWARM_STOP_OP_STOP, 5, apk, ask, &c);
    r = swarm_stop_apply(c.b, SWARM_STOP_CMD_LEN);
    ss_check("T4", "a replayed stop (epoch not above the last) is refused", r != SWARM_STOP_OK && !swarm_stop_is_stopped(), r);

    /* T7 resume: only a signed resume with a higher epoch */
    swarm_stop_make(SWARM_STOP_OP_STOP, 7, apk, ask, &c);
    swarm_stop_apply(c.b, SWARM_STOP_CMD_LEN);                 /* stopped at 7 */
    swarm_stop_make(SWARM_STOP_OP_RESUME, 8, epk, esk, &c);
    r = swarm_stop_apply(c.b, SWARM_STOP_CMD_LEN);
    INT t7a = (r != SWARM_STOP_OK && swarm_stop_is_stopped());
    swarm_stop_make(SWARM_STOP_OP_RESUME, 7, apk, ask, &c);
    r = swarm_stop_apply(c.b, SWARM_STOP_CMD_LEN);
    INT t7b = (r != SWARM_STOP_OK && swarm_stop_is_stopped());
    swarm_stop_make(SWARM_STOP_OP_RESUME, 9, apk, ask, &c);
    r = swarm_stop_apply(c.b, SWARM_STOP_CMD_LEN);
    ss_check("T7", "resume needs the table's key and a higher epoch", t7a && t7b && r == SWARM_STOP_OK && !swarm_stop_is_stopped(), r);

    /* put the node back as it was */
    swarm_stop_key_clear();
    for (i = 0; i < keys_were; i++) swarm_stop_key_add(keys_save[i]);
    ss_stopped = stopped_was; ss_epoch = epoch_was;

    out("[swarm-stop] "); ss_dec(ss_pass); out(" PASS / "); ss_dec(ss_fail); out(" FAIL\r\n");
    return ss_fail == 0 ? 0 : -1;
}
