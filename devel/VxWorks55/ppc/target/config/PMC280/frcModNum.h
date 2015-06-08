/******************************************************************************
*              (c), Copyright, FORCE COMPUTERS INDIA Limited                  *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF FORCE COMPUTERS, INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF FORCE OR ANY THIRD PARTY. FORCE RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO FORCE COMPUTERS.       *
* THIS CODE IS PROVIDED "AS IS". FORCE COMPUTERS MAKES NO WARRANTIES, EXPRESS,*
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
*******************************************************************************

 * frcModNum.h - FORCE COMPUTERS module numbers */

/* Copyright 1997-2000 FORCE COMPUTERS */

/*
modification history
--------------------
01i,16oct01,alkn	Added a define to exclude a variable definition
01h,21mar01,fxs	Added module number M_frcDevLib.
01g,18jan01,fxs	Added module number M_frcBnEnd.
01f,03may00,fxs	Added module number M_frcTaLib.
01e,15feb00,fxs	Added module number M_frcPSpan.
		Added comments.
01d,29jun99,rgr Changed M_frcI2CLib to M_frcI2cBus.
01c,25jan99,kks Added module number M_frcEEPROM and M_frcBibDrv.
01b,28jan98,kks Added module number for the PCI Library (errorhandling)
01a,11oct97,rgr Created.
*/

/*
DESCRIPTION
This header file defines all module numbers for any FORCE COMPUTERS software
modules.  For default, the base of these module numbers is 0xFC00, as defined
by "M_frc".  If this is a problem, because your application already uses the
range 0xFC00 to 0xFCff for its own error codes, you may change the base in
the following way:

1) Redefine the "M_frc" define within this file.

	#define M_frc     (newBase << 16)

2) Let all FORCE COMPUTERS software modules know about this change at run-time
   by adding the following line within the BSP file "sysLib.c".

   Add variable to the global section:

	int frcModNumBase = M_frc;     /@ set new base of module numbers @/

3) Update the "statTbl[]" symbol table for the error number definitions.
   This is used by function "printErrno()".

    % cd target/config/force/src
    % make CPU=<cpu> statTbl

*/

#ifndef __INCfrcModNumh
#define __INCfrcModNumh

#ifdef __cplusplus
extern "C" {
#endif

/* base of all FORCE COMPUTERS module numbers */

#define	M_frc		(0xFC00 << 16)

/* FORCE COMPUTERS module numbers */

#define M_if_frcBn	(M_frc |  (1 << 16))	/* 0x00010000 */
#define M_frcFlash	(M_frc |  (2 << 16))	/* 0x00020000 */
#define M_frcI2cBus	(M_frc |  (3 << 16))	/* 0x00030000 */
#define M_frcPciLib	(M_frc |  (4 << 16))	/* 0x00040000 */
#define M_frcEEPROM	(M_frc |  (5 << 16))	/* 0x00050000 */
#define M_frcBibDrv	(M_frc |  (6 << 16))	/* 0x00060000 */
#define M_frcPSpan	(M_frc |  (7 << 16))	/* 0x00070000 */
#define M_frcTaLib	(M_frc |  (8 << 16))	/* 0x00080000 */
#define M_frcBnEnd	(M_frc |  (9 << 16))	/* 0x00090000 */
#define M_frcDevLib	(M_frc | (10 << 16))	/* 0x000a0000 */

/* 
 * WARNING: If you add a module number above please add the new module
 * error strings in the file: host/resource/tcl/errnoTbl.tcl so that
 * windsh knows about the new error codes.
 */

/* macro to set "errno" correctly - considers value of "frcModNumBase" */

#define	SET_ERRNO(x)	errno = (x) + frcModNumBase - M_frc

#ifndef _ASMLANGUAGE

/* import module number base */
#ifdef	CPU
int	frcModNumBase;		/* is set to 0xFC00 for default */
#endif

#endif	/* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif /* __INCfrcModNumh */
