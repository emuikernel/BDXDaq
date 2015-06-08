/* CIOTimer.h - header for Zilog CIO timer driver */

/* Copyright 1984-2002, Wind River Systems, Inc. */

/*
modification history
--------------------
01b,26apr02,dat  Adding cplusplus protection, SPR 75017
01a,18feb94,gah Created to allow the use of (prototyped) functions outside of
		sysLib.c
*/

#ifndef __INCCIOTimerh
#define __INCCIOTimerh

#ifdef __cplusplus
extern "C" {
#endif

/* Function prototypes: */

void    sysCioInt (void);
void    sysClkInt (void);
STATUS  sysClkConnect (
	FUNCPTR routine,    /* routine called at each system clock interrupt */
    int arg        		/* argument with which to call routine */
	);
void    sysClkDisable (void);
void    sysClkEnable (void);
int     sysClkRateGet (void);   /* Now returns int, not UINT32. gah, 2/2/94 */
STATUS  sysClkRateSet (
	int ticksPerSecond  /* number of clock interrupts per second */
	);
void    sysAuxClkInt (void);
STATUS  sysAuxClkConnect (
    FUNCPTR routine,    /* routine called at each aux. clock interrupt */
    int     arg         /* argument with which to call routine         */
	);
void    sysAuxClkDisable (void);
void    sysAuxClkEnable (void);
int     sysAuxClkRateGet (void); /* Now returns int, not UINT32. gah, 2/2/94 */
STATUS  sysAuxClkRateSet (
	int ticksPerSecond  /* number of clock interrupts per second */
	);
void    sysHwInit2 (void);

#ifdef __cplusplus
}
#endif

#endif /* __INCCIOTimerh */
