/*
 *  student_delta_test.c — host cert for the one-bit delta, stage 1
 *  (arch/common/llm/student.c st_delta_sign;
 *   docs/architecture/30-module/research/delta-quant.md).
 *
 *  Two generations of one M-tier baby (fixed seed): BASE = 60 rounds on the
 *  first half of the training windows; FT = BASE + 20 more rounds on all of
 *  them (new data). D = FT - BASE. Rebuild FT from BASE + alpha*sign(D)
 *  (alpha = mean |D| per matrix) and compare with FT on held-out windows.
 *
 *  Expectations — WRITTEN BEFORE THE FIRST RUN (2026-10-04 19:48, committed before it ran), not tuned:
 *    [dq-size]    printed only: mean |D| / mean |w| per matrix family (the
 *                 memo's step 1, "measure the delta size first").
 *    [dq-visible] the delta matters: the logit MSE of BASE vs FT on held
 *                 windows is > 0 and |L(BASE) - L(FT)| >= GAP_MIN nats —
 *                 otherwise the cert cannot see anything (not discriminating).
 *    [dq-onebit]  the 1-bit rebuild is closer to FT than BASE is:
 *                 MSE(sign) <= 0.5 * MSE(BASE), and
 *                 |L(sign) - L(FT)| <= 0.5 * |L(BASE) - L(FT)|.
 *    [dq-signinfo] the signs carry the information: MSE(randsign) >= 2 *
 *                 MSE(sign) (same alpha, random signs).
 *    [dq-zero]    alpha = 0 (the matrices stay at BASE) is as far from FT as
 *                 BASE: MSE(zero) >= 0.5 * MSE(BASE).
 *    [dq-bytes]   the delta costs <= fp32 bytes / 16 (the memo hoped ~1/30).
 *    [dq-determ]  two calls give byte-identical weights.
 *    [dq-refuse]  dst == ft, a cross-tier model, a bad mode -> ST_E_ARG.
 *
 *  Negative control build (run_st_delta.sh): -DST_DQ_TEST_ALLPLUS replaces the
 *  method with "+alpha everywhere" (the sign thrown away) -> [dq-onebit] RED.
 *
 *  argv[1] (optional): init seed (default 0xC0FFEE). Build: -O1 -ffp-contract=off.
 */
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "../../arch/common/llm/student.h"

static int g_pass = 0, g_fail = 0;
#define CHECK(cond, msg) do { \
    if (cond) { printf("  PASS  %s\n", (msg)); g_pass++; } \
    else      { printf("  FAIL  %s\n", (msg)); g_fail++; } } while (0)

#define GAP_MIN 0.01f

static const uint8_t CORPUS[] =
    "the cat sat on the mat. the dog ran in the sun. she said the sea is blue "
    "and the sky is blue too. a bird sang on the old oak tree at dawn. the cat "
    "and the dog sat by the sea and saw the sun set red. the old man ran to the "
    "sea to see the red sun set on the blue sea at the end of the day. the bird "
    "and the cat saw the old man by the tree. the sun is red and the sea is blue.";

#define SEQLEN 24

static float heldout_loss(st_model *m, int from, int count)
{
    double tot = 0.0; int got = 0;
    for (int w = 0; w < count; w++) {
        int off = from + w * SEQLEN;
        if (off + SEQLEN > (int)sizeof(CORPUS) - 1) break;
        uint8_t buf[SEQLEN];
        for (int i = 0; i < SEQLEN; i++) buf[i] = CORPUS[off + i];
        int np = 0;
        float l = st_eval_loss(m, buf, SEQLEN, &np);
        if (np) { tot += l; got++; }
    }
    return got ? (float)(tot / got) : 0.0f;
}

/* mean squared logit difference of a vs b over the held windows */
static double logit_mse(st_model *a, st_model *b, int from, int count, float *la, float *lb)
{
    double tot = 0.0; long n = 0;
    for (int w = 0; w < count; w++) {
        int off = from + w * SEQLEN;
        if (off + SEQLEN > (int)sizeof(CORPUS) - 1) break;
        uint8_t buf[SEQLEN];
        for (int i = 0; i < SEQLEN; i++) buf[i] = CORPUS[off + i];
        st_forward(a, buf, SEQLEN, la);
        st_forward(b, buf, SEQLEN, lb);
        for (long i = 0; i < (long)SEQLEN * ST_VOCAB; i++) {
            double d = (double)la[i] - (double)lb[i];
            tot += d * d; n++;
        }
    }
    return n ? tot / (double)n : 0.0;
}

static void train(st_model *m, int from_w, int count_w, int rounds, float *logits)
{
    for (int r = 0; r < rounds; r++)
        for (int j = from_w; j < from_w + count_w; j++) {
            uint8_t b[SEQLEN];
            for (int i = 0; i < SEQLEN; i++) b[i] = CORPUS[j*SEQLEN + i];
            st_zero_grad(m); st_forward(m, b, SEQLEN, logits);
            st_backward(m, b, SEQLEN); st_adam_step(m, 0.02f);
        }
}

static double fabs_(double x) { return x < 0 ? -x : x; }

/* [dq-size]: mean |D| and mean |w(base)| over one family of offsets */
static void size_line(const char *name, const st_model *b, const st_model *f,
                      const int *offs, const int *lens, int k)
{
    double sd = 0.0, sw = 0.0; long n = 0;
    for (int j = 0; j < k; j++)
        for (int i = 0; i < lens[j]; i++) {
            sd += fabs_((double)f->w[offs[j] + i] - (double)b->w[offs[j] + i]);
            sw += fabs_((double)b->w[offs[j] + i]); n++;
        }
    printf("  [dq-size] %-6s n=%7ld mean|D|=%.6f mean|w|=%.6f ratio=%.4f\n",
           name, n, sd / n, sw / n, sd / (sw > 0 ? sw : 1));
}

int main(int argc, char **argv)
{
    uint32_t seed = argc > 1 ? (uint32_t)strtoul(argv[1], NULL, 0) : 0xC0FFEEu;
    printf("=== [st-delta] one-bit delta stage 1 cert (seed=0x%lx) ===\n", (unsigned long)seed);
#if defined(ST_DQ_TEST_ALLPLUS)
    printf("(NEGATIVE CONTROL build: -DST_DQ_TEST_ALLPLUS — expect RED)\n");
#endif
    int total_w = ((int)sizeof(CORPUS) - 1) / SEQLEN;
    int train_w = total_w * 3 / 4;
    int held_from = train_w * SEQLEN, held_w = total_w - train_w;

    float *lg = (float *)malloc((size_t)SEQLEN * ST_VOCAB * sizeof(float));
    float *lg2 = (float *)malloc((size_t)SEQLEN * ST_VOCAB * sizeof(float));
    st_model B, F, Q, Q2;
    if (!lg || !lg2 || st_init_tier(&F, seed, ST_TIER_M) != ST_OK ||
        st_init_tier(&B, 1, ST_TIER_M) != ST_OK || st_init_tier(&Q, 2, ST_TIER_M) != ST_OK ||
        st_init_tier(&Q2, 3, ST_TIER_M) != ST_OK) { printf("OOM\n"); return 2; }
    size_t np = (size_t)F.n_params;

    train(&F, 0, train_w / 2, 60, lg);                 /* generation t   */
    memcpy(B.w, F.w, np * sizeof(float));
    train(&F, 0, train_w, 20, lg);                     /* generation t+1 */

    const int D = F.d, DFF = F.dff, L = F.nlayer, E = F.nexpert;
    {
        int offs[64], lens[64], k = 0;
        offs[0] = F.o_embed; lens[0] = ST_VOCAB * D; size_line("embed", &B, &F, offs, lens, 1);
        for (int l = 0; l < L && k < 60; l++) {
            offs[k] = F.o_wq + l*D*D; lens[k++] = D*D; offs[k] = F.o_wk + l*D*D; lens[k++] = D*D;
            offs[k] = F.o_wv + l*D*D; lens[k++] = D*D; offs[k] = F.o_wo + l*D*D; lens[k++] = D*D;
        }
        size_line("attn", &B, &F, offs, lens, k);
        k = 0;
        for (int x = 0; x < L * E && k < 60; x++) {
            offs[k] = F.o_w1 + x*DFF*D; lens[k++] = DFF*D; offs[k] = F.o_w3 + x*DFF*D; lens[k++] = DFF*D;
            offs[k] = F.o_w2 + x*D*DFF; lens[k++] = D*DFF;
        }
        size_line("ffn", &B, &F, offs, lens, k);
        offs[0] = F.o_out; lens[0] = ST_VOCAB * D; size_line("out", &B, &F, offs, lens, 1);
    }

    float LB = heldout_loss(&B, held_from, held_w), LF = heldout_loss(&F, held_from, held_w);
    double mseB = logit_mse(&B, &F, held_from, held_w, lg, lg2);
    printf("  held-out: base %.4f  ft %.4f  (gap %+.4f)  logitMSE(base,ft) %.6f\n",
           (double)LB, (double)LF, (double)(LB - LF), mseB);

    static const char *MN[3] = { "sign", "randsign", "zero" };
    double mse[3]; float L3[3]; size_t bytes = 0; int rc_ok = 1;
    for (int md = 0; md < 3; md++) {
        int mode = md;
#if defined(ST_DQ_TEST_ALLPLUS)
        if (md == ST_DQ_SIGN) {                        /* the sign thrown away */
            if (st_delta_sign(&B, &F, &Q, ST_DQ_SIGN, &bytes) != ST_OK) rc_ok = 0;
            for (size_t i = 0; i < np; i++) {
                float d = Q.w[i] - B.w[i];
                if (d < 0) Q.w[i] = B.w[i] - d;
            }
            goto measured;
        }
#endif
        if (st_delta_sign(&B, &F, &Q, mode, md == ST_DQ_SIGN ? &bytes : NULL) != ST_OK) rc_ok = 0;
#if defined(ST_DQ_TEST_ALLPLUS)
    measured:
#endif
        L3[md] = heldout_loss(&Q, held_from, held_w);
        mse[md] = logit_mse(&Q, &F, held_from, held_w, lg, lg2);
        printf("  [st-delta] %-8s held %.4f (vs ft %+.4f)  logitMSE(.,ft) %.6f\n",
               MN[md], (double)L3[md], (double)(L3[md] - LF), mse[md]);
    }
    printf("  bytes: delta %zu vs fp32 %zu (1/%.1f)\n", bytes, np * 4,
           bytes ? (double)(np * 4) / (double)bytes : 0.0);

    double gap = fabs_((double)LB - (double)LF);
    printf("\n[visible]\n");
    CHECK(rc_ok, "st_delta_sign returned ST_OK for the 3 modes");
    CHECK(mseB > 0.0 && gap >= GAP_MIN, "[dq-visible] the delta matters (MSE > 0, |L(base)-L(ft)| >= GAP_MIN)");
    printf("[onebit]\n");
    CHECK(mse[0] <= 0.5 * mseB, "[dq-onebit] MSE(sign) <= 0.5 * MSE(base)");
    CHECK(fabs_((double)L3[0] - (double)LF) <= 0.5 * gap, "[dq-onebit] |L(sign)-L(ft)| <= 0.5 * |L(base)-L(ft)|");
    printf("[controls]\n");
    CHECK(mse[1] >= 2.0 * mse[0], "[dq-signinfo] MSE(randsign) >= 2 * MSE(sign)");
    CHECK(mse[2] >= 0.5 * mseB, "[dq-zero] MSE(zero) >= 0.5 * MSE(base)");
    printf("[bytes]\n");
    CHECK(bytes > 0 && bytes * 16 <= np * 4, "[dq-bytes] delta <= fp32 / 16");
    printf("[determinism]\n");
    st_delta_sign(&B, &F, &Q, ST_DQ_SIGN, NULL);
    st_delta_sign(&B, &F, &Q2, ST_DQ_SIGN, NULL);
    CHECK(memcmp(Q.w, Q2.w, np * sizeof(float)) == 0, "[dq-determ] twice -> byte-identical");
    printf("[refusal]\n");
    st_model S;
    st_init_tier(&S, 4, ST_TIER_S);
    CHECK(st_delta_sign(&B, &F, &F, ST_DQ_SIGN, NULL) == ST_E_ARG, "[dq-refuse] dst == ft refused");
    CHECK(st_delta_sign(&B, &F, &S, ST_DQ_SIGN, NULL) == ST_E_ARG, "[dq-refuse] cross-tier dst refused");
    CHECK(st_delta_sign(&B, &F, &Q, 7, NULL) == ST_E_ARG, "[dq-refuse] mode 7 refused");
    st_free(&S);

    printf("\n[st-delta] %d PASS / %d FAIL\n", g_pass, g_fail);
    st_free(&B); st_free(&F); st_free(&Q); st_free(&Q2); free(lg); free(lg2);
    return g_fail ? 1 : 0;
}
