/*
 * lr_sched_probe.c — PROBE (a measurement, not a cert) for the side finding of
 * brain-merge.md §3: "lowering the learning rate made the parents themselves
 * better (test loss const 2.19/2.08 -> linear 1.43/1.55)". That was Adam 0.02
 * on a 675-byte corpus. The resident baby trains at a constant 3e-3
 * (ST_DMN_LR, student_shell.c) on the TEACHER_FIXTURE. Does a decaying
 * schedule help THERE, and is any gain the schedule's SHAPE or just its lower
 * MEAN learning rate?
 *
 * Same setting as tests/llm/distill_proof.c (the resident split): seqlen 32,
 * first 3/4 of the windows train, last 1/4 held out, windows visited in
 * order, st_init default tier. argv: <fixture> <arm> <rounds> <seed>
 *   const      3e-3 throughout (today)
 *   linear     3e-3 -> 0 over the run
 *   cosine     3e-3 * 0.5 * (1 + cos(pi t))
 *   halfconst  1.5e-3 throughout  = the MEAN of linear and cosine (the control:
 *              if it matches linear, the gain is the level, not the shape)
 *   const6 / linear6   the same at a 6e-3 peak (is 3e-3 already "low"?)
 *   cool20 / cool20sq  3e-3, then a cooldown to 0 over the last 20% (linear /
 *              1-sqrt; Haegele et al., arXiv 2405.18392) — needs no horizon
 *              for the LIVE weights if only a saved copy is cooled
 *   swa        3e-3 throughout; also prints "swa-avg", the uniform average of
 *              the weights over the second half (no extra training)
 * Every arm makes the same number of Adam steps on the same windows in the
 * same order. Prints the held-out and train loss at 0, 1/4, 1/2, 3/4, 1 of
 * the run.
 *
 *   cc -std=c11 -O1 -Wall -Wextra -ffp-contract=off tests/llm/lr_sched_probe.c \
 *      arch/common/llm/student.c -lm -o lrp && ./lrp teacher.bytes linear 16 1
 */
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include "../../arch/common/llm/student.h"

#define SEQLEN 32

static uint8_t *g_corpus;
static int g_n;

static void window(uint8_t *dst, int off)
{
    for (int i = 0; i < SEQLEN; i++) dst[i] = g_corpus[(off + i) % g_n];
}

static float mean_loss(st_model *m, int first_win, int count)
{
    uint8_t buf[ST_MAXSEQ]; double s = 0; int got = 0;
    for (int w = 0; w < count; w++) {
        window(buf, (first_win + w) * SEQLEN);
        int np = 0; float l = st_eval_loss(m, buf, SEQLEN, &np);
        if (np) { s += l; got++; }
    }
    return got ? (float)(s / got) : 0.0f;
}

static double lr_at(const char *arm, long step, long total)
{
    double t = (double)step / (double)total;
    if (!strcmp(arm, "const"))     return 3e-3;
    if (!strcmp(arm, "linear"))    return 3e-3 * (1.0 - t);
    if (!strcmp(arm, "cosine"))    return 3e-3 * 0.5 * (1.0 + cos(3.14159265358979 * t));
    if (!strcmp(arm, "halfconst")) return 1.5e-3;
    if (!strcmp(arm, "const6"))    return 6e-3;
    if (!strcmp(arm, "linear6"))   return 6e-3 * (1.0 - t);
    /* constant + cooldown over the last 20% (Haegele et al. 2405.18392 eq. 1,
     * no warmup): linear, and their 1-sqrt shape */
    if (!strcmp(arm, "cool20"))    return t < 0.8 ? 3e-3 : 3e-3 * (1.0 - (t - 0.8) / 0.2);
    if (!strcmp(arm, "cool20sq"))  return t < 0.8 ? 3e-3 : 3e-3 * (1.0 - sqrt((t - 0.8) / 0.2));
    /* constant 3e-3, and the held loss of the uniform average of the weights
     * over the second half is printed too (SWA, ibid. §4.1) */
    if (!strcmp(arm, "swa"))       return 3e-3;
    return -1.0;
}

int main(int argc, char **argv)
{
    if (argc < 5) { fprintf(stderr, "usage: %s <fixture> <arm> <rounds> <seed>\n", argv[0]); return 2; }
    const char *arm = argv[2];
    int rounds = atoi(argv[3]);
    uint32_t seed = (uint32_t)strtoul(argv[4], NULL, 0);
    if (rounds < 4 || lr_at(arm, 0, 1) < 0) { fprintf(stderr, "bad arm or rounds\n"); return 2; }

    FILE *f = fopen(argv[1], "rb");
    if (!f) { fprintf(stderr, "cannot open %s\n", argv[1]); return 2; }
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    g_corpus = (uint8_t *)malloc((size_t)sz);
    if (fread(g_corpus, 1, (size_t)sz, f) != (size_t)sz) { fclose(f); return 2; }
    fclose(f);
    g_n = (int)sz;

    int total = g_n / SEQLEN;
    int trainw = total * 3 / 4, heldw = total - trainw;
    st_model m;
    if (st_init(&m, 0x0BABE + seed * 7919u) != ST_OK) { fprintf(stderr, "OOM\n"); return 1; }
    float *logits = (float *)malloc((size_t)SEQLEN * ST_VOCAB * sizeof(float));
    uint8_t buf[ST_MAXSEQ];
    long steps = (long)rounds * trainw, s = 0;
    double lr_sum = 0;
    int swa = !strcmp(arm, "swa");
    double *avg = swa ? (double *)calloc((size_t)m.n_params, sizeof(double)) : NULL;
    float *live = swa ? (float *)malloc((size_t)m.n_params * sizeof(float)) : NULL;
    long navg = 0;

    printf("[lrp] arm=%s rounds=%d seed=%u bytes=%d train=%d held=%d steps=%ld\n",
           arm, rounds, seed, g_n, trainw, heldw, steps);
    printf("[lrp] arm=%s seed=%u at=0/4 held=%.4f train=%.4f\n", arm, seed,
           (double)mean_loss(&m, trainw, heldw), (double)mean_loss(&m, 0, trainw));
    for (int r = 0; r < rounds; r++) {
        for (int w = 0; w < trainw; w++, s++) {
            window(buf, w * SEQLEN);
            double lr = lr_at(arm, s, steps);
            lr_sum += lr;
            st_zero_grad(&m);
            st_forward(&m, buf, SEQLEN, logits);
            st_backward(&m, buf, SEQLEN);
            st_adam_step(&m, (float)lr);
            if (swa && s >= steps / 2) {
                for (int i = 0; i < m.n_params; i++) avg[i] += m.w[i];
                navg++;
            }
        }
        if (swa && navg > 0 && (r + 1) % (rounds / 4) == 0) {
            memcpy(live, m.w, (size_t)m.n_params * sizeof(float));
            for (int i = 0; i < m.n_params; i++) m.w[i] = (float)(avg[i] / (double)navg);
            printf("[lrp] arm=swa-avg seed=%u at=%d/4 held=%.4f train=%.4f n=%ld\n", seed,
                   (r + 1) / (rounds / 4), (double)mean_loss(&m, trainw, heldw),
                   (double)mean_loss(&m, 0, trainw), navg);
            memcpy(m.w, live, (size_t)m.n_params * sizeof(float));
        }
        if ((r + 1) % (rounds / 4) == 0)
            printf("[lrp] arm=%s seed=%u at=%d/4 held=%.4f train=%.4f\n", arm, seed,
                   (r + 1) / (rounds / 4), (double)mean_loss(&m, trainw, heldw),
                   (double)mean_loss(&m, 0, trainw));
        fflush(stdout);
    }
    printf("[lrp] arm=%s seed=%u mean_lr=%.6f final held=%.4f train=%.4f\n", arm, seed,
           lr_sum / (double)steps, (double)mean_loss(&m, trainw, heldw),
           (double)mean_loss(&m, 0, trainw));
    free(avg); free(live);
    free(logits); st_free(&m); free(g_corpus);
    return 0;
}
