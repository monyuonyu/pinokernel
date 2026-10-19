/*
 * gptq_probe.c — research probe for R-1 stage 2 (GPTQ G3 red on seed 2;
 * docs/architecture/30-module/research/brain-quant-gptq-seq.md §1).
 * Not a test and not in CI. #includes student.c to reach its statics.
 *
 *   cc -std=c11 -O2 -ffp-contract=off tests/llm/gptq_probe.c \
 *      arch/common/llm/held_indep.c -lm -o p
 *   ./p tests/llm/b1_held.txt <seed>
 *
 * Arms (2 bit, same row absmax grid as RTN):
 *   base        lambda 1% of mean diag, fp32 X, full calib (== st_quant_gptq)
 *   lam0.1/3/10/30
 *   seq         X from the model with the earlier stages already quantized
 *   seq_lam10
 *   act         act-order (columns by descending H diag)
 *   calA/calB   calib on even / odd windows only (half the amount)
 *   C2          wo quantized with a_in's H (the audit-54 hole)
 * Diagnostics:
 *   per-class "only this class quantized, the rest fp32": bpb damage RTN vs base
 *   embed fp32 in both copies: G3 ratio without the shared embed floor
 *   direct objective per class on held-out calib windows (calB when the
 *   quantizer used calA), computed from the cache vectors, not from H
 */
#include "../../arch/common/llm/student.c"
#include "../../arch/common/llm/held_indep.h"
#include "../../arch/common/llm/teacher_fixture.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

#define SEQ 32
#define LN2 0.69314718055994530942
#define NCLS 8
static const char *cls_name[NCLS] = { "wq", "wk", "wv", "wo", "w1", "w3", "w2", "out" };

static uint8_t held[65536];
static int hn;
static float *lg_a, *lg_b;

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

static void smax(const float *l, double *p)
{
    double mx = l[0], s = 0.0;
    for (int o = 1; o < ST_VOCAB; o++) if (l[o] > mx) mx = l[o];
    for (int o = 0; o < ST_VOCAB; o++) { p[o] = exp((double)l[o] - mx); s += p[o]; }
    for (int o = 0; o < ST_VOCAB; o++) p[o] /= s;
}

static double kl_mean(st_model *a, st_model *b)
{
    static double pa[ST_VOCAB], pb[ST_VOCAB];
    double s = 0.0; long np = 0;
    for (int w = 0; (w + 1) * SEQ <= hn; w++) {
        st_forward(a, held + w * SEQ, SEQ, lg_a);
        st_forward(b, held + w * SEQ, SEQ, lg_b);
        for (int t = 0; t < SEQ; t++) {
            smax(lg_a + (size_t)t * ST_VOCAB, pa);
            smax(lg_b + (size_t)t * ST_VOCAB, pb);
            double k = 0.0;
            for (int o = 0; o < ST_VOCAB; o++)
                if (pa[o] > 0.0) k += pa[o] * (log(pa[o]) - log(pb[o] > 1e-300 ? pb[o] : 1e-300));
            s += k; np++;
        }
    }
    return np ? s / (double)np : 0.0;
}

/* ---- options ---- */
typedef struct { double lam; int act; int c2; } pr_opt;

static int pr_hinv_upper(const double *H, int n, double *U, double *T, double lamf)
{
    double tr = 0.0;
    for (int i = 0; i < n; i++) tr += H[(size_t)i * n + i];
    double lam = lamf * tr / (double)n;
    for (size_t i = 0; i < (size_t)n * n; i++) T[i] = H[i];
    for (int i = 0; i < n; i++) {
        if (T[(size_t)i * n + i] == 0.0) T[(size_t)i * n + i] = 1.0;
        T[(size_t)i * n + i] += lam;
    }
    if (!gq_chol(T, n)) return 0;
    for (size_t i = 0; i < (size_t)n * n; i++) U[i] = 0.0;
    for (int j = 0; j < n; j++) {
        U[(size_t)j * n + j] = 1.0 / T[(size_t)j * n + j];
        for (int i = j + 1; i < n; i++) {
            double s = 0.0;
            for (int k = j; k < i; k++) s += T[(size_t)i * n + k] * U[(size_t)k * n + j];
            U[(size_t)i * n + j] = -s / T[(size_t)i * n + i];
        }
    }
    for (int i = 0; i < n; i++)
        for (int j = i; j < n; j++) {
            double s = 0.0;
            for (int k = j; k < n; k++) s += U[(size_t)k * n + i] * U[(size_t)k * n + j];
            T[(size_t)i * n + j] = s; T[(size_t)j * n + i] = s;
        }
    if (!gq_chol(T, n)) return 0;
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) U[(size_t)i * n + j] = j >= i ? T[(size_t)j * n + i] : 0.0;
    return 1;
}

/* one matrix, with lambda and optional act-order */
static void pr_mat(const st_model *src, st_model *dst, int off, int R, int C,
                   const double *H, int qmax, const pr_opt *o)
{
    float *W = dst->w + off;
    const float *W0 = src->w + off;
    if (!H) { for (int r = 0; r < R; r++) st_quant_group(W0 + (size_t)r * C, W + (size_t)r * C, C, qmax); return; }
    int *p = malloc((size_t)C * sizeof(int));
    double *Hp = malloc((size_t)C * C * sizeof(double));
    double *U = malloc((size_t)C * C * sizeof(double));
    double *T = malloc((size_t)C * C * sizeof(double));
    float *s = malloc((size_t)R * sizeof(float));
    for (int i = 0; i < C; i++) p[i] = i;
    if (o->act) {   /* descending diag, stable insertion sort */
        for (int i = 1; i < C; i++) {
            int v = p[i], j = i - 1;
            while (j >= 0 && H[(size_t)p[j] * C + p[j]] < H[(size_t)v * C + v]) { p[j + 1] = p[j]; j--; }
            p[j + 1] = v;
        }
    }
    for (int i = 0; i < C; i++) for (int k = 0; k < C; k++) Hp[(size_t)i * C + k] = H[(size_t)p[i] * C + p[k]];
    if (!pr_hinv_upper(Hp, C, U, T, o->lam)) {
        printf("#   chol failed off %d\n", off);
        for (int r = 0; r < R; r++) st_quant_group(W0 + (size_t)r * C, W + (size_t)r * C, C, qmax);
        goto out;
    }
    for (int r = 0; r < R; r++) {
        s[r] = st_quant_scale(W0 + (size_t)r * C, C, qmax);
        for (int c = 0; c < C; c++) W[(size_t)r * C + c] = W0[(size_t)r * C + c];
    }
    for (int j = 0; j < C; j++) {
        double d = U[(size_t)j * C + j];
        int cj = p[j];
        for (int r = 0; r < R; r++) {
            float *Wr = W + (size_t)r * C;
            float w = Wr[cj];
            float q = s[r] == 0.0f ? 0.0f : st_quant_one(w, s[r], qmax);
            Wr[cj] = q;
            double err = ((double)w - (double)q) / d;
            const double *Uj = U + (size_t)j * C;
            for (int k = j + 1; k < C; k++) Wr[p[k]] = (float)((double)Wr[p[k]] - err * Uj[k]);
        }
    }
out:
    free(p); free(Hp); free(U); free(T); free(s);
}

/* matrix table: class, offset, R, C, which H */
typedef struct { int cls, off, R, C, layer, expert; } pr_m;
static pr_m mats[512]; static int nm;

static void build_table(const st_model *m)
{
    ST_DIMS(m);
    nm = 0;
    for (int l = 0; l < L; l++) {
        mats[nm++] = (pr_m){ 0, m->o_wq + l * D * D, D, D, l, -1 };
        mats[nm++] = (pr_m){ 1, m->o_wk + l * D * D, D, D, l, -1 };
        mats[nm++] = (pr_m){ 2, m->o_wv + l * D * D, D, D, l, -1 };
        mats[nm++] = (pr_m){ 3, m->o_wo + l * D * D, D, D, l, -1 };
        for (int e = 0; e < E; e++) {
            int x = l * E + e;
            mats[nm++] = (pr_m){ 4, m->o_w1 + x * DFF * D, DFF, D, l, e };
            mats[nm++] = (pr_m){ 5, m->o_w3 + x * DFF * D, DFF, D, l, e };
            mats[nm++] = (pr_m){ 6, m->o_w2 + x * D * DFF, D, DFF, l, e };
        }
    }
    mats[nm++] = (pr_m){ 7, m->o_out, ST_VOCAB, D, -1, -1 };
}

static const double *h_for(const st_model *m, const st_gq_h *h, const pr_m *t, int c2)
{
    ST_DIMS(m);
    size_t dd = (size_t)D * D, ff = (size_t)DFF * DFF;
    switch (t->cls) {
    case 0: case 1: case 2: return h->ha + (size_t)t->layer * dd;
    case 3: return c2 ? h->ha + (size_t)t->layer * dd : h->ho + (size_t)t->layer * dd;
    case 4: case 5: { int x = t->layer * E + t->expert; return h->cnt[x] ? h->hf + (size_t)x * dd : NULL; }
    case 6: { int x = t->layer * E + t->expert; return h->cnt[x] ? h->hh + (size_t)x * ff : NULL; }
    default: return h->hout;
    }
}

/* stage of a matrix in the sequential order: layer*4 + {attn qkv 0, wo 1, w1w3 2, w2 3}; out = 4L */
static int stage_of(const st_model *m, const pr_m *t)
{
    if (t->cls == 7) return 4 * m->nlayer;
    int sub = t->cls <= 2 ? 0 : t->cls == 3 ? 1 : t->cls <= 5 ? 2 : 3;
    return t->layer * 4 + sub;
}

/* RTN copy (embed RTN unless emb_fp), then GPTQ each matrix whose class bit is in mask
 * (others: RTN if in rtn_mask, else fp32). seq: H re-collected from dst before each stage. */
static void make_copy(const st_model *src, st_model *dst, int bits, const uint8_t *cal, int cn,
                      const pr_opt *o, int seq, unsigned gmask, unsigned rmask, int emb_fp)
{
    int qmax = (1 << (bits - 1)) - 1;
    st_quant_fake(src, dst, bits, ST_QG_ROW, NULL);
    /* restore fp32 for every matrix, then decide per matrix */
    for (int i = 0; i < src->n_params; i++) {
        int in_embed = i >= src->o_embed && i < src->o_embed + ST_VOCAB * src->d;
        if (!in_embed || emb_fp) dst->w[i] = src->w[i];
    }
    st_gq_h h = { 0 };
    st_model tmp;
    if (!seq) { gq_scratch(src, &tmp); gq_collect(&tmp, cal, cn, SEQ, &h); st_free(&tmp); }
    int nst = 4 * src->nlayer + 1;
    for (int st = 0; st < nst; st++) {
        if (seq) {
            gq_free(&h); memset(&h, 0, sizeof h);
            gq_scratch(dst, &tmp); gq_collect(&tmp, cal, cn, SEQ, &h); st_free(&tmp);
        }
        for (int i = 0; i < nm; i++) {
            if (stage_of(src, &mats[i]) != st) continue;
            unsigned b = 1u << mats[i].cls;
            if (gmask & b) pr_mat(src, dst, mats[i].off, mats[i].R, mats[i].C, h_for(src, &h, &mats[i], o->c2), qmax, o);
            else if (rmask & b) for (int r = 0; r < mats[i].R; r++)
                st_quant_group(src->w + mats[i].off + (size_t)r * mats[i].C, dst->w + mats[i].off + (size_t)r * mats[i].C, mats[i].C, qmax);
        }
    }
    gq_free(&h);
}

/* direct objective per class on calib windows: sum_t ||(W0 - Wq) x_t||^2, inputs
 * read from the fp32 model's cache (independent of the H tables). */
static void direct_obj(const st_model *src, const st_model *q, const uint8_t *cal, int cn, double *oc)
{
    ST_DIMS(src);
    st_model tmp; gq_scratch(src, &tmp);
    float *lg = malloc((size_t)SEQ * ST_VOCAB * sizeof(float));
    double *y = malloc((size_t)(DFF > ST_VOCAB ? DFF : ST_VOCAB) * sizeof(double));
    for (int c = 0; c < NCLS; c++) oc[c] = 0.0;
    for (int o = 0; o + SEQ <= cn; o += SEQ) {
        st_forward(&tmp, cal + o, SEQ, lg);
        const st_cache *c = (const st_cache *)tmp.cache;
        int n = c->n;
        for (int i = 0; i < nm; i++) {
            const pr_m *t = &mats[i];
            const float *W0 = src->w + t->off, *Wq = q->w + t->off;
            for (int tt = 0; tt < n; tt++) {
                const float *x = NULL;
                if (t->cls == 7) x = c->o_in + (size_t)tt * D;
                else {
                    size_t lt = (size_t)t->layer * n + tt;
                    if (t->cls <= 2) x = c->a_in + lt * D;
                    else if (t->cls == 3) x = c->attn_o + lt * D;
                    else {
                        int nk = c->topk_n[lt], jj = -1;
                        for (int j = 0; j < nk; j++) if (c->topk_e[lt * E + j] == t->expert) jj = j;
                        if (jj < 0) continue;
                        x = t->cls <= 5 ? c->f_in + lt * D : c->e_h + (lt * E + jj) * DFF;
                    }
                }
                for (int r = 0; r < t->R; r++) {
                    double a = 0.0;
                    for (int k = 0; k < t->C; k++)
                        a += ((double)W0[(size_t)r * t->C + k] - (double)Wq[(size_t)r * t->C + k]) * (double)x[k];
                    oc[t->cls] += a * a;
                }
            }
        }
    }
    (void)y; free(y); free(lg); st_free(&tmp);
}

static st_model m, qr, qg;
static double b_fp;

static void report(const char *name, st_model *q, double b_r, double k_r, const double *ocr,
                   const uint8_t *calM, int cmn)
{
    double b = bpb(q), k = kl_mean(&m, q);
    double oc[NCLS]; direct_obj(&m, q, calM, cmn, oc);
    printf("ARM %-10s bpb %.4f dmg %.4f ratio %.3f  KL %.4f (rtn %.4f)  obj/rtn:", name, b, b - b_fp,
           (b - b_fp) / (b_r - b_fp), k, k_r);
    for (int c = 0; c < NCLS; c++) printf(" %s %.3f", cls_name[c], ocr[c] > 0 ? oc[c] / ocr[c] : 0.0);
    printf("\n");
    fflush(stdout);
}

int main(int argc, char **argv)
{
    if (argc < 3) return 2;
    uint32_t seed = (uint32_t)strtoul(argv[2], NULL, 0);
    FILE *f = fopen(argv[1], "rb"); if (!f) return 2;
    hn = (int)fread(held, 1, sizeof held, f); fclose(f);
    const uint8_t *fx = (const uint8_t *)TEACHER_FIXTURE;
    int fxn = (int)sizeof(TEACHER_FIXTURE) - 1;
    int fx_end = st_held_split(fxn, SEQ).train_end;
    int nw = fx_end / SEQ;
    static uint8_t calA[65536], calB[65536]; int na = 0, nb = 0;
    for (int w = 0; w < nw; w++) {
        if (w % 2 == 0) { memcpy(calA + na, fx + w * SEQ, SEQ); na += SEQ; }
        else            { memcpy(calB + nb, fx + w * SEQ, SEQ); nb += SEQ; }
    }
    lg_a = malloc((size_t)SEQ * ST_VOCAB * sizeof(float));
    lg_b = malloc((size_t)SEQ * ST_VOCAB * sizeof(float));
    st_init_tier(&m, seed, ST_TIER_S); st_init_tier(&qr, seed, ST_TIER_S); st_init_tier(&qg, seed, ST_TIER_S);
    for (int r = 0; r < 8; r++)
        for (int o = 0; o + SEQ <= fx_end; o += SEQ) ST_TRAIN_STEP(&m, fx + o, SEQ, lg_a, 3e-3f);
    build_table(&m);
    b_fp = bpb(&m);
    printf("SEED %u D %d DFF %d L %d E %d calib %d B (A %d B %d) held %d B fp32 bpb %.4f\n",
           seed, m.d, m.dff, m.nlayer, m.nexpert, fx_end, na, nb, hn, b_fp);

    const int bits = 2;
    unsigned ALL = 0xFFu;
    pr_opt base = { 0.01, 0, 0 };
    /* RTN reference */
    st_quant_fake(&m, &qr, bits, ST_QG_ROW, NULL);
    double b_r = bpb(&qr), k_r = kl_mean(&m, &qr);
    double ocr_full[NCLS], ocr_B[NCLS];
    direct_obj(&m, &qr, fx, fx_end, ocr_full);
    direct_obj(&m, &qr, calB, nb, ocr_B);
    printf("RTN bpb %.4f dmg %.4f KL %.4f\n", b_r, b_r - b_fp, k_r);
    printf("RTN direct obj full:"); for (int c = 0; c < NCLS; c++) printf(" %s %.4g", cls_name[c], ocr_full[c]); printf("\n");

    /* sanity: base arm == st_quant_gptq */
    st_quant_gptq(&m, &qg, bits, fx, fx_end, SEQ, NULL);
    double b_lib = bpb(&qg);
    make_copy(&m, &qg, bits, fx, fx_end, &base, 0, ALL, 0, 0);
    printf("CHECK lib bpb %.6f probe base bpb %.6f\n", b_lib, bpb(&qg));
    report("base", &qg, b_r, k_r, ocr_full, fx, fx_end);

    double lams[4] = { 0.001, 0.03, 0.1, 0.3 };
    const char *ln[4] = { "lam0.1", "lam3", "lam10", "lam30" };
    for (int i = 0; i < 4; i++) {
        pr_opt o = { lams[i], 0, 0 };
        make_copy(&m, &qg, bits, fx, fx_end, &o, 0, ALL, 0, 0);
        report(ln[i], &qg, b_r, k_r, ocr_full, fx, fx_end);
    }
    make_copy(&m, &qg, bits, fx, fx_end, &base, 1, ALL, 0, 0);
    report("seq", &qg, b_r, k_r, ocr_full, fx, fx_end);
    { pr_opt o = { 0.1, 0, 0 }; make_copy(&m, &qg, bits, fx, fx_end, &o, 1, ALL, 0, 0); report("seq_lam10", &qg, b_r, k_r, ocr_full, fx, fx_end); }
    { pr_opt o = { 0.01, 1, 0 }; make_copy(&m, &qg, bits, fx, fx_end, &o, 0, ALL, 0, 0); report("act", &qg, b_r, k_r, ocr_full, fx, fx_end); }
    { pr_opt o = { 0.01, 1, 0 }; make_copy(&m, &qg, bits, fx, fx_end, &o, 1, ALL, 0, 0); report("seq_act", &qg, b_r, k_r, ocr_full, fx, fx_end); }
    /* calib amount; objective measured on the other half (held-out X) */
    make_copy(&m, &qg, bits, calA, na, &base, 0, ALL, 0, 0);
    report("calA>B", &qg, b_r, k_r, ocr_B, calB, nb);
    make_copy(&m, &qg, bits, calB, nb, &base, 0, ALL, 0, 0);
    report("calB(in)", &qg, b_r, k_r, ocr_B, calB, nb);
    /* the hole: C2, objective direct, full and held-out */
    { pr_opt o = { 0.01, 0, 1 }; make_copy(&m, &qg, bits, fx, fx_end, &o, 0, ALL, 0, 0); report("C2", &qg, b_r, k_r, ocr_full, fx, fx_end);
      make_copy(&m, &qg, bits, calA, na, &o, 0, ALL, 0, 0); report("C2calA>B", &qg, b_r, k_r, ocr_B, calB, nb); }

    /* embed fp32 in both copies */
    {
        st_model qr2; st_init_tier(&qr2, seed, ST_TIER_S);
        make_copy(&m, &qr2, bits, fx, fx_end, &base, 0, 0, ALL, 1);
        double b_r2 = bpb(&qr2);
        make_copy(&m, &qg, bits, fx, fx_end, &base, 0, ALL, 0, 1);
        double b_g2 = bpb(&qg);
        printf("EMBFP rtn dmg %.4f gptq dmg %.4f ratio %.3f\n", b_r2 - b_fp, b_g2 - b_fp, (b_g2 - b_fp) / (b_r2 - b_fp));
        /* embed only quantized (everything else fp32) */
        make_copy(&m, &qr2, bits, fx, fx_end, &base, 0, 0, 0, 0);
        printf("EMBONLY dmg %.4f\n", bpb(&qr2) - b_fp);
        /* per class alone, embed fp32 */
        for (int c = 0; c < NCLS; c++) {
            make_copy(&m, &qr2, bits, fx, fx_end, &base, 0, 0, 1u << c, 1);
            make_copy(&m, &qg, bits, fx, fx_end, &base, 0, 1u << c, 0, 1);
            double dr = bpb(&qr2) - b_fp, dg = bpb(&qg) - b_fp;
            printf("CLS %-3s alone: rtn dmg %.4f gptq dmg %.4f ratio %.3f\n", cls_name[c], dr, dg, dr != 0 ? dg / dr : 0.0);
            fflush(stdout);
        }
        st_free(&qr2);
    }
    printf("PROBE DONE\n");
    return 0;
}
