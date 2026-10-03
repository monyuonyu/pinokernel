#!/bin/bash
# ---------------------------------------------------------------------------
# allpoints.sh — ROADMAP 0-8: cut the power at EVERY device write of an ARK
# operation, for several kinds of operation.
#
# run.sh (b) cuts one overwrite at writes 1..8. Here each scenario first runs
# its operation once with ARK_COUNT=1 to learn N (how many device writes it
# makes), then for every k in 1..N and both modes (BEFORE = the k-th write
# never lands, TORN = half of it lands as garbage) starts again from the same
# durable state, cuts, remounts in a fresh process, and requires the visible
# state (`dump`: version, size and content hash of every path in the
# scenario) to be EXACTLY the state before the operation or the state after
# it. After every cut the store must also take a new write and read it back
# (not wedged).
# Operations: create, overwrite, multi-block create, grow (rewrite longer,
# shared first block = dedup), unlink, mkdir, checkpoint, compact. ARK has no
# rename, so there is nothing to cut there (printed as SKIP).
# Negative control: ARK_EXTRA_CFLAGS=-DARK_FAULT_TRUST_COMMIT_HEADER builds
# an ARK whose mount believes a commit with a valid header even when its
# payload is torn or missing (correct when nothing is cut) — must go RED, and
# the report names the scenario, mode and write number where it broke.
# (A first control, "commit before the blocks", was dropped: mount discards
# everything after the last valid commit, so that ARK lost every write even
# uncut and only the wedge probe noticed — not a test of the cut points.)
# Output: one "[ark-allpoints] <scenario> ..." line each, then ALL PASS
# (exit 0) or RED (exit 1).
# ---------------------------------------------------------------------------
set -u
HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
CC="${CC:-cc}"
WORK="$(mktemp -d)"
BIN="$WORK/arkfs_test"
IMG="$WORK/ark.img"
SIZE=1024
trap 'rm -rf "$WORK"' EXIT
"$CC" -Wall -Wextra -std=gnu11 -O1 -DARK_HOST_TEST ${ARK_EXTRA_CFLAGS:-} \
    -I"$ROOT/arch/common/include" -I"$ROOT/relay" \
    "$HERE/arkfs_test.c" "$ROOT/arch/common/arkfs.c" "$ROOT/relay/sha256.c" \
    -o "$BIN" || { echo "[ark-allpoints] build failed"; exit 1; }
[ -n "${ARK_EXTRA_CFLAGS:-}" ] && echo "[ark-allpoints] built with ${ARK_EXTRA_CFLAGS}"

run() { "$BIN" "$@"; }
apply() {  # <ops separated by |>; each op = "<cmd> <args...>"
    # c/a1..a3 MUST be local: scenario() holds its operation in variables of
    # the same names, and bash scoping is dynamic (the first version of this
    # script replayed the setup's last op as "the operation" and reported 182
    # false breaks).
    local IFS='|' o c a1 a2 a3
    for o in $1; do
        IFS=' ' read -r c a1 a2 a3 <<< "$o"
        run "$c" "$IMG" $a1 $a2 $a3 > /dev/null || return 1
    done
    return 0
}
fresh() { run format "$IMG" "$SIZE" > /dev/null && apply "$1"; }

TOTAL=0; REDS=""
scenario() {  # <name> <setup ops> <op> <paths>
    local name=$1 setup=$2 op=$3 paths=$4
    fresh "$setup" || { echo "[ark-allpoints] $name FAIL: setup failed"; REDS="$REDS $name"; return; }
    local d0 d1 n c a1 a2 a3
    d0="$(run dump "$IMG" $paths)"
    IFS=' ' read -r c a1 a2 a3 <<< "$op"
    n=$(ARK_COUNT=1 "$BIN" "$c" "$IMG" $a1 $a2 $a3 2>&1 >/dev/null | sed -n 's/^\[count\] writes=//p')
    d1="$(run dump "$IMG" $paths)"
    if [ -z "$n" ] || [ "$n" -lt 1 ]; then
        echo "[ark-allpoints] $name FAIL: the operation made no device write (n='$n')"
        REDS="$REDS $name"; return
    fi
    local old=0 new=0 bad=0 mode k d
    for mode in BEFORE TORN; do
        for k in $(seq 1 "$n"); do
            fresh "$setup" || { bad=$((bad + 1)); continue; }
            env "ARK_KILL_${mode}=$k" "$BIN" "$c" "$IMG" $a1 $a2 $a3 > /dev/null 2>&1
            d="$(run dump "$IMG" $paths)"
            if [ "$d" = "$d0" ]; then old=$((old + 1))
            elif [ "$d" = "$d1" ]; then new=$((new + 1))
            else
                bad=$((bad + 1))
                echo "[ark-allpoints]   $name BROKEN at $mode #$k of $n:"
                printf '%s\n' "$d" | sed 's/^/[ark-allpoints]     got   /'
                printf '%s\n' "$d0" | sed 's/^/[ark-allpoints]     before /'
                printf '%s\n' "$d1" | sed 's/^/[ark-allpoints]     after  /'
                continue
            fi
            if ! run write "$IMG" /probe.txt "PROBE-$k" > /dev/null || \
               [ "$(run read "$IMG" /probe.txt)" != "READ: PROBE-$k" ]; then
                bad=$((bad + 1))
                echo "[ark-allpoints]   $name WEDGED after $mode #$k of $n (no new write)"
            fi
        done
    done
    TOTAL=$((TOTAL + 2 * n))
    local same=""; [ "$d0" = "$d1" ] && same=" (state unchanged by design)"
    echo "[ark-allpoints] $name writes=$n cut-points=$((2 * n)) before=$old after=$new broken=$bad$same"
    [ "$bad" -eq 0 ] || REDS="$REDS $name"
}

V3="write /a.txt A1|write /a.txt A2|write /b.txt B1"
scenario create     "write /a.txt A1"            "write /b.txt B1"          "/a.txt /b.txt"
scenario overwrite  "write /a.txt A1"            "write /a.txt A2-second"   "/a.txt"
scenario multiblock "write /a.txt A1"            "writefill /big.bin 13000 k" "/a.txt /big.bin"
scenario grow       "writefill /log.bin 5000 a"  "writefill /log.bin 9000 a"  "/log.bin"
scenario unlink     "write /a.txt A1|write /b.txt B1" "unlink /b.txt"       "/a.txt /b.txt"
scenario mkdir      "write /a.txt A1"            "mkdir /d"                 "/a.txt /d"
scenario checkpoint "$V3"                        "checkpoint"               "/a.txt /b.txt"
scenario compact    "$V3"                        "compact"                  "/a.txt /b.txt"
echo "[ark-allpoints] rename SKIP: ARK has no rename operation (arkfs.h)"
echo "[ark-allpoints] total cut points: $TOTAL"
if [ -z "$REDS" ]; then echo "[ark-allpoints] ALL PASS"; exit 0; fi
echo "[ark-allpoints] RED:$REDS"; exit 1
