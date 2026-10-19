#!/usr/bin/env python3
"""stop_galaxy.py <p-kernel> <workdir> <port> - drives one hosted node for
tests/host/run_stop_galaxy.sh (ROADMAP 0-14 S2 stage 2b: galaxy POST /teach
obeys the stop). Prints "[stop-galaxy] <check> PASS|FAIL" lines."""
import os, subprocess, sys, time, urllib.request, urllib.error

pk, work, port = sys.argv[1], sys.argv[2], int(sys.argv[3])
base = "http://127.0.0.1:%d" % port
log = open(os.path.join(work, "node.log"), "wb")
env = dict(os.environ, PKERNEL_NODE_ID="1", PKERNEL_PFS_DIR=os.path.join(work, "pfs"),
           PKERNEL_GALAXY_PORT=str(port))
os.makedirs(env["PKERNEL_PFS_DIR"], exist_ok=True)
p = subprocess.Popen([pk], stdin=subprocess.PIPE, stdout=log, stderr=subprocess.STDOUT, env=env)


def res(name, ok):
    print("[stop-galaxy] %s %s" % (name, "PASS" if ok else "FAIL"), flush=True)


def http(path, body=None):
    req = urllib.request.Request(base + path, data=body.encode() if body is not None else None)
    try:
        with urllib.request.urlopen(req, timeout=120) as r:
            return r.status, dict(r.headers), r.read().decode("utf-8", "replace")
    except urllib.error.HTTPError as e:
        return e.code, dict(e.headers), e.read().decode("utf-8", "replace")


def shell(line):
    p.stdin.write((line + "\n").encode()); p.stdin.flush()


def node_log():
    log.flush()
    return open(os.path.join(work, "node.log"), "rb").read().decode("utf-8", "replace")


def wait_log(pat, n_before, secs):
    t = time.time() + secs
    while time.time() < t:
        if node_log().count(pat) > n_before:
            return True
        time.sleep(0.5)
    return False


try:
    mid = None
    for _ in range(120):
        try:
            st, hd, _b = http("/manifesto")
            mid = hd.get("X-Manifesto-Id")
            break
        except (urllib.error.URLError, ConnectionError, OSError):
            time.sleep(1)
    res("G0-up", mid is not None)
    st, _h, b = http("/profile", "ack=1&mid=%s" % mid)
    res("G1-consent", st == 200)
    # G2: not stopped -> taught
    st, _h, b = http("/teach", "k=sky&v=blue")
    res("G2-teach", st == 200 and '"ok":true' in b)
    # G3: stopped at the console -> 403 refused:stopped, nothing taught
    shell("stop now")
    up = wait_log("[stop] stopped 1", 0, 30)
    st, _h, b3 = http("/teach", "k=sea&v=green")
    res("G3-refused", up and st == 403 and '"refused":"stopped"' in b3)
    # G4: resumed -> taught again
    shell("stop resume")
    wait_log("[stop] stopped 0", 0, 30)
    st, _h, b = http("/teach", "k=ink&v=fresh")
    res("G4-teach", st == 200 and '"ok":true' in b)
    shell("mind provenance")
    wait_log("[mind]   \"ink\"", 0, 60)
    time.sleep(1)
    L = node_log()
    res("G5-facts", '[mind]   "sky" -> ' in L and '[mind]   "ink" -> ' in L and '[mind]   "sea" -> ' not in L)
    print("[stop-galaxy] G3 reply: %s" % b3.strip()[:120], flush=True)
finally:
    try:
        shell("exit")
        p.wait(timeout=20)
    except Exception:
        p.kill()
    log.close()
