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
 *    Clock Setting (MB9AF312K)
 */

#include <tk/tkernel.h>

#include "hwconfig.h"
#include "utk_config.h"

#include <machine.h>
#include <tk/sysdef.h>


/*
 * Clock Settings:
 * ---------------
 *	Crystal:  4 MHz
 *	HCLK:	 40 MHz
 *	PCLK0:	 20 MHz
 *	PCLK1:	 20 MHz
 *	PCLK2:	 20 MHz
 */
EXPORT void init_clock_control(void)
{
	UINT	u32RegisterRead;

#if (HWWD_DISABLE)				/* HW Watchdog Disable */
	// FM3_HWWDT_WDG_LCK = 0x1ACCE551;		/* HW Watchdog Unlock */
	// FM3_HWWDT_WDG_LCK = 0xE5331AAE;
	// FM3_HWWDT_WDG_CTL = 0;			/* HW Watchdog stop */
#endif

#if (CLOCK_SETUP)				/* Clock Setup */
	// FM3_CRG_BSC_PSR	  = BSC_PSR_Val;	/* set System Clock presacaler */
	// FM3_CRG_APBC0_PSR = APBC0_PSR_Val;	/* set APB0 presacaler */
	// FM3_CRG_APBC1_PSR = APBC1_PSR_Val;	/* set APB1 presacaler */
	// FM3_CRG_APBC2_PSR = APBC2_PSR_Val;	/* set APB2 presacaler */
	// FM3_CRG_SWC_PSR	  = SWC_PSR_Val | (1UL << 7); /* set SW Watchdog presacaler */
	// FM3_CRG_TTC_PSR	  = TTC_PSR_Val;	/* set Trace Clock presacaler */

	// FM3_CRG_CSW_TMR	  = CSW_TMR_Val;	/* set oscillation stabilization wait time */
	// if (SCM_CTL_Val & (1UL << 1)) {		/* Main clock oscillator enabled ? */
		// FM3_CRG_SCM_CTL |= (1UL << 1);	/* enable main oscillator */
		// while (!(FM3_CRG_SCM_STR & (1UL << 1))); /* wait for Main clock oscillation stable */
	// }
	// if (SCM_CTL_Val & (1UL << 3)) {		/* Sub clock oscillator enabled ? */
		// FM3_CRG_SCM_CTL |= (1UL << 3);	/* enable sub oscillator */
		// while (!(FM3_CRG_SCM_STR & (1UL << 3))); /* wait for Sub clock oscillation stable */
	// }

	// FM3_CRG_PSW_TMR	  = PSW_TMR_Val;	/* set PLL stabilization wait time */
	// FM3_CRG_PLL_CTL1  = PLL_CTL1_Val;	/* set PLLM and PLLK */
	// FM3_CRG_PLL_CTL2  = PLL_CTL2_Val;	/* set PLLN */
	// if (SCM_CTL_Val & (1UL << 4)) {		/* PLL enabled ? */
		// FM3_CRG_SCM_CTL |= (1UL << 4);	/* enable PLL */
		// while (!(FM3_CRG_SCM_STR & (1UL << 4))); /* wait for PLL stable */
	// }

	// FM3_CRG_SCM_CTL	 |= (SCM_CTL_Val & 0xE0); /* Set Master Clock switch */
	// do
	// {
		// u32RegisterRead = (FM3_CRG_SCM_CTL & 0xE0);
	// }while ((FM3_CRG_SCM_STR & 0xE0) != u32RegisterRead);
#endif

#if (CR_TRIM_SETUP)
	/* CR Trimming Data  */
	// if (0x000003FF != (FM3_FLASH_IF_CRTRMM & 0x000003FF)) {
		// /* UnLock (MCR_FTRM) */
		// FM3_CRTRIM_MCR_RLR = 0x1ACCE554;
		// /* Set MCR_FTRM */
		// FM3_CRTRIM_MCR_FTRM = FM3_FLASH_IF_CRTRMM;
		// /* Lock (MCR_FTRM) */
		// FM3_CRTRIM_MCR_RLR = 0x00000000;
	// }
#endif
}
