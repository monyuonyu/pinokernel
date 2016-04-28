/*
 *----------------------------------------------------------------------
 *    micro T-Kernel 2.0 Software Package
 *
 *    Copyright (C) 2006-2014 by Ken Sakamura.
 *    This software is distributed under the T-License 2.0.
 *----------------------------------------------------------------------
 *
 *    Released by T-Engine Forum(http://www.t-engine.org/) at 2014/09/01.
 *
 *----------------------------------------------------------------------
 */

/*
 *	@(#)syscall_common.h (tk)
 *
 *	micro T-Kernel
 */

#ifndef __TK_SYSCALL_COMMON_H__
#define __TK_SYSCALL_COMMON_H__

// #if APP_AT91
// #  include <tk/sysdepend/app_at91/cpuattr.h>
// #  include <tk/sysdepend/app_at91/cpudef.h>
// #endif

// #if APP_H8S2212
// #  include <tk/sysdepend/app_h8s2212/cpuattr.h>
// #  include <tk/sysdepend/app_h8s2212/cpudef.h>
// #endif

// #if APP_MB9AF312K
// #  include <tk/sysdepend/app_mb9af312k/cpuattr.h>
// #  include <tk/sysdepend/app_mb9af312k/cpudef.h>
// #endif

#if _APP_RL78G13_R5F100ADASP_
#  include <kernel/tkernel/cpuattr.h>
#  include <kernel/tkernel/cpudef.h>
#endif

#endif /* __TK_SYSCALL_COMMON_H__ */
