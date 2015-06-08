/* iFlashMem.c - Intel 28F256A Flash Memory library */

/* Copyright 1984-1992 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01b,28jan93,caf  made boot line begin at offset 0 (SPR #1933).
01a,30oct92,ccc  created.
*/

/*
DESCRIPTION
This library contains routines to manipulate Intel 28F256A Flash memory.
Read and write routines are included.

The Intel 28F256A provides 32K bytes of non-volatile RAM, thus providing
64K bytes with 2 devices.

The macros NV_RAM_ADRS and NV_RAM_SIZE must be defined to indicate the
address and size of the Flash memory.

INTERNAL
The routine sysFlashDelay() is to delay 1uS.  It has not been timed, and
should be timed because faster boards may have a problem.
*/

#include "drv/mem/iFlash.h"
#include "memLib.h"
#include "stdlib.h"

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
* they should use a semaphore to ensure mutually exclusive access.
*/

STATUS sysNvRamGet
    (
    char *string,    /* where to copy non-volatile RAM    */
    int strLen,      /* maximum number of bytes to copy   */
    int offset       /* byte offset into non-volatile RAM */
    )
    {
    offset += NV_BOOT_OFFSET;   /* boot line begins at <offset> = 0 */

    if ((offset < 0) || (strLen < 0) || ((offset + strLen) > NV_RAM_SIZE))
        return (ERROR);

    bcopyBytes (NV_RAM_ADRS + offset, string, strLen);
    string [strLen] = EOS;

    return (OK);
    }

/******************************************************************************
*
* sysFlashDelay - delay for 1uS
*
* RETURNS: N/A
*
* NOMANUAL
*/

void sysFlashDelay
    (
    int delayCount	/* number of uS to delay */
    )
    {
    int ix;
    volatile int bump = 0;

    for (ix = 0; ix < delayCount; ix++)
	{
	bump++;
	}
    }

/******************************************************************************
*
* sysFlashErase - erase the Intel 28F256A
*
* RETURNS: N/A
*
* NOMANUAL
*/

void sysFlashErase (void)
    {
    int address;
    int ix = 0;
    volatile unsigned short * pFlashAdrs;

    for (address = 0; address < NV_RAM_SIZE; address += 2)
	{
	pFlashAdrs = (unsigned short *) (NV_RAM_ADRS + address);

	for (ix = 0; ix < 25; ix++)
	    {
	    *pFlashAdrs = FLASH_CMD_PROG_SETUP;	 /* write setup */
	    *pFlashAdrs = 0x0000; 		 /* data to write */
	    sysFlashDelay (10);			 /* wait for write */

	    *pFlashAdrs = FLASH_CMD_PROG_VERIFY; /* verify command */
	    sysFlashDelay (6);			 /* wait for verify */

	    if (*pFlashAdrs == 0)	/* done? */
		break;
	    }
	}

    /* now it is all zeros - time to erase */

    address = 0;
    pFlashAdrs = (unsigned short *) NV_RAM_ADRS;

    for (ix = 0; ix < 3000; ix++)
	{
	*pFlashAdrs = FLASH_CMD_ERASE_SETUP;	/* erase setup */
	*pFlashAdrs = FLASH_CMD_ERASE;		/* erase */
	sysFlashDelay (1000);

	while (address < NV_RAM_SIZE)
	    {
	    *pFlashAdrs = FLASH_CMD_ERASE_VERIFY; 	/* erase verify */
	    sysFlashDelay (6);				/* wait for verify */

	    if (*pFlashAdrs != 0xffff)	/* erased ? */
		break;

	    address += 2;				/* next address */
	    pFlashAdrs = (unsigned short *) (NV_RAM_ADRS + address);
	    }

	if (address == NV_RAM_SIZE)
	    break;				/* done */
	}

    pFlashAdrs = (unsigned short *) NV_RAM_ADRS;

    *pFlashAdrs = FLASH_CMD_RESET;
    *pFlashAdrs = FLASH_CMD_RESET;
    *pFlashAdrs = FLASH_CMD_READ_MEM;
    }

/******************************************************************************
*
* sysFlashWrite - write data to the Intel 28F256A Flash Memory
*
* RETURNS: N/A
*/

void sysFlashWrite
    (
    unsigned short *pFlashBuffer /* string to be copied */
    )
    {
    int address;
    int ix = 0;
    volatile unsigned short * pFlashAdrs;

    for (address = 0; address < NV_RAM_SIZE; address += 2)
	{
	pFlashAdrs = (unsigned short *) (NV_RAM_ADRS + address);

	for (ix = 0; ix < 25; ix++)
	    {
	    *pFlashAdrs = FLASH_CMD_PROG_SETUP;		/* write setup */
	    *pFlashAdrs = *pFlashBuffer;		/* data to write */
	    sysFlashDelay (10);				/* wait for write */

	    *pFlashAdrs = FLASH_CMD_PROG_VERIFY;	/* verify command */
	    sysFlashDelay (6);				/* wait for verify */

	    if (*pFlashAdrs == *pFlashBuffer)	/* done? */
		break;
	    }
	pFlashBuffer ++;
	}
    pFlashAdrs = (unsigned short *) NV_RAM_ADRS;

    *pFlashAdrs = FLASH_CMD_RESET;
    *pFlashAdrs = FLASH_CMD_RESET;
    *pFlashAdrs = FLASH_CMD_READ_MEM;
    }

/******************************************************************************
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
* The Intel 28F256 is rated for 10,000 erase/program cycles minimum.
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
    char *tempBuffer;	/* temp buffer for existing data */

    offset += NV_BOOT_OFFSET;   /* boot line begins at <offset> = 0 */

    if ((offset < 0) || (strLen < 0) || ((offset + strLen) > NV_RAM_SIZE))
        return (ERROR);

    /* see if contents are actually changing */
    if (bcmp (NV_RAM_ADRS + offset, string, strLen) == 0)
	return (OK);

    /* first read existing data */
    tempBuffer = (char *) malloc (NV_RAM_SIZE);

    bcopyBytes (NV_RAM_ADRS, tempBuffer, NV_RAM_SIZE);

    sysFlashErase ();	/* first erase device */

    bcopyBytes (string, tempBuffer + offset, strLen);

    sysFlashWrite ((unsigned short *) tempBuffer);	/* write new buffer */

    free (tempBuffer);	/* return memory */

    return (OK);
    }
