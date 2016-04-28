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
 *	@(#)profile_common.h (sys)
 *
 *	System shared information  
 */

#ifndef __SYS_PROFILE_COMMON_H__
#define __SYS_PROFILE_COMMON_H__

// #ifdef _APP_AT91_
// #  include <sys/sysdepend/app_at91/profile_depend.h>
// #endif

// #ifdef _APP_H8S2212_
// #  include <sys/sysdepend/app_h8s2212/profile_depend.h>
// #endif

// #ifdef _APP_MB9AF312K_
// #  include <sys/sysdepend/app_mb9af312k/profile_depend.h>
// #endif

#ifdef _APP_RL78G13_R5F100ADASP_
#  include <sys/sysdepend/app_rl78g13_r5f100adasp/profile_depend.h>
#endif

#endif /* __SYS_PROFILE_COMMON_H__ */
