/* ppcDecTimer.h - PowerPC Timer header */

/* Copyright 1984-1996 Wind River Systems, Inc. */

/*
modification history
--------------------
01c,27apr95,caf  changed "60x" to "Dec", removed PPC60x_TIMER_INT_VECTOR macro.
01b,25jan95,vin	 cleanup.
01a,20jan95,kvk  created.
*/

/*
DESCRIPTION
This file contains header information for PowerPC timer routines.
*/

#ifndef __INCppcDecTimerh
#define __INCppcDecTimerh

#ifdef __cplusplus
extern "C" {
#endif

/* function declarations */

#ifndef	_ASMLANGUAGE
#if defined(__STDC__) || defined(__cplusplus)

IMPORT	STATUS	sysClkConnect (FUNCPTR routine, int arg);
IMPORT	void	sysClkDisable (void);
IMPORT	void	sysClkEnable (void);
IMPORT	int	sysClkRateGet (void);
IMPORT	STATUS	sysClkRateSet (int ticksPerSecond);

#else	/* __STDC__ */

IMPORT	STATUS	sysClkConnect ();
IMPORT	void	sysClkDisable ();
IMPORT	void	sysClkEnable ();
IMPORT	int	sysClkRateGet ();
IMPORT	STATUS	sysClkRateSet ();

#endif	/* __STDC__ */

#endif	/* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif /* __INCppcDecTimerh */
