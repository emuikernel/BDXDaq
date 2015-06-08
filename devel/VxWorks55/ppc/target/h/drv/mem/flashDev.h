/* flashDev.h - generic FLASH memory header */

/* Copyright 1994-2000 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,27mar00,zl   added FLASH_CHIP_WIDTH and FLASH_POLL_DEF
01a,08jan94,dzb  created.
*/

/*
DESCRIPTION
This file contains header information for generic FLASH memory routines.
*/

#ifndef __INCflashDevh
#define __INCflashDevh

#ifdef __cplusplus
extern "C" {
#endif

/* defines */

#if     (FLASH_WIDTH == 1)
#define FLASH_DEF       UINT8
#define FLASH_CAST      (UINT8 *)
#endif  /* FLASH_WIDTH */

#if     (FLASH_WIDTH == 2)
#define FLASH_DEF       UINT16
#define FLASH_CAST      (UINT16 *)
#endif  /* FLASH_WIDTH */

#if     (FLASH_WIDTH == 4)
#define FLASH_DEF       UINT32
#define FLASH_CAST      (UINT32 *)
#endif  /* FLASH_WIDTH */

/* Define the width of the chip itself */

#ifndef FLASH_CHIP_WIDTH
#define FLASH_CHIP_WIDTH 1		/* default to 1 byte */
#endif  /* FLASH_CHIP_WIDTH */

#if     (FLASH_CHIP_WIDTH == 1)
#define FLASH_POLL_DEF		UINT8
#define FLASH_POLL_CAST		(UINT8 *)
#elif 	(FLASH_CHIP_WIDTH == 2)
#define FLASH_POLL_DEF		UINT16
#define FLASH_POLL_CAST		(UINT16 *)
#elif 	(FLASH_CHIP_WIDTH == 4)
#define FLASH_POLL_DEF		UINT32
#define FLASH_POLL_CAST		(UINT32 *)
#endif	/* FLASH_CHIP_WIDTH */

#define	FLASH_UNKNOWN	0

/* function declarations */

#ifndef	_ASMLANGUAGE
#if defined(__STDC__) || defined(__cplusplus)

IMPORT	STATUS	sysFlashGet (char *string, int strLen, int offset);
IMPORT	STATUS	sysFlashSet (char *string, int strLen, int offset);

#else	/* __STDC__ */

IMPORT	STATUS	sysFlashGet ();
IMPORT	STATUS	sysFlashSet ();

#endif	/* __STDC__ */

#endif	/* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif /* __INCflashDevh */
