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
 *      @(#)str_align_common.h (sys)
 *
 *	Bit alignment definitions for structure
 */

#ifndef __SYS_STR_ALIGN_COMMON_H__
#define __SYS_STR_ALIGN_COMMON_H__

// #ifdef APP_AT91
// #  include <sys/sysdepend/app_at91/str_align_depend.h>
// #endif

// #ifdef APP_H8S2212
// #  include <sys/sysdepend/app_h8s2212/str_align_depend.h>
// #endif

// #ifdef _APP_MB9AF312K_
// #  include <sys/sysdepend/app_mb9af312k/str_align_depend.h>
// #endif

#ifdef _APP_RL78G13_R5F100ADASP_
#  include <kernel/tkernel/str_align_depend.h>
#endif

#endif /* __SYS_STR_ALIGN_COMMON_H__ */
