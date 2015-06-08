/* wvTmrLib.h - timer library header */

/* Copyright 1993 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,10dec93,smb  written.
*/

#ifndef __INCwvtmrlibh
#define __INCwvtmrlibh

/* typedefs */

typedef unsigned int             (*UINTFUNCPTR) ();

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__STDC__) || defined(__cplusplus)

void wvTmrRegister (UINTFUNCPTR wvTmrRtn, UINTFUNCPTR wvTmrLockRtn, 
		    FUNCPTR wvTmrEnable, FUNCPTR wvTmrDisable, 
		    FUNCPTR wvTmrConnect, UINTFUNCPTR wvTmrPeriod, 
		    UINTFUNCPTR wvTmrFreq);

#else   /* __STDC__ */

void wvTmrRegister ();

#endif  /* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INCwvtmrlibh*/

