/*
 * student_gptq_test.c — [st-gptq] (R-1 stage 2 and 3; designs:
 * docs/architecture/30-module/research/brain-quant-gptq.md §3 and
 * brain-quant-gptq-seq.md §2, gates fixed before the code).
 *
 * The student is trained as b1_held_test trains it (8 rounds, lr 3e-3,
 * seqlen 32, the fixture's train windows), then quantized two ways on the SAME
 * grid (row-wise symmetric absmax, st_quant_fake ST_QG_ROW): RTN and GPTQ
 * (st_quant_gptq). Stage 3: GPTQ is calibrated on the EVEN train windows only;
 * the layer objective is measured on the ODD ones (never calibrated on).
 *
 *   G1  layer objective, per class (stage 3): for each of the 8 matrix classes
 *       (wq wk wv wo w1 w3 w2 out) sum_t ||(W - W^) x_t||^2 over the odd
 *       windows, x_t read from the fp32 forward (st_quant_class_obj, not the
 *       H tables), GPTQ < RTN, at 2 and 4 bit
 *   G2  copy drift (2 bit): mean KL(fp32 || quantized) of the next-byte
 *       distribution over every position of the B1 held text, GPTQ < RTN
 *   G3  B1 bpb (2 bit): bpb(GPTQ) - bpb(fp32) <= (bpb(RTN) - bpb(fp32)) / 2.
 *       4 bit: printed only
 *   G4  every quantized element of the GPTQ copy is on its row's grid
 *       (s = absmax(original row) / qmax, an integer in [-qmax-1, qmax]) and
 *       the calibration bytes share no 16-byte run with the B1 held text
 * Printed only: the stage-2 objective through the H tables, and the floor
 * (bpb damage with only embed rounded, the rest fp32; GPTQ cannot touch it).
 *
 * Negative controls (run_st_gptq.sh): -DST_GPTQ_NOCOMP (no error feedback)
 * G1 G2 red; -DST_GPTQ_CALIB_HELD (calibrate on the held text) G4 red;
 * -DST_QUANT_BADSCALE (s/4) G4 red; -DST_GPTQ_WRONG_H (wo with a_in's H, the
 * audit-54 hole) G1 red. -DST_GPTQ_NOSEQ (stage 2) and -DST_GPTQ_BLOCKSEQ
 * (per block, as in the paper) print their G3 ratio only.
 *
 *   student_gptq_test <b1_held.txt> <seed>     exit 0 = G1..G4 PASS
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
    printf("[st-gptq] %s %s %s\n", ok ? "PASS" : "FAIL", id, what);
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

/* G4: the quantized rows of q are on src's per-row grid */
static long off_grid_mat(const st_model *src, const st_model *q, int off, int rows, int cols, int qmax)
{
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

static long off_grid(const st_model *src, const st_model *q, int bits)
{
    const int D = src->d, DFF = src->dff, L = src->nlayer, E = src->nexpert;
    int qmax = (1 << (bits - 1)) - 1;
    long bad = off_grid_mat(src, q, src->o_embed, ST_VOCAB, D, qmax);
    for (int l = 0; l < L; l++) {
        bad += off_grid_mat(src, q, src->o_wq + l * D * D, D, D, qmax);
        bad += off_grid_mat(src, q, src->o_wk + l * D * D, D, D, qmax);
        bad += off_grid_mat(src, q, src->o_wv + l * D * D, D, D, qmax);
        bad += off_grid_mat(src, q, src->o_wo + l * D * D, D, D, qmax);
    }
    for (int x = 0; x < L * E; x++) {
        bad += off_grid_mat(src, q, src->o_w1 + x * DFF * D, DFF, D, qmax);
        bad += off_grid_mat(src, q, src->o_w3 + x * DFF * D, DFF, D, qmax);
        bad += off_grid_mat(src, q, src->o_w2 + x * D * DFF, D, DFF, qmax);
    }
    bad += off_grid_mat(src, q, src->o_out, ST_VOCAB, D, qmax);
    return bad;
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
    /* even train windows calibrate, odd ones measure G1 */
    static uint8_t cal_even[65536], cal_odd[65536];
    int ne = 0, no = 0;
    for (int w = 0; (w + 1) * SEQ <= fx_end; w++) {
        if (w % 2 == 0) { memcpy(cal_even + ne, fx + w * SEQ, SEQ); ne += SEQ; }
        else            { memcpy(cal_odd + no, fx + w * SEQ, SEQ); no += SEQ; }
    }
#ifdef ST_GPTQ_CALIB_HELD
    const uint8_t *cal = held; int cn = (hn / SEQ) * SEQ;     /* negative control */
#else
    const uint8_t *cal = cal_even; int cn = ne;
#endif

    lg_a = (float *)malloc((size_t)SEQ * ST_VOCAB * sizeof(float));
    lg_b = (float *)malloc((size_t)SEQ * ST_VOCAB * sizeof(float));
    if (!lg_a || !lg_b) return 2;

    st_model m, qr, qg;
    if (st_init_tier(&m, seed, ST_TIER_S) || st_init_tier(&qr, seed, ST_TIER_S) ||
        st_init_tier(&qg, seed, ST_TIER_S)) { printf("[st-gptq] FAIL init\n"); return 1; }
    for (int r = 0; r < 8; r++)
        for (int o = 0; o + SEQ <= fx_end; o += SEQ) ST_TRAIN_STEP(&m, fx + o, SEQ, lg_a, 3e-3f);
    double b_fp = bpb(&m);
    printf("[st-gptq] seed %u: trained fixture 0..%d, calib %s %d B, G1 on odd windows %d B; fp32 bpb %.4f\n",
           seed, fx_end, cal == cal_even ? "fixture train even windows" : "B1 HELD (negative control)", cn, no, b_fp);
    {   /* the floor: only embed rounded (2 bit), every other weight fp32 */
        st_quant_fake(&m, &qr, 2, ST_QG_ROW, NULL);
        for (int i = 0; i < m.n_params; i++)
            if (i < m.o_embed || i >= m.o_embed + ST_VOCAB * m.d) qr.w[i] = m.w[i];
        printf("[st-gptq] 2 bit: embed-only floor bpb damage %.4f\n", bpb(&qr) - b_fp);
    }

    int ok1 = 1, ok2 = 1, ok3 = 1, ok4 = 1;
    for (int bits = 2; bits <= 4; bits += 2) {
        int rc1 = st_quant_fake(&m, &qr, bits, ST_QG_ROW, NULL);
        int rc2 = st_quant_gptq(&m, &qg, bits, cal, cn, SEQ, NULL);
        if (rc1 != ST_OK || rc2 != ST_OK) {
            printf("[st-gptq] %d bit: rc rtn %d gptq %d\n", bits, rc1, rc2);
            ok1 = ok2 = ok3 = ok4 = 0; continue;
        }
        double o_r = st_quant_layer_obj(&m, &qr, cal, cn, SEQ);
        double o_g = st_quant_layer_obj(&m, &qg, cal, cn, SEQ);
        double b_r = bpb(&qr), b_g = bpb(&qg);
        long bad = off_grid(&m, &qg, bits);
        printf("[st-gptq] %d bit: H-table obj (calib, stage 2 form) rtn %.6g gptq %.6g (ratio %.4f); bpb rtn %.4f gptq %.4f (fp32 %.4f); off-grid %ld\n",
               bits, o_r, o_g, o_r > 0 ? o_g / o_r : 0.0, b_r, b_g, b_fp, bad);
        double cr[ST_QC_N], cg[ST_QC_N];
        int rr = st_quant_class_obj(&m, &qr, cal_odd, no, SEQ, cr);
        int rg = st_quant_class_obj(&m, &qg, cal_odd, no, SEQ, cg);
        if (rr != ST_OK || rg != ST_OK) { printf("[st-gptq] %d bit: class obj rc %d %d\n", bits, rr, rg); ok1 = 0; }
        printf("[st-gptq] %d bit: direct obj on odd windows gptq/rtn:", bits);
        for (int c = 0; c < ST_QC_N; c++) {
            printf(" %s %.3f", cls_name[c], cr[c] > 0.0 ? cg[c] / cr[c] : -1.0);
            if (!(cg[c] < cr[c])) ok1 = 0;
        }
        printf("\n");
        if (bad != 0) ok4 = 0;
        if (bits == 2) {
            double k_r = kl_mean(&m, &qr), k_g = kl_mean(&m, &qg);
            printf("[st-gptq] 2 bit: held KL(fp32||q) rtn %.6f gptq %.6f\n", k_r, k_g);
            if (!(k_g < k_r)) ok2 = 0;
            printf("[st-gptq] 2 bit: bpb damage rtn %.4f gptq %.4f (G3 ratio %.3f, line 0.5)\n",
                   b_r - b_fp, b_g - b_fp, b_r - b_fp != 0.0 ? (b_g - b_fp) / (b_r - b_fp) : 0.0);
            if (!(b_g - b_fp <= (b_r - b_fp) / 2.0)) ok3 = 0;
        }
    }
    int shared = -1;
    {
        st_held_report r;
        int rc = st_held_check_seen(held, hn, hn, 0, 1, cal, cn, ST_HELD_K, &r);
        shared = rc < 0 ? -1 : r.shared;
    }
    printf("[st-gptq] calib vs B1 held: shared 16-byte runs %d\n", shared);
    if (shared != 0) ok4 = 0;

    item(ok1, "G1", "layer objective GPTQ < RTN for every class on held-out windows, 2 and 4 bit");
    item(ok2, "G2", "2-bit held KL(fp32||q) GPTQ < RTN");
    item(ok3, "G3", "2-bit bpb damage GPTQ <= half of RTN's");
    item(ok4, "G4", "GPTQ values on the grid, calib shares no 16-byte run with the held text");
    st_free(&m); st_free(&qr); st_free(&qg);
    printf("[st-gptq] %s\n", fails ? "RED" : "ALL PASS");
    return fails ? 1 : 0;
}
