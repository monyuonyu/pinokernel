#!/bin/bash
# ---------------------------------------------------------------------------
# run_st_qalloc_el.sh — [st-qalloc] stage 6 (R-1; docs/architecture/30-module/
#   research/brain-quant-alloc-el.md §3, gates fixed 2026-10-24 before any run
#   on these seeds). The stage 5 cert run_st_qalloc.sh with -DST_QALLOC_EL
#   (the node picks by st_quant_omega_el) on seeds 9-16. Not in CI.
#
#   plain        -> I0 P0 A0 P1 G4 K0 D1 on every seed, "value P2 1" on >= 6/8
#   INVERT       -> P1 FAIL on the first seed (picks the largest omega_el)
#   BADSCALE     -> D1 FAIL
#
#   ./run_st_qalloc_el.sh       exit 0 = ALL PASS
# ---------------------------------------------------------------------------
HERE="$(cd "$(dirname "$0")" && pwd)"
export ST_QALLOC_DEF="-DST_QALLOC_EL"
export ST_QALLOC_SEEDS="${ST_QALLOC_SEEDS:-9 10 11 12 13 14 15 16}"
exec bash "$HERE/run_st_qalloc.sh"
