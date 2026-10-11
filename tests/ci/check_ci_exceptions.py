#!/usr/bin/env python3
# tests/ci/check_ci_exceptions.py — every red that ci.yml allows must say until
# when (ROADMAP 0-13). Each `continue-on-error: true` line needs a trailing
#   # until YYYY-MM-DD: <reason>
# with a date that has not passed, or, for a step the owner made non-blocking on
# purpose (it informs, it does not stop), a trailing
#   # advisory: <who decided, where>
# Exit 1 lists every line without one, and every passed date. `--selftest`
# runs fixtures (each kind of offence must redden, a good file must pass).
#
# No PyYAML: a text scan of the workflow, like check_script_modes.py.
import datetime, re, sys

LINE = re.compile(r'^\s*continue-on-error:\s*true\b(.*)$')
UNTIL = re.compile(r'#\s*until\s+(\d{4}-\d{2}-\d{2})\s*:\s*\S')
ADVISORY = re.compile(r'#\s*advisory\s*:\s*\S')


def check(text, today):
    bad, n_until, n_adv = [], 0, 0
    for i, line in enumerate(text.splitlines(), 1):
        m = LINE.match(line)
        if not m:
            continue
        rest = m.group(1)
        u = UNTIL.search(rest)
        if u:
            try:
                d = datetime.date.fromisoformat(u.group(1))
            except ValueError:
                bad.append((i, 'bad date ' + u.group(1))); continue
            if d < today:
                bad.append((i, 'expired ' + u.group(1)))
            else:
                n_until += 1
        elif ADVISORY.search(rest):
            n_adv += 1
        else:
            bad.append((i, 'no "# until YYYY-MM-DD: reason" (or "# advisory: ...")'))
    return bad, n_until, n_adv


def selftest():
    today = datetime.date(2026, 10, 11)
    cases = [
        ('missing marker', '    continue-on-error: true\n', 1),
        ('marker without reason', '    continue-on-error: true  # until 2026-11-30:\n', 1),
        ('expired', '    continue-on-error: true  # until 2026-10-10: x\n', 1),
        ('impossible date', '    continue-on-error: true  # until 2026-02-30: x\n', 1),
        ('advisory without who', '    continue-on-error: true  # advisory:\n', 1),
        ('good until (today counts)', '    continue-on-error: true  # until 2026-10-11: smp0\n', 0),
        ('good advisory', '    continue-on-error: true  # advisory: owner, inbox #12.4\n', 0),
        ('false is not an exception', '    continue-on-error: false\n', 0),
        ('comment line is not an exception', '    # continue-on-error: true\n', 0),
    ]
    fails = 0
    for name, text, want in cases:
        got = len(check(text, today)[0])
        ok = got == want
        print('[ci-exc-selftest] %s %s (offences %d, want %d)' % ('PASS' if ok else 'FAIL', name, got, want))
        fails += 0 if ok else 1
    print('[ci-exc-selftest] %s' % ('ALL PASS' if fails == 0 else '%d FAILED' % fails))
    return 1 if fails else 0


if __name__ == '__main__':
    if len(sys.argv) > 1 and sys.argv[1] == '--selftest':
        sys.exit(selftest())
    repo = sys.argv[1] if len(sys.argv) > 1 else '.'
    wf = sys.argv[2] if len(sys.argv) > 2 else '.github/workflows/ci.yml'
    text = open(repo.rstrip('/') + '/' + wf, encoding='utf-8').read()
    bad, n_until, n_adv = check(text, datetime.date.today())
    for i, why in bad:
        print('check_ci_exceptions: %s:%d: %s' % (wf, i, why))
    print('check_ci_exceptions: %d allowed reds with a date, %d advisory, %d offences'
          % (n_until, n_adv, len(bad)))
    sys.exit(1 if bad else 0)
