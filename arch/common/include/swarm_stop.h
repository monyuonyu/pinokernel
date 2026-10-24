/*
 *  swarm_stop.h — S2: a human stops the swarm with a signed command
 *  (docs/architecture/30-module/swarm-stop.md, ROADMAP 0-14, global rule 9).
 *  This path is in rule 5's "AI must not change": the stop-key table is
 *  filled from the console only and an empty table refuses every command.
 */
#ifndef SWARM_STOP_H
#define SWARM_STOP_H

#include "kernel.h"
#include "ed25519.h"

#define SWARM_STOP_VER        1
#define SWARM_STOP_OP_STOP    1
#define SWARM_STOP_OP_RESUME  2
#define SWARM_STOP_KEYS_MAX   4
#define SWARM_STOP_BODY_LEN   (6 + 1 + 1 + 4)                   /* name, ver, op, epoch */
#define SWARM_STOP_CMD_LEN    (SWARM_STOP_BODY_LEN + ED25519_PUBLIC_KEY_LEN + ED25519_SIGNATURE_LEN)

/* wire image (108 bytes): "PKSTOP" | ver | op | epoch (LE) | signer pk | sig.
 * The signature covers everything before it (body + signer pk). */
typedef struct {
    U1 b[SWARM_STOP_CMD_LEN];
} SWARM_STOP_CMD;

/* results of swarm_stop_apply */
#define SWARM_STOP_OK          0
#define SWARM_STOP_E_HEAD    (-1)   /* name or version wrong          */
#define SWARM_STOP_E_OP      (-2)   /* unknown op                     */
#define SWARM_STOP_E_AUTH    (-3)   /* signer not in the stop table   */
#define SWARM_STOP_E_SIG     (-4)   /* signature does not verify      */
#define SWARM_STOP_E_REPLAY  (-5)   /* epoch not above the last seen  */

INT  swarm_stop_key_add(const U1 pk[ED25519_PUBLIC_KEY_LEN]);   /* console only */
void swarm_stop_key_clear(void);
INT  swarm_stop_key_count(void);
void swarm_stop_make(U1 op, U4 epoch, const U1 pk[ED25519_PUBLIC_KEY_LEN],
                     const U1 sk[ED25519_SECRET_KEY_LEN], SWARM_STOP_CMD *out);
INT  swarm_stop_apply(const U1 *cmd, UW len);
INT  swarm_stop_is_stopped(void);
void swarm_stop_reset(void);                                     /* self-test only */
INT  swarm_stop_selftest(void (*out)(const char *));             /* [swarm-stop] */
/* stage 2a: the console's `stop [status|now|resume|key <hex>|key clear|in <hex>|apply]`.
 * `stop now` / `stop resume` are this node's own stop (any time, no key); a
 * signed stop is lifted only by a signed resume. swarm_stop_is_stopped() is
 * 1 while either holds. -DSWARM_STOP_TEST_SIGN adds `stop sign` (tests). */
void swarm_stop_cmd(const UB *a, UW n, void (*out)(const char *));
/* [live]: take a command from "swarm/stop", send the last accepted one on;
 * one call per tick (mind_net_task) */
void swarm_stop_net_tick(void (*out)(const char *));
/* persist: read PKERNEL_PFS_DIR/swarm-stop.st at boot (a bad file: stopped here) */
void swarm_stop_restore(void (*out)(const char *));
INT  swarm_stop_refuses_mind(const UB *a, UW n);   /* stopped and `mind teach` */

#endif
