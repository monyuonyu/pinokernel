/*
 *  arch/linux/aarch64/preempt.c
 *  Signal-as-IRQ infrastructure, factored out of poc_preempt.c so the
 *  T-Kernel build can link against it.
 *
 *  Three things live here:
 *    1. arch_irq_disabled_flag and arch_irq_enable_with_drain —
 *       referenced by cpu_support.S and by cpu_status.h's macros.
 *    2. The SIGALRM signal handler. It calls into T-Kernel's
 *       knl_timer_handler_startup, which advances time and may set
 *       knl_schedtsk != knl_ctxtsk. We do NOT manipulate mcontext
 *       here — preemption happens at the next END_CRITICAL_SECTION
 *       (matching T-Kernel's bare-metal "preempt at safe point"
 *       semantics).
 *    3. arch_signals_init and arch_timer_start — called from
 *       tkdev_init.c during boot.
 *
 *  The Session 2 mcontext-rewrite approach is interesting but does not
 *  match T-Kernel's expectation that knl_dispatch is only entered from
 *  task context. Bridging that into the timer signal handler is
 *  Session 4 work.
 */

/* preempt.c does NOT include any T-Kernel headers — it only references
 * arch_irq_disabled_flag (a plain int) and the asm-defined symbol
 * knl_timer_handler_startup. Pure POSIX is enough. */
#define _GNU_SOURCE
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>

/* The flag itself — read and written by cpu_support.S via adrp/add.
 * Volatile sig_atomic_t so concurrent updates from signal handler and
 * task context don't tear. */
volatile int arch_irq_disabled_flag = 0;

static volatile sig_atomic_t pending_timer_ticks = 0;

/* The POSIX timer that delivers SIGALRM. File-scope so the signal
 * handler can call timer_getoverrun() to recover ticks Linux collapsed
 * while we were not running. */
static timer_t arch_alarm_tid;
static int     arch_alarm_tid_created = 0;

/* T-Kernel timer bridge — defined in cpu_support.S. Increments
 * knl_taskindp around the call to knl_timer_handler. */
extern void knl_timer_handler_startup(void);

extern void sio_send_frame(const unsigned char *buf, int size);

void arch_linux_trampoline_marker(void)
{
    sio_send_frame((const unsigned char *)"[asm] trampoline reached\r\n", 26);
}

void arch_linux_dispatch_loop_marker(void)
{
    sio_send_frame((const unsigned char *)"[asm] dispatch loop entered\r\n", 29);
}

static void hexdump(const char *lbl, int lbl_len, unsigned long v) {
    unsigned char buf[24];
    sio_send_frame((const unsigned char *)lbl, lbl_len);
    for (int i = 0; i < 16; i++) {
        int n = (v >> ((15 - i) * 4)) & 0xF;
        buf[i] = (n < 10) ? ('0' + n) : ('a' + n - 10);
    }
    buf[16] = '\r'; buf[17] = '\n';
    sio_send_frame(buf, 18);
}

void arch_linux_dispatch_after_cbz(unsigned long sched) { hexdump("[asm] after cbz, sched=", 23, sched); }
void arch_linux_dispatch_after_sp_switch(unsigned long sp_v) { hexdump("[asm] after mov sp, sp=", 23, sp_v); }
void arch_linux_dispatch_after_restore(unsigned long x30_v) { hexdump("[asm] after restore, x30=", 25, x30_v); }

void arch_linux_trampoline_about_to_br(unsigned long x17)
{
    unsigned char buf[24];
    sio_send_frame((const unsigned char *)"[asm] br x17 target=", 20);
    for (int i = 0; i < 16; i++) {
        int n = (x17 >> ((15 - i) * 4)) & 0xF;
        buf[i] = (n < 10) ? ('0' + n) : ('a' + n - 10);
    }
    buf[16] = '\r'; buf[17] = '\n';
    sio_send_frame(buf, 18);
}

void arch_irq_enable_with_drain(void)
{
    arch_irq_disabled_flag = 0;
    /* Drain every deferred tick. Bursts at re-enable time are
     * acceptable; dropping ticks is not, because tk_dly_tsk and other
     * timeout-bearing waits drift behind wall-clock by the amount we
     * drop. The handler call is short (queue walk + a few comparisons)
     * so even a 100-tick catch-up after a 1 s IRQ-disabled window
     * costs well under a millisecond. */
    sig_atomic_t to_replay = pending_timer_ticks;
    pending_timer_ticks = 0;
    while (to_replay-- > 0) {
        knl_timer_handler_startup();
    }
}

/* Examined opaquely (NULL / non-NULL only) so preempt.c stays free of
 * T-Kernel headers, per the file banner. Real type is TCB *. */
extern void * volatile knl_schedtsk;

/*
 *  knl_idle_wait — sleep the kernel thread until the next SIGALRM tick.
 *
 *  Called from the dispatcher's idle path (.Lidle in cpu_support.S) in
 *  place of the old sched_yield() busy-spin, which returned immediately
 *  and pegged a core at ~100% CPU when no task was runnable (observed
 *  live: a kernel pthread eating one core for 25 min, starving the UI).
 *
 *  A task can become runnable only from running-task context (then we
 *  are not idle) or from the SIGALRM handler (knl_timer_handler_startup
 *  may set knl_schedtsk). So sleeping until SIGALRM is exactly correct.
 *
 *  Race-free: block SIGALRM, then re-check knl_schedtsk. If a task is
 *  now runnable the tick that made it so is held PENDING (not lost), so
 *  we return at once without sleeping. Otherwise sigsuspend() atomically
 *  installs a mask that unblocks SIGALRM and sleeps; the pending/next
 *  tick delivers, sigalrm_handler runs (arch_irq_disabled_flag is 0 on
 *  this path — .Lidle did IRQ_FLAG_CLEAR — so the tick runs directly and
 *  may set knl_schedtsk), and sigsuspend returns with EINTR. We restore
 *  the prior mask and the dispatcher loops to re-check schedtsk.
 *
 *  Worst-case wake latency is one tick (~10 ms at 100 Hz): the bounded,
 *  documented tradeoff for going from 100% CPU to idle. The periodic
 *  timer keeps firing, so a missed-wakeup HANG is impossible.
 *
 *  Mask choice: we unblock exactly SIGALRM and leave everything else as
 *  the caller had it — fault signals (SIGSEGV/SIGBUS/SIGFPE) and term
 *  signals (SIGINT/SIGTERM) stay deliverable, so crash handling and
 *  Ctrl-C behave exactly as before.
 */
void knl_idle_wait(void)
{
    sigset_t block_alrm, prev, wait_mask;
    sigemptyset(&block_alrm);
    sigaddset(&block_alrm, SIGALRM);
    sigprocmask(SIG_BLOCK, &block_alrm, &prev);

    if (knl_schedtsk == 0) {
        wait_mask = prev;
        sigdelset(&wait_mask, SIGALRM);
        sigsuspend(&wait_mask);   /* sleeps; -1/EINTR after a signal */
    }

    sigprocmask(SIG_SETMASK, &prev, NULL);
}

#ifndef PK_HOSTED_NO_EXIT_DISPATCH
/* ---- ROADMAP 0-1, RNG0 on hosted Linux aarch64 (= Android): switch at the
 * "interrupt" exit (ledger RNG0-HOSTED-NO-SIGNAL-EXIT-DISPATCH). The x86_64
 * sibling (arch/linux/x86_64/preempt.c) has the full story; the rules are the
 * same: knl_dispatch is entered from task context only, never inside the
 * handler; only when the interrupted PC is in p-kernel's own text
 * (pk_text_begin..pk_text_end, checked at boot) and not in the trampoline; not
 * while the shared matmul pool is out (pk_parallel_busy).
 *
 * What differs: AArch64 user code cannot restore every register and then
 * branch to the interrupted PC (`br xN` / `ret` need a register that may be
 * live at that PC). So the handler COPIES the kernel's signal frame (siginfo +
 * ucontext, i.e. every register, NZCV, the FP/SIMD state and the signal mask
 * at the interruption) to the top of this task's side stack and resumes the
 * task at arch_preempt_tramp with SP just below the copy. The trampoline calls
 * arch_preempt_dispatch (the dispatch frame lives on the side stack while the
 * task is switched out), clears the busy word and issues rt_sigreturn with SP
 * at the copy: the kernel restores the whole interrupted context exactly as
 * an ordinary signal return would. A frame with an extra_context record (its
 * data lives outside the copied bytes) is not preempted (old behaviour).
 * pk_nosw.h regions work as on x86_64 (per-task depth; leaving the outermost
 * one makes the skipped switch).
 * -DPK_HOSTED_NO_EXIT_DISPATCH: the old behaviour (negative control). */
#include <ucontext.h>
#include <stdint.h>
#include <stddef.h>
#include <pthread.h>

extern void * volatile knl_ctxtsk;
extern volatile int knl_dispatch_disabled;
extern volatile int knl_taskindp;
extern void knl_dispatch_entry(void);
extern int tk_get_tid(void);
extern char pk_text_begin[], pk_text_end[];  /* pk_text_begin.c / pk_text_end.c */
void arch_preempt_tramp(void);
void arch_preempt_dispatch(void);
extern char arch_preempt_tramp_end[];

#define SIDE_SLOTS 129                       /* task ids 1..128 (CNF_MAX_TSKID) */
#define SIDE_SZ    16384                     /* frame copy (~4.7 KB) + dispatch */
#define FRAME_SZ   ((sizeof(siginfo_t) + sizeof(ucontext_t) + 15) & ~(size_t)15)
#define EXTRA_MAGIC_A64 0x45585401u          /* asm/sigcontext.h EXTRA_MAGIC */
#define FPSIMD_MAGIC_A64 0x46508001u         /* asm/sigcontext.h FPSIMD_MAGIC */
static uint64_t side_stack[SIDE_SLOTS][SIDE_SZ / 8] __attribute__((aligned(64)));
static int pk_text_ok;                       /* set by pk_text_check() at boot */

static int in_pk_text(uintptr_t a)
{
    return a >= (uintptr_t)pk_text_begin && a < (uintptr_t)pk_text_end;
}

/* the link-order bracket must hold libc outside and p-kernel inside */
static void pk_text_check(void)
{
    const uintptr_t libc[] = {
        (uintptr_t)&malloc, (uintptr_t)&free, (uintptr_t)&memcpy,
        (uintptr_t)&printf, (uintptr_t)&pthread_mutex_lock,
    };
    int ok = (uintptr_t)pk_text_begin < (uintptr_t)pk_text_end &&
             in_pk_text((uintptr_t)&arch_preempt_dispatch) &&
             in_pk_text((uintptr_t)&knl_dispatch_entry);
    for (unsigned i = 0; i < sizeof libc / sizeof libc[0]; i++)
        if (in_pk_text(libc[i])) ok = 0;
    pk_text_ok = ok;
    if (!ok)
        fprintf(stderr, "[preempt] p-kernel text bracket %p..%p holds libc or misses "
                "the kernel: exit dispatch OFF (old behaviour)\n",
                (void *)pk_text_begin, (void *)pk_text_end);
}

/* pk_nosw.h: per-task no-switch depth (index = task id) */
static unsigned short nosw_depth[SIDE_SLOTS];

/* knl_setup_context (task created / gone dormant): a task deleted while
 * switched out inside the trampoline must not leave its id marked busy,
 * nor a task deleted inside a no-switch region its id unpreemptable */
void arch_preempt_forget(int tskid)
{
    if (tskid > 0 && tskid < SIDE_SLOTS) {
        side_stack[tskid][SIDE_SZ / 8 - 1] = 0;
        nosw_depth[tskid] = 0;
    }
}

/* called by arch_preempt_tramp on the side stack, in task context */
void arch_preempt_dispatch(void)
{
    arch_irq_disabled_flag = 1;
    if (knl_ctxtsk != 0 && knl_ctxtsk != knl_schedtsk &&
        !knl_dispatch_disabled && knl_taskindp == 0)
        knl_dispatch_entry();               /* returns here when rescheduled */
    arch_irq_enable_with_drain();
}

/* x19 = the frame copy (SP on entry), x20 = the busy word; both set by
 * maybe_preempt in the live context (their interrupted values are in the copy) */
__asm__(
    "    .text\n"
    "    .globl arch_preempt_tramp\n"
    "    .globl arch_preempt_tramp_end\n"
    "    .type  arch_preempt_tramp, %function\n"
    "    .balign 16\n"
    "arch_preempt_tramp:\n"
    "    bl    arch_preempt_dispatch\n"
    "    mov   sp, x19\n"
    "    str   xzr, [x20]\n"                 /* busy word = 0 */
    "    mov   x8, #139\n"                   /* __NR_rt_sigreturn */
    "    svc   #0\n"
    "    brk   #0x3e8\n"                     /* not reached */
    "arch_preempt_tramp_end:\n"
    "    .size arch_preempt_tramp, . - arch_preempt_tramp\n"
);

extern volatile int pk_parallel_busy;        /* llm/pk_parallel.c: a pool job is out */
extern void (*pk_parallel_on_leave)(void);

/* pk_parallel.c calls this when its last dispatch is joined: the switch
 * maybe_preempt skipped while the pool was busy happens here (x86_64: `tkconf
 * pool` P5). -DPK_POOL_NO_LEAVE_DISPATCH: skip it (negative control). */
static void pk_pool_left(void)
{
#ifndef PK_POOL_NO_LEAVE_DISPATCH
    /* the pool used inside a pk_nosw.h region: the outermost pk_nosw_leave
     * dispatches instead. -DPK_POOL_LEAVE_IGNORES_NOSW: the old way (N18) */
#ifndef PK_POOL_LEAVE_IGNORES_NOSW
    int id = tk_get_tid();
    if (id > 0 && id < SIDE_SLOTS && nosw_depth[id]) return;
#endif
    if (pk_text_ok && !arch_irq_disabled_flag)
        arch_preempt_dispatch();
#endif
}

/* pk_nosw.h. Outside a task (id 0, or before the kernel runs) both are no-ops. */
void pk_nosw_enter(void)
{
    int id = tk_get_tid();
    if (id > 0 && id < SIDE_SLOTS) nosw_depth[id]++;
}

void pk_nosw_leave(void)
{
    int id = tk_get_tid();
    if (id > 0 && id < SIDE_SLOTS && nosw_depth[id] && --nosw_depth[id] == 0)
        pk_pool_left();
}

/* the frame's records must all sit inside the copied bytes */
static int frame_self_contained(const ucontext_t *uc)
{
    const unsigned char *p   = (const unsigned char *)uc->uc_mcontext.__reserved;
    const unsigned char *end = p + sizeof uc->uc_mcontext.__reserved;
    while (p + 8 <= end) {
        uint32_t magic, size;
        memcpy(&magic, p, 4); memcpy(&size, p + 4, 4);
        if (magic == 0) return 1;
        if (magic == EXTRA_MAGIC_A64 || size < 8 || size > (size_t)(end - p)) return 0;
        p += size;
    }
    return 0;
}

/* the thread arch_timer_start ran on (the T-Kernel thread). A tick taken on
 * any other thread (the pool's helpers are p-kernel code too) must never
 * enter the trampoline: it would dispatch on the wrong OS thread. Counted so
 * `tkconf pool` can show whether it happens (SIGEV_THREAD_ID should prevent
 * it; under qemu-user it was not known). */
static long arch_kernel_tid;
volatile unsigned long arch_preempt_foreign_ticks;

static void maybe_preempt(siginfo_t *si, ucontext_t *uc)
{
    if (arch_kernel_tid && syscall(SYS_gettid) != arch_kernel_tid) {
        arch_preempt_foreign_ticks++;
        return;
    }
    if (arch_irq_disabled_flag || knl_taskindp != 0 ||
        knl_dispatch_disabled || knl_ctxtsk == 0 || knl_ctxtsk == knl_schedtsk)
        return;
    if (pk_parallel_busy)
        return;                              /* the shared pool job: on leaving */
    uintptr_t pc = (uintptr_t)uc->uc_mcontext.pc;
    if (!pk_text_ok || !in_pk_text(pc))
        return;                              /* in libc etc.: not here */
    if (pc >= (uintptr_t)arch_preempt_tramp && pc < (uintptr_t)arch_preempt_tramp_end)
        return;                              /* on its way back */
    int id = tk_get_tid();
    if (id <= 0 || id >= SIDE_SLOTS) return;
    if (nosw_depth[id]) return;              /* pk_nosw.h region: on leaving */
    uint64_t *top = &side_stack[id][SIDE_SZ / 8];
    if (top[-1]) return;                     /* this task is already inside */
    /* the kernel's rt_sigframe is { siginfo; ucontext } back to back */
    if ((unsigned char *)uc != (unsigned char *)si + sizeof(siginfo_t)) return;
    if (!frame_self_contained(uc)) return;
    unsigned char *copy = (unsigned char *)&top[-2] - FRAME_SZ;  /* 16-aligned */
    memcpy(copy, si, sizeof(siginfo_t) + sizeof(ucontext_t));
#ifdef PK_PREEMPT_DROP_FP
    /* `tkconf fp` negative control (FP-PRESERVE): the copy's fpsimd record
     * loses v0-v31 (head 8 + fpsr 4 + fpcr 4, then 32 x 16 bytes), so
     * rt_sigreturn gives the task zeroed SIMD registers. F1 must go red. */
    {
        unsigned char *r = (unsigned char *)((ucontext_t *)(copy + sizeof(siginfo_t)))->uc_mcontext.__reserved;
        uint32_t magic;
        memcpy(&magic, r, 4);
        if (magic == FPSIMD_MAGIC_A64) memset(r + 16, 0, 512);
    }
#endif
    top[-1] = 1;
    uc->uc_mcontext.regs[19] = (unsigned long long)(uintptr_t)copy;
    uc->uc_mcontext.regs[20] = (unsigned long long)(uintptr_t)&top[-1];
    uc->uc_mcontext.sp = (unsigned long long)(uintptr_t)copy;
    uc->uc_mcontext.pc = (unsigned long long)(uintptr_t)arch_preempt_tramp;
}
#else
volatile unsigned long arch_preempt_foreign_ticks;
void arch_preempt_forget(int tskid) { (void)tskid; }
void pk_nosw_enter(void) { }
void pk_nosw_leave(void) { }
#endif

static void sigalrm_handler(int sig, siginfo_t *si, void *ucv)
{
    (void)sig; (void)si; (void)ucv;
    /* Linux collapses missed expirations of a periodic POSIX timer
     * into a single signal delivery and exposes the count via
     * timer_getoverrun(). Replay all of them so tick count tracks
     * wall-clock even after the process was descheduled. */
    int overrun = arch_alarm_tid_created ? timer_getoverrun(arch_alarm_tid) : 0;
    if (overrun < 0) overrun = 0;
    int to_run = 1 + overrun;

    if (arch_irq_disabled_flag) {
        pending_timer_ticks += to_run;
        return;
    }
    while (to_run-- > 0) {
        knl_timer_handler_startup();
    }
#ifndef PK_HOSTED_NO_EXIT_DISPATCH
    maybe_preempt(si, (ucontext_t *)ucv);
#endif
}

#define SIGSTACK_SZ (64 * 1024)
static unsigned char signal_stack[SIGSTACK_SZ] __attribute__((aligned(16)));

void arch_signals_init(void)
{
    stack_t ss = {
        .ss_sp = signal_stack, .ss_size = SIGSTACK_SZ, .ss_flags = 0,
    };
    if (sigaltstack(&ss, NULL) < 0) { perror("sigaltstack"); exit(1); }
#ifndef PK_HOSTED_NO_EXIT_DISPATCH
    pk_text_check();
    if (pk_text_ok) pk_parallel_on_leave = pk_pool_left;
#endif

    struct sigaction sa = { 0 };
    sa.sa_sigaction = sigalrm_handler;
    sa.sa_flags = SA_ONSTACK | SA_RESTART | SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGALRM, &sa, NULL) < 0) { perror("sigaction"); exit(1); }
}

void arch_timer_start(unsigned long period_us)
{
    if (!arch_alarm_tid_created) {
        struct sigevent sev = { 0 };
        /* Pin SIGALRM delivery to THIS thread (the T-Kernel thread).
         *
         * SIGEV_SIGNAL is process-directed: when the kernel runs on a
         * pthread inside a launcher (so_node, the Android JNI bridge),
         * Linux may deliver the tick to the launcher's main thread
         * instead. knl_timer_handler_startup then runs CONCURRENTLY
         * with task code on the kernel thread — arch_irq_disabled_flag
         * cannot protect across threads — and corrupts the ready/timer
         * queues. Observed in multi-node so_node runs as (a) a survivor
         * segfault and (b) a livelock: two nodes spinning forever at
         * the same PC in knl_tstdlib_bitsearch1 under
         * knl_ready_queue_delete, whose inner loop never terminates
         * once the bitmap/top_priority invariant is broken.
         *
         * arch_timer_start() is called during boot on the kernel
         * thread, so gettid() here is exactly the thread the signal-
         * as-IRQ emulation expects. */
#if defined(SIGEV_THREAD_ID)
        sev.sigev_notify = SIGEV_THREAD_ID;
# if defined(sigev_notify_thread_id)
        sev.sigev_notify_thread_id = (pid_t)syscall(SYS_gettid);
# else
        sev._sigev_un._tid         = (pid_t)syscall(SYS_gettid);
# endif
#else
        sev.sigev_notify = SIGEV_SIGNAL;   /* single-threaded fallback */
#endif
        sev.sigev_signo  = SIGALRM;
        if (timer_create(CLOCK_MONOTONIC, &sev, &arch_alarm_tid) < 0) {
            perror("timer_create"); exit(1);
        }
        arch_alarm_tid_created = 1;
#ifndef PK_HOSTED_NO_EXIT_DISPATCH
        arch_kernel_tid = (long)syscall(SYS_gettid);
#endif
    }
    struct itimerspec its = {
        .it_interval = { .tv_sec = period_us / 1000000,
                         .tv_nsec = (long)(period_us % 1000000) * 1000 },
        .it_value    = { .tv_sec = period_us / 1000000,
                         .tv_nsec = (long)(period_us % 1000000) * 1000 },
    };
    if (timer_settime(arch_alarm_tid, 0, &its, NULL) < 0) {
        perror("timer_settime"); exit(1);
    }
}
