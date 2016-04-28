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
 *	sysinit.h (sysinit)
 *	Initialize System
 */

#ifndef _SYSINIT_
#define _SYSINIT_

#include <lib/libc/basic.h>
#include <kernel/tkernel/tkernel.h>
#include <kernel/tkernel/tmonitor.h>

/*
 * Platform dependent sequence
 */
IMPORT ER knl_init_device( void );
IMPORT ER knl_start_device( void );
IMPORT ER knl_finish_device( void );
IMPORT ER knl_restart_device( W mode );

#endif /* _SYSINIT_ */
