#!/bin/bash
# ---------------------------------------------------------------------------
# run_idle_freed_stack_a64.sh — host cert for D5-k on linux_aarch64
# (the Android build runs the same kernel). Twin of run_idle_freed_stack.sh.
#
# THE BUG: tk_exd_tsk frees its own system stack (knl_del_tsk -> knl_Ifree)
# and then force-dispatches while still standing on it. If no task is runnable,
# the hosted idle (knl_idle_wait) runs on that freed Imalloc block.
#
# THE FIX: knl_dispatch_to_schedtsk switches sp to the static knl_tmp_stack
# first (as linux_x86_64 and upstream armv7m / rxv2 do).
#
# THE DETECTOR: .Lidle calls knl_idle_sp_check(sp), which aborts if sp is
# inside a free Imalloc area.
#
# HOW: as on x86_64, gdb stops the shell task in cmd_ver and calls
# drpc_local_restart(2, 4, 0): remote_counter, which sleeps 5 x 1 s and then
# tk_exd_tsk's while the shell is blocked on input -> the dispatcher idles.
# The kernel runs under qemu-aarch64 -g and gdb-multiarch attaches to it.
#   PASS: "Counter done", no detector abort, the kernel exits normally.
#   FAIL: the detector fires, or no counter, or no normal exit.
#
# Needs qemu-aarch64(-static), gdb-multiarch and the aarch64 cross toolchain.
# Exit 0 = PASS, 1 = FAIL, 2 = cannot run here.
# ---------------------------------------------------------------------------
set -u
HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
BOOT="$ROOT/boot/linux"
PORT="${IDLE_FREED_GDB_PORT:-12345}"

QEMU=""
for q in qemu-aarch64-static qemu-aarch64; do
    command -v "$q" >/dev/null && { QEMU="$q"; break; }
done
[ -n "$QEMU" ] || { echo "SKIP: qemu-aarch64 not installed"; exit 2; }
command -v gdb-multiarch >/dev/null || { echo "SKIP: gdb-multiarch not installed"; exit 2; }
[ -x "$BOOT/p-kernel" ] || make -C "$BOOT" >/dev/null || exit 1

CMDS=$(mktemp)
OUT=/tmp/idle_freed_stack_a64.log
KOUT=/tmp/idle_freed_stack_a64.kernel.log
trap 'rm -f "$CMDS"' EXIT
cat > "$CMDS" <<EOF
set pagination off
set confirm off
set unwindonsignal on
handle SIGALRM nostop noprint pass
handle SIGUSR1 nostop noprint pass
handle SIGUSR2 nostop noprint pass
handle SIGIO nostop noprint pass
handle SIGPIPE nostop noprint pass
handle SIGABRT stop print
target remote :$PORT
tbreak cmd_ver
continue
print (long)drpc_local_restart(2, 4, 0)
continue
bt 8
kill
quit
EOF

# The kernel's own stdout/stderr (the counter and the detector) go to KOUT;
# gdb's view (the stop on SIGABRT, the normal exit) goes to OUT.
{ sleep 5; echo ver; sleep 25; echo ver; sleep 3; echo exit; sleep 5; } | \
    timeout 120 "$QEMU" -g "$PORT" -L /usr/aarch64-linux-gnu "$BOOT/p-kernel" > "$KOUT" 2>&1 &
QPID=$!
sleep 1
timeout 110 gdb-multiarch -q -batch -x "$CMDS" "$BOOT/p-kernel" > "$OUT" 2>&1
wait "$QPID" 2>/dev/null

FAIL=0
if grep -aq 'idle is running on a freed' "$KOUT" "$OUT"; then
    echo "FAIL: idle ran on a freed Imalloc area after tk_exd_tsk"
    grep -ah 'idle is running on a freed' "$KOUT" "$OUT" | head -1
    FAIL=1
fi
grep -aq 'Counter done' "$KOUT" || { echo "FAIL: remote_counter never finished (harness did not reach tk_exd_tsk)"; FAIL=1; }
grep -aq 'exited normally' "$OUT" || { echo "FAIL: kernel did not exit normally"; FAIL=1; }

if [ "$FAIL" -eq 0 ]; then
    echo "PASS — tk_exd_tsk with nothing runnable idles off the freed stack (aarch64)"
fi
echo "log: $OUT $KOUT"
exit "$FAIL"
