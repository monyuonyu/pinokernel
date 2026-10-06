/*
 *  pk_nosw.h — no-switch regions for the hosted exit dispatch.
 *
 *  On hosted Linux (x86_64, and aarch64 = Android) the timer may switch tasks
 *  at any instruction of p-kernel's own code (arch/linux/<arch>/preempt.c). State shared between
 *  tasks that was safe only because "nobody switches me out between kernel
 *  calls" (static send buffers, kdds topic slots, ...) is bracketed with
 *  PK_NOSW_ENTER / PK_NOSW_LEAVE. The depth is per task, so a task that
 *  blocks in a tk_* call inside a region does not stop preemption of the
 *  others; leaving the outermost region makes the switch the timer skipped.
 *  The same on bare metal x86 / AArch64 (below). Everywhere else (Windows,
 *  host tests) the macros are empty. -DPK_NOSW_OFF: empty everywhere (the
 *  negative control for `tkconf nosw` and PK_TKCONF_NOSW).
 */
#ifndef PK_NOSW_H
#define PK_NOSW_H

/* PK_NOSW_HOSTED: the hosted Makefiles (x86_64 and aarch64) compile the LLM tier (student.c,
 * dlb.c, cradle.c, ...) with LLM_CFLAGS, which has no -D_LINUX_X86_64_; without
 * this second key their regions were silently empty (2026-10-06: tkconf nosw
 * N6 / N14 red with the regions, while N7 was green only because the probe
 * expands ST_TRAIN_STEP under the kernel CFLAGS). */
/* Bare metal x86 (_X86_PC_) and AArch64 (_AARCH64_VIRT_) switch at the
 * outermost interrupt exit too (RNG0), so the regions are live there as well
 * (R-NOSW-BARE, 2026-10-07; the depth and the exit check are in the two
 * kernel/.../sysdepend/<target>/cpu_cntl.c). Not in the AArch64 SMP self-test
 * build, which keeps its own exit path (cpu_cntl.c's SMP_SELFTEST). */
#if ((defined(_LINUX_X86_64_) || defined(_LINUX_AARCH64_) || defined(PK_NOSW_HOSTED)) || \
     ((defined(_X86_PC_) || defined(_AARCH64_VIRT_)) && !defined(SMP_SELFTEST))) \
    && !defined(PK_NOSW_OFF)
#define PK_NOSW_ACTIVE 1        /* a file may wrap a whole function only then */
void pk_nosw_enter(void);
void pk_nosw_leave(void);
#define PK_NOSW_ENTER() pk_nosw_enter()
#define PK_NOSW_LEAVE() pk_nosw_leave()
#else
#define PK_NOSW_ACTIVE 0
#define PK_NOSW_ENTER() ((void)0)
#define PK_NOSW_LEAVE() ((void)0)
#endif

/* A whole function as one region, written in front of its body:
 *   PK_NOSW_FN(INT, pfs_get, (const U1 *id, void *buf, UW n), (id, buf, n))
 *   { ...the body, returns as before... }
 * On hosted Linux the body becomes `static ret name_nosw(...)` and `name`
 * brackets it; elsewhere it is just `ret name(...)`, the same code as before. */
#if PK_NOSW_ACTIVE
#define PK_NOSW_FN(ret, name, params, args)                                 \
    static ret name##_nosw params;                                          \
    ret name params { PK_NOSW_ENTER(); ret r_ = name##_nosw args;           \
                      PK_NOSW_LEAVE(); return r_; }                         \
    static ret name##_nosw params
#else
#define PK_NOSW_FN(ret, name, params, args) ret name params
#endif

#endif /* PK_NOSW_H */
