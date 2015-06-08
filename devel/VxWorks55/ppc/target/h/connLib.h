/* connLib.h - target buffer connection library header */

/* Copyright 1993 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,10dec93,smb  written.
*/

#ifndef __INCconnLibh
#define __INCconnLibh

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__STDC__) || defined(__cplusplus)

extern void connRtnSet (FUNCPTR initRtn, VOIDFUNCPTR closeRtn, 
			VOIDFUNCPTR errorRtn, VOIDFUNCPTR dataTransferRtn);

#else   /* __STDC__ */

extern void connRtnSet ();

#endif  /* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INCconnLibh */

