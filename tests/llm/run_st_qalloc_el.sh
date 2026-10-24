#!/bin/bash
# ---------------------------------------------------------------------------
# run_st_qalloc_el.sh — [st-qalloc] stage 6 (R-1; docs/architecture/30-module/
#   research/brain-quant-alloc-el.md §3, gates fixed 2026-10-24 before any run
#   on these seeds). The stage 5 cert run_st_qalloc.sh with -DST_QALLOC_EL
#   (the node picks by st_quant_omega_el). Not in CI.
#
#   Stage 7 (research/brain-quant-alloc-eq.md §3, gates fixed 2026-10-24
#   before any run on seeds 17-24): the judged seeds are 17-24, and R
#   re-runs stage 6's seeds 9-16 against qalloc_el_expect.txt (stage 6's
#   record, audit-76/77). Stage 6 alone: ST_QALLOC_SEEDS="9 ... 16".
#
#   plain        -> I0 P0 A0 P1 G4 K0 D1 on every seed, "value P2 1" on >= 6/8,
#                   "value E" and an E4-E3O3 interval of width > 0 on every seed
#   R            -> seeds 9-16: omega_el and damage lines equal the record
#   INVERT       -> P1 FAIL on the first seed (picks the largest omega_el)
#   BADSCALE     -> D1 FAIL
#   BOOT_ONE     -> the E4-E3O3 interval has width 0
#
#   ./run_st_qalloc_el.sh       exit 0 = ALL PASS
# ---------------------------------------------------------------------------
HERE="$(cd "$(dirname "$0")" && pwd)"
export ST_QALLOC_DEF="-DST_QALLOC_EL"
export ST_QALLOC_SEEDS="${ST_QALLOC_SEEDS:-17 18 19 20 21 22 23 24}"
export ST_QALLOC_R_SEEDS="${ST_QALLOC_R_SEEDS-9 10 11 12 13 14 15 16}"
export ST_QALLOC_R_EXPECT="${ST_QALLOC_R_EXPECT:-$HERE/qalloc_el_expect.txt}"
exec bash "$HERE/run_st_qalloc.sh"
