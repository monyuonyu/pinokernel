/*
 * qprobe.c — probe for R-1 stage 6 (not a cert): on seeds given, compare three
 * ways to rank the stage-5 arms (E4, E3O3, A3) against their real B1 damage:
 *   om_mat : st_quant_omega (stage 5: mean F over the matrix x ||RTN - W||^2)
 *   om_el  : sum_i F_i (RTN_b(W)_i - W_i)^2         (element-wise, RTN)
 *   om_real: sum_i F_i (Wq_i - W_i)^2 on the built copy (element-wise, GPTQ)
 *   kl_gate: KL on the gate windows of the built copy
 * Build (from tests/llm): cc -std=c11 -O1 -ffp-contract=off qalloc_el_probe.c
 *   ../../arch/common/llm/held_indep.c ../../arch/common/llm/student.c -lm
 * Run: ./a.out b1_held.txt <seed>. Result on seeds 1-8 (2026-10-24, container):
 *   research/brain-quant-alloc-el.md
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
#define NARM 3
static const char *nm[NARM] = { "E4", "E3O3", "A3" };
static const int ab[NARM][3] = { { 4, 2, 2 }, { 3, 2, 3 }, { 2, 3, 2 } };
static uint8_t held[65536]; static int hn;

static double bpb(st_model *m)
{
    double s = 0.0; int got = 0;
    for (int w = 0; (w + 1) * SEQ <= hn; w++) {
        int np = 0; float l = st_eval_loss(m, held + w * SEQ, SEQ, &np);
        if (np) { s += l; got++; }
    }
    return got ? s / got / LN2 : 0.0;
}

static double el_mat(const st_model *m, const double *F, int off, int R, int C, int bits)
{
    int qmax = (1 << (bits - 1)) - 1; double t = 0.0;
    for (int r = 0; r < R; r++) {
        const float *w = m->w + off + (size_t)r * C;
        float amax = 0.0f;
        for (int c = 0; c < C; c++) { float v = fabsf(w[c]); if (v > amax) amax = v; }
        float s = amax / (float)qmax;
        for (int c = 0; c < C; c++) {
            double q = 0.0;
            if (s != 0.0f) {
                float rr = w[c] / s; int k = (int)(rr >= 0.0f ? rr + 0.5f : rr - 0.5f);
                if (k > qmax) k = qmax;
                if (k < -qmax - 1) k = -qmax - 1;
                q = (double)((float)k * s);
            }
            double d = (double)w[c] - q;
            t += F[off + (size_t)r * C + c] * d * d;
        }
    }
    return t;
}

static double om_el(const st_model *m, const double *F, int be, int ba, int bo)
{
    const int D = m->d, DFF = m->dff, L = m->nlayer, E = m->nexpert;
    double t = el_mat(m, F, m->o_embed, ST_VOCAB, D, be);
    for (int l = 0; l < L; l++) {
        t += el_mat(m, F, m->o_wq + l * D * D, D, D, ba); t += el_mat(m, F, m->o_wk + l * D * D, D, D, ba);
        t += el_mat(m, F, m->o_wv + l * D * D, D, D, ba); t += el_mat(m, F, m->o_wo + l * D * D, D, D, ba);
    }
    for (int x = 0; x < L * E; x++) {
        t += el_mat(m, F, m->o_w1 + x * DFF * D, DFF, D, 2); t += el_mat(m, F, m->o_w3 + x * DFF * D, DFF, D, 2);
        t += el_mat(m, F, m->o_w2 + x * D * DFF, D, DFF, 2);
    }
    return t + el_mat(m, F, m->o_out, ST_VOCAB, D, bo);
}

static double om_real(const st_model *m, const st_model *q, const double *F)
{
    double t = 0.0;
    for (int i = 0; i < m->n_params; i++) { double d = (double)m->w[i] - (double)q->w[i]; t += F[i] * d * d; }
    return t;
}

static int argmin(const double *v) { int k = 0; for (int i = 1; i < NARM; i++) if (v[i] < v[k]) k = i; return k; }

int main(int argc, char **argv)
{
    if (argc < 3) return 2;
    FILE *f = fopen(argv[1], "rb"); if (!f) return 2;
    hn = (int)fread(held, 1, sizeof held, f); fclose(f);
    uint32_t seed = (uint32_t)strtoul(argv[2], NULL, 0);
    const uint8_t *fx = (const uint8_t *)TEACHER_FIXTURE;
    int fx_end = st_held_split((int)sizeof(TEACHER_FIXTURE) - 1, SEQ).train_end;
    static uint8_t cal[65536], gate[65536]; int cn = 0, gn = 0;
    for (int w = 0; (w + 1) * SEQ <= fx_end; w++) {
        if (w % 2 == 0) { memcpy(cal + cn, fx + w * SEQ, SEQ); cn += SEQ; }
        else            { memcpy(gate + gn, fx + w * SEQ, SEQ); gn += SEQ; }
    }
    float *lg = malloc((size_t)SEQ * ST_VOCAB * sizeof(float));
    st_model m, q[NARM];
    st_init_tier(&m, seed, ST_TIER_S);
    for (int a = 0; a < NARM; a++) st_init_tier(&q[a], seed, ST_TIER_S);
    for (int r = 0; r < 8; r++)
        for (int o = 0; o + SEQ <= fx_end; o += SEQ) ST_TRAIN_STEP(&m, fx + o, SEQ, lg, 3e-3f);
    double b0 = bpb(&m);
    double *F = malloc((size_t)m.n_params * sizeof(double));
    st_quant_fisher(&m, cal, cn, SEQ, F);
    double d[NARM], o1[NARM], o2[NARM], o3[NARM], kg[NARM];
    for (int a = 0; a < NARM; a++) {
        st_quant_gptq_bits(&m, &q[a], 2, ab[a][0], ab[a][1], ab[a][2], cal, cn, SEQ, NULL);
        d[a] = bpb(&q[a]) - b0;
        o1[a] = st_quant_omega(&m, F, 2, ab[a][0], ab[a][1], ab[a][2]);
        o2[a] = om_el(&m, F, ab[a][0], ab[a][1], ab[a][2]);
        o3[a] = om_real(&m, &q[a], F);
        kg[a] = st_quant_copy_kl(&m, &q[a], gate, gn, SEQ);
    }
    int best = argmin(d);
    printf("[qprobe] seed %u best %s | damage", seed, nm[best]);
    for (int a = 0; a < NARM; a++) printf(" %s %.4f", nm[a], d[a]);
    printf(" | pick mat %s el %s real %s kl %s", nm[argmin(o1)], nm[argmin(o2)], nm[argmin(o3)], nm[argmin(kg)]);
    printf(" | el"); for (int a = 0; a < NARM; a++) printf(" %.4g", o2[a]);
    printf(" | real"); for (int a = 0; a < NARM; a++) printf(" %.4g", o3[a]);
    printf("\n");
    return 0;
}
