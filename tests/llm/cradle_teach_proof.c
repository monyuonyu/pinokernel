/*
 *  cradle_teach_proof.c — the [cradle-teach] cert (Thread T, T-fix-b / T-1).
 *                         (docs/architecture/thread-t-impl-plan.md §1.)
 *
 *  Standalone HOST harness for the teacher->student lesson bridge. It links the
 *  REAL production seam — arch/common/llm/cradle.c (the lesson ring + ingest +
 *  the cert) and arch/common/llm/student.c (the math) — and drives
 *  cradle_teach_self_test(), which:
 *
 *    - composes a lesson with a coined fact's TRAINED copy in the train region
 *      and a DISTINCT never-trained occurrence in the tail held region (the
 *      killer-objection fix, §1.2);
 *    - ingests it via the REAL cradle_lesson_ingest into the ring, trains B over
 *      the REAL window()/sleep math, and asserts B KNOWS the held-out probe it
 *      was NEVER directly trained on (generalization, weight-resident);
 *    - runs THREE falsification arms (§1.3): teaching-OFF (fixture fallback),
 *      scrambled-bytes (random LCG), never-taught probe — each must stay at
 *      chance (go RED).
 *
 *  Build (mirrors distill_proof.c):
 *    cc -std=c11 -O1 -ffp-contract=off cradle_teach_proof.c \
 *       ../../arch/common/llm/cradle.c ../../arch/common/llm/student.c \
 *       ../../arch/common/llm/held_indep.c \
 *       -o cradle_teach_proof
 *
 *  Exit 0 iff the cert PASSES (B learns the held probe AND all 3 arms hold).
 *
 *  HONEST [in-proc] vs [live]: this cert is IN-PROCESS (one process, the ring
 *  fed directly — the transport's pfs_dag/KDDS carrier is exercised in-kernel by
 *  cradle_net.c, NOT here). The true MULTI-PROCESS live teacher-convergence over
 *  ./relay is a DEFERRED [live] row (like SS-6 -> SS-6-live). The live in-kernel
 *  GGUF teacher HARVEST is also DEFERRED (CT-2): this cert seeds the teacher
 *  corpus. The BRIDGE + the student ingestion is the deliverable proven here.
 */
#include <stdio.h>
#include <stdint.h>
#include "../../arch/common/llm/student.h"
#include "../../arch/common/llm/held_indep.h"

static void emit(const char *s) { fputs(s, stdout); }

int main(void)
{
    setvbuf(stdout, NULL, _IONBF, 0);   /* stream progress (no block-buffer wait) */
    printf("=== cradle_teach_proof — the lesson BRIDGE cert (in-proc) ===\n");
    int fails = cradle_teach_self_test(emit);

    /* 0-12 step 3: the cert's held numbers mean "never trained on" only if
     * the held windows share no bytes with the train windows. Same bytes
     * (cradle_compose_canon) and the same split (st_held_split) as the cert. */
    static uint8_t canon[CRADLE_RING_BYTES];
    int n = cradle_compose_canon(canon, (int)sizeof canon, NULL);
    st_split sp = st_held_split(n > 0 ? n : 1, CRADLE_SEQLEN);
    st_held_report hr = { 0, 0, 0 };
    int hrc = n > 0 ? st_held_check(canon, n, CRADLE_SEQLEN, sp.trainw, sp.heldw, ST_HELD_K, &hr) : -1;
    printf("[held-indep] shared=%d/%d held %d-byte runs also in train, wrapped=%d -> %s\n",
           hr.shared, hr.held_kgrams, ST_HELD_K, hr.wrapped,
           hrc == 0 ? "independent" : "NOT independent (the held loss partly measures memory)");
    if (hrc != 0) fails++;
    printf("\n");
    if (fails == 0) {
        printf("RESULT: PASS — teacher->student bridge teaches a held-out probe "
               "weight-resident; 3 falsifiers RED.\n");
        return 0;
    }
    printf("RESULT: FAIL — %d assertion(s) failed.\n", fails);
    return 1;
}
