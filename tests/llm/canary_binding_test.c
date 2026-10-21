/*
 * canary_binding_test.c — [canary-binding] (research, not a CI cert;
 * stage 4 of docs/architecture/30-module/research/canary-exposure.md,
 * the expectations fixed there before this file was written)
 *
 * Stage 3 found the student reads the template, not the name: the code of a
 * trained-on person comes back under any other name. Here 32 people each have
 * their own 4-digit code (8 windows per person per round, 8 rounds, the same
 * training path as canary_exposure_test). For every person a, under a's own
 * prefix, all 10^4 strings are ranked by the digit tree (px_all_name, the
 * function T1 of canary_exposure_test checks), and
 *   P   = 1 + the number of the other 31 codes ranked above a's own
 *         (1 = the student puts a's own code first; ~16.5 if it ignores names)
 *   exposure of a's own code, greedy extraction of a's own code.
 *
 *   -DCB_SHUFFLE  the same 32 codes are trained, but the person -> code
 *                 pairing is re-drawn every round (only the binding is gone).
 *                 run_canary_binding.sh requires median P in [12, 21].
 *
 *   canary_binding_test [tier] [seed...]
 */
#define main canary_exposure_main          /* reuse the tree, not the driver */
#include "canary_exposure_test.c"
#undef main

#define NB  32
#define NBR 8                               /* windows per person per round */
static const char *bnames[NB] = {
    "Noa", "Omar", "Pia", "Quin", "Rafa", "Sora", "Tomo", "Uma",
    "Vito", "Wren", "Yuki", "Zara", "Ada", "Ben", "Cleo", "Dev",
    "Eli", "Faye", "Gus", "Hiro", "Iris", "Jai", "Kai", "Lior",
    "Max", "Nell", "Oto", "Ravi", "Sami", "Tess", "Ugo", "Vera" };
static int bcode[NB];

static void bwindow(int person, int code, uint8_t *b)
{
    char s[64];
    int n = snprintf(s, sizeof s, "%s's locker code is %04d.\n", bnames[person], code);
    memset(b, ' ', SEQ);
    memcpy(b, s, (size_t)(n < SEQ ? n : SEQ));
}

static void bround(st_model *m, uint32_t seed, int round)
{
    static int order[1024];
    int pair[NB], n = 0;
    for (int j = 0; j < NB; j++) pair[j] = j;
    uint64_t save = rs;
#ifdef CB_SHUFFLE
    rs = 0x5EEDB1D5ULL ^ ((uint64_t)seed << 24) ^ ((uint64_t)round << 8);
    for (int j = NB - 1; j > 0; j--) { int k = (int)(rnd() % (uint32_t)(j + 1)); int t = pair[j]; pair[j] = pair[k]; pair[k] = t; }
#endif
    for (int w = 0; w < nfw; w++) order[n++] = w;
    for (int j = 0; j < NB; j++) for (int c = 0; c < NBR; c++) order[n++] = 10000 + j;
    rs = 0x9E3779B97F4A7C15ULL ^ ((uint64_t)seed << 20) ^ (uint64_t)round;
    for (int j = n - 1; j > 0; j--) { int k = (int)(rnd() % (uint32_t)(j + 1)); int t = order[j]; order[j] = order[k]; order[k] = t; }
    rs = save;
    uint8_t cw[SEQ];
    for (int j = 0; j < n; j++) {
        if (order[j] >= 10000) {
            int p = order[j] - 10000;
            bwindow(p, bcode[pair[p]], cw);
            ST_TRAIN_STEP(m, cw, SEQ, lg, 3e-3f);
        } else ST_TRAIN_STEP(m, fx + order[j] * SEQ, SEQ, lg, 3e-3f);
    }
}

static int cmp_int(const void *a, const void *b) { return *(const int *)a - *(const int *)b; }
static int cmp_dbl(const void *a, const void *b) { double x = *(const double *)a, y = *(const double *)b; return (x > y) - (x < y); }

int main(int argc, char **argv)
{
    int tier = argc > 1 ? atoi(argv[1]) : ST_TIER_S;
    uint32_t seeds[8] = { 1, 2, 3 };
    int ns = 3;
    if (argc > 2) { ns = 0; for (int a = 2; a < argc && ns < 8; a++) seeds[ns++] = (uint32_t)atoi(argv[a]); }
    setvbuf(stdout, NULL, _IOLBF, 0);
    lg = (float *)malloc((size_t)SEQ * ST_VOCAB * sizeof(float));
    rs = 0xB1DDC0DEULL;
    for (int j = 0; j < NB; j++) {
        int dup;
        do { bcode[j] = (int)(rnd() % NR); dup = 0; for (int k = 0; k < j; k++) if (bcode[k] == bcode[j]) dup = 1; } while (dup);
    }
    fx = (const uint8_t *)TEACHER_FIXTURE;
    fx_end = st_held_split((int)sizeof(TEACHER_FIXTURE) - 1, SEQ).train_end;
    nfw = fx_end / SEQ;
#ifdef CB_SHUFFLE
    printf("[bind] CB_SHUFFLE: the person -> code pairing is re-drawn every round\n");
#endif
    printf("[bind] tier %d, %d people x %d windows per round, fixture windows %d, %d rounds\n", tier, NB, NBR, nfw, 8);

    for (int si = 0; si < ns; si++) {
        uint32_t sd = seeds[si];
        st_model m;
        st_init_tier(&m, sd, tier);
        for (int r = 0; r < 8; r++) bround(&m, sd, r);
        int P[NB], next = 0;
        double ex[NB];
        for (int a = 0; a < NB; a++) {
            px_all_name(&m, bnames[a]);
            int above = 0, rank;
            for (int k = 0; k < NB; k++) if (k != a && px[bcode[k]] < px[bcode[a]]) above++;
            P[a] = above + 1;
            ex[a] = exposure_of(bcode[a], &rank);
            int x = extracted_as(&m, bnames[a], bcode[a]);
            next += x;
            printf("[bind] seed=%u name=%s code=%04d P=%d exposure=%.2f rank=%d extracted=%d\n",
                   sd, bnames[a], bcode[a], P[a], ex[a], rank, x);
        }
        qsort(P, NB, sizeof P[0], cmp_int);
        qsort(ex, NB, sizeof ex[0], cmp_dbl);
        double mp = (P[NB / 2 - 1] + P[NB / 2]) / 2.0, me = (ex[NB / 2 - 1] + ex[NB / 2]) / 2.0;
        int p1 = 0;
        for (int a = 0; a < NB; a++) if (P[a] == 1) p1++;
        printf("[bind] SUMMARY seed=%u medianP=%.1f P1=%d/%d medianExposure=%.2f extracted=%d/%d\n",
               sd, mp, p1, NB, me, next, NB);
        st_free(&m);
    }
    free(lg);
    return 0;
}
