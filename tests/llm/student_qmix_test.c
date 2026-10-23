/*
 * student_qmix_test.c — [st-qmix] (R-1 stage 4; design:
 * docs/architecture/30-module/research/brain-quant-embed.md §3, gates fixed
 * 2026-10-23 before any E4 / O4 run).
 *
 * The student is trained as student_gptq_test trains it (8 rounds, lr 3e-3,
 * seqlen 32, the fixture's train windows; GPTQ calibrated on the EVEN train
 * windows, G1 measured on the ODD ones). Three 2-bit arms on the same grid:
 *   seq  st_quant_gptq(2)                    (the master of stage 3)
 *   E4   st_quant_gptq_mix(2, embed 4, out 2) embed 4-bit RTN, H through it
 *   O4   st_quant_gptq_mix(2, embed 2, out 4) out 4-bit GPTQ, the same bytes
 *
 *   I0  st_quant_gptq_mix(2, 2, 2) is byte-identical to st_quant_gptq(2)
 *   Q0  bytes_q(E4) == bytes_q(O4) == bytes_q(seq) + 4096, exactly
 *   Q1  B1 bpb damage E4 < O4 - 0.02, and KL(fp32||E4) < KL(fp32||O4)
 *   Q2  B1 bpb damage E4 < seq
 *   G1  per class, on the odd windows: E4 and O4 < RTN at the class's bits
 *       (out of O4 against 4-bit RTN, every other class against 2-bit RTN)
 *   G4  every quantized value of E4 and O4 on its row's grid at its bits, and
 *       the calibration shares no 16-byte run with the held text
 *   D0  E4 and O4 built twice are byte-identical
 * Printed only: embed fp32 (the paper's form, bits_embed 0) and the 4 KB
 * value of each arm (bpb gained per 4096 B).
 *
 * Negative controls (run_st_qmix.sh): -DST_QMIX_SWAP (the E4 call builds O4)
 * Q1 red; -DST_QMIX_EMB8 (embed 8 bit) Q0 red; -DST_QUANT_BADSCALE G4 red;
 * -DST_QMIX_EMBH2 (embed kept at 4 bit, H collected through the 2-bit embed)
 * values only.
 *
 *   student_qmix_test <b1_held.txt> <seed>     exit 0 = ALL PASS
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include "../../arch/common/llm/student.h"
#include "../../arch/common/llm/held_indep.h"
#include "../../arch/common/llm/teacher_fixture.h"

#define SEQ 32
#define LN2 0.69314718055994530942

static const char *cls_name[ST_QC_N] = { "wq", "wk", "wv", "wo", "w1", "w3", "w2", "out" };

static uint8_t held[65536];
static int hn;
static int fails;
static float *lg_a, *lg_b;

static void item(int ok, const char *id, const char *what)
{
    printf("[st-qmix] %s %s %s\n", ok ? "PASS" : "FAIL", id, what);
    if (!ok) fails++;
}

static double bpb(st_model *m)
{
    double s = 0.0; int got = 0;
    for (int w = 0; (w + 1) * SEQ <= hn; w++) {
        int np = 0;
        float l = st_eval_loss(m, held + w * SEQ, SEQ, &np);
        if (np) { s += l; got++; }
    }
    return got ? s / got / LN2 : 0.0;
}

static void softmax(const float *l, double *p)
{
    double mx = l[0], s = 0.0;
    for (int o = 1; o < ST_VOCAB; o++) if (l[o] > mx) mx = l[o];
    for (int o = 0; o < ST_VOCAB; o++) { p[o] = exp((double)l[o] - mx); s += p[o]; }
    for (int o = 0; o < ST_VOCAB; o++) p[o] /= s;
}

/* mean KL(a || b) per position over the held windows */
static double kl_mean(st_model *a, st_model *b)
{
    static double pa[ST_VOCAB], pb[ST_VOCAB];
    double s = 0.0; long np = 0;
    for (int w = 0; (w + 1) * SEQ <= hn; w++) {
        st_forward(a, held + w * SEQ, SEQ, lg_a);
        st_forward(b, held + w * SEQ, SEQ, lg_b);
        for (int t = 0; t < SEQ; t++) {
            softmax(lg_a + (size_t)t * ST_VOCAB, pa);
            softmax(lg_b + (size_t)t * ST_VOCAB, pb);
            double k = 0.0;
            for (int o = 0; o < ST_VOCAB; o++)
                if (pa[o] > 0.0) k += pa[o] * (log(pa[o]) - log(pb[o] > 1e-300 ? pb[o] : 1e-300));
            s += k; np++;
        }
    }
    return np ? s / (double)np : 0.0;
}

/* G4: the rows of q are on src's per-row grid at qmax */
static long off_grid_mat(const st_model *src, const st_model *q, int off, int rows, int cols, int bits)
{
    int qmax = (1 << (bits - 1)) - 1;
    long bad = 0;
    for (int r = 0; r < rows; r++) {
        const float *a = src->w + off + (size_t)r * cols, *b = q->w + off + (size_t)r * cols;
        float amax = 0.0f;
        for (int c = 0; c < cols; c++) { float v = a[c] < 0 ? -a[c] : a[c]; if (v > amax) amax = v; }
        if (amax == 0.0f) { for (int c = 0; c < cols; c++) if (b[c] != 0.0f) bad++; continue; }
        float s = amax / (float)qmax;
        for (int c = 0; c < cols; c++) {
            double k = (double)b[c] / (double)s;
            double rk = floor(k + 0.5);
            if (fabs(k - rk) > 1e-3 || rk > qmax || rk < -qmax - 1) bad++;
        }
    }
    return bad;
}

static long off_grid(const st_model *src, const st_model *q, int bits, int be, int bo)
{
    const int D = src->d, DFF = src->dff, L = src->nlayer, E = src->nexpert;
    long bad = off_grid_mat(src, q, src->o_embed, ST_VOCAB, D, be);
    for (int l = 0; l < L; l++) {
        bad += off_grid_mat(src, q, src->o_wq + l * D * D, D, D, bits);
        bad += off_grid_mat(src, q, src->o_wk + l * D * D, D, D, bits);
        bad += off_grid_mat(src, q, src->o_wv + l * D * D, D, D, bits);
        bad += off_grid_mat(src, q, src->o_wo + l * D * D, D, D, bits);
    }
    for (int x = 0; x < L * E; x++) {
        bad += off_grid_mat(src, q, src->o_w1 + x * DFF * D, DFF, D, bits);
        bad += off_grid_mat(src, q, src->o_w3 + x * DFF * D, DFF, D, bits);
        bad += off_grid_mat(src, q, src->o_w2 + x * D * DFF, D, DFF, bits);
    }
    bad += off_grid_mat(src, q, src->o_out, ST_VOCAB, D, bo);
    return bad;
}

static int same_w(const st_model *a, const st_model *b)
{
    return a->n_params == b->n_params &&
           memcmp(a->w, b->w, (size_t)a->n_params * sizeof(float)) == 0;
}

int main(int argc, char **argv)
{
    if (argc < 3) { fprintf(stderr, "usage: %s <b1_held.txt> <seed>\n", argv[0]); return 2; }
    uint32_t seed = (uint32_t)strtoul(argv[2], NULL, 0);
    FILE *f = fopen(argv[1], "rb");
    if (!f) { perror(argv[1]); return 2; }
    hn = (int)fread(held, 1, sizeof held, f);
    fclose(f);

    const uint8_t *fx = (const uint8_t *)TEACHER_FIXTURE;
    int fxn = (int)sizeof(TEACHER_FIXTURE) - 1;
    int fx_end = st_held_split(fxn, SEQ).train_end;
    static uint8_t cal[65536], cal_odd[65536];
    int cn = 0, no = 0;
    for (int w = 0; (w + 1) * SEQ <= fx_end; w++) {
        if (w % 2 == 0) { memcpy(cal + cn, fx + w * SEQ, SEQ); cn += SEQ; }
        else            { memcpy(cal_odd + no, fx + w * SEQ, SEQ); no += SEQ; }
    }

    lg_a = (float *)malloc((size_t)SEQ * ST_VOCAB * sizeof(float));
    lg_b = (float *)malloc((size_t)SEQ * ST_VOCAB * sizeof(float));
    if (!lg_a || !lg_b) return 2;

    st_model m, r2, r4, qs, qe, qo, qx;
    if (st_init_tier(&m, seed, ST_TIER_S) || st_init_tier(&r2, seed, ST_TIER_S) ||
        st_init_tier(&r4, seed, ST_TIER_S) || st_init_tier(&qs, seed, ST_TIER_S) ||
        st_init_tier(&qe, seed, ST_TIER_S) || st_init_tier(&qo, seed, ST_TIER_S) ||
        st_init_tier(&qx, seed, ST_TIER_S)) { printf("[st-qmix] FAIL init\n"); return 1; }
    for (int r = 0; r < 8; r++)
        for (int o = 0; o + SEQ <= fx_end; o += SEQ) ST_TRAIN_STEP(&m, fx + o, SEQ, lg_a, 3e-3f);
    double b_fp = bpb(&m);
    printf("[st-qmix] seed %u: trained fixture 0..%d, calib even windows %d B, G1 on odd windows %d B; fp32 bpb %.4f\n",
           seed, fx_end, cn, no, b_fp);

    int okI = 1, ok0 = 1, ok1 = 1, ok2 = 1, okG1 = 1, okG4 = 1, okD = 1;
    size_t by_s = 0, by_e = 0, by_o = 0, by_x = 0;
    int rr2 = st_quant_fake(&m, &r2, 2, ST_QG_ROW, NULL);
    int rr4 = st_quant_fake(&m, &r4, 4, ST_QG_ROW, NULL);
    int rs = st_quant_gptq(&m, &qs, 2, cal, cn, SEQ, &by_s);
    int re = st_quant_gptq_mix(&m, &qe, 2, 4, 2, cal, cn, SEQ, &by_e);
    int ro = st_quant_gptq_mix(&m, &qo, 2, 2, 4, cal, cn, SEQ, &by_o);
    printf("[st-qmix] rc rtn2 %d rtn4 %d seq %d E4 %d O4 %d\n", rr2, rr4, rs, re, ro);
    if (rr2 || rr4 || rs || re != ST_OK || ro != ST_OK) {
        item(0, "I0", "mix(2,2,2) byte-identical to st_quant_gptq(2)");
        item(0, "Q0", "bytes E4 == O4 == seq + 4096");
        item(0, "Q1", "bpb damage E4 < O4 - 0.02 and KL E4 < O4");
        item(0, "Q2", "bpb damage E4 < seq");
        item(0, "G1", "class objective E4, O4 < RTN at the class's bits");
        item(0, "G4", "E4, O4 on the grid; calib shares no 16-byte run with held");
        item(0, "D0", "E4, O4 built twice byte-identical");
        printf("[st-qmix] RED\n");
        return 1;
    }

    /* I0 */
    if (st_quant_gptq_mix(&m, &qx, 2, 2, 2, cal, cn, SEQ, &by_x) != ST_OK || !same_w(&qx, &qs) || by_x != by_s)
        okI = 0;
    /* D0 */
    if (st_quant_gptq_mix(&m, &qx, 2, 4, 2, cal, cn, SEQ, NULL) != ST_OK || !same_w(&qx, &qe)) okD = 0;
    if (st_quant_gptq_mix(&m, &qx, 2, 2, 4, cal, cn, SEQ, NULL) != ST_OK || !same_w(&qx, &qo)) okD = 0;

    /* Q0 */
    printf("[st-qmix] bytes seq %zu E4 %zu (+%ld) O4 %zu (+%ld)\n", by_s,
           by_e, (long)by_e - (long)by_s, by_o, (long)by_o - (long)by_s);
    if (!(by_e == by_s + 4096 && by_o == by_s + 4096)) ok0 = 0;

    /* Q1, Q2 */
    double d_s = bpb(&qs) - b_fp, d_e = bpb(&qe) - b_fp, d_o = bpb(&qo) - b_fp;
    double k_s = kl_mean(&m, &qs), k_e = kl_mean(&m, &qe), k_o = kl_mean(&m, &qo);
    printf("[st-qmix] bpb damage seq %.4f E4 %.4f O4 %.4f; KL seq %.6f E4 %.6f O4 %.6f\n",
           d_s, d_e, d_o, k_s, k_e, k_o);
    printf("[st-qmix] value per 4096 B: E4 %.4f bpb, O4 %.4f bpb (vs seq)\n", d_s - d_e, d_s - d_o);
    if (!(d_e < d_o - 0.02 && k_e < k_o)) ok1 = 0;
    if (!(d_e < d_s)) ok2 = 0;

    /* G1 */
    double cr2[ST_QC_N], cr4[ST_QC_N], ce[ST_QC_N], co[ST_QC_N];
    if (st_quant_class_obj(&m, &r2, cal_odd, no, SEQ, cr2) != ST_OK ||
        st_quant_class_obj(&m, &r4, cal_odd, no, SEQ, cr4) != ST_OK ||
        st_quant_class_obj(&m, &qe, cal_odd, no, SEQ, ce) != ST_OK ||
        st_quant_class_obj(&m, &qo, cal_odd, no, SEQ, co) != ST_OK) okG1 = 0;
    printf("[st-qmix] direct obj on odd windows E4/rtn O4/rtn:");
    for (int c = 0; c < ST_QC_N; c++) {
        double ref_o = c == ST_QC_OUT ? cr4[c] : cr2[c];
        printf(" %s %.3f/%.3f", cls_name[c], cr2[c] > 0.0 ? ce[c] / cr2[c] : -1.0,
               ref_o > 0.0 ? co[c] / ref_o : -1.0);
        if (!(ce[c] < cr2[c]) || !(co[c] < ref_o)) okG1 = 0;
    }
    printf("\n");

    /* G4 */
    long bad_e = off_grid(&m, &qe, 2, 4, 2), bad_o = off_grid(&m, &qo, 2, 2, 4);
    int shared = -1;
    {
        st_held_report r;
        int rc = st_held_check_seen(held, hn, hn, 0, 1, cal, cn, ST_HELD_K, &r);
        shared = rc < 0 ? -1 : r.shared;
    }
    printf("[st-qmix] off-grid E4 %ld O4 %ld; calib vs B1 held: shared 16-byte runs %d\n", bad_e, bad_o, shared);
    if (bad_e != 0 || bad_o != 0 || shared != 0) okG4 = 0;

    /* values only: embed fp32 (the paper keeps it in full precision) */
    {
        size_t by_f = 0;
        int rf = st_quant_gptq_mix(&m, &qx, 2, 0, 2, cal, cn, SEQ, &by_f);
        if (rf == ST_OK)
            printf("[st-qmix] value embed fp32: bpb damage %.4f, bytes %zu (+%ld)\n",
                   bpb(&qx) - b_fp, by_f, (long)by_f - (long)by_s);
        else
            printf("[st-qmix] value embed fp32: rc %d\n", rf);
    }

    item(okI, "I0", "mix(2,2,2) byte-identical to st_quant_gptq(2)");
    item(ok0, "Q0", "bytes E4 == O4 == seq + 4096");
    item(ok1, "Q1", "bpb damage E4 < O4 - 0.02 and KL E4 < O4");
    item(ok2, "Q2", "bpb damage E4 < seq");
    item(okG1, "G1", "class objective E4, O4 < RTN at the class's bits");
    item(okG4, "G4", "E4, O4 on the grid; calib shares no 16-byte run with held");
    item(okD, "D0", "E4, O4 built twice byte-identical");
    st_free(&m); st_free(&r2); st_free(&r4); st_free(&qs); st_free(&qe); st_free(&qo); st_free(&qx);
    printf("[st-qmix] %s\n", fails ? "RED" : "ALL PASS");
    return fails ? 1 : 0;
}
