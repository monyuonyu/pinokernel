#!/bin/bash
# tests/host/run_mind_explain.sh — ROADMAP 1-4, `mind explain <key>`
# (docs/architecture/30-module/mind-explain.md).
#
# One node, no networking: explain a key never taught, then teach sky blue
# and sky green, explain sky three times, then `mind ask sky`.
#   explain-none        : before any teach: "current: none held"
#   explain-current     : "current: "sky" -> "green""
#   explain-learned     : taught here, prov kept, with the local seq
#   explain-consolidated: PENDING|RETAINED n/N sleep rounds, salience 1
#   explain-revised     : the 1-3a record: blue (taught here) superseded by green
#   explain-count       : "revisions kept: 1"
#   explain-readonly    : the ask after three explains reports salience=2
#                         (default 1 + this ask). Explain touching salience
#                         (-DMIND_EXPLAIN_TOUCHES) gives 5 -> RED.
#   explain-usage       : bare `mind explain` prints the usage line
# Output: "[mind-explain] <check> PASS|FAIL", then ALL PASS (exit 0) or RED (exit 1).
set -u
HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
case "$(uname -m)" in
    aarch64|arm64) BOOT="$ROOT/boot/linux" ;;
    *)             BOOT="$ROOT/boot/linux_x86_64" ;;
esac
[ -x "$BOOT/p-kernel" ] || make -C "$BOOT" >/dev/null || exit 1
unset PKERNEL_RELAY PKERNEL_RELAY_HOST PKERNEL_RELAY_PORT PKERNEL_LAN PKERNEL_AUTONET
WORK="$(mktemp -d /tmp/mexpl.XXXXXX)"
REDS=""
res() { echo "[mind-explain] $1 $2"; [ "$2" = FAIL ] && REDS="$REDS $1"; return 0; }
chk() {  # <name> <file> <ERE>
    if grep -aqE "$3" "$2"; then res "$1" PASS; else res "$1" FAIL; fi
}
L="$WORK/explain.log"; mkdir -p "$WORK/d"
printf 'mind explain\nmind explain sky\nmind teach sky blue\nmind teach sky green\nmind explain sky\nmind explain sky\nmind explain sky\nmind ask sky\nexit\n' \
    | timeout 180 env PKERNEL_NODE_ID=1 PKERNEL_PFS_DIR="$WORK/d" "$BOOT/p-kernel" > "$L" 2>&1
chk explain-usage        "$L" '^usage: mind explain <key-word.id>'
chk explain-none         "$L" '^\[mind\]   current: none held'
chk explain-current      "$L" '^\[mind\]   current: "sky" -> "green"'
chk explain-learned      "$L" '^\[mind\]   learned: taught here( \(node [0-9]+\))?, prov kept, local seq [0-9]+'
chk explain-consolidated "$L" '^\[mind\]   consolidated: (PENDING|RETAINED) [0-9]+/[0-9]+ sleep rounds, salience 1/'
chk explain-revised      "$L" '^\[mind\]   revised: key [0-9]+ "sky": "blue" \(taught here.*superseded by "green" taught here'
chk explain-count        "$L" '^\[mind\]   revisions kept: 1 '
chk explain-readonly     "$L" '^\[mind\]   salience=2/'
echo "[mind-explain] log: $L"
if [ -z "$REDS" ]; then echo "[mind-explain] ALL PASS"; exit 0; fi
echo "[mind-explain] RED:$REDS"; exit 1
