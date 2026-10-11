#!/bin/bash
# tests/ci/build_all.sh [logdir] — build (do not run) everything CI builds, with
# CI's own commands, one after another, and say which ones broke.
#
# Run it in a scratch tree before merging into the local master (inbox #27):
# the link errors of CI run 38093212146 (#22) were builds nobody had made by
# hand. Takes a few minutes. Exit 0 = every build passed; otherwise the number
# of broken builds. Logs: <logdir>/<name>.log (default: a mktemp dir).
#
# It cleans and rebuilds the tree it sits in (make clean in each boot dir).
#
# Covered: hosted x86_64 (plain, ASan+UBSan, the three anti-theater stubs),
# hosted aarch64 (cross), Windows PE (mingw-w64), bare i686 + ring3 userland
# (all disk), bare AArch64 (virt and rpi3), relay, and tests/llm/run_dmoe.sh
# (compiles its own translation units outside the Makefiles, broke in #22;
# it also runs its gates, ~40 s).
# NOT covered: the Android APK (needs the NDK and gradle) and the other test
# scripts under tests/ that compile their own programs; CI is still the judge
# for those. (run_ss3_blob.sh also broke in #22 but is not in CI: its
# [ss3-blob-merge] is a known red, see ci.yml.)
set -u
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
LOG="${1:-$(mktemp -d)}"
mkdir -p "$LOG"
cd "$ROOT"
fails=0
failed=""
step() {  # $1 = name, rest = command (run with bash -eo pipefail, like CI)
    local name="$1"; shift
    local t0=$SECONDS
    if bash --noprofile --norc -eo pipefail -c "$*" > "$LOG/$name.log" 2>&1; then
        echo "[build] ok   $name ($((SECONDS - t0)) s)"
    else
        echo "[build] FAIL $name ($((SECONDS - t0)) s) — $LOG/$name.log"
        grep -a -m3 -E 'error|undefined reference' "$LOG/$name.log" | sed 's/^/         /'
        fails=$((fails + 1)); failed="$failed $name"
    fi
}

step x86_64       "make -C boot/linux_x86_64 clean >/dev/null && make -C boot/linux_x86_64"
step x86_64-san   "make -C boot/linux_x86_64 clean >/dev/null && make -C boot/linux_x86_64 NO_LIBTCC=1 \
                     EXTRA_CFLAGS='-fsanitize=address,undefined -fno-omit-frame-pointer -fno-sanitize-recover=all' \
                     LDFLAGS='-no-pie -lrt -lpthread -fsanitize=address,undefined'"
for d in CONSCIENCE_STUB SABOTAGE_FRONTIER_NOLABEL SCALING_AGG_STUB; do
    step "x86_64-$d" "make -C boot/linux_x86_64 clean >/dev/null && make -C boot/linux_x86_64 EXTRA_CFLAGS=-D$d"
done
step x86_64-again "make -C boot/linux_x86_64 clean >/dev/null && make -C boot/linux_x86_64"
step aarch64      "make -C boot/linux clean >/dev/null && make -C boot/linux"
step windows      "make -C boot/windows/x86_64 clean >/dev/null && make -C boot/windows/x86_64"
step bare-x86     "make -C boot/x86 clean >/dev/null && make -C boot/x86 all disk"
step bare-rpi3    "make -C boot/aarch64 rpi3"
step bare-aarch64 "make -C boot/aarch64 clean >/dev/null && make -C boot/aarch64"
step relay        "make -C relay"
step dmoe         "bash tests/llm/run_dmoe.sh"

if [ "$fails" -eq 0 ]; then
    echo "[build] ALL OK (logs: $LOG)"
else
    echo "[build] $fails FAILED:$failed (logs: $LOG)"
fi
exit "$fails"
