/* d5c_parent_probe.c — why does parent B fail to learn on seed 2 (D5-c
 * stage 4)? Same corpus/windows/train_list as student_distill_merge_test.c.
 * Prints the own-data loss curve of A/B in S1 and S2 for 8 seeds at one LR.
 * A probe, not a cert (research/brain-merge-stable.md §1).
 *   cc -std=c11 -O1 -ffp-contract=off -Iarch/common/llm \
 *      tests/llm/d5c_parent_probe.c arch/common/llm/student.c -lm -o probe
 *   ./probe 0.02     # the stage-4 LR; ./probe 0.003 = ST_DMN_LR */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include "student.h"

#define SEQLEN 24
#define ROUNDS 40
static const uint8_t CORPUS[] =
    "the cat sat on the mat. the dog ran in the sun. she said the sea is blue "
    "and the sky is blue too. a bird sang on the old oak tree at dawn. the cat "
    "and the dog sat by the sea and saw the sun set red. the old man ran to the "
    "sea to see the red sun set on the blue sea at the end of the day. the bird "
    "and the cat saw the old man by the tree. the sun is red and the sea is blue. "
    "a dog and a cat ran to the old tree by the sea. the man said the sky is red "
    "at dawn and blue at the end of the day. the bird sat on the mat by the dog. "
    "she ran in the sun to the sea and saw a red bird on the old oak tree. the "
    "cat said the dog is old and the man is at the sea. the sun set on the tree.";
static int NW, TW, VW, SW, NA, NB;
static const uint8_t *ALLW[64], *VALW[64], *TESTW[64], *TRA[64], *TRB[64];
static float *g_logits;

static float loss_on(st_model *m, const uint8_t *const *w, int n)
{
    double t = 0.0;
    for (int i = 0; i < n; i++) { int np = 0; t += st_eval_loss(m, w[i], SEQLEN, &np); }
    return n ? (float)(t / n) : 0.0f;
}

/* train like train_list, print own-data loss every `every` rounds */
static void curve(const char *tag, uint64_t seed, const uint8_t *const *w, int n,
                  int shift, int rounds, float lr, int every)
{
    st_model m;
    st_init_tier(&m, seed, ST_TIER_S);
    int seen[64] = {0};
    printf("%s seed=%llu lr=%.4f shift=%d n=%d |", tag, (unsigned long long)seed,
           (double)lr, shift, n);
    printf(" r0 %.3f", (double)loss_on(&m, w, n));
    float mx = 0; int spikes = 0; float prev = 99;
    for (int r = 0; r < rounds; r++) {
        for (int j = 0; j < n; j++) {
            int k = (j * shift + 3) % n; seen[k] = 1;
            const uint8_t *b = w[k];
            st_zero_grad(&m); st_forward(&m, b, SEQLEN, g_logits);
            st_backward(&m, b, SEQLEN); st_adam_step(&m, lr);
        }
        float l = loss_on(&m, w, n);
        if (l > prev + 0.3f) spikes++;
        if (l > mx) mx = l;
        prev = l;
        if ((r + 1) % every == 0) printf(" r%d %.3f", r + 1, (double)l);
    }
    int ns = 0; for (int i = 0; i < n; i++) ns += seen[i];
    printf(" | seen %d/%d spikes %d test %.3f\n", ns, n, spikes,
           (double)loss_on(&m, TESTW, SW));
    st_free(&m);
}

int main(int argc, char **argv)
{
    NW = ((int)sizeof(CORPUS) - 1) / SEQLEN;
    SW = NW / 6; VW = NW / 6; TW = NW - SW - VW;
    for (int i = 0; i < TW; i++) ALLW[i]  = CORPUS + i * SEQLEN;
    for (int i = 0; i < VW; i++) VALW[i]  = CORPUS + (TW + i) * SEQLEN;
    for (int i = 0; i < SW; i++) TESTW[i] = CORPUS + (TW + VW + i) * SEQLEN;
    NA = TW / 2; NB = TW - NA;
    for (int i = 0; i < NA; i++) TRA[i] = ALLW[i];
    for (int i = 0; i < NB; i++) TRB[i] = ALLW[NA + i];
    printf("windows NW=%d TW=%d (A %d / B %d) VW=%d SW=%d\n", NW, TW, NA, NB, VW, SW);
    g_logits = (float *)malloc((size_t)SEQLEN * ST_VOCAB * sizeof(float));
    float lr = argc > 1 ? (float)atof(argv[1]) : 0.02f;
    uint64_t seeds[] = {0xC0FFEE, 1, 2, 3, 4, 5, 6, 7};
    for (int s = 0; s < 8; s++) {
        uint64_t sd = seeds[s];
        curve("S1-A", sd, ALLW, TW, 1, ROUNDS, lr, 8);
        curve("S1-B", sd, ALLW, TW, 7, ROUNDS, lr, 8);
        curve("S2-A", sd, TRA, NA, 1, ROUNDS * 2, lr, 16);
        curve("S2-B", sd, TRB, NB, 1, ROUNDS * 2, lr, 16);
    }
    return 0;
}
