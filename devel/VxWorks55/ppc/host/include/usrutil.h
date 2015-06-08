/* usrutil.h - header for user interface subroutines */

/* Copyright 1995-1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01d,21jan98,c_c  Removed unused routines.
01c,01mar95,pad  removed old style error codes. Removed printErrno().
01b,22jan94,jcf  removed version().
01a,05jan95,jcf  based on vxWorks usrLib.
*/

#ifndef __INCusrLibh
#define __INCusrLibh

#ifdef __cplusplus
extern "C" {
#endif

/* function declarations */

extern void 	help (void);
extern void 	printLogo (void);


#ifdef __cplusplus
}
#endif

#endif /* __INCusrLibh */
