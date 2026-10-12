/*
 *  arch/windows/x86_64/pool_preempt_stub.c
 *
 *  The shared hosted shell (arch/linux/x86_64/usermain.c) dispatches
 *  `tkconf pool` / `tkconf nosw` / `tkconf fp` / `tkconf nr` to
 *  arch/linux/pool_preempt.c.
 *  Those probes test the hosted exit dispatch (SIGALRM + arch/linux/<arch>/
 *  preempt.c), which the Windows port does not have (Fiber dispatcher,
 *  no preempt.c; pk_nosw.h is empty here). This TU provides honest stubs
 *  instead of compiling the POSIX-signal probes, the same way selfc_stub.c
 *  does for selfc. CI run 38093212146 (2026-10-11) failed to link without it.
 */

#include "kernel.h"

static INT not_here(void (*out)(const char *), const char *what)
{
    out("[win] tkconf ");
    out(what);
    out(": not on Windows (no hosted exit dispatch; see pool_preempt_stub.c)\r\n");
    return E_NOSPT;
}

INT pool_preempt_run(void (*out)(const char *)) { return not_here(out, "pool"); }
INT nosw_run(void (*out)(const char *))         { return not_here(out, "nosw"); }
INT fp_preserve_run(void (*out)(const char *))  { return not_here(out, "fp"); }
INT nosw_relay_run(void (*out)(const char *))   { return not_here(out, "nr"); }
