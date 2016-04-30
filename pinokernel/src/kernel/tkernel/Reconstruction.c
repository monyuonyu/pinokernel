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
 *	@(#)asm.h
 *
 *	Assembler Macro
 */

#ifndef __TK_ASM_H__
#define __TK_ASM_H__

#include <sysdef.h>
#include <asm_common.h>

#endif /* __TK_ASM_H__ */
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
 *	@(#)asm_common.h (tk)
 *
 *	Assembler Macro
 */

#ifndef __TK_ASM_COMMON_H__
#define __TK_ASM_COMMON_H__

// #if APP_AT91
// #include <asm_depend.h>
// #endif

// #if APP_H8S2212
// #include <asm_depend.h>
// #endif

#if _APP_RL78G13_R5F100ADASP_
#include <asm_depend.h>
#endif

#endif /* __TK_ASM_COMMON_H__ */
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
 *	@(#)asm_depend.h (FM3)
 *
 *	Assembler Macro
 */

#ifndef __TK_ASM_DEPEND_H__
#define __TK_ASM_DEPEND_H__

/* ------------------------------------------------------------------------ */
/*
 *	Exception restore
 */

/*
 * Exception return
 */
.macro EXC_RETURN
//	THUMB
	bx	lr
.endm

/* ------------------------------------------------------------------------ */
#endif /* __TK_ASM_DEPEND_H__ */
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
 *	@(#)basic.h
 *
 *	Common basic header 
 *
 *	The header should normally be included at the start of 
 *	all source programs.
 */

#ifndef	__BASIC_H__
#define __BASIC_H__

#include <machine.h>
#include <stddef.h>
#include <typedef.h>

#endif /* __BASIC_H__ */
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
 *	bitop.c (tstdlib)
 *	T-Kernel common standard library
 */

/** [BEGIN Common Definitions] */
#include <basic.h>
#include <tkernel.h>
#include "utk_config.h"

/*** macros ***/
/* bit operation macro */
#if BIGENDIAN
#define _BIT_SET_N(n) ( (UB)0x80 >> ((n) & 7) )
#define _BIT_SHIFT(n) ( (UB)n >> 1 )
#else
#define _BIT_SET_N(n) ( (UB)0x01 << ((n) & 7) )
#define _BIT_SHIFT(n) ( (UB)n << 1 )
#endif
/** [END Common Definitions] */


/*** bit operation ***/
#ifdef USE_FUNC_TSTDLIB_BITCLR
/* tstdlib_bitclr : clear specified bit */
void
knl_tstdlib_bitclr( void *base, W offset )
{
	register UB *cp, mask;

	if (offset < 0) {
		return;
	}

	cp = (UB*)base;
	cp += offset / 8;

	mask = _BIT_SET_N(offset);

	*cp &= ~mask;
}
#endif /* USE_FUNC_TSTDLIB_BITCLR */

#ifdef USE_FUNC_TSTDLIB_BITSET
/* tstdlib_bitset : set specified bit */
void
knl_tstdlib_bitset( void *base, W offset )
{
	register UB *cp, mask;

	if (offset < 0) {
		return;
	}

	cp = (UB*)base;
	cp += offset / 8;

	mask = _BIT_SET_N(offset);

	*cp |= mask;
}
#endif /* USE_FUNC_TSTDLIB_BITSET */

#ifdef USE_FUNC_TSTDLIB_BITSEARCH1
/* tstdlib_bitsearch1 : perform 1 search on bit string */
W
knl_tstdlib_bitsearch1( void *base, W offset, W width )
{
	register UB *cp, mask;
	register W position;

	if ((offset < 0) || (width < 0)) {
		return -1;
	}

	cp = (UB*)base;
	cp += offset / 8;

	position = 0;
	mask = _BIT_SET_N(offset);

	while (position < width) {
		if (*cp) {		/* includes 1 --> search bit of 1 */
			while (1) {
				if (*cp & mask) {
					if (position < width) {
						return position;
					} else {
						return -1;
					}
				}
				mask = _BIT_SHIFT(mask);
				++position;
			}
		} else {		/* all bits are 0 --> 1 Byte skip */
			if (position) {
				position += 8;
			} else {
				position = 8 - (offset & 7);
				mask = _BIT_SET_N(0);
			}
			cp++;
		}
	}

	return -1;
}
#endif /* USE_FUNC_TSTDLIB_BITSEARCH1 */
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
 *	bitop.h (tstdlib)
 *
 *	T-Kernel common standard library
 *
 */

#ifndef	_BITOP_
#define _BITOP_

#include <stdtype.h>
#include "utk_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef	__size_t
typedef __size_t	size_t;
#undef	__size_t
#endif

#ifdef	__wchar_t
typedef __wchar_t	wchar_t;
#undef	__wchar_t
#endif

#define NULL		0

extern void knl_tstdlib_bitclr( void *base, W offset );
extern void knl_tstdlib_bitset( void *base, W offset );

extern W knl_tstdlib_bitsearch1( void *base, W offset, W width );

#ifdef __cplusplus
}
#endif
#endif /* _BITOP_ */
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
 *	check.h
 *	Macro for Error Check 
 */

#ifndef _CHECK_
#define _CHECK_

/*
 * Check object ID range (E_ID)
 */
#if CHK_ID
#define CHECK_TSKID(tskid) {					\
	if (!in_indp() && ((tskid) == TSK_SELF)) {		\
		return E_OBJ;					\
	} else if (!CHK_TSKID(tskid)) {				\
		return E_ID;					\
	}							\
}
#define CHECK_TSKID_SELF(tskid) {				\
	if ( !( (!in_indp() && (tskid) == TSK_SELF)		\
		|| CHK_TSKID(tskid) ) ) {			\
		return E_ID;					\
	}							\
}
#define CHECK_SEMID(semid) {					\
	if (!CHK_SEMID(semid)) {				\
		return E_ID;					\
	}							\
}
#define CHECK_FLGID(flgid) {					\
	if (!CHK_FLGID(flgid)) {				\
		return E_ID;					\
	}							\
}
#define CHECK_MBXID(mbxid) {					\
	if (!CHK_MBXID(mbxid)) {				\
		return E_ID;					\
	}							\
}
#define CHECK_MBFID(mbfid) {					\
	if (!CHK_MBFID(mbfid)) {				\
		return E_ID;					\
	}							\
}
#define CHECK_PORID(porid) {					\
	if (!CHK_PORID(porid)) {				\
		return E_ID;					\
	}							\
}
#define CHECK_MTXID(pisid) {					\
	if (!CHK_MTXID(pisid)) {				\
		return E_ID;					\
	}							\
}
#define CHECK_MPLID(mplid) {					\
	if (!CHK_MPLID(mplid)) {				\
		return E_ID;					\
	}							\
}
#define CHECK_MPFID(mpfid) {					\
	if (!CHK_MPFID(mpfid)) {				\
		return E_ID;					\
	}							\
}
#define CHECK_CYCID(cycid) {					\
	if (!CHK_CYCID(cycid)) {				\
		return E_ID;					\
	}							\
}
#define CHECK_ALMID(almid) {					\
	if (!CHK_ALMID(almid)) {				\
		return E_ID;					\
	}							\
}
#define CHECK_SSYID(ssid) {					\
	if (!CHK_SSYID(ssid)) {					\
		return E_ID;					\
	}							\
}
#define CHECK_SSYID_ALL(ssid) {					\
	if (!(CHK_SSYID(ssid) || (ssid) == 0)) {		\
		return E_ID;					\
	}							\
}
#else /* CHK_ID */
#define CHECK_TSKID(tskid)
#define CHECK_TSKID_SELF(tskid)
#define CHECK_SEMID(semid)
#define CHECK_FLGID(flgid)
#define CHECK_MBXID(mbxid)
#define CHECK_MBFID(mbfid)
#define CHECK_PORID(porid)
#define CHECK_MTXID(pisid)
#define CHECK_MPLID(mplid)
#define CHECK_MPFID(mpfid)
#define CHECK_CYCID(cycid)
#define CHECK_ALMID(almid)
#define CHECK_SSYID(ssid)
#define CHECK_SSYID_ALL(ssid)
#endif /* CHK_ID */

/*
 * Check whether its own task is specified (E_OBJ)
 */
#if CHK_SELF
#define CHECK_NONSELF(tskid) {					\
	if (!in_indp() && (tskid) == knl_ctxtsk->tskid) {		\
		return E_OBJ;					\
	}							\
}
#else /* CHK_SELF */
#define CHECK_NONSELF(tskid)
#endif /* CHK_SELF */

/*
 * Check task priority value (E_PAR)
 */
#if CHK_PAR
#define CHECK_PRI(pri) {					\
	if (!CHK_PRI(pri)) {					\
		return E_PAR;					\
	}							\
}
#define CHECK_PRI_INI(pri) {					\
	if ((pri) != TPRI_INI && !CHK_PRI(pri)) {		\
		return E_PAR;					\
	}							\
}
#define CHECK_PRI_RUN(pri) {					\
	if ((pri) != TPRI_RUN && !CHK_PRI(pri)) {		\
		return E_PAR;					\
	}							\
}
#else /* CHK_PAR */
#define CHECK_PRI(pri)
#define CHECK_PRI_INI(pri)
#define CHECK_PRI_RUN(pri)
#endif /* CHK_PAR */

/*
 * Check subsystem priority value (E_PAR)
 */
#if CHK_PAR
#define CHECK_SSYPRI(ssypri) {					\
	if (!CHK_SSYPRI(ssypri)) {				\
		return E_PAR;					\
	}							\
}
#else /* CHK_PAR */
#define CHECK_SSYPRI(ssypri)
#endif /* CHK_PAR */

/*
 * Check timeout specification value (E_PAR)
 */
#if CHK_PAR
#define CHECK_TMOUT(tmout) {					\
	if (!((tmout) >= TMO_FEVR)) {				\
		return E_PAR;					\
	}							\
}
#else /* CHK_PAR */
#define CHECK_TMOUT(tmout)
#endif /* CHK_PAR */

/*
 * Check other parameter errors (E_PAR)
 */
#if CHK_PAR
#define CHECK_PAR(exp) {					\
	if (!(exp)) {						\
		return E_PAR;					\
	}							\
}
#else /* CHK_PAR */
#define CHECK_PAR(exp)
#endif /* CHK_PAR */

/*
 * Check reservation attribute error (E_RSATR)
 */
#if CHK_RSATR
#define CHECK_RSATR(atr, maxatr) {				\
	if ((atr) & ~(maxatr)) {				\
	        return E_RSATR;					\
	}							\
}
#else /* CHK_RSATR */
#define CHECK_RSATR(atr, maxatr)
#endif /* CHK_RSATR */

/*
 * Check unsupported function (E_NOSPT)
 */
#if CHK_NOSPT
#define CHECK_NOSPT(exp) {					\
	if (!(exp)) {						\
		return E_NOSPT;					\
	}							\
}
#else /* CHK_NOSPT */
#define CHECK_NOSPT(exp)
#endif /* CHK_NOSPT */

/*
 * Check whether task-independent part is running (E_CTX)
 */
#if CHK_CTX
#define CHECK_INTSK() {						\
	if (in_indp()) {					\
		return E_CTX;					\
	}							\
}
#else /* CHK_CTX */
#define CHECK_INTSK()
#endif /* CHK_CTX */

/*
 * Check whether dispatch is in disabled state (E_CTX)
 */
#if CHK_CTX
#define CHECK_DISPATCH() {					\
	if (in_ddsp()) {					\
		return E_CTX;					\
	}							\
}
#define CHECK_DISPATCH_POL(tmout) {				\
	if ((tmout) != TMO_POL && in_ddsp()) {			\
		return E_CTX;					\
	}							\
}
#else /* CHK_CTX */
#define CHECK_DISPATCH()
#define CHECK_DISPATCH_POL(tmout)
#endif /* CHK_CTX */

/*
 * Check other context errors (E_CTX)
 */
#if CHK_CTX
#define CHECK_CTX(exp) {					\
	if (!(exp)) {						\
		return E_CTX;					\
	}							\
}
#else /* CHK_CTX */
#define CHECK_CTX(exp)
#endif /* CHK_CTX */

#endif /* _CHECK_ */
/*
 *----------------------------------------------------------------------
 *    micro T-Kernel 2.0 Software Package
 *
 *    Copyright (C) 2006-2014 by Ken Sakamura.
 *    This software is distributed under the T-License 2.0.
 *----------------------------------------------------------------------
 *
 *    Released by T-Engine Forum(http://www.t-engine.org/) at 2014/09/01.
 *    distribute by pkernel github(https://github.com/monyuonyu/) at 2016/04/29.
 *
 *----------------------------------------------------------------------
 */
/*--------------------------------------------------------------------*/
/*  Include definition                                                */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Struct definition                                                 */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Constant definition                                               */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Variable definition                                               */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Macro definition                                                  */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Prototype declaration                                             */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Function definition                                               */
/*--------------------------------------------------------------------*/

#include <typedef.h>

EXPORT void init_clock_control(void)
{
#if (HWWD_DISABLE)				/* HW Watchdog Disable */

#endif

#if (CLOCK_SETUP)				/* Clock Setup */

#endif

#if (CR_TRIM_SETUP)

#endif
}
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
 *	config.h
 *	System Configuration Definition
 */

#ifndef _CONFIG_
#define _CONFIG_

/* Task configuration */
#define MIN_TSKID	(1)
#define MAX_TSKID	(CFN_MAX_TSKID)
#define NUM_TSKID	(MAX_TSKID)
#define CHK_TSKID(id)	((MIN_TSKID) <= (id) && (id) <= (MAX_TSKID))
#define INDEX_TSK(id)	((id)-(MIN_TSKID))
#define ID_TSK(index)	((index)+(MIN_TSKID))

/* Semaphore configuration */
#define MIN_SEMID	(1)
#define MAX_SEMID	(CFN_MAX_SEMID)
#define NUM_SEMID	(MAX_SEMID)
#define CHK_SEMID(id)	((MIN_SEMID) <= (id) && (id) <= (MAX_SEMID))
#define INDEX_SEM(id)	((id)-(MIN_SEMID))
#define ID_SEM(index)	((index)+(MIN_SEMID))

/* Event flag configuration */
#define MIN_FLGID	(1)
#define MAX_FLGID	(CFN_MAX_FLGID)
#define NUM_FLGID	(MAX_FLGID)
#define CHK_FLGID(id)	((MIN_FLGID) <= (id) && (id) <= (MAX_FLGID))
#define INDEX_FLG(id)	((id)-(MIN_FLGID))
#define ID_FLG(index)	((index)+(MIN_FLGID))

/* Mailbox configuration */
#define MIN_MBXID	(1)
#define MAX_MBXID	(CFN_MAX_MBXID)
#define NUM_MBXID	(MAX_MBXID)
#define CHK_MBXID(id)	((MIN_MBXID) <= (id) && (id) <= (MAX_MBXID))
#define INDEX_MBX(id)	((id)-(MIN_MBXID))
#define ID_MBX(index)	((index)+(MIN_MBXID))

/* Mutex configuration */
#define MIN_MTXID	(1)
#define MAX_MTXID	(CFN_MAX_MTXID)
#define NUM_MTXID	(MAX_MTXID)
#define CHK_MTXID(id)	((MIN_MTXID) <= (id) && (id) <= (MAX_MTXID))
#define INDEX_MTX(id)	((id)-(MIN_MTXID))
#define ID_MTX(index)	((index)+(MIN_MTXID))

/* Message buffer configuration */
#define MIN_MBFID	(1)
#define MAX_MBFID	(CFN_MAX_MBFID)
#define NUM_MBFID	(MAX_MBFID)
#define CHK_MBFID(id)	((MIN_MBFID) <= (id) && (id) <= (MAX_MBFID))
#define INDEX_MBF(id)	((id)-(MIN_MBFID))
#define ID_MBF(index)	((index)+(MIN_MBFID))

/* Rendezvous configuration */
#define MIN_PORID	(1)
#define MAX_PORID	(CFN_MAX_PORID)
#define NUM_PORID	(MAX_PORID)
#define CHK_PORID(id)	((MIN_PORID) <= (id) && (id) <= (MAX_PORID))
#define INDEX_POR(id)	((id)-(MIN_PORID))
#define ID_POR(index)	((index)+(MIN_PORID))

/* Memory pool configuration */
#define MIN_MPLID	(1)
#define MAX_MPLID	(CFN_MAX_MPLID)
#define NUM_MPLID	(MAX_MPLID)
#define CHK_MPLID(id)	((MIN_MPLID) <= (id) && (id) <= (MAX_MPLID))
#define INDEX_MPL(id)	((id)-(MIN_MPLID))
#define ID_MPL(index)	((index)+(MIN_MPLID))

/* Fixed size memory pool configuration */
#define MIN_MPFID	(1)
#define MAX_MPFID	(CFN_MAX_MPFID)
#define NUM_MPFID	(MAX_MPFID)
#define CHK_MPFID(id)	((MIN_MPFID) <= (id) && (id) <= (MAX_MPFID))
#define INDEX_MPF(id)	((id)-(MIN_MPFID))
#define ID_MPF(index)	((index)+(MIN_MPFID))

/* Cyclic handler configuration */
#define MIN_CYCID	(1)
#define MAX_CYCID	(CFN_MAX_CYCID)
#define NUM_CYCID	(MAX_CYCID)
#define CHK_CYCID(id)	((MIN_CYCID) <= (id) && (id) <= (MAX_CYCID))
#define INDEX_CYC(id)	((id)-(MIN_CYCID))
#define ID_CYC(index)	((index)+(MIN_CYCID))

/* Alarm handler configuration */
#define MIN_ALMID	(1)
#define MAX_ALMID	(CFN_MAX_ALMID)
#define NUM_ALMID	(MAX_ALMID)
#define CHK_ALMID(id)	((MIN_ALMID) <= (id) && (id) <= (MAX_ALMID))
#define INDEX_ALM(id)	((id)-(MIN_ALMID))
#define ID_ALM(index)	((index)+(MIN_ALMID))

/* Subsystem manager configuration */
#define MIN_SSYID	(1)
#define MAX_SSYID	(CFN_MAX_SSYID)
#define NUM_SSYID	(MAX_SSYID)
#define CHK_SSYID(id)	((MIN_SSYID) <= (id) && (id) <= (MAX_SSYID))
#define INDEX_SSY(id)	((id)-(MIN_SSYID))
#define ID_SSY(index)	((index)+(MIN_SSYID))

/* Task priority configuration */
#define MIN_PRI		(1)	/* Minimum priority number = highest priority */
#define MAX_PRI		(32)	/* Maximum priority number = lowest priority */
#define NUM_PRI		(32)	/* Number of priority levels */
#define CHK_PRI(pri)	((MIN_PRI) <= (pri) && (pri) <= (MAX_PRI))

/* Subsystem manager configuration */
#define MIN_SSYPRI	(1)
#define MAX_SSYPRI	(CFN_MAX_SSYPRI)
#define NUM_SSYPRI	(MAX_SSYPRI)
#define CHK_SSYPRI(pri)	((MIN_SSYPRI) <= (pri) && (pri) <= (MAX_SSYPRI))


/*
 * Check parameter
 *   0: Do not check parameter
 *   1: Check parameter
 */
#define CHK_NOSPT	(1)	/* Check unsupported function (E_NOSPT) */
#define CHK_RSATR	(1)	/* Check reservation attribute error (E_RSATR) */
#define CHK_PAR		(1)	/* Check parameter (E_PAR) */
#define CHK_ID		(1)	/* Check object ID range (E_ID) */
#define CHK_OACV	(1)	/* Check Object Access Violation (E_OACV) */
#define CHK_CTX		(1)	/* Check whether task-independent part is running (E_CTX) */
#define CHK_CTX1	(1)	/* Check dispatch disable part */
#define CHK_CTX2	(1)	/* Check task independent part */
#define CHK_SELF	(1)	/* Check if its own task is specified (E_OBJ) */

/*
 * Debugger support function
 *   0: Invalid
 *   1: Valid
 */
#define USE_DBGSPT		(0)

/* Use object name (Add object name to each control block) */
#define USE_OBJECT_NAME		(0)	/* 0: Do not use object name */
					/* 1: Use object name */
#define OBJECT_NAME_LENGTH	(8)	/* Object name length in each control block */

/*
 * Output (error) messages from micro T-Kernel
 *   0: Do not output message
 *   1: Output message
 */
#define USE_KERNEL_MESSAGE	(1)



#endif /* _CONFIG_ */
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
 *	@(#)cpuattr.h (FM3)
 *
 *	Type Specific Function
 */

#ifndef __TK_CPUATTR_H__
#define __TK_CPUATTR_H__

/*
 * Global pointer support
 *   0: No global pointer support
 */
#define TA_GP		0		/* No global pointer support */

#endif /* __TK_CPUATTR_H__ */
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
 *	@(#)cpudef.h (FM3)
 *
 *	FM3 dependent definition
 */

#ifndef __TK_CPUDEF_H__
#define __TK_CPUDEF_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 * General purpose register			tk_get_reg tk_set_reg
 */
typedef struct t_regs {
	VW	r[13];		/* General purpose register R0-R12 */
	void	*lr;		/* Link register R14 */
} T_REGS;

/*
 * Exception-related register		tk_get_reg tk_set_reg
 */
typedef struct t_eit {
	void	*pc;		/* Program counter R15 */
	UW	xpsr;		/* Program status register */
	UW	taskmode;	/* Task mode flag */
} T_EIT;

/*
 * Control register			tk_get_reg tk_set_reg
 */
typedef struct t_cregs {
	void	*ssp;		/* System stack pointer R13_svc */
//	void	*usp;		/* User stack pointer R13_usr */
} T_CREGS;

#ifdef __cplusplus
}
#endif
#endif /* __TK_CPUDEF_H__ */
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
/*--------------------------------------------------------------------*/
/*  Include definition                                                */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Struct definition                                                 */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Constant definition                                               */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Variable definition                                               */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Macro definition                                                  */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Prototype declaration                                             */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Function definition                                               */
/*--------------------------------------------------------------------*/

#ifdef USE_FUNC_TK_DIS_DSP
/*
 * Dispatch enable/disable
 */
SYSCALL ER tk_dis_dsp_impl( void )
{
	CHECK_CTX(!in_loc());

	knl_dispatch_disabled = DDS_DISABLE;

	return E_OK;
}
#endif /* USE_FUNC_TK_DIS_DSP */

#ifdef USE_FUNC_TK_ENA_DSP
/*
 * Dispatch enable
 */
SYSCALL ER tk_ena_dsp_impl( void )
{
	CHECK_CTX(!in_loc());

	knl_dispatch_disabled = DDS_ENABLE;
	if ( knl_ctxtsk != knl_schedtsk ) {
		knl_dispatch();
	}

	return E_OK;
}
#endif /* USE_FUNC_TK_ENA_DSP */

/* ------------------------------------------------------------------------ */

/*
 * High level programming language
 */

#ifdef USE_FUNC_HLL_INTHDR
/* High level programming language interrupt handler entry */
Noinit(EXPORT FP knl_hll_inthdr[N_INTVEC]);
#endif /* USE_FUNC_HLL_INTHDR */

#ifdef USE_FUNC_TK_DEF_INT
IMPORT FP knl_hll_inthdr[];

/* High level programming language routine (Interrupt) */
IMPORT void knl_inthdr_startup();

/*
 * Interrupt handler definition
 */
SYSCALL ER tk_def_int_impl( UINT intno, CONST T_DINT *pk_dint )
{
	FP	inthdr;

	CHECK_PAR(intno < N_INTVEC);

	if ( pk_dint != NULL ) {
		/* Set interrupt handler */
		CHECK_RSATR(pk_dint->intatr, TA_HLNG);

		inthdr = pk_dint->inthdr;

		BEGIN_CRITICAL_SECTION;

#if USE_HLL_INTHDR
		if ( (pk_dint->intatr & TA_HLNG) != 0 ) {
			knl_hll_inthdr[intno] = inthdr;
			inthdr = knl_inthdr_startup;
		}
#endif

		knl_define_inthdr(intno, inthdr);
		END_CRITICAL_SECTION;
	} else {
		/* Clear interrupt handler */
		switch ( intno ) {
		  default:		inthdr = NULL;
		}

		BEGIN_CRITICAL_SECTION;
		knl_define_inthdr(intno, inthdr);
		END_CRITICAL_SECTION;
	}

	return E_OK;
}
#endif /* USE_FUNC_TK_DEF_INT */

/* ------------------------------------------------------------------------ */

#ifdef USE_FUNC_SET_REG
/*
 * Set task register contents
 */
EXPORT void knl_set_reg( TCB *tcb, CONST T_REGS *regs, CONST T_EIT *eit, CONST T_CREGS *cregs )
{
	SStackFrame	*ssp;
#if USE_TRAP
	UW	xpsr;
#endif
	INT	i;

	ssp = tcb->tskctxb.ssp;
#if USE_TRAP
	xpsr = ssp->xpsr;
#endif

	if ( cregs != NULL ) {
		ssp = cregs->ssp;
	}

	if ( regs != NULL ) {
		for ( i = 0; i < 4; ++i ) {
			ssp->r[i] = regs->r[i];
		}
		for ( i = 4; i < 12; ++i){
			ssp->r_[i - 4] = regs->r[i];
		}
#if USE_TRAP
		ssp->ip = regs->r[12];
		ssp->lr = regs->lr;
#endif
	}

	if ( eit != NULL ) {
		ssp->pc       = eit->pc;
#if USE_TRAP
		ssp->xpsr = (eit->xpsr & 0xff000000) | (xpsr & 0x00ffffff);
#endif
		ssp->taskmode = eit->taskmode;
	}

	if ( cregs != NULL ) {
		tcb->tskctxb.ssp  = cregs->ssp;

//		ssp->usp = cregs->usp;
	}
}
#endif /* USE_FUNC_SET_REG */

#ifdef USE_FUNC_TK_SET_REG
/*
 * Set task register contents
 */
SYSCALL ER tk_set_reg_impl( ID tskid,
		CONST T_REGS *pk_regs, CONST T_EIT *pk_eit, CONST T_CREGS *pk_cregs )
{
	TCB		*tcb;
	ER		ercd = E_OK;

	CHECK_INTSK();
	CHECK_TSKID(tskid);
	CHECK_NONSELF(tskid);

	tcb = get_tcb(tskid);

	BEGIN_CRITICAL_SECTION;
	if ( tcb->state == TS_NONEXIST ) {
		ercd = E_NOEXS;
	} else {
		knl_set_reg(tcb, pk_regs, pk_eit, pk_cregs);
	}
	END_CRITICAL_SECTION;

	return ercd;
}
#endif /* USE_FUNC_TK_SET_REG */

#ifdef USE_FUNC_GET_REG
/*
 * Get task register contents
 */
EXPORT void knl_get_reg( TCB *tcb, T_REGS *regs, T_EIT *eit, T_CREGS *cregs )
{
	SStackFrame	*ssp;
#if USE_TRAP
	UW		xpsr;
#endif
	INT		i;

	ssp = tcb->tskctxb.ssp;
#if USE_TRAP
	xpsr = ssp->xpsr;
#endif

	if ( regs != NULL ) {
		for ( i = 0; i < 4; ++i ) {
			regs->r[i] = ssp->r[i];
		}
		for ( i = 4; i < 12; ++i ){
			regs->r[i] = ssp->r_[i - 4];
		}
#if USE_TRAP
		regs->r[12] = ssp->ip;
		regs->lr = ssp->lr;
#endif
	}

	if ( eit != NULL ) {
		eit->pc       = ssp->pc;
#if USE_TRAP
		eit->xpsr     = ssp->xpsr;
#endif
		eit->taskmode = ssp->taskmode;
	}

	if ( cregs != NULL ) {
		cregs->ssp   = tcb->tskctxb.ssp;

//		cregs->usp = ssp->usp;
	}
}
#endif /* USE_FUNC_GET_REG */

#ifdef USE_FUNC_TK_GET_REG
/*
 * Get task register contents
 */
SYSCALL ER tk_get_reg_impl( ID tskid, T_REGS *pk_regs, T_EIT *pk_eit, T_CREGS *pk_cregs )
{
	TCB		*tcb;
	ER		ercd = E_OK;

	CHECK_INTSK();
	CHECK_TSKID(tskid);
	CHECK_NONSELF(tskid);

	tcb = get_tcb(tskid);

	BEGIN_CRITICAL_SECTION;
	if ( tcb->state == TS_NONEXIST ) {
		ercd = E_NOEXS;
	} else {
		knl_get_reg(tcb, pk_regs, pk_eit, pk_cregs);
	}
	END_CRITICAL_SECTION;

	return ercd;
}
#endif /* USE_FUNC_TK_GET_REG */

/* ------------------------------------------------------------------------ */
/*
 *	Debugger support function
 */
#if USE_DBGSPT

#ifdef USE_FUNC_TD_SET_REG
/*
 * Set task register
 */
SYSCALL ER td_set_reg_impl( ID tskid, CONST T_REGS *regs, CONST T_EIT *eit, CONST T_CREGS *cregs )
{
	TCB	*tcb;
	ER	ercd = E_OK;

	CHECK_TSKID(tskid);

	tcb = get_tcb(tskid);
	if ( tcb == knl_ctxtsk ) {
		return E_OBJ;
	}

	BEGIN_DISABLE_INTERRUPT;
	if ( tcb->state == TS_NONEXIST ) {
		ercd = E_NOEXS;
	} else {
		knl_set_reg(tcb, regs, eit, cregs);
	}
	END_DISABLE_INTERRUPT;

	return ercd;
}
#endif /* USE_FUNC_TD_SET_REG */

#ifdef USE_FUNC_TD_GET_REG
/*
 * Get task register
 */
SYSCALL ER td_get_reg_impl( ID tskid, T_REGS *regs, T_EIT *eit, T_CREGS *cregs )
{
	TCB	*tcb;
	ER	ercd = E_OK;

	CHECK_TSKID(tskid);

	tcb = get_tcb(tskid);
	if ( tcb == knl_ctxtsk ) {
		return E_OBJ;
	}

	BEGIN_DISABLE_INTERRUPT;
	if ( tcb->state == TS_NONEXIST ) {
		ercd = E_NOEXS;
	} else {
		knl_get_reg(tcb, regs, eit, cregs);
	}
	END_DISABLE_INTERRUPT;

	return ercd;
}
#endif /* USE_FUNC_TD_GET_REG */

#endif /* USE_DBGSPT */
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
 *	cpu_calls.h (FM3)
 *	CPU-Dependent System Call
 */

#ifndef _CPU_CALLS_H_
#define _CPU_CALLS_H_

IMPORT void knl_get_reg( TCB *tcb, T_REGS *regs, T_EIT *eit, T_CREGS *cregs );
IMPORT void knl_set_reg( TCB *tcb, CONST T_REGS *regs, CONST T_EIT *eit, CONST T_CREGS *cregs );

#endif
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
 *	cpu_conf.h (FM3)
 *	CPU-Dependent OS Configuration Information
 */

#ifndef _CPU_CONF_
#define _CPU_CONF_

/*
 * Definition of minimum system stack size
 *	Minimum system stack size when setting the system stack size
 *	per task by 'tk_cre_tsk().'
 *  this size must be larger than the size of SStackFrame
 */
#define MIN_SYS_STACK_SIZE	128


#endif /* _CPU_CONF_ */
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
/*--------------------------------------------------------------------*/
/*  Include definition                                                */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Struct definition                                                 */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Constant definition                                               */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Variable definition                                               */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Macro definition                                                  */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Prototype declaration                                             */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Function definition                                               */
/*--------------------------------------------------------------------*/

#include <tk_typedef.h>
#include <errno.h>
#include <typedef.h>

/*
 * CPU-dependent initialization
 */
EXPORT ER knl_cpu_initialize( void )
{
#if USE_TRAP
IMPORT void knl_dispatch_entry( void );		/* Dispatcher call */

	/* Register exception handler used on OS */
//	knl_define_inthdr(EXP_PSV, knl_dispatch_entry);
#if USE_DBGSPT
IMPORT void knl_call_dbgspt( void );		/* Debugger support call */
//	knl_define_inthdr(EXP_DBG, knl_call_dbgspt);
#endif
#endif /* USE_TRAP */

#if USE_TRAP || CFN_MAX_SSYID > 0
IMPORT void knl_call_entry( void );			/* System call */
//	knl_define_inthdr(EXP_SVC,      knl_call_entry);
#endif

	return E_OK;
}

#if USE_CLEANUP
/*
 * CPU-dependent finalization
 */
EXPORT void knl_cpu_shutdown( void )
{
}
#endif /* USE_CLEANUP */
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
 *	cpu_insn.h (FM3)
 *	LM3S9B92-Dependent Operation
 */

#ifndef _CPU_INSN_
#define _CPU_INSN_

#include <sysinfo.h>

/* ------------------------------------------------------------------------ */
/*
 *	Control register operation
 */

IMPORT UINT knl_getXPSR(void);
IMPORT UINT knl_getBASEPRI(void);
IMPORT UINT knl_getPRIMASK(void);

/* ------------------------------------------------------------------------ */
/*
 *	EIT-related
 */


/*
 * Set interrupt handler
 */
void knl_define_inthdr( INT vecno, FP inthdr )
{
	knl_intvec[vecno] = inthdr;
}

/*
 * If it is the task-independent part, TRUE
 */
BOOL knl_isTaskIndependent( void )
{
	return ( knl_taskindp > 0 )? TRUE: FALSE;
}

/*
 * Move to/Restore task independent part
 */
void knl_EnterTaskIndependent( void )
{
	knl_taskindp++;
}
void knl_LeaveTaskIndependent( void )
{
	knl_taskindp--;
}

/* ------------------------------------------------------------------------ */

#endif /* _CPU_INSN_ */
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
 *	cpu_status.h (FM3)
 *	FM3-Dependent Definition
 */

#ifndef _CPU_STATUS_
#define _CPU_STATUS_

#include <syslib.h>
#include <sysdef.h>
#include "cpu_insn.h"

/*
 * Start/End critical section
 */
#define BEGIN_CRITICAL_SECTION	{ UINT _basepri_ = disint();
#define END_CRITICAL_SECTION	if ( !isDI(_basepri_)			\
				  && knl_ctxtsk != knl_schedtsk		\
				  && !knl_dispatch_disabled ) {		\
					knl_dispatch();			\
				}					\
				enaint(_basepri_); }

/*
 * Start/End interrupt disable section
 */
#define BEGIN_DISABLE_INTERRUPT	{ UINT _basepri_ = disint();
#define END_DISABLE_INTERRUPT	enaint(_basepri_); }

/*
 * Interrupt enable/disable
 */
#define ENABLE_INTERRUPT	{ enaint(0); }
#define DISABLE_INTERRUPT	{ disint(); }

/*
 * Enable interrupt nesting
 *	Enable the interrupt that has a higher priority than 'level.'
 */
#define ENABLE_INTERRUPT_UPTO(level)	{ enaint(0); }

/*
 * Move to/Restore task independent part
 */
#define ENTER_TASK_INDEPENDENT	{ knl_EnterTaskIndependent(); }
#define LEAVE_TASK_INDEPENDENT	{ knl_LeaveTaskIndependent(); }

/* ----------------------------------------------------------------------- */
/*
 *	Check system state
 */

/*
 * When a system call is called from the task independent part, TRUE
 */
#define in_indp()	( knl_isTaskIndependent() || knl_ctxtsk == NULL )

/*
 * When a system call is called during dispatch disable, TRUE
 * Also include the task independent part as during dispatch disable.
 */
#define in_ddsp()	( knl_dispatch_disabled	\
			|| in_indp()		\
			|| isDI(knl_getBASEPRI()) )

/*
 * When a system call is called during CPU lock (interrupt disable), TRUE
 * Also include the task independent part as during CPU lock.
 */
#define in_loc()	( isDI(knl_getBASEPRI())		\
			|| in_indp() )

/*
 * When a system call is called during executing the quasi task part, TRUE
 * Valid only when in_indp() == FALSE because it is not discriminated from
 * the task independent part.
 */
#define in_qtsk()	( knl_ctxtsk->sysmode > knl_ctxtsk->isysmode )

/* ----------------------------------------------------------------------- */
/*
 *	Task dispatcher startup routine
 */

/*
 * Request for task dispatcher startup
 *	Do nothing at this point because there is no delayed
 *	interrupt function in ARM.
 *	Perform dispatcher startup with END_CRITICAL_SECTION.
 */
#define knl_dispatch_request()	/* */

IMPORT void knl_force_dispatch( void );
IMPORT void knl_dispatch( void );

/* ----------------------------------------------------------------------- */

/*
 * Task context block
 */
typedef struct {
	void	*ssp;		/* System stack pointer */
} CTXB;

#endif /* _CPU_STATUS_ */
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
 *	cpu_task.h (FM3)
 *	CPU-Dependent Task Start Processing
 */

#ifndef _CPU_TASK_
#define _CPU_TASK_

#include "cpu_insn.h"

/*
 * System stack configuration at task startup
 */
typedef struct {
	UW	r_[8];		/* R4-R11 */
	UW	taskmode;	/* Task mode */
	UW	exp_ret;	/* Exception return */
	/* Exception entry stack */
	UW	r[4];		/* R0-R3 */
	UW	ip;		/* R12 */
	void	*lr;		/* lr */
	void	*pc;		/* pc */
	UW	xpsr;		/* xpsr */
} SStackFrame;

/*
 * Size of system stack area destroyed by 'make_dormant()'
 * In other words, the size of area required to write by 'setup_context().'
 */
#define DORMANT_STACK_SIZE	( sizeof(VW) * 7 )	/* To 'taskmode' position */


/*
 * Create stack frame for task startup
 *	Call from 'make_dormant()'
 */
void knl_setup_context( TCB *tcb )
{
	SStackFrame	*ssp;
	UW		pc, xpsr;

	ssp = tcb->isstack;
	ssp--;

	xpsr = 0x01000000;
	pc = (UW)tcb->task;

	/* CPU context initialization */
	ssp->taskmode = 0;		/* Initial taskmode */
	ssp->exp_ret = 0xFFFFFFF9;
	ssp->lr = 0;
	ssp->xpsr = xpsr;		/* Initial SR */
	ssp->pc = (void *)(pc & ~0x00000001UL);	/* Task startup address */
	tcb->tskctxb.ssp = ssp;		/* System stack */
}

/*
 * Set task startup code
 *	Called by 'tk_sta_tsk()' processing.
 */
void knl_setup_stacd( TCB *tcb, INT stacd )
{
	SStackFrame	*ssp = tcb->tskctxb.ssp;

	ssp->r[0] = stacd;
	ssp->r[1] = (VW)tcb->exinf;
}

/*
 * Delete task contexts
 */
void knl_cleanup_context( TCB *tcb )
{
}

#endif /* _CPU_TASK_ */
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
 *	@(#)dbgspt.h
 *
 *	micro T-Kernel Debugger Support (Common parts)
 */

#ifndef __TK_DBGSPT_H__
#define __TK_DBGSPT_H__

#include <basic.h>
#include <tk_typedef.h>
#include <syscall.h>

#include <dbgspt_common.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Object name information		td_ref_dsname, td_set_dsname
 */
#define TN_TSK 0x01
#define TN_SEM 0x02
#define TN_FLG 0x03
#define TN_MBX 0x04
#define TN_MBF 0x05
#define TN_POR 0x06
#define TN_MTX 0x07
#define TN_MPL 0x08
#define TN_MPF 0x09
#define TN_CYC 0x0a
#define TN_ALM 0x0b

/*
 * Semaphore state information		td_ref_sem
 */
typedef	struct td_rsem {
	void	*exinf;		/* Extended information */
	ID	wtsk;		/* Wait task ID */
	INT	semcnt;		/* Current semaphore value */
} TD_RSEM;

/*
 * Event flag state information		td_ref_flg
 */
typedef	struct td_rflg {
	void	*exinf;		/* Extended information */
	ID	wtsk;		/* Wait task ID */
	UINT	flgptn;		/* Current event flag pattern */
} TD_RFLG;

/*
 * Mail box state information		td_ref_mbx
 */
typedef	struct td_rmbx {
	void	*exinf;		/* Extended information */
	ID	wtsk;		/* Wait task ID */
	T_MSG	*pk_msg;	/* Next received message */
} TD_RMBX;

/*
 * Mutex state information		td_ref_mtx
 */
typedef struct td_rmtx {
	void	*exinf;		/* Extended information */
	ID	htsk;		/* Locking task ID */
	ID	wtsk;		/* Lock wait task ID */
} TD_RMTX;

/*
 * Message buffer state information 	td_ref_mbf
 */
typedef struct td_rmbf {
	void	*exinf;		/* Extended information */
	ID	wtsk;		/* Receive wait task ID */
	ID	stsk;		/* Send wait task ID */
	INT	msgsz;		/* Next received message size (byte) */
	W	frbufsz;	/* Free buffer size (byte) */
	INT	maxmsz;		/* Maximum length of message (byte) */
} TD_RMBF;

/*
 * Rendezvous port state information	td_ref_por
 */
typedef struct td_rpor {
	void	*exinf;		/* Extended information */
	ID	wtsk;		/* Call wait task ID */
	ID	atsk;		/* Receive wait task ID */
	INT	maxcmsz;	/* Maximum length of call message (byte) */
	INT	maxrmsz;	/* Maximum length of replay message (byte) */
} TD_RPOR;

/*
 * Fixed size memory pool state information	td_ref_mpf
 */
typedef struct td_rmpf {
	void	*exinf;		/* Extended information */
	ID	wtsk;		/* Wait task ID */
	W	frbcnt;		/* Number of free blocks */
} TD_RMPF;

/*
 * Variable size memory pool state information	td_ref_mpl
 */
typedef struct td_rmpl {
	void	*exinf;		/* Extended information */
	ID	wtsk;		/* Wait task ID */
	W	frsz;		/* Total size of free area (byte) */
	W	maxsz;		/* Size of maximum continuous
				   free area (byte) */
} TD_RMPL;

/*
 * Cycle handler state information	td_ref_cyc
 */
typedef struct td_rcyc {
	void	*exinf;		/* Extended information */
	RELTIM	lfttim;		/* Remaining time until next handler startup */
	UINT	cycstat;	/* Cycle handler status */
} TD_RCYC;

/*
 * Alarm handler state information	td_ref_alm
 */
typedef struct td_ralm {
	void	*exinf;		/* Extended information */
	RELTIM	lfttim;		/* Remaining time until handler startup */
	UINT	almstat;	/* Alarm handler status */
} TD_RALM;

/*
 * Subsystem state information		td_ref_ssy
 */
typedef struct td_rssy {
	PRI	ssypri;		/* Subsystem priority */
	W	resblksz;	/* Resource management block size (byte) */
} TD_RSSY;

/*
 * Task state information		td_ref_tsk
 */
typedef	struct td_rtsk {
	void	*exinf;		/* Extended information */
	PRI	tskpri;		/* Current priority */
	PRI	tskbpri;	/* Base priority */
	UINT	tskstat;	/* Task state */
	UW	tskwait;	/* Wait factor */
	ID	wid;		/* Wait object ID */
	INT	wupcnt;		/* Number of wakeup requests queuing */
	INT	suscnt;		/* Number of SUSPEND request nests */
	FP	task;		/* Task startup address */
	W	stksz;		/* stack size (byte) */
	void	*istack;		/* stack pointer initial value */
} TD_RTSK;

/*
 * Task statistics information		td_inf_tsk
 */
typedef struct td_itsk {
	RELTIM	stime;		/* Cumulative system execution time
				   (milliseconds) */
	RELTIM	utime;		/* Cumulative user execution time
				   (milliseconds) */
} TD_ITSK;

/*
 * System state information		td_ref_sys
 */
typedef struct td_rsys {
	UINT	sysstat;	/* System state */
	ID	runtskid;	/* ID of task in execution state */
	ID	schedtskid;	/* ID of task that should be in
				   execution state */
} TD_RSYS;

/*
 * System call/extended SVC trace definition 	td_hok_svc
 */
typedef struct td_hsvc {
	FP	enter;		/* Hook routine before calling */
	FP	leave;		/* Hook routine after calling */
} TD_HSVC;

/*
 * Task dispatch trace definition		td_hok_dsp
 */
typedef struct td_hdsp {
	FP	exec;		/* Hook routine when starting execution */
	FP	stop;		/* Hook routine when stopping execution */
} TD_HDSP;

/*
 * Exception/Interrupt trace definition			td_hok_int
 */
typedef struct td_hint {
	FP	enter;		/* Hook routine before calling handler */
	FP	leave;		/* Hook routine after calling handler */
} TD_HINT;

/* ------------------------------------------------------------------------ */

/*
 * Definition for interface library automatic generation (mktdsvc)
 */
/*** DEFINE_TDSVC ***/

/* [BEGIN SYSCALLS] */

/* Refer each object usage state */
IMPORT INT td_lst_tsk( ID list[], INT nent );
IMPORT INT td_lst_sem( ID list[], INT nent );
IMPORT INT td_lst_flg( ID list[], INT nent );
IMPORT INT td_lst_mbx( ID list[], INT nent );
IMPORT INT td_lst_mtx( ID list[], INT nent );
IMPORT INT td_lst_mbf( ID list[], INT nent );
IMPORT INT td_lst_por( ID list[], INT nent );
IMPORT INT td_lst_mpf( ID list[], INT nent );
IMPORT INT td_lst_mpl( ID list[], INT nent );
IMPORT INT td_lst_cyc( ID list[], INT nent );
IMPORT INT td_lst_alm( ID list[], INT nent );
IMPORT INT td_lst_ssy( ID list[], INT nent );

/* Refer each object state */
IMPORT ER td_ref_sem( ID semid, TD_RSEM *rsem );
IMPORT ER td_ref_flg( ID flgid, TD_RFLG *rflg );
IMPORT ER td_ref_mbx( ID mbxid, TD_RMBX *rmbx );
IMPORT ER td_ref_mtx( ID mtxid, TD_RMTX *rmtx );
IMPORT ER td_ref_mbf( ID mbfid, TD_RMBF *rmbf );
IMPORT ER td_ref_por( ID porid, TD_RPOR *rpor );
IMPORT ER td_ref_mpf( ID mpfid, TD_RMPF *rmpf );
IMPORT ER td_ref_mpl( ID mplid, TD_RMPL *rmpl );
IMPORT ER td_ref_cyc( ID cycid, TD_RCYC *rcyc );
IMPORT ER td_ref_alm( ID almid, TD_RALM *ralm );
IMPORT ER td_ref_ssy( ID ssid, TD_RSSY *rssy );

/* Refer task state */
IMPORT ER td_ref_tsk( ID tskid, TD_RTSK *rtsk );
IMPORT ER td_inf_tsk( ID tskid, TD_ITSK *itsk, BOOL clr );
IMPORT ER td_get_reg( ID tskid, T_REGS *regs, T_EIT *eit, T_CREGS *cregs );
IMPORT ER td_set_reg( ID tskid, CONST T_REGS *regs, CONST T_EIT *eit, CONST T_CREGS *cregs );

/* Refer system state */
IMPORT ER td_ref_sys( TD_RSYS *rsys );
IMPORT ER td_get_tim( SYSTIM *tim, UW *ofs );
IMPORT ER td_get_otm( SYSTIM *tim, UW *ofs );

/* Refer ready queue */
IMPORT INT td_rdy_que( PRI pri, ID list[], INT nent );

/* Refer wait queue */
IMPORT INT td_sem_que( ID semid, ID list[], INT nent );
IMPORT INT td_flg_que( ID flgid, ID list[], INT nent );
IMPORT INT td_mbx_que( ID mbxid, ID list[], INT nent );
IMPORT INT td_mtx_que( ID mtxid, ID list[], INT nent );
IMPORT INT td_smbf_que( ID mbfid, ID list[], INT nent );
IMPORT INT td_rmbf_que( ID mbfid, ID list[], INT nent );
IMPORT INT td_cal_que( ID porid, ID list[], INT nent );
IMPORT INT td_acp_que( ID porid, ID list[], INT nent );
IMPORT INT td_mpf_que( ID mpfid, ID list[], INT nent );
IMPORT INT td_mpl_que( ID mplid, ID list[], INT nent );

/* Execution trace */
IMPORT ER td_hok_svc( CONST TD_HSVC *hsvc );
IMPORT ER td_hok_dsp( CONST TD_HDSP *hdsp );
IMPORT ER td_hok_int( CONST TD_HINT *hint );

/* Object name */
IMPORT ER td_ref_dsname( UINT type, ID id, UB *dsname );
IMPORT ER td_set_dsname( UINT type, ID id, CONST UB *dsname );

/* [END SYSCALLS] */

#ifdef __cplusplus
}
#endif
#endif /* __TK_DBGSPT_H__ */
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
 *	@(#)dbgspt_common.h (tk)
 *
 *	micro T-Kernel Debugger Support
 */

#ifndef __TK_DBGSPT_COMMON_H__
#define __TK_DBGSPT_COMMON_H__

/*
 * System-dependent definition
 */
// #if APP_AT91
// #include <dbgspt_depend.h>
// #endif

// #if APP_H8S2212
// #include <dbgspt_depend.h>
// #endif

// #if APP_MB9AF312K
// #include <dbgspt_depend.h>
// #endif

#if _APP_RL78G13_R5F100ADASP_
#include <dbgspt_depend.h>
#endif

#endif /* __TK_DBGSPT_COMMON_H__ */
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
 *	@(#)dbgspt_depend.h (FM3)
 *
 *	micro T-Kernel Debugger Support System-Dependent Definition
 */

#ifndef __TK_DBGSPT_DEPEND_H__
#define __TK_DBGSPT_DEPEND_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 * System call/extension SVC caller information 
 */
typedef struct td_calinf {
	void	*ssp;		/* System stack pointer */
	void	*r11;		/* Frame pointer when calling */
} TD_CALINF;

#ifdef __cplusplus
}
#endif
#endif /* __TK_DBGSPT_DEPEND_H__ */
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
 *	@(#)debug.h (sys)
 *
 *	Debug support
 */

#ifndef	__SYS_DEBUG_H__
#define __SYS_DEBUG_H__

#include <basic.h>

#ifdef DEBUG
#include <tmonitor.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Example
 *	DEBUG_PRINT(("error = %d\n", ercd));
 *
 *	DO_DEBUG( if ( ercd < E_OK ) DEBUG_PRINT(("error = %d\n", ercd)); )
 */
#ifdef DEBUG

#ifndef DEBUG_MODULE
#define DEBUG_MODULE	""	/* Normally define like "(module name)" */
#endif

/*
 * Example
 * #define DEBUG_PRINT(arg)						\
 * 	(								\
 * 		printf("%s#%d%s:", __FILE__, __LINE__, DEBUG_MODULE),	\
 * 		printf arg						\
 * 	)
 */

#define DEBUG_PRINT(arg)
#define DO_DEBUG(exp)	{ exp }

#else /* DEBUG */

#define DEBUG_PRINT(arg)	/* arg */
#define DO_DEBUG(exp)		/* exp */

#endif /* DEBUG */

#ifdef __cplusplus
}
#endif
#endif /* __SYS_DEBUG_H__ */
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
 *	device.c
 *	Device Management Function
 */

/** [BEGIN Common Definitions] */
#include "kernel.h"
#include "sysmgr.h"
#include "device.h"
/** [END Common Definitions] */

#if CFN_MAX_REGDEV > 0

#ifdef USE_FUNC_DEVMGRLOCK
/* Lock for device management exclusive control */
Noinit(EXPORT	FastMLock	knl_DevMgrLock);
#endif /* USE_FUNC_DEVMGRLOCK */

#ifdef USE_FUNC_DEFAULTIDEV
/* Device initial setting information */
Noinit(EXPORT	T_IDEV		knl_DefaultIDev);
#endif /* USE_FUNC_DEFAULTIDEV */

/* ------------------------------------------------------------------------ */
/*
 *	Device registration management
 */

#ifdef USE_FUNC_DEVCBTBL
Noinit(EXPORT	DevCB		knl_DevCBtbl[CFN_MAX_REGDEV]);	/* Device registration information table */
Noinit(EXPORT	QUEUE		knl_UsedDevCB);	/* In-use queue */
Noinit(EXPORT	QUEUE		knl_FreeDevCB);	/* Unused queue */
#endif /* USE_FUNC_DEVCBTBL */


#ifdef USE_FUNC_SEARCHDEVCB
/*
 * Search registration device
 */
EXPORT DevCB* knl_searchDevCB( CONST UB *devnm )
{
	QUEUE	*q;
	DevCB	*devcb;

	for ( q = knl_UsedDevCB.next; q != &knl_UsedDevCB; q = q->next ) {
		devcb = (DevCB*)q;

		if ( devcb->devnm[0] == devnm[0] && strcmp((char*)devcb->devnm, (char*)devnm) == 0 ) {
			return devcb; /* Found */
		}
	}

	return NULL;
}
#endif /* USE_FUNC_SEARCHDEVCB */

#ifdef USE_FUNC_TK_DEF_DEV
/*
 * Get DevCB for new registration
 */
LOCAL DevCB* newDevCB( CONST UB *devnm )
{
	DevCB	*devcb;

	devcb = (DevCB*)QueRemoveNext(&knl_FreeDevCB);
	if ( devcb == NULL ) {
		return NULL; /* No space */
	}

	strncpy((char*)devcb->devnm, (char*)devnm, L_DEVNM+1);
	QueInit(&devcb->openq);

	QueInsert(&devcb->q, &knl_UsedDevCB);

	return devcb;
}

/*
 * Free DevCB
 */
LOCAL void delDevCB( DevCB *devcb )
{
	QueRemove(&devcb->q);
	QueInsert(&devcb->q, &knl_FreeDevCB);
	devcb->devnm[0] = '\0';
}

/*
 * Device registration
 */
SYSCALL ID tk_def_dev_impl P3( CONST UB *devnm, CONST T_DDEV *pk_ddev, T_IDEV *pk_idev )
{
	DevCB	*devcb;
	INT	len;
	ER	ercd;

	LockREG();

	len = strlen((char*)devnm);
	if ( len == 0 || len > L_DEVNM ) {
		ercd = E_PAR;
		goto err_ret1;
	}

	if ( pk_ddev != NULL ) {
		if ( pk_ddev->nsub < 0 || pk_ddev->nsub > MAX_UNIT ) {
			ercd = E_PAR;
			goto err_ret1;
		}

		/* Make sure that the length of the logical device name
		   does not exceed the character limit */
		if ( pk_ddev->nsub > 0   ) {
			++len;
		}
		if ( pk_ddev->nsub > 10  ) {
			++len;
		}
		if ( pk_ddev->nsub > 100 ) {
			++len;
		}
		if ( len > L_DEVNM ) {
			ercd = E_PAR;
			goto err_ret1;
		}
	}

	LockDM();

	/* Search whether 'devnm' device is registered */
	devcb = knl_searchDevCB(devnm);
	if ( devcb == NULL ) {
		if ( pk_ddev == NULL ) {
			ercd = E_NOEXS;
			goto err_ret2;
		}

		/* Get 'devcb' for new registration because it is not
		   registered */
		devcb = newDevCB(devnm);
		if ( devcb == NULL ) {
			ercd = E_LIMIT;
			goto err_ret2;
		}
	}

	if ( pk_ddev != NULL ) {
		/* Set/update device registration information */
		devcb->ddev = *pk_ddev;
#if TA_GP
		if ( (pk_ddev->drvatr & TA_GP) == 0 ) {
			/* Apply caller 'gp' if TA_GP is not specified */
			devcb->ddev.gp = gp;
		}
#endif

		if ( pk_idev != NULL ) {
			/* Device initial setting information */
			*pk_idev = knl_DefaultIDev;
		}
	} else {
		if ( !isQueEmpty(&devcb->openq) ) {
			/* In use (open) */
			ercd = E_BUSY;
			goto err_ret2;
		}

		/* Device unregistration */
		delDevCB(devcb);
	}

	UnlockDM();
	UnlockREG();

	return DID(devcb);

err_ret2:
	UnlockDM();
err_ret1:
	UnlockREG();
	DEBUG_PRINT(("tk_def_dev_impl ercd = %d\n", ercd));
	return ercd;
}
#endif /* USE_FUNC_TK_DEF_DEV */

#ifdef USE_FUNC_TK_REF_IDV
/*
 * Check device initial information
 */
SYSCALL ER tk_ref_idv_impl( T_IDEV *pk_idev )
{
	LockDM();
	*pk_idev = knl_DefaultIDev;
	UnlockDM();

	return E_OK;
}
#endif /* USE_FUNC_TK_REF_IDV */

/* ------------------------------------------------------------------------ */

#ifdef USE_FUNC_PHYDEVNM
/*
 * Get physical device name
 *	Get the subunit number (return value) 
 *	from the logical device name (ldevnm) and the physical
 *	device name (pdevnm).
 */
EXPORT INT knl_phydevnm( UB *pdevnm, CONST UB *ldevnm )
{
	UB	c;
	INT	unitno;

	while ( (c = *ldevnm) != '\0' ) {
		if ( c >= '0' && c <= '9' ) {
			break;
		}
		*pdevnm++ = c;
		ldevnm++;
	}
	*pdevnm = '\0';

	unitno = 0;
	if (c != '\0') {
		while ( (c = *ldevnm) != '\0' ) {
			unitno = unitno * 10 + (c - '0');
			ldevnm++;
		}
		++unitno;
	}

	return unitno;
}
#endif /* USE_FUNC_PHYDEVNM */

#ifdef USE_FUNC_TK_GET_DEV
/*
 * Get logical device name
 *	Get the logical device name from
 *	the physical device name (pdevnm) and the subunit number (unitno).
 */
LOCAL void logdevnm( UB *ldevnm, UB *pdevnm, INT unitno )
{
	UB	unostr[12], *cp;

	strcpy((char*)ldevnm, (char*)pdevnm);
	if ( unitno > 0 ) {
		cp = &unostr[11];
		*cp = '\0';
		while (*ldevnm != '\0') {
			++ldevnm;
		}
		--unitno;
		do {
			*(--cp) = (UB)('0' + (unitno % 10));
			unitno /= 10;
		} while (unitno);
		strcat((char*)ldevnm, (char*)cp);
	}
}

/*
 * Get device name
 */
SYSCALL ID tk_get_dev_impl( ID devid, UB *devnm )
{
	DevCB	*devcb;
	ER	ercd;

	ercd = knl_check_devid(devid);
	if ( ercd < E_OK ) {
		goto err_ret1;
	}

	LockDM();

	devcb = DEVCB(devid);
	if ( (devcb->devnm[0] == '\0')||(UNITNO(devid) > devcb->ddev.nsub) ) {
		ercd = E_NOEXS;
		goto err_ret2;
	}

	logdevnm(devnm, devcb->devnm, UNITNO(devid));

	UnlockDM();

	return DID(devcb);

err_ret2:
	UnlockDM();
err_ret1:
	DEBUG_PRINT(("tk_get_dev_impl ercd = %d\n", ercd));
	return ercd;
}
#endif /* USE_FUNC_TK_GET_DEV */

#ifdef USE_FUNC_TK_REF_DEV
/*
 * Get device information
 */
SYSCALL ID tk_ref_dev_impl( CONST UB *devnm, T_RDEV *pk_rdev )
{
	UB	pdevnm[L_DEVNM + 1];
	DevCB	*devcb;
	INT	unitno;
	ER	ercd;

	unitno = knl_phydevnm(pdevnm, devnm);

	LockDM();

	devcb = knl_searchDevCB(pdevnm);
	if ( devcb == NULL || unitno > devcb->ddev.nsub ) {
		ercd = E_NOEXS;
		goto err_ret2;
	}

	if ( pk_rdev != NULL ) {
		pk_rdev->devatr = devcb->ddev.devatr;
		pk_rdev->blksz  = devcb->ddev.blksz;
		pk_rdev->nsub   = devcb->ddev.nsub;
		pk_rdev->subno  = unitno;
	}

	UnlockDM();

	return DEVID(devcb, unitno);

err_ret2:
	UnlockDM();
	DEBUG_PRINT(("tk_ref_dev_impl ercd = %d\n", ercd));
	return ercd;
}
#endif /* USE_FUNC_TK_REF_DEV */

#ifdef USE_FUNC_TK_OREF_DEV
/*
 * Get device information
 */
SYSCALL ID tk_oref_dev_impl( ID dd, T_RDEV *pk_rdev )
{
	OpnCB	*opncb;
	DevCB	*devcb;
	INT	unitno;
	ER	ercd;

	LockDM();

	ercd = knl_check_devdesc(dd, 0, &opncb);
	if ( ercd < E_OK ) {
		goto err_ret2;
	}

	devcb  = opncb->devcb;
	unitno = opncb->unitno;

	if ( pk_rdev != NULL ) {
		pk_rdev->devatr = devcb->ddev.devatr;
		pk_rdev->blksz  = devcb->ddev.blksz;
		pk_rdev->nsub   = devcb->ddev.nsub;
		pk_rdev->subno  = unitno;
	}

	UnlockDM();

	return DEVID(devcb, unitno);

err_ret2:
	UnlockDM();
	DEBUG_PRINT(("tk_oref_dev_impl ercd = %d\n", ercd));
	return ercd;
}
#endif /* USE_FUNC_TK_OREF_DEV */

#ifdef USE_FUNC_TK_LST_DEV
/*
 * Get registration device list
 */
SYSCALL INT tk_lst_dev_impl( T_LDEV *pk_ldev, INT start, INT ndev )
{
	DevCB	*devcb;
	QUEUE	*q;
	INT	n, end;
	ER	ercd;

	if ( start < 0 || ndev < 0 ) {
		ercd = E_PAR;
		goto err_ret;
	}
	LockDM();

	end = start + ndev;
	n = 0;
	for ( q = knl_UsedDevCB.next; q != &knl_UsedDevCB; q = q->next ) {
		if ( n >= start && n < end ) {
			devcb = (DevCB*)q;
			pk_ldev->devatr = devcb->ddev.devatr;
			pk_ldev->blksz  = devcb->ddev.blksz;
			pk_ldev->nsub   = devcb->ddev.nsub;
			strncpy((char*)pk_ldev->devnm, (char*)devcb->devnm, L_DEVNM);
			pk_ldev++;
		}
		n++;
	}

	UnlockDM();

	if ( start >= n ) {
		ercd = E_NOEXS;
		goto err_ret;
	}

	return n - start;

err_ret:
	DEBUG_PRINT(("tk_lst_dev_impl ercd = %d\n", ercd));
	return ercd;
}
#endif /* USE_FUNC_TK_LST_DEV */

/* ------------------------------------------------------------------------ */

#ifdef USE_FUNC_TK_EVT_DEV
/*
 * Send driver request event
 */
SYSCALL INT tk_evt_dev_impl( ID devid, INT evttyp, void *evtinf )
{
	DevCB	*devcb;
	EVTFN	eventfn;
	void	*exinf;
#if TA_GP
	void	*gp;
#endif
	ER	ercd;

	ercd = knl_check_devid(devid);
	if ( ercd < E_OK ) {
		goto err_ret1;
	}
	if ( evttyp < 0 ) {
		ercd = E_PAR;
		goto err_ret1;
	}

	LockDM();

	devcb = DEVCB(devid);
	if ( (devcb->devnm[0] == '\0')||(UNITNO(devid) > devcb->ddev.nsub) ) {
		ercd = E_NOEXS;
		goto err_ret2;
	}

	eventfn = (EVTFN)devcb->ddev.eventfn;
	exinf = devcb->ddev.exinf;
#if TA_GP
	gp = devcb->ddev.gp;
#endif

	UnlockDM();

	/* Device driver call */
	DISABLE_INTERRUPT;
	knl_ctxtsk->sysmode++;
	ENABLE_INTERRUPT;
#if TA_GP
	ercd = CallDeviceDriver(evttyp, evtinf, exinf, 0, (FP)eventfn, gp);
#else
	ercd = (*eventfn)(evttyp, evtinf, exinf);
#endif
	DISABLE_INTERRUPT;
	knl_ctxtsk->sysmode--;
	ENABLE_INTERRUPT;

	return ercd;

err_ret2:
	UnlockDM();
err_ret1:
	DEBUG_PRINT(("tk_evt_dev_impl ercd = %d\n", ercd));
	return ercd;
}
#endif /* USE_FUNC_TK_EVT_DEV */

/* ------------------------------------------------------------------------ */

#ifdef USE_FUNC_INITIALIZE_DEVMGR
/*
 * Initialization of device registration information table
 */
LOCAL ER initDevCB( void )
{
	DevCB	*devcb;
	INT	num = CFN_MAX_REGDEV;

	QueInit(&knl_UsedDevCB);
	QueInit(&knl_FreeDevCB);

	devcb = knl_DevCBtbl;
	while ( num-- > 0 ) {
		QueInsert(&devcb->q, &knl_FreeDevCB);
		devcb->devnm[0] = '\0';
		devcb++;
	}

	return E_OK;
}

/*
 * Initialization of device initial setting information
 */
LOCAL ER initIDev( void )
{
#if CFN_DEVT_MBFSZ0 >= 0
	T_CMBF	cmbf;
	ER	ercd;

	/* Generate message buffer for event notification */
	strncpy((char*)&cmbf.exinf, (char*)OBJNAME_DMMBF, sizeof(cmbf.exinf));
	cmbf.mbfatr = TA_TFIFO;
	cmbf.bufsz  = CFN_DEVT_MBFSZ0;
	cmbf.maxmsz = CFN_DEVT_MBFSZ1;
	ercd = tk_cre_mbf_impl(&cmbf);
	if ( ercd < E_OK ) {
		knl_DefaultIDev.evtmbfid = 0;
		goto err_ret;
	}
	knl_DefaultIDev.evtmbfid = ercd;

	return E_OK;

err_ret:
	DEBUG_PRINT(("initIDev ercd = %d\n", ercd));
	return ercd;
#else
	/* Do not use message buffer for event notification */
	knl_DefaultIDev.evtmbfid = 0;
	return E_OK;
#endif
}

/*
 * Initialization of system management
 */
EXPORT ER knl_initialize_devmgr( void )
{
	ER	ercd;

	/* Generate lock for device management exclusive control */
	ercd = CreateMLock(&knl_DevMgrLock, (UB*)OBJNAME_DMLOCK);
	if ( ercd < E_OK ) {
		goto err_ret;
	}

	/* Generate device registration information table */
	ercd = initDevCB();
	if ( ercd < E_OK ) {
		goto err_ret;
	}

	/* Initialization of device input/output-related */
	ercd = knl_initDevIO();
	if ( ercd < E_OK ) {
		goto err_ret;
	}

	/* Initialization of device initial setting information */
	ercd = initIDev();
	if ( ercd < E_OK ) {
		goto err_ret;
	}

	knl_devmgr_startup();

	return E_OK;

err_ret:
	DEBUG_PRINT(("initialize_devmgr ercd = %d\n", ercd));
	knl_finish_devmgr();
	return ercd;
}
#endif /* USE_FUNC_INITIALIZE_DEVMGR */

#ifdef USE_FUNC_FINISH_DEVMGR
/*
 * Unregister device initial setting information
 */
LOCAL ER delIDev( void )
{
	ER	ercd = E_OK;

#if CFN_DEVT_MBFSZ0 >= 0
	/* Delete message buffer for event notification */
	if ( knl_DefaultIDev.evtmbfid > 0 ) {
		ercd = tk_del_mbf_impl(knl_DefaultIDev.evtmbfid);
		knl_DefaultIDev.evtmbfid = 0;
	}

#ifdef DEBUG
	if ( ercd < E_OK ) {
		DEBUG_PRINT(("delIDev ercd = %d\n", ercd));
	}
#endif

#endif /* CFN_DEVT_MBFSZ0 >= 0 */

	return ercd;
}

/*
 * Finalization sequence of system management 
 */
EXPORT ER knl_finish_devmgr( void )
{
	ER	ercd;

	knl_devmgr_cleanup();

	/* Unregister device initial setting information */
	ercd = delIDev();
#ifdef DEBUG
	if ( ercd < E_OK ) {
		DEBUG_PRINT(("2. finish_devmgr -> delIDev ercd = %d\n", ercd));
	}
#endif

	/* Finalization sequence of device input/output-related */
	ercd = knl_finishDevIO();
#ifdef DEBUG
	if ( ercd < E_OK ) {
		DEBUG_PRINT(("3. finish_devmgr -> finishDevIO ercd = %d\n", ercd));
	}
#endif

	/* Delete lock for device management exclusive control */
	DeleteMLock(&knl_DevMgrLock);

	return ercd;
}
#endif /* USE_FUNC_FINISH_DEVMGR */

#endif /* CFN_MAX_REGDEV */
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
 *	device.h
 *	Device Management Function
 */

#ifndef _DEVICE_H_
#define _DEVICE_H_

/* Set Object Name in .exinf for DEBUG */
#define OBJNAME_DMMBF	"DEvt"		/* Event notification mbf */
#define OBJNAME_DMSEM	"DMSy"		/* semaphore of synchronous control */
#define OBJNAME_DMLOCK	"DMLk"		/* Multi-lock for Dev.Mgr. */

IMPORT	T_IDEV		knl_DefaultIDev;

#include "task.h"
/*
 *	Device registration management
 */

IMPORT DevCB knl_DevCBtbl[];	/* Device registration information table */
IMPORT QUEUE knl_UsedDevCB;	/* In-use queue */
IMPORT QUEUE knl_FreeDevCB;	/* Unused queue */

#define MAX_UNIT	255		/* Maximum number of subunits */

/*
 * Verify validity of device ID
 */
ER knl_check_devid( ID devid )
{
	devid >>= 8;
	if ( devid < 1 || devid > CFN_MAX_REGDEV ) {
		return E_ID;
	}
	return E_OK;
}

/*
 * Device Management: Input/Output
 */
IMPORT OpnCB knl_OpnCBtbl[];	/* Open management information table */
IMPORT QUEUE knl_FreeOpnCB;	/* Unused queue */

#define DD(opncb)		( (opncb) - knl_OpnCBtbl + 1 )
#define OPNCB(dd)		( knl_OpnCBtbl + ((dd) - 1) )

IMPORT ReqCB knl_ReqCBtbl[];	/* Request management information table */
IMPORT QUEUE knl_FreeReqCB;	/* Unused queue */

#define REQID(reqcb)		( (reqcb) - knl_ReqCBtbl + 1 )
#define REQCB(reqid)		( knl_ReqCBtbl + ((reqid) - 1) )

#define DEVREQ_REQCB(devreq)	((ReqCB*)((B*)(devreq) - offsetof(ReqCB, req)))

IMPORT ResCB knl_resource_control_block;


#include "limits.h"

/* Suspend disable request count */
IMPORT	INT	knl_DisSusCnt;

/* Maximum number of suspend disable request counts */
#define MAX_DISSUS	INT_MAX


/*
 * Device driver abort function call
 */
ER knl_call_abortfn( DevCB *devcb, ID tskid, T_DEVREQ *devreq, INT nreq )
{
	ER ercd;
	ABTFN	abortfn;

	abortfn = (ABTFN)devcb->ddev.abortfn;

	DISABLE_INTERRUPT;
	knl_ctxtsk->sysmode++;
	ENABLE_INTERRUPT;
#if TA_GP
	ercd = knl_CallDeviceDriver(tskid, devreq, nreq, devcb->ddev.exinf,
						(FP)abortfn, devcb->ddev.gp);
#else
	ercd = (*abortfn)(tskid, devreq, nreq, devcb->ddev.exinf);
#endif
	DISABLE_INTERRUPT;
	knl_ctxtsk->sysmode--;
	ENABLE_INTERRUPT;

	return ercd;
}


IMPORT ID knl_request( ID dd, W start, void *buf, W size, TMO tmout, INT cmd );
IMPORT BOOL knl_chkopen( DevCB *devcb, INT unitno );
IMPORT void knl_delReqCB( ReqCB *reqcb );
IMPORT ResCB* knl_GetResCB( void );
IMPORT void knl_delOpnCB( OpnCB *opncb, BOOL free );
IMPORT ER knl_close_device( OpnCB *opncb, UINT option );

#endif /* _DEVICE_H_ */
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
 *	deviceio.c
 *	Device Management Function: Input/Output
 */

/** [BEGIN Common Definitions] */
#include "kernel.h"
#include "sysmgr.h"
#include "device.h"
/** [END Common Definitions] */

#if CFN_MAX_REGDEV

#ifdef USE_FUNC_OPNCBTBL
Noinit(EXPORT OpnCB	knl_OpnCBtbl[CFN_MAX_OPNDEV]);	/* Open management information table */
Noinit(EXPORT QUEUE	knl_FreeOpnCB);	/* Unused queue */
#endif /* USE_FUNC_OPNCBTBL */

#ifdef USE_FUNC_REQCBTBL
Noinit(EXPORT ReqCB	knl_ReqCBtbl[CFN_MAX_REQDEV]);	/* Request management information table */
Noinit(EXPORT QUEUE	knl_FreeReqCB);	/* Unused queue */
#endif /* USE_FUNC_REQCBTBL */

#ifdef USE_FUNC_RESOURCE_CONTROL_BLOCK
Noinit(EXPORT ResCB knl_resource_control_block);
#endif /* USE_FUNC_RESOURCE_CONTROL_BLOCK */


#ifdef USE_FUNC_GETRESCB
/*
 * Get resource management information
 */
EXPORT ResCB* knl_GetResCB( void )
{
	LockDM();

	/* If the startup function is not called, initialize at this point */
	if ( knl_resource_control_block.openq.next == NULL ) {
		/* Initialization of open device management queue */
		QueInit(&(knl_resource_control_block.openq));
	}

	UnlockDM();

	return &knl_resource_control_block;
}
#endif /* USE_FUNC_GETRESCB */

#ifdef USE_FUNC_CHECK_DEVDESC
/*
 * Verify validity of device descriptor
 */
EXPORT ER knl_check_devdesc( ID dd, UINT mode, OpnCB **p_opncb )
{
	OpnCB	*opncb;

	if ( dd < 1 || dd > CFN_MAX_OPNDEV ) {
		return E_ID;
	}
	opncb = OPNCB(dd);
	if ( opncb->resid == 0 ) {
		return E_ID;
	}

	if ( mode != 0 ) {
		if ( (opncb->omode & mode) == 0 ) {
			return E_OACV;
		}
	}

	*p_opncb = opncb;
	return E_OK;
}
#endif /* USE_FUNC_CHECK_DEVDESC */

#ifdef USE_FUNC_DELOPNCB
/*
 * Free open management block
 */
EXPORT void knl_delOpnCB( OpnCB *opncb, BOOL free )
{
	QueRemove(&opncb->q);
	QueRemove(&opncb->resq);

	if ( free ) {
		QueInsert(&opncb->q, &knl_FreeOpnCB);
	}
	opncb->resid = 0;
}
#endif /* USE_FUNC_DELOPNCB */

#ifdef USE_FUNC_DELREQCB
/*
 * Free request management block
 */
EXPORT void knl_delReqCB( ReqCB *reqcb )
{
	QueRemove(&reqcb->q);

	QueInsert(&reqcb->q, &knl_FreeReqCB);
	reqcb->opncb = NULL;
}
#endif /* USE_FUNC_DELREQCB */

/* ------------------------------------------------------------------------ */

#ifdef USE_FUNC_CHKOPEN
/*
 * TRUE if specified device is open.
 */
EXPORT BOOL knl_chkopen( DevCB *devcb, INT unitno )
{
	QUEUE	*q;

	for ( q = devcb->openq.next; q != &devcb->openq; q = q->next ) {
		if ( ((OpnCB*)q)->unitno == unitno ) {
			return TRUE;
		}
	}
	return FALSE;
}
#endif /* USE_FUNC_CHKOPEN */

#ifdef USE_FUNC_TK_OPN_DEV

LOCAL CONST T_CSEM knl_pk_csem_DM = {
	NULL,
	TA_TFIFO | TA_FIRST,
	0,
	1,
};

/*
 * Get open management block
 */
LOCAL OpnCB* newOpnCB( DevCB *devcb, INT unitno, UINT omode, ResCB *rescb )
{
	OpnCB	*opncb;

	/* Get space in open management block */
	opncb = (OpnCB*)QueRemoveNext(&knl_FreeOpnCB);
	if ( opncb == NULL ) {
		return NULL; /* No space */
	}

	/* Register as open device */
	QueInsert(&opncb->q, &devcb->openq);
	QueInsert(&opncb->resq, &rescb->openq);

	opncb->devcb  = devcb;
	opncb->unitno = unitno;
	opncb->omode  = omode;
	QueInit(&opncb->requestq);
	opncb->waitone = 0;
	opncb->nwaireq = 0;
	opncb->abort_tskid = 0;

	opncb->resid  = 0; /* Indicate that open processing is not completed */

	return opncb;
}

/*
 * Check open mode
 */
LOCAL ER chkopenmode( DevCB *devcb, INT unitno, UINT omode )
{
	QUEUE	*q;
	OpnCB	*opncb;
	INT	read, write, rexcl, wexcl;

	if ( (omode & TD_UPDATE) == 0 ) {
		return E_PAR;
	}

	/* Check current open state */
	read = write = rexcl = wexcl = 0;
	for ( q = devcb->openq.next; q != &devcb->openq; q = q->next ) {
		opncb = (OpnCB*)q;

		if ( unitno == 0 || opncb->unitno == 0 || opncb->unitno == unitno ) {
			if ( (opncb->omode & TD_READ)  != 0 ) {
				read++;
			}
			if ( (opncb->omode & TD_WRITE) != 0 ) {
				write++;
			}
			if ( (opncb->omode & (TD_EXCL|TD_REXCL)) != 0) {
				rexcl++;
			}
			if ( (opncb->omode & (TD_EXCL|TD_WEXCL)) != 0) {
				wexcl++;
			}
		}
	}

	/* Is it able to open? */
	if ( (omode & (TD_EXCL|TD_REXCL)) != 0 && read  > 0 ) {
		return E_BUSY;
	}
	if ( (omode & (TD_EXCL|TD_WEXCL)) != 0 && write > 0 ) {
		return E_BUSY;
	}
	if ( (omode & TD_READ)  != 0 && rexcl > 0 ) {
		return E_BUSY;
	}
	if ( (omode & TD_WRITE) != 0 && wexcl > 0 ) {
		return E_BUSY;
	}

	return E_OK;
}

/*
 * Device open
 */
SYSCALL ID tk_opn_dev_impl( CONST UB *devnm, UINT omode )
{
	OPNFN	openfn;
	void	*exinf;
#if TA_GP
	void	*gp;
#endif
	UB	pdevnm[L_DEVNM + 1];
	INT	unitno;
	ResCB	*rescb;
	DevCB	*devcb;
	OpnCB	*opncb;
	ER	ercd;
	ID	semid;

	unitno = knl_phydevnm(pdevnm, devnm);

	/* Get resource management information */
	rescb = knl_GetResCB();
	if ( rescb == NULL ) {
		ercd = E_CTX;
		goto err_ret1;
	}

	LockDM();

	/* Search device to open */
	devcb = knl_searchDevCB(pdevnm);
	if ( devcb == NULL || unitno > devcb->ddev.nsub ) {
		ercd = E_NOEXS;
		goto err_ret2;
	}

	/* Check open mode */
	ercd = chkopenmode(devcb, unitno, omode);
	if ( ercd < E_OK ) {
		goto err_ret2;
	}

	openfn = (OPNFN)devcb->ddev.openfn;
	exinf = devcb->ddev.exinf;
#if TA_GP
	gp = devcb->ddev.gp;
#endif

	/* Is device driver call required? */
	if ( knl_chkopen(devcb, unitno) && (devcb->ddev.drvatr & TDA_OPENREQ) == 0 ) {
		openfn = NULL;
	}

	/* Get open management block */
	opncb = newOpnCB(devcb, unitno, omode, rescb);
	if ( opncb == NULL ) {
		ercd = E_LIMIT;
		goto err_ret2;
	}

	semid = tk_cre_sem_impl(&knl_pk_csem_DM);
	if ( semid < E_OK ) {
		ercd = E_SYS;
		goto err_ret2_5;
	}
	opncb->abort_semid = semid;

	UnlockDM();

	if ( openfn != NULL ) {
		/* Device driver call */
		DISABLE_INTERRUPT;
		knl_ctxtsk->sysmode++;
		ENABLE_INTERRUPT;
#if TA_GP
		ercd = CallDeviceDriver(DEVID(devcb, unitno), omode, exinf, 0,
								(FP)openfn, gp);
#else
		ercd = (*openfn)(DEVID(devcb, unitno), omode, exinf);
#endif
		DISABLE_INTERRUPT;
		knl_ctxtsk->sysmode--;
		ENABLE_INTERRUPT;

		if ( ercd < E_OK ) {
			goto err_ret3;
		}
	}

	LockDM();
	opncb->resid = 1; /* Indicate that open processing is completed */
	UnlockDM();

	return DD(opncb);

err_ret3:
	LockDM();
	tk_del_sem_impl(opncb->abort_semid);
err_ret2_5:
	knl_delOpnCB(opncb, TRUE);
err_ret2:
	UnlockDM();
err_ret1:
	DEBUG_PRINT(("tk_opn_dev_impl ercd = %d\n", ercd));
	return ercd;
}
#endif /* USE_FUNC_TK_OPN_DEV */

#ifdef USE_FUNC_CLOSE_DEVICE
/*
 * Abort all requests
 */
LOCAL void abort_allrequest( OpnCB *opncb )
{
	ABTFN	abortfn;
	WAIFN	waitfn;
	void	*exinf;
#if TA_GP
	void	*gp;
#endif
	DevCB	*devcb;
	ReqCB	*reqcb;
	QUEUE	*q;

	/* If 'execfn' and 'waitfn' are called, execute abort request. */
	LockDM();

	devcb = opncb->devcb;
	abortfn = (ABTFN)devcb->ddev.abortfn;
	waitfn  = (WAIFN)devcb->ddev.waitfn;
	exinf   = devcb->ddev.exinf;
#if TA_GP
	gp = devcb->ddev.gp;
#endif

	opncb->abort_tskid = tk_get_tid_impl();
	opncb->abort_cnt = 0;

	if ( opncb->nwaireq > 0 ) {
		/* Multiple requests wait */
		reqcb = DEVREQ_REQCB(opncb->waireqlst);

		/* Device driver call */
		DISABLE_INTERRUPT;
		knl_ctxtsk->sysmode++;
		ENABLE_INTERRUPT;
#if TA_GP
		CallDeviceDriver(reqcb->tskid, opncb->waireqlst,
					opncb->nwaireq, exinf, (FP)abortfn, gp);
#else
		(*abortfn)(reqcb->tskid, opncb->waireqlst, opncb->nwaireq,
								exinf);
#endif
		DISABLE_INTERRUPT;
		knl_ctxtsk->sysmode--;
		ENABLE_INTERRUPT;

		opncb->abort_cnt++;
	} else {
		/* Start request or single request wait */
		for ( q = opncb->requestq.next; q != &opncb->requestq; q = q->next ) {
			reqcb = (ReqCB*)q;
			if ( reqcb->tskid == 0 ) {
				continue;
			}

			reqcb->req.abort = TRUE;

			/* Device driver call */
			DISABLE_INTERRUPT;
			knl_ctxtsk->sysmode++;
			ENABLE_INTERRUPT;
#if TA_GP
			CallDeviceDriver(reqcb->tskid, &reqcb->req, 1, exinf,
								(FP)abortfn, gp);
#else
			(*abortfn)(reqcb->tskid, &reqcb->req, 1, exinf);
#endif
			DISABLE_INTERRUPT;
			knl_ctxtsk->sysmode--;
			ENABLE_INTERRUPT;

			opncb->abort_cnt++;
		}
	}

	UnlockDM();

	if ( opncb->abort_cnt > 0 ) {
		/* Wait for completion of abort request processing */
		tk_wai_sem_impl(opncb->abort_semid, 1, TMO_FEVR);
	}
	opncb->abort_tskid = 0;

	/* Abort remaining requests and wait for completion */
	LockDM();
	while ( !isQueEmpty(&opncb->requestq) ) {
		reqcb = (ReqCB*)opncb->requestq.next;
		reqcb->req.abort = TRUE;

		UnlockDM();

		/* Device driver call */
		DISABLE_INTERRUPT;
		knl_ctxtsk->sysmode++;
		ENABLE_INTERRUPT;
#if TA_GP
		CallDeviceDriver(&reqcb->req, 1, TMO_FEVR, exinf, (FP)waitfn, gp);
#else
		(*waitfn)(&reqcb->req, 1, TMO_FEVR, exinf);
#endif
		DISABLE_INTERRUPT;
		knl_ctxtsk->sysmode--;
		ENABLE_INTERRUPT;

		LockDM();

		/* Unregister completed request */
		knl_delReqCB(reqcb);
	}
	UnlockDM();
}

/*
 * Device close processing
 */
EXPORT ER knl_close_device( OpnCB *opncb, UINT option )
{
	CLSFN	closefn;
	void	*exinf;
#if TA_GP
	void	*gp;
#endif
	ID	devid;
	DevCB	*devcb;
	INT	unitno;
	ER	ercd = E_OK;

	/* Abort all requests during processing */
	abort_allrequest(opncb);

	LockDM();

	devcb  = opncb->devcb;
	unitno = opncb->unitno;
	closefn = (CLSFN)devcb->ddev.closefn;
	exinf = devcb->ddev.exinf;
#if TA_GP
	gp = devcb->ddev.gp;
#endif
	devid = DEVID(devcb, unitno);

	/* Delete semaphore for completion check of abortion */
	tk_del_sem_impl(opncb->abort_semid);

	/* Free open management block */
	knl_delOpnCB(opncb, FALSE);

	/* Is device driver call required? */
	if ( knl_chkopen(devcb, unitno) ) {
		option &= ~TD_EJECT;
		if ( (devcb->ddev.drvatr & TDA_OPENREQ) == 0 ) {
			closefn = NULL;
		}
	}

	UnlockDM();

	if ( closefn != NULL ) {
		/* Device driver call */
		DISABLE_INTERRUPT;
		knl_ctxtsk->sysmode++;
		ENABLE_INTERRUPT;
#if TA_GP
		ercd = CallDeviceDriver(devid, option, exinf, 0, (FP)closefn, gp);
#else
		ercd = (*closefn)(devid, option, exinf);
#endif
		DISABLE_INTERRUPT;
		knl_ctxtsk->sysmode--;
		ENABLE_INTERRUPT;
	}

	LockDM();
	/* Return open management block to FreeQue */
	QueInsert(&opncb->q, &knl_FreeOpnCB);
	UnlockDM();

#ifdef DEBUG
	if ( ercd < E_OK ) {
		DEBUG_PRINT(("knl_close_device ercd = %d\n", ercd));
	}
#endif
	return ercd;
}
#endif /* USE_FUNC_CLOSE_DEVICE */

#ifdef USE_FUNC_TK_CLS_DEV
/*
 * Device close
 */
SYSCALL ER tk_cls_dev_impl( ID dd, UINT option )
{
	OpnCB	*opncb;
	ER	ercd;

	LockDM();

	ercd = knl_check_devdesc(dd, 0, &opncb);
	if ( ercd < E_OK ) {
		UnlockDM();
		goto err_ret;
	}

	opncb->resid = 0; /* Indicate that it is during close processing */

	UnlockDM();

	/* Device close processing */
	ercd = knl_close_device(opncb, option);

err_ret:
#ifdef DEBUG
	if ( ercd < E_OK ) {
		DEBUG_PRINT(("tk_cls_dev_impl ercd = %d\n", ercd));
	}
#endif
	return ercd;
}
#endif /* USE_FUNC_TK_CLS_DEV */

/* ------------------------------------------------------------------------ */

#ifdef USE_FUNC_REQUEST
/*
 * Get request management block
 */
LOCAL ReqCB* newReqCB( OpnCB *opncb )
{
	ReqCB	*reqcb;

	/* Get space in request management block */
	reqcb = (ReqCB*)QueRemoveNext(&knl_FreeReqCB);
	if ( reqcb == NULL ) {
		return NULL; /* No space */
	}

	/* Register as requested open device */
	QueInsert(&reqcb->q, &opncb->requestq);

	reqcb->opncb = opncb;

	return reqcb;
}

/*
 * Request for starting input/output to device
 */
EXPORT ID knl_request( ID dd, W start, void *buf, W size, TMO tmout, INT cmd )
{
	EXCFN	execfn;
	void	*exinf;
#if TA_GP
	void	*gp;
#endif
	OpnCB	*opncb;
	DevCB	*devcb;
	ReqCB	*reqcb;
	UINT	m;
	ER	ercd;

	LockDM();

	if ( start <= -0x00010000 && start >= -0x7fffffff ) {
		m = 0; /* Ignore open mode */
	} else {
		m = ( cmd == TDC_READ )? TD_READ: TD_WRITE;
	}
	ercd = knl_check_devdesc(dd, m, &opncb);
	if ( ercd < E_OK ) {
		goto err_ret1;
	}

	devcb = opncb->devcb;
	execfn = (EXCFN)devcb->ddev.execfn;
	exinf = devcb->ddev.exinf;
#if TA_GP
	gp = devcb->ddev.gp;
#endif

	/* Get request management block */
	reqcb = newReqCB(opncb);
	if ( reqcb == NULL ) {
		ercd = E_LIMIT;
		goto err_ret1;
	}

	/* Set request packet */
	reqcb->req.next   = NULL;
	reqcb->req.exinf  = NULL;
	reqcb->req.devid  = DEVID(devcb, opncb->unitno);
	reqcb->req.cmd    = cmd;
	reqcb->req.abort  = FALSE;
	reqcb->req.start  = start;
	reqcb->req.size   = size;
	reqcb->req.buf    = buf;
	reqcb->req.asize  = 0;
	reqcb->req.error  = 0;

	/* Indicate that it is during processing */
	reqcb->tskid = tk_get_tid_impl();

	UnlockDM();

	/* Device driver call */
	DISABLE_INTERRUPT;
	knl_ctxtsk->sysmode++;
	ENABLE_INTERRUPT;
#if TA_GP
	ercd = CallDeviceDriver(&reqcb->req, tmout, exinf, 0, (FP)execfn, gp);
#else
	ercd = (*execfn)(&reqcb->req, tmout, exinf);
#endif
	DISABLE_INTERRUPT;
	knl_ctxtsk->sysmode--;
	ENABLE_INTERRUPT;

	LockDM();

	/* Indicate that it is not during processing */
	reqcb->tskid = 0;

	/* If there is an abort completion wait task,
	   notify abort completion */
	if ( opncb->abort_tskid > 0 && --opncb->abort_cnt == 0 ) {
		tk_sig_sem_impl(opncb->abort_semid, 1);
	}

	if ( ercd < E_OK ) {
		goto err_ret2;
	}

	UnlockDM();

	return REQID(reqcb);

err_ret2:
	knl_delReqCB(reqcb);
err_ret1:
	UnlockDM();
	DEBUG_PRINT(("knl_request ercd = %d\n", ercd));
	return ercd;
}
#endif /* USE_FUNC_REQUEST */

#ifdef USE_FUNC_TK_REA_DEV
/*
 * Start reading from device
 */
SYSCALL ID tk_rea_dev_impl( ID dd, W start, void *buf, SZ size, TMO tmout )
{
	ER	ercd;

	ercd = knl_request(dd, start, buf, size, tmout, TDC_READ);

#ifdef DEBUG
	if ( ercd < E_OK ) {
		DEBUG_PRINT(("tk_rea_dev_impl ercd = %d\n", ercd));
	}
#endif
	return ercd;
}
#endif /* USE_FUNC_TK_REA_DEV */

#ifdef USE_FUNC_TK_SREA_DEV
/*
 * Synchronous reading from device
 */
SYSCALL ER tk_srea_dev_impl( ID dd, W start, void *buf, SZ size, SZ *asize )
{
	ER	ercd, ioercd;

	ercd = tk_rea_dev_impl(dd, start, buf, size, TMO_FEVR);
	if ( ercd < E_OK ) {
		goto err_ret;
	}

	ercd = tk_wai_dev_impl(dd, ercd, asize, &ioercd, TMO_FEVR);
	if ( ercd < E_OK ) {
		goto err_ret;
	}

	return ioercd;

err_ret:
	DEBUG_PRINT(("tk_srea_dev_impl ercd = %d\n", ercd));
	return ercd;
}
#endif /* USE_FUNC_TK_SREA_DEV */

#ifdef USE_FUNC_TK_WRI_DEV
/*
 * Start writing to device
 */
SYSCALL ID tk_wri_dev_impl( ID dd, W start, CONST void *buf, SZ size, TMO tmout )
{
	ER	ercd;

	ercd = knl_request(dd, start, (void *)buf, size, tmout, TDC_WRITE);

#ifdef DEBUG
	if ( ercd < E_OK ) {
		DEBUG_PRINT(("tk_wri_dev_impl ercd = %d\n", ercd));
	}
#endif
	return ercd;
}
#endif /* USE_FUNC_TK_WRI_DEV */

#ifdef USE_FUNC_TK_SWRI_DEV
/*
 * Synchronous writing to device
 */
SYSCALL ER tk_swri_dev_impl( ID dd, W start, CONST void *buf, SZ size, SZ *asize )
{
	ER	ercd, ioercd;

	ercd = tk_wri_dev_impl(dd, start, buf, size, TMO_FEVR);
	if ( ercd < E_OK ) {
		goto err_ret;
	}

	ercd = tk_wai_dev_impl(dd, ercd, asize, &ioercd, TMO_FEVR);
	if ( ercd < E_OK ) {
		goto err_ret;
	}

	return ioercd;

err_ret:
	DEBUG_PRINT(("tk_swri_dev_impl ercd = %d\n", ercd));
	return ercd;
}
#endif /* USE_FUNC_TK_SWRI_DEV */

#ifdef USE_FUNC_TK_WAI_DEV
/*
 * Verify validity of request ID
 */
LOCAL ReqCB* knl_check_reqid( ID reqid, OpnCB *opncb )
{
	ReqCB	*reqcb;

	if ( reqid < 1 || reqid > CFN_MAX_REQDEV ) {
		return NULL;
	}
	reqcb = REQCB(reqid);
	if ( reqcb->opncb != opncb ) {
		return NULL;
	}

	return reqcb;
}

/*
 * Request completion wait
 */
SYSCALL ID tk_wai_dev_impl( ID dd, ID reqid, SZ *asize, ER *ioer, TMO tmout )
{
	WAIFN	waitfn;
	void	*exinf;
#if TA_GP
	void	*gp;
#endif
	OpnCB	*opncb;
	DevCB	*devcb;
	ReqCB	*reqcb;
	T_DEVREQ *devreq;
	INT	reqno, nreq;
	ID	tskid;
	ER	ercd;

	tskid = tk_get_tid_impl();

	LockDM();

	ercd = knl_check_devdesc(dd, 0, &opncb);
	if ( ercd < E_OK ) {
		goto err_ret2;
	}

	devcb = opncb->devcb;
	waitfn = (WAIFN)devcb->ddev.waitfn;
	exinf = devcb->ddev.exinf;
#if TA_GP
	gp = devcb->ddev.gp;
#endif

	if ( reqid == 0 ) {
		/* When waiting for completion of any of requests for 'dd' */
		if ( opncb->nwaireq > 0 || opncb->waitone > 0 ) {
			ercd = E_OBJ;
			goto err_ret2;
		}
		if ( isQueEmpty(&opncb->requestq) ) {
			ercd = E_NOEXS;
			goto err_ret2;
		}

		/* Create wait request list */
		reqcb = (ReqCB*)opncb->requestq.next;
		for ( nreq = 1;; nreq++ ) {
			reqcb->tskid = tskid;
			devreq = &reqcb->req;
			reqcb = (ReqCB*)reqcb->q.next;
			if ( reqcb == (ReqCB*)&opncb->requestq ) {
				break;
			}
			devreq->next = &reqcb->req;
		}
		devreq->next = NULL;
		devreq = &((ReqCB*)opncb->requestq.next)->req;

		opncb->waireqlst = devreq;
		opncb->nwaireq = nreq;
	} else {
		/* Wait for completion of abort request processing */
		reqcb = knl_check_reqid(reqid, opncb);
		if ( reqcb == NULL ) {
			ercd = E_ID;
			goto err_ret2;
		}
		if ( opncb->nwaireq > 0 || reqcb->tskid > 0 ) {
			ercd = E_OBJ;
			goto err_ret2;
		}

		/* Create waiting request list */
		reqcb->tskid = tskid;
		devreq = &reqcb->req;
		devreq->next = NULL;
		nreq = 1;

		opncb->waitone++;
	}

	UnlockDM();

	/* Device driver call */
	DISABLE_INTERRUPT;
	knl_ctxtsk->sysmode++;
	ENABLE_INTERRUPT;
#if TA_GP
	reqno = CallDeviceDriver(devreq, nreq, tmout, exinf, (FP)waitfn, gp);
#else
	reqno = (*waitfn)(devreq, nreq, tmout, exinf);
#endif
	DISABLE_INTERRUPT;
	knl_ctxtsk->sysmode--;
	ENABLE_INTERRUPT;

	if ( reqno <  E_OK ) {
		ercd = reqno;
	}
	if ( reqno >= nreq ) {
		ercd = E_SYS;
	}

	LockDM();

	/* Free wait processing */
	if ( reqid == 0 ) {
		opncb->nwaireq = 0;
	} else {
		opncb->waitone--;
	}

	/* If there is an abort completion wait task,
	   notify abort completion */
	if ( opncb->abort_tskid > 0 && --opncb->abort_cnt == 0 ) {
		tk_sig_sem_impl(opncb->abort_semid, 1);
	}

	/* Get processing result */
	while ( devreq != NULL ) {
		reqcb = DEVREQ_REQCB(devreq);
		if ( reqno-- == 0 ) {
			reqid = REQID(reqcb);
			*asize = devreq->asize;
			*ioer  = devreq->error;
		}
		reqcb->tskid = 0;
		devreq = devreq->next;
	}

	if ( ercd < E_OK ) {
		goto err_ret2;
	}

	/* Unregister completed request */
	knl_delReqCB(REQCB(reqid));

	UnlockDM();

	return reqid;

err_ret2:
	UnlockDM();
	DEBUG_PRINT(("tk_wai_dev_impl ercd = %d\n", ercd));
	return ercd;
}
#endif /* USE_FUNC_TK_WAI_DEV */

/* ------------------------------------------------------------------------ */

#ifdef USE_FUNC_DISSUSCNT
/* Suspend disable request count */
EXPORT INT	knl_DisSusCnt = 0;
#endif /* USE_FUNC_DISSUSCNT */

#ifdef USE_FUNC_TK_SUS_DEV
/*
 * Send driver request event to each device
 */
LOCAL ER sendevt_alldevice( INT evttyp, BOOL disk )
{
	EVTFN	eventfn;
	QUEUE	*q;
	DevCB	*devcb;
	BOOL	d;
	ER	ercd = E_OK;

	for ( q = knl_UsedDevCB.next; q != &knl_UsedDevCB; q = q->next ) {
		devcb = (DevCB*)q;

		d = ( (devcb->ddev.devatr & TD_DEVTYPE) == TDK_DISK )?
							TRUE: FALSE;
		if ( disk != d ) {
			continue;
		}

		/* Device driver call */
		eventfn = (EVTFN)devcb->ddev.eventfn;
		DISABLE_INTERRUPT;
		knl_ctxtsk->sysmode++;
		ENABLE_INTERRUPT;
#if TA_GP
		ercd = CallDeviceDriver(evttyp, NULL, devcb->ddev.exinf, 0,
						(FP)eventfn, devcb->ddev.gp);
#else
		ercd = (*eventfn)(evttyp, NULL, devcb->ddev.exinf);
#endif
		DISABLE_INTERRUPT;
		knl_ctxtsk->sysmode--;
		ENABLE_INTERRUPT;
	}

#ifdef DEBUG
	if ( ercd < E_OK ) {
		DEBUG_PRINT(("sendevt_alldevice ercd = %d\n", ercd));
	}
#endif
	return ercd;
}

/*
 * Suspend
 */
LOCAL ER do_suspend( void )
{
	ER	ercd;

	/* Stop accepting device registration/unregistration */
	LockREG();

	/* Suspend processing of device except for disks */
	ercd = sendevt_alldevice(TDV_SUSPEND, FALSE);
#ifdef DEBUG
	if ( ercd < E_OK ) {
		DEBUG_PRINT(("2. do_suspend -> sendevt_alldevice ercd = %d\n", ercd));
	}
#endif

	/* Suspend processing of disk device */
	ercd = sendevt_alldevice(TDV_SUSPEND, TRUE);
#ifdef DEBUG
	if ( ercd < E_OK ) {
		DEBUG_PRINT(("3. do_suspend -> sendevt_alldevice ercd = %d\n", ercd));
	}
#endif

	/* Stop accepting new requests */
	LockDM();

	/*
	 * Insert code to transit to suspend state here
	 */

	/*
	 * Insert code executed on returning from suspend state
	 */


	/* Resume accepting requests */
	UnlockDM();

	/* Resume processing of disk device */
	ercd = sendevt_alldevice(TDV_RESUME, TRUE);
#ifdef DEBUG
	if ( ercd < E_OK ) {
		DEBUG_PRINT(("7. do_suspend -> sendevt_alldevice ercd = %d\n", ercd));
	}
#endif

	/* Resume processing of device except for disks */
	ercd = sendevt_alldevice(TDV_RESUME, FALSE);
#ifdef DEBUG
	if ( ercd < E_OK ) {
		DEBUG_PRINT(("8. do_suspend -> sendevt_alldevice ercd = %d\n", ercd));
	}
#endif

	/* Resume accepting device registration/unregistration */
	UnlockREG();

	return ercd;
}

/*
 * Suspend processing
 */
SYSCALL INT tk_sus_dev_impl( UINT mode )
{
	ResCB	*rescb;
	BOOL	suspend = FALSE;
	ER	ercd;

	/* Get resource management information */
	rescb = knl_GetResCB();
	if ( rescb == NULL ) {
		ercd = E_CTX;
		goto err_ret1;
	}

	LockDM();

	switch ( mode & 0xf ) {
	  case TD_SUSPEND:	/* Suspend */
		if ( knl_DisSusCnt > 0 && (mode & TD_FORCE) == 0 ) {
			ercd = E_BUSY;
			goto err_ret2;
		}
		suspend = TRUE;
		break;

	  case TD_DISSUS:	/* Disable suspend */
		if ( knl_DisSusCnt >= MAX_DISSUS ) {
			ercd = E_QOVR;
			goto err_ret2;
		}
		knl_DisSusCnt++;
		rescb->dissus++;
		break;
	  case TD_ENASUS:	/* Enable suspend */
		if ( rescb->dissus > 0 ) {
			rescb->dissus--;
			knl_DisSusCnt--;
		}
		break;

	  case TD_CHECK:	/* Get suspend disable request count */
		break;

	  default:
		ercd = E_PAR;
		goto err_ret2;
	}

	UnlockDM();

	if ( suspend ) {
		/* Suspend */
		ercd = do_suspend();
		if ( ercd < E_OK ) {
			goto err_ret1;
		}
	}

	return knl_DisSusCnt;

err_ret2:
	UnlockDM();
err_ret1:
	DEBUG_PRINT(("tk_sus_dev_impl ercd = %d\n", ercd));
	return ercd;
}
#endif /* USE_FUNC_TK_SUS_DEV */

/* ------------------------------------------------------------------------ */

#ifdef USE_FUNC_DEVMGR_STARTUP
/*
 * Device management startup function
 */
EXPORT void knl_devmgr_startup( void )
{
	LockDM();

	/* Initialization of open device management queue */
	QueInit(&(knl_resource_control_block.openq));
	knl_resource_control_block.dissus = 0;
	
	UnlockDM();

	return;
}
#endif /* USE_FUNC_DEVMGR_STARTUP */

#ifdef USE_FUNC_DEVMGR_CLEANUP
/*
 * Device management cleanup function
 */
EXPORT void knl_devmgr_cleanup( void )
{
	OpnCB	*opncb;

	/* Do nothing if it is not used even once */
	if ( knl_resource_control_block.openq.next == NULL ) {
		return;
	}

	LockDM();

	/* Free suspend disable request */
	knl_DisSusCnt -= knl_resource_control_block.dissus;
	knl_resource_control_block.dissus = 0;

	/* Close all open devices */
	while ( !isQueEmpty(&(knl_resource_control_block.openq)) ) {
		opncb = RESQ_OPNCB(knl_resource_control_block.openq.next);

		/* Indicate that it is during close processing */
		opncb->resid = 0;

		UnlockDM();

		/* Device close processing */
		knl_close_device(opncb, 0);

		LockDM();
	}
	UnlockDM();

	return;
}
#endif /* USE_FUNC_DEVMGR_CLEANUP */

#ifdef USE_FUNC_INITDEVIO
/*
 * Initialization sequence of device input/output-related
 */
EXPORT ER knl_initDevIO( void )
{
	INT	i;

	QueInit(&knl_FreeOpnCB);
	for ( i = 0; i < CFN_MAX_OPNDEV; ++i ) {
		knl_OpnCBtbl[i].resid = 0;
		QueInsert(&knl_OpnCBtbl[i].q, &knl_FreeOpnCB);
	}

	QueInit(&knl_FreeReqCB);
	for ( i = 0; i < CFN_MAX_REQDEV; ++i ) {
		knl_ReqCBtbl[i].opncb = NULL;
		QueInsert(&knl_ReqCBtbl[i].q, &knl_FreeReqCB);
	}

	return E_OK;
}
#endif /* USE_FUNC_INITDEVIO */

#ifdef USE_FUNC_FINISHDEVIO
/*
 * Finalization sequence of device input/output-related
 */
EXPORT ER knl_finishDevIO( void )
{
	return E_OK;
}
#endif /* USE_FUNC_FINISHDEVIO */

#endif /* CFN_MAX_REGDEV */
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
/*--------------------------------------------------------------------*/
/*  Include definition                                                */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Struct definition                                                 */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Constant definition                                               */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Variable definition                                               */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Macro definition                                                  */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Prototype declaration                                             */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Function definition                                               */
/*--------------------------------------------------------------------*/

#include <tk_typedef.h>
#include <errno.h>
#include <typedef.h>

EXPORT	W	knl_taskindp = 0;
Noinit(EXPORT	UW	knl_taskmode);
#if USE_IMALLOC
/* Low level memory manager information */
Noinit(EXPORT	void	*knl_lowmem_top);	/* Head of area (Low address) */
Noinit(EXPORT	void	*knl_lowmem_limit);	/* End of area (High address) */
#endif

/* ------------------------------------------------------------------------ */

/*
 * Initialization before micro T-Kernel starts
 */

EXPORT ER knl_init_device( void )
{
	return E_OK;
}

/* ------------------------------------------------------------------------ */
/*
 * Start processing after T-Kernel starts
 *	Called from the initial task contexts.
 */
EXPORT ER knl_start_device( void )
{
	return E_OK;
}

#if USE_CLEANUP
/* ------------------------------------------------------------------------ */
/*
 * System finalization
 *	Called just before system shutdown.
 *	Execute finalization that must be done before system shutdown.
 */
EXPORT ER knl_finish_device( void )
{
	return E_OK;
}

/* ------------------------------------------------------------------------ */
/*
 *	Re-starting processing
 */

/*
 * Re-starting processing
 *	mode = -1		Reset and re-start	(cold boot)
 *	mode = -2		Re-start		(warm boot)
 *	mode = -3		Reboot			(normal boot)
 *	mode = 0xFFhhmmss	Re-start at hh:mm:ss
 *				0 <= hh < 24, 0 <= mm,ss < 60
 */
EXPORT ER knl_restart_device( W mode )
{
	if ( mode == -1 ) {
		/* Reset and re-start (cold boot) */
#if USE_KERNEL_MESSAGE
		tm_putstring((UB*)"\n<< SYSTEM RESTART >>\n");
#endif
		tm_exit(-1);  /* no return */
		return E_OBJ;
	}

	if ( mode == -3 ) {
		/* Reboot (normal boot) */
#if USE_KERNEL_MESSAGE
		tm_putstring((UB*)"\n<< SYSTEM REBOOT >>\n");
#endif
		return E_NOSPT;
	}

	if ( mode == -2 ) {
		return E_NOSPT; /* Unsupported */
	}

	if ( (mode & 0xff000000U) == 0xff000000U ) {
		/* Re-start at specified time */
		return E_NOSPT;	/* Unsupported */
	}

	return E_PAR;
}
#endif /* USE_CLEANUP */
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
 *	@(#)errno.h
 *
 *	micro T-Kernel Error Code
 */

#ifndef __TK_ERRNO_H__
#define __TK_ERRNO_H__

#if 0
/*
 * To port a program using macros concerning error codes from T-Kernel
 * into micro T-Kernel, you may modify and use following macros (ERCD, 
 * MERCD, SERCD).
 */
#ifndef _in_asm_source_
#include <tk_typedef.h>

#define MERCD(er)	( (ER)(er) >> 16 )	/* Main error code */
#define SERCD(er)	( (H)(er) )		/* Sub-error code */
#define ERCD(mer, ser)	( (ER)(((UW)(mer) << 16) | ((UW)(ser) & 0x0000FFFF)) )
#else
#define ERCD(mer, ser)	( ((mer) << 16) | ((ser) & 0xffff) )
#endif /* _in_asm_source_ */
#endif

#define E_OK		(0)	/* Completed successfully */

#define E_SYS		(-5)	/* System error */
#define E_NOCOP		(-6)	/* Coprocessor disable */
#define E_NOSPT		(-9)	/* Unsupported function */
#define E_RSFN		(-10)	/* Reserved function code number */
#define E_RSATR		(-11)	/* Reserved attribute */
#define E_PAR		(-17)	/* Parameter error */
#define E_ID		(-18)	/* Incorrect ID number */
#define E_CTX		(-25)	/* Context error */
#define E_MACV		(-26)	/* Inaccessible memory/access violation */
#define E_OACV		(-27)	/* Object access violation */
#define E_ILUSE		(-28)	/* Incorrect system call use */
#define E_NOMEM		(-33)	/* Insufficient memory */
#define E_LIMIT		(-34)	/* Exceed system limits */
#define E_OBJ		(-41)	/* Incorrect object state */
#define E_NOEXS		(-42)	/* Object does not exist */
#define E_QOVR		(-43)	/* Queuing overflow */
#define E_RLWAI		(-49)	/* Forcibly release wait state */
#define E_TMOUT		(-50)	/* Polling fail/time out */
#define E_DLT		(-51)	/* Waited object was deleted */
#define E_DISWAI	(-52)	/* Release wait caused by wait disable */

#define E_IO		(-57)	/* Output/input error */
#define E_NOMDA		(-58)	/* No media */
#define E_BUSY		(-65)	/* Busy state */
#define E_ABORT		(-66)	/* Aborted */
#define E_RONLY		(-67)	/* Write protected */

#endif /* __TK_ERRNO_H__ */
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
 *	eventflag.c
 *	Event Flag
 */

/** [BEGIN Common Definitions] */
#include "kernel.h"
#include "task.h"
#include "wait.h"
#include "check.h"
#include "eventflag.h"
/** [END Common Definitions] */

#if CFN_MAX_FLGID > 0

#ifdef USE_FUNC_FLGCB_TABLE
Noinit(EXPORT FLGCB	knl_flgcb_table[NUM_FLGID]);	/* Event flag control block */
Noinit(EXPORT QUEUE	knl_free_flgcb);	/* FreeQue */
#endif /* USE_FUNC_FLGCB_TABLE */


#ifdef USE_FUNC_EVENTFLAG_INITIALIZE
/*
 * Initialization of event flag control block 
 */
EXPORT ER knl_eventflag_initialize( void )
{
	FLGCB	*flgcb, *end;

	/* Get system information */
	if ( NUM_FLGID < 1 ) {
		return E_SYS;
	}

	/* Register all control blocks onto FreeQue */
	QueInit(&knl_free_flgcb);
	end = knl_flgcb_table + NUM_FLGID;
	for ( flgcb = knl_flgcb_table; flgcb < end; flgcb++ ) {
		flgcb->flgid = 0;
		QueInsert(&flgcb->wait_queue, &knl_free_flgcb);
	}

	return E_OK;
}
#endif /* USE_FUNC_EVENTFLAG_INITIALIZE */

#ifdef USE_FUNC_TK_CRE_FLG
/*
 * Create event flag
 */
SYSCALL ID tk_cre_flg_impl( CONST T_CFLG *pk_cflg )
{
#if CHK_RSATR
	const ATR VALID_FLGATR = {
		 TA_TPRI
		|TA_WMUL
#if USE_OBJECT_NAME
		|TA_DSNAME
#endif
	};
#endif
	FLGCB	*flgcb;
	ID	flgid;
	ER	ercd;

	CHECK_RSATR(pk_cflg->flgatr, VALID_FLGATR);

	BEGIN_CRITICAL_SECTION;
	/* Get control block from FreeQue */
	flgcb = (FLGCB*)QueRemoveNext(&knl_free_flgcb);
	if ( flgcb == NULL ) {
		ercd = E_LIMIT;
	} else {
		flgid = ID_FLG(flgcb - knl_flgcb_table);

		/* Initialize control block */
		QueInit(&flgcb->wait_queue);
		flgcb->flgid = flgid;
		flgcb->exinf = pk_cflg->exinf;
		flgcb->flgatr = pk_cflg->flgatr;
		flgcb->flgptn = pk_cflg->iflgptn;
#if USE_OBJECT_NAME
		if ( (pk_cflg->flgatr & TA_DSNAME) != 0 ) {
			strncpy((char*)flgcb->name, (char*)pk_cflg->dsname,
				OBJECT_NAME_LENGTH);
		}
#endif
		ercd = flgid;
	}
	END_CRITICAL_SECTION;

	return ercd;
}
#endif /* USE_FUNC_TK_CRE_FLG */

#ifdef USE_FUNC_TK_DEL_FLG
/*
 * Delete event flag
 */
SYSCALL ER tk_del_flg_impl( ID flgid )
{
	FLGCB	*flgcb;
	ER	ercd = E_OK;

	CHECK_FLGID(flgid);

	flgcb = get_flgcb(flgid);

	BEGIN_CRITICAL_SECTION;
	if ( flgcb->flgid == 0 ) {
		ercd = E_NOEXS;
	} else {
		/* Release wait state of task (E_DLT) */
		knl_wait_delete(&flgcb->wait_queue);

		/* Return to FreeQue */
		QueInsert(&flgcb->wait_queue, &knl_free_flgcb);
		flgcb->flgid = 0;
	}
	END_CRITICAL_SECTION;

	return ercd;
}
#endif /* USE_FUNC_TK_DEL_FLG */

#ifdef USE_FUNC_TK_SET_FLG
/*
 * Event flag set
 */
SYSCALL ER tk_set_flg_impl( ID flgid, UINT setptn )
{
	FLGCB	*flgcb;
	TCB	*tcb;
	QUEUE	*queue;
	UINT	wfmode, waiptn;
	ER	ercd = E_OK;

	CHECK_FLGID(flgid);

	flgcb = get_flgcb(flgid);

	BEGIN_CRITICAL_SECTION;
	if ( flgcb->flgid == 0 ) {
		ercd = E_NOEXS;
		goto error_exit;
	}

	/* Set event flag */
	flgcb->flgptn |= setptn;

	/* Search task which should be released */
	queue = flgcb->wait_queue.next;
	while ( queue != &flgcb->wait_queue ) {
		tcb = (TCB*)queue;
		queue = queue->next;

		/* Meet condition for release wait? */
		waiptn = tcb->winfo.flg.waiptn;
		wfmode = tcb->winfo.flg.wfmode;
		if ( knl_eventflag_cond(flgcb, waiptn, wfmode) ) {

			/* Release wait */
			*tcb->winfo.flg.p_flgptn = flgcb->flgptn;
			knl_wait_release_ok(tcb);

			/* Clear event flag */
			if ( (wfmode & TWF_BITCLR) != 0 ) {
				if ( (flgcb->flgptn &= ~waiptn) == 0 ) {
					break;
				}
			}
			if ( (wfmode & TWF_CLR) != 0 ) {
				flgcb->flgptn = 0;
				break;
			}
		}
	}

    error_exit:
	END_CRITICAL_SECTION;

	return ercd;
}
#endif /* USE_FUNC_TK_SET_FLG */

#ifdef USE_FUNC_TK_CLR_FLG
/*
 * Clear event flag 
 */
SYSCALL ER tk_clr_flg_impl( ID flgid, UINT clrptn )
{
	FLGCB	*flgcb;
	ER	ercd = E_OK;

	CHECK_FLGID(flgid);

	flgcb = get_flgcb(flgid);

	BEGIN_CRITICAL_SECTION;
	if ( flgcb->flgid == 0 ) {
		ercd = E_NOEXS;
	} else {
		flgcb->flgptn &= clrptn;
	}
	END_CRITICAL_SECTION;

	return ercd;
}
#endif /* USE_FUNC_TK_CLR_FLG */

#ifdef USE_FUNC_TK_WAI_FLG
/*
 * Processing if the priority of wait task changes
 */
LOCAL void flg_chg_pri( TCB *tcb, INT oldpri )
{
	FLGCB	*flgcb;

	flgcb = get_flgcb(tcb->wid);
	knl_gcb_change_priority((GCB*)flgcb, tcb);
}

/*
 * Definition of event flag wait specification
 */
LOCAL CONST WSPEC knl_wspec_flg_tfifo = { TTW_FLG, NULL, NULL };
LOCAL CONST WSPEC knl_wspec_flg_tpri  = { TTW_FLG, flg_chg_pri, NULL };

/*
 * Event flag wait
 */
SYSCALL ER tk_wai_flg_impl( ID flgid, UINT waiptn, UINT wfmode, UINT *p_flgptn, TMO tmout )
{
	FLGCB	*flgcb;
	ER	ercd = E_OK;

	CHECK_FLGID(flgid);
	CHECK_PAR(waiptn != 0);
	CHECK_PAR((wfmode & ~(TWF_ORW|TWF_CLR|TWF_BITCLR)) == 0);
	CHECK_TMOUT(tmout);
	CHECK_DISPATCH();

	flgcb = get_flgcb(flgid);

	BEGIN_CRITICAL_SECTION;
	if ( flgcb->flgid == 0 ) {
		ercd = E_NOEXS;
		goto error_exit;
	}
	if ( (flgcb->flgatr & TA_WMUL) == 0 && !isQueEmpty(&flgcb->wait_queue) ) {
		/* Disable multiple tasks wait */
		ercd = E_OBJ;
		goto error_exit;
	}

	/* Meet condition for release wait? */
	if ( knl_eventflag_cond(flgcb, waiptn, wfmode) ) {
		*p_flgptn = flgcb->flgptn;

		/* Clear event flag */
		if ( (wfmode & TWF_BITCLR) != 0 ) {
			flgcb->flgptn &= ~waiptn;
		}
		if ( (wfmode & TWF_CLR) != 0 ) {
			flgcb->flgptn = 0;
		}
	} else {
		/* Ready for wait */
		knl_ctxtsk->wspec = ( (flgcb->flgatr & TA_TPRI) != 0 )?
					&knl_wspec_flg_tpri: &knl_wspec_flg_tfifo;
		knl_ctxtsk->wercd = &ercd;
		knl_ctxtsk->winfo.flg.waiptn = waiptn;
		knl_ctxtsk->winfo.flg.wfmode = wfmode;
		knl_ctxtsk->winfo.flg.p_flgptn = p_flgptn;
		knl_gcb_make_wait((GCB*)flgcb, tmout);
	}

    error_exit:
	END_CRITICAL_SECTION;

	return ercd;
}
#endif /* USE_FUNC_TK_WAI_FLG */

#ifdef USE_FUNC_TK_REF_FLG
/*
 * Check event flag state
 */
SYSCALL ER tk_ref_flg_impl( ID flgid, T_RFLG *pk_rflg )
{
	FLGCB	*flgcb;
	ER	ercd = E_OK;

	CHECK_FLGID(flgid);

	flgcb = get_flgcb(flgid);

	BEGIN_CRITICAL_SECTION;
	if ( flgcb->flgid == 0 ) {
		ercd = E_NOEXS;
	} else {
		pk_rflg->exinf = flgcb->exinf;
		pk_rflg->wtsk = knl_wait_tskid(&flgcb->wait_queue);
		pk_rflg->flgptn = flgcb->flgptn;
	}
	END_CRITICAL_SECTION;

	return ercd;
}
#endif /* USE_FUNC_TK_REF_FLG */

/* ------------------------------------------------------------------------ */
/*
 *	Debugger support function
 */
#if USE_DBGSPT

#ifdef USE_FUNC_EVENTFLAG_GETNAME
#if USE_OBJECT_NAME
/*
 * Get object name from control block
 */
EXPORT ER knl_eventflag_getname(ID id, UB **name)
{
	FLGCB	*flgcb;
	ER	ercd = E_OK;

	CHECK_FLGID(id);

	BEGIN_DISABLE_INTERRUPT;
	flgcb = get_flgcb(id);
	if ( flgcb->flgid == 0 ) {
		ercd = E_NOEXS;
		goto error_exit;
	}
	if ( (flgcb->flgatr & TA_DSNAME) == 0 ) {
		ercd = E_OBJ;
		goto error_exit;
	}
	*name = flgcb->name;

    error_exit:
	END_DISABLE_INTERRUPT;

	return ercd;
}
#endif /* USE_OBJECT_NAME */
#endif /* USE_FUNC_EVENTFLAG_GETNAME */

#ifdef USE_FUNC_TD_LST_FLG
/*
 * Refer event flag usage state
 */
SYSCALL INT td_lst_flg_impl( ID list[], INT nent )
{
	FLGCB	*flgcb, *end;
	INT	n = 0;

	BEGIN_DISABLE_INTERRUPT;
	end = knl_flgcb_table + NUM_FLGID;
	for ( flgcb = knl_flgcb_table; flgcb < end; flgcb++ ) {
		if ( flgcb->flgid == 0 ) {
			continue;
		}

		if ( n++ < nent ) {
			*list++ = flgcb->flgid;
		}
	}
	END_DISABLE_INTERRUPT;

	return n;
}
#endif /* USE_FUNC_TD_LST_FLG */

#ifdef USE_FUNC_TD_REF_FLG
/*
 * Refer event flag state
 */
SYSCALL ER td_ref_flg_impl( ID flgid, TD_RFLG *pk_rflg )
{
	FLGCB	*flgcb;
	ER	ercd = E_OK;

	CHECK_FLGID(flgid);

	flgcb = get_flgcb(flgid);

	BEGIN_DISABLE_INTERRUPT;
	if ( flgcb->flgid == 0 ) {
		ercd = E_NOEXS;
	} else {
		pk_rflg->exinf = flgcb->exinf;
		pk_rflg->wtsk = knl_wait_tskid(&flgcb->wait_queue);
		pk_rflg->flgptn = flgcb->flgptn;
	}
	END_DISABLE_INTERRUPT;

	return ercd;
}
#endif /* USE_FUNC_TD_REF_FLG */

#ifdef USE_FUNC_TD_FLG_QUE
/*
 * Refer event flag wait queue
 */
SYSCALL INT td_flg_que_impl( ID flgid, ID list[], INT nent )
{
	FLGCB	*flgcb;
	QUEUE	*q;
	ER	ercd = E_OK;

	CHECK_FLGID(flgid);

	flgcb = get_flgcb(flgid);

	BEGIN_DISABLE_INTERRUPT;
	if ( flgcb->flgid == 0 ) {
		ercd = E_NOEXS;
	} else {
		INT n = 0;
		for ( q = flgcb->wait_queue.next; q != &flgcb->wait_queue; q = q->next ) {
			if ( n++ < nent ) {
				*list++ = ((TCB*)q)->tskid;
			}
		}
		ercd = n;
	}
	END_DISABLE_INTERRUPT;

	return ercd;
}
#endif /* USE_FUNC_TD_FLG_QUE */

#endif /* USE_DBGSPT */
#endif /* CFN_MAX_FLGID */
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
 *	eventflag.h
 *	Event Flag
 */

#ifndef _EVENTFLAG_H_
#define _EVENTFLAG_H_

/*
 * Event flag control block
 */
typedef struct eventflag_control_block {
	QUEUE	wait_queue;	/* Event flag wait queue */
	ID	flgid;		/* Event flag ID */
	void	*exinf;		/* Extended information */
	ATR	flgatr;		/* Event flag attribute */
	UINT	flgptn;		/* Event flag current pattern */
#if USE_OBJECT_NAME
	UB	name[OBJECT_NAME_LENGTH];	/* name */
#endif
} FLGCB;

IMPORT FLGCB knl_flgcb_table[];	/* Event flag control block */
IMPORT QUEUE knl_free_flgcb;	/* FreeQue */

#define get_flgcb(id)	( &knl_flgcb_table[INDEX_FLG(id)] )


/*
 * Check for event flag wait release condition
 */
BOOL knl_eventflag_cond( FLGCB *flgcb, UINT waiptn, UINT wfmode )
{
	if ( (wfmode & TWF_ORW) != 0 ) {
		return ( (flgcb->flgptn & waiptn) != 0 );
	} else {
		return ( (flgcb->flgptn & waiptn) == waiptn );
	}
}


#endif /* _EVENTFLAG_H_ */
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
 *	@(#)fastlock.c (libtk)
 *
 *	High-speed exclusive control lock 
 */

/** [BEGIN Common Definitions] */
#include <basic.h>
#include <tkernel.h>
#include <util.h>
#include <libstr.h>
#include "libtk_config.h"

/* ------------------------------------------------------------------------ */
/*
 *	Inc	Increment cnt, in result if cnt >  0, returns positive value.
 *		If cnt <= 0, returns 0 or negative value.
 *	Dec	Decrement cnt, in result if cnt >= 0, returns positive value.
 *		If cnt <  0, returns 0 or negative value. 
 *	Increment/Decrement and evaluation of the associated result must
 *	be executed exclusively.
 */

INT Inc( FastLock *lock )
{
	UINT	imask;
	INT	c;
	DI(imask);
	c = ++lock->cnt;
	EI(imask);
	return c;
}
INT Dec( FastLock *lock )
{
	UINT	imask;
	INT	c;
	DI(imask);
	c = lock->cnt--;
	EI(imask);
	return c;
}

/* ------------------------------------------------------------------------ */
/** [END Common Definitions] */

#ifdef USE_FUNC_LOCK
/*
 * Lock 
 */
EXPORT void Lock( FastLock *lock )
{
	if ( Inc(lock) > 0 ) {
		tk_wai_sem(lock->id, 1, TMO_FEVR);
	}
}
#endif /* USE_FUNC_LOCK */

#ifdef USE_FUNC_UNLOCK
/*
 * Lock release
 */
EXPORT void Unlock( FastLock *lock )
{
	if ( Dec(lock) > 0 ) {
		tk_sig_sem(lock->id, 1);
	}
}
#endif /* USE_FUNC_UNLOCK */

#ifdef USE_FUNC_CREATELOCK
/*
 * Create high-speed lock 
 */
EXPORT ER CreateLock( FastLock *lock, CONST UB *name )
{
	T_CSEM	csem;
	ER	ercd;

	if ( name == NULL ) {
		csem.exinf = NULL;
	} else {
		strncpy((char*)&csem.exinf, (char*)name, sizeof(csem.exinf));
	}
	csem.sematr  = TA_TPRI;
	csem.isemcnt = 0;
	csem.maxsem  = 1;

	ercd = tk_cre_sem(&csem);
	if ( ercd < E_OK ) {
		return ercd;
	}

	lock->id = ercd;
	lock->cnt = -1;

	return E_OK;
}
#endif /* USE_FUNC_CREATELOCK */

#ifdef USE_FUNC_DELETELOCK
/*
 * Delete high-speed lock
 */
EXPORT void DeleteLock( FastLock *lock )
{
	if ( lock->id > 0 ) {
		tk_del_sem(lock->id);
	}
	lock->id = 0;
}
#endif /* USE_FUNC_DELETELOCK */
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
 *	@(#)fastmlock.c (libtk)
 *
 *	High-speed exclusive control multi-lock 
 */

/** [BEGIN Common Definitions] */
#include <basic.h>
#include <tkernel.h>
#include <util.h>
#include <libstr.h>
#include "libtk_config.h"

/* ------------------------------------------------------------------------ */
/*
 *	void INC( INT *val )		increment 
 *	void DEC( INT *val )		decrement 
 *	BOOL BTS( UINT *val, INT no )	bit test and set 
 *	void BR( UINT *val, INT no )	bit reset 
 *
 *	The above must be operated exclusively.
 */

void INC( INT *val )
{
	UINT	imask;

	DI(imask);
	(*val)++;
	EI(imask);
}

void DEC( INT *val )
{
	UINT	imask;

	DI(imask);
	(*val)--;
	EI(imask);
}

BOOL BTS( UINT *val, INT no )
{
	UINT	imask;
	UINT	b;
	UINT	bm = (UINT)(1 << no);

	DI(imask);
	b = *val & bm;
	*val |= bm;
	EI(imask);
	return (BOOL)b;
}

void BR( UINT *val, INT no )
{
	UINT	imask;

	DI(imask);
	*val &= ~(UINT)(1 << no);
	EI(imask);
}

/* ------------------------------------------------------------------------ */
/** [END Common Definitions] */

#ifdef USE_FUNC_MLOCKTMO
/*
 * Lock with wait time designation 
 *	no	lock number 0 - 31 
 */
EXPORT ER MLockTmo( FastMLock *lock, INT no, TMO tmo )
{
	UINT	ptn = (UINT)(1 << no);
	UINT	flg;
	ER	ercd;

	INC(&lock->wai);
	for ( ;; ) {
		if ( !BTS(&lock->flg, no) ) {
			ercd = E_OK;
			break;
		}

		ercd = tk_wai_flg(lock->id, ptn, TWF_ORW|TWF_BITCLR, &flg, tmo);
		if ( ercd < E_OK ) {
			break;
		}
	}
	DEC(&lock->wai);

	return ercd;
}
#endif /* USE_FUNC_MLOCKTMO */

#ifdef USE_FUNC_MLOCK
/*
 * Lock 
 *	no	Lock number 0 - 31 
 */
EXPORT ER MLock( FastMLock *lock, INT no )
{
	return MLockTmo(lock, no, TMO_FEVR);
}
#endif /* USE_FUNC_MLOCK */

#ifdef USE_FUNC_MUNLOCK
/*
 * Lock release 
 *	no	Lock number 0 - 31 
 */
EXPORT ER MUnlock( FastMLock *lock, INT no )
{
	UINT	ptn = (UINT)(1 << no);
	ER	ercd;

	BR(&lock->flg, no);
	ercd = ( lock->wai == 0 )? E_OK: tk_set_flg(lock->id, ptn);

	return ercd;
}
#endif /* USE_FUNC_MUNLOCK */

#ifdef USE_FUNC_CREATEMLOCK
/*
 * Create multi-lock 
 */
EXPORT ER CreateMLock( FastMLock *lock, CONST UB *name )
{
	T_CFLG	cflg;
	ER	ercd;

	if ( name == NULL ) {
		cflg.exinf = NULL;
	} else {
		strncpy((char*)&cflg.exinf, (char*)name, sizeof(cflg.exinf));
	}
	cflg.flgatr  = TA_TPRI | TA_WMUL;
	cflg.iflgptn = 0;

	lock->id = ercd = tk_cre_flg(&cflg);
	if ( ercd < E_OK ) {
		return ercd;
	}

	lock->wai = 0;
	lock->flg = 0;

	return E_OK;
}
#endif /* USE_FUNC_CREATEMLOCK */

#ifdef USE_FUNC_DELETEMLOCK
/*
 * Delete multi-lock 
 */
EXPORT ER DeleteMLock( FastMLock *lock )
{
	ER	ercd;

	if ( lock->id <= 0 ) {
		return E_PAR;
	}

	ercd = tk_del_flg(lock->id);
	if ( ercd < E_OK ) {
		return ercd;
	}

	lock->id = 0;

	return E_OK;
}
#endif /* USE_FUNC_DELETEMLOCK */
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
 *	hardware configuration (MB9AF312K)
 */

#ifndef _HWCONFIG_H_
#define _HWCONFIG_H_

#define USE_TMONITOR 0

#define INTERNAL_RAM_SIZE       0x00004000
#define INTERNAL_RAM_START      0x1FFFE000

#define INTERNAL_RAM_END        (INTERNAL_RAM_START+INTERNAL_RAM_SIZE)

#define FLASH_BASE      0x00000000
#define SRAM_BASE       0x1FFFE000

#define FLASH_SIZE      0x00020000		/* 128KB */

#define FLASH_START     FLASH_BASE
#define FLASH_END       (FLASH_START+FLASH_SIZE)

#define SRAM_SIZE       0x0004000

#define SRAM_START      SRAM_BASE
#define SRAM_END        (SRAM_START+SRAM_SIZE)

#endif /* _CONFIG_H_ */
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
 *	@(#)imalloc.h (sys)
 *
 *	Kernel memory allocation 
 *
 *	A function for allocating memory used in parts of micro T-Kernel.
 *	Not for general use.
 */

#ifndef __SYS_IMALLOC_H__
#define __SYS_IMALLOC_H__

#include <basic.h>
#include <tk_typedef.h>

#ifdef __cplusplus
extern "C" {
#endif

IMPORT void* knl_Imalloc( size_t size );
IMPORT void* knl_Icalloc( size_t nmemb, size_t size );
IMPORT void  knl_Ifree( void *ptr );

#ifdef __cplusplus
}
#endif
#endif /* __SYS_IMALLOC_H__ */
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
 *	inittask_def.c
 *	Initial task definition
 */
/*--------------------------------------------------------------------*/
/*  Include definition                                                */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Struct definition                                                 */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Constant definition                                               */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Variable definition                                               */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Macro definition                                                  */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Prototype declaration                                             */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Function definition                                               */
/*--------------------------------------------------------------------*/

#include "inittask_def.h"

IMPORT void knl_init_task(void);

#if !USE_IMALLOC
INT	init_task_stack[INITTASK_STKSZ/sizeof(INT)];
#endif

/*
 * Initial task creation parameter
 */
EXPORT const T_CTSK knl_c_init_task = {
	(void *)INITTASK_EXINF,		/* exinf */
#if USE_IMALLOC
	INITTASK_TSKATR,		/* tskatr */
#else
	INITTASK_TSKATR|TA_USERBUF,	/* tskatr */
#endif
	(FP)&knl_init_task,		/* task */
	INITTASK_ITSKPRI,		/* itskpri */
	INITTASK_STKSZ,			/* stksz */
#if USE_OBJECT_NAME
	INITTASK_DSNAME,		/* dsname */
#endif
#if USE_IMALLOC
	INITTASK_STACK,			/* bufptr */
#else
	init_task_stack,		/* bufptr */
#endif
};
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
 *	inittask_def.h
 *	Initial task definition
 */

#ifndef _INITTASK_DEF_
#define _INITTASK_DEF_

#include <basic.h>
#include <tkernel.h>
#include "utk_config.h"

/*
 * Initial task parameter
 */
#define INITTASK_EXINF		(0x0)
#define INITTASK_TSKATR		(TA_HLNG | TA_RNG0)
#define INITTASK_ITSKPRI	(1)
#define INITTASK_STKSZ		(1*1024)
#define INITTASK_DSNAME		"inittsk"
#define INITTASK_STACK		(NULL)

#endif /* _INITTASK_DEF_ */
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
 *	inittask_main.c (sysmain)
 *	Initial Task
 */
/*--------------------------------------------------------------------*/
/*  Include definition                                                */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Struct definition                                                 */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Constant definition                                               */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Variable definition                                               */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Macro definition                                                  */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Prototype declaration                                             */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Function definition                                               */
/*--------------------------------------------------------------------*/

#include "sysmain.h"
#include "kernel.h"
#include <debug.h>

typedef INT	(*MAIN_FP)(INT, UB **);

#if USE_KERNEL_MESSAGE
LOCAL const char knl_boot_message[] = {	/* Boot message */
	BOOT_MESSAGE
};
#endif

/* ------------------------------------------------------------------------ */

/*
 * Initial task Main
 *	The available stack size is slightly less than 8KB.
 *	The initial task is the system task,
 *	so it should not be deleted.
 */
EXPORT INT knl_init_task_main( void )
{
	INT	fin;

	/* Start message */
#if USE_KERNEL_MESSAGE
	tm_putstring((UB*)knl_boot_message);
#endif

	fin = 1;

#if RI_USERINIT != NULL
	/* Perform user defined initialization sequence */
	fin = (*(MAIN_FP)RI_USERINIT)(0, NULL);
#endif

	if ( fin > 0 ) {
		/* Perform user main */
		fin = usermain();
	}

#if RI_USERINIT != NULL
	/* Perform user defined finalization sequence */
	(*(MAIN_FP)RI_USERINIT)(-1, NULL);
#endif

	return fin;
}
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
 *	@(#)int.c (libtk/FM3)
 *
 *	Interrupt controller
 */
/*--------------------------------------------------------------------*/
/*  Include definition                                                */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Struct definition                                                 */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Constant definition                                               */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Variable definition                                               */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Macro definition                                                  */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Prototype declaration                                             */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Function definition                                               */
/*--------------------------------------------------------------------*/

#include <basic.h>
#include <syslib.h>
#include <sysdef.h>

#define INTPRI_MIN_UNIT	(0x100 >> INTPRI_BITWIDTH)
#define INTPRI_MASK	(0x100 - INTPRI_MIN_UNIT)

/*
 * Set Interrupt Mask Level in CPU
 */
IMPORT void SetCpuIntLevel( INT level )
{
	enaint((level + INTPRI_MIN_UNIT) & INTPRI_MASK);
}

/*
 * Get Interrupt Mask Level in CPU
 */
IMPORT INT GetCpuIntLevel( void )
{
	return (get_basepri() - INTPRI_MIN_UNIT) & INTPRI_MASK;
}

/*
 * Enable interrupt 
 *	Enables the interrupt specified in intno.
 *	External Interrupt can be specified. 
 */
EXPORT void EnableInt( UINT intno, INT level )
{
	UINT	imask;
	
	DI(imask);
	/* Set interrupt priority level. */
	*(_UB*)(NVIC_IPR(intno)) = (UB)level;
	/* Enables the specified interrupt. */
	*(_UW*)(NVIC_ISER(intno)) = (0x01U << (intno % 32));

	EI(imask);
}

/*
 * Disable interrupt 
 *	Disables the interrupt specified in intno.
 *	External Interrupt can be specified. 
 */
EXPORT void DisableInt( UINT intno )
{
	*(_UW*)(NVIC_ICER(intno)) = (0x01U << (intno % 32));
}

/*
 * Set-Pending
 *  Pends the associated interrupt under software control.
 *	External Interrupt can be specified. 
 */
EXPORT void SetPendingInt( UINT intno )
{
	*(_UW*)(NVIC_ISPR(intno)) = (0x01U << (intno % 32));
}

/*
 * Clear-Pending
 *	Un-pends the associated interrupt under software control.
 *	External Interrupt can be specified. 
 */
EXPORT void ClearPendingInt( UINT intno )
{
	*(_UW*)(NVIC_ICPR(intno)) = (0x01U << (intno % 32));
}

/*
 * Check active state
 *	Current active state for the associated interrupt
 *	External Interrupt can be specified. 
 */
EXPORT BOOL CheckInt( UINT intno )
{
	return (*(_UW*)(NVIC_ICPR(intno)) & (0x01U << (intno % 32)));
}
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
 *	isyscall.h
 *	System Call Int. Format Definition
 */

#ifndef _ISYSCALL_
#define _ISYSCALL_

/* ------------------------------------------------------------------------ */

#if TA_GP
# define _CALL(p1, p2, p3, hdr, cb)	\
		CallUserHandler((INT)(p1), (INT)(p2), (INT)(p3), (FP)(hdr), (cb)->gp)
# define CallUserHandlerP1(   p1,         hdr, cb)	_CALL(p1, 0,  0,  hdr, cb)
# define CallUserHandlerP2(   p1, p2,     hdr, cb)	_CALL(p1, p2, 0,  hdr, cb)
# define CallUserHandlerP3(   p1, p2, p3, hdr, cb)	_CALL(p1, p2, p3, hdr, cb)
# define CallUserHandlerP2_GP(p1, p2,     hdr, cb)	_CALL(p1, p2, gp, hdr, cb)
#else
# define CallUserHandlerP1(   p1,         hdr, cb)	(*(hdr))(p1)
# define CallUserHandlerP2(   p1, p2,     hdr, cb)	(*(hdr))(p1, p2)
# define CallUserHandlerP3(   p1, p2, p3, hdr, cb)	(*(hdr))(p1, p2, p3)
# define CallUserHandlerP2_GP(p1, p2,     hdr, cb)	(*(hdr))(p1, p2)
#endif

/* ------------------------------------------------------------------------ */

#if TA_GP
#define P0(void)		( int _1,int _2,int _3,int _4,int _5, void *gp )
#define P1(p1)			( p1,    int _2,int _3,int _4,int _5, void *gp )
#define P2(p1, p2)		( p1, p2,       int _3,int _4,int _5, void *gp )
#define P3(p1, p2, p3)		( p1, p2, p3,          int _4,int _5, void *gp )
#define P4(p1, p2, p3, p4)	( p1, p2, p3, p4,             int _5, void *gp )
#define P5(p1, p2, p3, p4, p5)	( p1, p2, p3, p4, p5,                 void *gp )
#define P2GP(p1, p2)		( p1, p2,			      void *gp )
#else
#define P0(void)		( void )
#define P1(p1)			( p1 )
#define P2(p1, p2)		( p1, p2 )
#define P3(p1, p2, p3)		( p1, p2, p3 )
#define P4(p1, p2, p3, p4)	( p1, p2, p3, p4 )
#define P5(p1, p2, p3, p4, p5)	( p1, p2, p3, p4, p5 )
#define P2GP(p1, p2)		( p1, p2 )
#endif

/* ------------------------------------------------------------------------ */

/* micro T-Kernel */
IMPORT ID tk_cre_tsk_impl P1( CONST T_CTSK *pk_ctsk );
IMPORT ER tk_del_tsk_impl( ID tskid );
IMPORT ER tk_sta_tsk_impl( ID tskid, INT stacd );
IMPORT void tk_ext_tsk_impl( void );
IMPORT void tk_exd_tsk_impl( void );
IMPORT ER tk_ter_tsk_impl( ID tskid );
IMPORT ER tk_dis_dsp_impl( void );
IMPORT ER tk_ena_dsp_impl( void );
IMPORT ER tk_chg_pri_impl( ID tskid, PRI tskpri );
IMPORT ER tk_rot_rdq_impl( PRI tskpri );
IMPORT ER tk_rel_wai_impl( ID tskid );
IMPORT ID tk_get_tid_impl( void );
IMPORT ER tk_get_reg_impl( ID tskid, T_REGS *pk_regs, T_EIT *pk_eit, T_CREGS *pk_cregs );
IMPORT ER tk_set_reg_impl( ID tskid, CONST T_REGS *pk_regs, CONST T_EIT *pk_eit, CONST T_CREGS *pk_cregs );
IMPORT ER tk_ref_tsk_impl( ID tskid, T_RTSK *pk_rtsk );
IMPORT ER tk_sus_tsk_impl( ID tskid );
IMPORT ER tk_rsm_tsk_impl( ID tskid );
IMPORT ER tk_frsm_tsk_impl( ID tskid );
IMPORT ER tk_slp_tsk_impl( TMO tmout );
IMPORT ER tk_wup_tsk_impl( ID tskid );
IMPORT INT tk_can_wup_impl( ID tskid );

#if CFN_MAX_SEMID > 0
IMPORT ID tk_cre_sem_impl( CONST T_CSEM *pk_csem );
IMPORT ER tk_del_sem_impl( ID semid );
IMPORT ER tk_sig_sem_impl( ID semid, INT cnt );
IMPORT ER tk_wai_sem_impl( ID semid, INT cnt, TMO tmout );
IMPORT ER tk_ref_sem_impl( ID semid, T_RSEM *pk_rsem );
#endif

#if CFN_MAX_MTXID > 0
IMPORT ID tk_cre_mtx_impl( CONST T_CMTX *pk_cmtx );
IMPORT ER tk_del_mtx_impl( ID mtxid );
IMPORT ER tk_loc_mtx_impl( ID mtxid, TMO tmout );
IMPORT ER tk_unl_mtx_impl( ID mtxid );
IMPORT ER tk_ref_mtx_impl( ID mtxid, T_RMTX *pk_rmtx );
#endif

#if CFN_MAX_FLGID > 0
IMPORT ID tk_cre_flg_impl( CONST T_CFLG *pk_cflg );
IMPORT ER tk_del_flg_impl( ID flgid );
IMPORT ER tk_set_flg_impl( ID flgid, UINT setptn );
IMPORT ER tk_clr_flg_impl( ID flgid, UINT clrptn );
IMPORT ER tk_wai_flg_impl( ID flgid, UINT waiptn, UINT wfmode, UINT *p_flgptn, TMO tmout );
IMPORT ER tk_ref_flg_impl( ID flgid, T_RFLG *pk_rflg );
#endif

#if CFN_MAX_MBXID > 0
IMPORT ID tk_cre_mbx_impl( CONST T_CMBX* pk_cmbx );
IMPORT ER tk_del_mbx_impl( ID mbxid );
IMPORT ER tk_snd_mbx_impl( ID mbxid, T_MSG *pk_msg );
IMPORT ER tk_rcv_mbx_impl( ID mbxid, T_MSG **ppk_msg, TMO tmout );
IMPORT ER tk_ref_mbx_impl( ID mbxid, T_RMBX *pk_rmbx );
#endif

#if CFN_MAX_MBFID > 0
IMPORT ID tk_cre_mbf_impl( CONST T_CMBF *pk_cmbf );
IMPORT ER tk_del_mbf_impl( ID mbfid );
IMPORT ER tk_snd_mbf_impl( ID mbfid, CONST void *msg, INT msgsz, TMO tmout );
IMPORT INT tk_rcv_mbf_impl( ID mbfid, void *msg, TMO tmout );
IMPORT ER tk_ref_mbf_impl( ID mbfid, T_RMBF *pk_rmbf );
#endif

#if CFN_MAX_PORID > 0
IMPORT ID tk_cre_por_impl( CONST T_CPOR *pk_cpor );
IMPORT ER tk_del_por_impl( ID porid );
IMPORT INT tk_cal_por_impl( ID porid, UINT calptn, void *msg, INT cmsgsz, TMO tmout );
IMPORT INT tk_acp_por_impl( ID porid, UINT acpptn, RNO *p_rdvno, void *msg, TMO tmout );
IMPORT ER tk_fwd_por_impl( ID porid, UINT calptn, RNO rdvno, CONST void *msg, INT cmsgsz );
IMPORT ER tk_rpl_rdv_impl( RNO rdvno, CONST void *msg, INT rmsgsz );
IMPORT ER tk_ref_por_impl( ID porid, T_RPOR *pk_rpor );
#endif

IMPORT ER tk_def_int_impl P2( UINT intno, CONST T_DINT *pk_dint );
IMPORT void tk_ret_int_impl( void );

#if CFN_MAX_MPLID > 0
IMPORT ID tk_cre_mpl_impl( CONST T_CMPL *pk_cmpl );
IMPORT ER tk_del_mpl_impl( ID mplid );
IMPORT ER tk_get_mpl_impl( ID mplid, SZ blksz, void **p_blk, TMO tmout );
IMPORT ER tk_rel_mpl_impl( ID mplid, void *blk );
IMPORT ER tk_ref_mpl_impl( ID mplid, T_RMPL *pk_rmpl );
#endif

#if CFN_MAX_MPFID > 0
IMPORT ID tk_cre_mpf_impl( CONST T_CMPF *pk_cmpf );
IMPORT ER tk_del_mpf_impl( ID mpfid );
IMPORT ER tk_get_mpf_impl( ID mpfid, void **p_blf, TMO tmout );
IMPORT ER tk_rel_mpf_impl( ID mpfid, void *blf );
IMPORT ER tk_ref_mpf_impl( ID mpfid, T_RMPF *pk_rmpf );
#endif

IMPORT ER tk_set_tim_impl( CONST SYSTIM *pk_tim );
IMPORT ER tk_get_tim_impl( SYSTIM *pk_tim );
IMPORT ER tk_get_otm_impl( SYSTIM *pk_tim );
IMPORT ER tk_dly_tsk_impl( RELTIM dlytim );

#if CFN_MAX_CYCID > 0
IMPORT ID tk_cre_cyc_impl P1( CONST T_CCYC *pk_ccyc );
IMPORT ER tk_del_cyc_impl( ID cycid );
IMPORT ER tk_sta_cyc_impl( ID cycid );
IMPORT ER tk_stp_cyc_impl( ID cycid );
IMPORT ER tk_ref_cyc_impl( ID cycid, T_RCYC *pk_rcyc );
#endif

#if CFN_MAX_ALMID > 0
IMPORT ID tk_cre_alm_impl P1( CONST T_CALM *pk_calm );
IMPORT ER tk_del_alm_impl( ID almid );
IMPORT ER tk_sta_alm_impl( ID almid, RELTIM almtim );
IMPORT ER tk_stp_alm_impl( ID almid );
IMPORT ER tk_ref_alm_impl( ID almid, T_RALM *pk_ralm );
#endif

IMPORT ER tk_ref_ver_impl( T_RVER *pk_rver );
IMPORT ER tk_ref_sys_impl( T_RSYS *pk_rsys );

#if CFN_MAX_SSYID > 0
IMPORT ER tk_def_ssy_impl P2( ID ssid, CONST T_DSSY *pk_dssy );
IMPORT ER tk_ref_ssy_impl( ID ssid, T_RSSY *pk_rssy );
#endif

#if CFN_MAX_REGDEV > 0
IMPORT ID tk_opn_dev_impl( CONST UB *devnm, UINT omode );
IMPORT ER tk_cls_dev_impl( ID dd, UINT option );
IMPORT ID tk_rea_dev_impl( ID dd, W start, void *buf, SZ size, TMO tmout );
IMPORT ER tk_srea_dev_impl( ID dd, W start, void *buf, SZ size, SZ *asize );
IMPORT ID tk_wri_dev_impl( ID dd, W start, CONST void *buf, SZ size, TMO tmout );
IMPORT ER tk_swri_dev_impl( ID dd, W start, CONST void *buf, SZ size, SZ *asize );
IMPORT ID tk_wai_dev_impl( ID dd, ID reqid, SZ *asize, ER *ioer, TMO tmout );
IMPORT INT tk_sus_dev_impl( UINT mode );
IMPORT ID tk_get_dev_impl( ID devid, UB *devnm );
IMPORT ID tk_ref_dev_impl( CONST UB *devnm, T_RDEV *rdev );
IMPORT ID tk_oref_de_implv( ID dd, T_RDEV *rdev );
IMPORT INT tk_lst_dev_impl( T_LDEV *ldev, INT start, INT ndev );
IMPORT INT tk_evt_dev_impl( ID devid, INT evttype, void *evtinf );
IMPORT ID tk_def_dev_impl P3( CONST UB *devnm, CONST T_DDEV *ddev, T_IDEV *idev );
IMPORT ER tk_ref_idv_impl( T_IDEV *idev );
#endif

#endif /* _ISYSCALL_ */
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
 *	isysconf.h
 *	Default Value Definition for System Configuration File
 */

#ifndef _ISYSCONF_
#define _ISYSCONF_

/*
 * Definition of unused system call
 */
#if _Csym == 0

#ifndef USE_FUNC_TK_CRE_SEM
#define tk_cre_sem_impl	knl_no_support
#endif
#ifndef USE_FUNC_TK_DEL_SEM
#define tk_del_sem_impl	knl_no_support
#endif
#ifndef USE_FUNC_TK_SIG_SEM
#define tk_sig_sem_impl	knl_no_support
#endif
#ifndef USE_FUNC_TK_WAI_SEM
#define tk_wai_sem_impl	knl_no_support
#endif
#ifndef USE_FUNC_TK_REF_SEM
#define tk_ref_sem_impl	knl_no_support
#endif
#ifndef USE_FUNC_TD_LST_SEM
#define td_lst_sem_impl	knl_no_support
#endif
#ifndef USE_FUNC_TD_REF_SEM
#define td_ref_sem_impl	knl_no_support
#endif
#ifndef USE_FUNC_TD_SEM_QUE
#define td_sem_que_impl	knl_no_support
#endif

#ifndef USE_FUNC_TK_CRE_FLG
#define tk_cre_flg_impl	knl_no_support
#endif
#ifndef USE_FUNC_TK_DEL_FLG
#define tk_del_flg_impl	knl_no_support
#endif
#ifndef USE_FUNC_TK_SET_FLG
#define tk_set_flg_impl	knl_no_support
#endif
#ifndef USE_FUNC_TK_CLR_FLG
#define tk_clr_flg_impl	knl_no_support
#endif
#ifndef USE_FUNC_TK_WAI_FLG
#define tk_wai_flg_impl	knl_no_support
#endif
#ifndef USE_FUNC_TK_REF_FLG
#define tk_ref_flg_impl	knl_no_support
#endif
#ifndef USE_FUNC_TD_LST_FLG
#define td_lst_flg_impl	knl_no_support
#endif
#ifndef USE_FUNC_TD_REF_FLG
#define td_ref_flg_impl	knl_no_support
#endif
#ifndef USE_FUNC_TD_FLG_QUE
#define td_flg_que_impl	knl_no_support
#endif

#ifndef USE_FUNC_TK_CRE_MBX
#define tk_cre_mbx_impl	knl_no_support
#endif
#ifndef USE_FUNC_TK_DEL_MBX
#define tk_del_mbx_impl	knl_no_support
#endif
#ifndef USE_FUNC_TK_SND_MBX
#define tk_snd_mbx_impl	knl_no_support
#endif
#ifndef USE_FUNC_TK_RCV_MBX
#define tk_rcv_mbx_impl	knl_no_support
#endif
#ifndef USE_FUNC_TK_REF_MBX
#define tk_ref_mbx_impl	knl_no_support
#endif
#ifndef USE_FUNC_TD_LST_MBX
#define td_lst_mbx_impl	knl_no_support
#endif
#ifndef USE_FUNC_TD_REF_MBX
#define td_ref_mbx_impl	knl_no_support
#endif
#ifndef USE_FUNC_TD_MBX_QUE
#define td_mbx_que_impl	knl_no_support
#endif

#ifndef USE_FUNC_TK_CRE_MBF
#define tk_cre_mbf_impl	knl_no_support
#endif
#ifndef USE_FUNC_TK_DEL_MBF
#define tk_del_mbf_impl	knl_no_support
#endif
#ifndef USE_FUNC_TK_SND_MBF
#define tk_snd_mbf_impl	knl_no_support
#endif
#ifndef USE_FUNC_TK_RCV_MBF
#define tk_rcv_mbf_impl	knl_no_support
#endif
#ifndef USE_FUNC_TK_REF_MBF
#define tk_ref_mbf_impl	knl_no_support
#endif
#ifndef USE_FUNC_TD_LST_MBF
#define td_lst_mbf_impl	knl_no_support
#endif
#ifndef USE_FUNC_TD_REF_MBF
#define td_ref_mbf_impl	knl_no_support
#endif
#ifndef USE_FUNC_TD_SMBF_QUE
#define td_smbf_que_impl	knl_no_support
#endif
#ifndef USE_FUNC_TD_RMBF_QUE
#define td_rmbf_que_impl	knl_no_support
#endif

#ifndef USE_FUNC_TK_CRE_POR
#define tk_cre_por_impl	knl_no_support
#endif
#ifndef USE_FUNC_TK_DEL_POR
#define tk_del_por_impl	knl_no_support
#endif
#ifndef USE_FUNC_TK_CAL_POR
#define tk_cal_por_impl	knl_no_support
#endif
#ifndef USE_FUNC_TK_ACP_POR
#define tk_acp_por_impl	knl_no_support
#endif
#ifndef USE_FUNC_TK_FWD_POR
#define tk_fwd_por_impl	knl_no_support
#endif
#ifndef USE_FUNC_TK_RPL_RDV
#define tk_rpl_rdv_impl	knl_no_support
#endif
#ifndef USE_FUNC_TK_REF_POR
#define tk_ref_por_impl	knl_no_support
#endif
#ifndef USE_FUNC_TD_LST_POR
#define td_lst_por_impl	knl_no_support
#endif
#ifndef USE_FUNC_TD_REF_POR
#define td_ref_por_impl	knl_no_support
#endif
#ifndef USE_FUNC_TD_ACP_QUE
#define td_acp_que_impl	knl_no_support
#endif
#ifndef USE_FUNC_TD_CAL_QUE
#define td_cal_que_impl	knl_no_support
#endif

#ifndef USE_FUNC_TK_CRE_MTX
#define tk_cre_mtx_impl	knl_no_support
#endif
#ifndef USE_FUNC_TK_DEL_MTX
#define tk_del_mtx_impl	knl_no_support
#endif
#ifndef USE_FUNC_TK_LOC_MTX
#define tk_loc_mtx_impl	knl_no_support
#endif
#ifndef USE_FUNC_TK_UNL_MTX
#define tk_unl_mtx_impl	knl_no_support
#endif
#ifndef USE_FUNC_TK_REF_MTX
#define tk_ref_mtx_impl	knl_no_support
#endif
#ifndef USE_FUNC_TD_LST_MTX
#define td_lst_mtx_impl	knl_no_support
#endif
#ifndef USE_FUNC_TD_REF_MTX
#define td_ref_mtx_impl	knl_no_support
#endif
#ifndef USE_FUNC_TD_MTX_QUE
#define td_mtx_que_impl	knl_no_support
#endif

#ifndef USE_FUNC_TK_CRE_MPL
#define tk_cre_mpl_impl	knl_no_support
#endif
#ifndef USE_FUNC_TK_DEL_MPL
#define tk_del_mpl_impl	knl_no_support
#endif
#ifndef USE_FUNC_TK_GET_MPL
#define tk_get_mpl_impl	knl_no_support
#endif
#ifndef USE_FUNC_TK_REL_MPL
#define tk_rel_mpl_impl	knl_no_support
#endif
#ifndef USE_FUNC_TK_REF_MPL
#define tk_ref_mpl_impl	knl_no_support
#endif
#ifndef USE_FUNC_TD_LST_MPL
#define td_lst_mpl_impl	knl_no_support
#endif
#ifndef USE_FUNC_TD_REF_MPL
#define td_ref_mpl_impl	knl_no_support
#endif
#ifndef USE_FUNC_TD_MPL_QUE
#define td_mpl_que_impl	knl_no_support
#endif

#ifndef USE_FUNC_TK_CRE_MPF
#define tk_cre_mpf_impl	knl_no_support
#endif
#ifndef USE_FUNC_TK_DEL_MPF
#define tk_del_mpf_impl	knl_no_support
#endif
#ifndef USE_FUNC_TK_GET_MPF
#define tk_get_mpf_impl	knl_no_support
#endif
#ifndef USE_FUNC_TK_REL_MPF
#define tk_rel_mpf_impl	knl_no_support
#endif
#ifndef USE_FUNC_TK_REF_MPF
#define tk_ref_mpf_impl	knl_no_support
#endif
#ifndef USE_FUNC_TD_LST_MPF
#define td_lst_mpf_impl	knl_no_support
#endif
#ifndef USE_FUNC_TD_REF_MPF
#define td_ref_mpf_impl	knl_no_support
#endif
#ifndef USE_FUNC_TD_MPF_QUE
#define td_mpf_que_impl	knl_no_support
#endif

#ifndef USE_FUNC_TK_CRE_CYC
#define tk_cre_cyc_impl	knl_no_support
#endif
#ifndef USE_FUNC_TK_DEL_CYC
#define tk_del_cyc_impl	knl_no_support
#endif
#ifndef USE_FUNC_TK_STA_CYC
#define tk_sta_cyc_impl	knl_no_support
#endif
#ifndef USE_FUNC_TK_STP_CYC
#define tk_stp_cyc_impl	knl_no_support
#endif
#ifndef USE_FUNC_TK_REF_CYC
#define tk_ref_cyc_impl	knl_no_support
#endif
#ifndef USE_FUNC_TD_LST_CYC
#define td_lst_cyc_impl	knl_no_support
#endif
#ifndef USE_FUNC_TD_REF_CYC
#define td_ref_cyc_impl	knl_no_support
#endif

#ifndef USE_FUNC_TK_CRE_ALM
#define tk_cre_alm_impl	knl_no_support
#endif
#ifndef USE_FUNC_TK_DEL_ALM
#define tk_del_alm_impl	knl_no_support
#endif
#ifndef USE_FUNC_TK_STA_ALM
#define tk_sta_alm_impl	knl_no_support
#endif
#ifndef USE_FUNC_TK_STP_ALM
#define tk_stp_alm_impl	knl_no_support
#endif
#ifndef USE_FUNC_TK_REF_ALM
#define tk_ref_alm_impl	knl_no_support
#endif
#ifndef USE_FUNC_TD_LST_ALM
#define td_lst_alm_impl	knl_no_support
#endif
#ifndef USE_FUNC_TD_REF_ALM
#define td_ref_alm_impl	knl_no_support
#endif

#ifndef USE_FUNC_TK_DEF_SSY
#define tk_def_ssy_impl	knl_no_support
#endif
#ifndef USE_FUNC_TK_REF_SSY
#define tk_ref_ssy_impl	knl_no_support
#endif
#ifndef USE_FUNC_TD_LST_SSY
#define td_lst_ssy_impl	knl_no_support
#endif
#ifndef USE_FUNC_TD_REF_SSY
#define td_ref_ssy_impl	knl_no_support
#endif

#ifndef USE_FUNC_TK_OPN_DEV
#define tk_opn_dev_impl	knl_no_support
#endif
#ifndef USE_FUNC_TK_CLS_DEV
#define tk_cls_dev_impl	knl_no_support
#endif
#ifndef USE_FUNC_TK_REA_DEV
#define tk_rea_dev_impl	knl_no_support
#endif
#ifndef USE_FUNC_TK_SREA_DEV
#define tk_srea_dev_impl	knl_no_support
#endif
#ifndef USE_FUNC_TK_WRI_DEV
#define tk_wri_dev_impl	knl_no_support
#endif
#ifndef USE_FUNC_TK_SWRI_DEV
#define tk_swri_dev_impl	knl_no_support
#endif
#ifndef USE_FUNC_TK_WAI_DEV
#define tk_wai_dev_impl	knl_no_support
#endif
#ifndef USE_FUNC_TK_SUS_DEV
#define tk_sus_dev_impl	knl_no_support
#endif
#ifndef USE_FUNC_TK_GET_DEV
#define tk_get_dev_impl	knl_no_support
#endif
#ifndef USE_FUNC_TK_REF_DEV
#define tk_ref_dev_impl	knl_no_support
#endif
#ifndef USE_FUNC_TK_OREF_DEV
#define tk_oref_dev_impl	knl_no_support
#endif
#ifndef USE_FUNC_TK_LST_DEV
#define tk_lst_dev_impl	knl_no_support
#endif
#ifndef USE_FUNC_TK_EVT_DEV
#define tk_evt_dev_impl	knl_no_support
#endif
#ifndef USE_FUNC_TK_DEF_DEV
#define tk_def_dev_impl	knl_no_support
#endif
#ifndef USE_FUNC_TK_REF_IDV
#define tk_ref_idv_impl	knl_no_support
#endif

#ifndef USE_FUNC_TD_HOK_SVC
#define td_hok_svc_impl	knl_no_support
#endif
#ifndef USE_FUNC_TD_HOK_DSP
#define td_hok_dsp_impl	knl_no_support
#endif
#ifndef USE_FUNC_TD_HOK_INT
#define td_hok_int_impl	knl_no_support
#endif

#else /* _Csym == 1 */

#ifndef USE_FUNC_TK_CRE_SEM
#define _tk_cre_sem_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TK_DEL_SEM
#define _tk_del_sem_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TK_SIG_SEM
#define _tk_sig_sem_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TK_WAI_SEM
#define _tk_wai_sem_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TK_REF_SEM
#define _tk_ref_sem_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TD_LST_SEM
#define _td_lst_sem_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TD_REF_SEM
#define _td_ref_sem_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TD_SEM_QUE
#define _td_sem_que_impl	_knl_no_support
#endif

#ifndef USE_FUNC_TK_CRE_FLG
#define _tk_cre_flg_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TK_DEL_FLG
#define _tk_del_flg_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TK_SET_FLG
#define _tk_set_flg_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TK_CLR_FLG
#define _tk_clr_flg_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TK_WAI_FLG
#define _tk_wai_flg_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TK_REF_FLG
#define _tk_ref_flg_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TD_LST_FLG
#define _td_lst_flg_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TD_REF_FLG
#define _td_ref_flg_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TD_FLG_QUE
#define _td_flg_que_impl	_knl_no_support
#endif

#ifndef USE_FUNC_TK_CRE_MBX
#define _tk_cre_mbx_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TK_DEL_MBX
#define _tk_del_mbx_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TK_SND_MBX
#define _tk_snd_mbx_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TK_RCV_MBX
#define _tk_rcv_mbx_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TK_REF_MBX
#define _tk_ref_mbx_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TD_LST_MBX
#define _td_lst_mbx_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TD_REF_MBX
#define _td_ref_mbx_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TD_MBX_QUE
#define _td_mbx_que_impl	_knl_no_support
#endif

#ifndef USE_FUNC_TK_CRE_MBF
#define _tk_cre_mbf_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TK_DEL_MBF
#define _tk_del_mbf_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TK_SND_MBF
#define _tk_snd_mbf_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TK_RCV_MBF
#define _tk_rcv_mbf_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TK_REF_MBF
#define _tk_ref_mbf_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TD_LST_MBF
#define _td_lst_mbf_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TD_REF_MBF
#define _td_ref_mbf_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TD_SMBF_QUE
#define _td_smbf_que_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TD_RMBF_QUE
#define _td_rmbf_que_impl	_knl_no_support
#endif

#ifndef USE_FUNC_TK_CRE_POR
#define _tk_cre_por_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TK_DEL_POR
#define _tk_del_por_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TK_CAL_POR
#define _tk_cal_por_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TK_ACP_POR
#define _tk_acp_por_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TK_FWD_POR
#define _tk_fwd_por_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TK_RPL_RDV
#define _tk_rpl_rdv_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TK_REF_POR
#define _tk_ref_por_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TD_LST_POR
#define _td_lst_por_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TD_REF_POR
#define _td_ref_por_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TD_ACP_QUE
#define _td_acp_que_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TD_CAL_QUE
#define _td_cal_que_impl	_knl_no_support
#endif

#ifndef USE_FUNC_TK_CRE_MTX
#define _tk_cre_mtx_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TK_DEL_MTX
#define _tk_del_mtx_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TK_LOC_MTX
#define _tk_loc_mtx_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TK_UNL_MTX
#define _tk_unl_mtx_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TK_REF_MTX
#define _tk_ref_mtx_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TD_LST_MTX
#define _td_lst_mtx_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TD_REF_MTX
#define _td_ref_mtx_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TD_MTX_QUE
#define _td_mtx_que_impl	_knl_no_support
#endif

#ifndef USE_FUNC_TK_CRE_MPL
#define _tk_cre_mpl_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TK_DEL_MPL
#define _tk_del_mpl_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TK_GET_MPL
#define _tk_get_mpl_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TK_REL_MPL
#define _tk_rel_mpl_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TK_REF_MPL
#define _tk_ref_mpl_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TD_LST_MPL
#define _td_lst_mpl_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TD_REF_MPL
#define _td_ref_mpl_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TD_MPL_QUE
#define _td_mpl_que_impl	_knl_no_support
#endif

#ifndef USE_FUNC_TK_CRE_MPF
#define _tk_cre_mpf_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TK_DEL_MPF
#define _tk_del_mpf_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TK_GET_MPF
#define _tk_get_mpf_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TK_REL_MPF
#define _tk_rel_mpf_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TK_REF_MPF
#define _tk_ref_mpf_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TD_LST_MPF
#define _td_lst_mpf_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TD_REF_MPF
#define _td_ref_mpf_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TD_MPF_QUE
#define _td_mpf_que_impl	_knl_no_support
#endif

#ifndef USE_FUNC_TK_CRE_CYC
#define _tk_cre_cyc_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TK_DEL_CYC
#define _tk_del_cyc_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TK_STA_CYC
#define _tk_sta_cyc_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TK_STP_CYC
#define _tk_stp_cyc_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TK_REF_CYC
#define _tk_ref_cyc_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TD_LST_CYC
#define _td_lst_cyc_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TD_REF_CYC
#define _td_ref_cyc_impl	_knl_no_support
#endif

#ifndef USE_FUNC_TK_CRE_ALM
#define _tk_cre_alm_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TK_DEL_ALM
#define _tk_del_alm_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TK_STA_ALM
#define _tk_sta_alm_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TK_STP_ALM
#define _tk_stp_alm_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TK_REF_ALM
#define _tk_ref_alm_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TD_LST_ALM
#define _td_lst_alm_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TD_REF_ALM
#define _td_ref_alm_impl	_knl_no_support
#endif

#ifndef USE_FUNC_TK_DEF_SSY
#define _tk_def_ssy_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TK_REF_SSY
#define _tk_ref_ssy_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TD_LST_SSY
#define _td_lst_ssy_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TD_REF_SSY
#define _td_ref_ssy_impl	_knl_no_support
#endif

#ifndef USE_FUNC_TK_OPN_DEV
#define _tk_opn_dev_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TK_CLS_DEV
#define _tk_cls_dev_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TK_REA_DEV
#define _tk_rea_dev_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TK_SREA_DEV
#define _tk_srea_dev_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TK_WRI_DEV
#define _tk_wri_dev_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TK_SWRI_DEV
#define _tk_swri_dev_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TK_WAI_DEV
#define _tk_wai_dev_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TK_SUS_DEV
#define _tk_sus_dev_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TK_GET_DEV
#define _tk_get_dev_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TK_REF_DEV
#define _tk_ref_dev_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TK_OREF_DEV
#define _tk_oref_dev_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TK_LST_DEV
#define _tk_lst_dev_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TK_EVT_DEV
#define _tk_evt_dev_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TK_DEF_DEV
#define _tk_def_dev_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TK_REF_IDV
#define _tk_ref_idv_impl	_knl_no_support
#endif

#ifndef USE_FUNC_TD_HOK_SVC
#define _td_hok_svc_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TD_HOK_DSP
#define _td_hok_dsp_impl	_knl_no_support
#endif
#ifndef USE_FUNC_TD_HOK_INT
#define _td_hok_int_impl	_knl_no_support
#endif

#endif /* _Csym == 0 */

#endif /* _ISYSCONF_ */

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
 *	kernel.h
 *	micro T-Kernel Common Definition
 */

#ifndef _KERNEL_
#define _KERNEL_

#include <basic.h>
#include <tk_typedef.h>
#include <errno.h>
#include <syscall.h>
#include <dbgspt.h>
#include <imalloc.h>
#include <queue.h>
#include <libstr.h>

#include "utk_config.h"

#include "isyscall.h"

#define SYSCALL		EXPORT		/* Definition of system call */

/*
 * Kernel configuration file
 */
#include "config.h"
#include "cpu_conf.h"
#include "tkdev_conf.h"

#ifndef __tcb__
#define __tcb__
typedef struct task_control_block	TCB;
#endif

/*
 * Object lock
 *	Locked task is the highest run priority.
 *	Unable to nest lock.
 */
typedef struct objlock {
	QUEUE		wtskq;		/* Wait task queue */
} OBJLOCK;

void knl_InitOBJLOCK( OBJLOCK *loc )
{
	loc->wtskq.next = NULL;
}
IMPORT void knl_LockOBJ( OBJLOCK* );
IMPORT void knl_UnlockOBJ( OBJLOCK* );

BOOL knl_isLockedOBJ( OBJLOCK *loc )
{
	return ( loc->wtskq.next != NULL )? TRUE: FALSE;
}


/*
 * CPU-dependent include file
 */
#include "cpu_status.h"

/*
 * System initialization (each module)
 */
IMPORT ER knl_task_initialize( void );
IMPORT ER knl_semaphore_initialize( void );
IMPORT ER knl_eventflag_initialize( void );
IMPORT ER knl_mailbox_initialize( void );
IMPORT ER knl_messagebuffer_initialize( void );
IMPORT ER knl_rendezvous_initialize( void );
IMPORT ER knl_mutex_initialize( void );
IMPORT ER knl_memorypool_initialize( void );
IMPORT ER knl_fix_memorypool_initialize( void );
IMPORT ER knl_cyclichandler_initialize( void );
IMPORT ER knl_alarmhandler_initialize( void );
IMPORT ER knl_subsystem_initialize( void );
IMPORT ER knl_devicemanager_initialize( void );

/*
 * Start/Exit system (tkstart.c)
 */
IMPORT void knl_t_kernel_main( T_CTSK* );
IMPORT void knl_t_kernel_exit( void );

/*
 * Target system-dependent routine (cpu_init.c tkdev_init.c)
 */
IMPORT ER   knl_cpu_initialize( void );
IMPORT void knl_cpu_shutdown( void );
IMPORT ER   knl_tkdev_initialize( void );
IMPORT void knl_tkdev_exit( void );

/*
 * Mutex
 */
IMPORT void knl_signal_all_mutex( TCB *tcb );
IMPORT INT knl_chg_pri_mutex( TCB *tcb, INT priority );

#endif /* _KERNEL_ */
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
 *	klock.c
 *	Kernel Lock
 */

#include "kernel.h"
#include "task.h"
#include "ready_queue.h"

/*
 * Object lock
 *	Do not call from critical section
 */
EXPORT void knl_LockOBJ( OBJLOCK *loc )
{
	BOOL	klocked;

  retry:
	BEGIN_CRITICAL_SECTION;
	klocked = knl_ctxtsk->klocked;
	if ( !klocked ) {
		if ( loc->wtskq.next == NULL ) {
			/* Lock */
			QueInit(&loc->wtskq);

			knl_ctxtsk->klocked = klocked = TRUE;
			knl_ready_queue.klocktsk = knl_ctxtsk;
		} else {
			/* Ready for lock */
			knl_ready_queue_delete(&knl_ready_queue, knl_ctxtsk);
			knl_ctxtsk->klockwait = TRUE;
			QueInsert(&knl_ctxtsk->tskque, &loc->wtskq);

			knl_schedtsk = knl_ready_queue_top(&knl_ready_queue);
			knl_dispatch_request();
		}
	}
	END_CRITICAL_SECTION;
	/* Since wait could be freed without getting lock, 
	   need to re-try if lock is not got */
	if ( !klocked ) {
		goto retry;
	}
}

/*
 * Object unlock
 *	It may be called from a critical section.
 */
EXPORT void knl_UnlockOBJ( OBJLOCK *loc )
{
	TCB	*tcb;

	BEGIN_CRITICAL_SECTION;
	knl_ctxtsk->klocked = FALSE;
	knl_ready_queue.klocktsk = NULL;

	tcb = (TCB*)QueRemoveNext(&loc->wtskq);
	if ( tcb == NULL ) {
		/* Free lock */
		loc->wtskq.next = NULL;
	} else {
		/* Wake lock wait task */
		tcb->klockwait = FALSE;
		tcb->klocked = TRUE;
		knl_ready_queue_insert_top(&knl_ready_queue, tcb);
	}

	knl_schedtsk = knl_ready_queue_top(&knl_ready_queue);
	if ( knl_ctxtsk != knl_schedtsk ) {
		knl_dispatch_request();
	}
	END_CRITICAL_SECTION;
}
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
 *	@(#)libstr.h
 *
 *	Standard library for kernel link
 *
 */

#ifndef	__LIBSTR_H__
#define __LIBSTR_H__

#include <stdtype.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef	__size_t
typedef __size_t	size_t;
#undef	__size_t
#endif

#ifdef	__wchar_t
typedef __wchar_t	wchar_t;
#undef	__wchar_t
#endif

#define NULL		0

extern void* memset( void *s, int c, size_t n );
extern int memcmp( const void *s1, const void *s2, size_t n );
extern void* memcpy( void *dst, const void *src, size_t n );
extern void* memmove( void *dst, const void *src, size_t n );
extern void bzero( void *s, size_t n );

extern size_t strlen( const char *s );
extern int strcmp( const char *s1, const char *s2 );
extern int strncmp( const char *s1, const char *s2, size_t n );
extern char* strcpy( char *dst, const char *src );
extern char* strncpy( char *dst, const char *src, size_t n );
extern char* strcat( char *dst, const char *src );
extern char* strncat( char *dst, const char *src, size_t n );

extern long int strtol( const char *nptr, char **endptr, int base );

#ifdef __cplusplus
}
#endif
#endif /* __LIBSTR_H__ */
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

#ifndef _LIBSTR_CONFIG_H_
#define _LIBSTR_CONFIG_H_

#define USE_FUNC_MEMSET
#define USE_FUNC_MEMCMP
#define USE_FUNC_MEMCPY
#define USE_FUNC_MEMMOVE
#define USE_FUNC_BZERO
#define USE_FUNC_STRLEN
#define USE_FUNC_STRCMP
#define USE_FUNC_STRNCMP
#define USE_FUNC_STRCPY
#define USE_FUNC_STRNCPY
#define USE_FUNC_STRCAT
#define USE_FUNC_STRNCAT
#define USE_FUNC_STRTOL


#endif /* _LIBSTR_CONFIG_H_ */
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

#ifndef _LIBTK_CONFIG_H_
#define _LIBTK_CONFIG_H_

#define USE_FUNC_LOCK
#define USE_FUNC_UNLOCK
#define USE_FUNC_CREATELOCK
#define USE_FUNC_DELETELOCK
#define USE_FUNC_MLOCKTMO
#define USE_FUNC_MLOCK
#define USE_FUNC_MUNLOCK
#define USE_FUNC_CREATEMLOCK
#define USE_FUNC_DELETEMLOCK


#endif /* _LIBTK_CONFIG_H_ */
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
 *	limits.h
 *
 *	C language: integer size 
 */

#ifndef _LIMITS_
#define _LIMITS_


#define CHAR_BIT	(8)
#define SCHAR_MIN	(-128)
#define SCHAR_MAX	(+127)
#define UCHAR_MAX	(255)
#define CHAR_MIN	SCHAR_MIN
#define CHAR_MAX	SCHAR_MAX
#define MB_LEN_MAX	(2)

#define SHRT_MIN	(-32768)
#define SHRT_MAX	(+32767)
#define USHRT_MAX	(65535)

#define LONG_MIN	(-2147483648L)
#define LONG_MAX	(+2147483647L)
#define ULONG_MAX	(4294967295L)


#if INT_BITWIDTH == 16

#define INT_MIN		SHRT_MIN
#define INT_MAX		SHRT_MAX
#define UINT_MAX	USHRT_MAX

#else /* 32bit */

#define INT_MIN		LONG_MIN
#define INT_MAX		LONG_MAX
#define UINT_MAX	ULONG_MAX

#endif /* INT_BITWIDTH == 16 */
#endif /* _LIMITS_ */
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
 *	longlong.h
 *
 *	64 bit length integer 
 */

#ifndef _LONGLONG_
#define _LONGLONG_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __GNUC__

typedef long long	longlong;

#define ltoll(a)	( (longlong)(a) )
#define ultoll(a)	( (longlong)(a) )
#define uitoll(a)	( (longlong)(a) )
#define lltol(a)	( (long)(a) )
#define lltoul(a)	( (unsigned long)(a) )
#define ll_add(a,b)	( (a) + (b) )
#define ll_sub(a,b)	( (a) - (b) )
#define ll_mul(a,b)	( (a) * (b) )
#define li_mul(a,b)	( (a) * (b) )
#define lui_mul(a,b)	( (a) * (b) )
#define ll_div(a,b)	( (a) / (b) )
#define li_div(a,b)	( (a) / (b) )
#define lui_div(a,b)	( (a) / (b) )
#define ll_mod(a,b)	( (a) % (b) )
#define li_mod(a,b)	( (a) % (b) )
#define lui_mod(a,b)	( (a) % (b) )
#define ll_cmp(a,b)	( (a) - (b) )	/* +:a>b,0:a=b,-:a<b */
#define ll_sign(a)	( (a) )		/* +:a>0,0:a=0,-:a<0 */
#define ll_neg(a)	( -(a) )
#define ll_inc(a)	( (*(a))++ )
#define ll_dec(a)	( (*(a))-- )

#define hilo_ll(ll, h, l)	( (ll) = ((longlong)(h) << 32) | (l) )
#define ll_hilo(h, l, ll)	( (h) = (long)((ll) >> 32), \
				  (l) = (unsigned long)(ll) )

#else /* __GNUC__ */

typedef struct {
	long		hi;
	unsigned long	lo;
} longlong;

extern longlong	ltoll( long a );			/* (longlong)a */
extern longlong	ultoll( unsigned long a );		/* (longlong)a */
extern longlong	uitoll( UINT a );			/* (longlong)a */
extern long	lltol( longlong a );			/* (long)a */
extern unsigned long	lltoul( longlong a );	/* (unsigned long)a */
extern longlong	ll_add( longlong a, longlong b );	/* a+b */
extern longlong	ll_sub( longlong a, longlong b );	/* a-b */
extern longlong	ll_mul( longlong a, longlong b );	/* a*b */
extern longlong	li_mul( longlong a, long b );		/* a*b */
extern longlong	lui_mul( longlong a, UINT b );		/* a*b */
extern longlong	ll_div( longlong a, longlong b );	/* a/b */
extern longlong	li_div( longlong a, long b );		/* a/b */
extern longlong	lui_div( longlong a, UINT b );		/* a/b */
extern longlong	ll_mod( longlong a, longlong b );	/* a%b */
extern long	li_mod( longlong a, long b );		/* a%b */
extern long	lui_mod( longlong a, UINT b );		/* a%b */
extern int	ll_cmp( longlong a, longlong b );	/* +:a>b,0:a=b,-:a<b */
extern int	ll_sign( longlong a );			/* +:a>0,0:a=0,-:a<0 */
extern longlong ll_neg( longlong a );			/* -a */
extern void	ll_inc( longlong *a );			/* (*a)++ */
extern void	ll_dec( longlong *a );			/* (*a)-- */

#define hilo_ll(ll, h, l)	( (ll).hi = (h), (ll).lo = (l) )
#define ll_hilo(h, l, ll)	( (h) = (ll).hi, (l) = (ll).lo )

#endif /* __GNUC__ */

#ifdef __cplusplus
}
#endif
#endif /* _LONGLONG_ */
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
 *	@(#)machine.h
 *
 *	Machine type definition 
 *
 *	Describes differences between platforms, such as hardware
 *	and OS. Where the source code must be differentiated
 *	according to platform, macros and other definitions given
 *	here should be used.
 *
 *	Where possible, classifications should be based on general
 *	attributes rather than the machine type. Thus, the endian,
 *	for instance, should be described as BIGENDIAN rather than
 *	using a classification based on the machine type. 
 *
 *	* Machine type definition is not used solely by the machine
 *	C language source file, so non-macro definitions are not
 *	permitted.
 */

#ifndef __MACHINE_H__
#define __MACHINE_H__

/* ===== System dependencies definitions ================================ */

#include <machine_common.h>

/* ===== Common definitions ============================================= */

#ifndef Inline
#ifdef __cplusplus
#define Inline		inline
#else
#define Inline		static __inline__
#endif
#endif

#ifndef Asm
#ifdef __GNUC__
#define Asm		__asm__ volatile
#endif
#endif

/*
 * C symbol format 
 *	_Csym = 0	do not append _  
 *	_Csym = 1	append _  
 *
 *	* In the UNIX System V Release 4 C compiler,
 *	   _ is not appended to symbols.
 */
// #if _Csym == 0
// #define _sym	sym
// #else
// #define _sym	_##sym
// #endif


/*
 * No initialization section
 */
#ifdef __GNUC__
#define Noinit(decl)	decl __attribute__ ((section (".noinit")))
#else
#define Noinit(decl)	decl
#endif


#endif /* __MACHINE_H__ */
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
 *	@(#)machine_common.h (sys)
 *
 *	Machine type definition
 */

#ifndef __SYS_MACHINE_COMMON_H__
#define __SYS_MACHINE_COMMON_H__

//#ifdef _APP_AT91_
//#include <machine_depend.h>
//#endif

//#ifdef _APP_H8S2212_
//#include <machine_depend.h>
//#endif

//#ifdef _APP_MB9AF312K_
//#include <machine_depend.h>
//#endif

#include <machine_depend.h>

#endif /* __SYS_MACHINE_COMMON_H__ */
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
 *	@(#)machine_depend.h (FM3)
 *
 *	Machine type definition (system dependent)
 */

#ifndef __SYS_MACHINE_DEPEND_H__
#define __SYS_MACHINE_DEPEND_H__

/*
 * CPU_xxxx		CPU type
 * ALLOW_MISALIGN	1 if access to misalignment data is allowed
 * BIGENDIAN		1 if big endian
 * VIRTUAL_ADDRESS	1 if virtual memory
 * ALLOCA_NOSPT		1 if alloca() is not supported
 */

/* ----- FM3 (ARM Cortex-M3) definition ----- */
//#undef _APP_MB9AF312K_

//#define	_APP_MB9AF312K_		1

//#define CPU_ARMV7_M		1
//#define CPU_ARM_CORTEX_M3	1
//#define CPU_MB9AF		1
#define _APP_RL78G13_R5F100ADASP_		1
#define ALLOW_MISALIGN		0
#define BIGENDIAN		0
#define VIRTUAL_ADDRESS		0
//#define ALLOCA_NOSPT		0
#define INT_BITWIDTH		32

//#ifndef _Csym
// #define _Csym			1
//#endif

#endif /* __SYS_MACHINE_DEPEND_H__ */
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
 *	mailbox.c
 *	Mailbox
 */

/** [BEGIN Common Definitions] */
#include "kernel.h"
#include "task.h"
#include "wait.h"
#include "check.h"
#include "mailbox.h"
/** [END Common Definitions] */

#if CFN_MAX_MBXID > 0

#ifdef USE_FUNC_MBXCB_TABLE
Noinit(EXPORT MBXCB	knl_mbxcb_table[NUM_MBXID]);	/* Mailbox control block */
Noinit(EXPORT QUEUE	knl_free_mbxcb);	/* FreeQue */
#endif /* USE_FUNC_MBXCB_TABLE */


#ifdef USE_FUNC_MAILBOX_INITIALIZE
/*
 * Initialization of mailbox control block 
 */
EXPORT ER knl_mailbox_initialize( void )
{
	MBXCB	*mbxcb, *end;

	/* Get system information */
	if ( NUM_MBXID < 1 ) {
		return E_SYS;
	}

	/* Register all control blocks onto FreeQue */
	QueInit(&knl_free_mbxcb);
	end = knl_mbxcb_table + NUM_MBXID;
	for ( mbxcb = knl_mbxcb_table; mbxcb < end; mbxcb++ ) {
		mbxcb->mbxid = 0;
		QueInsert(&mbxcb->wait_queue, &knl_free_mbxcb);
	}

	return E_OK;
}
#endif /* USE_FUNC_MAILBOX_INITIALIZE */


#ifdef USE_FUNC_TK_CRE_MBX
/*
 * Create mailbox
 */
SYSCALL ID tk_cre_mbx_impl( CONST T_CMBX *pk_cmbx )
{
#if CHK_RSATR
	const ATR VALID_MBXATR = {
		 TA_MPRI
		|TA_TPRI
#if USE_OBJECT_NAME
		|TA_DSNAME
#endif
	};
#endif
	MBXCB	*mbxcb;
	ID	mbxid;
	ER	ercd;

	CHECK_RSATR(pk_cmbx->mbxatr, VALID_MBXATR);

	BEGIN_CRITICAL_SECTION;
	/* Get control block from FreeQue */
	mbxcb = (MBXCB*)QueRemoveNext(&knl_free_mbxcb);
	if ( mbxcb == NULL ) {
		ercd = E_LIMIT;
	} else {
		mbxid = ID_MBX(mbxcb - knl_mbxcb_table);

		/* Initialize control block */
		QueInit(&mbxcb->wait_queue);
		mbxcb->mbxid  = mbxid;
		mbxcb->exinf  = pk_cmbx->exinf;
		mbxcb->mbxatr = pk_cmbx->mbxatr;
		mbxcb->mq_head.msgque[0] = NULL;
#if USE_OBJECT_NAME
		if ( (pk_cmbx->mbxatr & TA_DSNAME) != 0 ) {
			strncpy((char*)mbxcb->name, (char*)pk_cmbx->dsname,
				OBJECT_NAME_LENGTH);
		}
#endif
		ercd = mbxid;
	}
	END_CRITICAL_SECTION;

	return ercd;
}
#endif /* USE_FUNC_TK_CRE_MBX */

#ifdef USE_FUNC_TK_DEL_MBX
/*
 * Delete mailbox
 */
SYSCALL ER tk_del_mbx_impl( ID mbxid )
{
	MBXCB	*mbxcb;
	ER	ercd = E_OK;

	CHECK_MBXID(mbxid);

	mbxcb = get_mbxcb(mbxid);

	BEGIN_CRITICAL_SECTION;
	if ( mbxcb->mbxid == 0 ) {
		ercd = E_NOEXS;
	} else {
		/* Release wait state of task (E_DLT) */
		knl_wait_delete(&mbxcb->wait_queue);

		/* Return to FreeQue */
		QueInsert(&mbxcb->wait_queue, &knl_free_mbxcb);
		mbxcb->mbxid = 0;
	}
	END_CRITICAL_SECTION;

	return ercd;
}
#endif /* USE_FUNC_TK_DEL_MBX */

#ifdef USE_FUNC_TK_SND_MBX
/*
 * Send to mailbox
 */
SYSCALL ER tk_snd_mbx_impl( ID mbxid, T_MSG *pk_msg )
{
	MBXCB	*mbxcb;
	TCB	*tcb;
	ER	ercd = E_OK;

	CHECK_MBXID(mbxid);

	mbxcb = get_mbxcb(mbxid);

	BEGIN_CRITICAL_SECTION;
	if (mbxcb->mbxid == 0) {
		ercd = E_NOEXS;
		goto error_exit;
	}

	if ( (mbxcb->mbxatr & TA_MPRI) != 0 ) {
		if ( ((T_MSG_PRI*)pk_msg)->msgpri <= 0 ) {
			ercd = E_PAR;
			goto error_exit;
		}
	}

	if ( !isQueEmpty(&mbxcb->wait_queue) ) {
		/* Directly send to receive wait task */
		tcb = (TCB*)(mbxcb->wait_queue.next);
		*tcb->winfo.mbx.ppk_msg = pk_msg;
		knl_wait_release_ok(tcb);

	} else {
		/* Connect message to queue */
		if ( (mbxcb->mbxatr & TA_MPRI) != 0 ) {
			/* Connect message to queue following priority */
			knl_queue_insert_mpri((T_MSG_PRI*)pk_msg, &mbxcb->mq_head);
		} else {
			/* Connect to end of queue */
			nextmsg(pk_msg) = NULL;
			if ( headmsg(mbxcb) == NULL ) {
				headmsg(mbxcb) = pk_msg;
			} else {
				nextmsg(mbxcb->mq_tail) = pk_msg;
			}
			mbxcb->mq_tail = pk_msg;
		}
	}

    error_exit:
	END_CRITICAL_SECTION;

	return ercd;
}
#endif /* USE_FUNC_TK_SND_MBX */

#ifdef USE_FUNC_TK_RCV_MBX
/*
 * Processing if the priority of wait task changes
 */
LOCAL void mbx_chg_pri( TCB *tcb, INT oldpri )
{
	MBXCB	*mbxcb;

	mbxcb = get_mbxcb(tcb->wid);
	knl_gcb_change_priority((GCB*)mbxcb, tcb);
}

/*
 * Definition of mailbox wait specification
 */
LOCAL CONST WSPEC knl_wspec_mbx_tfifo = { TTW_MBX, NULL, NULL };
LOCAL CONST WSPEC knl_wspec_mbx_tpri  = { TTW_MBX, mbx_chg_pri, NULL };

/*
 * Receive from mailbox
 */
SYSCALL ER tk_rcv_mbx_impl( ID mbxid, T_MSG **ppk_msg, TMO tmout )
{
	MBXCB	*mbxcb;
	ER	ercd = E_OK;

	CHECK_MBXID(mbxid);
	CHECK_TMOUT(tmout);
	CHECK_DISPATCH();

	mbxcb = get_mbxcb(mbxid);

	BEGIN_CRITICAL_SECTION;
	if ( mbxcb->mbxid == 0 ) {
		ercd = E_NOEXS;
		goto error_exit;
	}

	if ( headmsg(mbxcb) != NULL ) {
		/* Get message from head of queue */
		*ppk_msg = headmsg(mbxcb);
		headmsg(mbxcb) = nextmsg(*ppk_msg);
	} else {
		/* Ready for receive wait */
		knl_ctxtsk->wspec = ( (mbxcb->mbxatr & TA_TPRI) != 0 )?
					&knl_wspec_mbx_tpri: &knl_wspec_mbx_tfifo;
		knl_ctxtsk->wercd = &ercd;
		knl_ctxtsk->winfo.mbx.ppk_msg = ppk_msg;
		knl_gcb_make_wait((GCB*)mbxcb, tmout);
	}

    error_exit:
	END_CRITICAL_SECTION;

	return ercd;
}
#endif /* USE_FUNC_TK_RCV_MBX */

#ifdef USE_FUNC_TK_REF_MBX
/*
 * Refer mailbox state 
 */
SYSCALL ER tk_ref_mbx_impl( ID mbxid, T_RMBX *pk_rmbx )
{
	MBXCB	*mbxcb;
	ER	ercd = E_OK;

	CHECK_MBXID(mbxid);

	mbxcb = get_mbxcb(mbxid);

	BEGIN_CRITICAL_SECTION;
	if ( mbxcb->mbxid == 0 ) {
		ercd = E_NOEXS;
	} else {
		pk_rmbx->exinf = mbxcb->exinf;
		pk_rmbx->wtsk = knl_wait_tskid(&mbxcb->wait_queue);
		pk_rmbx->pk_msg = headmsg(mbxcb);
	}
	END_CRITICAL_SECTION;

	return ercd;
}
#endif /* USE_FUNC_TK_REF_MBX */

/* ------------------------------------------------------------------------ */
/*
 *	Debugger support function
 */
#if USE_DBGSPT

#ifdef USE_FUNC_MAILBOX_GETNAME
#if USE_OBJECT_NAME
/*
 * Get object name from control block
 */
EXPORT ER knl_mailbox_getname(ID id, UB **name)
{
	MBXCB	*mbxcb;
	ER	ercd = E_OK;

	CHECK_MBXID(id);

	BEGIN_DISABLE_INTERRUPT;
	mbxcb = get_mbxcb(id);
	if ( mbxcb->mbxid == 0 ) {
		ercd = E_NOEXS;
		goto error_exit;
	}
	if ( (mbxcb->mbxatr & TA_DSNAME) == 0 ) {
		ercd = E_OBJ;
		goto error_exit;
	}
	*name = mbxcb->name;

    error_exit:
	END_DISABLE_INTERRUPT;

	return ercd;
}
#endif /* USE_OBJECT_NAME */
#endif /* USE_FUNC_MAILBOX_GETNAME */

#ifdef USE_FUNC_TD_LST_MBX
/*
 * Refer mailbox usage state
 */
SYSCALL INT td_lst_mbx_impl( ID list[], INT nent )
{
	MBXCB	*mbxcb, *end;
	INT	n = 0;

	BEGIN_DISABLE_INTERRUPT;
	end = knl_mbxcb_table + NUM_MBXID;
	for ( mbxcb = knl_mbxcb_table; mbxcb < end; mbxcb++ ) {
		if ( mbxcb->mbxid == 0 ) {
			continue;
		}

		if ( n++ < nent ) {
			*list++ = mbxcb->mbxid;
		}
	}
	END_DISABLE_INTERRUPT;

	return n;
}
#endif /* USE_FUNC_TD_LST_MBX */

#ifdef USE_FUNC_TD_REF_MBX
/*
 * Refer mailbox state
 */
SYSCALL ER td_ref_mbx_impl( ID mbxid, TD_RMBX *pk_rmbx )
{
	MBXCB	*mbxcb;
	ER	ercd = E_OK;

	CHECK_MBXID(mbxid);

	mbxcb = get_mbxcb(mbxid);

	BEGIN_DISABLE_INTERRUPT;
	if ( mbxcb->mbxid == 0 ) {
		ercd = E_NOEXS;
	} else {
		pk_rmbx->exinf = mbxcb->exinf;
		pk_rmbx->wtsk = knl_wait_tskid(&mbxcb->wait_queue);
		pk_rmbx->pk_msg = headmsg(mbxcb);
	}
	END_DISABLE_INTERRUPT;

	return ercd;
}
#endif /* USE_FUNC_TD_REF_MBX */

#ifdef USE_FUNC_TD_MBX_QUE
/*
 * Refer mailbox wait queue
 */
SYSCALL INT td_mbx_que_impl( ID mbxid, ID list[], INT nent )
{
	MBXCB	*mbxcb;
	QUEUE	*q;
	ER	ercd = E_OK;

	CHECK_MBXID(mbxid);

	mbxcb = get_mbxcb(mbxid);

	BEGIN_DISABLE_INTERRUPT;
	if ( mbxcb->mbxid == 0 ) {
		ercd = E_NOEXS;
	} else {
		INT n = 0;
		for ( q = mbxcb->wait_queue.next; q != &mbxcb->wait_queue; q = q->next ) {
			if ( n++ < nent ) {
				*list++ = ((TCB*)q)->tskid;
			}
		}
		ercd = n;
	}
	END_DISABLE_INTERRUPT;

	return ercd;
}
#endif /* USE_FUNC_TD_MBX_QUE */

#endif /* USE_DBGSPT */
#endif /* CFN_MAX_MBXID */
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
 *	mailbox.h
 *	Mailbox
 */

#ifndef _MAILBOX_H_
#define _MAILBOX_H_
/*
 * Mailbox control block
 *
 *	'mq_head' is the first message queue pointer that
 *	points a message.
 *	It is NULL if the message queue is empty.
 *	'mq_tail' is a pointer that points end of message
 *	queue that is not empty.
 *	The message queue value is not guaranteed if the
 *	message queue is empty.
 *	It is used only if the message queue is FIFO (TA_MFIFO).
 */
typedef struct mailbox_control_block {
	QUEUE	wait_queue;	/* Mailbox wait queue */
	ID	mbxid;		/* Mailbox ID */
	void	*exinf;		/* Extended information */
	ATR	mbxatr;		/* Mailbox attribute */
	T_MSG	mq_head;	/* Head of message queue */
	T_MSG	*mq_tail;	/* End of message queue */
#if USE_OBJECT_NAME
	UB	name[OBJECT_NAME_LENGTH];	/* name */
#endif
} MBXCB;

IMPORT MBXCB knl_mbxcb_table[];	/* Mailbox control block */
IMPORT QUEUE knl_free_mbxcb;	/* FreeQue */

#define get_mbxcb(id)	( &knl_mbxcb_table[INDEX_MBX(id)] )

/*
 * Head message
 */
#define headmsg(mbxcb)	( (mbxcb)->mq_head.msgque[0] )

/*
 * Next message
 */
#define nextmsg(msg)	( ((T_MSG*)(msg))->msgque[0] )

/*
 * Insert a message queue following priority
 */
void knl_queue_insert_mpri( T_MSG_PRI *pk_msg, T_MSG *head )
{
	T_MSG_PRI	*msg;
	T_MSG		*prevmsg = head;

	while ( (msg = (T_MSG_PRI*)nextmsg(prevmsg)) != NULL ) {
		if ( msg->msgpri > pk_msg->msgpri ) {
			break;
		}
		prevmsg = (T_MSG*)msg;
	}
	nextmsg(pk_msg) = msg;
	nextmsg(prevmsg) = pk_msg;
}

#endif /* _MAILBOX_H_ */
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

/** [BEGIN Common Definitions] */
#include "kernel.h"
#include <imalloc.h>
#include "memory.h"
/** [END Common Definitions] */


#ifdef USE_FUNC_SEARCHFREEAREA
/*
 * FreeQue search
 *	Search the free area whose size is equal to 'blksz',
 *	or larger than
 *      'blksz' but closest.
 *	If it does not exist, return '&imacb->freeque'.
 */
EXPORT QUEUE* knl_searchFreeArea( IMACB *imacb, W blksz )
{
	QUEUE	*q = &imacb->freeque;

	/* For area whose memory pool size is less than 1/4,
	   search from smaller size.
	   Otherwise, search from larger size. */
	if ( blksz > imacb->memsz / 4 ) {
		/* Search from larger size. */
		W fsz = 0;
		while ( (q = q->prev) != &imacb->freeque ) {
			fsz = FreeSize(q);
			if ( fsz <= blksz ) {
				return ( fsz < blksz )? q->next: q;
			}
		}
		return ( fsz >= blksz )? q->next: q;
	} else {
		/* Search from smaller size. */
		while ( (q = q->next) != &imacb->freeque ) {
			if ( FreeSize(q) >= blksz ) {
				break;
			}
		}
		return q;
	}
}
#endif /* USE_FUNC_SEARCHFREEAREA */


#ifdef USE_FUNC_APPENDFREEAREA
/*
 * Registration of free area on FreeQue
 *	FreeQue is composed of 2 types: Queue that links the
 *	different size of areas by size and queue that links the
 *	same size of areas.
 *
 *	freeque
 *	|
 *	|   +-----------------------+	    +-----------------------+
 *	|   | AreaQue		    |	    | AreaQue		    |
 *	|   +-----------------------+	    +-----------------------+
 *	*---> FreeQue Size order    |	    | EmptyQue		    |
 *	|   | FreeQue Same size   --------->| FreeQue Same size   ----->
 *	|   |			    |	    |			    |
 *	|   |			    |	    |			    |
 *	|   +-----------------------+	    +-----------------------+
 *	|   | AreaQue		    |	    | AreaQue		    |
 *	v   +-----------------------+	    +-----------------------+
 */
EXPORT void knl_appendFreeArea( IMACB *imacb, QUEUE *aq )
{
	QUEUE	*fq;
	W	size = AreaSize(aq);

	/* Registration position search */
	/*  Search the free area whose size is equal to 'blksz',
	 *  or larger than 'blksz' but closest.
	 *  If it does not exist, return '&imacb->freeque'.
	 */
	fq = knl_searchFreeArea(imacb, size);

	/* Register */
	clrAreaFlag(aq, AREA_USE);
	if ( fq != &imacb->freeque && FreeSize(fq) == size ) {
		/* FreeQue Same size */
		(aq + 2)->next = (fq + 1)->next;
		(fq + 1)->next = aq + 2;
		(aq + 2)->prev = fq + 1;
		if( (aq + 2)->next != NULL ) {
			(aq + 2)->next->prev = aq + 2;
		}
		(aq + 1)->next = NULL;
	} else {
		/* FreeQue Size order */
		QueInsert(aq + 1, fq);
		(aq + 2)->next = NULL;
		(aq + 2)->prev = (QUEUE*)size;
	}
}
#endif /* USE_FUNC_APPENDFREEAREA */

#ifdef USE_FUNC_REMOVEFREEQUE
/*
 * Delete from FreeQue
 */
EXPORT void knl_removeFreeQue( QUEUE *fq )
{
	if ( fq->next == NULL ) {	/* FreeQue Same size */
		(fq + 1)->prev->next = (fq + 1)->next;
		if ( (fq + 1)->next != NULL ) {
			(fq + 1)->next->prev = (fq + 1)->prev;
		}
	} else {			/* FreeQue Size order */
		if ( (fq + 1)->next != NULL ) {		/* having FreeQue Same size */
			QueInsert((fq + 1)->next - 1, fq);
			(fq + 1)->next->prev = (fq + 1)->prev;
		}
		QueRemove(fq);
	}
}
#endif /* USE_FUNC_REMOVEFREEQUE */

#ifdef USE_FUNC_INSERTAREAQUE
/*
 * Register area
 *	Insert 'ent' just after 'que.'
 */
EXPORT void knl_insertAreaQue( QUEUE *que, QUEUE *ent )
{
	ent->prev = que;
	ent->next = que->next;
	Assign(que->next->prev, ent);
	que->next = ent;
}
#endif /* USE_FUNC_INSERTAREAQUE */

#ifdef USE_FUNC_REMOVEAREAQUE
/*
 * Delete area
 */
EXPORT void knl_removeAreaQue( QUEUE *aq )
{
	Mask(aq->prev)->next = aq->next;
	Assign(aq->next->prev, Mask(aq->prev));
}
#endif /* USE_FUNC_REMOVEAREAQUE */

/* ------------------------------------------------------------------------ */

#if USE_IMALLOC


/* ------------------------------------------------------------------------ */

#ifdef USE_FUNC_IMACB
Noinit(EXPORT IMACB *knl_imacb);
#endif /* USE_FUNC_IMACB */

/* ------------------------------------------------------------------------ */

#ifdef USE_FUNC_IMALLOC
/*
 * Get memory 
 */
EXPORT void* knl_Imalloc( size_t size )
{
	QUEUE	*q, *aq, *aq2;
	UINT	imask;

	/* If it is smaller than the minimum fragment size,
	   allocate the minimum size to it. */
	if ( size < MIN_FRAGMENT ) {
		size = MIN_FRAGMENT;
	} else {
		size = ROUND(size);
	}

	DI(imask);  /* Exclusive control by interrupt disable */

	/* Search FreeQue */
	q = knl_searchFreeArea(knl_imacb, size);
	if ( q == &(knl_imacb->freeque) ) {
		q = NULL; /* Insufficient memory */
		goto err_ret;
	}

	/* There is free area: Split from FreeQue once */
	knl_removeFreeQue(q);

	aq = q - 1;

	/* If there are fragments smaller than the minimum fragment size,
	   allocate them also */
	if ( FreeSize(q) - size >= MIN_FRAGMENT + sizeof(QUEUE) ) {

		/* Divide area into 2 */
		aq2 = (QUEUE*)((VB*)(aq + 1) + size);
		knl_insertAreaQue(aq, aq2);

		/* Register remaining area to FreeQue */
		knl_appendFreeArea(knl_imacb, aq2);
	}
	setAreaFlag(aq, AREA_USE);

err_ret:
	EI(imask);

	return (void *)q;
}
#endif /* USE_FUNC_IMALLOC */

#ifdef USE_FUNC_ICALLOC
/*
 * Get memory
 */
EXPORT void* knl_Icalloc( size_t nmemb, size_t size )
{
	size_t	sz = nmemb * size;
	void	*mem;

	mem = knl_Imalloc(sz);
	if ( mem == NULL ) {
		return NULL;
	}

	memset(mem, 0, sz);

	return mem;
}
#endif /* USE_FUNC_ICALLOC */

#ifdef USE_FUNC_IFREE
/*
 * Free memory
 *	It may be called during interrupt disable. In this case, need to wait
 *	 until interrupt is enabled and until free.
 */
EXPORT void  knl_Ifree( void *ptr )
{
	QUEUE	*aq;
	UINT	imask;

	DI(imask);  /* Exclusive control by interrupt disable */

	aq = (QUEUE*)ptr - 1;
	clrAreaFlag(aq, AREA_USE);

	if ( !chkAreaFlag(aq->next, AREA_USE) ) {
		/* Merge with free area in after location */
		knl_removeFreeQue(aq->next + 1);
		knl_removeAreaQue(aq->next);
	}

	if ( !chkAreaFlag(aq->prev, AREA_USE) ) {
		/* Merge with free area in front location */
		aq = aq->prev;
		knl_removeFreeQue(aq + 1);
		knl_removeAreaQue(aq->next);
	}

	knl_appendFreeArea(knl_imacb, aq);

	EI(imask);
}
#endif /* USE_FUNC_IFREE */

/* ------------------------------------------------------------------------ */

#ifdef USE_FUNC_INIT_IMALLOC
/*
 * IMACB Initialization 
 */
LOCAL void initIMACB( void )
{
	QueInit(&(knl_imacb->areaque));
	QueInit(&(knl_imacb->freeque));
}

/*
 * Imalloc initial setting 
 */
EXPORT ER knl_init_Imalloc( void )
{
/* Low-level memory management information */
IMPORT	void	*knl_lowmem_top, *knl_lowmem_limit;

	void	*memend;
	QUEUE	*top, *end;

	/* Acquire system configuration definition information */
	memend = CFN_REALMEMEND;
	if ( (UW)memend > (UW)knl_lowmem_limit ) {
		memend = knl_lowmem_limit;
	}

	/* Align top with 4 byte unit alignment for IMACB */
	knl_lowmem_top = (void *)(((UW)knl_lowmem_top + 3) & ~0x00000003UL);
	knl_imacb = (IMACB*)knl_lowmem_top;
	knl_lowmem_top = (void *)((UW)knl_lowmem_top + sizeof(IMACB));

	/* Align top with 8 byte unit alignment */
	knl_lowmem_top = (void *)(((UW)knl_lowmem_top + 7) & ~0x00000007UL);
	top = (QUEUE*)knl_lowmem_top;
	knl_imacb->memsz = (W)((UW)memend - (UW)knl_lowmem_top - sizeof(QUEUE)*2);

	knl_lowmem_top = memend;  /* Update memory free space */

	initIMACB();

	/* Register on AreaQue */
	end = (QUEUE*)((VB*)top + knl_imacb->memsz) + 1;
	knl_insertAreaQue(&knl_imacb->areaque, end);
	knl_insertAreaQue(&knl_imacb->areaque, top);
	setAreaFlag(end, AREA_USE);
	setAreaFlag(&knl_imacb->areaque, AREA_USE);

	knl_appendFreeArea(knl_imacb, top);

	return E_OK;
}
#endif /* USE_FUNC_INIT_IMALLOC */

#endif /* USE_IMALLOC */
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


#ifndef _MEMORY_H_
#define _MEMORY_H_

/*
 * Memory allocation management information
 *
 *  Order of members must not be changed because members are used
 *  with casting from MPLCB.
 */
typedef struct {
	W		memsz;

	/* AreaQue for connecting each area where reserved pages are
	   divided Sort in ascending order of addresses in a page.
	   Do not sort between pages. */
	QUEUE		areaque;
	/* FreeQue for connecting unused area in reserved pages
	   Sort from small to large free spaces. */
	QUEUE		freeque;
} IMACB;

/*
 * Compensation for aligning "&areaque" position to 2 bytes border
 */
#define AlignIMACB(imacb)	( (IMACB*)((UW)(imacb) & ~0x00000001UL) )

/*
 * Minimum unit of subdivision
 *	The lower 1 bit of address is always 0
 *	because memory is allocated by ROUNDSZ.
 *	AreaQue uses the lower 1 bit for flag.
 */
#define ROUNDSZ		( sizeof(QUEUE) )	/* 8 bytes */
#define ROUND(sz)	( ((UW)(sz) + (UW)(ROUNDSZ-1)) & ~(UW)(ROUNDSZ-1) )

/* Minimum fragment size */
#define MIN_FRAGMENT	( sizeof(QUEUE) * 2 )

/*
 * Adjusting the size which can be allocated 
 */
W roundSize( W sz )
{
	if ( sz < (W)MIN_FRAGMENT ) {
		sz = (W)MIN_FRAGMENT;
	}
	return (W)(((UW)sz + (UW)(ROUNDSZ-1)) & ~(UW)(ROUNDSZ-1));
}


/*
 * Flag that uses the lower bits of AreaQue's 'prev'.
 */
#define AREA_USE	0x00000001UL	/* In-use */
#define AREA_MASK	0x00000001UL

#define setAreaFlag(q, f)   ( (q)->prev = (QUEUE*)((UW)(q)->prev |  (UW)(f)) )
#define clrAreaFlag(q, f)   ( (q)->prev = (QUEUE*)((UW)(q)->prev & ~(UW)(f)) )
#define chkAreaFlag(q, f)   ( ((UW)(q)->prev & (UW)(f)) != 0 )

#define Mask(x)		( (QUEUE*)((UW)(x) & ~AREA_MASK) )
#define Assign(x, y)	( (x) = (QUEUE*)(((UW)(x) & AREA_MASK) | (UW)(y)) )
/*
 * Area size
 */
#define AreaSize(aq)	( (VB*)(aq)->next - (VB*)((aq) + 1) )
#define FreeSize(fq)	( (W)((fq) + 1)->prev )


IMPORT QUEUE* knl_searchFreeArea( IMACB *imacb, W blksz );
IMPORT void knl_appendFreeArea( IMACB *imacb, QUEUE *aq );
IMPORT void knl_removeFreeQue( QUEUE *fq );
IMPORT void knl_insertAreaQue( QUEUE *que, QUEUE *ent );
IMPORT void knl_removeAreaQue( QUEUE *aq );

IMPORT IMACB *knl_imacb;
IMPORT ER knl_init_Imalloc( void );

#endif /* _MEMORY_H_ */
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
 *	mempfix.c
 *	Fixed Size Memory Pool
 */

/** [BEGIN Common Definitions] */
#include "kernel.h"
#include "task.h"
#include "wait.h"
#include "check.h"
#include "mempfix.h"
/** [END Common Definitions] */

#if CFN_MAX_MPFID > 0

#ifdef USE_FUNC_MPFCB_TABLE
Noinit(EXPORT MPFCB	knl_mpfcb_table[NUM_MPFID]);	/* Fixed size memory pool control block */
Noinit(EXPORT QUEUE	knl_free_mpfcb);	/* FreeQue */
#endif /* USE_FUNC_MPFCB_TABLE */


#ifdef USE_FUNC_FIX_MEMORYPOOL_INITIALIZE
/*
 * Initialization of fixed size memory pool control block
 */
EXPORT ER knl_fix_memorypool_initialize( void )
{
	MPFCB	*mpfcb, *end;

	/* Get system information */
	if ( NUM_MPFID < 1 ) {
		return E_SYS;
	}

	/* Register all control blocks onto FreeQue */
	QueInit(&knl_free_mpfcb);
	end = knl_mpfcb_table + NUM_MPFID;
	for ( mpfcb = knl_mpfcb_table; mpfcb < end; mpfcb++ ) {
		mpfcb->mpfid = 0;
		knl_InitOBJLOCK(&mpfcb->lock);
		QueInsert(&mpfcb->wait_queue, &knl_free_mpfcb);
	}

	return E_OK;
}
#endif /* USE_FUNC_FIX_MEMORYPOOL_INITIALIZE */


#ifdef USE_FUNC_TK_CRE_MPF
/*
 * Create fixed size memory pool
 */
SYSCALL ID tk_cre_mpf_impl( CONST T_CMPF *pk_cmpf )
{
#if CHK_RSATR
	const ATR VALID_MPFATR = {
		 TA_TPRI
		|TA_RNG3
		|TA_USERBUF
#if USE_OBJECT_NAME
		|TA_DSNAME
#endif
	};
#endif
	MPFCB	*mpfcb;
	ID	mpfid;
	W	blfsz, mpfsz;
	void	*mempool;

	CHECK_RSATR(pk_cmpf->mpfatr, VALID_MPFATR);
	CHECK_PAR(pk_cmpf->mpfcnt > 0);
	CHECK_PAR(pk_cmpf->blfsz > 0);
#if !USE_IMALLOC
	/* TA_USERBUF must be specified if configured in no Imalloc */
	CHECK_PAR((pk_cmpf->mpfatr & TA_USERBUF) != 0);
#endif
	CHECK_DISPATCH();

	blfsz = (W)MINSZ(pk_cmpf->blfsz);
	mpfsz = blfsz * pk_cmpf->mpfcnt;

#if USE_IMALLOC
	if ( (pk_cmpf->mpfatr & TA_USERBUF) != 0 ) {
		/* Size of user buffer must be multiples of sizeof(FREEL) */
		if ( blfsz != pk_cmpf->blfsz ) {
			return E_PAR;
		}
		/* Use user buffer */
		mempool = pk_cmpf->bufptr;
	} else {
		/* Allocate memory for memory pool */
		mempool = knl_Imalloc((UW)mpfsz);
		if ( mempool == NULL ) {
			return E_NOMEM;
		}
	}
#else
	/* Size of user buffer must be larger than sizeof(FREEL) */
	if ( blfsz != pk_cmpf->blfsz ) {
		return E_PAR;
	}
	/* Use user buffer */
	mempool = pk_cmpf->bufptr;
#endif

	/* Get control block from FreeQue */
	DISABLE_INTERRUPT;
	mpfcb = (MPFCB*)QueRemoveNext(&knl_free_mpfcb);
	ENABLE_INTERRUPT;

	if ( mpfcb == NULL ) {
#if USE_IMALLOC
		if ( (pk_cmpf->mpfatr & TA_USERBUF) == 0 ) {
			knl_Ifree(mempool);
		}
#endif
		return E_LIMIT;
	}

	knl_LockOBJ(&mpfcb->lock);
	mpfid = ID_MPF(mpfcb - knl_mpfcb_table);

	/* Initialize control block */
	QueInit(&mpfcb->wait_queue);
	mpfcb->exinf    = pk_cmpf->exinf;
	mpfcb->mpfatr   = pk_cmpf->mpfatr;
	mpfcb->mpfcnt   = mpfcb->frbcnt = pk_cmpf->mpfcnt;
	mpfcb->blfsz    = blfsz;
	mpfcb->mpfsz    = mpfsz;
	mpfcb->unused   = mpfcb->mempool = mempool;
	mpfcb->freelist = NULL;
#if USE_OBJECT_NAME
	if ( (pk_cmpf->mpfatr & TA_DSNAME) != 0 ) {
		strncpy((char*)mpfcb->name, (char*)pk_cmpf->dsname, OBJECT_NAME_LENGTH);
	}
#endif

	mpfcb->mpfid    = mpfid;  /* Set ID after completion */
	knl_UnlockOBJ(&mpfcb->lock);

	return mpfid;
}
#endif /* USE_FUNC_TK_CRE_MPF */

#ifdef USE_FUNC_TK_DEL_MPF
/*
 * Delete fixed size memory pool 
 */
SYSCALL ER tk_del_mpf_impl( ID mpfid )
{
	MPFCB	*mpfcb;
	void	*mempool = NULL;
	ATR	memattr = 0;
	ER	ercd = E_OK;

	CHECK_MPFID(mpfid);
	CHECK_DISPATCH();

	mpfcb = get_mpfcb(mpfid);

	knl_LockOBJ(&mpfcb->lock);
	if ( mpfcb->mpfid == 0 ) {
		ercd = E_NOEXS;
	} else {
		DISABLE_INTERRUPT;
		mempool = mpfcb->mempool;
		memattr = mpfcb->mpfatr;

		/* Release wait state of task (E_DLT) */
		knl_wait_delete(&mpfcb->wait_queue);

		/* Return to FreeQue */
		QueInsert(&mpfcb->wait_queue, &knl_free_mpfcb);
		mpfcb->mpfid = 0;
		ENABLE_INTERRUPT;
	}
	knl_UnlockOBJ(&mpfcb->lock);

#if USE_IMALLOC
	if ( (mempool != NULL) && ((memattr & TA_USERBUF) == 0) ) {
		knl_Ifree(mempool);
	}
#endif

	return ercd;
}
#endif /* USE_FUNC_TK_DEL_MPF */

#ifdef USE_FUNC_TK_GET_MPF
/*
 * Processing if the priority of wait task changes
 */
LOCAL void knl_mpf_chg_pri( TCB *tcb, INT oldpri )
{
	MPFCB	*mpfcb;

	mpfcb = get_mpfcb(tcb->wid);
	knl_gcb_change_priority((GCB*)mpfcb, tcb);
}

/*
 * Definition of fixed size memory pool wait specification
 */
LOCAL CONST WSPEC knl_wspec_mpf_tfifo = { TTW_MPF, NULL, NULL };
LOCAL CONST WSPEC knl_wspec_mpf_tpri  = { TTW_MPF, knl_mpf_chg_pri, NULL };

/*
 * Get fixed size memory block 
 */
SYSCALL ER tk_get_mpf_impl( ID mpfid, void **p_blf, TMO tmout )
{
	MPFCB	*mpfcb;
	FREEL	*free;
	ER	ercd = E_OK;

	CHECK_MPFID(mpfid);
	CHECK_TMOUT(tmout);
	CHECK_DISPATCH();

	mpfcb = get_mpfcb(mpfid);

	knl_LockOBJ(&mpfcb->lock);
	if ( mpfcb->mpfid == 0 ) {
		ercd = E_NOEXS;
		goto error_exit;
	}

	/* If there is no space, ready for wait */
	if ( mpfcb->frbcnt <= 0 ) {
		goto wait_mpf;
	} else {
		/* Get memory block */
		if ( mpfcb->freelist != NULL ) {
			free = mpfcb->freelist;
			mpfcb->freelist = free->next;
			*p_blf = free;
		} else {
			*p_blf = mpfcb->unused;
			mpfcb->unused = (VB*)mpfcb->unused + mpfcb->blfsz;
		}
		mpfcb->frbcnt--;
	}

    error_exit:
	knl_UnlockOBJ(&mpfcb->lock);

	return ercd;

wait_mpf:
	/* Ready for wait */
	BEGIN_CRITICAL_SECTION;
	knl_ctxtsk->wspec = ( (mpfcb->mpfatr & TA_TPRI) != 0 )?
				&knl_wspec_mpf_tpri: &knl_wspec_mpf_tfifo;
	knl_ctxtsk->wercd = &ercd;
	knl_ctxtsk->winfo.mpf.p_blf = p_blf;
	knl_gcb_make_wait((GCB*)mpfcb, tmout);

	knl_UnlockOBJ(&mpfcb->lock);
	END_CRITICAL_SECTION;

	return ercd;
}
#endif /* USE_FUNC_TK_GET_MPF */

#ifdef USE_FUNC_TK_REL_MPF
/*
 * Return fixed size memory block 
 */
SYSCALL ER tk_rel_mpf_impl( ID mpfid, void *blf )
{
	MPFCB	*mpfcb;
	TCB	*tcb;
	FREEL	*free;
	ER	ercd = E_OK;

	CHECK_MPFID(mpfid);
	CHECK_DISPATCH();

	mpfcb = get_mpfcb(mpfid);

	knl_LockOBJ(&mpfcb->lock);
	if ( mpfcb->mpfid == 0 ) {
		ercd = E_NOEXS;
		goto error_exit;
	}
#if CHK_PAR
	if ( blf < mpfcb->mempool || blf >= knl_mempool_end(mpfcb) || (((VB*)blf - (VB*)mpfcb->mempool) % mpfcb->blfsz) != 0 ) {
		ercd = E_PAR;
		goto error_exit;
	}
#endif

	DISABLE_INTERRUPT;
	if ( !isQueEmpty(&mpfcb->wait_queue) ) {
		/* Send memory block to waiting task,
		   and then release the task */
		tcb = (TCB*)mpfcb->wait_queue.next;
		*tcb->winfo.mpf.p_blf = blf;
		knl_wait_release_ok(tcb);
		ENABLE_INTERRUPT;
	} else {
		ENABLE_INTERRUPT;
		/* Free memory block */
		free = (FREEL*)blf;
		free->next = mpfcb->freelist;
		mpfcb->freelist = free;
		mpfcb->frbcnt++;
	}

error_exit:
	knl_UnlockOBJ(&mpfcb->lock);

	return ercd;
}
#endif /* USE_FUNC_TK_REL_MPF */

#ifdef USE_FUNC_TK_REF_MPF
/*
 * Check fixed size pool state
 */
SYSCALL ER tk_ref_mpf_impl( ID mpfid, T_RMPF *pk_rmpf )
{
	MPFCB	*mpfcb;
	ER	ercd = E_OK;

	CHECK_MPFID(mpfid);
	CHECK_DISPATCH();

	mpfcb = get_mpfcb(mpfid);

	knl_LockOBJ(&mpfcb->lock);
	if ( mpfcb->mpfid == 0 ) {
		ercd = E_NOEXS;
	} else {
		DISABLE_INTERRUPT;
		pk_rmpf->wtsk = knl_wait_tskid(&mpfcb->wait_queue);
		ENABLE_INTERRUPT;
		pk_rmpf->exinf = mpfcb->exinf;
		pk_rmpf->frbcnt = mpfcb->frbcnt;
	}
	knl_UnlockOBJ(&mpfcb->lock);

	return ercd;
}
#endif /* USE_FUNC_TK_REF_MPF */

/* ------------------------------------------------------------------------ */
/*
 *	Debugger support function
 */
#if USE_DBGSPT

#ifdef USE_FUNC_FIX_MEMORYPOOL_GETNAME
#if USE_OBJECT_NAME
/*
 * Get object name from control block
 */
EXPORT ER knl_fix_memorypool_getname(ID id, UB **name)
{
	MPFCB	*mpfcb;
	ER	ercd = E_OK;

	CHECK_MPFID(id);

	BEGIN_DISABLE_INTERRUPT;
	mpfcb = get_mpfcb(id);
	if ( mpfcb->mpfid == 0 ) {
		ercd = E_NOEXS;
		goto error_exit;
	}
	if ( (mpfcb->mpfatr & TA_DSNAME) == 0 ) {
		ercd = E_OBJ;
		goto error_exit;
	}
	*name = mpfcb->name;

    error_exit:
	END_DISABLE_INTERRUPT;

	return ercd;
}
#endif /* USE_OBJECT_NAME */
#endif /* USE_FUNC_FIX_MEMORYPOOL_GETNAME */

#ifdef USE_FUNC_TD_LST_MPF
/*
 * Refer fixed size memory pool usage state
 */
SYSCALL INT td_lst_mpf_impl( ID list[], INT nent )
{
	MPFCB	*mpfcb, *end;
	INT	n = 0;

	BEGIN_DISABLE_INTERRUPT;
	end = knl_mpfcb_table + NUM_MPFID;
	for ( mpfcb = knl_mpfcb_table; mpfcb < end; mpfcb++ ) {
		if ( mpfcb->mpfid == 0 ) {
			continue;
		}

		if ( n++ < nent ) {
			*list++ = ID_MPF(mpfcb - knl_mpfcb_table);
		}
	}
	END_DISABLE_INTERRUPT;

	return n;
}
#endif /* USE_FUNC_TD_LST_MPF */

#ifdef USE_FUNC_TD_REF_MPF
/*
 * Refer fixed size memory pool state 
 */
SYSCALL ER td_ref_mpf_impl( ID mpfid, TD_RMPF *pk_rmpf )
{
	MPFCB	*mpfcb;
	ER	ercd = E_OK;

	CHECK_MPFID(mpfid);

	mpfcb = get_mpfcb(mpfid);

	BEGIN_DISABLE_INTERRUPT;
	if ( mpfcb->mpfid == 0 ) {
		ercd = E_NOEXS;
	} else if ( knl_isLockedOBJ(&mpfcb->lock) ) {
		ercd = E_CTX;
	} else {
		pk_rmpf->wtsk = knl_wait_tskid(&mpfcb->wait_queue);
		pk_rmpf->exinf = mpfcb->exinf;
		pk_rmpf->frbcnt = mpfcb->frbcnt;
	}
	END_DISABLE_INTERRUPT;

	return ercd;
}
#endif /* USE_FUNC_TD_REF_MPF */

#ifdef USE_FUNC_TD_MPF_QUE
/*
 * Refer fixed size memory wait queue 
 */
SYSCALL INT td_mpf_que_impl( ID mpfid, ID list[], INT nent )
{
	MPFCB	*mpfcb;
	QUEUE	*q;
	ER	ercd = E_OK;

	CHECK_MPFID(mpfid);

	mpfcb = get_mpfcb(mpfid);

	BEGIN_DISABLE_INTERRUPT;
	if ( mpfcb->mpfid == 0 ) {
		ercd = E_NOEXS;
	} else {
		INT n = 0;
		for ( q = mpfcb->wait_queue.next; q != &mpfcb->wait_queue; q = q->next ) {
			if ( n++ < nent ) {
				*list++ = ((TCB*)q)->tskid;
			}
		}
		ercd = n;
	}
	END_DISABLE_INTERRUPT;

	return ercd;
}
#endif /* USE_FUNC_TD_MPF_QUE */

#endif /* USE_DBGSPT */
#endif /* CFN_MAX_MPFID */
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
 *	mempfix.h
 *	Fixed Size Memory Pool
 */

#ifndef _MEMPFIX_H_
#define _MEMPFIX_H_

/*
 * Fixed size memory pool control block
 */
typedef struct free_list {
	struct free_list *next;
} FREEL;

typedef struct fix_memorypool_control_block {
	QUEUE	wait_queue;	/* Memory pool wait queue */
	ID	mpfid;		/* Fixed size memory pool ID */
	void	*exinf;		/* Extended information */
	ATR	mpfatr;		/* Memory pool attribute */
	W	mpfcnt;		/* Number of blocks in whole memory pool */
	W	blfsz;		/* Fixed size memory block size */
	W	mpfsz;		/* Whole memory pool size */
	W	frbcnt;		/* Number of blocks in free area */
	void	*mempool;	/* Top address of memory pool */
	void	*unused;		/* Top address of unused area */
	FREEL	*freelist;	/* Free block list */
	OBJLOCK	lock;		/* Lock for object exclusive access */
#if USE_OBJECT_NAME
	UB	name[OBJECT_NAME_LENGTH];	/* name */
#endif
} MPFCB;

IMPORT MPFCB knl_mpfcb_table[];	/* Fixed size memory pool control block */
IMPORT QUEUE knl_free_mpfcb;	/* FreeQue */

#define get_mpfcb(id)	( &knl_mpfcb_table[INDEX_MPF(id)] )


#define MINSIZE		( sizeof(FREEL) )
#define MINSZ(sz)	( ((UW)(sz) + (UW)(MINSIZE-1)) & ~(UW)(MINSIZE-1) )

/*
 * Return end address in memory pool area
 */
void *knl_mempool_end( MPFCB *mpfcb )
{
	return (VB*)mpfcb->mempool + mpfcb->mpfsz;
}



#endif /* _MEMPFIX_H_ */
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
 *	mempool.c
 *	Variable Size Memory Pool
 */

/** [BEGIN Common Definitions] */
#include "kernel.h"
#include "task.h"
#include "wait.h"
#include "check.h"
#include "memory.h"
#include "mempool.h"
/** [END Common Definitions] */

#if CFN_MAX_MPLID > 0


#ifdef USE_FUNC_MPLCB_TABLE
Noinit(EXPORT MPLCB knl_mplcb_table[NUM_MPLID]);	/* Variable size memory pool control block */
Noinit(EXPORT QUEUE knl_free_mplcb);	/* FreeQue */
#endif /* USE_FUNC_MPLCB_TABLE */


#ifdef USE_FUNC_MEMORYPOOL_INITIALIZE
/*
 * Initialization of variable size memory pool control block
 */
EXPORT ER knl_memorypool_initialize( void )
{
	MPLCB	*mplcb, *end;

	if ( NUM_MPLID < 1 ) {
		return E_SYS;
	}

	/* Register all control blocks onto FreeQue */
	QueInit(&knl_free_mplcb);
	end = knl_mplcb_table + NUM_MPLID;
	for ( mplcb = knl_mplcb_table; mplcb < end; mplcb++ ) {
		mplcb->mplid = 0;
		QueInsert(&mplcb->wait_queue, &knl_free_mplcb);
	}

	return E_OK;
}
#endif /* USE_FUNC_MEMORYPOOL_INITIALIZE */

/* ------------------------------------------------------------------------ */

#ifdef USE_FUNC_APPENDFREEAREABOUND
/*
 * Registration of free area on FreeQue
 *   Specialized version for merging with top/end area
 */
EXPORT void knl_appendFreeAreaBound( MPLCB *mplcb, QUEUE *aq )
{
	IMACB	*imacb = (IMACB*)&(mplcb->mplsz);
	QUEUE	*fq, *top, *end;
	W	size;

	if ( aq == &(mplcb->areaque) ) {
		top = (QUEUE*)mplcb->mempool;
	} else {
		top = aq + 1;
	}

	if ( aq->next == &(mplcb->areaque_end) ) {
		end = (QUEUE*)((VB*)mplcb->mempool + mplcb->mplsz);
	} else {
		end = aq->next;
	}

	size = (W)((VB*)end - (VB*)top);

	/* Registration position search */
	/*  Search the free area whose size is equal to 'blksz',
	 *  or larger than 'blksz' but closest.
	 *  If it does not exist, return '&imacb->freeque'.
	 */
	fq = knl_searchFreeArea(imacb, size);

	/* Register */
	clrAreaFlag(aq, AREA_USE);
	if ( fq != &imacb->freeque && FreeSize(fq) == size ) {
		/* FreeQue Same size */
		(top + 1)->next = (fq + 1)->next;
		(fq  + 1)->next = top + 1;
		(top + 1)->prev = fq + 1;
		if( (top + 1)->next != NULL ) {
			(top + 1)->next->prev = top + 1;
		}
		top->next = NULL;
	} else {
		/* FreeQue Size order */
		QueInsert(top, fq);
		(top + 1)->next = NULL;
		(top + 1)->prev = (QUEUE*)size;
	}
}
#endif /* USE_FUNC_APPENDFREEAREABOUND */

#ifdef USE_FUNC_GET_BLK
/*
 * Get memory block 
 *	'blksz' must be larger than minimum fragment size
 *	and adjusted by ROUNDSZ unit.
 */
EXPORT void *knl_get_blk( MPLCB *mplcb, W blksz )
{
	QUEUE	*q, *aq, *aq2;
	IMACB*	imacb = (IMACB*)&(mplcb->mplsz);

	/* Search FreeQue */
	q = knl_searchFreeArea(imacb, blksz);
	if ( q == &(imacb->freeque) ) {
		return NULL;
	}

	/* remove free area from FreeQue */
	knl_removeFreeQue(q);
	aq = ((void *)q == mplcb->mempool) ? &(mplcb->areaque) :  q - 1;

	/* If there is a fragment smaller than the minimum fragment size,
	   allocate them together */
	if ( FreeSize(q) - (UW)blksz >= MIN_FRAGMENT + sizeof(QUEUE) ) {

		/* Divide the area into 2. */
		aq2 = (QUEUE*)((VB*)q + blksz);
		knl_insertAreaQue(aq, aq2);

		/* Register the remaining area onto FreeQue */
		if ( aq2->next == &(mplcb->areaque_end) ) {
			knl_appendFreeAreaBound(mplcb, aq2);
		} else {
			knl_appendFreeArea(imacb, aq2);
		}
	}
	setAreaFlag(aq, AREA_USE);

	return (void *)q;
}
#endif /* USE_FUNC_GET_BLK */

#ifdef USE_FUNC_REL_BLK
/*
 * Free memory block 
 */
EXPORT ER knl_rel_blk( MPLCB *mplcb, void *blk )
{
	QUEUE	*aq;
	IMACB*	imacb = (IMACB*)&(mplcb->mplsz);

	aq = (blk == mplcb->mempool) ? &(mplcb->areaque) : (QUEUE*)blk - 1;

#if CHK_PAR
	if ( !chkAreaFlag(aq, AREA_USE) ) {
		return E_PAR;
	}
#endif
	clrAreaFlag(aq, AREA_USE);

	if ( !chkAreaFlag(aq->next, AREA_USE) ) {
		/* Merge to the next area */
		knl_removeFreeQue(aq->next + 1);
		knl_removeAreaQue(aq->next);
	}
	if ( !chkAreaFlag(aq->prev, AREA_USE) ) {
		/* Merge to the previous area */
		QUEUE *fq;
		aq = aq->prev;
		fq = (aq == &(mplcb->areaque)) ? (QUEUE*)(mplcb->mempool) : aq + 1;

		knl_removeFreeQue(fq);
		knl_removeAreaQue(aq->next);
	}

	/* Register free area onto FreeQue */
	if ( aq == &(mplcb->areaque) || aq->next == &(mplcb->areaque_end) ) {
		knl_appendFreeAreaBound(mplcb, aq);
	} else {
		knl_appendFreeArea(imacb, aq);
	}

	return E_OK;
}
#endif /* USE_FUNC_REL_BLK */

/* ------------------------------------------------------------------------ */

#ifdef USE_FUNC_MPL_WAKEUP
/*
 * Allocate memory and release wait task,
 * as long as there are enough free memory.
 */
EXPORT void knl_mpl_wakeup( MPLCB *mplcb )
{
	TCB	*top;
	void	*blk;
	W	blksz;

	while ( !isQueEmpty(&mplcb->wait_queue) ) {
		top = (TCB*)mplcb->wait_queue.next;
		blksz = top->winfo.mpl.blksz;

		/* Check free space */
		if ( blksz > knl_MaxFreeSize(mplcb) ) {
			break;
		}

		/* Get memory block */
		blk = knl_get_blk(mplcb, blksz);
		*top->winfo.mpl.p_blk = blk;

		/* Release wait task */
		knl_wait_release_ok(top);
	}
}
#endif /* USE_FUNC_MPL_WAKEUP */


#ifdef USE_FUNC_TK_CRE_MPL
/*
 * Memory pool initial setting
 */
LOCAL void init_mempool( MPLCB *mplcb )
{
	QueInit(&mplcb->areaque);
	QueInit(&mplcb->freeque);

	/* Register onto AreaQue */
	knl_insertAreaQue(&mplcb->areaque, &mplcb->areaque_end);

	/* Set AREA_USE for locations that must not be free area */
	setAreaFlag(&mplcb->areaque_end, AREA_USE);

	/* Register onto FreeQue */
	knl_appendFreeAreaBound(mplcb, &mplcb->areaque);
}

/*
 * Create variable size memory pool 
 */
SYSCALL ID tk_cre_mpl_impl( CONST T_CMPL *pk_cmpl )
{
#if CHK_RSATR
	const ATR VALID_MPLATR = {
		 TA_TPRI
		|TA_RNG3
		|TA_USERBUF
#if USE_OBJECT_NAME
		|TA_DSNAME
#endif
	};
#endif
	MPLCB	*mplcb;
	ID	mplid;
	W	mplsz;
	void	*mempool;
	ER	ercd;

	CHECK_RSATR(pk_cmpl->mplatr, VALID_MPLATR);
	CHECK_PAR(pk_cmpl->mplsz > 0);
#if !USE_IMALLOC
	/* TA_USERBUF must be specified if configured in no Imalloc */
	CHECK_PAR((pk_cmpl->mplatr & TA_USERBUF) != 0);
#endif
	CHECK_DISPATCH();

	mplsz = roundSize(pk_cmpl->mplsz);

#if USE_IMALLOC
	if ( (pk_cmpl->mplatr & TA_USERBUF) != 0 ) {
		/* Size of user buffer must be multiples of sizeof(QUEUE)
			and larger than sizeof(QUEUE)*2 */
		if ( mplsz != pk_cmpl->mplsz ) {
			return E_PAR;
		}
		/* Use user buffer */
		mempool = pk_cmpl->bufptr;
	} else {
		/* Allocate memory for memory pool */
		mempool = knl_Imalloc((UW)mplsz);
		if ( mempool == NULL ) {
			return E_NOMEM;
		}
	}
#else
	/* Size of user buffer must be multiples of sizeof(QUEUE)
		and larger than sizeof(QUEUE)*2 */
	if ( mplsz != pk_cmpl->mplsz ) {
		return E_PAR;
	}
	/* Use user buffer */
	mempool = pk_cmpl->bufptr;
#endif

	BEGIN_CRITICAL_SECTION;
	/* Get control block from FreeQue */
	mplcb = (MPLCB*)QueRemoveNext(&knl_free_mplcb);
	if ( mplcb == NULL ) {
		ercd = E_LIMIT;
	} else {
		mplid = ID_MPL(mplcb - knl_mplcb_table);

		/* Initialize control block */
		QueInit(&mplcb->wait_queue);
		mplcb->mplid  = mplid;
		mplcb->exinf  = pk_cmpl->exinf;
		mplcb->mplatr = pk_cmpl->mplatr;
		mplcb->mplsz  = mplsz;
#if USE_OBJECT_NAME
		if ( (pk_cmpl->mplatr & TA_DSNAME) != 0 ) {
			strncpy((char*)mplcb->name, (char*)pk_cmpl->dsname, OBJECT_NAME_LENGTH);
		}
#endif

		mplcb->mempool = mempool;

		/* Initialize memory pool */
		init_mempool(mplcb);

		ercd = mplid;
	}
	END_CRITICAL_SECTION;

#if USE_IMALLOC
	if ( (ercd < E_OK) && ((pk_cmpl->mplatr & TA_USERBUF) == 0) ) {
		knl_Ifree(mempool);
	}
#endif

	return ercd;
}
#endif /* USE_FUNC_TK_CRE_MPL */

#ifdef USE_FUNC_TK_DEL_MPL
/*
 * Delete variable size memory pool 
 */
SYSCALL ER tk_del_mpl_impl( ID mplid )
{
	MPLCB	*mplcb;
	void	*mempool = NULL;
	ATR	memattr = 0;
	ER	ercd = E_OK;

	CHECK_MPLID(mplid);
	CHECK_DISPATCH();

	mplcb = get_mplcb(mplid);

	BEGIN_CRITICAL_SECTION;
	if ( mplcb->mplid == 0 ) {
		ercd = E_NOEXS;
	} else {
		mempool = mplcb->mempool;
		memattr = mplcb->mplatr;

		/* Free wait state of task (E_DLT) */
		knl_wait_delete(&mplcb->wait_queue);

		/* Return to FreeQue */
		QueInsert(&mplcb->wait_queue, &knl_free_mplcb);
		mplcb->mplid = 0;
	}
	END_CRITICAL_SECTION;

#if USE_IMALLOC
	if ( (ercd == E_OK) && ((memattr & TA_USERBUF) == 0) ) {
		knl_Ifree(mempool);
	}
#endif

	return ercd;
}
#endif /* USE_FUNC_TK_DEL_MPL */

#ifdef USE_FUNC_TK_GET_MPL
/*
 * Processing if the priority of wait task changes.
 *	You need to execute with interrupt disable.
 */
LOCAL void mpl_chg_pri( TCB *tcb, INT oldpri )
{
	MPLCB	*mplcb;

	mplcb = get_mplcb(tcb->wid);
	if ( oldpri >= 0 ) {
		/* Reorder wait line */
		knl_gcb_change_priority((GCB*)mplcb, tcb);
	}

	/* From the new top task of a wait queue, free the assign
	   wait of memory blocks as much as possible. */
	knl_mpl_wakeup(mplcb);
}

/*
 * Processing if the wait task is freed
 */
LOCAL void mpl_rel_wai( TCB *tcb )
{
	mpl_chg_pri(tcb, -1);
}

/*
 * Definition of variable size memory pool wait specification
 */
LOCAL CONST WSPEC knl_wspec_mpl_tfifo = { TTW_MPL, NULL,        mpl_rel_wai };
LOCAL CONST WSPEC knl_wspec_mpl_tpri  = { TTW_MPL, mpl_chg_pri, mpl_rel_wai };

/*
 * Get variable size memory block 
 */
SYSCALL ER tk_get_mpl_impl( ID mplid, SZ blksz, void **p_blk, TMO tmout )
{
	MPLCB	*mplcb;
	void	*blk = NULL;
	ER	ercd = E_OK;

	CHECK_MPLID(mplid);
	CHECK_PAR(blksz > 0);
	CHECK_TMOUT(tmout);
	CHECK_DISPATCH();

	mplcb = get_mplcb(mplid);
	blksz = roundSize(blksz);

	BEGIN_CRITICAL_SECTION;
	if ( mplcb->mplid == 0 ) {
		ercd = E_NOEXS;
		goto error_exit;
	}

#if CHK_PAR
	if ( blksz > mplcb->mplsz ) {
		ercd = E_PAR;
		goto error_exit;
	}
#endif

	if ( knl_gcb_top_of_wait_queue((GCB*)mplcb, knl_ctxtsk) == knl_ctxtsk
	  && (blk = knl_get_blk(mplcb, blksz)) != NULL ) {
		/* Get memory block */
		*p_blk = blk;
	} else {
		/* Ready for wait */
		knl_ctxtsk->wspec = ( (mplcb->mplatr & TA_TPRI) != 0 )?
					&knl_wspec_mpl_tpri: &knl_wspec_mpl_tfifo;
		knl_ctxtsk->wercd = &ercd;
		knl_ctxtsk->winfo.mpl.blksz = blksz;
		knl_ctxtsk->winfo.mpl.p_blk = p_blk;
		knl_gcb_make_wait((GCB*)mplcb, tmout);
	}

    error_exit:
	END_CRITICAL_SECTION;

	return ercd;
}
#endif /* USE_FUNC_TK_GET_MPL */

#ifdef USE_FUNC_TK_REL_MPL
/*
 * Return variable size memory block 
 */
SYSCALL ER tk_rel_mpl_impl( ID mplid, void *blk )
{
	MPLCB	*mplcb;
	ER	ercd = E_OK;

	CHECK_MPLID(mplid);
	CHECK_DISPATCH();

	mplcb = get_mplcb(mplid);

	BEGIN_CRITICAL_SECTION;
	if ( mplcb->mplid == 0 ) {
		ercd = E_NOEXS;
		goto error_exit;
	}
#if CHK_PAR
	if ( (B*)blk < (B*)mplcb->mempool || (B*)blk > (B*)mplcb->mempool + mplcb->mplsz ) {
		ercd = E_PAR;
		goto error_exit;
	}
#endif

	/* Free memory block */
	ercd = knl_rel_blk(mplcb, blk);
	if ( ercd < E_OK ) {
		goto error_exit;
	}

	/* Assign memory block to waiting task */
	knl_mpl_wakeup(mplcb);

    error_exit:
	END_CRITICAL_SECTION;

	return ercd;
}
#endif /* USE_FUNC_TK_REL_MPL */

#ifdef USE_FUNC_TK_REF_MPL
/*
 * Refer variable size memory pool state
 */
SYSCALL ER tk_ref_mpl_impl( ID mplid, T_RMPL *pk_rmpl )
{
	MPLCB	*mplcb;
	QUEUE	*fq, *q;
	W	frsz, blksz;
	ER	ercd = E_OK;

	CHECK_MPLID(mplid);
	CHECK_DISPATCH();

	mplcb = get_mplcb(mplid);

	BEGIN_CRITICAL_SECTION;
	if ( mplcb->mplid == 0 ) {
		ercd = E_NOEXS;
	} else {
		pk_rmpl->exinf = mplcb->exinf;
		pk_rmpl->wtsk  = knl_wait_tskid(&mplcb->wait_queue);
		frsz = 0;
		for ( fq = mplcb->freeque.next; fq != &mplcb->freeque; fq = fq->next ) {
			blksz = FreeSize(fq);
			frsz += blksz;
			for ( q = (fq+1)->next; q != NULL; q = q->next ) {
				frsz += blksz;
			}
		}
		pk_rmpl->frsz  = frsz;
		pk_rmpl->maxsz = knl_MaxFreeSize(mplcb);
	}
	END_CRITICAL_SECTION;

	return ercd;
}
#endif /* USE_FUNC_TK_REF_MPL */

/* ------------------------------------------------------------------------ */
/*
 *	Debugger support function
 */
#if USE_DBGSPT

#ifdef USE_FUNC_MEMORYPOOL_GETNAME
#if USE_OBJECT_NAME
/*
 * Get object name from control block
 */
EXPORT ER knl_memorypool_getname(ID id, UB **name)
{
	MPLCB	*mplcb;
	ER	ercd = E_OK;

	CHECK_MPLID(id);

	BEGIN_DISABLE_INTERRUPT;
	mplcb = get_mplcb(id);
	if ( mplcb->mplid == 0 ) {
		ercd = E_NOEXS;
		goto error_exit;
	}
	if ( (mplcb->mplatr & TA_DSNAME) == 0 ) {
		ercd = E_OBJ;
		goto error_exit;
	}
	*name = mplcb->name;

    error_exit:
	END_DISABLE_INTERRUPT;

	return ercd;
}
#endif /* USE_OBJECT_NAME */
#endif /* USE_FUNC_FIX_MEMORYPOOL_GETNAME */

#ifdef USE_FUNC_TD_LST_MPL
/*
 * Refer variable size memory pool usage state
 */
SYSCALL INT td_lst_mpl_impl( ID list[], INT nent )
{
	MPLCB	*mplcb, *end;
	INT	n = 0;

	BEGIN_DISABLE_INTERRUPT;
	end = knl_mplcb_table + NUM_MPLID;
	for ( mplcb = knl_mplcb_table; mplcb < end; mplcb++ ) {
		if ( mplcb->mplid == 0 ) {
			continue;
		}

		if ( n++ < nent ) {
			*list++ = ID_MPL(mplcb - knl_mplcb_table);
		}
	}
	END_DISABLE_INTERRUPT;

	return n;
}
#endif /* USE_FUNC_TD_LST_MPL */

#ifdef USE_FUNC_TD_REF_MPL
/*
 * Refer variable size memory pool state
 */
SYSCALL ER td_ref_mpl_impl( ID mplid, TD_RMPL *pk_rmpl )
{
	MPLCB	*mplcb;
	QUEUE	*fq, *q;
	W	frsz, blksz;
	ER	ercd = E_OK;

	CHECK_MPLID(mplid);

	mplcb = get_mplcb(mplid);

	BEGIN_DISABLE_INTERRUPT;
	if ( mplcb->mplid == 0 ) {
		ercd = E_NOEXS;
	} else {
		pk_rmpl->exinf = mplcb->exinf;
		pk_rmpl->wtsk  = knl_wait_tskid(&mplcb->wait_queue);
		frsz = 0;
		for ( fq = mplcb->freeque.next; fq != &mplcb->freeque; fq = fq->next ) {
			blksz = FreeSize(fq);
			frsz += blksz;
			for ( q = (fq+1)->next; q != NULL; q = q->next ) {
				frsz += blksz;
			}
		}
		pk_rmpl->frsz  = frsz;
		pk_rmpl->maxsz = knl_MaxFreeSize(mplcb);
	}
	END_DISABLE_INTERRUPT;

	return ercd;
}
#endif /* USE_FUNC_TD_REF_MPL */

#ifdef USE_FUNC_TD_MPL_QUE
/*
 * Refer variable size memory pool wait queue 
 */
SYSCALL INT td_mpl_que_impl( ID mplid, ID list[], INT nent )
{
	MPLCB	*mplcb;
	QUEUE	*q;
	ER	ercd = E_OK;

	CHECK_MPLID(mplid);

	mplcb = get_mplcb(mplid);

	BEGIN_DISABLE_INTERRUPT;
	if ( mplcb->mplid == 0 ) {
		ercd = E_NOEXS;
	} else {
		INT n = 0;
		for ( q = mplcb->wait_queue.next; q != &mplcb->wait_queue; q = q->next ) {
			if ( n++ < nent ) {
				*list++ = ((TCB*)q)->tskid;
			}
		}
		ercd = n;
	}
	END_DISABLE_INTERRUPT;

	return ercd;
}
#endif /* USE_FUNC_TD_MPL_QUE */

#endif /* USE_DBGSPT */
#endif /* CFN_MAX_MPLID */
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
 *	mempool.h
 *	Variable Size Memory Pool
 */

#ifndef _MEMPOOL_H_
#define _MEMPOOL_H_

/*
 * Variable size memory pool control block
 *	'areaque' connects memory blocks in address ascending order
 *	'freeque' connects memory blocks in size increasing order
 *
 *  Order of members must not be changed because a part of members
 *  are used with casting to IMACB.
 */
typedef struct memorypool_control_block {
	QUEUE	wait_queue;	/* Memory pool wait queue */
	ID	mplid;		/* Variable size memory pool ID */
	void	*exinf;		/* Extended information */
	ATR	mplatr;		/* Memory pool attribute */
	W	mplsz;		/* Whole memory pool size */
	QUEUE	areaque;	/* Queue connecting all blocks */
	QUEUE	freeque;	/* Queue connecting free blocks */
	QUEUE	areaque_end;	/* the last element of areaque */
	void	*mempool;	/* Top address of memory pool */
#if USE_OBJECT_NAME
	UB	name[OBJECT_NAME_LENGTH];	/* name */
#endif
} MPLCB;

IMPORT MPLCB knl_mplcb_table[];	/* Variable size memory pool control block */
IMPORT QUEUE knl_free_mplcb;	/* FreeQue */

#define get_mplcb(id)	( &knl_mplcb_table[INDEX_MPL(id)] )


/*
 * Maximum free area size
 */
W knl_MaxFreeSize( MPLCB *mplcb )
{
	if ( isQueEmpty(&mplcb->freeque) ) {
		return 0;
	}
	return FreeSize(mplcb->freeque.prev);
}

/*
 * Definition of variable size memory pool wait specification
 */
IMPORT void knl_appendFreeAreaBound( MPLCB *mplcb, QUEUE *aq );
IMPORT void *knl_get_blk( MPLCB *mplcb, W blksz );
IMPORT ER knl_rel_blk( MPLCB *mplcb, void *blk );
IMPORT void knl_mpl_wakeup( MPLCB *mplcb );

#endif /* _MEMPOOL_H_ */
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
 *	messagebuf.c
 *	Message Buffer
 */

/** [BEGIN Common Definitions] */
#include "kernel.h"
#include "task.h"
#include "wait.h"
#include "check.h"
#include "messagebuf.h"
/** [END Common Definitions] */

#if CFN_MAX_MBFID > 0


#ifdef USE_FUNC_MBFCB_TABLE
Noinit(EXPORT MBFCB knl_mbfcb_table[NUM_MBFID]);	/* Message buffer control block */
Noinit(EXPORT QUEUE knl_free_mbfcb);	/* FreeQue */
#endif /* USE_FUNC_MBFCB_TABLE */


#ifdef USE_FUNC_MESSAGEBUFFER_INITIALIZE
/*
 * Initialization of message buffer control block 
 */
EXPORT ER knl_messagebuffer_initialize( void )
{
	MBFCB	*mbfcb, *end;

	/* Get system information */
	if ( NUM_MBFID < 1 ) {
		return E_SYS;
	}

	/* Register all control blocks onto FreeQue */
	QueInit(&knl_free_mbfcb);
	end = knl_mbfcb_table + NUM_MBFID;
	for ( mbfcb = knl_mbfcb_table; mbfcb < end; mbfcb++ ) {
		mbfcb->mbfid = 0;
		QueInsert(&mbfcb->send_queue, &knl_free_mbfcb);
	}

	return E_OK;
}
#endif /* USE_FUNC_MESSAGEBUFFER_INITIALIZE */

/* ------------------------------------------------------------------------ */

#ifdef USE_FUNC_MSG_TO_MBF
/*
 * Store the message to message buffer.
 */
EXPORT void knl_msg_to_mbf( MBFCB *mbfcb, CONST void *msg, INT msgsz )
{
	W	tail = mbfcb->tail;
	VB	*buffer = mbfcb->buffer;
	W	remsz;

	mbfcb->frbufsz -= (W)(HEADERSZ + ROUNDSZ(msgsz));

	*(HEADER*)&buffer[tail] = msgsz;
	tail += HEADERSZ;
	if ( tail >= mbfcb->bufsz ) {
		tail = 0;
	}

	if ( (remsz = mbfcb->bufsz - tail) < (W)msgsz ) {
		memcpy(&buffer[tail], msg, (size_t)remsz);
		msg = (VB*)msg + remsz;
		msgsz -= (INT)remsz;
		tail = 0;
	}
	memcpy(&buffer[tail], msg, (size_t)msgsz);
	tail += (W)ROUNDSZ(msgsz);
	if ( tail >= mbfcb->bufsz ) {
		tail = 0;
	}

	mbfcb->tail = tail;
}
#endif /* USE_FUNC_MSG_TO_MBF */

/* ------------------------------------------------------------------------ */

#ifdef USE_FUNC_MBF_WAKEUP
/*
 * Accept message and release wait task,
 * as long as there are free message area.
 */
EXPORT void knl_mbf_wakeup( MBFCB *mbfcb )
{
	TCB	*top;
	INT	msgsz;

	while ( !isQueEmpty(&mbfcb->send_queue) ) {
		top = (TCB*)mbfcb->send_queue.next;
		msgsz = top->winfo.smbf.msgsz;
		if ( !knl_mbf_free(mbfcb, msgsz) ) {
			break;
		}

		/* Store a message from waiting task and release it */
		knl_msg_to_mbf(mbfcb, top->winfo.smbf.msg, msgsz);
		knl_wait_release_ok(top);
	}
}
#endif /* USE_FUNC_MBF_WAKEUP */


#ifdef USE_FUNC_TK_CRE_MBF
/*
 * Create message buffer
 */
SYSCALL ID tk_cre_mbf_impl( CONST T_CMBF *pk_cmbf )
{
#if CHK_RSATR
	const ATR VALID_MBFATR = {
		 TA_TPRI
		|TA_USERBUF
#if USE_OBJECT_NAME
		|TA_DSNAME
#endif
	};
#endif
	MBFCB	*mbfcb;
	ID	mbfid;
	W	bufsz;
	VB	*msgbuf;
	ER	ercd;

	CHECK_RSATR(pk_cmbf->mbfatr, VALID_MBFATR);
	CHECK_PAR(pk_cmbf->bufsz >= 0);
	CHECK_PAR(pk_cmbf->maxmsz > 0);
#if !USE_IMALLOC
	/* TA_USERBUF must be specified if configured in no Imalloc */
	CHECK_PAR((pk_cmbf->mbfatr & TA_USERBUF) != 0);
#endif
	bufsz = (W)ROUNDSZ(pk_cmbf->bufsz);

	if ( bufsz > 0 ) {
#if USE_IMALLOC
		if ( (pk_cmbf->mbfatr & TA_USERBUF) != 0 ) {
			/* Size of user buffer must be multiples of sizeof(HEADER) */
			if ( bufsz != pk_cmbf->bufsz ) {
				return E_PAR;
			}
			/* Use user buffer */
			msgbuf = (VB*) pk_cmbf->bufptr;
		} else {
			/* Allocate by kernel */
			msgbuf = knl_Imalloc((UW)bufsz);
			if ( msgbuf == NULL ) {
				return E_NOMEM;
			}
		}
#else
		/* Size of user buffer must be multiples of sizeof(HEADER) */
		if ( bufsz != pk_cmbf->bufsz ) {
			return E_PAR;
		}
		/* Use user buffer */
		msgbuf = (VB*) pk_cmbf->bufptr;
#endif
	} else {
		msgbuf = NULL;
	}

	BEGIN_CRITICAL_SECTION;
	/* Get control block from FreeQue */
	mbfcb = (MBFCB*)QueRemoveNext(&knl_free_mbfcb);
	if ( mbfcb == NULL ) {
		ercd = E_LIMIT;
	} else {
		mbfid = ID_MBF(mbfcb - knl_mbfcb_table);

		/* Initialize control block */
		QueInit(&mbfcb->send_queue);
		mbfcb->mbfid = mbfid;
		mbfcb->exinf = pk_cmbf->exinf;
		mbfcb->mbfatr = pk_cmbf->mbfatr;
		QueInit(&mbfcb->recv_queue);
		mbfcb->buffer = msgbuf;
		mbfcb->bufsz = mbfcb->frbufsz = bufsz;
		mbfcb->maxmsz = pk_cmbf->maxmsz;
		mbfcb->head = mbfcb->tail = 0;
#if USE_OBJECT_NAME
		if ( (pk_cmbf->mbfatr & TA_DSNAME) != 0 ) {
			strncpy((char*)mbfcb->name, (char*)pk_cmbf->dsname,
				OBJECT_NAME_LENGTH);
		}
#endif
		ercd = mbfid;
	}
	END_CRITICAL_SECTION;

#if USE_IMALLOC
	if ( (ercd < E_OK) && (msgbuf != NULL) && ((pk_cmbf->mbfatr & TA_USERBUF) == 0 ) ) {
		knl_Ifree(msgbuf);
	}
#endif

	return ercd;
}
#endif /* USE_FUNC_TK_CRE_MBF */

#ifdef USE_FUNC_TK_DEL_MBF
/*
 * Delete message buffer
 */
SYSCALL ER tk_del_mbf_impl( ID mbfid )
{
	MBFCB	*mbfcb;
	VB	*msgbuf = NULL;
	ER	ercd = E_OK;

	CHECK_MBFID(mbfid);

	mbfcb = get_mbfcb(mbfid);

	BEGIN_CRITICAL_SECTION;
	if ( mbfcb->mbfid == 0 ) {
		ercd = E_NOEXS;
	} else {
		msgbuf = mbfcb->buffer;

		/* Release wait state of task (E_DLT) */
		knl_wait_delete(&mbfcb->recv_queue);
		knl_wait_delete(&mbfcb->send_queue);

		/* Return to FreeQue */
		QueInsert(&mbfcb->send_queue, &knl_free_mbfcb);
		mbfcb->mbfid = 0;
	}
	END_CRITICAL_SECTION;

#if USE_IMALLOC
	if ( msgbuf != NULL && ((mbfcb->mbfatr & TA_USERBUF) == 0 ) ) {
		knl_Ifree(msgbuf);
	}
#endif

	return ercd;
}
#endif /* USE_FUNC_TK_DEL_MBF */

#ifdef USE_FUNC_TK_SND_MBF
/*
 * Processing if the priority of wait task changes
 */
LOCAL void knl_mbf_chg_pri( TCB *tcb, INT oldpri )
{
	MBFCB	*mbfcb;

	mbfcb = get_mbfcb(tcb->wid);
	if ( oldpri >= 0 ) {
		/* Reorder wait queue */
		knl_gcb_change_priority((GCB*)mbfcb, tcb);
	}

	/* If the new head task in a send wait queue is able to sent, 
	   send its message */
	knl_mbf_wakeup(mbfcb);
}

/*
 * Processing if the wait task is released
 */
LOCAL void knl_mbf_rel_wai( TCB *tcb )
{
	knl_mbf_chg_pri(tcb, -1);
}

/*
 * Definition of message buffer wait specification
 */
LOCAL CONST WSPEC knl_wspec_smbf_tfifo = { TTW_SMBF, NULL, knl_mbf_rel_wai };
LOCAL CONST WSPEC knl_wspec_smbf_tpri  = { TTW_SMBF, knl_mbf_chg_pri, knl_mbf_rel_wai };

/*
 * Send to message buffer
 */
SYSCALL ER tk_snd_mbf_impl( ID mbfid, CONST void *msg, INT msgsz, TMO tmout )
{
	MBFCB	*mbfcb;
	TCB	*tcb;
	ER	ercd = E_OK;

	CHECK_MBFID(mbfid);
	CHECK_PAR(msgsz > 0);
	CHECK_TMOUT(tmout);
	CHECK_DISPATCH_POL(tmout);

	mbfcb = get_mbfcb(mbfid);

	BEGIN_CRITICAL_SECTION;
	if ( mbfcb->mbfid == 0 ) {
		ercd = E_NOEXS;
		goto error_exit;
	}
#if CHK_PAR
	if ( msgsz > mbfcb->maxmsz ) {
		ercd = E_PAR;
		goto error_exit;
	}
#endif

	if ( !isQueEmpty(&mbfcb->recv_queue) ) {
		/* Send directly to the receive wait task */
		tcb = (TCB*)mbfcb->recv_queue.next;
		memcpy(tcb->winfo.rmbf.msg, msg, (size_t)msgsz);
		*tcb->winfo.rmbf.p_msgsz = msgsz;
		knl_wait_release_ok(tcb);

	} else if ( (in_indp() || knl_gcb_top_of_wait_queue((GCB*)mbfcb, knl_ctxtsk) == knl_ctxtsk)
		  &&(knl_mbf_free(mbfcb, msgsz)) ) {
		/* Store the message to message buffer */
		knl_msg_to_mbf(mbfcb, msg, msgsz);

	} else {
		ercd = E_TMOUT;
		if ( tmout != TMO_POL ) {
			/* Ready for send wait */
			knl_ctxtsk->wspec = ( (mbfcb->mbfatr & TA_TPRI) != 0 )?
					&knl_wspec_smbf_tpri: &knl_wspec_smbf_tfifo;
			knl_ctxtsk->wercd = &ercd;
			knl_ctxtsk->winfo.smbf.msg = msg;
			knl_ctxtsk->winfo.smbf.msgsz = msgsz;
			knl_gcb_make_wait((GCB*)mbfcb, tmout);
		}
	}

    error_exit:
	END_CRITICAL_SECTION;

	return ercd;
}
#endif /* USE_FUNC_TK_SND_MBF */

#ifdef USE_FUNC_TK_RCV_MBF

LOCAL CONST WSPEC knl_wspec_rmbf = { TTW_RMBF, NULL, NULL };

/*
 * Get a message from message buffer.
 * Return the message size.
 */
LOCAL INT knl_mbf_to_msg( MBFCB *mbfcb, void *msg )
{
	W	head = mbfcb->head;
	VB	*buffer = mbfcb->buffer;
	INT	msgsz, actsz;
	W	remsz;

	actsz = msgsz = *(HEADER*)&buffer[head];
	mbfcb->frbufsz += (W)(HEADERSZ + ROUNDSZ(msgsz));

	head += (W)HEADERSZ;
	if ( head >= mbfcb->bufsz ) {
		head = 0;
	}

	if ( (remsz = mbfcb->bufsz - head) < (W)msgsz ) {
		memcpy(msg, &buffer[head], (size_t)remsz);
		msg = (VB*)msg + remsz;
		msgsz -= (INT)remsz;
		head = 0;
	}
	memcpy(msg, &buffer[head], (size_t)msgsz);
	head += (INT)ROUNDSZ(msgsz);
	if ( head >= mbfcb->bufsz ) {
		head = 0;
	}

	mbfcb->head = head;

	return actsz;
}

/*
 * Receive from message buffer
 */
SYSCALL INT tk_rcv_mbf_impl( ID mbfid, void *msg, TMO tmout )
{
	MBFCB	*mbfcb;
	TCB	*tcb;
	INT	rcvsz;
	ER	ercd = E_OK;

	CHECK_MBFID(mbfid);
	CHECK_TMOUT(tmout);
	CHECK_DISPATCH();

	mbfcb = get_mbfcb(mbfid);

	BEGIN_CRITICAL_SECTION;
	if (mbfcb->mbfid == 0) {
		ercd = E_NOEXS;
		goto error_exit;
	}

	if ( !knl_mbf_empty(mbfcb) ) {
		/* Read from message buffer */
		rcvsz = knl_mbf_to_msg(mbfcb, msg);

		/* Accept message from sending task(s) */
		knl_mbf_wakeup(mbfcb);

	} else if ( !isQueEmpty(&mbfcb->send_queue) ) {
		/* Receive directly from send wait task */
		tcb = (TCB*)mbfcb->send_queue.next;
		rcvsz = tcb->winfo.smbf.msgsz;
		memcpy(msg, tcb->winfo.smbf.msg, (size_t)rcvsz);
		knl_wait_release_ok(tcb);
		knl_mbf_wakeup(mbfcb);
	} else {
		ercd = E_TMOUT;
		if ( tmout != TMO_POL ) {
			/* Ready for receive wait */
			knl_ctxtsk->wspec = &knl_wspec_rmbf;
			knl_ctxtsk->wid = mbfid;
			knl_ctxtsk->wercd = &ercd;
			knl_ctxtsk->winfo.rmbf.msg = msg;
			knl_ctxtsk->winfo.rmbf.p_msgsz = &rcvsz;
			knl_make_wait(tmout, mbfcb->mbfatr);
			QueInsert(&knl_ctxtsk->tskque, &mbfcb->recv_queue);
		}
	}

    error_exit:
	END_CRITICAL_SECTION;

	return ( ercd < E_OK )? ercd: rcvsz;
}
#endif /* USE_FUNC_TK_RCV_MBF */

#ifdef USE_FUNC_TK_REF_MBF
/*
 * Refer message buffer state
 */
SYSCALL ER tk_ref_mbf_impl( ID mbfid, T_RMBF *pk_rmbf )
{
	MBFCB	*mbfcb;
	TCB	*tcb;
	ER	ercd = E_OK;

	CHECK_MBFID(mbfid);

	mbfcb = get_mbfcb(mbfid);

	BEGIN_CRITICAL_SECTION;
	if ( mbfcb->mbfid == 0 ) {
		ercd = E_NOEXS;
	} else {
		pk_rmbf->exinf = mbfcb->exinf;
		pk_rmbf->wtsk = knl_wait_tskid(&mbfcb->recv_queue);
		pk_rmbf->stsk = knl_wait_tskid(&mbfcb->send_queue);
		if ( !knl_mbf_empty(mbfcb) ) {
			pk_rmbf->msgsz = *(HEADER*)&mbfcb->buffer[mbfcb->head];
		} else {
			if ( !isQueEmpty(&mbfcb->send_queue) ) {
				tcb = (TCB*)mbfcb->send_queue.next;
				pk_rmbf->msgsz = tcb->winfo.smbf.msgsz;
			} else {
				pk_rmbf->msgsz = 0;
			}
		}
		pk_rmbf->frbufsz = mbfcb->frbufsz;
		pk_rmbf->maxmsz = mbfcb->maxmsz;
	}
	END_CRITICAL_SECTION;

	return ercd;
}
#endif /* USE_FUNC_TK_REF_MBF */

/* ------------------------------------------------------------------------ */
/*
 *	Debugger support function
 */
#if USE_DBGSPT

#ifdef USE_FUNC_MESSAGEBUFFER_GETNAME
#if USE_OBJECT_NAME
/*
 * Get object name from control block
 */
EXPORT ER knl_messagebuffer_getname(ID id, UB **name)
{
	MBFCB	*mbfcb;
	ER	ercd = E_OK;

	CHECK_MBFID(id);

	BEGIN_DISABLE_INTERRUPT;
	mbfcb = get_mbfcb(id);
	if ( mbfcb->mbfid == 0 ) {
		ercd = E_NOEXS;
		goto error_exit;
	}
	if ( (mbfcb->mbfatr & TA_DSNAME) == 0 ) {
		ercd = E_OBJ;
		goto error_exit;
	}
	*name = mbfcb->name;

    error_exit:
	END_DISABLE_INTERRUPT;

	return ercd;
}
#endif /* USE_OBJECT_NAME */
#endif /* USE_FUNC_MESSAGEBUFFER_GETNAME */

#ifdef USE_FUNC_TD_LST_MBF
/*
 * Refer message buffer usage state
 */
SYSCALL INT td_lst_mbf_impl( ID list[], INT nent )
{
	MBFCB	*mbfcb, *end;
	INT	n = 0;

	BEGIN_DISABLE_INTERRUPT;
	end = knl_mbfcb_table + NUM_MBFID;
	for ( mbfcb = knl_mbfcb_table; mbfcb < end; mbfcb++ ) {
		if ( mbfcb->mbfid == 0 ) {
			continue;
		}

		if ( n++ < nent ) {
			*list++ = mbfcb->mbfid;
		}
	}
	END_DISABLE_INTERRUPT;

	return n;
}
#endif /* USE_FUNC_TD_LST_MBF */

#ifdef USE_FUNC_TD_REF_MBF
/*
 * Refer message buffer state
 */
SYSCALL ER td_ref_mbf_impl( ID mbfid, TD_RMBF *pk_rmbf )
{
	MBFCB	*mbfcb;
	TCB	*tcb;
	ER	ercd = E_OK;

	CHECK_MBFID(mbfid);

	mbfcb = get_mbfcb(mbfid);

	BEGIN_DISABLE_INTERRUPT;
	if ( mbfcb->mbfid == 0 ) {
		ercd = E_NOEXS;
	} else {
		pk_rmbf->exinf = mbfcb->exinf;
		pk_rmbf->wtsk = knl_wait_tskid(&mbfcb->recv_queue);
		pk_rmbf->stsk = knl_wait_tskid(&mbfcb->send_queue);
		if ( !knl_mbf_empty(mbfcb) ) {
			pk_rmbf->msgsz = *(HEADER*)&mbfcb->buffer[mbfcb->head];
		} else {
			if ( !isQueEmpty(&mbfcb->send_queue) ) {
				tcb = (TCB*)mbfcb->send_queue.next;
				pk_rmbf->msgsz = tcb->winfo.smbf.msgsz;
			} else {
				pk_rmbf->msgsz = 0;
			}
		}
		pk_rmbf->frbufsz = mbfcb->frbufsz;
		pk_rmbf->maxmsz = mbfcb->maxmsz;
	}
	END_DISABLE_INTERRUPT;

	return ercd;
}
#endif /* USE_FUNC_TD_REF_MBF */

#ifdef USE_FUNC_TD_SMBF_QUE
/*
 * Refer message buffer send wait queue
 */
SYSCALL INT td_smbf_que_impl( ID mbfid, ID list[], INT nent )
{
	MBFCB	*mbfcb;
	QUEUE	*q;
	ER	ercd = E_OK;

	CHECK_MBFID(mbfid);

	mbfcb = get_mbfcb(mbfid);

	BEGIN_DISABLE_INTERRUPT;
	if ( mbfcb->mbfid == 0 ) {
		ercd = E_NOEXS;
	} else {
		INT n = 0;
		for ( q = mbfcb->send_queue.next; q != &mbfcb->send_queue; q = q->next ) {
			if ( n++ < nent ) {
				*list++ = ((TCB*)q)->tskid;
			}
		}
		ercd = n;
	}
	END_DISABLE_INTERRUPT;

	return ercd;
}
#endif /* USE_FUNC_TD_SMBF_QUE */

#ifdef USE_FUNC_TD_RMBF_QUE
/*
 * Refer message buffer receive wait queue
 */
SYSCALL INT td_rmbf_que_impl( ID mbfid, ID list[], INT nent )
{
	MBFCB	*mbfcb;
	QUEUE	*q;
	ER	ercd = E_OK;

	CHECK_MBFID(mbfid);

	mbfcb = get_mbfcb(mbfid);

	BEGIN_DISABLE_INTERRUPT;
	if ( mbfcb->mbfid == 0 ) {
		ercd = E_NOEXS;
	} else {
		INT n = 0;
		for ( q = mbfcb->recv_queue.next; q != &mbfcb->recv_queue; q = q->next ) {
			if ( n++ < nent ) {
				*list++ = ((TCB*)q)->tskid;
			}
		}
		ercd = n;
	}
	END_DISABLE_INTERRUPT;

	return ercd;
}
#endif /* USE_FUNC_TD_RMBF_QUE */

#endif /* USE_DBGSPT */
#endif /* CFN_MAX_MBFID */
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
 *	messagebuf.h
 *	Message Buffer
 */

#ifndef _MESSAGEBUF_H_
#define _MESSAGEBUF_H_

/*
 * Message buffer control block
 *
 *	Because Receive wait task (TTW_MBF) and Send wait task (TTW_SMBF)
 *	do not co-exist for one message buffer, the wait queue may be
 *	allowed to share.
 *	However, when the size of message buffer is 0, it is difficult
 *	to judge the wait queue if it is for receive or send, 
 *	therefore do not use this method.
 */
typedef struct messagebuffer_control_block {
	QUEUE	send_queue;	/* Message buffer send wait queue */
	ID	mbfid;		/* message buffer ID */
	void	*exinf;		/* Extended information */
	ATR	mbfatr;		/* Message buffer attribute */
	QUEUE	recv_queue;	/* Message buffer receive wait queue */
	W	bufsz;		/* Message buffer size */
	INT	maxmsz;		/* Maximum length of message */
	W	frbufsz;	/* Free buffer size */
	W	head;		/* First message store address */
	W	tail;		/* Next to the last message store address */
	VB	*buffer;	/* Message buffer address */
#if USE_OBJECT_NAME
	UB	name[OBJECT_NAME_LENGTH];	/* name */
#endif
} MBFCB;

IMPORT MBFCB knl_mbfcb_table[];	/* Message buffer control block */
IMPORT QUEUE knl_free_mbfcb;	/* FreeQue */

#define get_mbfcb(id)	( &knl_mbfcb_table[INDEX_MBF(id)] )


/*
 * Message header format
 */
typedef INT		HEADER;
#define HEADERSZ	(sizeof(HEADER))

#define ROUNDSIZE	(sizeof(HEADER))
#define ROUNDSZ(sz)	(((UW)(sz) + (UW)(ROUNDSIZE-1)) & ~(UW)(ROUNDSIZE-1))

/*
 * Check message buffer free space
 *	If 'msgsz' message is able to be stored, return TRUE.
 */
BOOL knl_mbf_free( MBFCB *mbfcb, INT msgsz )
{
	return ( HEADERSZ + (UW)msgsz <= (UW)mbfcb->frbufsz );
}

/*
 * If message buffer is empty, return TRUE.
 */
BOOL knl_mbf_empty( MBFCB *mbfcb )
{
	return ( mbfcb->frbufsz == mbfcb->bufsz );
}


/*
 * Definition of message buffer wait specification
 */
IMPORT void knl_msg_to_mbf( MBFCB *mbfcb, CONST void *msg, INT msgsz );
IMPORT void knl_mbf_wakeup( MBFCB *mbfcb );

#endif /* _MESSAGEBUF_H_ */
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
 *	misc_calls.c
 *	Other System Calls
 */

/** [BEGIN Common Definitions] */
#include "kernel.h"
#include "task.h"
#include "check.h"
#include "misc_calls.h"
/** [END Common Definitions] */


#ifdef USE_FUNC_TK_REF_SYS
/*
 * Refer system state
 */
SYSCALL ER tk_ref_sys_impl( T_RSYS *pk_rsys )
{
	if ( in_indp() ) {
		pk_rsys->sysstat = TSS_INDP;
	} else {
		if ( in_qtsk() ) {
			pk_rsys->sysstat = TSS_QTSK;
		} else {
			pk_rsys->sysstat = TSS_TSK;
		}
		if ( in_loc() ) {
			pk_rsys->sysstat |= TSS_DINT;
		}
		if ( in_ddsp() ) {
			pk_rsys->sysstat |= TSS_DDSP;
		}
	}
	pk_rsys->runtskid = ( knl_ctxtsk != NULL )? knl_ctxtsk->tskid: 0;
	pk_rsys->schedtskid = ( knl_schedtsk != NULL )? knl_schedtsk->tskid: 0;

	return E_OK;
}
#endif /* USE_FUNC_TK_REF_SYS */

#ifdef USE_FUNC_TK_REF_VER
/*
 * Refer version information
 *	If there is no kernel version information,
 *	set 0 in each information. (Do NOT cause errors.)
 */
SYSCALL ER tk_ref_ver_impl( T_RVER *pk_rver )
{
	pk_rver->maker = (UH)CFN_VER_MAKER;	/* OS manufacturer */
	pk_rver->prid  = (UH)CFN_VER_PRID;	/* OS identification number */
	pk_rver->spver = (UH)CFN_VER_SPVER;	/* Specification version */
	pk_rver->prver = (UH)CFN_VER_PRVER;	/* OS product version */
	pk_rver->prno[0] = (UH)CFN_VER_PRNO1;	/* Product number */
	pk_rver->prno[1] = (UH)CFN_VER_PRNO2;	/* Product number */
	pk_rver->prno[2] = (UH)CFN_VER_PRNO3;	/* Product number */
	pk_rver->prno[3] = (UH)CFN_VER_PRNO4;	/* Product number */

	return E_OK;
}
#endif /* USE_FUNC_TK_REF_VER */

#ifdef USE_FUNC_LOWPOW_DISCNT
/*
 * Number of times for disabling power-saving mode switch
 *	If it is 0, the mode switch is enabled.
 */
EXPORT UINT	knl_lowpow_discnt = 0;
#endif /* USE_FUNC_LOWPOW_DISCNT */
/* ------------------------------------------------------------------------ */
/*
 *	Debugger support function
 */
#if USE_DBGSPT

/*
 * Hook routine address
 */
#ifdef USE_FUNC_HOOK_ENTERFN
Noinit(EXPORT FP knl_hook_enterfn);
Noinit(EXPORT FP knl_hook_leavefn);
#if TA_GP
Noinit(EXPORT void *knl_hook_svc_gp);
#endif
#endif /* USE_FUNC_HOOK_ENTERFN */

#ifdef USE_FUNC_HOOK_EXECFN
Noinit(EXPORT FP knl_hook_execfn);
Noinit(EXPORT FP knl_hook_stopfn);
#if TA_GP
Noinit(EXPORT void *knl_hook_dsp_gp);
#endif
#endif /* USE_FUNC_HOOK_EXECFN */

#ifdef USE_FUNC_HOOK_IENTERFN
Noinit(EXPORT FP knl_hook_ienterfn);
Noinit(EXPORT FP knl_hook_ileavefn);
#if TA_GP
Noinit(EXPORT void *knl_hook_int_gp);
#endif
#endif /* USE_FUNC_HOOK_IENTERFN */


#ifdef USE_FUNC_TD_HOK_SVC
/*
 * Set/Cancel system call/extended SVC hook routine
 */
SYSCALL ER td_hok_svc_impl P1( TD_HSVC *hsvc )
{
	BEGIN_DISABLE_INTERRUPT;
	if ( hsvc == NULL ) { /* Cancel system call hook routine */
		/* Cancel */
		knl_unhook_svc();
	} else {
		/* Set */
		knl_hook_enterfn = hsvc->enter;
		knl_hook_leavefn = hsvc->leave;
#if TA_GP
		knl_hook_svc_gp = gp;
#endif
		knl_hook_svc();
	}
	END_DISABLE_INTERRUPT;

	return E_OK;
}
#endif /* USE_FUNC_TD_HOK_SVC */

#ifdef USE_FUNC_TD_HOK_DSP
/*
 * Set/Cancel dispatcher hook routine
 */
SYSCALL ER td_hok_dsp_impl P1( TD_HDSP *hdsp )
{
	BEGIN_DISABLE_INTERRUPT;
	if ( hdsp == NULL ) { /* Cancel dispatcher hook routine */
		/* Cancel */
		knl_unhook_dsp();
	} else {
		/* Set */
		knl_hook_execfn = hdsp->exec;
		knl_hook_stopfn = hdsp->stop;
#if TA_GP
		knl_hook_dsp_gp = gp;
#endif
		knl_hook_dsp();
	}
	END_DISABLE_INTERRUPT;

	return E_OK;
}
#endif /* USE_FUNC_TD_HOK_DSP */

#ifdef USE_FUNC_TD_HOK_INT
/*
 * Set/Cancel EIT handler hook routine
 */
SYSCALL ER td_hok_int_impl P1( TD_HINT *hint )
{
#if USE_HLL_INTHDR
	BEGIN_DISABLE_INTERRUPT;
	if ( hint == NULL ) { /* Cancel interrupt handler hook routine */
		/* Cancel */
		knl_unhook_int();
	} else {
		/* Set */
		knl_hook_ienterfn = hint->enter;
		knl_hook_ileavefn = hint->leave;
#if TA_GP
		knl_hook_int_gp = gp;
#endif
		knl_hook_int();
	}
	END_DISABLE_INTERRUPT;

	return E_OK;
#else
	return E_NOSPT;
#endif /* USE_HLL_INTHDR */
}
#endif /* USE_FUNC_TD_HOK_INT */

#ifdef USE_FUNC_TD_REF_SYS
/*
 * Refer system state
 */
SYSCALL ER td_ref_sys_impl( TD_RSYS *pk_rsys )
{
	if ( in_indp() ) {
		pk_rsys->sysstat = TSS_INDP;
	} else {
		if ( in_qtsk() ) {
			pk_rsys->sysstat = TSS_QTSK;
		} else {
			pk_rsys->sysstat = TSS_TSK;
		}
		if ( in_loc() ) {
			pk_rsys->sysstat |= TSS_DINT;
		}
		if ( in_ddsp() ) {
			pk_rsys->sysstat |= TSS_DDSP;
		}
	}
	pk_rsys->runtskid = ( knl_ctxtsk != NULL )? knl_ctxtsk->tskid: 0;
	pk_rsys->schedtskid = ( knl_schedtsk != NULL )? knl_schedtsk->tskid: 0;

	return E_OK;
}
#endif /* USE_FUNC_TD_REF_SYS */

#endif /* USE_DBGSPT */
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
 *	misc_calls.h
 *	Other System Calls
 */

#ifndef _MISC_CALLS_H_
#define _MISC_CALLS_H_

IMPORT UINT	knl_lowpow_discnt;

/*
 * Hook routine address
 */
IMPORT FP knl_hook_enterfn;
IMPORT FP knl_hook_leavefn;
IMPORT FP knl_hook_execfn;
IMPORT FP knl_hook_stopfn;
IMPORT FP knl_hook_ienterfn;
IMPORT FP knl_hook_ileavefn;

#if TA_GP
IMPORT void *knl_hook_svc_gp;
IMPORT void *knl_hook_dsp_gp;
IMPORT void *knl_hook_int_gp;
#endif

/*
 * Hook enable/disable setting
 */
IMPORT void knl_hook_svc( void );
IMPORT void knl_unhook_svc( void );
IMPORT void knl_hook_dsp( void );
IMPORT void knl_unhook_dsp( void );
IMPORT void knl_hook_int( void );
IMPORT void knl_unhook_int( void );


#endif /* _MISC_CALLS_H_ */
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
 *	mutex.c
 *	Mutex
 */

/** [BEGIN Common Definitions] */
#include "kernel.h"
#include "task.h"
#include "wait.h"
#include "check.h"
#include "mutex.h"
/** [END Common Definitions] */

#if CFN_MAX_MTXID > 0

#ifdef USE_FUNC_MTXCB_TABLE
Noinit(EXPORT MTXCB	knl_mtxcb_table[NUM_MTXID]);	/* Mutex control block */
Noinit(EXPORT QUEUE	knl_free_mtxcb);	/* FreeQue */
#endif /* USE_FUNC_MTXCB_TABLE */


#ifdef USE_FUNC_MUTEX_INITIALIZE
/*
 * Initialization of mutex control block 
 */
EXPORT ER knl_mutex_initialize(void)
{
	MTXCB	*mtxcb, *end;

	/* Get system information */
	if ( NUM_MTXID < 1 ) {
		return E_SYS;
	}

	/* Register all control blocks onto FreeQue */
	QueInit(&knl_free_mtxcb);
	end = knl_mtxcb_table + NUM_MTXID;
	for( mtxcb = knl_mtxcb_table; mtxcb < end; mtxcb++ ) {
		mtxcb->mtxid = 0;
		QueInsert(&mtxcb->wait_queue, &knl_free_mtxcb);
	}

	return E_OK;
}
#endif /* USE_FUNC_MUTEX_INITIALIZE */


#ifdef USE_FUNC_RELEASE_MUTEX
/*
 * Release the lock and delete it from list, and then adjust the
 * priority of task.
 * Set the highest priority between listed below:
 *	(A) The highest priority in all mutexes in which 'tcb' task locks. 
 *	(B) The base priority of 'tcb' task.
 */
EXPORT void knl_release_mutex( TCB *tcb, MTXCB *relmtxcb )
{
	MTXCB	*mtxcb, **prev;
	INT	newpri, pri;

	/* (B) The base priority of task */
	newpri = tcb->bpriority;

	/* (A) The highest priority in mutex which is locked */
	pri = newpri;
	prev = &tcb->mtxlist;
	while ( (mtxcb = *prev) != NULL ) {
		if ( mtxcb == relmtxcb ) {
			/* Delete from list */
			*prev = mtxcb->mtxlist;
			continue;
		}

		switch ( mtxcb->mtxatr & TA_CEILING ) {
		  case TA_CEILING:
			pri = mtxcb->ceilpri;
			break;
		  case TA_INHERIT:
			if ( mtx_waited(mtxcb) ) {
				pri = mtx_head_pri(mtxcb);
			}
			break;
		  default: /* TA_TFIFO, TA_TPRI */
			/* nothing to do */
			break;
		}
		if ( newpri > pri ) {
			newpri = pri;
		}

		prev = &mtxcb->mtxlist;
	}

	if ( newpri != tcb->priority ) {
		/* Change priority of lock get task */
		knl_change_task_priority(tcb, newpri);
	}
}
#endif /* USE_FUNC_RELEASE_MUTEX */

#ifdef USE_FUNC_SIGNAL_ALL_MUTEX
/*
 * Free mutex when task is terminated
 *	Free all mutexes which the task holds.
 *	Do not need to handle mutex list and priority of terminated task.
 *	
 */
EXPORT void knl_signal_all_mutex( TCB *tcb )
{
	MTXCB	*mtxcb, *next_mtxcb;
	TCB	*next_tcb;

	next_mtxcb = tcb->mtxlist;
	while ( (mtxcb = next_mtxcb) != NULL ) {
		next_mtxcb = mtxcb->mtxlist;

		if ( mtx_waited(mtxcb) ) {
			next_tcb = (TCB*)mtxcb->wait_queue.next;

			/* Wake wait task */
			knl_wait_release_ok(next_tcb);

			/* Change mutex get task */
			mtxcb->mtxtsk = next_tcb;
			mtxcb->mtxlist = next_tcb->mtxlist;
			next_tcb->mtxlist = mtxcb;

			if ( (mtxcb->mtxatr & TA_CEILING) == TA_CEILING ) {
				if ( next_tcb->priority > mtxcb->ceilpri ) {
					/* Raise the priority for the task
					   that got lock to the highest
					   priority limit */
					knl_change_task_priority(next_tcb,
							mtxcb->ceilpri);
				}
			}
		} else {
			/* No wait task */
			mtxcb->mtxtsk = NULL;
		}
	}
}
#endif /* USE_FUNC_SIGNAL_ALL_MUTEX */

#ifdef USE_FUNC_CHG_PRI_MUTEX
/*
 * Limit the priority change by mutex at task priority change
 *    1.If the 'tcb' task locks mutex, cannot set lower priority than the 
 *	highest priority in all mutexes which hold lock. In such case, 
 *	return the highest priority of locked mutex. 
 *    2.If mutex with TA_CEILING attribute is locked or waiting to be locked, 
 *	cannot set higher priority than the lowest within the highest 
 *	priority limit of mutex with TA_CEILING attribute.
 *	In this case, return E_ILUSE.
 *    3.Other than above, return the 'priority'.
 */
EXPORT INT knl_chg_pri_mutex( TCB *tcb, INT priority )
{
	MTXCB	*mtxcb;
	INT	hi_pri, low_pri, pri;

	hi_pri  = priority;
	low_pri = int_priority(MIN_PRI);

	/* Mutex lock wait */
	if ( (tcb->state & TS_WAIT) != 0 && (tcb->wspec->tskwait & TTW_MTX) != 0 ) {
		mtxcb = get_mtxcb(tcb->wid);
		if ( (mtxcb->mtxatr & TA_CEILING) == TA_CEILING ) {
			pri = mtxcb->ceilpri;
			if ( pri > low_pri ) {
				low_pri = pri;
			}
		}
	}

	/* Locked Mutex */
	pri = hi_pri;
	for ( mtxcb = tcb->mtxlist; mtxcb != NULL; mtxcb = mtxcb->mtxlist ) {
		switch ( mtxcb->mtxatr & TA_CEILING ) {
		  case TA_CEILING:
			pri = mtxcb->ceilpri;
			if ( pri > low_pri ) {
				low_pri = pri;
			}
			break;
		  case TA_INHERIT:
			if ( mtx_waited(mtxcb) ) {
				pri = mtx_head_pri(mtxcb);
			}
			break;
		  default: /* TA_TFIFO, TA_TPRI */
			/* nothing to do */
			break;
		}
		if ( pri < hi_pri ) {
			hi_pri = pri;
		}
	}

	if ( priority < low_pri ) {
		return E_ILUSE;
	}
	return hi_pri;
}
#endif /* USE_FUNC_CHG_PRI_MUTEX */


#ifdef USE_FUNC_TK_CRE_MTX
/*
 * Create mutex
 */
SYSCALL ID tk_cre_mtx_impl( CONST T_CMTX *pk_cmtx )
{
#if CHK_RSATR
	const ATR VALID_MTXATR = {
		 TA_CEILING
#if USE_OBJECT_NAME
		|TA_DSNAME
#endif
	};
#endif
	MTXCB	*mtxcb;
	ID	mtxid;
	INT	ceilpri;
	ER	ercd;

	CHECK_RSATR(pk_cmtx->mtxatr, VALID_MTXATR);

	if ( (pk_cmtx->mtxatr & TA_CEILING) == TA_CEILING ) {
		CHECK_PRI(pk_cmtx->ceilpri);
		ceilpri = int_priority(pk_cmtx->ceilpri);
	} else {
		ceilpri = 0;
	}

	BEGIN_CRITICAL_SECTION;
	/* Get control block from FreeQue */
	mtxcb = (MTXCB*)QueRemoveNext(&knl_free_mtxcb);
	if ( mtxcb == NULL ) {
		ercd = E_LIMIT;
	} else {
		mtxid = ID_MTX(mtxcb - knl_mtxcb_table);

		/* Initialize control block */
		QueInit(&mtxcb->wait_queue);
		mtxcb->mtxid   = mtxid;
		mtxcb->exinf   = pk_cmtx->exinf;
		mtxcb->mtxatr  = pk_cmtx->mtxatr;
		mtxcb->ceilpri = ceilpri;
		mtxcb->mtxtsk  = NULL;
		mtxcb->mtxlist = NULL;
#if USE_OBJECT_NAME
		if ( (pk_cmtx->mtxatr & TA_DSNAME) != 0 ) {
			strncpy((char*)mtxcb->name, (char*)pk_cmtx->dsname,
				(UINT)OBJECT_NAME_LENGTH);
		}
#endif
		ercd = mtxid;
	}
	END_CRITICAL_SECTION;

	return ercd;
}
#endif /* USE_FUNC_TK_CRE_MTX */

#ifdef USE_FUNC_TK_DEL_MTX
/*
 * Delete mutex
 */
SYSCALL ER tk_del_mtx_impl( ID mtxid )
{
	MTXCB	*mtxcb;
	ER	ercd = E_OK;

	CHECK_MTXID(mtxid);

	mtxcb = get_mtxcb(mtxid);

	BEGIN_CRITICAL_SECTION;
	if ( mtxcb->mtxid == 0 ) {
		ercd = E_NOEXS;
	} else {
		/* If there is a task that holds mutex to delete,
		 * delete the mutex from the list
		 * and adjust the task priority if necessary.
		 */
		if ( mtxcb->mtxtsk != NULL ) {
			knl_release_mutex(mtxcb->mtxtsk, mtxcb);
		}

		/* Free wait state of task (E_DLT) */
		knl_wait_delete(&mtxcb->wait_queue);

		/* Return to FreeQue */
		QueInsert(&mtxcb->wait_queue, &knl_free_mtxcb);
		mtxcb->mtxid = 0;
	}
	END_CRITICAL_SECTION;

	return ercd;
}
#endif /* USE_FUNC_TK_DEL_MTX */


#ifdef USE_FUNC_TK_LOC_MTX
/*
 * Processing if the priority of wait task changes
 */
LOCAL void mtx_chg_pri( TCB *tcb, INT oldpri )
{
	MTXCB	*mtxcb;
	TCB	*mtxtsk;

	mtxcb = get_mtxcb(tcb->wid);
	knl_gcb_change_priority((GCB*)mtxcb, tcb);

	if ( (mtxcb->mtxatr & TA_CEILING) == TA_INHERIT ) {
		mtxtsk = mtxcb->mtxtsk;
		if ( mtxtsk->priority > tcb->priority ) {
			/* Since the highest priority of the lock wait task
			   became higher, raise the lock get task priority
			   higher */
			knl_change_task_priority(mtxtsk, tcb->priority);

		} else if ( mtxtsk->priority == oldpri ) {
			/* Since the highest priority of the lock wait task
			   might become lower, adjust this priority */
			reset_priority(mtxtsk);
		}
	}
}

/*
 * Processing if the wait task is released (For TA_INHERIT only)
 */
LOCAL void mtx_rel_wai( TCB *tcb )
{
	MTXCB	*mtxcb;
	TCB	*mtxtsk;

	mtxcb = get_mtxcb(tcb->wid);
	mtxtsk = mtxcb->mtxtsk;

	if ( mtxtsk->priority == tcb->priority ) {
		/* Since the highest priority of the lock wait task might 
		   become lower, adjust this priority */
		reset_priority(mtxtsk);
	}
}

/*
 * Definition of mutex wait specification
 */
LOCAL CONST WSPEC knl_wspec_mtx_tfifo   = { TTW_MTX, NULL, NULL };
LOCAL CONST WSPEC knl_wspec_mtx_tpri    = { TTW_MTX, mtx_chg_pri, NULL };
LOCAL CONST WSPEC knl_wspec_mtx_inherit = { TTW_MTX, mtx_chg_pri, mtx_rel_wai };

/*
 * Lock mutex
 */
SYSCALL ER tk_loc_mtx_impl( ID mtxid, TMO tmout )
{
	MTXCB	*mtxcb;
	TCB	*mtxtsk;
	ATR	mtxatr;
	ER	ercd = E_OK;

	CHECK_MTXID(mtxid);
	CHECK_TMOUT(tmout);
	CHECK_DISPATCH();

	mtxcb = get_mtxcb(mtxid);

	BEGIN_CRITICAL_SECTION;
	if ( mtxcb->mtxid == 0 ) {
		ercd = E_NOEXS;
		goto error_exit;
	}
	if ( mtxcb->mtxtsk == knl_ctxtsk ) {
		ercd = E_ILUSE;  /* Multiplexed lock */
		goto error_exit;
	}

	mtxatr = mtxcb->mtxatr & TA_CEILING;
	if ( mtxatr == TA_CEILING ) {
		if ( knl_ctxtsk->bpriority < mtxcb->ceilpri ) {
			/* Violation of highest priority limit */
			ercd = E_ILUSE;
			goto error_exit;
		}
	}

	mtxtsk = mtxcb->mtxtsk;
	if ( mtxtsk == NULL ) {
		/* Get lock */
		mtxcb->mtxtsk = knl_ctxtsk;
		mtxcb->mtxlist = knl_ctxtsk->mtxlist;
		knl_ctxtsk->mtxlist = mtxcb;

		if ( mtxatr == TA_CEILING ) {
			if ( knl_ctxtsk->priority > mtxcb->ceilpri ) {
				/* Raise its own task to the highest
				   priority limit */
				knl_change_task_priority(knl_ctxtsk, mtxcb->ceilpri);
			}
		}
	} else {
		ercd = E_TMOUT;
		if ( tmout == TMO_POL ) {
			goto error_exit;
		}

		if ( mtxatr == TA_INHERIT ) {
			if ( mtxtsk->priority > knl_ctxtsk->priority ) {
				/* Raise the priority of task during
				   locking to the same priority as its
				   own task */
				knl_change_task_priority(mtxtsk, knl_ctxtsk->priority);
			}
		}

		/* Ready for wait */
		knl_ctxtsk->wspec = ( mtxatr == TA_TFIFO   )? &knl_wspec_mtx_tfifo:
				( mtxatr == TA_INHERIT )? &knl_wspec_mtx_inherit:
							  &knl_wspec_mtx_tpri;
		knl_ctxtsk->wercd = &ercd;
		knl_ctxtsk->wid = mtxcb->mtxid;
		knl_make_wait(tmout, mtxcb->mtxatr);
		if ( mtxatr == TA_TFIFO ) {
			QueInsert(&knl_ctxtsk->tskque, &mtxcb->wait_queue);
		} else {
			knl_queue_insert_tpri(knl_ctxtsk, &mtxcb->wait_queue);
		}
	}

    error_exit:
	END_CRITICAL_SECTION;

	return ercd;
}
#endif /* USE_FUNC_TK_LOC_MTX */

#ifdef USE_FUNC_TK_UNL_MTX
/*
 * Unlock mutex
 */
SYSCALL ER tk_unl_mtx_impl( ID mtxid )
{
	MTXCB	*mtxcb;	
	TCB	*tcb;
	ER	ercd = E_OK;

	CHECK_MTXID(mtxid);
	CHECK_INTSK();

	mtxcb = get_mtxcb(mtxid);

	BEGIN_CRITICAL_SECTION;
	if ( mtxcb->mtxid == 0 ) {
		ercd = E_NOEXS;
		goto error_exit;
	}
	if ( mtxcb->mtxtsk != knl_ctxtsk ) {
		ercd = E_ILUSE;  /* This is not locked by its own task */
		goto error_exit;
	}

	/* Delete the mutex from the list,
	   and adjust its own task priority if necessary. */
	knl_release_mutex(knl_ctxtsk, mtxcb);

	if ( mtx_waited(mtxcb) ) {
		tcb = (TCB*)mtxcb->wait_queue.next;

		/* Release wait */
		knl_wait_release_ok(tcb);

		/* Change mutex get task */
		mtxcb->mtxtsk = tcb;
		mtxcb->mtxlist = tcb->mtxlist;
		tcb->mtxlist = mtxcb;

		if ( (mtxcb->mtxatr & TA_CEILING) == TA_CEILING ) {
			if ( tcb->priority > mtxcb->ceilpri ) {
				/* Raise the priority of the task that
				   got lock to the highest priority limit */
				knl_change_task_priority(tcb, mtxcb->ceilpri);
			}
		}
	} else {
		/* No wait task */
		mtxcb->mtxtsk = NULL;
	}

    error_exit:
	END_CRITICAL_SECTION;

	return ercd;
}
#endif /* USE_FUNC_TK_UNL_MTX */


#ifdef USE_FUNC_TK_REF_MTX
/*
 * Refer mutex state
 */
SYSCALL ER tk_ref_mtx_impl( ID mtxid, T_RMTX *pk_rmtx )
{
	MTXCB	*mtxcb;
	ER	ercd = E_OK;

	CHECK_MTXID(mtxid);

	mtxcb = get_mtxcb(mtxid);

	BEGIN_CRITICAL_SECTION;
	if ( mtxcb->mtxid == 0 ) {
		ercd = E_NOEXS;
	} else {
		pk_rmtx->exinf = mtxcb->exinf;
		pk_rmtx->htsk = ( mtxcb->mtxtsk != NULL )?
					mtxcb->mtxtsk->tskid: 0;
		pk_rmtx->wtsk = knl_wait_tskid(&mtxcb->wait_queue);
	}
	END_CRITICAL_SECTION;

	return ercd;
}
#endif /* USE_FUNC_TK_REF_MTX */

/* ------------------------------------------------------------------------ */
/*
 *	Debugger support function
 */
#if USE_DBGSPT

#ifdef USE_FUNC_MUTEX_GETNAME
#if USE_OBJECT_NAME
/*
 * Get object name from control block
 */
EXPORT ER knl_mutex_getname(ID id, UB **name)
{
	MTXCB	*mtxcb;
	ER	ercd = E_OK;

	CHECK_MTXID(id);

	BEGIN_DISABLE_INTERRUPT;
	mtxcb = get_mtxcb(id);
	if ( mtxcb->mtxid == 0 ) {
		ercd = E_NOEXS;
		goto error_exit;
	}
	if ( (mtxcb->mtxatr & TA_DSNAME) == 0 ) {
		ercd = E_OBJ;
		goto error_exit;
	}
	*name = mtxcb->name;

    error_exit:
	END_DISABLE_INTERRUPT;

	return ercd;
}
#endif /* USE_OBJECT_NAME */
#endif /* USE_FUNC_MUTEX_GETNAME */

#ifdef USE_FUNC_TD_LST_MTX
/*
 * Refer mutex usage state
 */
SYSCALL INT td_lst_mtx_impl( ID list[], INT nent )
{
	MTXCB	*mtxcb, *end;
	INT	n = 0;

	BEGIN_DISABLE_INTERRUPT;
	end = knl_mtxcb_table + NUM_MTXID;
	for ( mtxcb = knl_mtxcb_table; mtxcb < end; mtxcb++ ) {
		if ( mtxcb->mtxid == 0 ) {
			continue;
		}

		if ( n++ < nent ) {
			*list++ = mtxcb->mtxid;
		}
	}
	END_DISABLE_INTERRUPT;

	return n;
}
#endif /* USE_FUNC_TD_LST_MTX */

#ifdef USE_FUNC_TD_REF_MTX
/*
 * Refer mutex state
 */
SYSCALL ER td_ref_mtx_impl( ID mtxid, TD_RMTX *pk_rmtx )
{
	MTXCB	*mtxcb;
	ER	ercd = E_OK;

	CHECK_MTXID(mtxid);

	mtxcb = get_mtxcb(mtxid);

	BEGIN_DISABLE_INTERRUPT;
	if ( mtxcb->mtxid == 0 ) {
		ercd = E_NOEXS;
	} else {
		pk_rmtx->exinf = mtxcb->exinf;
		pk_rmtx->htsk = ( mtxcb->mtxtsk != NULL )?
					mtxcb->mtxtsk->tskid: 0;
		pk_rmtx->wtsk = knl_wait_tskid(&mtxcb->wait_queue);
	}
	END_DISABLE_INTERRUPT;

	return ercd;
}
#endif /* USE_FUNC_TD_REF_MTX */

#ifdef USE_FUNC_TD_MTX_QUE
/*
 * Refer mutex wait queue
 */
SYSCALL INT td_mtx_que_impl( ID mtxid, ID list[], INT nent )
{
	MTXCB	*mtxcb;
	QUEUE	*q;
	ER	ercd = E_OK;

	CHECK_MTXID(mtxid);

	mtxcb = get_mtxcb(mtxid);

	BEGIN_DISABLE_INTERRUPT;
	if ( mtxcb->mtxid == 0 ) {
		ercd = E_NOEXS;
	} else {
		INT n = 0;
		for ( q = mtxcb->wait_queue.next; q != &mtxcb->wait_queue; q = q->next ) {
			if ( n++ < nent ) {
				*list++ = ((TCB*)q)->tskid;
			}
		}
		ercd = n;
	}
	END_DISABLE_INTERRUPT;

	return ercd;
}
#endif /* USE_FUNC_TD_MTX_QUE */

#endif /* USE_DBGSPT */
#endif /* CFN_MAX_MTXID */
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
 *	mutex.h
 *	Mutex
 */

#ifndef _MUTEX_H_
#define _MUTEX_H_

#ifndef __mtxcb__
#define __mtxcb__
typedef struct mutex_control_block	MTXCB;
#endif

/*
 * Mutex control block
 */
struct mutex_control_block {
	QUEUE	wait_queue;	/* Mutex wait queue */
	ID	mtxid;		/* Mutex ID */
	void	*exinf;		/* Extended information */
	ATR	mtxatr;		/* Mutex attribute */
	UB	ceilpri;	/* Highest priority limit of mutex */
	TCB	*mtxtsk;	/* Mutex get task */
	MTXCB	*mtxlist;	/* Mutex get list */
#if USE_OBJECT_NAME
	UB	name[OBJECT_NAME_LENGTH];	/* name */
#endif
};

IMPORT MTXCB knl_mtxcb_table[];	/* Mutex control block */
IMPORT QUEUE knl_free_mtxcb;	/* FreeQue */

#define get_mtxcb(id)	( &knl_mtxcb_table[INDEX_MTX(id)] )


/*
 * If there is a mutex 'mtxcb' with the task of lock wait, it is TRUE
 */
#define mtx_waited(mtxcb)	( !isQueEmpty(&(mtxcb)->wait_queue) )

/*
 * Return the highest priority in the task of lock wait at mutex 'mtxcb'
 */
#define mtx_head_pri(mtxcb)	( ((TCB*)(mtxcb)->wait_queue.next)->priority )

/*
 * Reset priority of lock get task (For TA_INHERIT only)
 */
#define reset_priority(tcb)	knl_release_mutex((tcb), NULL)


IMPORT void knl_release_mutex( TCB *tcb, MTXCB *relmtxcb );

#endif /* _MUTEX_H_ */
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
 *	objname.c
 *	Object name support
 */

/** [BEGIN Common Definitions] */
#include "kernel.h"
#include <libstr.h>
/** [END Common Definitions] */

#if USE_DBGSPT

#if USE_OBJECT_NAME
#ifdef USE_FUNC_OBJECT_GETNAME
EXPORT ER knl_object_getname( UINT objtype, ID objid, UB **name)
{
	ER	ercd;

	switch (objtype) {
#if CFN_MAX_TSKID > 0
	  case TN_TSK:
		{
			IMPORT ER knl_task_getname(ID id, UB **name);
			ercd = knl_task_getname(objid, name);
			break;
		}
#endif

#if CFN_MAX_SEMID > 0
	  case TN_SEM:
		{
			IMPORT ER knl_semaphore_getname(ID id, UB **name);
			ercd = knl_semaphore_getname(objid, name);
			break;
		}
#endif

#if CFN_MAX_FLGID > 0
	  case TN_FLG:
		{
			IMPORT ER knl_eventflag_getname(ID id, UB **name);
			ercd = knl_eventflag_getname(objid, name);
			break;
		}
#endif

#if CFN_MAX_MBXID > 0
	  case TN_MBX:
		{
			IMPORT ER knl_mailbox_getname(ID id, UB **name);
			ercd = knl_mailbox_getname(objid, name);
			break;
		}
#endif

#if CFN_MAX_MBFID > 0
	  case TN_MBF:
		{
			IMPORT ER knl_messagebuffer_getname(ID id, UB **name);
			ercd = knl_messagebuffer_getname(objid, name);
			break;
		}
#endif

#if CFN_MAX_PORID > 0
	  case TN_POR:
		{
			IMPORT ER knl_rendezvous_getname(ID id, UB **name);
			ercd = knl_rendezvous_getname(objid, name);
			break;
		}
#endif

#if CFN_MAX_MTXID > 0
	  case TN_MTX:
		{
			IMPORT ER knl_mutex_getname(ID id, UB **name);
			ercd = knl_mutex_getname(objid, name);
			break;
		}
#endif

#if CFN_MAX_MPLID > 0
	  case TN_MPL:
		{
			IMPORT ER knl_memorypool_getname(ID id, UB **name);
			ercd = knl_memorypool_getname(objid, name);
			break;
		}
#endif

#if CFN_MAX_MPFID > 0
	  case TN_MPF:
		{
			IMPORT ER knl_fix_memorypool_getname(ID id, UB **name);
			ercd = knl_fix_memorypool_getname(objid, name);
			break;
		}
#endif

#if CFN_MAX_CYCID > 0
	  case TN_CYC:
		{
			IMPORT ER knl_cyclichandler_getname(ID id, UB **name);
			ercd = knl_cyclichandler_getname(objid, name);
			break;
		}
#endif

#if CFN_MAX_ALMID > 0
	  case TN_ALM:
		{
			IMPORT ER knl_alarmhandler_getname(ID id, UB **name);
			ercd = knl_alarmhandler_getname(objid, name);
			break;
		}
#endif

	  default:
		ercd = E_PAR;

	}

	return ercd;
}
#endif /* USE_FUNC_OBJECT_GETNAME */
#endif /* USE_OBJECT_NAME */

#ifdef USE_FUNC_TD_REF_DSNAME
#if USE_OBJECT_NAME
IMPORT ER knl_object_getname( UINT objtype, ID objid, UB **name);
#endif /* USE_OBJECT_NAME */

SYSCALL ER td_ref_dsname_impl( UINT type, ID id, UB *dsname )
{
#if USE_OBJECT_NAME
	UB	*name_cb;
	ER	ercd;

	ercd = knl_object_getname(type, id, &name_cb);
	if (ercd == E_OK) {
		strncpy((char*)dsname, (char*)name_cb, OBJECT_NAME_LENGTH);
	}

	return ercd;
#else
	return E_NOSPT;
#endif /* USE_OBJECT_NAME */
}
#endif /* USE_FUNC_TD_REF_DSNAME */

#ifdef USE_FUNC_TD_SET_DSNAME
#if USE_OBJECT_NAME
IMPORT ER knl_object_getname( UINT objtype, ID objid, UB **name);
#endif /* USE_OBJECT_NAME */

SYSCALL ER td_set_dsname_impl( UINT type, ID id, CONST UB *dsname )
{
#if USE_OBJECT_NAME
	UB	*name_cb;
	ER	ercd;

	ercd = knl_object_getname(type, id, &name_cb);
	if (ercd == E_OK) {
		strncpy((char*)name_cb, (char*)dsname, OBJECT_NAME_LENGTH);
	}

	return ercd;
#else
	return E_NOSPT;
#endif /* USE_OBJECT_NAME */
}
#endif /* USE_FUNC_TD_SET_DSNAME */

#endif /* USE_DBGSPT */
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
 *	offset.h (FM3)
 *	TCB Offset Definition
 */

#ifndef _OFFSET_
#define _OFFSET_

/* Adjust offset of TCB member variables in offset.h for cpu_support.S */
#include "config.h"

/*	TCB.wrdvno	*/
#if CFN_MAX_PORID > 0
#define TCBSZ_POR	(4)	/* = sizeof(RNO) */
#else
#define TCBSZ_POR	(0)
#endif

/*	TCB.mtxlist	*/
#if CFN_MAX_MTXID > 0
#define TCBSZ_MTX	(4)	/* = sizeof(MTXCB*) */
#else
#define TCBSZ_MTX	(0)
#endif

/*	TCB.winfo.xxx	*/
#if CFN_MAX_PORID > 0
#define TCBSZ_WINFO	(16)
#else
#if CFN_MAX_FLGID > 0
#define TCBSZ_WINFO	(12)
#else
#if CFN_MAX_MBFID > 0 || CFN_MAX_MPLID > 0
#define TCBSZ_WINFO	(8)
#else
#if CFN_MAX_SEMID > 0 || CFN_MAX_MBXID > 0 || CFN_MAX_MPFID > 0
#define TCBSZ_WINFO	(4)
#else
#define TCBSZ_WINFO	(0)
#endif
#endif
#endif
#endif

/*	TCB.stime, TCB.utime */
#if USE_DBGSPT && defined(USE_FUNC_TD_INF_TSK)
#define TCBSZ_EXECTIME	(8)
#else
#define TCBSZ_EXECTIME	(0)
#endif

#define _ALIGN_CPU(x)	(((x)+3)&0xFFFFFFFC)	/* Cortex-m4 : 32 bit CPU	*/
#define _ALIGN_64(x)	(((x)+7)&0xFFFFFFF8)	/* Struct use 64bit align */


/*----------------------------------------------------------------------*/
/*	machine dependent data						*/
/*----------------------------------------------------------------------*/
#define	TCB_winfo	(60)		/* tskque - wercd		*/
#define	TCB_wtmeb	_ALIGN_64(TCB_winfo+TCBSZ_WINFO)
#define	TCBsz_wtmeb2isstack	(24+TCBSZ_MTX+TCBSZ_POR+TCBSZ_EXECTIME)
					/* wtmeb - istack		*/
#define TCBSZ_GP	(0)		/* No global pointer support	*/

/*----------------------------------------------------------------------*/
/*	offset data in TCB						*/
/*----------------------------------------------------------------------*/
#define TCB_isstack	(TCB_wtmeb+TCBsz_wtmeb2isstack)
#define TCB_tskctxb	_ALIGN_CPU(TCB_isstack+4+TCBSZ_GP)

#define TCB_tskid	8
#define TCB_tskatr	16
#define TCB_state	35
#define CTXB_ssp	0

#endif /* _OFFSET_ */
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
 *	patch.c (FM3)
 *	System-dependent initialize process
 */
/*--------------------------------------------------------------------*/
/*  Include definition                                                */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Struct definition                                                 */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Constant definition                                               */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Variable definition                                               */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Macro definition                                                  */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Prototype declaration                                             */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Function definition                                               */
/*--------------------------------------------------------------------*/

#if USE_SYSDEPEND_PATCH1
/*
 * System-dependent processes (before start_system)
 */
EXPORT void sysdepend_patch1( void )
{
	/* Do nothing */
}
#endif

#if USE_SYSDEPEND_PATCH2
/*
 * System-dependent processes (after start_system)
 */
EXPORT void sysdepend_patch2( void )
{
	/* Do nothing */
}
#endif
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
 *	patch.h (FM3)
 *	System-dependent initialize process
 */

#ifndef _PATCH_
#define _PATCH_

/* Define '1' when using patch processes */
#define USE_SYSDEPEND_PATCH1	0
#define USE_SYSDEPEND_PATCH2	0

EXPORT void sysdepend_patch1( void );
EXPORT void sysdepend_patch2( void );

#endif /* _PATCH_ */
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
 *	power.c (FM3)
 *	Power-Saving Function
 */
/*--------------------------------------------------------------------*/
/*  Include definition                                                */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Struct definition                                                 */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Constant definition                                               */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Variable definition                                               */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Macro definition                                                  */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Prototype declaration                                             */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Function definition                                               */
/*--------------------------------------------------------------------*/

#include <tk_typedef.h>

/*
 * Switch to power-saving mode
 */
EXPORT void knl_low_pow( void )
{
}

/*
 * Move to suspend mode
 */
EXPORT void knl_off_pow( void )
{
}
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
 *	@(#)profile.h (sys)
 *
 *	Service Profile
 */

#ifndef __SYS_PROFILE_H__
#define __SYS_PROFILE_H__

#include <profile_common.h>

#if !(TK_SUPPORT_USERBUF || TK_SUPPORT_AUTOBUF)
# error "Either TK_SUPPORT_USERBUF or TK_SUPPORT_AUTOBUF MUST be defined as TRUE."
#endif

#if TK_MAX_TSKPRI < 16
# error "TK_MAX_TSKPRI MUST be greater than or equal to 16."
#endif

#if TK_WAKEUP_MAXCNT < 1
# error "TK_WAKEUP_MAXCNT MUST be greater than or equal to 1."
#endif

#if TK_SEMAPHORE_MAXCNT < 32767
# error "TK_WAKEUP_MAXCNT MUST be greater than or equal to 32767."
#endif

#if TK_SUSPEND_MAXCNT < 32767
# error "TK_WAKEUP_MAXCNT MUST be greater than or equal to 32767."
#endif

#if !(TK_MEM_RNG0 >= 0 && TK_MEM_RNG0 <= 3)
# error "TK_MEM_RNG0 has an invalid value."
#endif

#if !(TK_MEM_RNG1 >= 0 && TK_MEM_RNG1 <= 3 && TK_MEM_RNG0 <= TK_MEM_RNG1)
# error "TK_MEM_RNG1 has an invalid value."
#endif

#if !(TK_MEM_RNG2 >= 0 && TK_MEM_RNG2 <= 3 && TK_MEM_RNG1 <= TK_MEM_RNG2)
# error "TK_MEM_RNG1 has an invalid value."
#endif

#if !(TK_MEM_RNG3 >= 0 && TK_MEM_RNG3 <= 3 && TK_MEM_RNG2 <= TK_MEM_RNG3)
# error "TK_MEM_RNG1 has an invalid value."
#endif

#endif /* __SYS_PROFILE_H__ */
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
// #include <profile_depend.h>
// #endif

// #ifdef _APP_H8S2212_
// #include <profile_depend.h>
// #endif

// #ifdef _APP_MB9AF312K_
// #include <profile_depend.h>
// #endif

#ifdef _APP_RL78G13_R5F100ADASP_
#include <profile_depend.h>
#endif

#endif /* __SYS_PROFILE_COMMON_H__ */
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
 *	@(#)profile_depend.h (sys/sysdepend/app_mb9af312k)
 *
 *	System common information  
 */

#ifndef __SYS_CORE_PROFILE_DEPEND_H__
#define __SYS_CORE_PROFILE_DEPEND_H__

#include <machine.h>
#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif


/*
 * T-Kernel family
 * 	TK_STDTK:   Standard T-Kernel	(always FALSE)
 * 	TK_MICROTK: micro T-Kernel	(must be TRUE)
 */
#define TK_STDTK	FALSE
#define TK_MICROTK	TRUE

/*
 * Memory misalign access is permitted
 */
#define TK_ALLOW_MISALIGN	(ALLOW_MISALIGN)
/*
 * Is Big Endian (Must be defined)
 */
#define TK_BIGENDIAN		(BIGENDIAN)
/*
 * Use virtual address(physical memory != logical memory)
 */
#define TK_VIRTUAL_ADDRESS	(VIRTUAL_ADDRESS)
/*
 * Has virtual memory (non-resident memory exists.)
 */
#define TK_VIRTUAL_MEMORY	FALSE
/*
 * Use CPU Trap instruction for system call entry
 */
#define TK_TRAP_SVC		(USE_TRAP)
/*
 * Support of 64-bit data types (D, UD, VD)
 */
#define TK_HAS_DOUBLEWORD	FALSE
/*
 * Support of sub error code
 */
#define TK_SUPPORT_SERCD	FALSE
/*
 * Task has a separate system stack.
 */
#define TK_HAS_SYSSTACK		FALSE
/*
 * Support of FPU
 */
#define TK_SUPPORT_FPU		FALSE
/*
 * Support of co-processors
 */
#define TK_SUPPORT_COP0		FALSE  
#define TK_SUPPORT_COP1		FALSE
#define TK_SUPPORT_COP2		FALSE
#define TK_SUPPORT_COP3		FALSE
/*
 * Support of resource group
 */
#define TK_SUPPORT_RESOURCE	FALSE
/*
 * Support of user-specified buffer (TA_USERBUF)
 */
#define TK_SUPPORT_USERBUF	TRUE
/*
 * Support of automatic buffer allocation (No TA_USERBUF specification)
 */
#define TK_SUPPORT_AUTOBUF	TRUE
/*
 * Support of setting task slice time (tk_chg_slt)
 */
#define TK_SUPPORT_SLICETIME	FALSE
/*
 * Support of getting task statistics (tk_inf_tsk)
 */
#define TK_SUPPORT_TASKINF	FALSE
/*
 * Support of microsecond
 */
#define TK_SUPPORT_USEC		FALSE
/*
 * Support of task space
 */
#define TK_SUPPORT_TASKSPACE	FALSE
/*
 * Support of task event
 */
#define TK_SUPPORT_TASKEVENT	FALSE
/*
 * Support of disabling wait.
 */
#define TK_SUPPORT_DISWAI	FALSE
/*
 * Support of get/set register operation
 */
#define TK_SUPPORT_REGOPS	TRUE
/*
 * Support of assembly language function entry/exit
 */
#define TK_SUPPORT_ASM		FALSE
/*
 * Support of DS object names
 */
#define TK_SUPPORT_DSNAME	(USE_OBJECT_NAME)	
/*
 * Support of task exception
 */
#define TK_SUPPORT_TASKEXCEPTION	FALSE	

/*
 * Support of power management
 */
#define TK_SUPPORT_LOWPOWER	FALSE
/*
 * Support of subsystem event processing
 */
#define TK_SUPPORT_SSYEVENT	FALSE
/*
 * Support of Large Mass-storage Device (64-bit)
 */
#define TK_SUPPORT_LARGEDEV	FALSE
/*
 * Support of interrupt controller management.
 */
#define TK_SUPPORT_INTCTRL	TRUE
/*
 * Can specify interrupt priority level
 */
#define TK_HAS_ENAINTLEVEL	TRUE
/*
 * Support of get/set of CPU interrupt mask level
 */
#define TK_SUPPORT_CPUINTLEVEL	TRUE
/*
 * Support of get/set of interrupt controller interrupt mask level
 */
#define TK_SUPPORT_CTRLINTLEVEL	FALSE
/*
 * Supoprt of interrupt mode setting
 */
#define TK_SUPPORT_INTMODE	FALSE
/*
 * Support of getting system configuration information
 */
#define TK_SUPPORT_SYSCONF	FALSE
/*
 * Support of I/O port access
 */
#define TK_SUPPORT_IOPORT	TRUE
/*
 * Support of micro wait
 */
#define TK_SUPPORT_MICROWAIT	FALSE
/*
 * support of cache control
 */
#define TK_SUPPORT_CACHECTRL	FALSE
/*
 * Support of write-back cache
 */
#define TK_SUPPORT_WBCACHE	FALSE
/*
 * Support of write-through cache
 */
#define TK_SUPPORT_WTCACHE	FALSE
/*
 * Support of system memory allocation
 */
#define TK_SUPPORT_SYSMEMBLK	FALSE
/*
 * Support of memory allocation library
 */
#define TK_SUPPORT_MEMLIB	FALSE
/*
 * Support of address space management
 */
#define TK_SUPPORT_ADDRSPACE	FALSE
/*
 * Support of physical timer
 */
#define TK_SUPPORT_PTIMER	FALSE
/*
 * Support of T-Kernel/DS
 */
#define TK_SUPPORT_DBGSPT	(USE_DBGSPT)
/*
 * Version number of micro T-Kernel
 *	TK_SPECVER_MAJOR: Major Version number of micro T-Kernel
 *	TK_SPECVER_MINOR: micro T-Kernel minor version number
 */
#define TK_SPECVER_MAJOR	(CFN_VER_MAJOR)
#define TK_SPECVER_MINOR	(CFN_VER_MINOR)
#define TK_SPECVER		((TK_SPECVER_MAJOR << 8) | TK_SPECVER_MINOR)
/*
 * Maximum task priority (>= 16)
 */
#define TK_MAX_TSKPRI		(MAX_PRI)  
/*
 * Maximum queuing number of the task wakeup requests. (>= 1)
 */
#define TK_WAKEUP_MAXCNT	(+2147483647L)
/*
 * Maximum semaphore count (>= 32767)
 */
#define TK_SEMAPHORE_MAXCNT	(+2147483647L)
/*
 * Maximum queuing count of the forced wait of tasks.
 */
#define TK_SUSPEND_MAXCNT	(+2147483647L)
/*
 * Real memory protection level of TA_RNGn (0..3)
 */
#define TK_MEM_RNG0		0
#define TK_MEM_RNG1		0
#define TK_MEM_RNG2		0
#define TK_MEM_RNG3		0
/*
 * Maximum number of physical timers. (>= 0)
 */
#define TK_MAX_PTIMER		0


#ifdef __cplusplus
}
#endif
#endif /* __SYS_CORE_PROFILE_DEPEND_H__ */
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
 *	@(#)queue.h (sys)
 *
 *	Queuing operation
 */

#ifndef	__SYS_QUEUE_H__
#define __SYS_QUEUE_H__

#include <basic.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Double-link queue (ring)
 */
typedef struct queue {
	struct queue	*next;
	struct queue	*prev;
} QUEUE;

/*
 * Queue initialization 
 */
void QueInit( QUEUE *que )
{
	que->next = (struct queue *)que;
	que->prev = (struct queue *)que;
}

/*
 * TRUE if the queue is empty 
 */
BOOL isQueEmpty( QUEUE *que )
{
	return ( que->next == que )? TRUE: FALSE;
}

/*
 * Insert in queue 
 *	Inserts entry directly prior to que 
 */
void QueInsert( QUEUE *entry, QUEUE *que )
{
	entry->prev = (struct queue*) que->prev;
	entry->next = que;
	que->prev->next = entry;
	que->prev = entry;
}

/*
 * Delete from queue 
 *	Deletes entry from queue 
 *	No action is performed if entry is empty. 
 */
void QueRemove( QUEUE *entry )
{
	if ( entry->next != entry ) {
		entry->prev->next = (struct queue*) entry->next;
		entry->next->prev = (struct queue*) entry->prev;
	}
}

/*
 * Remove top entry 
 *	Deletes the entry directly after que from the queue,
 *	and returns the deleted entry.
 *	Returns NULL if que is empty.
 */
QUEUE* QueRemoveNext( QUEUE *que )
{
	QUEUE	*entry;

	if ( que->next == que ) {
		return NULL;
	}

	entry = que->next;
	que->next = (struct queue*)entry->next;
	entry->next->prev = que;

	return entry;
}

#ifdef __cplusplus
}
#endif
#endif /* __SYS_QUEUE_H__ */
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
 *	ready_queue.h
 *	Ready Queue Operation Routine
 */

#ifndef _READY_QUEUE_
#define _READY_QUEUE_

#include <libstr.h>
#include "bitop.h"

/*
 * Definition of ready queue structure 
 *	In the ready queue, the task queue 'tskque' is provided per priority.
 *	The task TCB is registered onto queue with the applicable priority.
 *	For effective ready queue search, the bitmap area 'bitmap' is provided
 *	to indicate whether there are tasks in task queue per priority.
 *	
 *	Also, to search a task at the highest priority in the ready queue  
 *    	effectively, put the highest task priority in the 'top_priority' field.
 *	If the ready queue is empty, set the value in this field to NUM_PRI. 
 *	In this case, to return '0' with refering 'tskque[top_priority]',
 *      there is 'null' field which is always '0'.
 *
 *	Multiple READY tasks with kernel lock do not exist at the same time.
 */

#define BITMAPSZ	( sizeof(UINT) * 8 )
#define NUM_BITMAP	( (NUM_PRI + BITMAPSZ - 1) / BITMAPSZ )

typedef	struct ready_queue {
	INT	top_priority;		/* Highest priority in ready queue */
	QUEUE	tskque[NUM_PRI];	/* Task queue per priority */
	TCB	*null;			/* When the ready queue is empty, */
	UINT	bitmap[NUM_BITMAP];	/* Bitmap area per priority */
	TCB	*klocktsk;	/* READY task with kernel lock */
} RDYQUE;

IMPORT RDYQUE	knl_ready_queue;

/*
 * Ready queue initialization
 */
void knl_ready_queue_initialize( RDYQUE *rq )
{
	INT	i;

	rq->top_priority = NUM_PRI;
	for ( i = 0; i < NUM_PRI; i++ ) {
		QueInit(&rq->tskque[i]);
	}
	rq->null = NULL;
	rq->klocktsk = NULL;
	memset(rq->bitmap, 0, sizeof(rq->bitmap));
}

/*
 * Return the highest priority task in ready queue
 */
TCB* knl_ready_queue_top( RDYQUE *rq )
{
	/* If there is a task at kernel lock, that is the highest priority task */
	if ( rq->klocktsk != NULL ) {
		return rq->klocktsk;
	}

	return (TCB*)rq->tskque[rq->top_priority].next;
}

/*
 * Return the priority of the highest priority task in the ready queue
 */
INT knl_ready_queue_top_priority( const RDYQUE *rq )
{
	return rq->top_priority;
}

/*
 * Insert task in ready queue
 *	Insert it at the end of the same priority tasks with task priority 
 *	indicated with 'tcb'. Set the applicable bit in the bitmap area and 
 *	update 'top_priority' if necessary. When updating 'top_priority,' 
 *	return TRUE, otherwise FALSE.
 */
BOOL knl_ready_queue_insert( RDYQUE *rq, TCB *tcb )
{
	INT	priority = tcb->priority;

	QueInsert(&tcb->tskque, &rq->tskque[priority]);
	knl_tstdlib_bitset(rq->bitmap, priority);

	if ( tcb->klocked ) {
		rq->klocktsk = tcb;
	}

	if ( priority < rq->top_priority ) {
		rq->top_priority = priority;
		return TRUE;
	}
	return FALSE;
}

/*
 * Insert task at head in ready queue 
 */
void knl_ready_queue_insert_top( RDYQUE *rq, TCB *tcb )
{
	INT	priority = tcb->priority;

	QueInsert(&tcb->tskque, rq->tskque[priority].next);
	knl_tstdlib_bitset(rq->bitmap, priority);

	if ( tcb->klocked ) {
		rq->klocktsk = tcb;
	}

	if ( priority < rq->top_priority ) {
		rq->top_priority = priority;
	}
}

/*
 * Delete task from ready queue
 *	Take out TCB from the applicable priority task queue, and if the task 
 *	queue becomes empty, clear the applicable bit from the bitmap area.
 *	In addition, update 'top_priority' if the deleted task had the highest 
 *	priority. In such case, use the bitmap area to search the second
 *	highest priority task.
 */
void knl_ready_queue_delete( RDYQUE *rq, TCB *tcb )
{
	INT	priority = tcb->priority;
	INT	i;

	if ( rq->klocktsk == tcb ) {
		rq->klocktsk = NULL;
	}

	QueRemove(&tcb->tskque);
	if ( tcb->klockwait ) {
		/* Delete from kernel lock wait queue */
		tcb->klockwait = FALSE;
		return;
	}
	if ( !isQueEmpty(&rq->tskque[priority]) ) {
		return;
	}

	knl_tstdlib_bitclr(rq->bitmap, priority);
	if ( priority != rq->top_priority ) {
		return;
	}

	i = knl_tstdlib_bitsearch1(rq->bitmap, priority, NUM_PRI - priority);
	if ( i >= 0 ) {
		rq->top_priority = priority + i;
	} else {
		rq->top_priority = NUM_PRI;
	}
}

/*
 * Move the task, whose ready queue priority is 'priority', at head of
 * queue to the end of queue. Do nothing, if the queue is empty.
 */
void knl_ready_queue_rotate( RDYQUE *rq, INT priority )
{
	QUEUE	*tskque = &rq->tskque[priority];
	TCB	*tcb;

	tcb = (TCB*)QueRemoveNext(tskque);
	if ( tcb != NULL ) {
		QueInsert((QUEUE*)tcb, tskque);
	}
}

/*
 * Put 'tcb' to the end of ready queue. 
 */
TCB* knl_ready_queue_move_last( RDYQUE *rq, TCB *tcb )
{
	QUEUE	*tskque = &rq->tskque[tcb->priority];

	QueRemove(&tcb->tskque);
	QueInsert(&tcb->tskque, tskque);

	return (TCB*)tskque->next;	/* New task at head of queue */
}

#endif /* _READY_QUEUE_ */
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
 *	rendezvous.c
 *	Rendezvous
 */


/** [BEGIN Common Definitions] */
#include "kernel.h"
#include "task.h"
#include "wait.h"
#include "check.h"
#include "rendezvous.h"
/** [END Common Definitions] */

#if CFN_MAX_PORID > 0


#ifdef USE_FUNC_PORCB_TABLE
Noinit(EXPORT PORCB knl_porcb_table[NUM_PORID]);	/* Rendezvous port control block */
Noinit(EXPORT QUEUE knl_free_porcb);	/* FreeQue */
#endif /* USE_FUNC_PORCB_TABLE */


#ifdef USE_FUNC_RENDEZVOUS_INITIALIZE
/* 
 * Initialization of port control block 
 */
EXPORT ER knl_rendezvous_initialize( void )
{
	PORCB	*porcb, *end;

	/* Get system information */
	if ( NUM_PORID < 1 ) {
		return E_SYS;
	}

	/* Register all control blocks onto FreeQue */
	QueInit(&knl_free_porcb);
	end = knl_porcb_table + NUM_PORID;
	for ( porcb = knl_porcb_table; porcb < end; porcb++ ) {
		porcb->porid = 0;
		QueInsert(&porcb->call_queue, &knl_free_porcb);
	}

	return E_OK;
}
#endif /* USE_FUNC_RENDEZVOUS_INITIALIZE */


#ifdef USE_FUNC_WSPEC_CAL
/*
 * Processing if the priority of send wait task changes
 */
LOCAL void cal_chg_pri( TCB *tcb, INT oldpri )
{
	PORCB	*porcb;

	porcb = get_porcb(tcb->wid);
	knl_gcb_change_priority((GCB*)porcb, tcb);
}

/*
 * Definition of rendezvous wait specification
 */
EXPORT CONST WSPEC knl_wspec_cal_tfifo = { TTW_CAL, NULL, NULL };
EXPORT CONST WSPEC knl_wspec_cal_tpri  = { TTW_CAL, cal_chg_pri, NULL };
#endif /* USE_FUNC_WSPEC_CAL */

#ifdef USE_FUNC_WSPEC_RDV
EXPORT CONST WSPEC knl_wspec_rdv       = { TTW_RDV, NULL, NULL };
#endif /* USE_FUNC_WSPEC_RDV */


#ifdef USE_FUNC_TK_CRE_POR
/*
 * Create rendezvous port
 */
SYSCALL ID tk_cre_por_impl( CONST T_CPOR *pk_cpor )
{
#if CHK_RSATR
	const ATR VALID_PORATR = {
		 TA_TPRI
#if USE_OBJECT_NAME
		|TA_DSNAME
#endif
	};
#endif
	PORCB	*porcb;
	ID	porid;
	ER	ercd;

	CHECK_RSATR(pk_cpor->poratr, VALID_PORATR);
	CHECK_PAR(pk_cpor->maxcmsz >= 0);
	CHECK_PAR(pk_cpor->maxrmsz >= 0);
	CHECK_INTSK();

	BEGIN_CRITICAL_SECTION;
	/* Get control block from FreeQue */
	porcb = (PORCB*)QueRemoveNext(&knl_free_porcb);
	if ( porcb == NULL ) {
		ercd = E_LIMIT;
	} else {
		porid = ID_POR(porcb - knl_porcb_table);

		/* Initialize control block */
		QueInit(&porcb->call_queue);
		porcb->porid = porid;
		porcb->exinf = pk_cpor->exinf;
		porcb->poratr = pk_cpor->poratr;
		QueInit(&porcb->accept_queue);
		porcb->maxcmsz = pk_cpor->maxcmsz;
		porcb->maxrmsz = pk_cpor->maxrmsz;
#if USE_OBJECT_NAME
		if ( (pk_cpor->poratr & TA_DSNAME) != 0 ) {
			strncpy((char*)porcb->name, (char*)pk_cpor->dsname,
				OBJECT_NAME_LENGTH);
		}
#endif
		ercd = porid;
	}
	END_CRITICAL_SECTION;

	return ercd;
}
#endif /* USE_FUNC_TK_CRE_POR */

#ifdef USE_FUNC_TK_DEL_POR
/*
 * Delete rendezvous port
 */
SYSCALL ER tk_del_por_impl( ID porid )
{
	PORCB	*porcb;
	ER	ercd = E_OK;

	CHECK_PORID(porid);
	CHECK_INTSK();

	porcb = get_porcb(porid);

	BEGIN_CRITICAL_SECTION;
	if ( porcb->porid == 0 ) {
		ercd = E_NOEXS;
	} else {
		/* Release wait state of task (E_DLT) */
		knl_wait_delete(&porcb->call_queue);
		knl_wait_delete(&porcb->accept_queue);

		/* Return to FreeQue */
		QueInsert(&porcb->call_queue, &knl_free_porcb);
		porcb->porid = 0;
	}
	END_CRITICAL_SECTION;

	return ercd;
}
#endif /* USE_FUNC_TK_DEL_POR */

#ifdef USE_FUNC_TK_CAL_POR
/*
 * Call rendezvous
 */
SYSCALL INT tk_cal_por_impl( ID porid, UINT calptn, void *msg, INT cmsgsz, TMO tmout )
{
	PORCB	*porcb;
	TCB	*tcb;
	QUEUE	*queue;
	RNO	rdvno;
	INT	rmsgsz;
	ER	ercd = E_OK;

	CHECK_PORID(porid);
	CHECK_PAR(calptn != 0);
	CHECK_PAR(cmsgsz >= 0);
	CHECK_TMOUT(tmout);
	CHECK_DISPATCH();

	porcb = get_porcb(porid);

	BEGIN_CRITICAL_SECTION;
	if ( porcb->porid == 0 ) {
		ercd = E_NOEXS;
		goto error_exit;
	}
#if CHK_PAR
	if ( cmsgsz > porcb->maxcmsz ) {
		ercd = E_PAR;
		goto error_exit;
	}
#endif

	/* Search accept wait task */
	queue = porcb->accept_queue.next;
	while ( queue != &porcb->accept_queue ) {
		tcb = (TCB*)queue;
		queue = queue->next;
		if ( (calptn & tcb->winfo.acp.acpptn) == 0 ) {
			continue;
		}

		/* Send message */
		rdvno = knl_gen_rdvno(knl_ctxtsk);
		if ( cmsgsz > 0 ) {
			memcpy(tcb->winfo.acp.msg, msg, (UINT)cmsgsz);
		}
		*tcb->winfo.acp.p_rdvno = rdvno;
		*tcb->winfo.acp.p_cmsgsz = cmsgsz;
		knl_wait_release_ok(tcb);

		/* Ready for rendezvous end wait */
		ercd = E_TMOUT;
		knl_ctxtsk->wspec = &knl_wspec_rdv;
		knl_ctxtsk->wid = 0;
		knl_ctxtsk->wercd = &ercd;
		knl_ctxtsk->winfo.rdv.rdvno = rdvno;
		knl_ctxtsk->winfo.rdv.msg = msg;
		knl_ctxtsk->winfo.rdv.maxrmsz = porcb->maxrmsz;
		knl_ctxtsk->winfo.rdv.p_rmsgsz = &rmsgsz;
		knl_make_wait(TMO_FEVR, porcb->poratr);
		QueInit(&knl_ctxtsk->tskque);

		goto error_exit;
	}

	/* Ready for rendezvous call wait */
	knl_ctxtsk->wspec = ( (porcb->poratr & TA_TPRI) != 0 )?
					&knl_wspec_cal_tpri: &knl_wspec_cal_tfifo;
	knl_ctxtsk->wercd = &ercd;
	knl_ctxtsk->winfo.cal.calptn = calptn;
	knl_ctxtsk->winfo.cal.msg = msg;
	knl_ctxtsk->winfo.cal.cmsgsz = cmsgsz;
	knl_ctxtsk->winfo.cal.p_rmsgsz = &rmsgsz;
	knl_gcb_make_wait((GCB*)porcb, tmout);

    error_exit:
	END_CRITICAL_SECTION;

	return ( ercd < E_OK )? ercd: rmsgsz;
}
#endif /* USE_FUNC_TK_CAL_POR */

#ifdef USE_FUNC_TK_ACP_POR

LOCAL CONST WSPEC knl_wspec_acp = { TTW_ACP, NULL, NULL };

/*
 * Accept rendezvous
 */
SYSCALL INT tk_acp_por_impl( ID porid, UINT acpptn, RNO *p_rdvno, void *msg, TMO tmout )
{
	PORCB	*porcb;
	TCB	*tcb;
	QUEUE	*queue;
	RNO	rdvno;
	INT	cmsgsz;
	ER	ercd = E_OK;

	CHECK_PORID(porid);
	CHECK_PAR(acpptn != 0);
	CHECK_TMOUT(tmout);
	CHECK_DISPATCH();

	porcb = get_porcb(porid);
    
	BEGIN_CRITICAL_SECTION;
	if ( porcb->porid == 0 ) {
		ercd = E_NOEXS;
		goto error_exit;
	}

	/* Search call wait task */
	queue = porcb->call_queue.next;
	while ( queue != &porcb->call_queue ) {
		tcb = (TCB*)queue;
		queue = queue->next;
		if ( (acpptn & tcb->winfo.cal.calptn) == 0 ) {
			continue;
		}

		/* Receive message */
		*p_rdvno = rdvno = knl_gen_rdvno(tcb);
		cmsgsz = tcb->winfo.cal.cmsgsz;
		if ( cmsgsz > 0 ) {
			memcpy(msg, tcb->winfo.cal.msg, (UINT)cmsgsz);
		}

		knl_wait_cancel(tcb);

		/* Make the other task at rendezvous end wait state */
		tcb->wspec = &knl_wspec_rdv;
		tcb->wid = 0;
		tcb->winfo.rdv.rdvno = rdvno;
		tcb->winfo.rdv.msg = tcb->winfo.cal.msg;
		tcb->winfo.rdv.maxrmsz = porcb->maxrmsz;
		tcb->winfo.rdv.p_rmsgsz = tcb->winfo.cal.p_rmsgsz;
		knl_timer_insert(&tcb->wtmeb, TMO_FEVR,
					(CBACK)knl_wait_release_tmout, tcb);
		QueInit(&tcb->tskque);

		goto error_exit;
	}

	ercd = E_TMOUT;
	if ( tmout != TMO_POL ) {
		/* Ready for rendezvous accept wait */
		knl_ctxtsk->wspec = &knl_wspec_acp;
		knl_ctxtsk->wid = porid;
		knl_ctxtsk->wercd = &ercd;
		knl_ctxtsk->winfo.acp.acpptn = acpptn;
		knl_ctxtsk->winfo.acp.msg = msg;
		knl_ctxtsk->winfo.acp.p_rdvno = p_rdvno;
		knl_ctxtsk->winfo.acp.p_cmsgsz = &cmsgsz;
		knl_make_wait(tmout, porcb->poratr);
		QueInsert(&knl_ctxtsk->tskque, &porcb->accept_queue);
	}

    error_exit:
	END_CRITICAL_SECTION;

	return ( ercd < E_OK )? ercd: cmsgsz;
}
#endif /* USE_FUNC_TK_ACP_POR */

#ifdef USE_FUNC_TK_FWD_POR
/*
 * Forward Rendezvous to Other Port
 */
SYSCALL ER tk_fwd_por_impl( ID porid, UINT calptn, RNO rdvno, CONST void *msg, INT cmsgsz )
{
	PORCB	*porcb;
	TCB	*caltcb, *tcb;
	QUEUE	*queue;
	RNO	new_rdvno;
	ER	ercd = E_OK;

	CHECK_PORID(porid);
	CHECK_PAR(calptn != 0);
	CHECK_RDVNO(rdvno);
	CHECK_PAR(cmsgsz >= 0);
	CHECK_INTSK();

	porcb = get_porcb(porid);
	caltcb = get_tcb(knl_get_tskid_rdvno(rdvno));

	BEGIN_CRITICAL_SECTION;
	if ( porcb->porid == 0 ) {
		ercd = E_NOEXS;
		goto error_exit;
	}
#if CHK_PAR
	if ( cmsgsz > porcb->maxcmsz ) {
		ercd = E_PAR;
		goto error_exit;
	}
#endif
	if ( (caltcb->state & TS_WAIT) == 0
	  || caltcb->wspec != &knl_wspec_rdv
	  || rdvno != caltcb->winfo.rdv.rdvno ) {
		ercd = E_OBJ;
		goto error_exit;
	}
	if ( porcb->maxrmsz > caltcb->winfo.rdv.maxrmsz ) {
		ercd = E_OBJ;
		goto error_exit;
	}
#if CHK_PAR
	if ( cmsgsz > caltcb->winfo.rdv.maxrmsz ) {
		ercd = E_PAR;
		goto error_exit;
	}
#endif

	/* Search accept wait task */
	queue = porcb->accept_queue.next;
	while ( queue != &porcb->accept_queue ) {
		tcb = (TCB*)queue;
		queue = queue->next;
		if ( (calptn & tcb->winfo.acp.acpptn) == 0 ) {
			continue;
		}

		/* Send message */
		new_rdvno = knl_gen_rdvno(caltcb);
		if ( cmsgsz > 0 ) {
			memcpy(tcb->winfo.acp.msg, msg, (UINT)cmsgsz);
		}
		*tcb->winfo.acp.p_rdvno = new_rdvno;
		*tcb->winfo.acp.p_cmsgsz = cmsgsz;
		knl_wait_release_ok(tcb);

		/* Change rendezvous end wait of the other task */
		caltcb->winfo.rdv.rdvno = new_rdvno;
		caltcb->winfo.rdv.msg = caltcb->winfo.cal.msg;
		caltcb->winfo.rdv.maxrmsz = porcb->maxrmsz;
		caltcb->winfo.rdv.p_rmsgsz = caltcb->winfo.cal.p_rmsgsz;

		goto error_exit;
	}

	/* Change the other task to rendezvous call wait */
	caltcb->wspec = ( (porcb->poratr & TA_TPRI) != 0 )?
				&knl_wspec_cal_tpri: &knl_wspec_cal_tfifo;
	caltcb->wid = porid;
	caltcb->winfo.cal.calptn = calptn;
	caltcb->winfo.cal.msg = caltcb->winfo.rdv.msg;
	caltcb->winfo.cal.cmsgsz = cmsgsz;
	caltcb->winfo.cal.p_rmsgsz = caltcb->winfo.rdv.p_rmsgsz;
	knl_timer_insert(&caltcb->wtmeb, TMO_FEVR,
			(CBACK)knl_wait_release_tmout, caltcb);
	if ( (porcb->poratr & TA_TPRI) != 0 ) {
		knl_queue_insert_tpri(caltcb, &porcb->call_queue);
	} else {
		QueInsert(&caltcb->tskque, &porcb->call_queue);
	}

	if ( cmsgsz > 0 ) {
		memcpy(caltcb->winfo.cal.msg, msg, (UINT)cmsgsz);
	}

    error_exit:
	END_CRITICAL_SECTION;

	return ercd;
}
#endif /* USE_FUNC_TK_FWD_POR */

#ifdef USE_FUNC_TK_RPL_RDV
/*
 * Reply rendezvous
 */
SYSCALL ER tk_rpl_rdv_impl( RNO rdvno, CONST void *msg, INT rmsgsz )
{
	TCB	*caltcb;
	ER	ercd = E_OK;

	CHECK_RDVNO(rdvno);
	CHECK_PAR(rmsgsz >= 0);
	CHECK_INTSK();

	caltcb = get_tcb(knl_get_tskid_rdvno(rdvno));

	BEGIN_CRITICAL_SECTION;
	if ( (caltcb->state & TS_WAIT) == 0
	  || caltcb->wspec != &knl_wspec_rdv
	  || rdvno != caltcb->winfo.rdv.rdvno ) {
		ercd = E_OBJ;
		goto error_exit;
	}
#if CHK_PAR
	if ( rmsgsz > caltcb->winfo.rdv.maxrmsz ) {
		ercd = E_PAR;
		goto error_exit;
	}
#endif

	/* Send message */
	if ( rmsgsz > 0 ) {
		memcpy(caltcb->winfo.rdv.msg, msg, (UINT)rmsgsz);
	}
	*caltcb->winfo.rdv.p_rmsgsz = rmsgsz;
	knl_wait_release_ok(caltcb);

    error_exit:
	END_CRITICAL_SECTION;

	return ercd;
}
#endif /* USE_FUNC_TK_RPL_RDV */

#ifdef USE_FUNC_TK_REF_POR
/*
 * Refer rendezvous port
 */
SYSCALL ER tk_ref_por_impl( ID porid, T_RPOR *pk_rpor )
{
	PORCB	*porcb;
	ER	ercd = E_OK;

	CHECK_PORID(porid);

	porcb = get_porcb(porid);

	BEGIN_CRITICAL_SECTION;
	if ( porcb->porid == 0 ) {
		ercd = E_NOEXS;
	} else {
		pk_rpor->exinf = porcb->exinf;
		pk_rpor->wtsk = knl_wait_tskid(&porcb->call_queue);
		pk_rpor->atsk = knl_wait_tskid(&porcb->accept_queue);
		pk_rpor->maxcmsz = porcb->maxcmsz;
		pk_rpor->maxrmsz = porcb->maxrmsz;
	}
	END_CRITICAL_SECTION;

	return ercd;
}
#endif /* USE_FUNC_TK_REF_POR */

/* ------------------------------------------------------------------------ */
/*
 *	Debugger support function
 */
#if USE_DBGSPT

#ifdef USE_FUNC_RENDEZVOUS_GETNAME
#if USE_OBJECT_NAME
/*
 * Get object name from control block
 */
EXPORT ER knl_rendezvous_getname(ID id, UB **name)
{
	PORCB	*porcb;
	ER	ercd = E_OK;

	CHECK_PORID(id);

	BEGIN_DISABLE_INTERRUPT;
	porcb = get_porcb(id);
	if ( porcb->porid == 0 ) {
		ercd = E_NOEXS;
		goto error_exit;
	}
	if ( (porcb->poratr & TA_DSNAME) == 0 ) {
		ercd = E_OBJ;
		goto error_exit;
	}
	*name = porcb->name;

    error_exit:
	END_DISABLE_INTERRUPT;

	return ercd;
}
#endif /* USE_OBJECT_NAME */
#endif /* USE_FUNC_RENDEZVOUS_GETNAME */

#ifdef USE_FUNC_TD_LST_POR
/*
 * Refer rendezvous port usage state
 */
SYSCALL INT td_lst_por_impl( ID list[], INT nent )
{
	PORCB	*porcb, *end;
	INT	n = 0;

	BEGIN_DISABLE_INTERRUPT;
	end = knl_porcb_table + NUM_PORID;
	for ( porcb = knl_porcb_table; porcb < end; porcb++ ) {
		if ( porcb->porid == 0 ) {
			continue;
		}

		if ( n++ < nent ) {
			*list++ = porcb->porid;
		}
	}
	END_DISABLE_INTERRUPT;

	return n;
}
#endif /* USE_FUNC_TD_LST_POR */

#ifdef USE_FUNC_TD_REF_POR
/*
 * Refer rendezvous port
 */
SYSCALL ER td_ref_por_impl( ID porid, TD_RPOR *pk_rpor )
{
	PORCB	*porcb;
	ER	ercd = E_OK;

	CHECK_PORID(porid);

	porcb = get_porcb(porid);

	BEGIN_DISABLE_INTERRUPT;
	if ( porcb->porid == 0 ) {
		ercd = E_NOEXS;
	} else {
		pk_rpor->exinf = porcb->exinf;
		pk_rpor->wtsk = knl_wait_tskid(&porcb->call_queue);
		pk_rpor->atsk = knl_wait_tskid(&porcb->accept_queue);
		pk_rpor->maxcmsz = porcb->maxcmsz;
		pk_rpor->maxrmsz = porcb->maxrmsz;
	}
	END_DISABLE_INTERRUPT;

	return ercd;
}
#endif /* USE_FUNC_TD_REF_POR */

#ifdef USE_FUNC_TD_CAL_QUE
/*
 * Refer rendezvous call wait queue
 */
SYSCALL INT td_cal_que_impl( ID porid, ID list[], INT nent )
{
	PORCB	*porcb;
	QUEUE	*q;
	ER	ercd = E_OK;

	CHECK_PORID(porid);

	porcb = get_porcb(porid);

	BEGIN_DISABLE_INTERRUPT;
	if ( porcb->porid == 0 ) {
		ercd = E_NOEXS;
	} else {
		INT n = 0;
		for ( q = porcb->call_queue.next; q != &porcb->call_queue; q = q->next ) {
			if ( n++ < nent ) {
				*list++ = ((TCB*)q)->tskid;
			}
		}
		ercd = n;
	}
	END_DISABLE_INTERRUPT;

	return ercd;
}
#endif /* USE_FUNC_TD_CAL_QUE */

#ifdef USE_FUNC_TD_ACP_QUE
/*
 * Refer rendezvous accept wait queue
 */
SYSCALL INT td_acp_que_impl( ID porid, ID list[], INT nent )
{
	PORCB	*porcb;
	QUEUE	*q;
	ER	ercd = E_OK;

	CHECK_PORID(porid);

	porcb = get_porcb(porid);

	BEGIN_DISABLE_INTERRUPT;
	if ( porcb->porid == 0 ) {
		ercd = E_NOEXS;
	} else {
		INT n = 0;
		for ( q = porcb->accept_queue.next; q != &porcb->accept_queue; q = q->next ) {
			if ( n++ < nent ) {
				*list++ = ((TCB*)q)->tskid;
			}
		}
		ercd = n;
	}
	END_DISABLE_INTERRUPT;

	return ercd;
}
#endif /* USE_FUNC_TD_ACP_QUE */

#endif /* USE_DBGSPT */
#endif /* CFN_MAX_PORID */
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
 *	rendezvous.h
 *	Rendezvous
 */

#ifndef _RENDEZVOUS_H_
#define _RENDEZVOUS_H_

/*
 * Rendezvous port control block
 */
typedef struct port_control_block {
	QUEUE	call_queue;	/* Port call wait queue */
	ID	porid;		/* Port ID */
	void	*exinf;		/* Extended information */
	ATR	poratr;		/* Port attribute */
	QUEUE	accept_queue;	/* Port accept wait queue */
	INT	maxcmsz;	/* Maximum length of call message */
	INT	maxrmsz;	/* Maximum length of reply message */
#if USE_OBJECT_NAME
	UB	name[OBJECT_NAME_LENGTH];	/* name */
#endif
} PORCB;
/** [END Common Definitions] */

IMPORT PORCB knl_porcb_table[];	/* Rendezvous port control block */
IMPORT QUEUE knl_free_porcb;	/* FreeQue */

#define get_porcb(id)	( &knl_porcb_table[INDEX_POR(id)] )


#if CFN_MAX_PORID > 0

#define RDVNO_SHIFT	(sizeof(RNO)*8/2)

/*
 * Create rendezvous number
 */
RNO knl_gen_rdvno( TCB *tcb )
{
	RNO	rdvno;

	rdvno = tcb->wrdvno;
	tcb->wrdvno += (1U << RDVNO_SHIFT);

	return rdvno;
}

/*
 * Get task ID from rendezvous number 
 */
ID knl_get_tskid_rdvno( RNO rdvno )
{
	return (ID)((UINT)rdvno & ((1U << RDVNO_SHIFT) - 1));
}

#endif /* CFN_MAX_PORID > 0 */

/*
 * Check validity of rendezvous number
 */
#define CHECK_RDVNO(rdvno) {					\
	if ( !CHK_TSKID(knl_get_tskid_rdvno(rdvno)) ) {		\
		return E_OBJ;					\
	}							\
}


/*
 * Definition of rendezvous wait specification
 */
IMPORT CONST WSPEC knl_wspec_cal_tfifo;
IMPORT CONST WSPEC knl_wspec_cal_tpri;
IMPORT CONST WSPEC knl_wspec_rdv;


#endif /* _RENDEZVOUS_H_ */
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
 *	semaphore.c
 *	Semaphore
 */

/** [BEGIN Common Definitions] */
#include "kernel.h"
#include "task.h"
#include "wait.h"
#include "check.h"
#include "semaphore.h"
/** [END Common Definitions] */

#if CFN_MAX_SEMID > 0

#ifdef USE_FUNC_SEMCB_TABLE
Noinit(EXPORT SEMCB knl_semcb_table[NUM_SEMID]);	/* Semaphore control block */
Noinit(EXPORT QUEUE knl_free_semcb);	/* FreeQue */
#endif /* USE_FUNC_SEMCB_TABLE */


#ifdef USE_FUNC_SEMAPHORE_INITIALIZE
/* 
 * Initialization of semaphore control block 
 */
EXPORT ER knl_semaphore_initialize( void )
{
	SEMCB	*semcb, *end;

	/* Get system information */
	if ( NUM_SEMID < 1 ) {
		return E_SYS;
	}

	/* Register all control blocks onto FreeQue */
	QueInit(&knl_free_semcb);
	end = knl_semcb_table + NUM_SEMID;
	for ( semcb = knl_semcb_table; semcb < end; semcb++ ) {
		semcb->semid = 0;
		QueInsert(&semcb->wait_queue, &knl_free_semcb);
	}

	return E_OK;
}
#endif /* USE_FUNC_SEMAPHORE_INITIALIZE */


#ifdef USE_FUNC_TK_CRE_SEM
/*
 * Create semaphore
 */
SYSCALL ID tk_cre_sem_impl( CONST T_CSEM *pk_csem )
{
#if CHK_RSATR
	const ATR VALID_SEMATR = {
		 TA_TPRI
		|TA_CNT
#if USE_OBJECT_NAME
		|TA_DSNAME
#endif
	};
#endif
	SEMCB	*semcb;
	ID	semid;
	ER	ercd;

	CHECK_RSATR(pk_csem->sematr, VALID_SEMATR);
	CHECK_PAR(pk_csem->isemcnt >= 0);
	CHECK_PAR(pk_csem->maxsem > 0);
	CHECK_PAR(pk_csem->maxsem >= pk_csem->isemcnt);

	BEGIN_CRITICAL_SECTION;
	/* Get control block from FreeQue */
	semcb = (SEMCB*)QueRemoveNext(&knl_free_semcb);
	if ( semcb == NULL ) {
		ercd = E_LIMIT;
	} else {
		semid = ID_SEM(semcb - knl_semcb_table);

		/* Initialize control block */
		QueInit(&semcb->wait_queue);
		semcb->semid = semid;
		semcb->exinf = pk_csem->exinf;
		semcb->sematr = pk_csem->sematr;
		semcb->semcnt = pk_csem->isemcnt;
		semcb->maxsem = pk_csem->maxsem;
#if USE_OBJECT_NAME
		if ( (pk_csem->sematr & TA_DSNAME) != 0 ) {
			strncpy((char*)semcb->name, (char*)pk_csem->dsname,
				OBJECT_NAME_LENGTH);
		}
#endif
		ercd = semid;
	}
	END_CRITICAL_SECTION;

	return ercd;
}
#endif /* USE_FUNC_TK_CRE_SEM */

#ifdef USE_FUNC_TK_DEL_SEM
/*
 * Delete semaphore
 */
SYSCALL ER tk_del_sem_impl( ID semid )
{
	SEMCB	*semcb;
	ER	ercd = E_OK;

	CHECK_SEMID(semid);

	semcb = get_semcb(semid);

	BEGIN_CRITICAL_SECTION;
	if ( semcb->semid == 0 ) {
		ercd = E_NOEXS;
	} else {
		/* Release wait state of task (E_DLT) */
		knl_wait_delete(&semcb->wait_queue);

		/* Return to FreeQue */
		QueInsert(&semcb->wait_queue, &knl_free_semcb);
		semcb->semid = 0;
	}
	END_CRITICAL_SECTION;

	return ercd;
}
#endif /* USE_FUNC_TK_DEL_SEM */

#ifdef USE_FUNC_TK_SIG_SEM
/*
 * Signal semaphore
 */
SYSCALL ER tk_sig_sem_impl( ID semid, INT cnt )
{
	SEMCB	*semcb;
	TCB	*tcb;
	QUEUE	*queue;
	ER	ercd = E_OK;
    
	CHECK_SEMID(semid);
	CHECK_PAR(cnt > 0);

	semcb = get_semcb(semid);

	BEGIN_CRITICAL_SECTION;
	if ( semcb->semid == 0 ) {
		ercd = E_NOEXS;
		goto error_exit;
	}
	if ( cnt > (semcb->maxsem - semcb->semcnt) ) {
		ercd = E_QOVR;
		goto error_exit;
	}

	/* Return semaphore counts */
	semcb->semcnt += cnt;

	/* Search task that frees wait */
	queue = semcb->wait_queue.next;
	while ( queue != &semcb->wait_queue ) {
		tcb = (TCB*)queue;
		queue = queue->next;

		/* Meet condition for Releasing wait? */
		if ( semcb->semcnt < tcb->winfo.sem.cnt ) {
			if ( (semcb->sematr & TA_CNT) == 0 ) {
				break;
			}
			continue;
		}

		/* Release wait */
		knl_wait_release_ok(tcb);

		semcb->semcnt -= tcb->winfo.sem.cnt;
		if ( semcb->semcnt <= 0 ) {
			break;
		}
	}

    error_exit:
	END_CRITICAL_SECTION;

	return ercd;
}
#endif /* USE_FUNC_TK_SIG_SEM */

#ifdef USE_FUNC_TK_WAI_SEM
/*
 * Processing if the priority of wait task changes
 */
LOCAL void sem_chg_pri( TCB *tcb, INT oldpri )
{
	SEMCB	*semcb;
	QUEUE	*queue;
	TCB	*top;

	semcb = get_semcb(tcb->wid);
	if ( oldpri >= 0 ) {
		/* Reorder wait line */
		knl_gcb_change_priority((GCB*)semcb, tcb);
	}

	if ( (semcb->sematr & TA_CNT) != 0 ) {
		return;
	}

	/* From the head task in a wait queue, allocate semaphore counts
	   and release wait state as much as possible */
	queue = semcb->wait_queue.next;
	while ( queue != &semcb->wait_queue ) {
		top = (TCB*)queue;
		queue = queue->next;

		/* Meet condition for releasing wait? */
		if ( semcb->semcnt < top->winfo.sem.cnt ) {
			break;
		}

		/* Release wait */
		knl_wait_release_ok(top);

		semcb->semcnt -= top->winfo.sem.cnt;
	}
}

/*
 * Processing if the wait task is freed
 */
LOCAL void sem_rel_wai( TCB *tcb )
{
	sem_chg_pri(tcb, -1);
}

/*
 * Definition of semaphore wait specification
 */
LOCAL CONST WSPEC knl_wspec_sem_tfifo = { TTW_SEM, NULL,        sem_rel_wai };
LOCAL CONST WSPEC knl_wspec_sem_tpri  = { TTW_SEM, sem_chg_pri, sem_rel_wai };

/*
 * Wait on semaphore
 */
SYSCALL ER tk_wai_sem_impl( ID semid, INT cnt, TMO tmout )
{
	SEMCB	*semcb;
	ER	ercd = E_OK;

	CHECK_SEMID(semid);
	CHECK_PAR(cnt > 0);
	CHECK_TMOUT(tmout);
	CHECK_DISPATCH();

	semcb = get_semcb(semid);

	BEGIN_CRITICAL_SECTION;
	if ( semcb->semid == 0 ) {
		ercd = E_NOEXS;
		goto error_exit;
	}
#if CHK_PAR
	if ( cnt > semcb->maxsem ) {
		ercd = E_PAR;
		goto error_exit;
	}
#endif

	if ( ((semcb->sematr & TA_CNT) != 0
	      || knl_gcb_top_of_wait_queue((GCB*)semcb, knl_ctxtsk) == knl_ctxtsk)
	  && semcb->semcnt >= cnt ) {
		/* Get semaphore count */
		semcb->semcnt -= cnt;

	} else {
		/* Ready for wait */
		knl_ctxtsk->wspec = ( (semcb->sematr & TA_TPRI) != 0 )?
					&knl_wspec_sem_tpri: &knl_wspec_sem_tfifo;
		knl_ctxtsk->wercd = &ercd;
		knl_ctxtsk->winfo.sem.cnt = cnt;
		knl_gcb_make_wait((GCB*)semcb, tmout);
	}

    error_exit:
	END_CRITICAL_SECTION;

	return ercd;
}
#endif /* USE_FUNC_TK_WAI_SEM */

#ifdef USE_FUNC_TK_REF_SEM
/*
 * Refer semaphore state
 */
SYSCALL ER tk_ref_sem_impl( ID semid, T_RSEM *pk_rsem )
{
	SEMCB	*semcb;
	ER	ercd = E_OK;

	CHECK_SEMID(semid);

	semcb = get_semcb(semid);

	BEGIN_CRITICAL_SECTION;
	if ( semcb->semid == 0 ) {
		ercd = E_NOEXS;
	} else {
		pk_rsem->exinf  = semcb->exinf;
		pk_rsem->wtsk   = knl_wait_tskid(&semcb->wait_queue);
		pk_rsem->semcnt = semcb->semcnt;
	}
	END_CRITICAL_SECTION;

	return ercd;
}
#endif /* USE_FUNC_TK_REF_SEM */

/* ------------------------------------------------------------------------ */
/*
 *	Debugger support function
 */
#if USE_DBGSPT

#ifdef USE_FUNC_SEMAPHORE_GETNAME
#if USE_OBJECT_NAME
/*
 * Get object name from control block
 */
EXPORT ER knl_semaphore_getname(ID id, UB **name)
{
	SEMCB	*semcb;
	ER	ercd = E_OK;

	CHECK_SEMID(id);

	BEGIN_DISABLE_INTERRUPT;
	semcb = get_semcb(id);
	if ( semcb->semid == 0 ) {
		ercd = E_NOEXS;
		goto error_exit;
	}
	if ( (semcb->sematr & TA_DSNAME) == 0 ) {
		ercd = E_OBJ;
		goto error_exit;
	}
	*name = semcb->name;

    error_exit:
	END_DISABLE_INTERRUPT;

	return ercd;
}
#endif /* USE_OBJECT_NAME */
#endif /* USE_FUNC_SEMAPHORE_GETNAME */

#ifdef USE_FUNC_TD_LST_SEM
/*
 * Refer object usage state
 */
SYSCALL INT td_lst_sem_impl( ID list[], INT nent )
{
	SEMCB	*semcb, *end;
	INT	n = 0;

	BEGIN_DISABLE_INTERRUPT;
	end = knl_semcb_table + NUM_SEMID;
	for ( semcb = knl_semcb_table; semcb < end; semcb++ ) {
		if ( semcb->semid == 0 ) {
			continue;
		}

		if ( n++ < nent ) {
			*list++ = semcb->semid;
		}
	}
	END_DISABLE_INTERRUPT;

	return n;
}
#endif /* USE_FUNC_TD_LST_SEM */

#ifdef USE_FUNC_TD_REF_SEM
/*
 * Refer object state
 */
SYSCALL ER td_ref_sem_impl( ID semid, TD_RSEM *pk_rsem )
{
	SEMCB	*semcb;
	ER	ercd = E_OK;

	CHECK_SEMID(semid);

	semcb = get_semcb(semid);

	BEGIN_DISABLE_INTERRUPT;
	if ( semcb->semid == 0 ) {
		ercd = E_NOEXS;
	} else {
		pk_rsem->exinf  = semcb->exinf;
		pk_rsem->wtsk   = knl_wait_tskid(&semcb->wait_queue);
		pk_rsem->semcnt = semcb->semcnt;
	}
	END_DISABLE_INTERRUPT;

	return ercd;
}
#endif /* USE_FUNC_TD_REF_SEM */

#ifdef USE_FUNC_TD_SEM_QUE
/*
 * Refer wait queue
 */
SYSCALL INT td_sem_que_impl( ID semid, ID list[], INT nent )
{
	SEMCB	*semcb;
	QUEUE	*q;
	ER	ercd;

	CHECK_SEMID(semid);

	semcb = get_semcb(semid);

	BEGIN_DISABLE_INTERRUPT;
	if ( semcb->semid == 0 ) {
		ercd = E_NOEXS;
	} else {
		INT	n = 0;
		for ( q = semcb->wait_queue.next; q != &semcb->wait_queue; q = q->next ) {
			if ( n++ < nent ) {
				*list++ = ((TCB*)q)->tskid;
			}
		}
		ercd = n;
	}
	END_DISABLE_INTERRUPT;

	return ercd;
}
#endif /* USE_FUNC_TD_SEM_QUE */

#endif /* USE_DBGSPT */
#endif /* CFN_MAX_SEMID */
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
 *	semaphore.h
 *	Semaphore
 */

#ifndef _SEMAPHORE_H_
#define _SEMAPHORE_H_

/*
 * Semaphore control block
 */
typedef struct semaphore_control_block {
	QUEUE	wait_queue;	/* Semaphore wait queue */
	ID	semid;		/* Semaphore ID */
	void	*exinf;		/* Extended information */
	ATR	sematr;		/* Semaphore attribute */
	INT	semcnt;		/* Semaphore current count value */
	INT	maxsem;		/* Semaphore maximum count value */
#if USE_OBJECT_NAME
	UB	name[OBJECT_NAME_LENGTH];	/* name */
#endif
} SEMCB;

IMPORT SEMCB knl_semcb_table[];	/* Semaphore control block */
IMPORT QUEUE knl_free_semcb;	/* FreeQue */

#define get_semcb(id)	( &knl_semcb_table[INDEX_SEM(id)] )


#endif /* _SEMAPHORE_H_ */
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
/*--------------------------------------------------------------------*/
/*  Include definition                                                */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Struct definition                                                 */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Constant definition                                               */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Variable definition                                               */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Macro definition                                                  */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Prototype declaration                                             */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Function definition                                               */
/*--------------------------------------------------------------------*/

#include <typedef.h>

// #define	TERM_PORT		0

// #define CPU_CLOCK		(20000000UL)
// #define BAUD_RATE		(115200UL)

// #define GPIO_BASE		(0x40033000UL)
// #define GPIO_PFR1		((_UW*)(GPIO_BASE + 0x0004UL))
// #define GPIO_PFR2		((_UW*)(GPIO_BASE + 0x0008UL))
// #define GPIO_PFR3		((_UW*)(GPIO_BASE + 0x000cUL))
// #define GPIO_PFR4		((_UW*)(GPIO_BASE + 0x0010UL))
// #define GPIO_EPFR07		((_UW*)(GPIO_BASE + 0x061cUL))
// #define GPIO_EPFR08		((_UW*)(GPIO_BASE + 0x0620UL))
// #define	GPIO_ADE		((_UW*)(GPIO_BASE + 0x0500UL))

// #if TERM_PORT == 0		/* Sxx0_0 */
// #define UART_BASE		(0x40038000UL)
// #elif TERM_PORT == 1		/* Sxx1_1 */
// #define UART_BASE		(0x40038100UL)
// #elif TERM_PORT == 2		/* Sxx2_2 */
// #define UART_BASE		(0x40038200UL)
// #elif TERM_PORT == 3		/* Sxx3_2 */
// #define UART_BASE		(0x40038300UL)
// #elif TERM_PORT == 4		/* Sxx4_1 */
// #define UART_BASE		(0x40038400UL)
// #elif TERM_PORT == 5		/* Sxx5_2 */
// #define UART_BASE		(0x40038500UL)
// #elif TERM_PORT == 6		/* Sxx6_1 */
// #define UART_BASE		(0x40038600UL)
// #elif TERM_PORT == 7		/* Sxx7_1 */
// #define UART_BASE		(0x40038700UL)
// #elif TERM_PORT == 10		/* Sxx0_1 */
// #define UART_BASE		(0x40038000UL)
// #else				/* default Sxx0_0 */
// #define UART_BASE		(0x40038000UL)
// #endif	/* TERM_PORT */
// #define UART_SMR		((_UB*)(UART_BASE + 0x00UL))
// #define UART_SCR		((_UB*)(UART_BASE + 0x01UL))
// #define UART_ESCR		((_UB*)(UART_BASE + 0x04UL))
// #define UART_SSR		((_UB*)(UART_BASE + 0x05UL))
// #define UART_DR			((_UB*)(UART_BASE + 0x08UL))
// #define UART_BGR		((_UH*)(UART_BASE + 0x0cUL))

LOCAL void sendLine( const UB *buf )
{
	// while( *buf++ != '\0' ){
		// while( (*UART_SSR & 0x02U) == 0 );
		// *UART_DR = *buf;
	// }
}

LOCAL void sendChar( const UB *buf )
{
	// while( (*UART_SSR & 0x02U) == 0 );
	// *UART_DR = *buf;
}

LOCAL UB getChar( UB *buf )
{
	// while( (*UART_SSR & 0x04U) == 0 );
	// *buf = *UART_DR;

	return *buf;
}

EXPORT void sio_send_frame( const UB* buf, INT size )
{
	if(size == 1) {			/* for tm_putchar */
		sendChar(buf);
	}
	else if (size >= 2) {		/* for tm_putstring */
		sendLine(buf);
	}
	else {
	}
}


EXPORT void sio_recv_frame( UB* buf, INT size )
{
	if(size == 1) {			/* for tm_getchar */
		getChar( buf );
	}
	else {
	}
}


EXPORT void sio_init(void)
{
//	UW	r;

// #if TERM_PORT == 0
	// r = *GPIO_ADE;
	// r &= 0xffffff7f;
	// *GPIO_ADE = r;
	// r = *GPIO_PFR2;
	// r |= 0x00000006U;
	// *GPIO_PFR2 = r; 	/* use P21, P22, P23 for Serial I/O */
	// r = *GPIO_EPFR07;
	// r |= 0x00000040U;
	// *GPIO_EPFR07 = r; 	/* use UART0_0 for IN/OUT/CLOCK */

// #elif TERM_PORT == 1
	// r = *GPIO_ADE;
	// r &= 0xfffffff1;
	// *GPIO_ADE = r;
	// *GPIO_PFR1 = 0x0000000eU; /* use P11, P12, P13 for Serial I/O */
	// *GPIO_EPFR07 = 0x0000a800U; /* use UART1_1 for IN/OUT/CLOCK */

// #elif TERM_PORT == 2
	// r = *GPIO_ADE;
	// r &= 0xfffffc7f;
	// *GPIO_ADE = r;
	// *GPIO_PFR1 = 0x00000380U; /* use P17, P18, P19 for Serial I/O */
	// *GPIO_EPFR07 = 0x003f0000U; /* use UART2_2 for IN/OUT/CLOCK */

// #elif TERM_PORT == 3
	// /* There are no bits for the UART3 in the GPIO_ADE register */
	// *GPIO_PFR4 = 0x00000700U; /* use P48, P49, P4A for Serial I/O */
	// *GPIO_EPFR07 = 0x0fc00000U; /* use UART3_2 for IN/OUT/CLOCK */

// #elif TERM_PORT == 4
	// r = *GPIO_ADE;
	// r &= 0xffffe3ff;
	// *GPIO_ADE = r;
	// *GPIO_PFR1 = 0x00001c00U; /* use P1A, P1B, P1C for Serial I/O */
	// *GPIO_EPFR08 = 0x000002a0U; /* use UART4_1 for IN/OUT/CLOCK */

// #elif TERM_PORT == 5
	/* There are no bits for the UART5 in the GPIO_ADE register */
	// *GPIO_PFR3 = 0x000001c0U; /* use P36, P37, P38 for Serial I/O */
	// *GPIO_EPFR08 = 0x0000fc00U; /* use UART5_2 for IN/OUT/CLOCK */

// #elif TERM_PORT == 6
	/* There are no bits for the UART6 in the GPIO_ADE register */
	// *GPIO_PFR3 = 0x0000000eU; /* use P33, P32, P31 for Serial I/O */
	// *GPIO_EPFR08 = 0x002a0000U; /* use UART6_1 for IN/OUT/CLOCK */

// #elif TERM_PORT == 7
	/* There are no bits for the UART7 in the GPIO_ADE register */
	// *GPIO_PFR4 = 0x00007000U; /* use P4C, P4D, P4E for Serial I/O */
	// *GPIO_EPFR08 = 0x0a800000U; /* use UART7_1 for IN/OUT/CLOCK */

// #elif TERM_PORT == 10
	// r = *GPIO_ADE;
	// r &= 0xffffff8f;
	// *GPIO_ADE = r;
	// *GPIO_PFR1 = 0x00000070U; /* use P14, P15, P16 for Serial I/O */
	// *GPIO_EPFR07 = 0x000002a0U; /* use UART0_1 for IN/OUT/CLOCK */

// #else
	// r = *GPIO_ADE;
	// r &= 0xfff8ffff;
	// *GPIO_ADE = r;
	// *GPIO_PFR2 = 0x0000000eU; /* use P21, P22, P23 for Serial I/O */
	// *GPIO_EPFR07 = 0x00000040U; /* use UART0_0 for IN/OUT/CLOCK */

// #endif	/* TERM_PORT */

	// *UART_SCR = 0x80U; /* Clear */
	// *UART_SMR = 0x01U; /* async normal mode, stop-bit = 1, LSB-first, output enable */

	// *UART_BGR = (((CPU_CLOCK)/ BAUD_RATE) - 1); /* baud rate */
	// *UART_SCR = 0x80U; /* Clear */
	// *UART_ESCR = 0x00U; /* no hardware flow control, non-parity, stop-bit = 1, data length = 8 bits */
	// *UART_SCR = 0x03U; /* RX, TX enable */
}
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
 *	@(#)stdtype.h
 *
 *	C language: standard type
 */

#ifndef __STDTYPE_H__
#define __STDTYPE_H__

#define __size_t	unsigned long

#ifndef	__cplusplus
#define __wchar_t	int
#endif

#endif /* __STDTYPE_H__ */
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
 *      @(#)str_align.h (sys)
 *
 *	Bit alignment definitions for structure 
 */

#ifndef __SYS_STR_ALIGN_H__
#define __SYS_STR_ALIGN_H__

#include <str_align_common.h>

#endif /* __SYS_STR_ALIGN_H__ */
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
// #include <str_align_depend.h>
// #endif

// #ifdef APP_H8S2212
// #include <str_align_depend.h>
// #endif

// #ifdef _APP_MB9AF312K_
// #include <str_align_depend.h>
// #endif

#ifdef _APP_RL78G13_R5F100ADASP_
#include <str_align_depend.h>
#endif

#endif /* __SYS_STR_ALIGN_COMMON_H__ */
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
 *      @(#)str_align_depend.h (FM3)
 *
 *	Bit alignment definitions for structure
 */

#ifndef __SYS_STR_ALIGN_DEPEND_H__
#define __SYS_STR_ALIGN_DEPEND_H__

/* 32 bit alignment */
#if BIGENDIAN
#  define _pad_b(n)	int :n;
#  define _pad_l(n)
#else
#  define _pad_b(n)
#  define _pad_l(n)	int :n;
#endif

#define _align64

#endif /* __SYS_STR_ALIGN_DEPEND_H__ */
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
 *	subsystem.c
 *	Subsystem Manager
 */

/** [BEGIN Common Definitions] */
#include "kernel.h"
#include "task.h"
#include "check.h"
#include "bitop.h"
#include "subsystem.h"
/** [END Common Definitions] */

#if CFN_MAX_SSYID > 0


#ifdef USE_FUNC_SSYCB_TABLE
Noinit(EXPORT SSYCB knl_ssycb_table[NUM_SSYID]);	/* Subsystem control block */
#endif /* USE_FUNC_SSYCB_TABLE */


#ifdef USE_FUNC_SUBSYSTEM_INITIALIZE
/*
 * Initialization of subsystem control block
 */
EXPORT ER knl_subsystem_initialize( void )
{
	INT	i;

	/* Get system information */
	if ( NUM_SSYID < 1 ) {
		return E_SYS;
	}
	if ( NUM_SSYPRI < 1 ) {
		return E_SYS;
	}

	for ( i = 0; i < NUM_SSYID; i++ ) {
		knl_ssycb_table[i].svchdr    = knl_no_support;
	}

	return E_OK;
}
#endif /* USE_FUNC_SUBSYSTEM_INITIALIZE */


#ifdef USE_FUNC_TK_DEF_SSY
/*
 * Definition of subsystem
 */
SYSCALL ER tk_def_ssy_impl P2( ID ssid, CONST T_DSSY *pk_dssy )
{
	SSYCB	*ssycb;
	ER	ercd = E_OK;

	CHECK_SSYID(ssid);
#if CHK_RSATR
	if ( pk_dssy != NULL ) {
		CHECK_RSATR(pk_dssy->ssyatr, TA_NULL|TA_GP);
	}
#endif

	ssycb = get_ssycb(ssid);

	BEGIN_CRITICAL_SECTION;
	if ( pk_dssy != NULL ) {
		/* Register */
		if ( ssycb->svchdr != knl_no_support ) {
			ercd = E_OBJ;  /* Registered */
			goto error_exit;
		}
		ssycb->svchdr    = (SVC)pk_dssy->svchdr;
#if TA_GP
		if ( (pk_dssy->ssyatr & TA_GP) != 0 ) {
			gp = pk_dssy->gp;
		}
		ssycb->gp = gp;
#endif

	} else {
		/* Delete */
		if ( ssycb->svchdr == knl_no_support ) {
			ercd = E_NOEXS;  /* Not registered */
			goto error_exit;
		}

		ssycb->svchdr    = knl_no_support;
	}

    error_exit:
	END_CRITICAL_SECTION;

	return ercd;
}
#endif /* USE_FUNC_TK_DEF_SSY */

#ifdef USE_FUNC_TK_REF_SSY
/*
 * Refer subsystem definition information
 */
SYSCALL ER tk_ref_ssy_impl( ID ssid, T_RSSY *pk_rssy )
{
	SSYCB	*ssycb;
	ER	ercd = E_OK;

	CHECK_SSYID(ssid);

	ssycb = get_ssycb(ssid);

	BEGIN_CRITICAL_SECTION;
	if ( ssycb->svchdr == knl_no_support ) {
		ercd = E_NOEXS;
	}
	END_CRITICAL_SECTION;

	return ercd;
}
#endif /* USE_FUNC_TK_REF_SSY */

#if USE_DBGSPT

#ifdef USE_FUNC_TD_LST_SSY
/*
 * Refer subsystem usage state
 */
SYSCALL INT td_lst_ssy_impl( ID list[], INT nent )
{
	SSYCB	*ssycb, *end;
	INT	n = 0;

	BEGIN_DISABLE_INTERRUPT;
	end = knl_ssycb_table + NUM_SSYID;
	for ( ssycb = knl_ssycb_table; ssycb < end; ssycb++ ) {
		if ( ssycb->svchdr == knl_no_support ) {
			continue;
		}

		if ( n++ < nent ) {
			*list++ = ID_SSY(ssycb - knl_ssycb_table);
		}
	}
	END_DISABLE_INTERRUPT;

	return n;
}
#endif /* USE_FUNC_TD_LST_SSY */

#ifdef USE_FUNC_TD_REF_SSY
/*
 * Refer subsystem definition information
 */
SYSCALL ER td_ref_ssy_impl( ID ssid, TD_RSSY *pk_rssy )
{
	SSYCB	*ssycb;
	ER	ercd = E_OK;

	CHECK_SSYID(ssid);

	ssycb = get_ssycb(ssid);

	BEGIN_DISABLE_INTERRUPT;
	if ( ssycb->svchdr == knl_no_support ) {
		ercd = E_NOEXS;
	}
	END_DISABLE_INTERRUPT;

	return ercd;
}
#endif /* USE_FUNC_TD_REF_SSY */

#endif /* USE_DBGSPT */

#ifdef USE_FUNC_SVC_IENTRY
/*
 * Branch routine to extended SVC handler
 */
EXPORT ER knl_svc_ientry P2GP( void *pk_para, FN fncd )
{
	ID	ssid;
	SSYCB	*ssycb;
	ER	ercd;

	/* Lower 8 bits are subsystem ID */
	ssid = fncd & 0xff;
	if ( ssid < MIN_SSYID || ssid > MAX_SSYID ) {
		return E_RSFN;
	}

	ssycb = get_ssycb(ssid);

	if ( in_indp() ) {
		/* Execute at task-independent part */
		ercd = CallUserHandlerP2_GP(pk_para, fncd,
						ssycb->svchdr, ssycb);
	} else {
		DISABLE_INTERRUPT;
		knl_ctxtsk->sysmode++;
		ENABLE_INTERRUPT;

		/* Call extended SVC handler */
		ercd = CallUserHandlerP2_GP(pk_para, fncd,
						ssycb->svchdr, ssycb);

		DISABLE_INTERRUPT;
		knl_ctxtsk->sysmode--;
		ENABLE_INTERRUPT;
	}

	return ercd;
}
#endif /* USE_FUNC_SVC_IENTRY */

#endif /* CFN_MAX_SSYID */
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
 *	subsystem.h
 *	Subsystem Manager
 */

#ifndef _SUBSYSTEM_H_
#define _SUBSYSTEM_H_

typedef INT  (*SVC)( void *pk_para, FN fncd );	/* Extended SVC handler */

/*
 * Definition of subsystem control block
 */
typedef struct subsystem_control_block	SSYCB;
struct subsystem_control_block {
	SVC	svchdr;		/* Extended SVC handler */
#if TA_GP
	void	*gp;		/* Global pointer */
#endif
};

IMPORT SSYCB knl_ssycb_table[];	/* Subsystem control block */

#define get_ssycb(id)	( &knl_ssycb_table[INDEX_SSY(id)] )

/*
 * Undefined extended SVC function 
 */
IMPORT INT knl_no_support( void *pk_para, FN fncd );

#endif /* _SUBSYSTEM_H_ */
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
 *	@(#)syscall.h
 *
 *	micro T-Kernel (Common parts)
 */

#ifndef __TK_SYSCALL_H__
#define __TK_SYSCALL_H__

#include <config.h>

#include <basic.h>
#include <tk_typedef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Task creation */
#define TSK_SELF	0		/* Its own task specify */
#define TPRI_INI	0		/* Specify priority at task startup */
#define TPRI_RUN	0		/* Specify highest priority during
					   running */

#define TA_ASM		0x00000000UL	/* Program by assembler */
#define TA_HLNG		0x00000001UL	/* Program by high level programming
					   language */
#define TA_USERBUF	0x00000020UL	/* Specify user buffer */
#define TA_DSNAME	0x00000040UL	/* Use object name */

#define TA_RNG0		0x00000000UL	/* Execute by protection level 0 */
#define TA_RNG1		0x00000100UL	/* Execute by protection level 1 */
#define TA_RNG2		0x00000200UL	/* Execute by protection level 2 */
#define TA_RNG3		0x00000300UL	/* Execute by protection level 3 */

#define TA_COP0		0x00001000U	/* Use coprocessor (ID=0) */
#define TA_COP1		0x00002000U	/* Use coprocessor (ID=1) */
#define TA_COP2		0x00004000U	/* Use coprocessor (ID=2) */
#define TA_COP3		0x00008000U	/* Use coprocessor (ID=3) */

/* Task state tskstat */
#define TTS_RUN		0x00000001U	/* RUN */
#define TTS_RDY		0x00000002U	/* READY */
#define TTS_WAI		0x00000004U	/* WAIT */
#define TTS_SUS		0x00000008U	/* SUSPEND */
#define TTS_WAS		0x0000000cU	/* WAIT-SUSPEND */
#define TTS_DMT		0x00000010U	/* DORMANT */
#define TTS_NODISWAI	0x00000080U	/* Wait disable rejection state */

/* Wait factor tskwait */
#define TTW_SLP		0x00000001UL	/* Wait caused by wakeup wait */
#define TTW_DLY		0x00000002UL	/* Wait caused by task delay */
#define TTW_SEM		0x00000004UL	/* Semaphore wait */
#define TTW_FLG		0x00000008UL	/* Event flag wait */
#define TTW_MBX		0x00000040UL	/* Mail box wait */
#define TTW_MTX		0x00000080UL	/* Mutex wait */
#define TTW_SMBF	0x00000100UL	/* Message buffer send wait */
#define TTW_RMBF	0x00000200UL	/* Message buffer receive wait */
#define TTW_CAL		0x00000400UL	/* Rendezvous call wait */
#define TTW_ACP		0x00000800UL	/* Rendezvous accept wait */
#define TTW_RDV		0x00001000UL	/* Rendezvous end wait */
#define TTW_MPF		0x00002000UL	/* Fixed size memory pool wait */
#define TTW_MPL		0x00004000UL	/* Variable size memory pool wait */

/* Semaphore generation */
#define TA_TFIFO	0x00000000UL	/* Manage wait task by FIFO */
#define TA_TPRI		0x00000001UL	/* Manage wait task by priority
					   order */
#define TA_FIRST	0x00000000UL	/* Give priority to task at head of
					   wait queue */
#define TA_CNT		0x00000002UL	/* Give priority to task whose
					   request counts is less */
#define TA_DSNAME	0x00000040UL	/* Use object name */

/* Mutex */
#define TA_TFIFO	0x00000000UL	/* Manage wait task by FIFO */
#define TA_TPRI		0x00000001UL	/* Manage wait task by priority
					   order */
#define TA_INHERIT	0x00000002UL	/* Priority inherited protocol */
#define TA_CEILING	0x00000003UL	/* Upper limit priority protocol */
#define TA_DSNAME	0x00000040UL	/* Use object name */

/* Event flag */
#define TA_TFIFO	0x00000000UL	/* Manage wait task by FIFO */
#define TA_TPRI		0x00000001UL	/* Manage wait task by priority
					   order */
#define TA_WSGL		0x00000000UL	/* Disable multiple tasks wait */
#define TA_WMUL		0x00000008UL	/* Enable multiple tasks wait */
#define TA_DSNAME	0x00000040UL	/* Use object name */

/* Event flag wait mode */
#define TWF_ANDW	0x00000000U	/* AND wait */
#define TWF_ORW		0x00000001U	/* OR wait */
#define TWF_CLR		0x00000010U	/* All clear specify */
#define TWF_BITCLR	0x00000020U	/* Only condition bit clear specify */

/* Mail box */
#define TA_TFIFO	0x00000000UL	/* Manage wait task by FIFO */
#define TA_TPRI		0x00000001UL	/* Manage wait task by priority
					   order */
#define TA_MFIFO	0x00000000UL	/* Manage messages by FIFO */
#define TA_MPRI		0x00000002UL	/* Manage messages by priority
					   order */
#define TA_DSNAME	0x00000040UL	/* Use object name */

/* Message buffer */
#define TA_TFIFO	0x00000000UL	/* Manage wait task by FIFO */
#define TA_TPRI		0x00000001UL	/* Manage wait task by priority
					   order */
#define TA_USERBUF	0x00000020UL	/* Specify user buffer */
#define TA_DSNAME	0x00000040UL	/* Use object name */

/* Rendezvous */
#define TA_TFIFO	0x00000000UL	/* Manage wait task by FIFO */
#define TA_TPRI		0x00000001UL	/* Manage wait task by priority
					   order */
#define TA_DSNAME	0x00000040UL	/* Use object name */

/* Handler */
#define TA_ASM		0x00000000UL	/* Program by assembler */
#define TA_HLNG		0x00000001UL	/* Program by high level programming
					   language */

/* Variable size memory pool */
#define TA_TFIFO	0x00000000UL	/* Manage wait task by FIFO */
#define TA_TPRI		0x00000001UL	/* Manage wait task by priority
					   order */
#define TA_USERBUF	0x00000020UL	/* Specify user buffer */
#define TA_DSNAME	0x00000040UL	/* Use object name */
#define TA_RNG0		0x00000000UL	/* Protection level 0 */
#define TA_RNG1		0x00000100UL	/* Protection level 1 */
#define TA_RNG2		0x00000200UL	/* Protection level 2 */
#define TA_RNG3		0x00000300UL	/* Protection level 3 */

/* Fixed size memory pool */
#define TA_TFIFO	0x00000000UL	/* Manage wait task by FIFO */
#define TA_TPRI		0x00000001UL	/* Manage wait task by priority
					   order */
#define TA_USERBUF	0x00000020UL	/* Specify user buffer */
#define TA_DSNAME	0x00000040UL	/* Use object name */
#define TA_RNG0		0x00000000UL	/* Protection level 0 */
#define TA_RNG1		0x00000100UL	/* Protection level 1 */
#define TA_RNG2		0x00000200UL	/* Protection level 2 */
#define TA_RNG3		0x00000300UL	/* Protection level 3 */

/* Cycle handler */
#define TA_ASM		0x00000000UL	/* Program by assembler */
#define TA_HLNG		0x00000001UL	/* Program by high level programming
					   language */
#define TA_STA		0x00000002UL	/* Cycle handler startup */
#define TA_PHS		0x00000004UL	/* Save cycle handler phase */
#define TA_DSNAME	0x00000040UL	/* Use object name */

#define TCYC_STP	0x00U		/* Cycle handler is not operating */
#define TCYC_STA	0x01U		/* Cycle handler is operating */

/* Alarm handler address */
#define TA_ASM		0x00000000UL	/* Program by assembler */
#define TA_HLNG		0x00000001UL	/* Program by high level programming
					   language */
#define TA_DSNAME	0x00000040UL	/* Use object name */

#define TALM_STP	0x00U		/* Alarm handler is not operating */
#define TALM_STA	0x01U		/* Alarm handler is operating */

/* System state */
#define TSS_TSK		0x00U	/* During execution of task part(context) */
#define TSS_DDSP	0x01U	/* During dispatch disable */
#define TSS_DINT	0x02U	/* During Interrupt disable */
#define TSS_INDP	0x04U	/* During execution of task independent part */
#define TSS_QTSK	0x08U	/* During execution of semi-task part */

#ifdef __cplusplus
}
#endif

/* System dependencies */
#include <syscall_common.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Task creation information 		tk_cre_tsk
 */
typedef struct t_ctsk {
	void	*exinf;		/* Extended information */
	ATR	tskatr;		/* Task attribute */
	FP	task;		/* Task startup address */
	PRI	itskpri;	/* Priority at task startup */
	SZ	stksz;		/* User stack size (byte) */
#if USE_OBJECT_NAME
	UB	dsname[OBJECT_NAME_LENGTH];	/* Object name */
#endif
	void	*bufptr;	/* User buffer */
#if TA_GP
	void	*gp;		/* Global pointer (gp) */
#endif
} T_CTSK;

/*
 * Task state information 		tk_ref_tsk
 */
typedef	struct t_rtsk {
	void	*exinf;		/* Extended information */
	PRI	tskpri;		/* Current priority */
	PRI	tskbpri;	/* Base priority */
	UINT	tskstat;	/* Task state */
	UW	tskwait;	/* Wait factor */
	ID	wid;		/* Wait object ID */
	INT	wupcnt;		/* Number of wakeup requests queuing */
	INT	suscnt;		/* Number of SUSPEND request nests */
} T_RTSK;

/*
 * Semaphore creation information		tk_cre_sem
 */
typedef	struct t_csem {
	void	*exinf;		/* Extended information */
	ATR	sematr;		/* Semaphore attribute */
	INT	isemcnt;	/* Semaphore initial count value */
	INT	maxsem;		/* Semaphore maximum count value */
#if USE_OBJECT_NAME
	UB	dsname[OBJECT_NAME_LENGTH];	/* Object name */
#endif
} T_CSEM;

/*
 * Semaphore state information		tk_ref_sem
 */
typedef	struct t_rsem {
	void	*exinf;		/* Extended information */
	ID	wtsk;		/* Wait task ID */
	INT	semcnt;		/* Current semaphore value */
} T_RSEM;

/*
 * Mutex creation information		tk_cre_mtx
 */
typedef struct t_cmtx {
	void	*exinf;		/* Extended information */
	ATR	mtxatr;		/* Mutex attribute */
	PRI	ceilpri;	/* Upper limit priority of mutex */
#if USE_OBJECT_NAME
	UB	dsname[OBJECT_NAME_LENGTH];	/* Object name */
#endif
} T_CMTX;

/*
 * Mutex state information		tk_ref_mtx
 */
typedef struct t_rmtx {
	void	*exinf;		/* Extended information */
	ID	htsk;		/* Locking task ID */
	ID	wtsk;		/* Lock wait task ID */
} T_RMTX;

/*
 * Event flag creation information	tk_cre_flg
 */
typedef	struct t_cflg {
	void	*exinf;		/* Extended information */
	ATR	flgatr;		/* Event flag attribute */
	UINT	iflgptn;	/* Event flag initial value */
#if USE_OBJECT_NAME
	UB	dsname[OBJECT_NAME_LENGTH];	/* Object name */
#endif
} T_CFLG;

/*
 * Event flag state information		tk_ref_flg
 */
typedef	struct t_rflg {
	void	*exinf;		/* Extended information */
	ID	wtsk;		/* Wait task ID */
	UINT	flgptn;		/* Current event flag pattern */
} T_RFLG;

/*
 * Mail box creation information	tk_cre_mbx
 */
typedef	struct t_cmbx {
	void	*exinf;		/* Extended information */
	ATR	mbxatr;		/* Mail box attribute */
#if USE_OBJECT_NAME
	UB	dsname[OBJECT_NAME_LENGTH];	/* Object name */
#endif
} T_CMBX;

/*
 * Mail box message header
 */
typedef struct t_msg {
	void	*msgque[1];	/* Area for message queue */
} T_MSG;

typedef struct t_msg_pri {
	T_MSG	msgque;		/* Area for message queue */
	PRI	msgpri;		/* Message priority */
} T_MSG_PRI;

/*
 * Mail box state information		tk_ref_mbx
 */
typedef	struct t_rmbx {
	void	*exinf;		/* Extended information */
	ID	wtsk;		/* Wait task ID */
	T_MSG	*pk_msg;	/* Next received message */
} T_RMBX;

/*
 * Message buffer creation information	tk_cre_mbf
 */
typedef	struct t_cmbf {
	void	*exinf;		/* Extended information */
	ATR	mbfatr;		/* Message buffer attribute */
	SZ	bufsz;		/* Message buffer size (byte) */
	INT	maxmsz;		/* Maximum length of message (byte) */
#if USE_OBJECT_NAME
	UB	dsname[OBJECT_NAME_LENGTH];	/* Object name */
#endif
	void	*bufptr;		/* User buffer */
} T_CMBF;

/*
 * Message buffer state information 	tk_ref_mbf
 */
typedef struct t_rmbf {
	void	*exinf;		/* Extended information */
	ID	wtsk;		/* Receive wait task ID */
	ID	stsk;		/* Send wait task ID */
	INT	msgsz;		/* Next received message size (byte) */
	SZ	frbufsz;	/* Free buffer size (byte) */
	INT	maxmsz;		/* Maximum length of message (byte) */
} T_RMBF;

/*
 * Rendezvous port creation information	tk_cre_por
 */
typedef	struct t_cpor {
	void	*exinf;		/* Extended information */
	ATR	poratr;		/* Port attribute */
	INT	maxcmsz;	/* Maximum length of call message (byte) */
	INT	maxrmsz;	/* Maximum length of replay message (byte) */
#if USE_OBJECT_NAME
	UB	dsname[OBJECT_NAME_LENGTH];	/* Object name */
#endif
} T_CPOR;

/*
 * Rendezvous port state information	tk_ref_por
 */
typedef struct t_rpor {
	void	*exinf;		/* Extended information */
	ID	wtsk;		/* Call wait task ID */
	ID	atsk;		/* Receive wait task ID */
	INT	maxcmsz;	/* Maximum length of call message (byte) */
	INT	maxrmsz;	/* Maximum length of replay message (byte) */
} T_RPOR;

/*
 * Interrupt handler definition information	tk_def_int
 */
typedef struct t_dint {
	ATR	intatr;		/* Interrupt handler attribute */
	FP	inthdr;		/* Interrupt handler address */
#if TA_GP
	void	*gp;		/* Global pointer (gp) */
#endif
} T_DINT;

/*
 * Variable size memory pool creation information	tk_cre_mpl
 */
typedef	struct t_cmpl {
	void	*exinf;		/* Extended information */
	ATR	mplatr;		/* Memory pool attribute */
	SZ	mplsz;		/* Size of whole memory pool (byte) */
#if USE_OBJECT_NAME
	UB	dsname[OBJECT_NAME_LENGTH];	/* Object name */
#endif
	void	*bufptr;		/* User buffer */
} T_CMPL;

/*
 * Variable size memory pool state information	tk_ref_mpl
 */
typedef struct t_rmpl {
	void	*exinf;		/* Extended information */
	ID	wtsk;		/* Wait task ID */
	SZ	frsz;		/* Total size of free area (byte) */
	SZ	maxsz;		/* Size of maximum continuous free area
				   (byte) */
} T_RMPL;

/*
 * Fixed size memory pool state information	tk_cre_mpf
 */
typedef	struct t_cmpf {
	void	*exinf;		/* Extended information */
	ATR	mpfatr;		/* Memory pool attribute */
	SZ	mpfcnt;		/* Number of blocks in whole memory pool */
	SZ	blfsz;		/* Fixed size memory block size (byte) */
#if USE_OBJECT_NAME
	UB	dsname[OBJECT_NAME_LENGTH];	/* Object name */
#endif
	void	*bufptr;		/* User buffer */
} T_CMPF;

/*
 * Fixed size memory pool state information	tk_ref_mpf
 */
typedef struct t_rmpf {
	void	*exinf;		/* Extended information */
	ID	wtsk;		/* Wait task ID */
	SZ	frbcnt;		/* Number of free area blocks */
} T_RMPF;

/*
 * Cycle handler creation information 	tk_cre_cyc
 */
typedef struct t_ccyc {
	void	*exinf;		/* Extended information */
	ATR	cycatr;		/* Cycle handler attribute */
	FP	cychdr;		/* Cycle handler address */
	RELTIM	cyctim;		/* Cycle interval */
	RELTIM	cycphs;		/* Cycle phase */
#if USE_OBJECT_NAME
	UB	dsname[OBJECT_NAME_LENGTH];	/* Object name */
#endif
#if TA_GP
	void	*gp;		/* Global pointer (gp) */
#endif
} T_CCYC;

/*
 * Cycle handler state information	tk_ref_cyc
 */
typedef struct t_rcyc {
	void	*exinf;		/* Extended information */
	RELTIM	lfttim;		/* Remaining time until next handler startup */
	UINT	cycstat;	/* Cycle handler status */
} T_RCYC;

/*
 * Alarm handler creation information		tk_cre_alm
 */
typedef struct t_calm {
	void	*exinf;		/* Extended information */
	ATR	almatr;		/* Alarm handler attribute */
	FP	almhdr;		/* Alarm handler address */
#if USE_OBJECT_NAME
	UB	dsname[OBJECT_NAME_LENGTH];	/* Object name */
#endif
#if TA_GP
	void	*gp;		/* Global pointer (gp) */
#endif
} T_CALM;

/*
 * Alarm handler state information	tk_ref_alm
 */
typedef struct t_ralm {
	void	*exinf;		/* Extended information */
	RELTIM	lfttim;		/* Remaining time until handler startup */
	UINT	almstat;	/* Alarm handler state */
} T_RALM;

/*
 * Version information		tk_ref_ver
 */
typedef struct t_rver {
	UH	maker;		/* OS manufacturer */
	UH	prid;		/* OS identification number */
	UH	spver;		/* Specification version */
	UH	prver;		/* OS product version */
	UH	prno[4];	/* Product number, Product management
				   information */
} T_RVER;

/*
 * System state information		tk_ref_sys
 */
typedef struct t_rsys {
	UINT	sysstat;	/* System state */
	ID	runtskid;	/* ID of task in execution state */
	ID	schedtskid;	/* ID of the task that should be the
				   execution state */
} T_RSYS;

/*
 * Subsystem definition information 		tk_def_ssy
 */
typedef struct t_dssy {
	ATR	ssyatr;		/* Subsystem attribute */
	PRI	ssypri;		/* Subsystem priority */
	FP	svchdr;		/* Extended SVC handler address */
	FP	breakfn;	/* Break function address */
	FP	startupfn;	/* Startup function address */
	FP	cleanupfn;	/* Cleanup function address */
	FP	eventfn;	/* Event function address */
	SZ	resblksz;	/* Resource management block size (byte) */
#if TA_GP
	void	*gp;		/* Global pointer (gp) */
#endif
} T_DSSY;

/*
 * Subsystem state information		tk_ref_ssy
 */
typedef struct t_rssy {
	PRI	ssypri;		/* Subsystem priority */
	SZ	resblksz;	/* Resource management block size (byte) */
} T_RSSY;

/* ------------------------------------------------------------------------ */

/*
 * Device manager
 */

#define L_DEVNM		8	/* Device name length */

/*
 * Device attribute (ATR)
 *
 *	IIII IIII IIII IIII PRxx xxxx KKKK KKKK
 *
 *	The first 16-bit is the device-dependent attribute and
 *	defined by each device.
 *	The last 16-bit is the standard attribute and defined
 *	like as followings.
 */
#define TD_PROTECT	0x8000U		/* P: Write protected */
#define TD_REMOVABLE	0x4000U		/* R: Media remove enabled */

#define TD_DEVKIND	0x00ffU		/* K: Device/media type */
#define TD_DEVTYPE	0x00f0U		/*    Device type */

/* Device type */
#define TDK_UNDEF	0x0000U		/* Undefined/Unknown */
#define TDK_DISK	0x0010U		/* Disk device */

/* Disk type */
#define TDK_DISK_UNDEF	0x0010U		/* Other disks */
#define TDK_DISK_RAM	0x0011U		/* RAM disk (Use main memory) */
#define TDK_DISK_ROM	0x0012U		/* ROM disk (Use main memory) */
#define TDK_DISK_FLA	0x0013U		/* Flash ROM, other silicon disks */
#define TDK_DISK_FD	0x0014U		/* Floppy disk */
#define TDK_DISK_HD	0x0015U		/* Hard disk */
#define TDK_DISK_CDROM	0x0016U		/* CD-ROM */

/*
 * Device open mode
 */
#define TD_READ		0x0001U		/* Read only */
#define TD_WRITE	0x0002U		/* Write only */
#define TD_UPDATE	0x0003U		/* Read and write */
#define TD_EXCL		0x0100U		/* Exclusive */
#define TD_WEXCL	0x0200U		/* Exclusive write */
#define TD_REXCL	0x0400U		/* Exclusive read */

/*
 * Device close option
 */
#define TD_EJECT	0x0001U		/* Media eject */

/*
 * Suspend mode
 */
#define TD_SUSPEND	0x0001U		/* Suspend */
#define TD_DISSUS	0x0002U		/* Disable suspend */
#define TD_ENASUS	0x0003U		/* Enable suspend */
#define TD_CHECK	0x0004U		/* Get suspend disable request count */
#define TD_FORCE	0x8000U		/* Specify forced suspend */

/*
 * Device information
 */
typedef struct t_rdev {
	ATR	devatr;		/* Device attribute */
	W	blksz;		/* Specific data block size (-1: Unknown) */
	INT	nsub;		/* Number of subunits */
	INT	subno;		/* 0: Physical device,
				   1 - nsub: Subunit number +1 */
} T_RDEV;

/*
 * Registration device information
 */
typedef struct t_ldev {
	ATR	devatr;		/* Device attribute */
	W	blksz;		/* Specific data block size (-1: Unknown) */
	INT	nsub;		/* Number of subunits */
	UB	devnm[L_DEVNM];	/* Physical device name */
} T_LDEV;

/*
 * Common attribute data number
 *	RW: Readable (tk_rea_dev)/writable (tk_wri_dev)
 *	R-: Readable (tk_rea_dev) only
 */
#define TDN_EVENT	(-1)	/* RW:Message buffer ID
				      for event notification */
#define TDN_DISKINFO	(-2)	/* R-:Disk information */
#define TDN_DISPSPEC	(-3)	/* R-:Display device specification */
#define TDN_PCMCIAINFO	(-4)	/* R-:PC card information */

/*
 * Device event type
 */
typedef	enum tdevttyp {
	TDE_unknown	= 0,		/* Undefined */
	TDE_MOUNT	= 0x01,		/* Media insert */
	TDE_EJECT	= 0x02,		/* Media eject */
	TDE_ILLMOUNT	= 0x03,		/* Media incorrect insert */
	TDE_ILLEJECT	= 0x04,		/* Media incorrect eject */
	TDE_REMOUNT	= 0x05,		/* Media re-insert */
	TDE_CARDBATLOW	= 0x06,		/* Card battery low */
	TDE_CARDBATFAIL	= 0x07,		/* Card battery abnormal */
	TDE_REQEJECT	= 0x08,		/* Media eject request */
	TDE_PDBUT	= 0x11,		/* PD button state change */
	TDE_PDMOVE	= 0x12,		/* PD position move */
	TDE_PDSTATE	= 0x13,		/* PD state change */
	TDE_PDEXT	= 0x14,		/* PD extended event */
	TDE_KEYDOWN	= 0x21,		/* Key down */
	TDE_KEYUP	= 0x22,		/* Key up */
	TDE_KEYMETA	= 0x23,		/* Meta key state change */
	TDE_POWEROFF	= 0x31,		/* Power switch off */
	TDE_POWERLOW	= 0x32,		/* Power low */
	TDE_POWERFAIL	= 0x33,		/* Power abnormal */
	TDE_POWERSUS	= 0x34,		/* Automatic suspend */
	TDE_POWERUPTM	= 0x35,		/* Clock update */
	TDE_CKPWON	= 0x41		/* Automatic power on notification */
} TDEvtTyp;

/*
 * Device event message format
 */
typedef struct t_devevt {
	TDEvtTyp	evttyp;		/* Event type */
	/* Information by each event type is added below */
} T_DEVEVT;

/*
 * Device event message format with device ID
 */
typedef struct t_devevt_id {
	TDEvtTyp	evttyp;		/* Event type */
	ID		devid;		/* Device ID */
	/* Information by each event type is added below */
} T_DEVEVT_ID;

/* ------------------------------------------------------------------------ */

/*
 * Device registration information
 */
typedef struct t_ddev {
	void	*exinf;		/* Extended information */
	ATR	drvatr;		/* Driver attribute */
	ATR	devatr;		/* Device attribute */
	INT	nsub;		/* Number of subunits */
	W	blksz;		/* Specific data block size (-1: Unknown) */
	FP	openfn;		/* Open function */
	FP	closefn;	/* Close function */
	FP	execfn;		/* Execute function */
	FP	waitfn;		/* Completion wait function */
	FP	abortfn;	/* Abort function */
	FP	eventfn;	/* Event function */
#if TA_GP
	void	*gp;		/* Global pointer (gp) */
#endif
} T_DDEV;

/*
 * Open function:
 *	ER  openfn( ID devid, UINT omode, void *exinf )
 * Close function:
 *	ER  closefn( ID devid, UINT option, void *exinf )
 * Execute function:
 *	ER  execfn( T_DEVREQ *devreq, TMO tmout, void *exinf )
 * Completion wait function:
 *	INT waitfn( T_DEVREQ *devreq, INT nreq, TMO tmout, void *exinf )
 * Abort function:
 *	ER  abortfn( ID tskid, T_DEVREQ *devreq, INT nreq, void *exinf) 
 * Event function:
 *	INT eventfn( INT evttyp, void *evtinf, void *exinf )
 */

/*
 * Driver attribute
 */
#define TDA_OPENREQ	0x0001U	/* Every time open/close */

/*
 * Device initial setting information
 */
typedef struct t_idev {
	ID	evtmbfid;	/* Message buffer ID for event notification */
} T_IDEV;

/*
 * Device request packet
 *	 I: Input parameter
 *	 O: Output parameter
 */
typedef struct t_devreq {
	struct t_devreq	*next;	/* I:Link to request packet (NULL:End) */
	void	*exinf;		/* X:Extended information */
	ID	devid;		/* I:Target device ID */
	INT	cmd:4;		/* I:Request command */
	BOOL	abort:1;	/* I:When executing abort request, TRUE */
	W	start;		/* I:Start data number */
	W	size;		/* I:Request size */
	void	*buf;		/* I:Input/output buffer address */
	W	asize;		/* O:Result size */
	ER	error;		/* O:Result error */
} T_DEVREQ;

/*
 * Request command
 */
#define TDC_READ	1	/* Read request */
#define TDC_WRITE	2	/* Write request */

/*
 * Driver request event
 */
#define TDV_SUSPEND	(-1)	/* Suspend */
#define TDV_RESUME	(-2)	/* Resume */
#define TDV_CARDEVT	1	/* PC card event (Refer card manager) */
#define TDV_USBEVT	2	/* USB event     (Refer USB manager) */

/* ------------------------------------------------------------------------ */
/*
 * Definition for interface library automatic generation (mktksvc)
 */
/*** DEFINE_TKSVC ***/

/* [BEGIN SYSCALLS] */
IMPORT ID tk_cre_tsk( CONST T_CTSK *pk_ctsk );
IMPORT ER tk_del_tsk( ID tskid );
IMPORT ER tk_sta_tsk( ID tskid, INT stacd );
IMPORT void tk_ext_tsk( void );
IMPORT void tk_exd_tsk( void );
IMPORT ER tk_ter_tsk( ID tskid );
IMPORT ER tk_dis_dsp( void );
IMPORT ER tk_ena_dsp( void );
IMPORT ER tk_chg_pri( ID tskid, PRI tskpri );
IMPORT ER tk_rot_rdq( PRI tskpri );
IMPORT ER tk_rel_wai( ID tskid );
IMPORT ID tk_get_tid( void );
IMPORT ER tk_get_reg( ID tskid, T_REGS *pk_regs, T_EIT *pk_eit, T_CREGS *pk_cregs );
IMPORT ER tk_set_reg( ID tskid, CONST T_REGS *pk_regs, CONST T_EIT *pk_eit, CONST T_CREGS *pk_cregs );
IMPORT ER tk_ref_tsk( ID tskid, T_RTSK *pk_rtsk );
IMPORT ER tk_sus_tsk( ID tskid );
IMPORT ER tk_rsm_tsk( ID tskid );
IMPORT ER tk_frsm_tsk( ID tskid );
IMPORT ER tk_slp_tsk( TMO tmout );
IMPORT ER tk_wup_tsk( ID tskid );
IMPORT INT tk_can_wup( ID tskid );
IMPORT ID tk_cre_sem( CONST T_CSEM *pk_csem );
IMPORT ER tk_del_sem( ID semid );
IMPORT ER tk_sig_sem( ID semid, INT cnt );
IMPORT ER tk_wai_sem( ID semid, INT cnt, TMO tmout );
IMPORT ER tk_ref_sem( ID semid, T_RSEM *pk_rsem );
IMPORT ID tk_cre_mtx( CONST T_CMTX *pk_cmtx );
IMPORT ER tk_del_mtx( ID mtxid );
IMPORT ER tk_loc_mtx( ID mtxid, TMO tmout );
IMPORT ER tk_unl_mtx( ID mtxid );
IMPORT ER tk_ref_mtx( ID mtxid, T_RMTX *pk_rmtx );
IMPORT ID tk_cre_flg( CONST T_CFLG *pk_cflg );
IMPORT ER tk_del_flg( ID flgid );
IMPORT ER tk_set_flg( ID flgid, UINT setptn );
IMPORT ER tk_clr_flg( ID flgid, UINT clrptn );
IMPORT ER tk_wai_flg( ID flgid, UINT waiptn, UINT wfmode, UINT *p_flgptn, TMO tmout );
IMPORT ER tk_ref_flg( ID flgid, T_RFLG *pk_rflg );
IMPORT ID tk_cre_mbx( CONST T_CMBX* pk_cmbx );
IMPORT ER tk_del_mbx( ID mbxid );
IMPORT ER tk_snd_mbx( ID mbxid, T_MSG *pk_msg );
IMPORT ER tk_rcv_mbx( ID mbxid, T_MSG **ppk_msg, TMO tmout );
IMPORT ER tk_ref_mbx( ID mbxid, T_RMBX *pk_rmbx );
IMPORT ID tk_cre_mbf( CONST T_CMBF *pk_cmbf );
IMPORT ER tk_del_mbf( ID mbfid );
IMPORT ER tk_snd_mbf( ID mbfid, CONST void *msg, INT msgsz, TMO tmout );
IMPORT INT tk_rcv_mbf( ID mbfid, void *msg, TMO tmout );
IMPORT ER tk_ref_mbf( ID mbfid, T_RMBF *pk_rmbf );
IMPORT ID tk_cre_por( CONST T_CPOR *pk_cpor );
IMPORT ER tk_del_por( ID porid );
IMPORT INT tk_cal_por( ID porid, UINT calptn, void *msg, INT cmsgsz, TMO tmout );
IMPORT INT tk_acp_por( ID porid, UINT acpptn, RNO *p_rdvno, void *msg, TMO tmout );
IMPORT ER tk_fwd_por( ID porid, UINT calptn, RNO rdvno, CONST void *msg, INT cmsgsz );
IMPORT ER tk_rpl_rdv( RNO rdvno, CONST void *msg, INT rmsgsz );
IMPORT ER tk_ref_por( ID porid, T_RPOR *pk_rpor );
IMPORT ER tk_def_int( UINT intno, CONST T_DINT *pk_dint );
IMPORT void tk_ret_int( void );
IMPORT ID tk_cre_mpl( CONST T_CMPL *pk_cmpl );
IMPORT ER tk_del_mpl( ID mplid );
IMPORT ER tk_get_mpl( ID mplid, SZ blksz, void **p_blk, TMO tmout );
IMPORT ER tk_rel_mpl( ID mplid, void *blk );
IMPORT ER tk_ref_mpl( ID mplid, T_RMPL *pk_rmpl );
IMPORT ID tk_cre_mpf( CONST T_CMPF *pk_cmpf );
IMPORT ER tk_del_mpf( ID mpfid );
IMPORT ER tk_get_mpf( ID mpfid, void **p_blf, TMO tmout );
IMPORT ER tk_rel_mpf( ID mpfid, void *blf );
IMPORT ER tk_ref_mpf( ID mpfid, T_RMPF *pk_rmpf );
IMPORT ER tk_set_tim( CONST SYSTIM *pk_tim );
IMPORT ER tk_get_tim( SYSTIM *pk_tim );
IMPORT ER tk_get_otm( SYSTIM *pk_tim );
IMPORT ER tk_dly_tsk( RELTIM dlytim );
IMPORT ID tk_cre_cyc( CONST T_CCYC *pk_ccyc );
IMPORT ER tk_del_cyc( ID cycid );
IMPORT ER tk_sta_cyc( ID cycid );
IMPORT ER tk_stp_cyc( ID cycid );
IMPORT ER tk_ref_cyc( ID cycid, T_RCYC *pk_rcyc );
IMPORT ID tk_cre_alm( CONST T_CALM *pk_calm );
IMPORT ER tk_del_alm( ID almid );
IMPORT ER tk_sta_alm( ID almid, RELTIM almtim );
IMPORT ER tk_stp_alm( ID almid );
IMPORT ER tk_ref_alm( ID almid, T_RALM *pk_ralm );
IMPORT ER tk_ref_ver( T_RVER *pk_rver );
IMPORT ER tk_ref_sys( T_RSYS *pk_rsys );
IMPORT ER tk_def_ssy( ID ssid, CONST T_DSSY *pk_dssy );
IMPORT ER tk_ref_ssy( ID ssid, T_RSSY *pk_rssy );

IMPORT ID tk_opn_dev( CONST UB *devnm, UINT omode );
IMPORT ER tk_cls_dev( ID dd, UINT option );
IMPORT ID tk_rea_dev( ID dd, W start, void *buf, SZ size, TMO tmout );
IMPORT ER tk_srea_dev( ID dd, W start, void *buf, SZ size, SZ *asize );
IMPORT ID tk_wri_dev( ID dd, W start, CONST void *buf, SZ size, TMO tmout );
IMPORT ER tk_swri_dev( ID dd, W start, CONST void *buf, SZ size, SZ *asize );
IMPORT ID tk_wai_dev( ID dd, ID reqid, SZ *asize, ER *ioer, TMO tmout );
IMPORT INT tk_sus_dev( UINT mode );
IMPORT ID tk_get_dev( ID devid, UB *devnm );
IMPORT ID tk_ref_dev( CONST UB *devnm, T_RDEV *pk_rdev );
IMPORT ID tk_oref_dev( ID dd, T_RDEV *pk_rdev );
IMPORT INT tk_lst_dev( T_LDEV *pk_ldev, INT start, INT ndev );
IMPORT INT tk_evt_dev( ID devid, INT evttyp, void *evtinf );
IMPORT ID tk_def_dev( CONST UB *devnm, CONST T_DDEV *pk_ddev, T_IDEV *pk_idev );
IMPORT ER tk_ref_idv( T_IDEV *pk_idev );
/* [END SYSCALLS] */

#ifdef __cplusplus
}
#endif

#endif /* __TK_SYSCALL_H__ */
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
// #include <cpuattr.h>
// #include <cpudef.h>
// #endif

// #if APP_H8S2212
// #include <cpuattr.h>
// #include <cpudef.h>
// #endif

// #if APP_MB9AF312K
// #include <cpuattr.h>
// #include <cpudef.h>
// #endif

#if _APP_RL78G13_R5F100ADASP_
#include <cpuattr.h>
#include <cpudef.h>
#endif

#endif /* __TK_SYSCALL_COMMON_H__ */
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
 *	@(#)sysdef.h
 *
 *	System dependencies definition.
 *	Included also from assembler program.
 */

#ifndef __TK_SYSDEF_H__
#define __TK_SYSDEF_H__

#include <sysdef_common.h>

#endif /* __TK_SYSDEF_H__ */
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
 *	@(#)sysdef_common.h (tk)
 *
 *	System dependencies definition.
 *	Included also from assembler program.
 */

#ifndef __TK_SYSDEF_COMMON_H__
#define __TK_SYSDEF_COMMON_H__

// #if APP_AT91
// #include <sysdef_depend.h>
// #endif

// #if APP_H8S2212
// #include <sysdef_depend.h>
// #endif

// #if APP_MB9AF312K
// #include <sysdef_depend.h>
// #endif

#if _APP_RL78G13_R5F100ADASP_
#include <sysdef_depend.h>
#endif

#endif /* __TK_SYSDEF_COMMON_H__ */
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
 *	@(#)sysdef_depend.h (FM3)
 *
 *	Definition about FM3
 *
 *	Included also from assembler program.
 */

#ifndef __TK_SYSDEF_DEPEND_H__
#define __TK_SYSDEF_DEPEND_H__

/*
 * Program status register (PSR)
 */
#define PSR_N		0x80000000	/* Condition flag Negative */
#define PSR_Z		0x40000000	/* Zero */
#define PSR_C		0x20000000	/* Carry */
#define PSR_V		0x10000000	/* Overflow */
#define PSR_Q		0x08000000	/* Saturation */

#define PSR_INT_MSK	0x000000FF	/* Interrupt status mask */

/* Exception model. */
#define EXP_M(n)	(n)		/* Exception model */
#define EXP_USR		EXP_M(0)	/* User mode, No Exception */
#define EXP_RST		EXP_M(1)	/* Reset */
#define EXP_NMI		EXP_M(2)	/* Non Maskable Interrupt */
#define EXP_HDF		EXP_M(3)	/* Hardware fault */
#define EXP_MEM		EXP_M(4)	/* Memory fault */
#define EXP_BUS		EXP_M(5)	/* Bus fault */
#define EXP_USF		EXP_M(6)	/* Usage fault */
#define EXP_SVC		EXP_M(11)	/* SVC call */
#define EXP_DBG		EXP_M(12)	/* Debug monitor */
#define EXP_PSV		EXP_M(14)	/* Software asynchronous System call */
#define EXP_STK		EXP_M(15)	/* System tick */
#define EXP_EXT(n)	(EXP_M(16) + n)	/* External interrupt */


/* ------------------------------------------------------------------------ */

/*
 * Parameter of SVC instruction
 */
#define SVC_SYSCALL		0x00	/* micro T-Kernel system call/extension SVC */
#define SVC_EXTENDED_SVC	0x10	/* Extended SVC */
#define SVC_DEBUG_SUPPORT	0xFF	/* Debugger support function */

/* ------------------------------------------------------------------------ */

/*
 * System control block
 */
#define SCB_ICSR	0xE000ED04
#define SCB_VTOR	0xE000ED08
#define SCB_AIRCR	0xE000ED0C

#define SCB_SCR		0xE000ED10
#define SCB_CCR		0xE000ED14
#define SCB_SHPR1	0xE000ED18
#define SCB_SHPR2	0xE000ED1C
#define SCB_SHPR3	0xE000ED20

#define SCB_STIR	0xE000EF00

#define ICSR_PENDSVSET	0x10000000	/* Trigger PendSV exception. */
#define ICSR_PENDSVCLR	0x08000000	/* Remove the pending state from the PendSV exception. */

/*
 * System Timer
 */
#define SYST_CSR	0xE000E010	/* SysTick Control and Status */
#define SYST_RVR	0xE000E014	/* SysTick Reload value */
#define SYST_CVR	0xE000E018	/* SysTick Current value */

/* ------------------------------------------------------------------------ */

/*
 * Clock Control
 */
/* Registers */
// #define FM3_CRG_BASE		0x40010000
// #define FM3_CRG_SCM_CTL		(*(volatile UW*)(FM3_CRG_BASE + 0x00))
// #define FM3_CRG_SCM_STR		(*(volatile UW*)(FM3_CRG_BASE + 0x04))
// #define FM3_CRG_STB_CTL		(*(volatile UW*)(FM3_CRG_BASE + 0x08))
// #define FM3_CRG_RST_STR		(*(volatile UW*)(FM3_CRG_BASE + 0x0C))
// #define FM3_CRG_BSC_PSR		(*(volatile UW*)(FM3_CRG_BASE + 0x10))
// #define FM3_CRG_APBC0_PSR	(*(volatile UW*)(FM3_CRG_BASE + 0x14))
// #define FM3_CRG_APBC1_PSR	(*(volatile UW*)(FM3_CRG_BASE + 0x18))
// #define FM3_CRG_APBC2_PSR	(*(volatile UW*)(FM3_CRG_BASE + 0x1C))
// #define FM3_CRG_SWC_PSR		(*(volatile UW*)(FM3_CRG_BASE + 0x20))
// #define FM3_CRG_TTC_PSR		(*(volatile UW*)(FM3_CRG_BASE + 0x28))
// #define FM3_CRG_CSW_TMR		(*(volatile UW*)(FM3_CRG_BASE + 0x30))
// #define FM3_CRG_PSW_TMR		(*(volatile UW*)(FM3_CRG_BASE + 0x34))
// #define FM3_CRG_PLL_CTL1	(*(volatile UW*)(FM3_CRG_BASE + 0x38))
// #define FM3_CRG_PLL_CTL2	(*(volatile UW*)(FM3_CRG_BASE + 0x3C))
// #define FM3_CRG_CSV_CTL		(*(volatile UW*)(FM3_CRG_BASE + 0x40))
// #define FM3_CRG_CSV_STR		(*(volatile UW*)(FM3_CRG_BASE + 0x44))
// #define FM3_CRG_FCSWH_CTL	(*(volatile UW*)(FM3_CRG_BASE + 0x48))
// #define FM3_CRG_FCSWL_CTL	(*(volatile UW*)(FM3_CRG_BASE + 0x4C))
// #define FM3_CRG_FCSWD_CTL	(*(volatile UW*)(FM3_CRG_BASE + 0x50))
// #define FM3_CRG_DBWDT_CTL	(*(volatile UW*)(FM3_CRG_BASE + 0x54))
// #define FM3_CRG_INT_ENR		(*(volatile UW*)(FM3_CRG_BASE + 0x60))
// #define FM3_CRG_INT_STR		(*(volatile UW*)(FM3_CRG_BASE + 0x64))
// #define FM3_CRG_INT_CLR		(*(volatile UW*)(FM3_CRG_BASE + 0x68))

#define CLOCK_SETUP	1
#define HWWD_DISABLE	1
#define CR_TRIM_SETUP	1
#define SCM_CTL_Val	0x00000052
#define CSW_TMR_Val	0x0000005C
#define PSW_TMR_Val	0x00000000
#define PLL_CTL1_Val	0x00000001
#define PLL_CTL2_Val	0x00000009
#define BSC_PSR_Val	0x00000000
#define APBC0_PSR_Val	0x00000001
#define APBC1_PSR_Val	0x00000081
#define APBC2_PSR_Val	0x00000081
#define SWC_PSR_Val	0x00000003
#define TTC_PSR_Val	0x00000000

/* ------------------------------------------------------------------------ */

/*
 *  Flash interface
 */
/* Registers */
// #define FM3_FLASH_IF_BASE	0x40000000
// #define FM3_FLASH_IF_FASZR	(*(volatile UW*)(FM3_FLASH_IF_BASE + 0x00))
// #define FM3_FLASH_IF_FRWTR	(*(volatile UW*)(FM3_FLASH_IF_BASE + 0x04))
// #define FM3_FLASH_IF_FSTR	(*(volatile UW*)(FM3_FLASH_IF_BASE + 0x08))
// #define FM3_FLASH_IF_FSYNDN	(*(volatile UW*)(FM3_FLASH_IF_BASE + 0x10))
// #define FM3_FLASH_IF_CRTRMM	(*(volatile UW*)(FM3_FLASH_IF_BASE + 0x100))

/* ------------------------------------------------------------------------ */

/*
 *  Hardware watchdog
 */
/* Registers */
// #define FM3_HWWDT_BASE		0x40011000
// #define FM3_HWWDT_WDG_LDR	(*(volatile UW*)(FM3_HWWDT_BASE + 0x00))
// #define FM3_HWWDT_WDG_VLR	(*(volatile UW*)(FM3_HWWDT_BASE + 0x04))
// #define FM3_HWWDT_WDG_CTL	(*(volatile UW*)(FM3_HWWDT_BASE + 0x08))
// #define FM3_HWWDT_WDG_ICL	(*(volatile UW*)(FM3_HWWDT_BASE + 0x0C))
// #define FM3_HWWDT_WDG_RIS	(*(volatile UW*)(FM3_HWWDT_BASE + 0x10))
// #define FM3_HWWDT_WDG_LCK	(*(volatile UW*)(FM3_HWWDT_BASE + 0xC00))

/* ------------------------------------------------------------------------ */

/*
 *  CR trimming
 */
/* Registers */
// #define FM3_CRTRIM_BASE		0x4002E000
// #define FM3_CRTRIM_MCR_PSR	(*(volatile UW*)(FM3_CRTRIM_BASE + 0x00))
// #define FM3_CRTRIM_MCR_FTRM	(*(volatile UW*)(FM3_CRTRIM_BASE + 0x04))
// #define FM3_CRTRIM_MCR_RLR	(*(volatile UW*)(FM3_CRTRIM_BASE + 0x0C))

/* ------------------------------------------------------------------------ */

/*
 * NVIC (Nested Vectored Interrupt Controller)
 */
#define NVIC_ICTR	0xE000E004

#define NVIC_ISER_BASE	0xE000E100
#define NVIC_ISER(x)	(NVIC_ISER_BASE + (((x) / 32) << 2))

#define NVIC_ICER_BASE	0xE000E180
#define NVIC_ICER(x)	(NVIC_ICER_BASE + (((x) / 32) << 2))

#define NVIC_ISPR_BASE	0xE000E200
#define NVIC_ISPR(x)	(NVIC_ISPR_BASE + (((x) / 32) << 2))

#define NVIC_ICPR_BASE	0xE000E280
#define NVIC_ICPR(x)	(NVIC_ICPR_BASE + (((x) / 32) << 2))

#define NVIC_IABR_BASE	0xE000E300
#define NVIC_IABR(x)	(NVIC_IABR_BASE + (((x) / 32) << 2))

#define NVIC_IPR_BASE	0xE000E400
#define NVIC_IPR(x)	(NVIC_IPR_BASE  + (x))

/* ------------------------------------------------------------------------ */
/*
 * The number of the implemented bit width for priority value fields.
 */
#define INTPRI_BITWIDTH		4

/*
 * force dispatch interrupt for micro T-Kernel
 */
#define INTNO_FORCE_DISPATCH	47
#define NVIC_IPR_FORCE_DISPATCH	NVIC_IPR(INTNO_FORCE_DISPATCH)

/*
 * Highest Ext. interrupt level for disint()
 */
#define MAX_EXTINT_PRI		0x10

/* ------------------------------------------------------------------------ */

#endif /* __TK_SYSDEF_DEPEND_H__ */
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
 *	@(#)sysinfo.h (sys)
 *
 *	System shared information  
 */

#ifndef __SYS_SYSINFO_H__
#define __SYS_SYSINFO_H__

#include <sysinfo_common.h>

#endif /* __SYS_SYSINFO_H__ */
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
 *	@(#)sysinfo_common.h (sys)
 *
 *	System shared information
 */

#ifndef __SYS_SYSINFO_COMMON_H__
#define __SYS_SYSINFO_COMMON_H__

// #ifdef APP_AT91
// #include <sysinfo_depend.h>
// #endif

// #ifdef APP_H8S2212
// #include <sysinfo_depend.h>
// #endif

// #ifdef _APP_MB9AF312K_
// #include <sysinfo_depend.h>
// #endif

#ifdef _APP_RL78G13_R5F100ADASP_
#include <sysinfo_depend.h>
#endif

#endif /* __SYS_SYSINFO_COMMON_H__ */
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
 *	@(#)sysinfo_depend.h (FM3)
 *
 *	System common information  
 */

#ifndef __SYS_SYSINFO_DEPEND_H__
#define __SYS_SYSINFO_DEPEND_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _in_asm_source_

#define N_INTVEC	48

IMPORT	FP	knl_intvec[];
IMPORT	W	knl_taskindp;
IMPORT	UW	knl_taskmode;

#endif /* _in_asm_source_ */

#ifdef __cplusplus
}
#endif
#endif /* __SYS_SYSINFO_DEPEND_H__ */
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

#include <basic.h>
#include <tkernel.h>
#include <tmonitor.h>

/*
 * Platform dependent sequence
 */
IMPORT ER knl_init_device( void );
IMPORT ER knl_start_device( void );
IMPORT ER knl_finish_device( void );
IMPORT ER knl_restart_device( W mode );

#endif /* _SYSINIT_ */
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
 *	sysinit_main.c (sysinit)
 *	Initialize System
 */
/*--------------------------------------------------------------------*/
/*  Include definition                                                */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Struct definition                                                 */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Constant definition                                               */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Variable definition                                               */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Macro definition                                                  */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Prototype declaration                                             */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Function definition                                               */
/*--------------------------------------------------------------------*/

#include <tk_typedef.h>
#include <errno.h>
#include <typedef.h>
#include <syscall.h>
#include <cpu_status.h>

/*
 * Initialize sequence before micro T-Kernel starts
 *	Perform preparation necessary to start micro T-Kernel.
 */
IMPORT void knl_init_system( void );

/*
 * Start micro T-Kernel
 *	Start micro T-Kernel and the initial task specified by 'ctsk'.
 */
IMPORT void knl_t_kernel_main( T_CTSK *ctsk );

/*
 * Start System
 *	At this point, start each subsystem and each device driver.
 *	Return from function after starting.
 */
IMPORT void knl_start_system( void );

#if USE_CLEANUP
/*
 * Stop System
 *	Never return from this function.
 *
 *	fin  =	 0 : Power off
 *		-1 : reset and re-start	(Reset -> Boot -> Start)
 *		-2 : fast re-start		(Start)
 *		-3 : Normal re-start		(Boot -> Start)
 *
 *	fin are not always supported.
 */
IMPORT void knl_shutdown_system( INT fin );
#endif /* USE_CLEANUP */

/*
 * Main initial task sequence (sysmain)
 */
IMPORT INT knl_init_task_main( void );

/* ------------------------------------------------------------------------ */

/*
 * Initial task
 */
EXPORT void knl_init_task(void)
{
	INT fin;

#if USE_SYSDEPEND_PATCH1
	/* System-dependent processes (before knl_start_system) */
	knl_sysdepend_patch1();
#endif

	/* Start system */
	knl_start_system();

#if USE_SYSDEPEND_PATCH2
	/* System-dependent processes (after knl_start_system) */
	knl_sysdepend_patch2();
#endif

	/* Initial task main */
	fin = knl_init_task_main();

	/* Stop System */
#if USE_CLEANUP
	knl_shutdown_system(fin);	/* Never return */
#else
	DISABLE_INTERRUPT;
	for(;;) {
		;
	}
#endif /* USE_CLEANUP */
}

/*
 * Initial task creation parameter
 */
IMPORT const T_CTSK knl_c_init_task;

/*
 * Entry for starting Kernel
 */
EXPORT int main( void )
{
	/* Initialize sequence before T-Kernel starts */
	knl_init_system();

	/* Start T-Kernel */
	knl_t_kernel_main((T_CTSK *)&knl_c_init_task);	/* Never return */

	return 0;
}

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
 *	@(#)syslib.h
 *
 *	System Library
 */

#ifndef __TK_SYSLIB_H__
#define __TK_SYSLIB_H__

#include <basic.h>
#include <tk_typedef.h>

#include <syslib_common.h>

#endif /* __TK_SYSLIB_H__ */
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
 *	@(#)syslib_common.h (tk)
 *
 *	System Library
 */

#ifndef __TK_SYSLIB_COMMON_H__
#define __TK_SYSLIB_COMMON_H__

/*
 * CPU-dependent
 */
// #if APP_AT91
// #include <syslib_depend.h>
// #endif

// #if APP_H8S2212
// #include <syslib_depend.h>
// #endif

// #if APP_MB9AF312K
// #include <syslib_depend.h>
// #endif

#if _APP_RL78G13_R5F100ADASP_
#include <syslib_depend.h>
#endif

#endif /* __TK_SYSLIB_COMMON_H__ */
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
 *	@(#)syslib_depend.h (FM3)
 *
 *	micro T-Kernel FM3 Library
 */

#ifndef __TK_SYSLIB_DEPEND_H__
#define __TK_SYSLIB_DEPEND_H__

#include <errno.h>
#include <sysdef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * CPU interrupt control
 *	As armv7-m architecture does not support disable interrupt in
 *	xpsr register, we have to raise the excution priority to 
 *	that the interrupt group have. Write the BASEPRI to implement 
 *	disint and enaint.
 */
IMPORT UINT disint( void );
IMPORT UINT enaint( UINT intsts );
IMPORT UINT get_basepri( void );
#define DI(intsts)	( (intsts) = disint() )
#define EI(intsts)	( enaint(intsts) )
#define isDI(intsts)	( (intsts) != 0 )

/* Convert to interrupt definition number */
#define DINTNO(intvec)	(intvec)

#define INTLEVEL_DI	(0)
#define INTLEVEL_EI	(255)

/*
 * Interrupt priority grouping
 *
 *	PRIGROUP in the AIRCR register determins the split of group
 *	priority from subpriority. PRIGROUP is initialized to 3 ('011'
 *	in binary) in the boot sequence.
 */
#define INTPRI_GROUP(pri, subpri)	(((pri) << (8-INTPRI_BITWIDTH)) | (subpri))

/*
 * Set Interrupt Mask Level in CPU
 */
IMPORT void SetCpuIntLevel( INT level );

/*
 * Get Interrupt Mask Level in CPU
 */
IMPORT INT GetCpuIntLevel( void );

/*
 * Interrupt enable
 *	Enable the interrupt specified by 'intno.'
 */
IMPORT void EnableInt( UINT intno, INT level );

/*
 * Interrupt disable
 *	Disable the interrupt specified by 'intno.'
 */
IMPORT void DisableInt( UINT intno );

/*
 * Clear interrupt
 *
 *	Clear the state that the specified interrupt has been
 *	asserted. 
 *
 *	We just clear the pending state from the specified interrupt.
 *	Because the processor automatically removes the pending state
 *	from the interrupt when it enters the interrupt service
 *	routine, and removes the active state from the interrupt when
 *	it returns from the interrupt service routine.
 */
#define ClearInt(intno) ClearPendingInt(intno)

/*
 * Issue EOI to Interrupt Controller.
 */
#define EndOfInt(intno)

/*
 * Check active state
 *	Current active state for the associated interrupt
 */
IMPORT BOOL CheckInt( UINT intno );

/* ------------------------------------------------------------------------ */
/*
 * Interrupt Controller functions for Cortex-M3.
 */

/*
 * Set-Pending
 *  Pends the associated interrupt under software control.
 */
IMPORT void SetPendingInt( UINT intno );

/*
 * Clear-Pending
 *	Un-pends the associated interrupt under software control.
 */
IMPORT void ClearPendingInt( UINT intno );

/* ------------------------------------------------------------------------ */

/*
 * I/O port access
 *	Only memory mapped I/O for Arm
 */
void out_w( UW port, UW data )
{
	*(_UW*)port = data;
}
void out_h( UW port, UH data )
{
	*(_UH*)port = data;
}
void out_b( UW port, UB data )
{
	*(_UB*)port = data;
}

UW in_w( UW port )
{
	return *(_UW*)port;
}
UH in_h( UW port )
{
	return *(_UH*)port;
}
UB in_b( UW port )
{
	return *(_UB*)port;
}

#ifdef __cplusplus
}
#endif
#endif /* __TK_SYSLIB_DEPEND_H__ */
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
 *	sysmain.h (sysmain)
 *	Kernel Main
 */

#ifndef _SYSMAIN_
#define _SYSMAIN_

#include <basic.h>
#include <tkernel.h>
#include <tmonitor.h>

/* Boot message */
#define BOOT_MESSAGE \
	"\n" \
	"micro T-Kernel Version 2.00.00\n" \
	"\n\0"

/*
 * User main
 */
IMPORT INT usermain( void );

#endif /* _SYSMAIN_ */
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
 *	sysmgr.h
 */

#ifndef _SYSMGR_
#define _SYSMGR_

#include "utk_config.h"

#include <basic.h>
#include <tkernel.h>
#include <libstr.h>
#include <queue.h>
#include <util.h>
#include <debug.h>

#include <imalloc.h>

/* ------------------------------------------------------------------------ */
/*
 *	Device management function
 */

/*
 * Lock for device management exclusive control
 */
IMPORT FastMLock	knl_DevMgrLock;
#define LockDM()	MLock(&knl_DevMgrLock, 0)
#define UnlockDM()	MUnlock(&knl_DevMgrLock, 0)

/*
 * Lock for device registration exclusive control
 */
#define LockREG()	MLock(&knl_DevMgrLock, 1)
#define UnlockREG()	MUnlock(&knl_DevMgrLock, 1)

/*
 * Device registration information
 */
typedef struct DeviceControlBlock {
	QUEUE	q;
	UB	devnm[L_DEVNM+1];	/* Device name */
	T_DDEV	ddev;			/* Registration information */
	QUEUE	openq;			/* Open device management queue */
} DevCB;

IMPORT	DevCB		knl_DevCBtbl[];	/* Device registration information
					   table */
IMPORT	QUEUE		knl_UsedDevCB;	/* In-use queue */

#define DID(devcb)		( ((devcb) - knl_DevCBtbl + 1) << 8 )
#define DEVID(devcb, unitno)	( DID(devcb) + (unitno) )
#define DEVCB(devid)		( knl_DevCBtbl + (((devid) >> 8) - 1) )
#define UNITNO(devid)		( (devid) & 0xff )

/*
 * Open management information
 */
typedef struct OpenControlBlock {
	QUEUE		q;
	QUEUE		resq;		/* For connection from resource
					   management */
	ID		resid;		/* Section resource ID */
	DevCB		*devcb;		/* Target device */
	INT		unitno;		/* Subunit number
					   (0: Physical device) */
	UINT		omode;		/* Open mode */
	QUEUE		requestq;	/* Request management queue */
	UH		waitone;	/* Number of individual request
					   waits */
	T_DEVREQ	*waireqlst;	/* List of multiple request waits */
	INT		nwaireq;	/* Number of multiple request waits */
	ID		abort_tskid;	/* Abort completion wait task */
	INT		abort_cnt;	/* Number of abort completion wait
					   requests */
	ID		abort_semid; /* Semaphore for abort completion wait */
} OpnCB;

#define RESQ_OPNCB(rq)		( (OpnCB*)((B*)(rq) - offsetof(OpnCB, resq)) )

/*
 * Request management information
 */
typedef struct RequestControlBlock {
	QUEUE		q;
	OpnCB		*opncb;		/* Open device */
	ID		tskid;		/* Processing task */
	T_DEVREQ	req;		/* Request packet */
} ReqCB;

/*
 * Resource management information
 */
typedef struct ResourceControlBlock {
	QUEUE		openq;		/* Open device management queue */
	INT		dissus;		/* Suspend disable request count */
} ResCB;

/*
 * Request function types
 */

typedef ER  (*OPNFN)( ID devid, UINT omode, void *exinf );
typedef ER  (*ABTFN)( ID tskid, T_DEVREQ *devreq, INT nreq, void *exinf );
typedef INT (*WAIFN)( T_DEVREQ *devreq, INT nreq, TMO tmout, void *exinf );
typedef INT (*EVTFN)( INT evttyp, void *evtinf, void *exinf );
typedef ER  (*CLSFN)( ID devid, UINT option, void *exinf );
typedef ER  (*EXCFN)( T_DEVREQ *devreq, TMO tmout, void *exinf );

/* ------------------------------------------------------------------------ */

#if TA_GP
/*
 * Device driver call 
 */
IMPORT INT knl_CallDeviceDriver( INT p1, INT p2, INT p3, INT p4, FP drv, void *gp );
#define CallDeviceDriver(p1, p2, p3, p4, drv, gp ) \
		knl_CallDeviceDriver((INT)(p1), (INT)(p2), (INT)(p3), (INT)(p4), \
							(FP)(drv), (gp))
#endif

#define IMPORT_DEFINE	1
#if IMPORT_DEFINE
/* device.c */
IMPORT	FastMLock	knl_DevMgrLock;
IMPORT	DevCB		knl_DevCBtbl[];
IMPORT	QUEUE		knl_UsedDevCB;
IMPORT	DevCB*		knl_searchDevCB( CONST UB *devnm );
IMPORT	INT		knl_phydevnm( UB *pdevnm, CONST UB *ldevnm );
IMPORT	ER		knl_initialize_devmgr( void );
IMPORT	ER		knl_finish_devmgr( void );
/* deviceio.c */
IMPORT ER knl_check_devdesc( ID dd, UINT mode, OpnCB **p_opncb );
IMPORT void knl_devmgr_startup( void );
IMPORT void knl_devmgr_cleanup( void );
IMPORT ER knl_initDevIO( void );
IMPORT ER knl_finishDevIO( void );

#endif

#endif /* _SYSMGR_ */
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
 *	sysstartup.c (sysinit)
 *	Start/Stop System
 */
/*--------------------------------------------------------------------*/
/*  Include definition                                                */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Struct definition                                                 */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Constant definition                                               */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Variable definition                                               */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Macro definition                                                  */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Prototype declaration                                             */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Function definition                                               */
/*--------------------------------------------------------------------*/

#include "sysinit.h"
#include "kernel.h"

/*
 * Manager/Driver
 */
IMPORT ER knl_init_Imalloc( void );			/* Internal memory allocation */
#if CFN_MAX_REGDEV > 0
IMPORT ER knl_initialize_devmgr( void );
#endif

/* ------------------------------------------------------------------------ */

/*
 * Initialize sequence before T-Kernel starts
 */
EXPORT void knl_init_system( void )
{
	ER	ercd;

	/* Platform dependent initialize sequence */
	ercd = knl_init_device();
	if ( ercd < E_OK ) {
		goto err_ret;
	}

	/* Initialize Imalloc */
#if USE_IMALLOC
	ercd = knl_init_Imalloc();
	if ( ercd < E_OK ) {
		goto err_ret;
	}
#endif /* USE_IMALLOC */

	return;

err_ret:
#if USE_KERNEL_MESSAGE
	tm_putstring((UB*)"!ERROR! init_kernel\n");
#endif
	tm_monitor(); /* Stop */
}

/*
 * Start system
 */
EXPORT void knl_start_system( void )
{
	ER	ercd;

#if CFN_MAX_REGDEV > 0
	/* Initialize Device manager */
	ercd = knl_initialize_devmgr();
	if ( ercd < E_OK ) {
		goto err_ret;
	}
#endif

	/* Start system dependent sequence */
	ercd = knl_start_device();
	if ( ercd < E_OK ) {
		goto err_ret;
	}

	return;

err_ret:
#if USE_KERNEL_MESSAGE
	tm_putstring((UB*)"!ERROR! start_system\n");
#endif
	tm_monitor();	/* Stop */
}

#if USE_CLEANUP
/*
 * Stop system
 */
EXPORT void knl_shutdown_system( INT fin )
{
	/* Platform dependent finalize sequence */
	knl_finish_device();

	/* Shutdown message output */
#if USE_KERNEL_MESSAGE
	if ( fin >= 0 ) {
	  tm_putstring((UB*)"\n<< SYSTEM SHUTDOWN >>\n");
	}
#endif

	if ( fin < 0 ) {
		/* Re-start sequence (platform dependent) */
		knl_restart_device(fin);
	}

	/* Stop system */
	knl_t_kernel_exit();
}
#endif /* USE_CLEANUP */
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
 *	task.c
 *	Task Control
 */

/** [BEGIN Common Definitions] */
#include "kernel.h"
#include "task.h"
#include "ready_queue.h"
#include "wait.h"
#include "cpu_task.h"
#include "tkdev_timer.h"
#include "check.h"
/** [END Common Definitions] */

#ifdef USE_FUNC_CTXTSK
/*
 * Task dispatch disable state
 */
Noinit(EXPORT INT	knl_dispatch_disabled);	/* DDS_XXX see task.h */

/*
 * Task execution control 
 */
Noinit(EXPORT TCB	*knl_ctxtsk);	/* Task in execution */
Noinit(EXPORT TCB	*knl_schedtsk);	/* Task which should be executed */
Noinit(EXPORT RDYQUE	knl_ready_queue);	/* Ready queue */
#endif /* USE_FUNC_CTXTSK */

#ifdef USE_FUNC_TCB_TABLE
/*
 * Task control information
 */
Noinit(EXPORT TCB	knl_tcb_table[NUM_TSKID]);	/* Task control block */
Noinit(EXPORT QUEUE	knl_free_tcb);	/* FreeQue */
#endif /* USE_FUNC_TCB_TABLE */

#ifdef USE_FUNC_TASK_INITIALIZE
/*
 * TCB Initialization
 */
EXPORT ER knl_task_initialize( void )
{
	INT	i;
	TCB	*tcb;
	ID	tskid;

	/* Get system information */
	if ( NUM_TSKID < 1 ) {
		return E_SYS;
	}

	/* Initialize task execution control information */
	knl_ctxtsk = knl_schedtsk = NULL;
	knl_ready_queue_initialize(&knl_ready_queue);
	knl_dispatch_disabled = DDS_ENABLE;

	/* Register all TCBs onto FreeQue */
	QueInit(&knl_free_tcb);
	for ( tcb = knl_tcb_table, i = 0; i < NUM_TSKID; tcb++, i++ ) {
		tskid = ID_TSK(i);
		tcb->tskid = tskid;
		tcb->state = TS_NONEXIST;
#if CFN_MAX_PORID > 0
		tcb->wrdvno = tskid;
#endif

		QueInsert(&tcb->tskque, &knl_free_tcb);
	}

	return E_OK;
}
#endif /* USE_FUNC_TASK_INITIALIZE */

#ifdef USE_FUNC_MAKE_DORMANT
/*
 * Prepare task execution.
 */
EXPORT void knl_make_dormant( TCB *tcb )
{
	/* Initialize variables which should be reset at DORMANT state */
	tcb->state	= TS_DORMANT;
	tcb->priority	= tcb->bpriority = tcb->ipriority;
	tcb->sysmode	= tcb->isysmode;
	tcb->wupcnt	= 0;
	tcb->suscnt	= 0;

	tcb->klockwait	= FALSE;
	tcb->klocked	= FALSE;

#if USE_DBGSPT && defined(USE_FUNC_TD_INF_TSK)
	tcb->stime	= 0;
	tcb->utime	= 0;
#endif

	tcb->wercd = NULL;

#if CFN_MAX_MTXID > 0
	tcb->mtxlist	= NULL;
#endif

	/* Set context to start task */
	knl_setup_context(tcb);
}
#endif /* USE_FUNC_MAKE_DORMANT */

/* ------------------------------------------------------------------------ */

#ifdef USE_FUNC_MAKE_READY
/*
 * Set task to READY state.
 *	Update the task state and insert in the ready queue. If necessary, 
 *	update 'knl_schedtsk' and request to start task dispatcher. 
 */
EXPORT void knl_make_ready( TCB *tcb )
{
	tcb->state = TS_READY;
	if ( knl_ready_queue_insert(&knl_ready_queue, tcb) ) {
		knl_schedtsk = tcb;
		knl_dispatch_request();
	}
}
#endif /* USE_FUNC_MAKE_READY */

#ifdef USE_FUNC_MAKE_NON_READY
/*
 * Set task to non-executable state.
 *	Delete the task from the ready queue.
 *	If the deleted task is 'knl_schedtsk', set 'knl_schedtsk' to the
 *	highest priority task in the ready queue. 
 *	'tcb' task must be READY.
 */
EXPORT void knl_make_non_ready( TCB *tcb )
{
	knl_ready_queue_delete(&knl_ready_queue, tcb);
	if ( knl_schedtsk == tcb ) {
		knl_schedtsk = knl_ready_queue_top(&knl_ready_queue);
		knl_dispatch_request();
	}
}
#endif /* USE_FUNC_MAKE_NON_READY */

#ifdef USE_FUNC_CHANGE_TASK_PRIORITY
/*
 * Change task priority.
 */
EXPORT void knl_change_task_priority( TCB *tcb, INT priority )
{
	INT	oldpri;

	if ( tcb->state == TS_READY ) {
		/*
		 * When deleting a task from the ready queue, 
		 * a value in the 'priority' field in TCB is needed. 
		 * Therefore you need to delete the task from the
		 * ready queue before changing 'tcb->priority.'
		 */
		knl_ready_queue_delete(&knl_ready_queue, tcb);
		tcb->priority = (UB)priority;
		knl_ready_queue_insert(&knl_ready_queue, tcb);
		knl_reschedule();
	} else {
		oldpri = tcb->priority;
		tcb->priority = (UB)priority;

		/* If the hook routine at the task priority change is defined,
		   execute it */
		if ( (tcb->state & TS_WAIT) != 0 && tcb->wspec->chg_pri_hook) {
			(*tcb->wspec->chg_pri_hook)(tcb, oldpri);
		}
	}
}
#endif /* USE_FUNC_CHANGE_TASK_PRIORITY */

#ifdef USE_FUNC_ROTATE_READY_QUEUE
/*
 * Rotate ready queue.
 */
EXPORT void knl_rotate_ready_queue( INT priority )
{
	knl_ready_queue_rotate(&knl_ready_queue, priority);
	knl_reschedule();
}
#endif /* USE_FUNC_ROTATE_READY_QUEUE */

#ifdef USE_FUNC_ROTATE_READY_QUEUE_RUN
/*
 * Rotate the ready queue including the highest priority task.
 */
EXPORT void knl_rotate_ready_queue_run( void )
{
	if ( knl_schedtsk != NULL ) {
		knl_ready_queue_rotate(&knl_ready_queue,
				knl_ready_queue_top_priority(&knl_ready_queue));
		knl_reschedule();
	}
}
#endif /* USE_FUNC_ROTATE_READY_QUEUE_RUN */

/* ------------------------------------------------------------------------ */
/*
 *	Debug support function
 */
#if USE_DBGSPT

#ifdef USE_FUNC_TD_RDY_QUE
/*
 * Refer ready queue
 */
SYSCALL INT td_rdy_que_impl( PRI pri, ID list[], INT nent )
{
	QUEUE	*q, *tskque;
	INT	n = 0;

	CHECK_PRI(pri);

	BEGIN_DISABLE_INTERRUPT;
	tskque = &knl_ready_queue.tskque[int_priority(pri)];
	for ( q = tskque->next; q != tskque; q = q->next ) {
		if ( n++ < nent ) {
			*(list++) = ((TCB*)q)->tskid;
		}
	}
	END_DISABLE_INTERRUPT;

	return n;
}
#endif /* USE_FUNC_TD_RDY_QUE */

#endif /* USE_DBGSPT */
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
 *	task.h
 *	Task Definition
 */

#ifndef _TASK_
#define _TASK_

#ifndef __mtxcb__
#define __mtxcb__
typedef struct mutex_control_block	MTXCB;
#endif

#ifndef __tcb__
#define __tcb__
typedef struct task_control_block	TCB;
#endif

#include <queue.h>
#include <str_align.h>
#include "timer.h"
#include "winfo.h"

/*
 * Internal expression of task state
 *	Can check with 'state & TS_WAIT' whether the task is in the wait state.
 *	Can check with 'state & TS_SUSPEND' whether the task is in the forced 
 *	wait state.
 */
typedef enum {
	TS_NONEXIST	= 0,	/* Unregistered state */
	TS_READY	= 1,	/* RUN or READY state */
	TS_WAIT		= 2,	/* WAIT state */
	TS_SUSPEND	= 4,	/* SUSPEND state */
	TS_WAITSUS	= 6,	/* Both WAIT and SUSPEND state */
	TS_DORMANT	= 8	/* DORMANT state */
} TSTAT;

/*
 * If the task is alive ( except NON-EXISTENT,DORMANT ), return TRUE.
 */
BOOL knl_task_alive( TSTAT state )
{
	return ( (state & (TS_READY|TS_WAIT|TS_SUSPEND)) != 0 );
}


/*
 * Task priority internal/external expression conversion macro
 */
#define int_priority(x)		( (INT)((x) - MIN_PRI) )
#define ext_tskpri(x)		( (PRI)((x) + MIN_PRI) )

/*
 * Task control block (TCB)
 */
struct task_control_block {
	QUEUE	tskque;		/* Task queue */
	ID	tskid;		/* Task ID */
	void	*exinf;		/* Extended information */
	ATR	tskatr;		/* Task attribute */
	FP	task;		/* Task startup address */

	W	sstksz;		/* stack size */

	INT	:0;		/* ### From here */
	B	isysmode;	/* Task operation mode initial value */
	H	sysmode;	/* Task operation mode, quasi task part
				   call level */
	INT	:0;		/* ### To here, since it might be accessed
				   from outside of the critical section,
				   need to be assigned as an independent
				   word. Also, there is a case where one
				   word is read from 'reqdct' and is read
				   all at once from 'reqdct', 'isysmode',
				   and 'sysmode', so do not change the
				   order and size. */

	UB	ipriority;	/* Priority at task startup */
	UB	bpriority;	/* Base priority */
	UB	priority;	/* Current priority */

	UB /*TSTAT*/	state;	/* Task state (Int. expression) */

	BOOL	klockwait:1;	/* TRUE at wait kernel lock */
	BOOL	klocked:1;	/* TRUE at hold kernel lock */

	CONST WSPEC *wspec;	/* Wait specification */
	ID	wid;		/* Wait object ID */
	INT	wupcnt;		/* Number of wakeup requests queuing */
	INT	suscnt;		/* Number of SUSPEND request nests */
	ER	*wercd;		/* Wait error code set area */
	WINFO	winfo;		/* Wait information */
	TMEB	wtmeb;		/* Wait timer event block */

#if CFN_MAX_PORID > 0
	RNO	wrdvno;		/* For creating rendezvous number */
#endif
#if CFN_MAX_MTXID > 0
	MTXCB	*mtxlist;	/* List of hold mutexes */
#endif

#if USE_DBGSPT && defined(USE_FUNC_TD_INF_TSK)
	UW	stime;		/* System execution time (ms) */
	UW	utime;		/* User execution time (ms) */
#endif

	void	*isstack;	/* stack pointer initial value */
#if TA_GP
	void	*gp;		/* Global pointer */
#endif
	_align64		/* alignment for CTXB.ssp */
	CTXB	tskctxb;	/* Task context block */
#if USE_OBJECT_NAME
	UB	name[OBJECT_NAME_LENGTH];	/* name */
#endif
};

/*
 * Task dispatch disable state
 *	0 = DDS_ENABLE		 : ENABLE
 *	1 = DDS_DISABLE_IMPLICIT : DISABLE with implicit process
 *	2 = DDS_DISABLE		 : DISABLE with tk_dis_dsp()
 *	|	|
 *	|	use in *.c
 *	use in *.S
 *	  --> Do NOT change these literals, because using in assembler code
 *
 *	'dispatch_disabled' records dispatch disable status set by tk_dis_dsp()
 *	for some CPU, that accepts delayed interrupt.
 *	In this case, you can NOT refer the dispatch disabled status
 *	only by 'dispatch_disabled'.
 *	Use 'in_ddsp()' to refer the task dispatch status.
 *	'in_ddsp()' is a macro definition in CPU-dependent definition files.
 */
#define DDS_ENABLE		(0)
#define DDS_DISABLE_IMPLICIT	(1)	/* set with implicit process */
#define DDS_DISABLE		(2)	/* set by tk_dis_dsp() */
IMPORT INT	knl_dispatch_disabled;

/*
 * Task in execution
 *	ctxtsk is a variable that indicates TCB task in execution
 *	(= the task that CPU holds context). During system call processing,
 *	when checking information about the task that requested system call,
 *	use 'ctxtsk'. Only task dispatcher changes 'ctxtsk'.
 */
IMPORT TCB	*knl_ctxtsk;

/*
 * Task which should be executed
 *	'schedtsk' is a variable that indicates the task TCB to be executed.
 *	If a dispatch is delayed by the delayed dispatch or dispatch disable, 
 *	it does not match with 'ctxtsk.' 
 */
IMPORT TCB	*knl_schedtsk;

/*
 * Task control information
 */
IMPORT TCB	knl_tcb_table[];	/* Task control block */
IMPORT QUEUE	knl_free_tcb;	/* FreeQue */

/*
 * Get TCB from task ID.
 */
#define get_tcb(id)		( &knl_tcb_table[INDEX_TSK(id)] )
#define get_tcb_self(id)	( ( (id) == TSK_SELF )? knl_ctxtsk: get_tcb(id) )

/*
 * Prepare task execution.
 */
IMPORT void knl_make_dormant( TCB *tcb );

/*
 * Make task executable.
 *	If the 'tcb' task priority is higher than the executed task,
 *	make it executable. If the priority is lower, connect the task to the 
 *	ready queue.
 */
IMPORT void knl_make_ready( TCB *tcb );

/*
 * Make task non-executable.
 *	Change the 'tcb' task state to be a non-executable state (wait state, 
 *	forced wait, or dormant state). When calling this function, the
 *	task must be executable. Change 'tcb->state' on the caller side
 *	after returning from this function.
 */
IMPORT void knl_make_non_ready( TCB *tcb );

/*
 * Change task priority.
 *	Change 'tcb' task priority to 'priority'.
 *	Then make the required task state transition occur.
 */
IMPORT void knl_change_task_priority( TCB *tcb, INT priority );

/*
 * Rotate ready queue.
 *	'rotate_ready_queue' rotates the priority ready queue at 'priority'.
 *	'rotate_ready_queue_run' rotates the ready queue including the highest 
 *	priority task in the ready queue.
 */
IMPORT void knl_rotate_ready_queue( INT priority );
IMPORT void knl_rotate_ready_queue_run( void );


#include "ready_queue.h"

/*
 * Reselect task to execute
 *	Set 'schedtsk' to the head task at the ready queue.
 */
void knl_reschedule( void )
{
	TCB	*toptsk;

	toptsk = knl_ready_queue_top(&knl_ready_queue);
	if ( knl_schedtsk != toptsk ) {
		knl_schedtsk = toptsk;
		knl_dispatch_request();
	}
}

IMPORT void knl_del_tsk( TCB *tcb );
IMPORT void knl_ter_tsk( TCB *tcb );

#endif /* _TASK_ */
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
 *	task_manage.c
 *	Task Management Function
 */

/** [BEGIN Common Definitions] */
#include "kernel.h"
#include "task.h"
#include "wait.h"
#include "check.h"
#include "cpu_task.h"
#include <tmonitor.h>
/** [END Common Definitions] */


#ifdef USE_FUNC_TK_CRE_TSK
/*
 * Create task
 */
SYSCALL ID tk_cre_tsk_impl P1( CONST T_CTSK *pk_ctsk )
{
#if CHK_RSATR
	const ATR VALID_TSKATR = {	/* Valid value of task attribute */
		 TA_HLNG
		|TA_RNG3
		|TA_USERBUF
		|TA_GP
#if USE_OBJECT_NAME
		|TA_DSNAME
#endif
	};
#endif
	TCB	*tcb;
	W	sstksz;
	void	*stack;
	ER	ercd;

	CHECK_RSATR(pk_ctsk->tskatr, VALID_TSKATR);
#if !USE_IMALLOC
	/* TA_USERBUF must be specified if configured in no Imalloc */
	CHECK_PAR((pk_ctsk->tskatr & TA_USERBUF) != 0);
#endif
	CHECK_PRI(pk_ctsk->itskpri);
	CHECK_PAR(pk_ctsk->stksz >= MIN_SYS_STACK_SIZE);

	/* Adjust stack size by 8 bytes */
	sstksz  = (pk_ctsk->stksz  + 7) / 8 * 8;

#if USE_IMALLOC
	if ( (pk_ctsk->tskatr & TA_USERBUF) != 0 ) {
		/* Size of User buffer must be multiples of 8 */
		if ( sstksz != pk_ctsk->stksz ) {
			return E_PAR;
		}
		/* Use user buffer */
		stack = pk_ctsk->bufptr;
	} else {
		/* Allocate system stack area */
		stack = knl_Imalloc((UW)sstksz);
		if ( stack == NULL ) {
			return E_NOMEM;
		}
	}
#else
	/* Size of User buffer must be multiples of 8 */
	if ( sstksz != pk_ctsk->stksz ) {
		return E_PAR;
	}
	/* Use user buffer */
	stack = pk_ctsk->bufptr;
#endif

	BEGIN_CRITICAL_SECTION;
	/* Get control block from FreeQue */
	tcb = (TCB*)QueRemoveNext(&knl_free_tcb);
	if ( tcb == NULL ) {
		ercd = E_LIMIT;
		goto error_exit;
	}

	/* Initialize control block */
	tcb->exinf     = pk_ctsk->exinf;
	tcb->tskatr    = pk_ctsk->tskatr & ~TA_RNG3; /* handling as TA_RNG0 */
	tcb->task      = pk_ctsk->task;
	tcb->ipriority = (UB)int_priority(pk_ctsk->itskpri);
	tcb->sstksz    = sstksz;
#if USE_OBJECT_NAME
	if ( (pk_ctsk->tskatr & TA_DSNAME) != 0 ) {
		strncpy((char*)tcb->name, (char*)pk_ctsk->dsname, OBJECT_NAME_LENGTH);
	}
#endif
#if TA_GP
	/* Set global pointer */
	if ( (pk_ctsk->tskatr & TA_GP) != 0 ) {
		gp = pk_ctsk->gp;
	}
	tcb->gp = gp;
#endif

	/* Set stack pointer */
	tcb->isstack = (VB*)stack + sstksz;

	/* Set initial value of task operation mode */
	tcb->isysmode = 1;
	tcb->sysmode  = 1;

	/* make it to DORMANT state */
	knl_make_dormant(tcb);

	ercd = tcb->tskid;

    error_exit:
	END_CRITICAL_SECTION;

#if USE_IMALLOC
	if ( (ercd < E_OK) && ((pk_ctsk->tskatr & TA_USERBUF) == 0) ) {
		knl_Ifree(stack);
	}
#endif

	return ercd;
}
#endif /* USE_FUNC_TK_CRE_TSK */

#ifdef USE_FUNC_DEL_TSK
/*
 * Task deletion
 *	Call from critical section
 */
EXPORT void knl_del_tsk( TCB *tcb )
{
#if USE_IMALLOC
	if ( (tcb->tskatr & TA_USERBUF) == 0 ) {
		/* User buffer is not used */
		/* Free system stack */
		void *stack = (VB*)tcb->isstack - tcb->sstksz;
		knl_Ifree(stack);
	}
#endif

	/* Return control block to FreeQue */
	QueInsert(&tcb->tskque, &knl_free_tcb);
	tcb->state = TS_NONEXIST;
}
#endif /* USE_FUNC_DEL_TSK */

#ifdef USE_FUNC_TK_DEL_TSK
/*
 * Delete task 
 */
SYSCALL ER tk_del_tsk_impl( ID tskid )
{
	TCB	*tcb;
	TSTAT	state;
	ER	ercd = E_OK;

	CHECK_TSKID(tskid);
	CHECK_NONSELF(tskid);

	tcb = get_tcb(tskid);

	BEGIN_CRITICAL_SECTION;
	state = (TSTAT)tcb->state;
	if ( state != TS_DORMANT ) {
		ercd = ( state == TS_NONEXIST )? E_NOEXS: E_OBJ;
	} else {
		knl_del_tsk(tcb);
	}
	END_CRITICAL_SECTION;

	return ercd;
}
#endif /* USE_FUNC_TK_DEL_TSK */

/* ------------------------------------------------------------------------ */

#ifdef USE_FUNC_TK_STA_TSK
/*
 * Start task
 */
SYSCALL ER tk_sta_tsk_impl( ID tskid, INT stacd )
{
	TCB	*tcb;
	TSTAT	state;
	ER	ercd = E_OK;

	CHECK_TSKID(tskid);
	CHECK_NONSELF(tskid);

	tcb = get_tcb(tskid);

	BEGIN_CRITICAL_SECTION;
	state = (TSTAT)tcb->state;
	if ( state != TS_DORMANT ) {
		ercd = ( state == TS_NONEXIST )? E_NOEXS: E_OBJ;
	} else {
		knl_setup_stacd(tcb, stacd);
		knl_make_ready(tcb);
	}
	END_CRITICAL_SECTION;

	return ercd;
}
#endif /* USE_FUNC_TK_STA_TSK */

#ifdef USE_FUNC_TER_TSK
/*
 * Task finalization
 *	Call from critical section
 */
EXPORT void knl_ter_tsk( TCB *tcb )
{
	TSTAT	state;

	state = (TSTAT)tcb->state;
	if ( state == TS_READY ) {
		knl_make_non_ready(tcb);

	} else if ( (state & TS_WAIT) != 0 ) {
		knl_wait_cancel(tcb);
		if ( tcb->wspec->rel_wai_hook != NULL ) {
			(*tcb->wspec->rel_wai_hook)(tcb);
		}
	}

#if CFN_MAX_MTXID > 0
	/* signal mutex */
	knl_signal_all_mutex(tcb);
#endif

	knl_cleanup_context(tcb);
}
#endif /* USE_FUNC_TER_TSK */

#ifdef USE_FUNC_TK_EXT_TSK
/*
 * End its own task
 */
SYSCALL void tk_ext_tsk_impl( void )
{
#ifdef DORMANT_STACK_SIZE
	/* To avoid destroying stack used in 'knl_make_dormant', 
	   allocate the dummy area on the stack. */
	volatile VB _dummy[DORMANT_STACK_SIZE];
#endif

	/* Check context error */
#if CHK_CTX2
	if ( in_indp() ) {
#if USE_KERNEL_MESSAGE
		tm_putstring((UB*)"tk_ext_tsk was called in the task independent\n");
#endif
		tm_monitor(); /* To monitor */
	}
#endif
#if CHK_CTX1
	if ( in_ddsp() ) {
#if USE_KERNEL_MESSAGE
		tm_putstring((UB*)"tk_ext_tsk was called in the dispatch disabled\n");
#endif
	}
#endif

	DISABLE_INTERRUPT;
	knl_ter_tsk(knl_ctxtsk);
	knl_make_dormant(knl_ctxtsk);

	knl_force_dispatch();
	/* No return */

#ifdef DORMANT_STACK_SIZE
	/* for WARNING */
	_dummy[0] = 0;
#endif
}
#endif /* USE_FUNC_TK_EXT_TSK */

#ifdef USE_FUNC_TK_EXD_TSK
/*
 * End and delete its own task
 */
SYSCALL void tk_exd_tsk_impl( void )
{
	/* Check context error */
#if CHK_CTX2
	if ( in_indp() ) {
#if USE_KERNEL_MESSAGE
		tm_putstring((UB*)"tk_exd_tsk was called in the task independent\n");
#endif
		tm_monitor(); /* To monitor */
	}
#endif
#if CHK_CTX1
	if ( in_ddsp() ) {
#if USE_KERNEL_MESSAGE
		tm_putstring((UB*)"tk_exd_tsk was called in the dispatch disabled\n");
#endif
	}
#endif

	DISABLE_INTERRUPT;
	knl_ter_tsk(knl_ctxtsk);
	knl_del_tsk(knl_ctxtsk);

	knl_force_dispatch();
	/* No return */
}
#endif /* USE_FUNC_TK_EXD_TSK */

#ifdef USE_FUNC_TK_TER_TSK
/*
 * Termination of other task
 */
SYSCALL ER tk_ter_tsk_impl( ID tskid )
{
	TCB	*tcb;
	TSTAT	state;
	ER	ercd = E_OK;

	CHECK_TSKID(tskid);
	CHECK_NONSELF(tskid);

	tcb = get_tcb(tskid);

	BEGIN_CRITICAL_SECTION;
	state = (TSTAT)tcb->state;
	if ( !knl_task_alive(state) ) {
		ercd = ( state == TS_NONEXIST )? E_NOEXS: E_OBJ;
	} else if ( tcb->klocked ) {
		/* Normally, it does not become this state.
		 * When the state is page-in wait in the virtual memory
		 * system and when trying to terminate any task,
		 * it becomes this state.
		 */
		ercd = E_OBJ;
	} else {
		knl_ter_tsk(tcb);
		knl_make_dormant(tcb);
	}
	END_CRITICAL_SECTION;

	return ercd;
}
#endif /* USE_FUNC_TK_TER_TSK */

/* ------------------------------------------------------------------------ */

#ifdef USE_FUNC_TK_CHG_PRI
/*
 * Change task priority
 */
SYSCALL ER tk_chg_pri_impl( ID tskid, PRI tskpri )
{
	TCB	*tcb;
	INT	priority;
	ER	ercd;

	CHECK_TSKID_SELF(tskid);
	CHECK_PRI_INI(tskpri);

	tcb = get_tcb_self(tskid);

	BEGIN_CRITICAL_SECTION;
	if ( tcb->state == TS_NONEXIST ) {
		ercd = E_NOEXS;
		goto error_exit;
	}

	/* Conversion priority to internal expression */
	if ( tskpri == TPRI_INI ) {
		priority = tcb->ipriority;
	} else {
		priority = int_priority(tskpri);
	}

#if CFN_MAX_MTXID > 0
	/* Mutex priority change limit */
	ercd = knl_chg_pri_mutex(tcb, priority);
	if ( ercd < E_OK ) {
		goto error_exit;
	}

	tcb->bpriority = (UB)priority;
	priority = ercd;
#else
	tcb->bpriority = priority;
#endif

	/* Change priority */
	knl_change_task_priority(tcb, priority);

	ercd = E_OK;
    error_exit:
	END_CRITICAL_SECTION;

	return ercd;
}
#endif /* USE_FUNC_TK_CHG_PRI */

#ifdef USE_FUNC_TK_ROT_RDQ
/*
 * Rotate ready queue
 */
SYSCALL ER tk_rot_rdq_impl( PRI tskpri )
{
	CHECK_PRI_RUN(tskpri);

	BEGIN_CRITICAL_SECTION;
	if ( tskpri == TPRI_RUN ) {
		if ( in_indp() ) {
			knl_rotate_ready_queue_run();
		} else {
			knl_rotate_ready_queue(knl_ctxtsk->priority);
		}
	} else {
		knl_rotate_ready_queue(int_priority(tskpri));
	}
	END_CRITICAL_SECTION;

	return E_OK;
}
#endif /* USE_FUNC_TK_ROT_RDQ */

/* ------------------------------------------------------------------------ */

#ifdef USE_FUNC_TK_GET_TID
/*
 * Refer task ID at execution
 */
SYSCALL ID tk_get_tid_impl( void )
{
	return ( knl_ctxtsk == NULL )? 0: knl_ctxtsk->tskid;
}
#endif /* USE_FUNC_TK_GET_TID */

#ifdef USE_FUNC_TK_REF_TSK
/*
 * Refer task state
 */
SYSCALL ER tk_ref_tsk_impl( ID tskid, T_RTSK *pk_rtsk )
{
	TCB	*tcb;
	TSTAT	state;
	ER	ercd = E_OK;

	CHECK_TSKID_SELF(tskid);

	tcb = get_tcb_self(tskid);

	memset(pk_rtsk, 0, sizeof(*pk_rtsk));

	BEGIN_CRITICAL_SECTION;
	state = (TSTAT)tcb->state;
	if ( state == TS_NONEXIST ) {
		ercd = E_NOEXS;
	} else {
		if ( ( state == TS_READY ) && ( tcb == knl_ctxtsk ) ) {
			pk_rtsk->tskstat = TTS_RUN;
		} else {
			pk_rtsk->tskstat = (UINT)state << 1;
		}
		if ( (state & TS_WAIT) != 0 ) {
			pk_rtsk->tskwait = tcb->wspec->tskwait;
			pk_rtsk->wid     = tcb->wid;
		}
		pk_rtsk->exinf     = tcb->exinf;
		pk_rtsk->tskpri    = ext_tskpri(tcb->priority);
		pk_rtsk->tskbpri   = ext_tskpri(tcb->bpriority);
		pk_rtsk->wupcnt    = tcb->wupcnt;
		pk_rtsk->suscnt    = tcb->suscnt;
	}
	END_CRITICAL_SECTION;

	return ercd;
}
#endif /* USE_FUNC_TK_REF_TSK */

/* ------------------------------------------------------------------------ */


#ifdef USE_FUNC_TK_REL_WAI
/*
 * Release wait
 */
SYSCALL ER tk_rel_wai_impl( ID tskid )
{
	TCB	*tcb;
	TSTAT	state;
	ER	ercd = E_OK;

	CHECK_TSKID(tskid);

	tcb = get_tcb(tskid);

	BEGIN_CRITICAL_SECTION;
	state = (TSTAT)tcb->state;
	if ( (state & TS_WAIT) == 0 ) {
		ercd = ( state == TS_NONEXIST )? E_NOEXS: E_OBJ;
	} else {
		knl_wait_release_ng(tcb, E_RLWAI);
	}
	END_CRITICAL_SECTION;

	return ercd;
}
#endif /* USE_FUNC_TK_REL_WAI */

/* ------------------------------------------------------------------------ */
/*
 *	Debug support function
 */
#if USE_DBGSPT

#ifdef USE_FUNC_TASK_GETNAME
#if USE_OBJECT_NAME
/*
 * Get object name from control block
 */
EXPORT ER knl_task_getname(ID id, UB **name)
{
	TCB	*tcb;
	ER	ercd = E_OK;

	CHECK_TSKID_SELF(id);

	BEGIN_DISABLE_INTERRUPT;
	tcb = get_tcb_self(id);
	if ( tcb->state == TS_NONEXIST ) {
		ercd = E_NOEXS;
		goto error_exit;
	}
	if ( (tcb->tskatr & TA_DSNAME) == 0 ) {
		ercd = E_OBJ;
		goto error_exit;
	}
	*name = tcb->name;

    error_exit:
	END_DISABLE_INTERRUPT;

	return ercd;
}
#endif /* USE_OBJECT_NAME */
#endif /* USE_FUNC_TASK_GETNAME */

#ifdef USE_FUNC_TD_LST_TSK
/*
 * Refer task usage state
 */
SYSCALL INT td_lst_tsk_impl( ID list[], INT nent )
{
	TCB	*tcb, *end;
	INT	n = 0;

	BEGIN_DISABLE_INTERRUPT;
	end = knl_tcb_table + NUM_TSKID;
	for ( tcb = knl_tcb_table; tcb < end; tcb++ ) {
		if ( tcb->state == TS_NONEXIST ) {
			continue;
		}

		if ( n++ < nent ) {
			*list++ = tcb->tskid;
		}
	}
	END_DISABLE_INTERRUPT;

	return n;
}
#endif /* USE_FUNC_TD_LST_TSK */

#ifdef USE_FUNC_TD_REF_TSK
/*
 * Refer task state
 */
SYSCALL ER td_ref_tsk_impl( ID tskid, TD_RTSK *pk_rtsk )
{
	TCB	*tcb;
	TSTAT	state;
	ER	ercd = E_OK;

	CHECK_TSKID_SELF(tskid);

	tcb = get_tcb_self(tskid);

	memset(pk_rtsk, 0, sizeof(*pk_rtsk));

	BEGIN_DISABLE_INTERRUPT;
	state = (TSTAT)tcb->state;
	if ( state == TS_NONEXIST ) {
		ercd = E_NOEXS;
	} else {
		if ( ( state == TS_READY ) && ( tcb == knl_ctxtsk ) ) {
			pk_rtsk->tskstat = TTS_RUN;
		} else {
			pk_rtsk->tskstat = (UINT)state << 1;
		}
		if ( (state & TS_WAIT) != 0 ) {
			pk_rtsk->tskwait = tcb->wspec->tskwait;
			pk_rtsk->wid     = tcb->wid;
		}
		pk_rtsk->exinf     = tcb->exinf;
		pk_rtsk->tskpri    = ext_tskpri(tcb->priority);
		pk_rtsk->tskbpri   = ext_tskpri(tcb->bpriority);
		pk_rtsk->wupcnt    = tcb->wupcnt;
		pk_rtsk->suscnt    = tcb->suscnt;

		pk_rtsk->task      = tcb->task;
		pk_rtsk->stksz     = tcb->sstksz;
		pk_rtsk->istack    = tcb->isstack;
	}
	END_DISABLE_INTERRUPT;

	return ercd;
}
#endif /* USE_FUNC_TD_REF_TSK */

#ifdef USE_FUNC_TD_INF_TSK
/*
 * Get task statistic information
 */
SYSCALL ER td_inf_tsk_impl( ID tskid, TD_ITSK *pk_itsk, BOOL clr )
{
	TCB	*tcb;
	ER	ercd = E_OK;

	CHECK_TSKID_SELF(tskid);

	tcb = get_tcb_self(tskid);

	BEGIN_DISABLE_INTERRUPT;
	if ( tcb->state == TS_NONEXIST ) {
		ercd = E_NOEXS;
	} else {
		pk_itsk->stime = tcb->stime;
		pk_itsk->utime = tcb->utime;
		if ( clr ) {
			tcb->stime = 0;
			tcb->utime = 0;
		}
	}
	END_DISABLE_INTERRUPT;

	return ercd;
}
#endif /* USE_FUNC_TD_INF_TSK */

#endif /* USE_DBGSPT */
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
 *	task_sync.c
 *	Task with Synchronize Function
 */

/** [BEGIN Common Definitions] */
#include "kernel.h"
#include "task.h"
#include "wait.h"
#include "check.h"
#include "limits.h"
#include "task_sync.h"
/** [END Common Definitions] */

#ifdef USE_FUNC_TK_SUS_TSK
/*
 * Suspend task
 */
SYSCALL ER tk_sus_tsk_impl( ID tskid )
{
	TCB	*tcb;
	TSTAT	state;
	ER	ercd = E_OK;

	CHECK_TSKID(tskid);
	CHECK_NONSELF(tskid);

	tcb = get_tcb(tskid);

	BEGIN_CRITICAL_SECTION;
	state = (TSTAT)tcb->state;
	if ( !knl_task_alive(state) ) {
		ercd = ( state == TS_NONEXIST )? E_NOEXS: E_OBJ;
		goto error_exit;
	}
	if ( tcb == knl_ctxtsk && knl_dispatch_disabled >= DDS_DISABLE ) {
		ercd = E_CTX;
		goto error_exit;
	}
	if ( tcb->suscnt == INT_MAX ) {
		ercd = E_QOVR;
		goto error_exit;
	}

	/* Update suspend request count */
	++tcb->suscnt;

	/* Move to forced wait state */
	if ( state == TS_READY ) {
		knl_make_non_ready(tcb);
		tcb->state = TS_SUSPEND;

	} else if ( state == TS_WAIT ) {
		tcb->state = TS_WAITSUS;
	}

    error_exit:
	END_CRITICAL_SECTION;

	return ercd;
}
#endif /* USE_FUNC_TK_SUS_TSK */

#ifdef USE_FUNC_TK_RSM_TSK
/*
 * Resume task
 */
SYSCALL ER tk_rsm_tsk_impl( ID tskid )
{
	TCB	*tcb;
	ER	ercd = E_OK;

	CHECK_TSKID(tskid);
	CHECK_NONSELF(tskid);

	tcb = get_tcb(tskid);

	BEGIN_CRITICAL_SECTION;
	switch ( tcb->state ) {
	  case TS_NONEXIST:
		ercd = E_NOEXS;
		break;

	  case TS_DORMANT:
	  case TS_READY:
	  case TS_WAIT:
		ercd = E_OBJ;
		break;

	  case TS_SUSPEND:
		if ( --tcb->suscnt == 0 ) {
			knl_make_ready(tcb);
		}
		break;
	  case TS_WAITSUS:
		if ( --tcb->suscnt == 0 ) {
			tcb->state = TS_WAIT;
		}
		break;

	  default:
		ercd = E_SYS;
		break;
	}
	END_CRITICAL_SECTION;

	return ercd;
}
#endif /* USE_FUNC_TK_RSM_TSK */

#ifdef USE_FUNC_TK_FRSM_TSK
/*
 * Force resume task
 */
SYSCALL ER tk_frsm_tsk_impl( ID tskid )
{
	TCB	*tcb;
	ER	ercd = E_OK;

	CHECK_TSKID(tskid);
	CHECK_NONSELF(tskid);

	tcb = get_tcb(tskid);

	BEGIN_CRITICAL_SECTION;
	switch ( tcb->state ) {
	  case TS_NONEXIST:
		ercd = E_NOEXS;
		break;

	  case TS_DORMANT:
	  case TS_READY:
	  case TS_WAIT:
		ercd = E_OBJ;
		break;

	  case TS_SUSPEND:
		tcb->suscnt = 0;
		knl_make_ready(tcb);
		break;
	  case TS_WAITSUS:
		tcb->suscnt = 0;
		tcb->state = TS_WAIT;
		break;

	  default:
		ercd = E_SYS;
		break;
	}
	END_CRITICAL_SECTION;

	return ercd;
}
#endif /* USE_FUNC_TK_FRSM_TSK */

/* ------------------------------------------------------------------------ */

#ifdef USE_FUNC_WSPEC_SLP
/*
 * Definition of task wait specification
 */
EXPORT CONST WSPEC knl_wspec_slp = { TTW_SLP, NULL, NULL };
#endif /* USE_FUNC_WSPEC_SLP */

#ifdef USE_FUNC_TK_SLP_TSK
/*
 * Move its own task state to wait state
 */
SYSCALL ER tk_slp_tsk_impl( TMO tmout )
{
	ER	ercd = E_OK;

	CHECK_TMOUT(tmout);
	CHECK_DISPATCH();

	BEGIN_CRITICAL_SECTION;

	if ( knl_ctxtsk->wupcnt > 0 ) {
		knl_ctxtsk->wupcnt--;
	} else {
		ercd = E_TMOUT;
		if ( tmout != TMO_POL ) {
			knl_ctxtsk->wspec = &knl_wspec_slp;
			knl_ctxtsk->wid = 0;
			knl_ctxtsk->wercd = &ercd;
			knl_make_wait(tmout, TA_NULL);
			QueInit(&knl_ctxtsk->tskque);
		}
	}

	END_CRITICAL_SECTION;

	return ercd;
}
#endif /* USE_FUNC_TK_SLP_TSK */

#ifdef USE_FUNC_TK_WUP_TSK
/*
 * Wakeup task
 */
SYSCALL ER tk_wup_tsk_impl( ID tskid )
{
	TCB	*tcb;
	TSTAT	state;
	ER	ercd = E_OK;

	CHECK_TSKID(tskid);
	CHECK_NONSELF(tskid);

	tcb = get_tcb(tskid);

	BEGIN_CRITICAL_SECTION;
	state = (TSTAT)tcb->state;
	if ( !knl_task_alive(state) ) {
		ercd = ( state == TS_NONEXIST )? E_NOEXS: E_OBJ;

	} else if ( (state & TS_WAIT) != 0 && tcb->wspec == &knl_wspec_slp ) {
		knl_wait_release_ok(tcb);

	} else if ( tcb->wupcnt == INT_MAX ) {
		ercd = E_QOVR;
	} else {
		++tcb->wupcnt;
	}
	END_CRITICAL_SECTION;

	return ercd;
}
#endif /* USE_FUNC_TK_WUP_TSK */

#ifdef USE_FUNC_TK_CAN_WUP
/*
 * Cancel wakeup request
 */
SYSCALL INT tk_can_wup_impl( ID tskid )
{
	TCB	*tcb;
	ER	ercd = E_OK;

	CHECK_TSKID_SELF(tskid);

	tcb = get_tcb_self(tskid);

	BEGIN_CRITICAL_SECTION;
	switch ( tcb->state ) {
	  case TS_NONEXIST:
		ercd = E_NOEXS;
		break;
	  case TS_DORMANT:
		ercd = E_OBJ;
		break;

	  default:
		ercd = tcb->wupcnt;
		tcb->wupcnt = 0;
	}
	END_CRITICAL_SECTION;

	return ercd;
}
#endif /* USE_FUNC_TK_CAN_WUP */
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
 *	task_sync.h
 *	Task with Synchronize Function
 */

#ifndef _TASK_SYNC_H_
#define _TASK_SYNC_H_

/*
 * Definition of task wait specification
 */
IMPORT CONST WSPEC knl_wspec_slp;

#endif /* _TASK_SYNC_H_ */
/*
 *	micro T-Kernel function code (Debugger Support)
 */

#ifndef _TDFNCD_H_
#define _TDFNCD_H_

#if INT_BITWIDTH == 16
#include "tdfncd16.h"
#else
#include "tdfncd32.h"
#endif

#endif /* _TDFNCD_H_ */
/*
 *	micro T-Kernel function code (Debugger Support)
 */

#define TDFN_LST_TSK	0x8012
#define TDFN_LST_SEM	0x8022
#define TDFN_LST_FLG	0x8032
#define TDFN_LST_MBX	0x8042
#define TDFN_LST_MTX	0x8052
#define TDFN_LST_MBF	0x8062
#define TDFN_LST_POR	0x8072
#define TDFN_LST_MPF	0x8082
#define TDFN_LST_MPL	0x8092
#define TDFN_LST_CYC	0x80a2
#define TDFN_LST_ALM	0x80b2
#define TDFN_LST_SSY	0x80c2
#define TDFN_REF_SEM	0x80d2
#define TDFN_REF_FLG	0x80e2
#define TDFN_REF_MBX	0x80f2
#define TDFN_REF_MTX	0x8102
#define TDFN_REF_MBF	0x8112
#define TDFN_REF_POR	0x8122
#define TDFN_REF_MPF	0x8132
#define TDFN_REF_MPL	0x8142
#define TDFN_REF_CYC	0x8152
#define TDFN_REF_ALM	0x8162
#define TDFN_REF_SSY	0x8172
#define TDFN_REF_TSK	0x8182
#define TDFN_INF_TSK	0x8193
#define TDFN_GET_REG	0x81a4
#define TDFN_SET_REG	0x81b4
#define TDFN_REF_SYS	0x81c1
#define TDFN_GET_TIM	0x81d2
#define TDFN_GET_OTM	0x81e2
#define TDFN_RDY_QUE	0x81f3
#define TDFN_SEM_QUE	0x8203
#define TDFN_FLG_QUE	0x8213
#define TDFN_MBX_QUE	0x8223
#define TDFN_MTX_QUE	0x8233
#define TDFN_SMBF_QUE	0x8243
#define TDFN_RMBF_QUE	0x8253
#define TDFN_CAL_QUE	0x8263
#define TDFN_ACP_QUE	0x8273
#define TDFN_MPF_QUE	0x8283
#define TDFN_MPL_QUE	0x8293
#define TDFN_HOK_SVC	0x82a1
#define TDFN_HOK_DSP	0x82b1
#define TDFN_HOK_INT	0x82c1
#define TDFN_REF_DSNAME	0x82d3
#define TDFN_SET_DSNAME	0x82e3

/*
 *	micro T-Kernel function code (Debugger Support)
 */

#define TDFN_LST_TSK	0x80010200
#define TDFN_LST_SEM	0x80020200
#define TDFN_LST_FLG	0x80030200
#define TDFN_LST_MBX	0x80040200
#define TDFN_LST_MTX	0x80050200
#define TDFN_LST_MBF	0x80060200
#define TDFN_LST_POR	0x80070200
#define TDFN_LST_MPF	0x80080200
#define TDFN_LST_MPL	0x80090200
#define TDFN_LST_CYC	0x800a0200
#define TDFN_LST_ALM	0x800b0200
#define TDFN_LST_SSY	0x800c0200
#define TDFN_REF_SEM	0x800d0200
#define TDFN_REF_FLG	0x800e0200
#define TDFN_REF_MBX	0x800f0200
#define TDFN_REF_MTX	0x80100200
#define TDFN_REF_MBF	0x80110200
#define TDFN_REF_POR	0x80120200
#define TDFN_REF_MPF	0x80130200
#define TDFN_REF_MPL	0x80140200
#define TDFN_REF_CYC	0x80150200
#define TDFN_REF_ALM	0x80160200
#define TDFN_REF_SSY	0x80170200
#define TDFN_REF_TSK	0x80180200
#define TDFN_INF_TSK	0x80190300
#define TDFN_GET_REG	0x801a0400
#define TDFN_SET_REG	0x801b0400
#define TDFN_REF_SYS	0x801c0100
#define TDFN_GET_TIM	0x801d0200
#define TDFN_GET_OTM	0x801e0200
#define TDFN_RDY_QUE	0x801f0300
#define TDFN_SEM_QUE	0x80200300
#define TDFN_FLG_QUE	0x80210300
#define TDFN_MBX_QUE	0x80220300
#define TDFN_MTX_QUE	0x80230300
#define TDFN_SMBF_QUE	0x80240300
#define TDFN_RMBF_QUE	0x80250300
#define TDFN_CAL_QUE	0x80260300
#define TDFN_ACP_QUE	0x80270300
#define TDFN_MPF_QUE	0x80280300
#define TDFN_MPL_QUE	0x80290300
#define TDFN_HOK_SVC	0x802a0100
#define TDFN_HOK_DSP	0x802b0100
#define TDFN_HOK_INT	0x802c0100
#define TDFN_REF_DSNAME	0x802d0300
#define TDFN_SET_DSNAME	0x802e0300

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
 *	tdsvcproxy.h
 *	micro T-Kernel system call proxy for direct call
 */

#define td_lst_tsk_impl	td_lst_tsk
#define td_lst_sem_impl	td_lst_sem
#define td_lst_flg_impl	td_lst_flg
#define td_lst_mbx_impl	td_lst_mbx
#define td_lst_mtx_impl	td_lst_mtx
#define td_lst_mbf_impl	td_lst_mbf
#define td_lst_por_impl	td_lst_por
#define td_lst_mpf_impl	td_lst_mpf
#define td_lst_mpl_impl	td_lst_mpl
#define td_lst_cyc_impl	td_lst_cyc
#define td_lst_alm_impl	td_lst_alm
#define td_lst_ssy_impl	td_lst_ssy
#define td_ref_sem_impl	td_ref_sem
#define td_ref_flg_impl	td_ref_flg
#define td_ref_mbx_impl	td_ref_mbx
#define td_ref_mtx_impl	td_ref_mtx
#define td_ref_mbf_impl	td_ref_mbf
#define td_ref_por_impl	td_ref_por
#define td_ref_mpf_impl	td_ref_mpf
#define td_ref_mpl_impl	td_ref_mpl
#define td_ref_cyc_impl	td_ref_cyc
#define td_ref_alm_impl	td_ref_alm
#define td_ref_ssy_impl	td_ref_ssy
#define td_ref_tsk_impl	td_ref_tsk
#define td_inf_tsk_impl	td_inf_tsk
#define td_get_reg_impl	td_get_reg
#define td_set_reg_impl	td_set_reg
#define td_ref_sys_impl	td_ref_sys
#define td_get_tim_impl	td_get_tim
#define td_get_otm_impl	td_get_otm
#define td_rdy_que_impl	td_rdy_que
#define td_sem_que_impl	td_sem_que
#define td_flg_que_impl	td_flg_que
#define td_mbx_que_impl	td_mbx_que
#define td_mtx_que_impl	td_mtx_que
#define td_smbf_que_impl	td_smbf_que
#define td_rmbf_que_impl	td_rmbf_que
#define td_cal_que_impl	td_cal_que
#define td_acp_que_impl	td_acp_que
#define td_mpf_que_impl	td_mpf_que
#define td_mpl_que_impl	td_mpl_que
#define td_hok_svc_impl	td_hok_svc
#define td_hok_dsp_impl	td_hok_dsp
#define td_hok_int_impl	td_hok_int
#define td_ref_dsname_impl	td_ref_dsname
#define td_set_dsname_impl	td_set_dsname
/*
 *	micro T-Kernel system call branch table (Debugger Support)
 *
 *	   created from /usr/local/te/include/tk/dbgspt.h
 */

#include <machine.h>

#define _TDSC_ENTRY(name)	.long	_name##_impl

#define N_TDFN	46

	_TDSC_ENTRY(td_lst_tsk)
	_TDSC_ENTRY(td_lst_sem)
	_TDSC_ENTRY(td_lst_flg)
	_TDSC_ENTRY(td_lst_mbx)
	_TDSC_ENTRY(td_lst_mtx)
	_TDSC_ENTRY(td_lst_mbf)
	_TDSC_ENTRY(td_lst_por)
	_TDSC_ENTRY(td_lst_mpf)
	_TDSC_ENTRY(td_lst_mpl)
	_TDSC_ENTRY(td_lst_cyc)
	_TDSC_ENTRY(td_lst_alm)
	_TDSC_ENTRY(td_lst_ssy)
	_TDSC_ENTRY(td_ref_sem)
	_TDSC_ENTRY(td_ref_flg)
	_TDSC_ENTRY(td_ref_mbx)
	_TDSC_ENTRY(td_ref_mtx)
	_TDSC_ENTRY(td_ref_mbf)
	_TDSC_ENTRY(td_ref_por)
	_TDSC_ENTRY(td_ref_mpf)
	_TDSC_ENTRY(td_ref_mpl)
	_TDSC_ENTRY(td_ref_cyc)
	_TDSC_ENTRY(td_ref_alm)
	_TDSC_ENTRY(td_ref_ssy)
	_TDSC_ENTRY(td_ref_tsk)
	_TDSC_ENTRY(td_inf_tsk)
	_TDSC_ENTRY(td_get_reg)
	_TDSC_ENTRY(td_set_reg)
	_TDSC_ENTRY(td_ref_sys)
	_TDSC_ENTRY(td_get_tim)
	_TDSC_ENTRY(td_get_otm)
	_TDSC_ENTRY(td_rdy_que)
	_TDSC_ENTRY(td_sem_que)
	_TDSC_ENTRY(td_flg_que)
	_TDSC_ENTRY(td_mbx_que)
	_TDSC_ENTRY(td_mtx_que)
	_TDSC_ENTRY(td_smbf_que)
	_TDSC_ENTRY(td_rmbf_que)
	_TDSC_ENTRY(td_cal_que)
	_TDSC_ENTRY(td_acp_que)
	_TDSC_ENTRY(td_mpf_que)
	_TDSC_ENTRY(td_mpl_que)
	_TDSC_ENTRY(td_hok_svc)
	_TDSC_ENTRY(td_hok_dsp)
	_TDSC_ENTRY(td_hok_int)
	_TDSC_ENTRY(td_ref_dsname)
	_TDSC_ENTRY(td_set_dsname)
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
 *	timer.c
 *	Timer Control
 */

#include "kernel.h"
#include "timer.h"
#include "task.h"
#include "tkdev_timer.h"

/*
 * Current time (Software clock)
 *	'current_time' shows the total operation time since
 *	operating system Starts. 'real_time_ofs' shows difference
 *	between the current time and the operating system clock
 *	(current_time). Do not change 'current_time' when setting
 *	time by 'set_tim()'. Set 'real_time_ofs' with the time  	 
 *   	difference between 'current_time' and setup time.
 *	Therefore 'current_time' does not affect with time change
 *	and it increases simply.
 */
Noinit(EXPORT LSYSTIM	knl_current_time);	/* System operation time */
Noinit(EXPORT LSYSTIM	knl_real_time_ofs);	/* Actual time - System operation time */

/* 
 * Timer event queue
 */
Noinit(EXPORT QUEUE	knl_timer_queue);

/*
 * Initialization of timer module
 */
EXPORT ER knl_timer_initialize( void )
{
	if ( CFN_TIMER_PERIOD < MIN_TIMER_PERIOD
	  || CFN_TIMER_PERIOD > MAX_TIMER_PERIOD ) {
		return E_SYS;
	}

	knl_current_time = knl_real_time_ofs = ltoll(0);
	QueInit(&knl_timer_queue);

	/* Start timer interrupt */
	knl_start_hw_timer();

	return E_OK;
}

#if USE_CLEANUP
/*
 * Stop timer
 */
EXPORT void knl_timer_shutdown( void )
{
	knl_terminate_hw_timer();
}
#endif /* USE_CLEANUP */


/*
 * Insert timer event to timer event queue
 */
LOCAL void knl_enqueue_tmeb( TMEB *event )
{
	QUEUE	*q;

	for ( q = knl_timer_queue.next; q != &knl_timer_queue; q = q->next ) {
		if ( ll_cmp(event->time, ((TMEB*)q)->time) < 0) {
			break;
		}
	}
	QueInsert(&event->queue, q);
}

/*
 * Set timeout event
 *	Register the timer event 'event' onto the timer queue to
 *	start after the timeout 'tmout'. At timeout, start with the
 *	argument 'arg' on the callback function 'callback'.
 *	When 'tmout' is TMO_FEVR, do not register onto the timer
 *	queue, but initialize queue area in case 'timer_delete' 
 *	is called later.
 *
 *	"include/tk/tk_typedef.h"
 *	typedef	W		TMO;
 *	typedef UW		RELTIM;
 *	#define TMO_FEVR	(-1)
 */
EXPORT void knl_timer_insert( TMEB *event, TMO tmout, CBACK callback, void *arg )
{
	event->callback = callback;
	event->arg = arg;

	if ( tmout == TMO_FEVR ) {
		QueInit(&event->queue);
	} else {
		/* To guarantee longer wait time specified by 'tmout',
		   add TIMER_PERIOD on wait time */
		event->time = ll_add( ll_add(knl_current_time, ltoll(tmout)),
					uitoll(CFN_TIMER_PERIOD) );
		knl_enqueue_tmeb(event);
	}
}

EXPORT void knl_timer_insert_reltim( TMEB *event, RELTIM tmout, CBACK callback, void *arg )
{
	event->callback = callback;
	event->arg = arg;

	/* To guarantee longer wait time specified by 'tmout',
	   add TIMER_PERIOD on wait time */
	event->time = ll_add( ll_add(knl_current_time, ultoll(tmout)),
				uitoll(CFN_TIMER_PERIOD) );
	knl_enqueue_tmeb(event);
}

/*
 * Set time specified event
 *	Register the timer event 'evt' onto the timer queue to start at the 
 *	(absolute) time 'time'.
 *	'time' is not an actual time. It is system operation time.
 */
EXPORT void knl_timer_insert_abs( TMEB *evt, LSYSTIM time, CBACK cback, void *arg )
{
	evt->callback = cback;
	evt->arg = arg;
	evt->time = time;
	knl_enqueue_tmeb(evt);
}

/* ------------------------------------------------------------------------ */

/*
 * Timer interrupt handler
 *	Timer interrupt handler starts every TIMER_PERIOD millisecond 
 *	interval by hardware timer. Update the software clock and start the 
 *	timer event upon arriving at start time.
 */

EXPORT void knl_timer_handler( void )
{
	TMEB	*event;

	knl_clear_hw_timer_interrupt();		/* Clear timer interrupt */

	BEGIN_CRITICAL_SECTION;
	knl_current_time = ll_add(knl_current_time, uitoll(CFN_TIMER_PERIOD));

#if USE_DBGSPT && defined(USE_FUNC_TD_INF_TSK)
	if ( knl_ctxtsk != NULL ) {
		/* Task at execution */
		if ( knl_ctxtsk->sysmode > 0 ) {
			knl_ctxtsk->stime += CFN_TIMER_PERIOD;
		} else {
			knl_ctxtsk->utime += CFN_TIMER_PERIOD;
		}
	}
#endif

	/* Execute event that passed occurring time. */
	while ( !isQueEmpty(&knl_timer_queue) ) {
		event = (TMEB*)knl_timer_queue.next;

		if ( ll_cmp(event->time, knl_current_time) > 0 ) {
			break;
		}

		QueRemove(&event->queue);
		if ( event->callback != NULL ) {
			(*event->callback)(event->arg);
		}
	}

	END_CRITICAL_SECTION;

	knl_end_of_hw_timer_interrupt();		/* Clear timer interrupt */
}
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
 *	timer.h
 *	Timer Module Definition
 */

#ifndef _TIMER_
#define _TIMER_

#include <longlong.h>

/*
 * SYSTIM internal expression and conversion
 */
typedef	longlong	LSYSTIM;	/* SYSTIM int. expression */

LSYSTIM knl_toLSYSTIM( CONST SYSTIM *time )
{
	LSYSTIM		ltime;

	hilo_ll(ltime, time->hi, time->lo);

	return ltime;
}

SYSTIM knl_toSYSTIM( LSYSTIM ltime )
{
	SYSTIM		time;

	ll_hilo(time.hi, time.lo, ltime);

	return time;
}

/*
 * Definition of timer event block 
 */
typedef void	(*CBACK)(void *);	/* Type of callback function */

typedef struct timer_event_block {
	QUEUE	queue;		/* Timer event queue */
	LSYSTIM	time;		/* Event time */
	CBACK	callback;	/* Callback function */
	void	*arg;		/* Argument to be sent to callback function */
} TMEB;

/*
 * Current time (Software clock)
 */
IMPORT LSYSTIM	knl_current_time;	/* System operation time */
IMPORT LSYSTIM	knl_real_time_ofs;	/* Difference from actual time */

/*
 * Timer event queue
 */
IMPORT QUEUE	knl_timer_queue;

/* Actual time */
#define real_time()	( ll_add(knl_current_time, knl_real_time_ofs) )

/*
 * Timer initialization and stop
 */
IMPORT ER   knl_timer_initialize( void );
IMPORT void knl_timer_shutdown( void );

/*
 * Register timer event onto timer queue
 */
IMPORT void knl_timer_insert( TMEB *evt, TMO tmout, CBACK cback, void *arg );
IMPORT void knl_timer_insert_reltim( TMEB *event, RELTIM tmout, CBACK callback, void *arg );
IMPORT void knl_timer_insert_abs( TMEB *evt, LSYSTIM time, CBACK cback, void *arg );

/*
 * Delete from timer queue
 */
void knl_timer_delete( TMEB *event )
{
	QueRemove(&event->queue);
}

#endif /* _TIMER_ */
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
 *	time_calls.c
 *	Time Management Function
 */

/** [BEGIN Common Definitions] */
#include "kernel.h"
#include "timer.h"
#include "task.h"
#include "wait.h"
#include "check.h"
#include "tkdev_timer.h"
#include "time_calls.h"
/** [END Common Definitions] */

#ifdef USE_FUNC_TK_SET_TIM
/*
 * Set system clock
 */
SYSCALL ER tk_set_tim_impl( CONST SYSTIM *pk_tim )
{
	CHECK_PAR(pk_tim->hi >= 0);

	BEGIN_CRITICAL_SECTION;
	knl_real_time_ofs = ll_sub(knl_toLSYSTIM(pk_tim), knl_current_time);
	END_CRITICAL_SECTION;

	return E_OK;
}
#endif /* USE_FUNC_TK_SET_TIM */

#ifdef USE_FUNC_TK_GET_TIM
/*
 * Refer system clock
 */
SYSCALL ER tk_get_tim_impl( SYSTIM *pk_tim )
{
	BEGIN_CRITICAL_SECTION;
	*pk_tim = knl_toSYSTIM(real_time());
	END_CRITICAL_SECTION;

	return E_OK;
}
#endif /* USE_FUNC_TK_GET_TIM */

#ifdef USE_FUNC_TK_GET_OTM
/*
 * Refer system operating time
 */
SYSCALL ER tk_get_otm_impl( SYSTIM *pk_tim )
{
	BEGIN_CRITICAL_SECTION;
	*pk_tim = knl_toSYSTIM(knl_current_time);
	END_CRITICAL_SECTION;

	return E_OK;
}
#endif /* USE_FUNC_TK_GET_OTM */

#if USE_DBGSPT
#ifdef USE_FUNC_TD_GET_TIM
/*
 * Refer system clock
 */
SYSCALL ER td_get_tim_impl( SYSTIM *tim, UW *ofs )
{
	BEGIN_DISABLE_INTERRUPT;
	*ofs = knl_get_hw_timer_nsec();
	*tim = knl_toSYSTIM(real_time());
	END_DISABLE_INTERRUPT;

	return E_OK;
}
#endif /* USE_FUNC_TD_GET_TIM */

#ifdef USE_FUNC_TD_GET_OTM
/*
 * Refer system operating time
 */
SYSCALL ER td_get_otm_impl( SYSTIM *tim, UW *ofs )
{
	BEGIN_DISABLE_INTERRUPT;
	*ofs = knl_get_hw_timer_nsec();
	*tim = knl_toSYSTIM(knl_current_time);
	END_DISABLE_INTERRUPT;

	return E_OK;
}
#endif /* USE_FUNC_TD_GET_OTM */
#endif /* USE_DBGSPT */

/* ------------------------------------------------------------------------ */

#ifdef USE_FUNC_TK_DLY_TSK
/*
 * Definition of task delay wait specification
 */
LOCAL CONST WSPEC knl_wspec_dly = { TTW_DLY, NULL, NULL };

/*
 * Task delay
 */
SYSCALL ER tk_dly_tsk_impl( RELTIM dlytim )
{
	ER	ercd = E_OK;

	CHECK_DISPATCH();

	if ( dlytim > 0 ) {
		BEGIN_CRITICAL_SECTION;
		knl_ctxtsk->wspec = &knl_wspec_dly;
		knl_ctxtsk->wid = 0;
		knl_ctxtsk->wercd = &ercd;
		knl_make_wait_reltim(dlytim, TA_NULL);
		QueInit(&knl_ctxtsk->tskque);
		END_CRITICAL_SECTION;
	}

	return ercd;
}
#endif /* USE_FUNC_TK_DLY_TSK */

/* ------------------------------------------------------------------------ */
/*
 *	Cyclic handler
 */

#if CFN_MAX_CYCID > 0

#ifdef USE_FUNC_CYCCB_TABLE
Noinit(EXPORT CYCCB knl_cyccb_table[NUM_CYCID]);	/* Cyclic handler control block */
Noinit(EXPORT QUEUE	knl_free_cyccb);	/* FreeQue */
#endif /* USE_FUNC_CYCCB_TABLE */


#ifdef USE_FUNC_CYCLICHANDLER_INITIALIZE
/*
 * Initialization of cyclic handler control block
 */
EXPORT ER knl_cyclichandler_initialize( void )
{
	CYCCB	*cyccb, *end;

	/* Get system information */
	if ( NUM_CYCID < 1 ) {
		return E_SYS;
	}

	/* Register all control blocks onto FreeQue */
	QueInit(&knl_free_cyccb);
	end = knl_cyccb_table + NUM_CYCID;
	for ( cyccb = knl_cyccb_table; cyccb < end; cyccb++ ) {
		cyccb->cychdr = NULL; /* Unregistered handler */
		QueInsert((QUEUE*)cyccb, &knl_free_cyccb);
	}

	return E_OK;
}
#endif /* USE_FUNC_CYCLICHANDLER_INITIALIZE */


#ifdef USE_FUNC_CALL_CYCHDR
/*
 * Cyclic handler routine
 */
EXPORT void knl_call_cychdr( CYCCB *cyccb )
{
	/* Set next startup time */
	knl_cyc_timer_insert(cyccb, knl_cyc_next_time(cyccb));

	/* Execute cyclic handler / Enable interrupt nest */
	ENABLE_INTERRUPT_UPTO(TIMER_INTLEVEL);
	CallUserHandlerP1(cyccb->exinf, cyccb->cychdr, cyccb);
	DISABLE_INTERRUPT;
}
#endif /* USE_FUNC_CALL_CYCHDR */

#ifdef USE_FUNC_TK_CRE_CYC
/*
 * Immediate call of cyclic handler 
 */
LOCAL void knl_immediate_call_cychdr( CYCCB *cyccb )
{
	/* Set next startup time */
	knl_cyc_timer_insert(cyccb, knl_cyc_next_time(cyccb));

	/* Execute cyclic handler in task-independent part
	   (Keep interrupt disabled) */
	ENTER_TASK_INDEPENDENT;
	CallUserHandlerP1(cyccb->exinf, cyccb->cychdr, cyccb);
	LEAVE_TASK_INDEPENDENT;
}

/*
 * Create cyclic handler 
 */
SYSCALL ID tk_cre_cyc_impl P1( CONST T_CCYC *pk_ccyc )
{
#if CHK_RSATR
	const ATR VALID_CYCATR = {
		 TA_HLNG
		|TA_STA
		|TA_PHS
		|TA_GP
#if USE_OBJECT_NAME
		|TA_DSNAME
#endif
	};
#endif
	CYCCB	*cyccb;
	LSYSTIM	tm;
	ER	ercd = E_OK;

	CHECK_RSATR(pk_ccyc->cycatr, VALID_CYCATR);
	CHECK_PAR(pk_ccyc->cychdr != NULL);
	CHECK_PAR(pk_ccyc->cyctim > 0);

	BEGIN_CRITICAL_SECTION;
	/* Get control block from FreeQue */
	cyccb = (CYCCB*)QueRemoveNext(&knl_free_cyccb);
	if ( cyccb == NULL ) {
		ercd = E_LIMIT;
		goto error_exit;
	}

	/* Initialize control block */
	cyccb->exinf   = pk_ccyc->exinf;
	cyccb->cycatr  = pk_ccyc->cycatr;
	cyccb->cychdr  = pk_ccyc->cychdr;
	cyccb->cyctim  = pk_ccyc->cyctim;
#if USE_OBJECT_NAME
	if ( (pk_ccyc->cycatr & TA_DSNAME) != 0 ) {
		strncpy((char*)cyccb->name, (char*)pk_ccyc->dsname, OBJECT_NAME_LENGTH);
	}
#endif
#if TA_GP
	if ( (pk_ccyc->cycatr & TA_GP) != 0 ) {
		gp = pk_ccyc->gp;
	}
	cyccb->gp = gp;
#endif

	/* First startup time
	 *	To guarantee the start of handler after the specified time,
	 *	add CFN_TIMER_PERIOD. 
	 */
	tm = ll_add(knl_current_time, uitoll(pk_ccyc->cycphs));
	tm = ll_add(tm, uitoll(CFN_TIMER_PERIOD));

	if ( (pk_ccyc->cycatr & TA_STA) != 0 ) {
		/* Start cyclic handler */
		cyccb->cycstat = TCYC_STA;

		if ( pk_ccyc->cycphs == 0 ) {
			/* Immediate execution */
			cyccb->cyctmeb.time = tm;
			knl_immediate_call_cychdr(cyccb);
		} else {
			/* Register onto timer event queue */
			knl_cyc_timer_insert(cyccb, tm);
		}
	} else {
		/* Initialize only counter */
		cyccb->cycstat = TCYC_STP;
		cyccb->cyctmeb.time = tm;
	}

	ercd = ID_CYC(cyccb - knl_cyccb_table);

    error_exit:
	END_CRITICAL_SECTION;

	return ercd;
}
#endif /* USE_FUNC_TK_CRE_CYC */

#ifdef USE_FUNC_TK_DEL_CYC
/*
 * Delete cyclic handler 
 */
SYSCALL ER tk_del_cyc_impl( ID cycid )
{
	CYCCB	*cyccb;
	ER	ercd = E_OK;

	CHECK_CYCID(cycid);

	cyccb = get_cyccb(cycid);

	BEGIN_CRITICAL_SECTION;
	if ( cyccb->cychdr == NULL ) { /* Unregistered handler */
		ercd = E_NOEXS;
	} else {
		if ( (cyccb->cycstat & TCYC_STA) != 0 ) {
			/* Delete timer event queue */
			knl_timer_delete(&cyccb->cyctmeb);
		}

		/* Return to FreeQue */
		QueInsert((QUEUE*)cyccb, &knl_free_cyccb);
		cyccb->cychdr = NULL; /* Unregistered handler */
	}
	END_CRITICAL_SECTION;

	return ercd;
}
#endif /* USE_FUNC_TK_DEL_CYC */

#ifdef USE_FUNC_TK_STA_CYC
/*
 * Start cyclic handler 
 */
SYSCALL ER tk_sta_cyc_impl( ID cycid )
{
	CYCCB	*cyccb;
	LSYSTIM	tm;
	ER	ercd = E_OK;

	CHECK_CYCID(cycid);

	cyccb = get_cyccb(cycid);

	BEGIN_CRITICAL_SECTION;
	if ( cyccb->cychdr == NULL ) { /* Unregistered handler */
		ercd = E_NOEXS;
		goto error_exit;
	}

	if ( (cyccb->cycatr & TA_PHS) != 0 ) {
		/* Continue cyclic phase */
		if ( (cyccb->cycstat & TCYC_STA) == 0 ) {
			/* Start cyclic handler */
			tm = cyccb->cyctmeb.time;
			if ( ll_cmp(tm, knl_current_time) <= 0 ) {
				tm = knl_cyc_next_time(cyccb);
			}
			knl_cyc_timer_insert(cyccb, tm);
		}
	} else {
		/* Reset cyclic interval */
		if ( (cyccb->cycstat & TCYC_STA) != 0 ) {
			/* Stop once */
			knl_timer_delete(&cyccb->cyctmeb);
		}

		/* FIRST ACTIVATION TIME
		 *	Adjust the first activation time with CFN_TIMER_PERIOD.
		 *	CFN_TIMER_PERIOD is Timer interrupt interval (millisecond).
		 */
		tm = ll_add(knl_current_time, uitoll(cyccb->cyctim));
		tm = ll_add(tm, uitoll(CFN_TIMER_PERIOD));

		/* Start cyclic handler */
		knl_cyc_timer_insert(cyccb, tm);
	}
	cyccb->cycstat |= TCYC_STA;

    error_exit:
	END_CRITICAL_SECTION;

	return ercd;
}
#endif /* USE_FUNC_TK_STA_CYC */

#ifdef USE_FUNC_TK_STP_CYC
/*
 * Stop cyclic handler 
 */
SYSCALL ER tk_stp_cyc_impl( ID cycid )
{
	CYCCB	*cyccb;
	ER	ercd = E_OK;

	CHECK_CYCID(cycid);

	cyccb = get_cyccb(cycid);

	BEGIN_CRITICAL_SECTION;
	if ( cyccb->cychdr == NULL ) { /* Unregistered handler */
		ercd = E_NOEXS;
	} else {
		if ( (cyccb->cycstat & TCYC_STA) != 0 ) {
			/* Stop cyclic handler */
			knl_timer_delete(&cyccb->cyctmeb);
		}
		cyccb->cycstat &= ~TCYC_STA;
	}
	END_CRITICAL_SECTION;

	return ercd;
}
#endif /* USE_FUNC_TK_STP_CYC */

#ifdef USE_FUNC_TK_REF_CYC
/*
 * Refer cyclic handler state
 */
SYSCALL ER tk_ref_cyc_impl( ID cycid, T_RCYC* pk_rcyc )
{
	CYCCB	*cyccb;
	LSYSTIM	tm;
	ER	ercd = E_OK;

	CHECK_CYCID(cycid);

	cyccb = get_cyccb(cycid);

	BEGIN_CRITICAL_SECTION;
	if ( cyccb->cychdr == NULL ) { /* Unregistered handler */
		ercd = E_NOEXS;
	} else {
		tm = cyccb->cyctmeb.time;
		if ( (cyccb->cycstat & TCYC_STA) == 0 ) {
			if ( ll_cmp(tm, knl_current_time) <= 0 ) {
				tm = knl_cyc_next_time(cyccb);
			}
		}
		tm = ll_sub(tm, knl_current_time);
		tm = ll_sub(tm, uitoll(CFN_TIMER_PERIOD));
		if ( ll_sign(tm) < 0 ) {
			tm = ltoll(0);
		}

		pk_rcyc->exinf   = cyccb->exinf;
		pk_rcyc->lfttim  = lltoul(tm);
		pk_rcyc->cycstat = cyccb->cycstat;
	}
	END_CRITICAL_SECTION;

	return ercd;
}
#endif /* USE_FUNC_TK_REF_CYC */

#if USE_DBGSPT

#ifdef USE_FUNC_CYCLICHANDLER_GETNAME
#if USE_OBJECT_NAME
/*
 * Get object name from control block
 */
EXPORT ER knl_cyclichandler_getname(ID id, UB **name)
{
	CYCCB	*cyccb;
	ER	ercd = E_OK;

	CHECK_CYCID(id);

	BEGIN_DISABLE_INTERRUPT;
	cyccb = get_cyccb(id);
	if ( cyccb->cychdr == NULL ) {
		ercd = E_NOEXS;
		goto error_exit;
	}
	if ( (cyccb->cycatr & TA_DSNAME) == 0 ) {
		ercd = E_OBJ;
		goto error_exit;
	}
	*name = cyccb->name;

    error_exit:
	END_DISABLE_INTERRUPT;

	return ercd;
}
#endif /* USE_OBJECT_NAME */
#endif /* USE_FUNC_CYCLICHANDLER_GETNAME */

#ifdef USE_FUNC_TD_LST_CYC
/*
 * Refer cyclic handler usage state
 */
SYSCALL INT td_lst_cyc_impl( ID list[], INT nent )
{
	CYCCB	*cyccb, *end;
	INT	n = 0;

	BEGIN_DISABLE_INTERRUPT;
	end = knl_cyccb_table + NUM_CYCID;
	for ( cyccb = knl_cyccb_table; cyccb < end; cyccb++ ) {
		/* Unregistered handler */
		if ( cyccb->cychdr == NULL ) {
			continue;
		}

		if ( n++ < nent ) {
			*list++ = ID_CYC(cyccb - knl_cyccb_table);
		}
	}
	END_DISABLE_INTERRUPT;

	return n;
}
#endif /* USE_FUNC_TD_LST_CYC */

#ifdef USE_FUNC_TD_REF_CYC
/*
 * Refer cyclic handler state
 */
SYSCALL ER td_ref_cyc_impl( ID cycid, TD_RCYC* pk_rcyc )
{
	CYCCB	*cyccb;
	LSYSTIM	tm;
	ER	ercd = E_OK;

	CHECK_CYCID(cycid);

	cyccb = get_cyccb(cycid);

	BEGIN_DISABLE_INTERRUPT;
	if ( cyccb->cychdr == NULL ) { /* Unregistered handler */
		ercd = E_NOEXS;
	} else {
		tm = cyccb->cyctmeb.time;
		if ( (cyccb->cycstat & TCYC_STA) == 0 ) {
			if ( ll_cmp(tm, knl_current_time) <= 0 ) {
				tm = knl_cyc_next_time(cyccb);
			}
		}
		tm = ll_sub(tm, knl_current_time);
		tm = ll_sub(tm, uitoll(CFN_TIMER_PERIOD));
		if ( ll_sign(tm) < 0 ) {
			tm = ltoll(0);
		}

		pk_rcyc->exinf   = cyccb->exinf;
		pk_rcyc->lfttim  = lltoul(tm);
		pk_rcyc->cycstat = cyccb->cycstat;
	}
	END_DISABLE_INTERRUPT;

	return ercd;
}
#endif /* USE_FUNC_TD_REF_CYC */

#endif /* USE_DBGSPT */
#endif /* CFN_MAX_CYCID */

/* ------------------------------------------------------------------------ */
/*
 *	Alarm handler
 */

#if CFN_MAX_ALMID > 0

#ifdef USE_FUNC_ALMCB_TABLE
Noinit(EXPORT ALMCB knl_almcb_table[NUM_ALMID]);	/* Alarm handler control block */
Noinit(EXPORT QUEUE	knl_free_almcb);	/* FreeQue */
#endif /* USE_FUNC_ALMCB_TABLE */


#ifdef USE_FUNC_ALARMHANDLER_INITIALIZE
/*
 * Initialization of alarm handler control block 
 */
EXPORT ER knl_alarmhandler_initialize( void )
{
	ALMCB	*almcb, *end;

	/* Get system information */
	if ( NUM_ALMID < 1 ) {
		return E_SYS;
	}

	/* Register all control blocks onto FreeQue */
	QueInit(&knl_free_almcb);
	end = knl_almcb_table + NUM_ALMID;
	for ( almcb = knl_almcb_table; almcb < end; almcb++ ) {
		almcb->almhdr = NULL; /* Unregistered handler */
		QueInsert((QUEUE*)almcb, &knl_free_almcb);
	}

	return E_OK;
}
#endif /* USE_FUNC_ALARMHANDLER_INITIALIZE */


#ifdef USE_FUNC_CALL_ALMHDR
/*
 * Alarm handler start routine
 */
EXPORT void knl_call_almhdr( ALMCB *almcb )
{
	almcb->almstat &= ~TALM_STA;

	/* Execute alarm handler/ Enable interrupt nesting */
	ENABLE_INTERRUPT_UPTO(TIMER_INTLEVEL);
	CallUserHandlerP1(almcb->exinf, almcb->almhdr, almcb);
	DISABLE_INTERRUPT;
}
#endif /* USE_FUNC_CALL_ALMHDR */


#ifdef USE_FUNC_TK_CRE_ALM
/*
 * Create alarm handler
 */
SYSCALL ID tk_cre_alm_impl P1( CONST T_CALM *pk_calm )
{
#if CHK_RSATR
	const ATR VALID_ALMATR = {
		 TA_HLNG
		|TA_GP
#if USE_OBJECT_NAME
		|TA_DSNAME
#endif
	};
#endif
	ALMCB	*almcb;
	ER	ercd = E_OK;

	CHECK_RSATR(pk_calm->almatr, VALID_ALMATR);
	CHECK_PAR(pk_calm->almhdr != NULL);

	BEGIN_CRITICAL_SECTION;
	/* Get control block from free queue */
	almcb = (ALMCB*)QueRemoveNext(&knl_free_almcb);
	if ( almcb == NULL ) {
		ercd = E_LIMIT;
		goto error_exit;
	}

	/* Initialize control block */
	almcb->exinf   = pk_calm->exinf;
	almcb->almatr  = pk_calm->almatr;
	almcb->almhdr  = pk_calm->almhdr;
	almcb->almstat = TALM_STP;
#if USE_OBJECT_NAME
	if ( (pk_calm->almatr & TA_DSNAME) != 0 ) {
		strncpy((char*)almcb->name, (char*)pk_calm->dsname, OBJECT_NAME_LENGTH);
	}
#endif
#if TA_GP
	if ( (pk_calm->almatr & TA_GP) != 0 ) {
		gp = pk_calm->gp;
	}
	almcb->gp = gp;
#endif

	ercd = ID_ALM(almcb - knl_almcb_table);

    error_exit:
	END_CRITICAL_SECTION;

	return ercd;
}
#endif /* USE_FUNC_TK_CRE_ALM */

#ifdef USE_FUNC_TK_DEL_ALM
/*
 * Delete alarm handler
 */
SYSCALL ER tk_del_alm_impl( ID almid )
{
	ALMCB	*almcb;
	ER	ercd = E_OK;

	CHECK_ALMID(almid);

	almcb = get_almcb(almid);

	BEGIN_CRITICAL_SECTION;
	if ( almcb->almhdr == NULL ) { /* Unregistered handler */
		ercd = E_NOEXS;
	} else {
		if ( (almcb->almstat & TALM_STA) != 0 ) {
			/* Delete from timer event queue */
			knl_timer_delete(&almcb->almtmeb);
		}

		/* Return to FreeQue */
		QueInsert((QUEUE*)almcb, &knl_free_almcb);
		almcb->almhdr = NULL; /* Unregistered handler */
	}
	END_CRITICAL_SECTION;

	return ercd;
}
#endif /* USE_FUNC_TK_DEL_ALM */

#ifdef USE_FUNC_TK_STA_ALM
/*
 * Alarm handler immediate call
 */
LOCAL void knl_immediate_call_almhdr( ALMCB *almcb )
{
	almcb->almstat &= ~TALM_STA;

	/* Execute alarm handler in task-independent part
	   (Keep interrupt disabled) */
	ENTER_TASK_INDEPENDENT;
	CallUserHandlerP1(almcb->exinf, almcb->almhdr, almcb);
	LEAVE_TASK_INDEPENDENT;
}

/*
 * Start alarm handler
 */
SYSCALL ER tk_sta_alm_impl( ID almid, RELTIM almtim )
{
	ALMCB	*almcb;
	ER	ercd = E_OK;

	CHECK_ALMID(almid);

	almcb = get_almcb(almid);

	BEGIN_CRITICAL_SECTION;
	if ( almcb->almhdr == NULL ) { /* Unregistered handler */
		ercd = E_NOEXS;
		goto error_exit;
	}

	if ( (almcb->almstat & TALM_STA) != 0 ) {
		/* Cancel current settings */
		knl_timer_delete(&almcb->almtmeb);
	}

	if ( almtim > 0 ) {
		/* Register onto timer event queue */
		knl_alm_timer_insert(almcb, almtim);
		almcb->almstat |= TALM_STA;
	} else {
		/* Immediate execution */
		knl_immediate_call_almhdr(almcb);
	}

    error_exit:
	END_CRITICAL_SECTION;

	return ercd;
}
#endif /* USE_FUNC_TK_STA_ALM */

#ifdef USE_FUNC_TK_STP_ALM
/*
 * Stop alarm handler
 */
SYSCALL ER tk_stp_alm_impl( ID almid )
{
	ALMCB	*almcb;
	ER	ercd = E_OK;

	CHECK_ALMID(almid);

	almcb = get_almcb(almid);

	BEGIN_CRITICAL_SECTION;
	if ( almcb->almhdr == NULL ) { /* Unregistered handler */
		ercd = E_NOEXS;
	} else {
		if ( (almcb->almstat & TALM_STA) != 0 ) {
			/* Stop alarm handler address */
			knl_timer_delete(&almcb->almtmeb);
			almcb->almstat &= ~TALM_STA;
		}
	}
	END_CRITICAL_SECTION;

	return ercd;
}
#endif /* USE_FUNC_TK_STP_ALM */

#ifdef USE_FUNC_TK_REF_ALM
/*
 * Refer alarm handler state
 */
SYSCALL ER tk_ref_alm_impl( ID almid, T_RALM *pk_ralm )
{
	ALMCB	*almcb;
	LSYSTIM	tm;
	ER	ercd = E_OK;

	CHECK_ALMID(almid);

	almcb = get_almcb(almid);

	BEGIN_CRITICAL_SECTION;
	if ( almcb->almhdr == NULL ) { /* Unregistered handler */
		ercd = E_NOEXS;
	} else {
		if ( (almcb->almstat & TALM_STA) != 0 ) {
			tm = almcb->almtmeb.time;
			tm = ll_sub(tm, knl_current_time);
			tm = ll_sub(tm, uitoll(CFN_TIMER_PERIOD));
			if ( ll_sign(tm) < 0 ) {
				tm = ltoll(0);
			}
		} else {
			tm = ltoll(0);
		}

		pk_ralm->exinf   = almcb->exinf;
		pk_ralm->lfttim  = lltoul(tm);
		pk_ralm->almstat = almcb->almstat;
	}
	END_CRITICAL_SECTION;

	return ercd;
}
#endif /* USE_FUNC_TK_REF_ALM */

#if USE_DBGSPT

#ifdef USE_FUNC_ALARMHANDLER_GETNAME
#if USE_OBJECT_NAME
/*
 * Get object name from control block
 */
EXPORT ER knl_alarmhandler_getname(ID id, UB **name)
{
	ALMCB	*almcb;
	ER	ercd = E_OK;

	CHECK_ALMID(id);

	BEGIN_DISABLE_INTERRUPT;
	almcb = get_almcb(id);
	if ( almcb->almhdr == NULL ) {
		ercd = E_NOEXS;
		goto error_exit;
	}
	if ( (almcb->almatr & TA_DSNAME) == 0 ) {
		ercd = E_OBJ;
		goto error_exit;
	}
	*name = almcb->name;

    error_exit:
	END_DISABLE_INTERRUPT;

	return ercd;
}
#endif /* USE_OBJECT_NAME */
#endif /* USE_FUNC_ALARMHANDLER_GETNAME */

#ifdef USE_FUNC_TD_LST_ALM
/*
 * Refer alarm handler usage state
 */
SYSCALL INT td_lst_alm_impl( ID list[], INT nent )
{
	ALMCB	*almcb, *end;
	INT	n = 0;

	BEGIN_DISABLE_INTERRUPT;
	end = knl_almcb_table + NUM_ALMID;
	for ( almcb = knl_almcb_table; almcb < end; almcb++ ) {
		/* Unregistered handler */
		if ( almcb->almhdr == NULL ) {
			continue;
		}

		if ( n++ < nent ) {
			*list++ = ID_ALM(almcb - knl_almcb_table);
		}
	}
	END_DISABLE_INTERRUPT;

	return n;
}
#endif /* USE_FUNC_TD_LST_ALM */

#ifdef USE_FUNC_TD_REF_ALM
/*
 * Refer alarm handler state
 */
SYSCALL ER td_ref_alm_impl( ID almid, TD_RALM *pk_ralm )
{
	ALMCB	*almcb;
	LSYSTIM	tm;
	ER	ercd = E_OK;

	CHECK_ALMID(almid);

	almcb = get_almcb(almid);

	BEGIN_DISABLE_INTERRUPT;
	if ( almcb->almhdr == NULL ) { /* Unregistered handler */
		ercd = E_NOEXS;
	} else {
		if ( (almcb->almstat & TALM_STA) != 0 ) {
			tm = almcb->almtmeb.time;
			tm = ll_sub(tm, knl_current_time);
			tm = ll_sub(tm, uitoll(CFN_TIMER_PERIOD));
			if ( ll_sign(tm) < 0 ) {
				tm = ltoll(0);
			}
		} else {
			tm = ltoll(0);
		}

		pk_ralm->exinf   = almcb->exinf;
		pk_ralm->lfttim  = lltoul(tm);
		pk_ralm->almstat = almcb->almstat;
	}
	END_DISABLE_INTERRUPT;

	return ercd;
}
#endif /* USE_FUNC_TD_REF_ALM */

#endif /* USE_DBGSPT */
#endif /* CFN_MAX_ALMID */
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
 *	time_calls.h
 *	Time Management Function
 */

#ifndef _TIME_CALLS_H
#define _TIME_CALLS_H

/*
 * Cyclic handler control block
 */
typedef struct cyclic_handler_control_block {
	void	*exinf;		/* Extended information */
	ATR	cycatr;		/* Cyclic handler attribute */
	FP	cychdr;		/* Cyclic handler address */
	UINT	cycstat;	/* Cyclic handler state */
	RELTIM	cyctim;		/* Cyclic time */
	TMEB	cyctmeb;	/* Timer event block */
#if TA_GP
	void	*gp;		/* Global pointer */
#endif
#if USE_OBJECT_NAME
	UB	name[OBJECT_NAME_LENGTH];	/* name */
#endif
} CYCCB;

IMPORT CYCCB	knl_cyccb_table[];	/* Cyclic handler control block */
IMPORT QUEUE	knl_free_cyccb;	/* FreeQue */

#define get_cyccb(id)	( &knl_cyccb_table[INDEX_CYC(id)] )


/*
 * Next startup time
 */
LSYSTIM knl_cyc_next_time( CYCCB *cyccb )
{
	LSYSTIM		tm;
	longlong	n;

	tm = ll_add(cyccb->cyctmeb.time, uitoll(cyccb->cyctim));

	if ( ll_cmp(tm, knl_current_time) <= 0 ) {

		/* Adjust time to be later than current time */
		tm = ll_sub(knl_current_time, cyccb->cyctmeb.time);
		n  = lui_div(tm, cyccb->cyctim);
		ll_inc(&n);
		tm = lui_mul(n, cyccb->cyctim);
		tm = ll_add(cyccb->cyctmeb.time, tm);
	}

	return tm;
}

IMPORT void knl_call_cychdr( CYCCB* cyccb );

/*
 * Register timer event queue
 */
void knl_cyc_timer_insert( CYCCB *cyccb, LSYSTIM tm )
{
	knl_timer_insert_abs(&cyccb->cyctmeb, tm, (CBACK)knl_call_cychdr, cyccb);
}


/*
 * Alarm handler control block
 */
typedef struct alarm_handler_control_block {
	void	*exinf;		/* Extended information */
	ATR	almatr;		/* Alarm handler attribute */
	FP	almhdr;		/* Alarm handler address */
	UINT	almstat;	/* Alarm handler state */
	TMEB	almtmeb;	/* Timer event block */
#if TA_GP
	void	*gp;		/* Global pointer */
#endif
#if USE_OBJECT_NAME
	UB	name[OBJECT_NAME_LENGTH];	/* name */
#endif
} ALMCB;
/** [END Common Definitions] */

IMPORT ALMCB	knl_almcb_table[];	/* Alarm handler control block */
IMPORT QUEUE	knl_free_almcb;	/* FreeQue */

#define get_almcb(id)	( &knl_almcb_table[INDEX_ALM(id)] )

IMPORT void knl_call_almhdr( ALMCB *almcb );

/*
 * Register onto timer event queue
 */
void knl_alm_timer_insert( ALMCB *almcb, RELTIM reltim )
{
	LSYSTIM	tm;

	/* To guarantee to start the handler after the specified time, 
	   add CFN_TIMER_PERIOD */
	tm = ll_add(knl_current_time, uitoll(reltim));
	tm = ll_add(tm, uitoll(CFN_TIMER_PERIOD));

	knl_timer_insert_abs(&almcb->almtmeb, tm, (CBACK)knl_call_almhdr, almcb);
}


#endif /* _TIME_CALLS_H */
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
 *	tkdev_conf.h (FM3)
 *	Target System Configuration
 */

#ifndef _TKDEV_CONF_
#define _TKDEV_CONF_
/* Also included from assembler source */

#define ARM_INT_MASK 0xC0
#define ARM_IRQ_BIT  0x80
#define ARM_FIQ_BIT  0x40
#define ARM_THM_BIT  0x20


// swkim_timer
#define TMCLK		40	//120	/* Timer clock input (MHz) */

/*
 * Timer interrupt level
 */
#define TIMER_INTLEVEL		0

#endif /* _TKDEV_CONF_ */
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
 *	tkdev_init.c (FM3)
 *	micro T-Kernel Device-Dependent Initialization/Finalization
 */
/*--------------------------------------------------------------------*/
/*  Include definition                                                */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Struct definition                                                 */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Constant definition                                               */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Variable definition                                               */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Macro definition                                                  */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Prototype declaration                                             */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Function definition                                               */
/*--------------------------------------------------------------------*/

#include <tk_typedef.h>
#include <errno.h>

/*
 * Target system-dependent initialization
 */
EXPORT ER knl_tkdev_initialize( void )
{
	return E_OK;
}

#if USE_CLEANUP
/*
 * Target system-dependent finalization
 *	No return from this function.
 */
EXPORT void knl_tkdev_exit( void )
{
	disint();
	tm_exit(0);	/* Turn off power and exit */

	/* Not suppose to return from 'tm_exit,' but just in case */
	for ( ;; ) {
		tm_monitor();  /* To T-Monitor equivalent function */
	}
}
#endif /* USE_CLEANUP */
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
 *	tkdev_timer.h (FM3)
 *	Hardware-Dependent Timer Processing
 */

#ifndef _TKDEV_TIMER_
#define _TKDEV_TIMER_

#include <syslib.h>
#include <sysinfo.h>
#include "tkdev_conf.h"

#define ENAINT	Asm("ldr	r0, =0 \n" \
		    "msr	basepri, r0")

#define DISINT	Asm("ldr	r0, =MAX_EXTINT_PRI \n" \
		    "msr	basepri, r0")

/*
 * Settable interval range (millisecond)
 */
#define MIN_TIMER_PERIOD	1
#define MAX_TIMER_PERIOD	50

/*
 * Set timer
 */
void knl_init_hw_timer( void )
{
	UINT	n, imask;

	DI(imask);

	/* Set System timer CLK source to Core, Systick exception enable */
	out_w(SYST_CSR, 0x00000006);

	/* Set counter */
	n = (UW)(CFN_TIMER_PERIOD * (TMCLK * 1000) - 1);
	out_w(SYST_RVR, n);

	/* Start timer count */
	out_w(SYST_CSR, 0x00000007);

	EI(imask);
}

/*
 * Timer start processing
 *	Initialize the timer and start the periodical timer interrupt.
 */
void knl_start_hw_timer( void )
{
IMPORT	void	knl_timer_handler_startup( void );

	/* Set timer */
	knl_init_hw_timer();
}

/*
 * Clear timer interrupt
 *	Clear the timer interrupt request. Depending on the type of
 *	hardware, there are two timings for clearing: at the beginning
 *	and the end of the interrupt handler.
 *	'clear_hw_timer_interrupt()' is called at the beginning of the
 *	timer interrupt handler.
 *	'end_of_hw_timer_interrupt()' is called at the end of the timer
 *	interrupt handler.
 *	Use either or both according to hardware.
 */
IMPORT void knl_clear_hw_timer_interrupt( void );


void knl_end_of_hw_timer_interrupt( void )
{

}

/*
 * Timer stop processing
 *	Stop the timer operation.
 *	Called when system stops.
 */
void knl_terminate_hw_timer( void )
{
	/* Timer interrupt disable */
	out_w(SYST_CSR, 0x00000000);
}

/*
 * Get processing time from the previous timer interrupt to the
 * current (nanosecond)
 *	Consider the possibility that the timer interrupt occurred
 *	during the interrupt disable and calculate the processing time
 *	within the following
 *	range: 0 <= Processing time < CFN_TIMER_PERIOD * 2
 */
UW knl_get_hw_timer_nsec( void )
{
	UW	ofs;
	UINT	imask;

	DI(imask);

	ofs = (in_w(SYST_CVR) & 0xffffff) + 1;

	EI(imask);

	return (ofs * 1000 ) / TMCLK;
}

#endif /* _TKDEV_TIMER_ */
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
 *	@(#)tkernel.h
 *
 *	T-Kernel Definition
 */

#ifndef __TK_TKERNEL_H__
#define __TK_TKERNEL_H__

#include <basic.h>
#include <tk_typedef.h>
#include <errno.h>

#include <syscall.h>
#include <syslib.h>

#include <profile.h>

#endif /* __TK_TKERNEL_H__ */
/*
 *	micro T-Kernel function code
 */

#ifndef _TKFNCD_H_
#define _TKFNCD_H_

#if INT_BITWIDTH == 16
#include "tkfncd16.h"
#else
#include "tkfncd32.h"
#endif

#endif /* _TKFNCD_H_ */
/*
 *	micro T-Kernel function code
 */

#define TFN_CRE_TSK	0x8011
#define TFN_DEL_TSK	0x8021
#define TFN_STA_TSK	0x8032
#define TFN_EXT_TSK	0x8040
#define TFN_EXD_TSK	0x8050
#define TFN_TER_TSK	0x8061
#define TFN_DIS_DSP	0x8070
#define TFN_ENA_DSP	0x8080
#define TFN_CHG_PRI	0x8092
#define TFN_ROT_RDQ	0x80a1
#define TFN_REL_WAI	0x80b1
#define TFN_GET_TID	0x80c0
#define TFN_GET_REG	0x80d4
#define TFN_SET_REG	0x80e4
#define TFN_REF_TSK	0x80f2
#define TFN_SUS_TSK	0x8101
#define TFN_RSM_TSK	0x8111
#define TFN_FRSM_TSK	0x8121
#define TFN_SLP_TSK	0x8131
#define TFN_WUP_TSK	0x8141
#define TFN_CAN_WUP	0x8151
#define TFN_CRE_SEM	0x8161
#define TFN_DEL_SEM	0x8171
#define TFN_SIG_SEM	0x8182
#define TFN_WAI_SEM	0x8193
#define TFN_REF_SEM	0x81a2
#define TFN_CRE_MTX	0x81b1
#define TFN_DEL_MTX	0x81c1
#define TFN_LOC_MTX	0x81d2
#define TFN_UNL_MTX	0x81e1
#define TFN_REF_MTX	0x81f2
#define TFN_CRE_FLG	0x8201
#define TFN_DEL_FLG	0x8211
#define TFN_SET_FLG	0x8222
#define TFN_CLR_FLG	0x8232
#define TFN_WAI_FLG	0x8245
#define TFN_REF_FLG	0x8252
#define TFN_CRE_MBX	0x8261
#define TFN_DEL_MBX	0x8271
#define TFN_SND_MBX	0x8282
#define TFN_RCV_MBX	0x8293
#define TFN_REF_MBX	0x82a2
#define TFN_CRE_MBF	0x82b1
#define TFN_DEL_MBF	0x82c1
#define TFN_SND_MBF	0x82d4
#define TFN_RCV_MBF	0x82e3
#define TFN_REF_MBF	0x82f2
#define TFN_CRE_POR	0x8301
#define TFN_DEL_POR	0x8311
#define TFN_CAL_POR	0x8325
#define TFN_ACP_POR	0x8335
#define TFN_FWD_POR	0x8345
#define TFN_RPL_RDV	0x8353
#define TFN_REF_POR	0x8362
#define TFN_DEF_INT	0x8372
#define TFN_RET_INT	0x8380
#define TFN_CRE_MPL	0x8391
#define TFN_DEL_MPL	0x83a1
#define TFN_GET_MPL	0x83b4
#define TFN_REL_MPL	0x83c2
#define TFN_REF_MPL	0x83d2
#define TFN_CRE_MPF	0x83e1
#define TFN_DEL_MPF	0x83f1
#define TFN_GET_MPF	0x8403
#define TFN_REL_MPF	0x8412
#define TFN_REF_MPF	0x8422
#define TFN_SET_TIM	0x8431
#define TFN_GET_TIM	0x8441
#define TFN_GET_OTM	0x8451
#define TFN_DLY_TSK	0x8461
#define TFN_CRE_CYC	0x8471
#define TFN_DEL_CYC	0x8481
#define TFN_STA_CYC	0x8491
#define TFN_STP_CYC	0x84a1
#define TFN_REF_CYC	0x84b2
#define TFN_CRE_ALM	0x84c1
#define TFN_DEL_ALM	0x84d1
#define TFN_STA_ALM	0x84e2
#define TFN_STP_ALM	0x84f1
#define TFN_REF_ALM	0x8502
#define TFN_REF_VER	0x8511
#define TFN_REF_SYS	0x8521
#define TFN_DEF_SSY	0x8532
#define TFN_REF_SSY	0x8542
#define TFN_OPN_DEV	0x8552
#define TFN_CLS_DEV	0x8562
#define TFN_REA_DEV	0x8575
#define TFN_SREA_DEV	0x8585
#define TFN_WRI_DEV	0x8595
#define TFN_SWRI_DEV	0x85a5
#define TFN_WAI_DEV	0x85b5
#define TFN_SUS_DEV	0x85c1
#define TFN_GET_DEV	0x85d2
#define TFN_REF_DEV	0x85e2
#define TFN_OREF_DEV	0x85f2
#define TFN_LST_DEV	0x8603
#define TFN_EVT_DEV	0x8613
#define TFN_DEF_DEV	0x8623
#define TFN_REF_IDV	0x8631

/*
 *	micro T-Kernel function code
 */

#define TFN_CRE_TSK	0x80010100
#define TFN_DEL_TSK	0x80020100
#define TFN_STA_TSK	0x80030200
#define TFN_EXT_TSK	0x80040000
#define TFN_EXD_TSK	0x80050000
#define TFN_TER_TSK	0x80060100
#define TFN_DIS_DSP	0x80070000
#define TFN_ENA_DSP	0x80080000
#define TFN_CHG_PRI	0x80090200
#define TFN_ROT_RDQ	0x800a0100
#define TFN_REL_WAI	0x800b0100
#define TFN_GET_TID	0x800c0000
#define TFN_GET_REG	0x800d0400
#define TFN_SET_REG	0x800e0400
#define TFN_REF_TSK	0x800f0200
#define TFN_SUS_TSK	0x80100100
#define TFN_RSM_TSK	0x80110100
#define TFN_FRSM_TSK	0x80120100
#define TFN_SLP_TSK	0x80130100
#define TFN_WUP_TSK	0x80140100
#define TFN_CAN_WUP	0x80150100
#define TFN_CRE_SEM	0x80160100
#define TFN_DEL_SEM	0x80170100
#define TFN_SIG_SEM	0x80180200
#define TFN_WAI_SEM	0x80190300
#define TFN_REF_SEM	0x801a0200
#define TFN_CRE_MTX	0x801b0100
#define TFN_DEL_MTX	0x801c0100
#define TFN_LOC_MTX	0x801d0200
#define TFN_UNL_MTX	0x801e0100
#define TFN_REF_MTX	0x801f0200
#define TFN_CRE_FLG	0x80200100
#define TFN_DEL_FLG	0x80210100
#define TFN_SET_FLG	0x80220200
#define TFN_CLR_FLG	0x80230200
#define TFN_WAI_FLG	0x80240500
#define TFN_REF_FLG	0x80250200
#define TFN_CRE_MBX	0x80260100
#define TFN_DEL_MBX	0x80270100
#define TFN_SND_MBX	0x80280200
#define TFN_RCV_MBX	0x80290300
#define TFN_REF_MBX	0x802a0200
#define TFN_CRE_MBF	0x802b0100
#define TFN_DEL_MBF	0x802c0100
#define TFN_SND_MBF	0x802d0400
#define TFN_RCV_MBF	0x802e0300
#define TFN_REF_MBF	0x802f0200
#define TFN_CRE_POR	0x80300100
#define TFN_DEL_POR	0x80310100
#define TFN_CAL_POR	0x80320500
#define TFN_ACP_POR	0x80330500
#define TFN_FWD_POR	0x80340500
#define TFN_RPL_RDV	0x80350300
#define TFN_REF_POR	0x80360200
#define TFN_DEF_INT	0x80370200
#define TFN_RET_INT	0x80380000
#define TFN_CRE_MPL	0x80390100
#define TFN_DEL_MPL	0x803a0100
#define TFN_GET_MPL	0x803b0400
#define TFN_REL_MPL	0x803c0200
#define TFN_REF_MPL	0x803d0200
#define TFN_CRE_MPF	0x803e0100
#define TFN_DEL_MPF	0x803f0100
#define TFN_GET_MPF	0x80400300
#define TFN_REL_MPF	0x80410200
#define TFN_REF_MPF	0x80420200
#define TFN_SET_TIM	0x80430100
#define TFN_GET_TIM	0x80440100
#define TFN_GET_OTM	0x80450100
#define TFN_DLY_TSK	0x80460100
#define TFN_CRE_CYC	0x80470100
#define TFN_DEL_CYC	0x80480100
#define TFN_STA_CYC	0x80490100
#define TFN_STP_CYC	0x804a0100
#define TFN_REF_CYC	0x804b0200
#define TFN_CRE_ALM	0x804c0100
#define TFN_DEL_ALM	0x804d0100
#define TFN_STA_ALM	0x804e0200
#define TFN_STP_ALM	0x804f0100
#define TFN_REF_ALM	0x80500200
#define TFN_REF_VER	0x80510100
#define TFN_REF_SYS	0x80520100
#define TFN_DEF_SSY	0x80530200
#define TFN_REF_SSY	0x80540200
#define TFN_OPN_DEV	0x80550200
#define TFN_CLS_DEV	0x80560200
#define TFN_REA_DEV	0x80570500
#define TFN_SREA_DEV	0x80580500
#define TFN_WRI_DEV	0x80590500
#define TFN_SWRI_DEV	0x805a0500
#define TFN_WAI_DEV	0x805b0500
#define TFN_SUS_DEV	0x805c0100
#define TFN_GET_DEV	0x805d0200
#define TFN_REF_DEV	0x805e0200
#define TFN_OREF_DEV	0x805f0200
#define TFN_LST_DEV	0x80600300
#define TFN_EVT_DEV	0x80610300
#define TFN_DEF_DEV	0x80620300
#define TFN_REF_IDV	0x80630100

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
 *	tkstart.c
 *	micro T-Kernel Startup / Finalization
 */

#include "kernel.h"
#include "timer.h"
#include "task.h"
#include <tmonitor.h>

/*
 * Start initial task
 */
LOCAL void knl_init_task_startup( T_CTSK *ctsk )
{
	ER	ercd;

#if TA_GP
	extern int _gp;
	ercd = tk_cre_tsk_impl(ctsk, 0,0,0,0,&_gp);
#else
	ercd = tk_cre_tsk_impl(ctsk);
#endif
	if ( ercd < E_OK ) {
		goto err_exit;
	}

	ercd = tk_sta_tsk_impl((ID)ercd, 0);
	if ( ercd < E_OK ) {
		goto err_exit;
	}

	return;

err_exit:
#if USE_KERNEL_MESSAGE
	tm_putstring((UB*)"init_task can not started\n");
#endif
	tm_monitor();
}

/*
 * Call module initialization
 */
LOCAL void knl_init_module( ER (*initfunc)( void ), UB *name )
{
	ER	ercd;

	ercd = (*initfunc)();
	if ( ercd < E_OK ) {
#if USE_KERNEL_MESSAGE
		tm_putstring(name);
		tm_putstring((UB*)" : module initialize error\n");
#endif
		tm_monitor();
	}
}
#define InitModule(name)	knl_init_module( knl_##name##_initialize, (UB*)#name )

/*
 * Initialize kernel and create/start initial task
 */
EXPORT void knl_t_kernel_main( T_CTSK *inittask )
{

	DISABLE_INTERRUPT;

	/*
	 * Target-dependent initialization
	 */
	InitModule(cpu);
	InitModule(tkdev);

	/*
	 * Each module initialization
	 */
	InitModule(task);
#if CFN_MAX_SEMID > 0
#ifdef USE_FUNC_SEMAPHORE_INITIALIZE
	InitModule(semaphore);
#endif
#endif
#if CFN_MAX_FLGID > 0
#ifdef USE_FUNC_EVENTFLAG_INITIALIZE
	InitModule(eventflag);
#endif
#endif
#if CFN_MAX_MBXID > 0
#ifdef USE_FUNC_MAILBOX_INITIALIZE
	InitModule(mailbox);
#endif
#endif
#if CFN_MAX_MBFID > 0
#ifdef USE_FUNC_MESSAGEBUFFER_INITIALIZE
	InitModule(messagebuffer);
#endif
#endif
#if CFN_MAX_PORID > 0
#ifdef USE_FUNC_RENDEZVOUS_INITIALIZE
	InitModule(rendezvous);
#endif
#endif
#if CFN_MAX_MTXID > 0
#ifdef USE_FUNC_MUTEX_INITIALIZE
	InitModule(mutex);
#endif
#endif
#if CFN_MAX_MPLID > 0
#ifdef USE_FUNC_MEMORYPOOL_INITIALIZE
	InitModule(memorypool);
#endif
#endif
#if CFN_MAX_MPFID > 0
#ifdef USE_FUNC_FIX_MEMORYPOOL_INITIALIZE
	InitModule(fix_memorypool);
#endif
#endif
#if CFN_MAX_CYCID > 0
#ifdef USE_FUNC_CYCLICHANDLER_INITIALIZE
	InitModule(cyclichandler);
#endif
#endif
#if CFN_MAX_ALMID > 0
#ifdef USE_FUNC_ALARMHANDLER_INITIALIZE
	InitModule(alarmhandler);
#endif
#endif
#if CFN_MAX_SSYID > 0
#ifdef USE_FUNC_SUBSYSTEM_INITIALIZE
	InitModule(subsystem);
#endif
#endif
	InitModule(timer);

	/*
	 * Create/start initial task
	 */
	knl_init_task_startup(inittask);

	knl_force_dispatch();
	/* No return */
}

#if USE_CLEANUP
/*
 * Finalization
 */
EXPORT void knl_t_kernel_shutdown( void )
{
	knl_timer_shutdown();
	knl_cpu_shutdown();
}

/*
 * Execute finalization and stop
 */
EXPORT void knl_t_kernel_exit( void )
{
	knl_t_kernel_shutdown();
	knl_tkdev_exit();
	/* No return */
}
#endif /* USE_CLEANUP */
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
 *	tksvcproxy.h
 *	micro T-Kernel system call proxy for direct call
 */

#define tk_cre_tsk_impl	tk_cre_tsk
#define tk_del_tsk_impl	tk_del_tsk
#define tk_sta_tsk_impl	tk_sta_tsk
#define tk_ext_tsk_impl	tk_ext_tsk
#define tk_exd_tsk_impl	tk_exd_tsk
#define tk_ter_tsk_impl	tk_ter_tsk
#define tk_dis_dsp_impl	tk_dis_dsp
#define tk_ena_dsp_impl	tk_ena_dsp
#define tk_chg_pri_impl	tk_chg_pri
#define tk_rot_rdq_impl	tk_rot_rdq
#define tk_rel_wai_impl	tk_rel_wai
#define tk_get_tid_impl	tk_get_tid
#define tk_get_reg_impl	tk_get_reg
#define tk_set_reg_impl	tk_set_reg
#define tk_ref_tsk_impl	tk_ref_tsk
#define tk_sus_tsk_impl	tk_sus_tsk
#define tk_rsm_tsk_impl	tk_rsm_tsk
#define tk_frsm_tsk_impl	tk_frsm_tsk
#define tk_slp_tsk_impl	tk_slp_tsk
#define tk_wup_tsk_impl	tk_wup_tsk
#define tk_can_wup_impl	tk_can_wup
#define tk_cre_sem_impl	tk_cre_sem
#define tk_del_sem_impl	tk_del_sem
#define tk_sig_sem_impl	tk_sig_sem
#define tk_wai_sem_impl	tk_wai_sem
#define tk_ref_sem_impl	tk_ref_sem
#define tk_cre_mtx_impl	tk_cre_mtx
#define tk_del_mtx_impl	tk_del_mtx
#define tk_loc_mtx_impl	tk_loc_mtx
#define tk_unl_mtx_impl	tk_unl_mtx
#define tk_ref_mtx_impl	tk_ref_mtx
#define tk_cre_flg_impl	tk_cre_flg
#define tk_del_flg_impl	tk_del_flg
#define tk_set_flg_impl	tk_set_flg
#define tk_clr_flg_impl	tk_clr_flg
#define tk_wai_flg_impl	tk_wai_flg
#define tk_ref_flg_impl	tk_ref_flg
#define tk_cre_mbx_impl	tk_cre_mbx
#define tk_del_mbx_impl	tk_del_mbx
#define tk_snd_mbx_impl	tk_snd_mbx
#define tk_rcv_mbx_impl	tk_rcv_mbx
#define tk_ref_mbx_impl	tk_ref_mbx
#define tk_cre_mbf_impl	tk_cre_mbf
#define tk_del_mbf_impl	tk_del_mbf
#define tk_snd_mbf_impl	tk_snd_mbf
#define tk_rcv_mbf_impl	tk_rcv_mbf
#define tk_ref_mbf_impl	tk_ref_mbf
#define tk_cre_por_impl	tk_cre_por
#define tk_del_por_impl	tk_del_por
#define tk_cal_por_impl	tk_cal_por
#define tk_acp_por_impl	tk_acp_por
#define tk_fwd_por_impl	tk_fwd_por
#define tk_rpl_rdv_impl	tk_rpl_rdv
#define tk_ref_por_impl	tk_ref_por
#define tk_def_int_impl	tk_def_int
#define tk_ret_int_impl	tk_ret_int
#define tk_cre_mpl_impl	tk_cre_mpl
#define tk_del_mpl_impl	tk_del_mpl
#define tk_get_mpl_impl	tk_get_mpl
#define tk_rel_mpl_impl	tk_rel_mpl
#define tk_ref_mpl_impl	tk_ref_mpl
#define tk_cre_mpf_impl	tk_cre_mpf
#define tk_del_mpf_impl	tk_del_mpf
#define tk_get_mpf_impl	tk_get_mpf
#define tk_rel_mpf_impl	tk_rel_mpf
#define tk_ref_mpf_impl	tk_ref_mpf
#define tk_set_tim_impl	tk_set_tim
#define tk_get_tim_impl	tk_get_tim
#define tk_get_otm_impl	tk_get_otm
#define tk_dly_tsk_impl	tk_dly_tsk
#define tk_cre_cyc_impl	tk_cre_cyc
#define tk_del_cyc_impl	tk_del_cyc
#define tk_sta_cyc_impl	tk_sta_cyc
#define tk_stp_cyc_impl	tk_stp_cyc
#define tk_ref_cyc_impl	tk_ref_cyc
#define tk_cre_alm_impl	tk_cre_alm
#define tk_del_alm_impl	tk_del_alm
#define tk_sta_alm_impl	tk_sta_alm
#define tk_stp_alm_impl	tk_stp_alm
#define tk_ref_alm_impl	tk_ref_alm
#define tk_ref_ver_impl	tk_ref_ver
#define tk_ref_sys_impl	tk_ref_sys
#define tk_def_ssy_impl	tk_def_ssy
#define tk_ref_ssy_impl	tk_ref_ssy
#define tk_opn_dev_impl	tk_opn_dev
#define tk_cls_dev_impl	tk_cls_dev
#define tk_rea_dev_impl	tk_rea_dev
#define tk_srea_dev_impl	tk_srea_dev
#define tk_wri_dev_impl	tk_wri_dev
#define tk_swri_dev_impl	tk_swri_dev
#define tk_wai_dev_impl	tk_wai_dev
#define tk_sus_dev_impl	tk_sus_dev
#define tk_get_dev_impl	tk_get_dev
#define tk_ref_dev_impl	tk_ref_dev
#define tk_oref_dev_impl	tk_oref_dev
#define tk_lst_dev_impl	tk_lst_dev
#define tk_evt_dev_impl	tk_evt_dev
#define tk_def_dev_impl	tk_def_dev
#define tk_ref_idv_impl	tk_ref_idv
/*
 *	micro T-Kernel system call branch table
 *
 *	   created from /usr/local/te/include/tk/syscall.h
 */

#include <machine.h>

#define _SVC_ENTRY(name)	.long	_name##_impl

#define N_TFN	99

	_SVC_ENTRY(tk_cre_tsk)
	_SVC_ENTRY(tk_del_tsk)
	_SVC_ENTRY(tk_sta_tsk)
	_SVC_ENTRY(tk_ext_tsk)
	_SVC_ENTRY(tk_exd_tsk)
	_SVC_ENTRY(tk_ter_tsk)
	_SVC_ENTRY(tk_dis_dsp)
	_SVC_ENTRY(tk_ena_dsp)
	_SVC_ENTRY(tk_chg_pri)
	_SVC_ENTRY(tk_rot_rdq)
	_SVC_ENTRY(tk_rel_wai)
	_SVC_ENTRY(tk_get_tid)
	_SVC_ENTRY(tk_get_reg)
	_SVC_ENTRY(tk_set_reg)
	_SVC_ENTRY(tk_ref_tsk)
	_SVC_ENTRY(tk_sus_tsk)
	_SVC_ENTRY(tk_rsm_tsk)
	_SVC_ENTRY(tk_frsm_tsk)
	_SVC_ENTRY(tk_slp_tsk)
	_SVC_ENTRY(tk_wup_tsk)
	_SVC_ENTRY(tk_can_wup)
	_SVC_ENTRY(tk_cre_sem)
	_SVC_ENTRY(tk_del_sem)
	_SVC_ENTRY(tk_sig_sem)
	_SVC_ENTRY(tk_wai_sem)
	_SVC_ENTRY(tk_ref_sem)
	_SVC_ENTRY(tk_cre_mtx)
	_SVC_ENTRY(tk_del_mtx)
	_SVC_ENTRY(tk_loc_mtx)
	_SVC_ENTRY(tk_unl_mtx)
	_SVC_ENTRY(tk_ref_mtx)
	_SVC_ENTRY(tk_cre_flg)
	_SVC_ENTRY(tk_del_flg)
	_SVC_ENTRY(tk_set_flg)
	_SVC_ENTRY(tk_clr_flg)
	_SVC_ENTRY(tk_wai_flg)
	_SVC_ENTRY(tk_ref_flg)
	_SVC_ENTRY(tk_cre_mbx)
	_SVC_ENTRY(tk_del_mbx)
	_SVC_ENTRY(tk_snd_mbx)
	_SVC_ENTRY(tk_rcv_mbx)
	_SVC_ENTRY(tk_ref_mbx)
	_SVC_ENTRY(tk_cre_mbf)
	_SVC_ENTRY(tk_del_mbf)
	_SVC_ENTRY(tk_snd_mbf)
	_SVC_ENTRY(tk_rcv_mbf)
	_SVC_ENTRY(tk_ref_mbf)
	_SVC_ENTRY(tk_cre_por)
	_SVC_ENTRY(tk_del_por)
	_SVC_ENTRY(tk_cal_por)
	_SVC_ENTRY(tk_acp_por)
	_SVC_ENTRY(tk_fwd_por)
	_SVC_ENTRY(tk_rpl_rdv)
	_SVC_ENTRY(tk_ref_por)
	_SVC_ENTRY(tk_def_int)
	_SVC_ENTRY(tk_ret_int)
	_SVC_ENTRY(tk_cre_mpl)
	_SVC_ENTRY(tk_del_mpl)
	_SVC_ENTRY(tk_get_mpl)
	_SVC_ENTRY(tk_rel_mpl)
	_SVC_ENTRY(tk_ref_mpl)
	_SVC_ENTRY(tk_cre_mpf)
	_SVC_ENTRY(tk_del_mpf)
	_SVC_ENTRY(tk_get_mpf)
	_SVC_ENTRY(tk_rel_mpf)
	_SVC_ENTRY(tk_ref_mpf)
	_SVC_ENTRY(tk_set_tim)
	_SVC_ENTRY(tk_get_tim)
	_SVC_ENTRY(tk_get_otm)
	_SVC_ENTRY(tk_dly_tsk)
	_SVC_ENTRY(tk_cre_cyc)
	_SVC_ENTRY(tk_del_cyc)
	_SVC_ENTRY(tk_sta_cyc)
	_SVC_ENTRY(tk_stp_cyc)
	_SVC_ENTRY(tk_ref_cyc)
	_SVC_ENTRY(tk_cre_alm)
	_SVC_ENTRY(tk_del_alm)
	_SVC_ENTRY(tk_sta_alm)
	_SVC_ENTRY(tk_stp_alm)
	_SVC_ENTRY(tk_ref_alm)
	_SVC_ENTRY(tk_ref_ver)
	_SVC_ENTRY(tk_ref_sys)
	_SVC_ENTRY(tk_def_ssy)
	_SVC_ENTRY(tk_ref_ssy)
	_SVC_ENTRY(tk_opn_dev)
	_SVC_ENTRY(tk_cls_dev)
	_SVC_ENTRY(tk_rea_dev)
	_SVC_ENTRY(tk_srea_dev)
	_SVC_ENTRY(tk_wri_dev)
	_SVC_ENTRY(tk_swri_dev)
	_SVC_ENTRY(tk_wai_dev)
	_SVC_ENTRY(tk_sus_dev)
	_SVC_ENTRY(tk_get_dev)
	_SVC_ENTRY(tk_ref_dev)
	_SVC_ENTRY(tk_oref_dev)
	_SVC_ENTRY(tk_lst_dev)
	_SVC_ENTRY(tk_evt_dev)
	_SVC_ENTRY(tk_def_dev)
	_SVC_ENTRY(tk_ref_idv)
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
 *	@(#)typedef.h (T-Kernel)
 *
 *	T-Kernel Standard Data Type Definition
 */

#ifndef	__TK_TYPEDEF_H__
#define __TK_TYPEDEF_H__

#include <basic.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Data type in which meaning is defined in T-Kernel/OS specification 
 */
typedef INT		FN;		/* Function code */
typedef INT		RNO;		/* Rendezvous number */
typedef UW		ATR;		/* Object/handler attribute */
typedef INT		ER;		/* Error code */
typedef INT		PRI;		/* Priority */
typedef W		TMO;		/* Time out setting */
typedef UW		RELTIM;		/* Relative time */

typedef struct systim {			/* System time */
	W	hi;			/* Upper 32 bits */
	UW	lo;			/* Lower 32 bits */
} SYSTIM;

/*
 * Common constant
 */
#define NULL		0		/* Invalid address */
#define TA_NULL		0U		/* No special attributes indicated */
#define TMO_POL		0		/* Polling */
#define TMO_FEVR	(-1)		/* Permanent wait */

/* ------------------------------------------------------------------------ */

/*
 * 64 bits value
 */
typedef struct dw {
#if BIGENDIAN
	W	hi;	/* Upper 32 bits */
	UW	lo;	/* Lower 32 bits */
#else
	UW	lo;	/* Lower 32 bits */
	W	hi;	/* Upper 32 bits */
#endif
} DW;

#ifdef __cplusplus
}
#endif
#endif /* __TK_TYPEDEF_H__ */
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
 *	@(#)tmonitor.h (tm)
 *
 *	T-Monitor equivalent function
 */

#ifndef __TM_TMONITOR_H__
#define __TM_TMONITOR_H__

#include <basic.h>

#ifdef __cplusplus
extern "C" {
#endif


/*
 * Monitor service function
 */
IMPORT void tm_monitor( void );
IMPORT INT  tm_getchar( INT wait );
IMPORT INT  tm_putchar( INT c );
IMPORT INT  tm_getline( UB *buff );
IMPORT INT  tm_putstring( const UB *buff );
IMPORT INT  tm_command( const UB *buff );
IMPORT void tm_exit( INT mode );
IMPORT INT  tm_printf( const UB *format, ... );

#ifdef __cplusplus
}
#endif
#endif /* __TM_TMONITOR_H__ */
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

#include <typedef.h>

INT tm_command ( UB *buff )
{
	for(;;) {
		;
	}
}
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

#include <typedef.h>

void tm_exit( INT mode )
{
	for(;;) {
		;
	}
}
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

#include <typedef.h>
#include <stddef.h>
#include <syslib.h>

IMPORT void sio_recv_frame( UB* buf, INT size );

/*
 * supported only on wait != 0 (polling not supported)
 */
EXPORT INT tm_getchar( INT wait )
{
	UB	p;
	UINT	imask;

	DI(imask);

	sio_recv_frame(&p, 1);

	EI(imask);

	return (INT)p;
}
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

#include <typedef.h>
#include <stddef.h>
#include <syslib.h>

IMPORT void sio_send_frame( const UB* buf, INT size );
IMPORT void sio_recv_frame( UB* buf, INT size );

/*
 * special key is not supported
 */
EXPORT INT tm_getline( UB *buff )
{
	UB* p = buff;
	int len = 0;
	static const char LF = 0x0a;
	INT imask;

	DI(imask);

	for (;;) {
		sio_recv_frame(p, 1);
		sio_send_frame(p, 1); /* echo back */
		if (*p == 0x0d) {
			sio_send_frame((const void *)&LF, 1);
			*p = 0x00;
			goto err_ret;
		} else if (*p == 0x03) {
			*p = 0x00;
			len = -1;
			goto err_ret;
		}
		p++;
		len++;
	}

err_ret:
	EI(imask);

	return len;
}
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

#include <typedef.h>
#include <stddef.h>

EXPORT void tm_monitor( void )
{
	for(;;) {
		;
	}
}
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
 *	@(#)tm_printf.c (libtm)
 *
 *	printf() / sprintf() routine using t-monitor function.
 *
 *	- Unsupported specifiers: floating point, long long and others.
 *		Coversion:	 a, A, e, E, f, F, g, G, n
 *		Size qualifier:  hh, ll, j, z, t, L
 *	- No limitation of output string length.
 *	- Minimize stack usage.
 *		Depending on available stack size, define OUTBUF_SZ by
 *		appropriate value.
 */
#include <stdarg.h>
#include <basic.h>

/* Output Buffer size in stack */
#define	OUTBUF_SZ	0

extern	int	tm_putchar( int c );
extern	int	tm_putstring( UB *s );

/* Output function */
typedef	struct {
	short	len;		/* Total output length */
	short	cnt;		/* Buffer counts */
	UB	*bufp;		/* Buffer pointer for tm_sprintf */
} OutPar;
typedef	void	(*OutFn)( UB *str, int len, OutPar *par );

/*
 *	Output integer value
 */
static	UB	*outint( UB *ep, unsigned long val, UB base )
{
static const UB  digits[32] = "0123456789abcdef0123456789ABCDEF";
	UB	caps;

	caps = (base & 0x40) >> 2;		/* 'a' or 'A' */
	for (base &= 0x3F; val >= base; val /= base) {
		*--ep = digits[(val % base) + caps];
	}
	*--ep = digits[val + caps];
	return ep;				/* buffer top pointer */
}

/*
 *	Output with format (limitted version)
 */
static	void	_vsprintf( OutFn ostr, OutPar *par, const UB *fmt, va_list ap )
{
#define	MAX_DIGITS		14
	unsigned long	v;
	short			wid, prec, n;
	UB	*fms, *cbs, *cbe, cbuf[MAX_DIGITS];
	UB	c, base, flg, sign, qual;

/* flg */
#define	F_LEFT		0x01
#define	F_PLUS		0x02
#define	F_SPACE		0x04
#define	F_PREFIX	0x08
#define	F_ZERO		0x10

	for (fms = NULL; (c = *fmt++) != '\0'; ) {

		if (c != '%') {	/* Fixed string */
			if (fms == NULL) fms = (UB*)fmt - 1;
			continue;
		}

		/* Output fix string */
		if (fms != NULL) {
			(*ostr)(fms, fmt - fms - 1, par);
			fms = NULL;
		}

		/* Get flags */
		for (flg = 0; ; ) {
			switch (c = *fmt++) {
			case '-': flg |= F_LEFT;	continue;
			case '+': flg |= F_PLUS;	continue;
			case ' ': flg |= F_SPACE;	continue;
			case '#': flg |= F_PREFIX;	continue;
			case '0': flg |= F_ZERO;	continue;
			}
			break;
		}

		/* Get field width */
		if (c == '*') {
			wid = va_arg(ap, int);
			if (wid < 0) {
				wid = -wid;
				flg |= F_LEFT;
			}
			c = *fmt++;
		} else {
			for (wid = 0; c >= '0' && c <= '9'; c = *fmt++)
				wid = wid * 10 + c - '0';
		}

		/* Get precision */
		prec = -1;
		if (c == '.') {
			c = *fmt++;
			if (c == '*') {
				prec = va_arg(ap, int);
				if (prec < 0) prec = 0;
				c = *fmt++;
			} else {
				for (prec = 0;c >= '0' && c <= '9';c = *fmt++)
					prec = prec * 10 + c - '0';
			}
			flg &= ~F_ZERO;		/* No ZERO padding */
		}

		/* Get qualifier */
		qual = 0;
		if (c == 'h' || c == 'l') {
			qual = c;
			c = *fmt++;
		}

		/* Format items */
		base = 10;
		sign = 0;
		cbe = &cbuf[MAX_DIGITS];	/* buffer end pointer */

		switch (c) {
		case 'i':
		case 'd':
		case 'u':
		case 'X':
		case 'x':
		case 'o':
			if (qual == 'l') {
				v = va_arg(ap, unsigned long);
			} else {
				v = va_arg(ap, unsigned int);
				if (qual == 'h') {
					v = (c == 'i' || c == 'd') ?
						(short)v :(unsigned short)v;
				}
			}
			switch (c) {
			case 'i':
			case 'd':
				if ((long)v < 0) {
					v = - (long)v;
					sign = '-';
				} else if ((flg & F_PLUS) != 0) {
					sign = '+';
				} else if ((flg & F_SPACE) != 0) {
					sign = ' ';
				} else {
					break;
				}
				wid--;		/* for sign */
			case 'u':
				break;
			case 'X':
				base += 0x40;	/* base = 16 + 0x40 */
			case 'x':
				base += 8;	/* base = 16 */
			case 'o':
				base -= 2;	/* base = 8 */
				if ((flg & F_PREFIX) != 0 && v != 0) {
					wid -= (base == 8) ? 1 : 2;
					base |= 0x80;
				}
				break;
			}
			/* Note: None outputs when v == 0 && prec == 0 */
			cbs = (v == 0 && prec == 0) ?
						cbe : outint(cbe, v, base);
			break;
		case 'p':
			v = (unsigned long)va_arg(ap, void *);
			if (v != 0) {
				base = 16 | 0x80;
				wid -= 2;
			}
			cbs = outint(cbe, v, base);
			break;
		case 's':
			cbe = cbs = va_arg(ap, UB *);
			if (prec < 0) {
				while (*cbe != '\0') cbe++;
			} else {
				while (--prec >= 0 && *cbe != '\0') cbe++;
			}
			break;
		case 'c':
			cbs = cbe;
			*--cbs = (UB)va_arg(ap, int);
			prec = 0;
			break;
		case '\0':
			fmt--;
			continue;
		default:
			/* Output as fixed string */
			fms = (UB*)fmt - 1;
			continue;
		}

		n = cbe - cbs;				/* item length */
		if ((prec -= n) > 0) n += prec;
		wid -= n;				/* pad length */

		/* Output preceding spaces */
		if ((flg & (F_LEFT | F_ZERO)) == 0 ) {
			while (--wid >= 0) (*ostr)((UB*)" ", 1, par);
		}

		/* Output sign */
		if (sign != 0) {
			(*ostr)(&sign, 1, par);
		}

		/* Output prefix "0x", "0X" or "0" */
		if ((base & 0x80) != 0) {
			(*ostr)((UB*)"0", 1, par);
			if ((base & 0x10) != 0) {
				(*ostr)((base & 0x40) ? (UB*)"X" : (UB*)"x", 1, par);
			}
		}

		/* Output preceding zeros for precision or padding */
		if ((n = prec) <= 0) {
			if ((flg & (F_LEFT | F_ZERO)) == F_ZERO ) {
				n = wid;
				wid = 0;
			}
		}
		while (--n >= 0) (*ostr)((UB*)"0", 1, par);

		/* Output item string */
		(*ostr)(cbs, cbe - cbs, par);

		/* Output tailing spaces */
		while (--wid >= 0) (*ostr)((UB*)" ", 1, par);
	}

	/* Output last fix string */
	if (fms != NULL) {
		(*ostr)(fms, fmt - fms - 1, par);
	}
#if	OUTBUF_SZ > 0
	/* Flush output */
	(*ostr)(NULL, 0, par);
#endif
}

/*
 *	Output to console
 */
static	void	out_cons( UB *str, int len,  OutPar *par )
{
#if	OUTBUF_SZ == 0
	/* Direct output to console */
	par->len += len;
	while (--len >= 0) tm_putchar(*str++);
#else
	/* Buffered output to console */
	if (str == NULL) {	/* Flush */
		if (par->cnt > 0) {
			par->bufp[par->cnt] = '\0';
			tm_putstring(par->bufp);
			par->cnt = 0;
		}
	} else {
		par->len += len;
		while (--len >= 0) {
			if (par->cnt >= OUTBUF_SZ - 1) {
				par->bufp[par->cnt] = '\0';
				tm_putstring(par->bufp);
				par->cnt = 0;
			}
			par->bufp[par->cnt++] = *str++;
		}
	}
#endif
}

INT	tm_printf( const UB *format, ... )
{
	va_list	ap;

#if	OUTBUF_SZ == 0
	short	len = 0;

	va_start(ap, format);
	_vsprintf(out_cons, (OutPar*)&len, format, ap);
	va_end(ap);
	return len;
#else
	char	obuf[OUTBUF_SZ];
	OutPar	par;

	par.len = par.cnt = 0;
	par.bufp = obuf;
	va_start(ap, format);
	_vsprintf(out_cons, (OutPar*)&par, format, ap);
	va_end(ap);
	return par.len;
#endif
}

/*
 *	Output to buffer
 */
static	void	out_buf( UB *str, int len, OutPar *par )
{
	par->len += len;
	while (--len >= 0) *(par->bufp)++ = *str++;
}

int	tm_sprintf( UB *str, const UB *format, ... )
{
	OutPar	par;
	va_list	ap;

	par.len = 0;
	par.bufp = str;
	va_start(ap, format);
	_vsprintf(out_buf, &par, format, ap);
	va_end(ap);
	str[par.len] = '\0';
	return par.len;
}
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

#include <typedef.h>
#include <stddef.h>

IMPORT void sio_send_frame( const UB* buf, INT size );

/*
 * Ctrl-C is not supported
 */
EXPORT INT tm_putchar( INT c )
{
	static const char CR = 0x0d;
	UB buf = (UB)c;

	if (buf == 0x0a) {
		sio_send_frame((const UB*)&CR, 1);
	}
	sio_send_frame(&buf, 1);

	return 0;
}
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

#include <typedef.h>
#include <stddef.h>
#include <syslib.h>

IMPORT void sio_send_frame( const UB* buf, INT size );
IMPORT void sio_recv_frame( UB* buf, INT size );

/*
 * Ctrl-C is not supported
 */
EXPORT INT tm_putstring( UB *buff )
{
	const UB* p = buff;
	static const char CR = 0x0d;
	INT imask;

	DI(imask);

	while (*p != '\0') {
		if (*p == 0x0a) { /* LF */
			sio_send_frame((const void *)&CR, 1);
			sio_send_frame((const void *)p, 1);
		} else {
			sio_send_frame((const void *)p, 1);
		}
		p++;
	}

	EI(imask);

	return 0;
}
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
 *	usermain.c (usermain)
 *	User Main
 */
/*--------------------------------------------------------------------*/
/*  Include definition                                                */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Struct definition                                                 */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Constant definition                                               */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Variable definition                                               */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Macro definition                                                  */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Prototype declaration                                             */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*  Function definition                                               */
/*--------------------------------------------------------------------*/

#include <basic.h>
#include <tkernel.h>
#include <tmonitor.h>

/*
 * Entry routine for the user application.
 * At this point, Initialize and start the user application.
 *
 * Entry routine is called from the initial task for Kernel,
 * so system call for stopping the task should not be issued 
 * from the contexts of entry routine.
 * We recommend that:
 * (1)'usermain()' only generates the user initial task.
 * (2)initialize and start the user application by the user
 * initial task.
 */

EXPORT	INT	usermain( void )
{
	tm_putstring((UB*)"Push any key to shutdown the micro T-Kernel.\n");
	tm_getchar(-1);

	return 0;
}
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
 *	@(#)util.h
 *
 *	General Utilities 
 */

#ifndef	__TK_UTIL_H__
#define __TK_UTIL_H__

#include <basic.h>
#include <tk_typedef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Fast Lock
 */
typedef struct {
	INT	cnt;
	ID	id;
} FastLock;

IMPORT ER CreateLock( FastLock *lock, CONST UB *name );
IMPORT void DeleteLock( FastLock *lock );
IMPORT void Lock( FastLock *lock );
IMPORT void Unlock( FastLock *lock );

/*
 * Multi Lock
 *	Can use the maximum of 16 or 32 independent locks with a single FastMLock.
 *	Divided by the lock number (no). Can specify 0-15 or 0-31 for 'no.'
 *	(Slightly less efficient than FastLock)
 */
typedef struct {
	UINT	flg;
	INT	wai;
	ID	id;
} FastMLock;

IMPORT ER CreateMLock( FastMLock *lock, CONST UB *name );
IMPORT ER DeleteMLock( FastMLock *lock );
IMPORT ER MLockTmo( FastMLock *lock, INT no, TMO tmout );
IMPORT ER MLock( FastMLock *lock, INT no );
IMPORT ER MUnlock( FastMLock *lock, INT no );



#ifdef __cplusplus
}
#endif
#endif /* __TK_UTIL_H__ */
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
 *	utk_config.h
 *	System Configuration Definition
 */

#ifndef _UTK_CONFIG_H_
#define _UTK_CONFIG_H_

#include "config.h"

// #ifdef _APP_H8S2212_
// #include "utk_config_depend.h"
// #endif

// #ifdef _APP_AT91_
// #include "utk_config_depend.h"
// #endif

// #ifdef _APP_MB9AF312K_
// #include "utk_config_depend.h"
// #endif

#ifdef _APP_RL78G13_R5F100ADASP_
#include "utk_config_depend.h"
#endif

#include "utk_config_func.h"

#if !USE_TRAP && !(USE_DBGSPT && USE_HOOK_TRACE)
#include "tksvcproxy.h"
#include "tdsvcproxy.h"
#endif

#endif /* _UTK_CONFIG_H_ */
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
 *	utk_config_depend.h (MB9AF312K)
 *	System Configuration Definition
 */

/* RAMINFO */
#define SYSTEMAREA_TOP		0x1FFFE000	/* RAM system area top */
#define SYSTEMAREA_END		0x20002000	/* RAM system area end */

/* User definition */
#define RI_USERAREA_TOP		0x1FFFE000	/* RAM user area top */
#define RI_USERINIT		NULL		/* User initialization program */


/* SYSCONF */
#define CFN_TIMER_PERIOD	10
#define CFN_MAX_TSKID		32
#define CFN_MAX_SEMID		16
#define CFN_MAX_FLGID		16
#define CFN_MAX_MBXID		8
#define CFN_MAX_MTXID		2
#define CFN_MAX_MBFID		8
#define CFN_MAX_PORID		4
#define CFN_MAX_MPLID		2
#define CFN_MAX_MPFID		8
#define CFN_MAX_CYCID		4
#define CFN_MAX_ALMID		8
#define CFN_MAX_SSYID		4
#define CFN_MAX_SSYPRI		16

#define CFN_MAX_REGDEV		(8)
#define CFN_MAX_OPNDEV		(16)
#define CFN_MAX_REQDEV		(16)
#define CFN_DEVT_MBFSZ0		(-1)
#define CFN_DEVT_MBFSZ1		(-1)

#define CFN_VER_MAKER		0x011C
#define CFN_VER_PRID		0
#define CFN_VER_SPVER		0x6101
#define CFN_VER_PRVER		0x0101
#define CFN_VER_PRNO1		0
#define CFN_VER_PRNO2		0
#define CFN_VER_PRNO3		0
#define CFN_VER_PRNO4		0

#define CFN_REALMEMEND		((void *)0x20002000)

/*
 * Use zero-clear bss section
 */
#define USE_NOINIT		(0)

/*
 * Stack size for each mode
 */
#define EXC_STACK_SIZE		0x200
#define TMP_STACK_SIZE		0x80
#define USR_STACK_SIZE		0		/* not used */

#define EXCEPTION_STACK_TOP	SYSTEMAREA_END
#define TMP_STACK_TOP		(EXCEPTION_STACK_TOP - EXC_STACK_SIZE)
#define APPLICATION_STACK_TOP	(TMP_STACK_TOP - TMP_STACK_SIZE)

/*
 * Use dynamic memory allocation
 */
#define USE_IMALLOC		(1)

/*
 * Use program trace function (in debugger support)
 */
#define USE_HOOK_TRACE		(0)

/*
 * Use clean-up sequence
 */
#define USE_CLEANUP		(1)

/*
 * Use high level programming language support routine
 */
#define USE_HLL_INTHDR		(1)
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
 *	utk_config_func.h
 *	System Configuration Definition
 */

#ifndef _UTK_CONFIG_FUNC_H_
#define _UTK_CONFIG_FUNC_H_

/*
 * System calls
 */
#define USE_FUNC_TK_CRE_TSK
#define USE_FUNC_TK_DEL_TSK
#define USE_FUNC_TK_STA_TSK
#define USE_FUNC_TK_EXT_TSK
#define USE_FUNC_TK_EXD_TSK
#define USE_FUNC_TK_TER_TSK
#define USE_FUNC_TK_DIS_DSP
#define USE_FUNC_TK_ENA_DSP
#define USE_FUNC_TK_CHG_PRI
#define USE_FUNC_TK_ROT_RDQ
#define USE_FUNC_TK_REL_WAI
#define USE_FUNC_TK_GET_TID
#define USE_FUNC_TK_GET_REG
#define USE_FUNC_TK_SET_REG
#define USE_FUNC_TK_REF_TSK
#define USE_FUNC_TK_SUS_TSK
#define USE_FUNC_TK_RSM_TSK
#define USE_FUNC_TK_FRSM_TSK
#define USE_FUNC_TK_SLP_TSK
#define USE_FUNC_TK_WUP_TSK
#define USE_FUNC_TK_CAN_WUP

#if CFN_MAX_SEMID > 0
#define USE_FUNC_TK_CRE_SEM
#define USE_FUNC_TK_DEL_SEM
#define USE_FUNC_TK_SIG_SEM
#define USE_FUNC_TK_WAI_SEM
#define USE_FUNC_TK_REF_SEM
#define USE_FUNC_SEMAPHORE_INITIALIZE
#define USE_FUNC_SEMCB_TABLE
#if USE_DBGSPT
#define USE_FUNC_TD_LST_SEM
#define USE_FUNC_TD_REF_SEM
#define USE_FUNC_TD_SEM_QUE
#define USE_FUNC_SEMAPHORE_GETNAME
#endif
#endif

#if CFN_MAX_MTXID > 0
#define USE_FUNC_TK_CRE_MTX
#define USE_FUNC_TK_DEL_MTX
#define USE_FUNC_TK_LOC_MTX
#define USE_FUNC_TK_UNL_MTX
#define USE_FUNC_TK_REF_MTX
#define USE_FUNC_MUTEX_INITIALIZE
#define USE_FUNC_MTXCB_TABLE
#define USE_FUNC_RELEASE_MUTEX
#define USE_FUNC_SIGNAL_ALL_MUTEX
#define USE_FUNC_CHG_PRI_MUTEX
#if USE_DBGSPT
#define USE_FUNC_TD_LST_MTX
#define USE_FUNC_TD_REF_MTX
#define USE_FUNC_TD_MTX_QUE
#define USE_FUNC_MUTEX_GETNAME
#endif
#endif

#if CFN_MAX_FLGID > 0
#define USE_FUNC_TK_CRE_FLG
#define USE_FUNC_TK_DEL_FLG
#define USE_FUNC_TK_SET_FLG
#define USE_FUNC_TK_CLR_FLG
#define USE_FUNC_TK_WAI_FLG
#define USE_FUNC_TK_REF_FLG
#define USE_FUNC_EVENTFLAG_INITIALIZE
#define USE_FUNC_FLGCB_TABLE
#if USE_DBGSPT
#define USE_FUNC_TD_LST_FLG
#define USE_FUNC_TD_REF_FLG
#define USE_FUNC_TD_FLG_QUE
#define USE_FUNC_EVENTFLAG_GETNAME
#endif
#endif

#if CFN_MAX_MBXID > 0
#define USE_FUNC_TK_CRE_MBX
#define USE_FUNC_TK_DEL_MBX
#define USE_FUNC_TK_SND_MBX
#define USE_FUNC_TK_RCV_MBX
#define USE_FUNC_TK_REF_MBX
#define USE_FUNC_MAILBOX_INITIALIZE
#define USE_FUNC_MBXCB_TABLE
#if USE_DBGSPT
#define USE_FUNC_TD_LST_MBX
#define USE_FUNC_TD_REF_MBX
#define USE_FUNC_TD_MBX_QUE
#define USE_FUNC_MAILBOX_GETNAME
#endif
#endif

#if CFN_MAX_MBFID > 0
#define USE_FUNC_TK_CRE_MBF
#define USE_FUNC_TK_DEL_MBF
#define USE_FUNC_TK_SND_MBF
#define USE_FUNC_TK_RCV_MBF
#define USE_FUNC_TK_REF_MBF
#define USE_FUNC_MESSAGEBUFFER_INITIALIZE
#define USE_FUNC_MBFCB_TABLE
#define USE_FUNC_MSG_TO_MBF
#define USE_FUNC_MBF_WAKEUP
#if USE_DBGSPT
#define USE_FUNC_TD_LST_MBF
#define USE_FUNC_TD_REF_MBF
#define USE_FUNC_TD_SMBF_QUE
#define USE_FUNC_TD_RMBF_QUE
#define USE_FUNC_MESSAGEBUFFER_GETNAME
#endif
#endif

#if CFN_MAX_PORID > 0
#define USE_FUNC_TK_CRE_POR
#define USE_FUNC_TK_DEL_POR
#define USE_FUNC_TK_CAL_POR
#define USE_FUNC_TK_ACP_POR
#define USE_FUNC_TK_FWD_POR
#define USE_FUNC_TK_RPL_RDV
#define USE_FUNC_TK_REF_POR
#define USE_FUNC_RENDEZVOUS_INITIALIZE
#define USE_FUNC_PORCB_TABLE
#define USE_FUNC_WSPEC_CAL
#define USE_FUNC_WSPEC_RDV
#if USE_DBGSPT
#define USE_FUNC_TD_LST_POR
#define USE_FUNC_TD_REF_POR
#define USE_FUNC_TD_CAL_QUE
#define USE_FUNC_TD_ACP_QUE
#define USE_FUNC_RENDEZVOUS_GETNAME
#endif
#endif

#define USE_FUNC_HLL_INTHDR
#define USE_FUNC_TK_DEF_INT
#define USE_FUNC_TK_RET_INT

#if CFN_MAX_MPLID > 0
#define USE_FUNC_TK_CRE_MPL
#define USE_FUNC_TK_DEL_MPL
#define USE_FUNC_TK_GET_MPL
#define USE_FUNC_TK_REL_MPL
#define USE_FUNC_TK_REF_MPL
#define USE_FUNC_MEMORYPOOL_INITIALIZE
#define USE_FUNC_MPLCB_TABLE
#define USE_FUNC_MPL_WAKEUP
#if USE_DBGSPT
#define USE_FUNC_TD_LST_MPL
#define USE_FUNC_TD_REF_MPL
#define USE_FUNC_TD_MPL_QUE
#define USE_FUNC_MEMORYPOOL_GETNAME
#endif
#endif

#if CFN_MAX_MPFID > 0
#define USE_FUNC_TK_CRE_MPF
#define USE_FUNC_TK_DEL_MPF
#define USE_FUNC_TK_GET_MPF
#define USE_FUNC_TK_REL_MPF
#define USE_FUNC_TK_REF_MPF
#define USE_FUNC_FIX_MEMORYPOOL_INITIALIZE
#define USE_FUNC_MPFCB_TABLE
#if USE_DBGSPT
#define USE_FUNC_TD_LST_MPF
#define USE_FUNC_TD_REF_MPF
#define USE_FUNC_TD_MPF_QUE
#define USE_FUNC_FIX_MEMORYPOOL_GETNAME
#endif
#endif

#define USE_FUNC_TK_SET_TIM
#define USE_FUNC_TK_GET_TIM
#define USE_FUNC_TK_GET_OTM
#define USE_FUNC_TK_DLY_TSK

#if CFN_MAX_CYCID > 0
#define USE_FUNC_TK_CRE_CYC
#define USE_FUNC_TK_DEL_CYC
#define USE_FUNC_TK_STA_CYC
#define USE_FUNC_TK_STP_CYC
#define USE_FUNC_TK_REF_CYC
#define USE_FUNC_CYCLICHANDLER_INITIALIZE
#define USE_FUNC_CYCCB_TABLE
#define USE_FUNC_CALL_CYCHDR
#if USE_DBGSPT
#define USE_FUNC_TD_LST_CYC
#define USE_FUNC_TD_REF_CYC
#define USE_FUNC_CYCLICHANDLER_GETNAME
#endif
#endif

#if CFN_MAX_ALMID > 0
#define USE_FUNC_TK_CRE_ALM
#define USE_FUNC_TK_DEL_ALM
#define USE_FUNC_TK_STA_ALM
#define USE_FUNC_TK_STP_ALM
#define USE_FUNC_TK_REF_ALM
#define USE_FUNC_ALARMHANDLER_INITIALIZE
#define USE_FUNC_ALMCB_TABLE
#define USE_FUNC_CALL_ALMHDR
#if USE_DBGSPT
#define USE_FUNC_TD_LST_ALM
#define USE_FUNC_TD_REF_ALM
#define USE_FUNC_ALARMHANDLER_GETNAME
#endif
#endif

#define USE_FUNC_TK_REF_VER
#define USE_FUNC_TK_REF_SYS
#define USE_FUNC_LOWPOW_DISCNT

#if CFN_MAX_SSYID > 0
#define USE_FUNC_TK_DEF_SSY
#define USE_FUNC_TK_REF_SSY
#define USE_FUNC_SUBSYSTEM_INITIALIZE
#define USE_FUNC_SSYCB_TABLE
#define USE_FUNC_SVC_IENTRY
#if USE_DBGSPT
#define USE_FUNC_TD_LST_SSY
#define USE_FUNC_TD_REF_SSY
#endif
#endif

#if CFN_MAX_REGDEV > 0
#define USE_FUNC_TK_OPN_DEV
#define USE_FUNC_TK_CLS_DEV
#define USE_FUNC_TK_REA_DEV
#define USE_FUNC_TK_SREA_DEV
#define USE_FUNC_TK_WRI_DEV
#define USE_FUNC_TK_SWRI_DEV
#define USE_FUNC_TK_WAI_DEV
#define USE_FUNC_TK_SUS_DEV
#define USE_FUNC_TK_GET_DEV
#define USE_FUNC_TK_REF_DEV
#define USE_FUNC_TK_OREF_DEV
#define USE_FUNC_TK_LST_DEV
#define USE_FUNC_TK_EVT_DEV
#define USE_FUNC_TK_DEF_DEV
#define USE_FUNC_TK_REF_IDV
#define USE_FUNC_INITIALIZE_DEVMGR
#define USE_FUNC_FINISH_DEVMGR
#define USE_FUNC_SEARCHDEVCB
#define USE_FUNC_GETRESCB
#define USE_FUNC_CHECK_DEVDESC
#define USE_FUNC_DELOPNCB
#define USE_FUNC_DELREQCB
#define USE_FUNC_CHKOPEN
#define USE_FUNC_CLOSE_DEVICE
#define USE_FUNC_DEVMGR_STARTUP
#define USE_FUNC_DEVMGR_CLEANUP
#define USE_FUNC_INITDEVIO
#define USE_FUNC_FINISHDEVIO
#define USE_FUNC_REQUEST
#define USE_FUNC_DEVCBTBL
#define USE_FUNC_OPNCBTBL
#define USE_FUNC_REQCBTBL
#define USE_FUNC_RESOURCE_CONTROL_BLOCK
#define USE_FUNC_DISSUSCNT
#define USE_FUNC_DEVMGRLOCK
#define USE_FUNC_DEFAULTIDEV
#define USE_FUNC_PHYDEVNM
#endif

#if USE_DBGSPT
/* Refer each object usage state */
#define USE_FUNC_TD_LST_TSK
#define USE_FUNC_TD_REF_TSK
#define USE_FUNC_TD_INF_TSK
#define USE_FUNC_TD_GET_REG
#define USE_FUNC_TD_SET_REG
#define USE_FUNC_TASK_GETNAME

/* Refer system state */
#define USE_FUNC_TD_REF_SYS
#define USE_FUNC_TD_GET_TIM
#define USE_FUNC_TD_GET_OTM

/* Refer ready queue */
#define USE_FUNC_TD_RDY_QUE

/* Execution trace */
#if USE_HOOK_TRACE
#define USE_FUNC_TD_HOK_SVC
#define USE_FUNC_TD_HOK_DSP
#define USE_FUNC_TD_HOK_INT
#define USE_FUNC_HOOK_ENTERFN
#define USE_FUNC_HOOK_EXECFN
#define USE_FUNC_HOOK_IENTERFN
#endif

/* Object name */
#define USE_FUNC_OBJECT_GETNAME
#define USE_FUNC_TD_REF_DSNAME
#define USE_FUNC_TD_SET_DSNAME

#endif /* USE_DBGSPT */

/*
 * Other functions
 */

/* tstdlib */
#define USE_FUNC_TSTDLIB_BITCLR
#define USE_FUNC_TSTDLIB_BITSET
#define USE_FUNC_TSTDLIB_BITSEARCH1


/* other functions */
#define USE_FUNC_WSPEC_SLP

/* task concerning */
#define USE_FUNC_CTXTSK
#define USE_FUNC_TCB_TABLE
#define USE_FUNC_TASK_INITIALIZE
#define USE_FUNC_MAKE_DORMANT
#define USE_FUNC_MAKE_READY
#define USE_FUNC_MAKE_NON_READY
#define USE_FUNC_CHANGE_TASK_PRIORITY
#define USE_FUNC_ROTATE_READY_QUEUE
#define USE_FUNC_ROTATE_READY_QUEUE_RUN
#define USE_FUNC_DEL_TSK
#define USE_FUNC_TER_TSK

/* wait concerning */
#define USE_FUNC_WAIT_RELEASE_OK
#define USE_FUNC_WAIT_RELEASE_OK_ERCD
#define USE_FUNC_WAIT_RELEASE_NG
#define USE_FUNC_WAIT_RELEASE_TMOUT
#define USE_FUNC_MAKE_WAIT
#define USE_FUNC_MAKE_WAIT_RELTIM
#define USE_FUNC_WAIT_DELETE
#define USE_FUNC_WAIT_TSKID
#define USE_FUNC_GCB_MAKE_WAIT
#define USE_FUNC_GCB_CHANGE_PRIORITY
#define USE_FUNC_GCB_TOP_OF_WAIT_QUEUE

/*  */
#define USE_FUNC_SET_REG
#define USE_FUNC_GET_REG

/* memory concerning */
#define USE_FUNC_APPENDFREEAREABOUND
#define USE_FUNC_GET_BLK
#define USE_FUNC_REL_BLK
#define USE_FUNC_SEARCHFREEAREA
#define USE_FUNC_APPENDFREEAREA
#define USE_FUNC_REMOVEFREEQUE
#define USE_FUNC_INSERTAREAQUE
#define USE_FUNC_REMOVEAREAQUE
#if USE_IMALLOC
#define USE_FUNC_IMACB
#define USE_FUNC_IMALLOC
#define USE_FUNC_ICALLOC
#define USE_FUNC_IFREE
#define USE_FUNC_INIT_IMALLOC
#endif /* USE_IMALLOC */

#endif /* _UTK_CONFIG_FUNC_H_ */
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
 *	wait.c
 *	Common Routine for Synchronization
 */

/** [BEGIN Common Definitions] */
#include "kernel.h"
#include "task.h"
#include "wait.h"
/** [END Common Definitions] */

#ifdef USE_FUNC_WAIT_RELEASE_OK
EXPORT void knl_wait_release_ok( TCB *tcb )
{
	knl_wait_release(tcb);
	*tcb->wercd = E_OK;
}
#endif /* USE_FUNC_WAIT_RELEASE_OK */

#ifdef USE_FUNC_WAIT_RELEASE_OK_ERCD
EXPORT void knl_wait_release_ok_ercd( TCB *tcb, ER ercd )
{
	knl_wait_release(tcb);
	*tcb->wercd = ercd;
}
#endif /* USE_FUNC_WAIT_RELEASE_OK_ERCD */

#ifdef USE_FUNC_WAIT_RELEASE_NG
EXPORT void knl_wait_release_ng( TCB *tcb, ER ercd )
{
	knl_wait_release(tcb);
	if ( tcb->wspec->rel_wai_hook != NULL ) {
		(*tcb->wspec->rel_wai_hook)(tcb);
	}
	*tcb->wercd = ercd;
}
#endif /* USE_FUNC_WAIT_RELEASE_NG */

#ifdef USE_FUNC_WAIT_RELEASE_TMOUT
EXPORT void knl_wait_release_tmout( TCB *tcb )
{
	QueRemove(&tcb->tskque);
	knl_make_non_wait(tcb);
	if ( tcb->wspec->rel_wai_hook != NULL ) {
		(*tcb->wspec->rel_wai_hook)(tcb);
	}
}
#endif /* USE_FUNC_WAIT_RELEASE_TMOUT */

#ifdef USE_FUNC_MAKE_WAIT
/*
 * Change the active task state to wait state and connect to the
 * timer event queue.
 *	Normally, 'knl_ctxtsk' is in the RUN state, but when an interrupt
 *	occurs during executing system call, 'knl_ctxtsk' may become the
 *	other state by system call called in the interrupt handler.
 *	However, it does not be in WAIT state.
 *
 *	"include/tk/tk_typedef.h"
 *	typedef	W		TMO;
 *	typedef UW		RELTIM;
 *	#define TMO_FEVR	(-1)
 */
EXPORT void knl_make_wait( TMO tmout, ATR atr )
{
	switch ( knl_ctxtsk->state ) {
	  case TS_READY:
		knl_make_non_ready(knl_ctxtsk);
		knl_ctxtsk->state = TS_WAIT;
		break;
	  case TS_SUSPEND:
		knl_ctxtsk->state = TS_WAITSUS;
		break;
	}
	knl_timer_insert(&knl_ctxtsk->wtmeb, tmout, (CBACK)knl_wait_release_tmout, knl_ctxtsk);
}
#endif /* USE_FUNC_MAKE_WAIT */

#ifdef USE_FUNC_MAKE_WAIT_RELTIM
EXPORT void knl_make_wait_reltim( RELTIM tmout, ATR atr )
{
	switch ( knl_ctxtsk->state ) {
	  case TS_READY:
		knl_make_non_ready(knl_ctxtsk);
		knl_ctxtsk->state = TS_WAIT;
		break;
	  case TS_SUSPEND:
		knl_ctxtsk->state = TS_WAITSUS;
		break;
	}
	knl_timer_insert_reltim(&knl_ctxtsk->wtmeb, tmout, (CBACK)knl_wait_release_tmout, knl_ctxtsk);
}
#endif /* USE_FUNC_MAKE_WAIT_RELTIM */

#ifdef USE_FUNC_WAIT_DELETE
/*
 * Release all tasks connected to the wait queue, and define it
 * as E_DLT error.
 */
EXPORT void knl_wait_delete( QUEUE *wait_queue )
{
	TCB	*tcb;

	while ( !isQueEmpty(wait_queue) ) {
		tcb = (TCB*)wait_queue->next;
		knl_wait_release(tcb);
		*tcb->wercd = E_DLT;
	}
}
#endif /* USE_FUNC_WAIT_DELETE */

#ifdef USE_FUNC_WAIT_TSKID
/*
 * Get ID of the head task in the wait queue.
 */
EXPORT ID knl_wait_tskid( QUEUE *wait_queue )
{
	if ( isQueEmpty(wait_queue) ) {
		return 0;
	}

	return ((TCB*)wait_queue->next)->tskid;
}
#endif /* USE_FUNC_WAIT_TSKID */

#ifdef USE_FUNC_GCB_MAKE_WAIT
/*
 * Change the active task state to wait state and connect to the timer wait 
 * queue and the object wait queue. Also set 'wid' in 'knl_ctxtsk'.
 */
EXPORT void knl_gcb_make_wait( GCB *gcb, TMO tmout )
{
	*knl_ctxtsk->wercd = E_TMOUT;
	if ( tmout != TMO_POL ) {
		knl_ctxtsk->wid = gcb->objid;
		knl_make_wait(tmout, gcb->objatr);
		if ( (gcb->objatr & TA_TPRI) != 0 ) {
			knl_queue_insert_tpri(knl_ctxtsk, &gcb->wait_queue);
		} else {
			QueInsert(&knl_ctxtsk->tskque, &gcb->wait_queue);
		}
	}
}
#endif /* USE_FUNC_GCB_MAKE_WAIT */

#ifdef USE_FUNC_GCB_CHANGE_PRIORITY
/*
 * When the task priority changes, adjust the task position at the wait queue.
 * It is called only if the object attribute TA_TPRI is specified.
 *
 */
EXPORT void knl_gcb_change_priority( GCB *gcb, TCB *tcb )
{
	QueRemove(&tcb->tskque);
	knl_queue_insert_tpri(tcb, &gcb->wait_queue);
}
#endif /* USE_FUNC_GCB_CHANGE_PRIORITY */

#ifdef USE_FUNC_GCB_TOP_OF_WAIT_QUEUE
/*
 * Search the first task of wait queue include "tcb" with target.
 * (Not insert "tcb" into wait queue.)
 *
 */
EXPORT TCB* knl_gcb_top_of_wait_queue( GCB *gcb, TCB *tcb )
{
	TCB	*q;

	if ( isQueEmpty(&gcb->wait_queue) ) {
		return tcb;
	}

	q = (TCB*)gcb->wait_queue.next;
	if ( (gcb->objatr & TA_TPRI) == 0 ) {
		return q;
	}

	return ( tcb->priority < q->priority )? tcb: q;
}
#endif /* USE_FUNC_GCB_TOP_OF_WAIT_QUEUE */
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
 *	wait.h
 *	Definition of Common Routine for Synchronization
 */

#ifndef _WAIT_
#define _WAIT_

#include <queue.h>
#include "timer.h"
#include "task.h"

/*
 * Release wait state of the task.
 *	Remove the task from the timer queue and the wait queue, then
 *	update the task state. 'wait_release_ok' sends E_OK to the
 *	wait released task.
 *	'wait_release_ok_recd' is normal wait release as well as
 *	'wait_release_ok', but it sends 'ercd' to the wait released task.
 *	It needs to be ercd >= 0. 
 *	'wait_release_ng' sends 'ercd' to the wait released task. Use for
 *	releasing the forced wait task. It needs to be ercd < 0.
 *	'wait_release_tmout' don't remove from the timer queue. Use for
 *	time out processing.
 */
IMPORT void knl_wait_release_ok( TCB *tcb );
IMPORT void knl_wait_release_ok_ercd( TCB *tcb, ER ercd );
IMPORT void knl_wait_release_ng( TCB *tcb, ER ercd );
IMPORT void knl_wait_release_tmout( TCB *tcb );

/*
 * Cancel task wait state.
 *	Remove the task from the timer queue and the wait queue.
 *	Do not update the task state.
 */
void knl_wait_cancel( TCB *tcb )
{
	knl_timer_delete(&tcb->wtmeb);
	QueRemove(&tcb->tskque);
}

/*
 * Change the active task to wait state and connect to the
 * timer event queue.
 */
IMPORT void knl_make_wait( TMO tmout, ATR atr );
IMPORT void knl_make_wait_reltim( RELTIM tmout, ATR atr );

/*
 * Release wait state of all tasks connected to the wait queue,
 * and set it as E_DLT error.
 * Use when synchronization between tasks or communication
 * object is deleted.
 */
IMPORT void knl_wait_delete( QUEUE *wait_queue );

/*
 * Get ID of the head task in the wait queue.
 */
IMPORT ID knl_wait_tskid( QUEUE *wait_queue );

/*
 * Connect the task to the prioritized wait queue.
 */
void knl_queue_insert_tpri( TCB *tcb, QUEUE *queue )
{
	QUEUE *q;
	QUEUE *start, *end;
	UB val;
	W offset;

	start = end = queue;
	val = tcb->priority;
	offset = offsetof(TCB, priority);

	for ( q = start->next; q != end; q = q->next ) {
		if ( *(UB*)((VB*)q + offset) > val ) {
			break;
		}
	}

	QueInsert(&tcb->tskque, q);
}

/*
 * Common part of control block
 *	For synchronization between tasks and communication object,
 *	the head part of control block is common. The followings are
 *	common routines.
 *	Define common part as GCB (generic control block) type.
 *	Cannot use these routines if an object has multiple wait queues
 *	and when it operates a wait queue after the first one.
 *	Cannot use these routines if TA_TPRI, TA_NODISWAI object attribute
 *	bits are used for other purposes since these bits are checked.
 */
typedef struct generic_control_block {
	QUEUE	wait_queue;	/* Wait queue */
	ID	objid;		/* Object ID */
	void	*exinf;		/* Extended information */
	ATR	objatr;		/* Object attribute */
	/* It is OK to have another field after this point, */
	/* but it is not used for generic operation routines. */
} GCB ;

/*
 * Change the active task to wait state and connect to the timer event 
 * queue and the object wait queue. Also, set 'wid' in 'ctxtsk'. 
 */
IMPORT void knl_gcb_make_wait( GCB *gcb, TMO tmout );

/*
 * When the task priority changes, adjust the task position in the
 * wait queue.
 * Do nothing if TA_TPRI is not specified in the object attribute.
 */
IMPORT void knl_gcb_change_priority( GCB *gcb, TCB *tcb );

/*
 * Search the first task of wait queue include "tcb" with target.
 * (Not insert "tcb" into wait queue.)
 *
 */
IMPORT TCB* knl_gcb_top_of_wait_queue( GCB *gcb, TCB *tcb );

/*
 * Update the task state to release wait. When it becomes ready state,
 * connect to the ready queue.
 * Call when the task is in the wait state (including double wait).
 */
void knl_make_non_wait( TCB *tcb )
{
	if ( tcb->state == TS_WAIT ) {
		knl_make_ready(tcb);
	} else {
		tcb->state = TS_SUSPEND;
	}
}

/*
 * Release wait state of the task.
 */
void knl_wait_release( TCB *tcb )
{
	knl_timer_delete(&tcb->wtmeb);
	QueRemove(&tcb->tskque);
	knl_make_non_wait(tcb);
}

#endif /* _WAIT_ */
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
 *	winfo.h
 *	Definition of Wait Information for Synchronization/Communication Object
 */

#ifndef _WINFO_
#define _WINFO_

/*
 * Semaphore wait (TTW_SEM)
 */
typedef struct {
	INT	cnt;		/* Request resource number */
} WINFO_SEM;

/*
 * Event flag wait (TTW_FLG)
 */
typedef struct {
	UINT	waiptn;		/* Wait bit pattern */
	UINT	wfmode;		/* Wait mode */
	UINT	*p_flgptn;	/* Address that has a bit pattern
				   at wait released */
} WINFO_FLG;

/*
 * Mailbox wait (TTW_MBX)
 */
typedef struct {
	T_MSG	**ppk_msg;	/* Address that has the head of a
				   message packet */
} WINFO_MBX;

/*
 * Message buffer receive/send wait (TTW_RMBF, TTW_SMBF)
 */
typedef struct {
	void	*msg;		/* Address that has a received message */
	INT	*p_msgsz;	/* Address that has a received message size */
} WINFO_RMBF;

typedef struct {
	CONST void *msg;	/* Send message head address */
	INT	msgsz;		/* Send message size */
} WINFO_SMBF;

/*
 * Rendezvous call/accept/end wait (TTW_CAL, TTW_ACP, TTW_RDV)
 */
typedef struct {
	UINT	calptn;		/* Bit pattern that indicates caller
				   select condition */
	void	*msg;		/* Address that has a message */
	INT	cmsgsz;		/* Call message size */
	INT	*p_rmsgsz;	/* Address that has a reply message size */
} WINFO_CAL;

typedef struct {
	UINT	acpptn;		/* Bit pattern that indicates receiver
				   select condition */
	void	*msg;		/* Address that has a call message */
	RNO	*p_rdvno;	/* Address that has the rendezvous number */
	INT	*p_cmsgsz;	/* Address that has the call message size */
} WINFO_ACP;

typedef struct {
	RNO	rdvno;		/* Rendezvous number */
	void	*msg;		/* Address that has a message */
	INT	maxrmsz;	/* Maximum length of reply message */
	INT	*p_rmsgsz;	/* Address that has a reply message size */
} WINFO_RDV;

/*
 * Variable size memory pool wait (TTW_MPL)
 */
typedef struct {
	W	blksz;		/* Memory block size */
	void	**p_blk;		/* Address that has the head of a
				   memory block */
} WINFO_MPL;

/*
 * Fixed size memory pool wait (TTW_MPF)
 */
typedef struct {
	void	**p_blf;		/* Address that has the head of a
				   memory block */
} WINFO_MPF;

/*
 * Definition of wait information in task control block
 */
typedef union {
#if CFN_MAX_SEMID > 0
	WINFO_SEM	sem;
#endif
#if CFN_MAX_FLGID > 0
	WINFO_FLG	flg;
#endif
#if CFN_MAX_MBXID > 0
	WINFO_MBX	mbx;
#endif
#if CFN_MAX_MBFID > 0
	WINFO_RMBF	rmbf;
	WINFO_SMBF	smbf;
#endif
#if CFN_MAX_PORID > 0
	WINFO_CAL	cal;
	WINFO_ACP	acp;
	WINFO_RDV	rdv;
#endif
#if CFN_MAX_MPLID > 0
	WINFO_MPL	mpl;
#endif
#if CFN_MAX_MPFID > 0
	WINFO_MPF	mpf;
#endif
} WINFO;

/*
 * Definition of wait specification structure
 */
typedef struct {
	UW	tskwait;			/* Wait factor */
	void	(*chg_pri_hook)(TCB *, INT);	/* Process at task priority
						   change */
	void	(*rel_wai_hook)(TCB *);		/* Process at task wait
						   release */
} WSPEC;

#endif /* _WINFO_ */
