#!/bin/bash
# tests/host/run_mind_provenance.sh — ROADMAP 1-4, `mind provenance`
# (docs/architecture/30-module/mind-explain.md).
#
# One node, no networking: list before any teach, then teach sky blue and
# fire warm, list three times, then `mind ask sky`.
#   prov-empty    : before any teach: "provenance: 0 fact(s), 0 key(s): ..."
#   prov-sky      : the line for sky: "sky" -> "blue": taught here, prov
#                   kept, local seq n, PENDING|RETAINED
#   prov-fire     : the same for fire -> warm
#   prov-count    : "2 fact(s), 2 key(s): 2 taught here, 0 from other nodes,
#                   0 teacher unknown"
#   prov-readonly : the ask after three listings reports salience=2 (default
#                   1 + this ask). Listing touching salience
#                   (-DMIND_PROVENANCE_TOUCHES) gives 5 -> RED.
# Not covered here: the "from node n" kind (needs two nodes; explain's
# "learned:" uses the same helper, m_teacher_puts).
# Output: "[mind-prov] <check> PASS|FAIL", then ALL PASS (exit 0) or RED (exit 1).
set -u
HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
case "$(uname -m)" in
    aarch64|arm64) BOOT="$ROOT/boot/linux" ;;
    *)             BOOT="$ROOT/boot/linux_x86_64" ;;
esac
[ -x "$BOOT/p-kernel" ] || make -C "$BOOT" >/dev/null || exit 1
unset PKERNEL_RELAY PKERNEL_RELAY_HOST PKERNEL_RELAY_PORT PKERNEL_LAN PKERNEL_AUTONET
WORK="$(mktemp -d /tmp/mprov.XXXXXX)"
REDS=""
res() { echo "[mind-prov] $1 $2"; [ "$2" = FAIL ] && REDS="$REDS $1"; return 0; }
chk() {  # <name> <file> <ERE>
    if grep -aqE "$3" "$2"; then res "$1" PASS; else res "$1" FAIL; fi
}
L="$WORK/prov.log"; mkdir -p "$WORK/d"
printf 'mind provenance\nmind teach sky blue\nmind teach fire warm\nmind provenance\nmind provenance\nmind provenance\nmind ask sky\nexit\n' \
    | timeout 180 env PKERNEL_NODE_ID=1 PKERNEL_PFS_DIR="$WORK/d" "$BOOT/p-kernel" > "$L" 2>&1
T='taught here( \(node [0-9]+\))?, prov kept, local seq [0-9]+, (PENDING|RETAINED)'
chk prov-empty    "$L" '^\[mind\] provenance: 0 fact\(s\), 0 key\(s\): 0 taught here, 0 from other nodes, 0 teacher unknown'
chk prov-sky      "$L" "^\\[mind\\]   \"sky\" -> \"blue\": $T"
chk prov-fire     "$L" "^\\[mind\\]   \"fire\" -> \"warm\": $T"
chk prov-count    "$L" '^\[mind\] provenance: 2 fact\(s\), 2 key\(s\): 2 taught here, 0 from other nodes, 0 teacher unknown'
chk prov-readonly "$L" '^\[mind\]   salience=2/'
echo "[mind-prov] log: $L"
if [ -z "$REDS" ]; then echo "[mind-prov] ALL PASS"; exit 0; fi
echo "[mind-prov] RED:$REDS"; exit 1
