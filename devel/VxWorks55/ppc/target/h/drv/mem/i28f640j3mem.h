/******************************************************************************
*
* Module: flash.h
*
* Description: Flash Module Definitions
*
* Project:     HSI VisionWare -- vWare
*
* Functions:
*
* Notes:
*
* History:		See Visual SourceSafe History
*
* Disclaimer: Copyright 1999-2000 Wind River Systems, Inc. All Rights Reserved
*
*******************************************************************************/
/* i28F640J3Mem.h - Intel 28F640J3 FLASH memory header */

/* Copyright 2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,16apr01,gh   created.
*/

/*
DESCRIPTION
This file contains header information for Intel 28F640J3 FLASH memory routines.
*/

#ifndef __INCi28F640J3Memh
#define __INCi28F640J3Memh

#ifdef __cplusplus
extern "C" {
#endif

/* typedefs */
typedef union tFLASH
{
	UINT8 Integer;
	UINT8 Float;
} tFLASH;

/* function declarations */

#ifndef	_ASMLANGUAGE
#if defined(__STDC__) || defined(__cplusplus)

IMPORT  STATUS  sysNvRamGet(char *string, int strLen, int offset);
IMPORT  STATUS  sysNvRamSet(char *string, int strLen, int offset);

#else	/* __STDC__ */

IMPORT  STATUS  sysNvRamGet();
IMPORT  STATUS  sysNvRamSet();

#endif	/* __STDC__ */

#endif	/* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif /* __INCi28F640J3Memh */
