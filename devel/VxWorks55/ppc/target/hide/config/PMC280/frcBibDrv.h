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

 * frcBibDrv.h - header file for Board Information Block Driver */

/*
################################################################################
#
#  Project:	The Board Information Block (BIB)
#
#  Portability Status:
#		FULLY PORTABLE
#
################################################################################
#
#  $Id: frcBibDrv.h,v 1.2 2002/12/02 11:56:52 champ Exp $
#
#		Boardlevel Software Group
#		FORCE COMPUTERS GmbH
#		Prof.-Messerschmitt-Strasse 1
#	D-85579	Neubiberg/Muenchen
#		Germany
#
#	Remarks: Note! This material is proprietary of FORCE COMPUTERS and may
#	only be viewed under the terms of a signed non-disclosure agreement,
#	or a source code license agreement!
#	The information in this document is subject to change without notice
#	and should not be construed as a commitment by FORCE COMPUTERS.
#	Neither FORCE COMPUTERS, nor the authors assume any responsibility for
#	the use or reliability of this document or the described software.
#
#	Copyright (C) 1999-2002, FORCE COMPUTERS GmbH. All rights reserved.
#
################################################################################

modification history:
=====================
01e,04jun2002,rgr	Added inclusion of header file <stdio.h>.
01d,27may2002,rgr	Implemented new driver API, all routines begin with
			frcBibXxx now, defined structure BIB_INTF.
			Defined OLD_BIB_DRV_INTF to include old driver API.
			Defined macro GET_ERRNO, added fct ptr <frcBibShowRtn>.
			Added error codes NOT_ENOUGH_MEMORY, READ_INTF_ERROR,
			and WRITE_INTF_ERROR.
01c,17may2002,rgr	Added macro IMPORT_FRC_MOD_NUM_BASE to import or define
			the global variable <frcModNumBase>.
01b,15feb2000,fxs	Added Prototypes for allocation callout fct. pointers
01a,20jan1999,rage	Initial version, supports BIB version 2.0.

DESCRIPTION:
This header file contains the prototypes for the interface routines
(call-in and call-out functions) of the BIB driver.
*/


#ifndef	__INCfrcBibDrvh
#define	__INCfrcBibDrvh

#ifndef	_ASMLANGUAGE

#include "stdio.h"

/*=============================== DEFINES ==============================*/

#ifndef	OK
#define	OK		0
#endif

#ifndef	ERROR
#define	ERROR		(-1)
#endif

    /* switch whether to provide old BIB driver interface */

#ifndef	OLD_BIB_DRV_INTF
#define	OLD_BIB_DRV_INTF	FALSE
#endif

    /* default routines for memory allocation */

#ifndef	BIB_DRV_MALLOC
#define	BIB_DRV_MALLOC	malloc
#endif

#ifndef	BIB_DRV_FREE
#define	BIB_DRV_FREE	free
#endif

    /* macros for error number handling */

#ifndef	IMPORT_ERRNO
#define	IMPORT_ERRNO	extern int errno
#endif
#ifndef	SET_ERRNO
#define	SET_ERRNO(x)	errno = (x)
#endif
#ifndef	GET_ERRNO
#define	GET_ERRNO	errno
#endif

    /* module number for frcBibDrv */

#ifndef	M_frcBibDrv
#define	M_frcBibDrv	(0 << 16)
#endif

    /* macro to import or define the module number variable */

#ifndef	IMPORT_FRC_MOD_NUM_BASE
					/* do NOT initialize (must be 'bss') */
#define	IMPORT_FRC_MOD_NUM_BASE		int frcModNumBase
#endif

    /* error codes */

#define	S_frcBibDrv_INVALID_PARAMETER	(M_frcBibDrv |  1)
#define	S_frcBibDrv_ALREADY_ATTACHED	(M_frcBibDrv |  2)
#define	S_frcBibDrv_UNKNOWN_HANDLE	(M_frcBibDrv |  3)
#define	S_frcBibDrv_NO_BIB_FOUND	(M_frcBibDrv |  4)
#define	S_frcBibDrv_BIB_INVALID		(M_frcBibDrv |  5)
#define	S_frcBibDrv_INVALID_DIR		(M_frcBibDrv |  6)
#define	S_frcBibDrv_INVALID_DIR_ENTRY	(M_frcBibDrv |  7)
#define	S_frcBibDrv_CHKSUM_ERROR	(M_frcBibDrv |  8)
#define	S_frcBibDrv_ELEMENT_NOT_FOUND	(M_frcBibDrv |  9)
#define	S_frcBibDrv_BUFFER_TOO_SMALL	(M_frcBibDrv | 10)
#define	S_frcBibDrv_ELEMENT_TOO_LARGE	(M_frcBibDrv | 11)
#define	S_frcBibDrv_NOT_ENOUGH_MEMORY	(M_frcBibDrv | 12)
#define	S_frcBibDrv_READ_INTF_ERROR	(M_frcBibDrv | 13)
#define	S_frcBibDrv_WRITE_INTF_ERROR	(M_frcBibDrv | 14)

/*=============================== GLOBALS ==============================*/

IMPORT	void	*(*bibMallocRtn)(size_t size);
IMPORT	void	(*bibFreeRtn)	(void *ptr);
IMPORT	STATUS	(*frcBibShowRtn)(void *pImage, size_t imgSize,
				 BOOL verbose, FILE  *output);

/*=============================== TYPEDEFS =============================*/

#ifndef	GLOBAL
#define	GLOBAL
#endif

#ifndef	IMPORT
#define	IMPORT		extern
#endif

#ifndef	LOCAL
#define	LOCAL		static
#endif

#ifndef	STATUS
#define	STATUS		int
#endif

    /* default typedef for BIB handles (cookie) */

#ifndef	BIB_HDL
#define	BIB_HDL		UINT32
#endif

    /* BIB interface structure */

typedef struct
{
    /* interface routines to access this BIB device */
    STATUS (*bibReadIntfRtn) (BIB_HDL handle,  UINT32 offset,
			      UINT32  byteCnt, void  *pDstBuf);
    STATUS (*bibWriteIntfRtn)(BIB_HDL handle,  UINT32 offset,
			      UINT32  byteCnt, void  *pSrcBuf);

    /* optional local memory pool for this BIB */
    void    *pMemPool;		/* start address of local memory pool	*/
    size_t   memPoolSize;	/* pool size in bytes			*/
} BIB_INTF;

#ifdef	__cplusplus
IMPORT "C" {
#endif

/* function declarations */

#if	defined(__STDC__) || defined(__cplusplus)

    /* call-in functions provided by the driver */
IMPORT	STATUS	frcBibAttach	(BIB_HDL handle) ;
IMPORT	STATUS	frcBibDetach	(BIB_HDL handle);
IMPORT	STATUS	frcBibRead	(BIB_HDL handle,  int    element,
				 void   *pDstBuf, UINT32 bufSize);
IMPORT	STATUS	frcBibWrite	(BIB_HDL handle,  int    element,
				 void   *pSrcBuf);
IMPORT	STATUS	frcBibDrvShow	(int infoLvl);

#if	(OLD_BIB_DRV_INTF == TRUE)	/* include old BIB driver interface */
IMPORT	STATUS	bibAttach	(BIB_HDL handle);
IMPORT	STATUS	bibDetach	(BIB_HDL handle);
IMPORT	STATUS	bibRead		(BIB_HDL handle,  int    element,
				 void   *pDstBuf, UINT32 bufSize);
IMPORT	STATUS	bibWrite	(BIB_HDL handle,  int    element,
				 void   *pSrcBuf);
#endif
    /* call-out functions required by the driver */
IMPORT	STATUS	bibReadIntf	(BIB_HDL handle,  UINT32 offset,
				 UINT32  byteCnt, void  *pDstBuf);
IMPORT	STATUS	bibWriteIntf	(BIB_HDL handle,  UINT32 offset,
				 UINT32  byteCnt, void  *pSrcBuf);

#else   /* __STDC__ */

    /* call-in functions provided by the driver */
IMPORT	STATUS	frcBibAttach	();
IMPORT	STATUS	frcBibDetach	();
IMPORT	STATUS	frcBibRead	();
IMPORT	STATUS	frcBibWrite	();
IMPORT	STATUS	frcBibDrvShow	();

#if	(OLD_BIB_DRV_INTF == TRUE)	/* include old BIB driver interface */
IMPORT	STATUS	bibAttach	();
IMPORT	STATUS	bibDetach	();
IMPORT	STATUS	bibRead		();
IMPORT	STATUS	bibWrite	();

    /* call-out functions required by the driver */
IMPORT	STATUS	bibReadIntf	();
IMPORT	STATUS	bibWriteIntf	();
#endif

#endif	/* __STDC__ */

#ifdef	__cplusplus
}
#endif


#endif	/* _ASMLANGUAGE */

#endif	/* __INCfrcBibDrvh */

