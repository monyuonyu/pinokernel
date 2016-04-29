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
	ercd = E_OK;

	/* Platform dependent initialize sequence */
	ercd = knl_init_device();

	/* Initialize Imalloc */
#if USE_IMALLOC
	if ( ercd >= E_OK ) {
		ercd = knl_init_Imalloc();
	}
#endif /* USE_IMALLOC */

	if (ercd < E_OK) {
#if USE_KERNEL_MESSAGE
		tm_putstring((UB*)"!ERROR! init_kernel\n");
#endif
		tm_monitor(); /* Stop */
	}

	return;
}

/*
 * Start system
 */
EXPORT void knl_start_system( void )
{
	ER	ercd;
	ercd = E_OK;

#if CFN_MAX_REGDEV > 0
	/* Initialize Device manager */
	ercd = knl_initialize_devmgr();
#endif

	/* Start system dependent sequence */
	if ( ercd >= E_OK ) {
		ercd = knl_start_device();
	}

	if (ercd < E_OK) {
#if USE_KERNEL_MESSAGE
		tm_putstring((UB*)"!ERROR! start_system\n");
#endif
		tm_monitor();	/* Stop */
	}

	return;
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
