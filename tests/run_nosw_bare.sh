#!/bin/bash
# ---------------------------------------------------------------------------
# run_nosw_bare.sh — no-switch regions on bare metal (R-NOSW-BARE,
#   arch/common/include/pk_nosw.h; gap-ledger EXIT-DISPATCH-SHARED-STATE).
#
# Bare x86 and AArch64 switch at the outermost interrupt exit (RNG0), so a
# task reading a kdds topic slot could be switched out half-way through the
# copy. tk_conform.c (built with -DPK_TKCONF -DPK_TKCONF_NOSW) prints:
#   [nosw-bare]   torn reads of a LATEST_ONLY topic a higher task publishes to
#   [nosw-leave]  a task woken inside a region runs at PK_NOSW_LEAVE, not before
#   [nosw-ext]    a task that tk_ext_tsk's inside a region is preemptible in
#                 its next life
#   [nosw-ter]    the same after tk_ter_tsk inside a region
#   [nosw-sleep]  a task asleep inside a region does not stop the interrupt-exit
#                 switch for other tasks
# Per arch, five arms; each line is judged PASS / FAIL / - (not judged):
#   new       regions on              bare PASS leave PASS ext PASS ter PASS sleep PASS
#   off       -DPK_NOSW_OFF (regions gone)  bare FAIL leave FAIL (25-35 torn
#             reads in 2026-10-07's boots)
#   noleave   cpu_cntl.c: leaving the region does not switch     leave FAIL
#   noforget  cpu_task.h: a new life keeps the old region depth  ext FAIL ter FAIL
#   glob      cpu_cntl.c: one depth for all tasks                 sleep FAIL
# (noleave / noforget: audit-25 showed [nosw-bare] stays green under both;
#  glob: audit-26 showed bare/leave/ext/ter all stay green under it.)
#
#   ./tests/run_nosw_bare.sh            (both arches)
#   ARCHES=x86 ./tests/run_nosw_bare.sh
# ---------------------------------------------------------------------------
set -u
HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/.." && pwd)"
ARCHES="${ARCHES:-x86 aarch64}"
ARMS="${NOSW_ARMS:-new off noleave noforget glob}"
BOOT_TIMEOUT="${NOSW_BOOT_TIMEOUT:-90}"
WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT

rc_all=0
break_kernel() {  # $1 = tree, $2 = sysdepend dir, $3 = file, $4 = old, $5 = new [, $6 = count]
    python3 - "$1/kernel/mtkernel3/kernel/sysdepend/$2/$3" "$4" "$5" "${6:-1}" <<'EOF'
import sys
p, old, new, n = sys.argv[1:5]
s = open(p).read()
assert s.count(old) == int(n), ('anchor not found', n, p, old)
open(p, 'w').write(s.replace(old, new))
EOF
}
arm() {  # $1 = arch, $2 = arm
    local d="$WORK/$1-$2" log="$WORK/$1-$2.serial" cf sd want tag w line v
    cf="-DPK_TKCONF -DPK_TKCONF_NOSW"
    sd=x86_pc; [ "$1" = aarch64 ] && sd=aarch64_virt
    mkdir -p "$d"
    tar -C "$ROOT" --exclude=./.git -cf - . | tar -C "$d" -xf -
    case "$2" in
        new)      want="bare=PASS leave=PASS ext=PASS ter=PASS sleep=PASS" ;;
        off)      want="bare=FAIL leave=FAIL ext=- ter=- sleep=-"; cf="$cf -DPK_NOSW_OFF" ;;
        noleave)  want="bare=- leave=FAIL ext=- ter=- sleep=-"
                  break_kernel "$d" "$sd" cpu_cntl.c $'\tif ( out ) {' $'\tif ( 0 && out ) {' \
                      || { echo "[result] $1 $2 anchor missing (FAIL)"; rc_all=1; return; } ;;
        noforget) want="bare=- leave=- ext=FAIL ter=FAIL sleep=-"
                  break_kernel "$d" "$sd" cpu_task.h $'\tknl_nosw_forget(tcb->tskid);' $'\t(void)0;' \
                      || { echo "[result] $1 $2 anchor missing (FAIL)"; rc_all=1; return; } ;;
        glob)     want="bare=- leave=- ext=- ter=- sleep=FAIL"
                  { break_kernel "$d" "$sd" cpu_cntl.c 'INDEX_TSK(knl_ctxtsk->tskid)]' '0]' 3 &&
                    break_kernel "$d" "$sd" cpu_cntl.c 'INDEX_TSK(tskid)]' '0]' 1; } \
                      || { echo "[result] $1 $2 anchor missing (FAIL)"; rc_all=1; return; } ;;
        *) echo "[result] unknown arm $2"; rc_all=1; return ;;
    esac
    : > "$log"
    if [ "$1" = x86 ]; then
        if ! ( cd "$d/boot/x86" && make clean && make EXTRA_CFLAGS="$cf" bootloader.bin ) > "$WORK/$1-$2.build" 2>&1; then
            tail -20 "$WORK/$1-$2.build"; echo "[result] $1 $2 build FAILED"; rc_all=1; return
        fi
        ( cd "$d/boot/x86" && timeout "$BOOT_TIMEOUT" qemu-system-x86_64 -m 256 \
            -kernel bootloader.bin -serial file:"$log" -cpu qemu64 -display none \
            -no-reboot ) > "$WORK/$1-$2.qemu" 2>&1
    else
        if ! ( cd "$d/boot/aarch64" && make clean && make EXTRA_CFLAGS="$cf" ) > "$WORK/$1-$2.build" 2>&1; then
            tail -20 "$WORK/$1-$2.build"; echo "[result] $1 $2 build FAILED"; rc_all=1; return
        fi
        ( cd "$d/boot/aarch64" && timeout -k 3 "$BOOT_TIMEOUT" qemu-system-aarch64 \
            -M virt,gic-version=2 -cpu cortex-a53 -m 256M -serial stdio -display none \
            -no-reboot -nic none -kernel kernel.elf ) > "$log" 2>&1 < /dev/null
    fi
    for w in $want; do
        tag="${w%%=*}"
        line=$(tr -d '\r' < "$log" | grep -a -m1 "^\[nosw-$tag\]")
        echo "  $1 $2: ${line:-(no [nosw-$tag] line)}"
        case "$line" in
            "[nosw-$tag] PASS"*) v=PASS ;;
            "[nosw-$tag] FAIL"*) v=FAIL ;;
            *) v=NONE ;;
        esac
        [ "${w#*=}" = - ] && continue
        if [ "$v" = "${w#*=}" ]; then echo "[result] $1 $2 $tag $v (expected)"
        else echo "[result] $1 $2 $tag $v (UNEXPECTED, want ${w#*=})"; rc_all=1; fi
    done
}

for a in $ARCHES; do
    for m in $ARMS; do arm "$a" "$m"; done
done
if [ "$rc_all" -eq 0 ]; then echo "[nosw-bare] PASS"; else echo "[nosw-bare] FAIL"; fi
exit "$rc_all"
