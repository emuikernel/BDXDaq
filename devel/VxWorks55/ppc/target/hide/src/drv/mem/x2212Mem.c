/* x2212Mem.c - Xicor X2212 EEPROM non-volatile RAM library */

/* Copyright 1984-1992 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01d,28jan93,caf  made boot line begin at offset 0 (SPR #1933).
01c,16oct92,caf  fixed X2212_RECALL register access, fixed <offset> usage,
		 added read before write in sysNvRamSet(). (SPR 1651).
		 defined X2212_RAM_SIZE.
01b,02sep92,ccc  renamed x2212Lib.c to x2212Mem.c.
01a,26jun92,caf  derived from version 01c of hkv3d/sysLib.c.
*/

/*
DESCRIPTION
This library contains routines to manipulate Xicor X2212 EEPROM.
Read and write routines are included.

The Xicor X2212 EEPROM provides 128 bytes of non-volatile RAM.

The macros X2212_RAM, X2212_RECALL and X2212_STORE must be defined to
indicate the address of the X2212 RAM, the address of the recall operation,
and the address of the store operation, respectively.
*/

/* defines */

#define	X2212_RAM_SIZE	128

/******************************************************************************
*
* sysNvRamGet - get the contents of non-volatile RAM
*
* This routine copies the contents of non-volatile memory into a specified
* string.  The string will be terminated with an EOS.
*
* RETURNS: OK, or ERROR if access is outside the non-volatile RAM range.
*
* SEE ALSO: sysNvRamSet()
*
* INTERNAL
* If multiple tasks are calling sysNvRamSet() and sysNvRamGet(),
* they should use a semaphore to ensure mutually exclusive access to EEPROM.
*/

STATUS sysNvRamGet
    (
    char *string,    /* where to copy non-volatile RAM    */
    int strLen,      /* maximum number of bytes to copy   */
    int offset       /* byte offset into non-volatile RAM */
    )
    {
    FAST int		ix;
    FAST char *		pNvRam;
    volatile char	bitBucket;

    offset += NV_BOOT_OFFSET;	/* boot line begins at <offset> = 0 */

    if ((offset < 0) || (strLen < 0) || ((offset + strLen) > X2212_RAM_SIZE))
        return (ERROR);

    pNvRam = ((char *) X2212_RAM) + (2 * offset);

    /* read from NVRAM into shadow RAM */

    bitBucket = *((char *) X2212_RECALL);

    /* wait at least 1 microsecond for recall operation to complete */

    taskDelay (1);

    /*
     * Construct the string from NV nibbles.  Nibbles are ordered as:
     *
     * <lower addresses> (high nibble)(low nibble)... <higher addresses>
     *
     * Odd addresses contain low order nibbles, even addresses contain
     * high order nibbles.
     */

    for (ix = 0; ix < strLen; ix ++)
	{
	*string ++ = (*pNvRam << 4) | (*(pNvRam + 1) & 0xf);
	pNvRam += 2;
	}

    *string = EOS;

    return (OK);
    }

/*******************************************************************************
*
* sysNvRamSet - write to non-volatile RAM
*
* This routine copies a specified string into non-volatile RAM.
*
* RETURNS: OK, or ERROR if access is outside the non-volatile RAM range.
*
* SEE ALSO: sysNvRamGet()
*
* INTERNAL
* The XICOR X2212 EEPROM is rated for 10,000 store cycles, minimum.
* If multiple tasks are calling sysNvRamSet() and sysNvRamGet(),
* they should use a semaphore to ensure mutually exclusive access to EEPROM.
*/

STATUS sysNvRamSet
    (
    char *string,     /* string to be copied into non-volatile RAM */
    int strLen,       /* maximum number of bytes to copy           */
    int offset        /* byte offset into non-volatile RAM         */
    )
    {
    FAST int		ix;
    FAST char *		pNvRam;
    volatile char	bitBucket;

    offset += NV_BOOT_OFFSET;	/* boot line begins at <offset> = 0 */

    if ((offset < 0) || (strLen < 0) || ((offset + strLen) > X2212_RAM_SIZE))
        return (ERROR);

    pNvRam = ((char *) X2212_RAM) + (2 * offset);

    /* read from NVRAM into shadow RAM */

    bitBucket = *((char *) X2212_RECALL);

    /* wait at least 1 microsecond for recall operation to complete */

    taskDelay (1);

    /*
     * Construct the NV nibbles from string.  Nibbles are ordered as:
     *
     * <lower addresses> (high nibble)(low nibble)... <higher addresses>
     *
     * Odd addresses contain low order nibbles, even addresses contain
     * high order nibbles.
     */

    for (ix = 0; ix < strLen; ix ++)
	{
	*pNvRam ++ = (string [ix] >> 4) & 0xf;
	*pNvRam ++ = string [ix] & 0xf;
	}

    /* write from shadow RAM to NVRAM */

    (void) vxTas ((char *) X2212_STORE);

    /* wait at least 10 milliseconds for store operation to complete */

    taskDelay (sysClkRateGet() / 100 + 1);

    return (OK);
    }
