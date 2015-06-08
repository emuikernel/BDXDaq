/* m8260Flash.c - Flash memory device driver for 28F016 flash devices */

/* Copyright 1984-1999 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01d,15jul99,ms_  make compliant with our coding standards
01c,05jun99,cn   added sysFlashUpdate, updated documentation, removed 
		 unnecessary code.
01b,28may99,ms_  get flash28.h locally, not from the driver include location
01a,05may99,cn   derived from version 01k of mem/flashMem.c.
*/

/*
DESCRIPTION
This library contains routines to manipulate the 28F016 flash memory.  Read 
and write routines are included.

The macro values FLASH_ADRS, FLASH_SIZE, and FLASH_WIDTH must be defined to
indicate the address, size (in bytes), and the data-access width (in bytes) of
the flash memory. Besides, the macro FLASH_SIZE_WRITEABLE should be defined
to a reasonable amount of flash-memory that needs to be modified. In fact, 
the user is not likely to need to change the entire contents of the device.

*/

#include "drv/mem/flash28.h"

/* defines */

#define FLASH_DEF       UINT32
#define FLASH_CAST      (UINT32 *)
#define FLASH_BLK_SZ	0x10000
#define FLASH_ALIGN_SZ (FLASH_BLK_SZ * FLASH_WIDTH)

#ifdef FLASH_SIZE_WRITEABLE
#   define FLASH_MEM_SIZE		FLASH_SIZE_WRITEABLE
#else
#   define FLASH_MEM_SIZE		FLASH_SIZE
#endif	/* FLASH_SIZE_WRITEABLE */

/* globals */

/* forward declarations */

STATUS	sysFlashErase (UINT32 offset);
STATUS	sysFlashWrite (FLASH_DEF * pFB, UINT32 size, UINT32 offset,
                       FLASH_DEF value);
STATUS	sysFlashGet (UCHAR * string, UINT32 strLen, UINT32 offset);
UINT8	sysFlashTypeGet (void);
STATUS	sysFlashUpdate (UCHAR * filename);

/******************************************************************************
*
* sysFlashGet - get the contents of flash memory
*
* This routine copies the contents of flash memory into a specified
* string.  The string is terminated with an EOS.
*
* RETURNS: OK, or ERROR if access is outside the flash memory range.
*
* SEE ALSO: sysFlashSet()
*
* INTERNAL
* If multiple tasks are calling sysFlashSet() and sysFlashGet(),
* they should use a semaphore to ensure mutually exclusive access.
*/

STATUS sysFlashGet
    (
    UCHAR	* string,	/* where to copy flash memory      */
    UINT32	  strLen,	/* maximum number of bytes to copy */
    UINT32	  offset	/* byte offset into flash memory   */
    )
    {
    if ((offset < 0) || (strLen < 0) || ((offset + strLen) > FLASH_MEM_SIZE))
        return (ERROR);

    bcopyBytes ((char *) (FLASH_ADRS + offset), string, strLen);
    string [strLen] = EOS;

    return (OK);
    }

/******************************************************************************
*
* sysFlashErase - erase the contents of flash memory
*
* This routine clears the contents of flash memory from the specified
* <offset> up to the top.
*
* Flash 28F016 devices support block erase mode. This means each of the 32
* 64-KByte blocks is erased by writing a flash erase command to
* the device at any address within the target block, and verifying that 
* the operation completed successfully.
*
* RETURNS: OK, or ERROR if the contents of flash memory cannot be erased.
*/

STATUS sysFlashErase
    (
    UINT32 offset		/* offset from flash base address */
    )
    {
    volatile FLASH_DEF * pFA = FLASH_CAST (offset + FLASH_ADRS);
    STATUS retVal = OK;

    *pFA = FLASH28_CMD_RESET;

    for (; (pFA <= FLASH_CAST (FLASH_ADRS + ((UINT32) FLASH_MEM_SIZE - 1)))
	 && (retVal == OK); pFA += FLASH_BLK_SZ)
	{
	*pFA = FLASH28_CMD_ERASE_SETUP;             /* setup */
	*pFA = FLASH28F008_CMD_ERASE;               /* erase */
 
	/* Check Write State Machine Status */
 
	do
	    *pFA = FLASH28F008_CMD_READ_STATUS;
	while ((*pFA & FLASH28F008_STAT_WSMS) != FLASH28F008_STAT_WSMS);
 
	/* Check Erase Error Status */
     
	if ((*pFA & FLASH28F008_STAT_EWS) != 0)
	    {
	    *pFA = FLASH28F008_CMD_CLEAR_STATUS;
	    retVal = ERROR;
	    }

	/* check we are at the last block */

	if (pFA >= FLASH_CAST (FLASH_ADRS + (FLASH_MEM_SIZE - FLASH_ALIGN_SZ)))
	    break;
	}

    pFA = FLASH_CAST (FLASH_ADRS);
    *pFA = FLASH28_CMD_RESET;

    return (retVal);
    }

/******************************************************************************
*
* sysFlashWrite - write data to flash memory
*
* This routine copies specified data of a specified length, <size>, into a
* specified offset, <offset>, in the flash memory.  Data is passed as a string,
* <pFB>, if not NULL.  If NULL, data is taken as a repeated sequence of
* <value>.
* The parameter <offset> must be appropriately aligned for the width of
* the Flash devices in use.
*
* Flash 28F016 devices are programmed by a sequence of operations:
* .iP
* set up device to write
* .iP
* perform write
* .iP
* verify the write
* .LP
*
* RETURNS: OK, or ERROR if the write operation fails.
*
* SEE ALSO: sysFlashSet()
*/

STATUS sysFlashWrite
    (
    FLASH_DEF * pFB,	/* string to be copied; use <value> if NULL */
    UINT32 size,	/* size to program in bytes */
    UINT32 offset,	/* byte offset into flash memory */
    FLASH_DEF value	/* value to program */
    )
    {
    volatile FLASH_DEF * pFA = FLASH_CAST (FLASH_ADRS);	/* flash address */
    STATUS retVal = OK;

    *pFA = FLASH28_CMD_RESET;

    for (pFA = FLASH_CAST (FLASH_ADRS + offset); (pFA < FLASH_CAST
	(FLASH_ADRS + size + offset)) && (retVal == OK); pFA++)
	{
	if (pFB != NULL)
	    value = *pFB++;

	*pFA = FLASH28_CMD_PROG_SETUP;	/* write setup */
	*pFA = value;			/* data to write */

	/* Check Write State Machine Status */

	do
	    {
	    *pFA = FLASH28F008_CMD_READ_STATUS;
	    }
	while ((*pFA & FLASH28F008_STAT_WSMS) != FLASH28F008_STAT_WSMS);


	/* Check Byte Write Error Status */

	if ((*pFA & FLASH28F008_STAT_BWS) != 0)
	    {
	    *pFA = FLASH28F008_CMD_CLEAR_STATUS;
	    retVal = ERROR;
	    }
	}

    pFA = FLASH_CAST (FLASH_ADRS);
    *pFA = FLASH28_CMD_RESET;

    return (retVal);
    }

/******************************************************************************
*
* sysFlashTypeGet - determine the device type of on-board flash memory
*
* This routine uses the `read identifier code' command to determine the 
* device type of * on-board flash memory. The returned value should be 
* the one in drv/mem/flash28.h
*
* RETURNS: An integer indicating the device type of on-board flash memory.
*/

UINT8 sysFlashTypeGet (void)
    {
    volatile FLASH_DEF * pFA = FLASH_CAST (FLASH_ADRS);	/* flash address */
    UINT8 retVal;
    UINT32 temp;

    *pFA = FLASH28_CMD_RESET;
    *pFA = FLASH28_CMD_READ_ID;

    temp = (UINT32) *++pFA;

    retVal = (UINT8) temp;

    *pFA = FLASH28_CMD_RESET;

    return (retVal);
    }

/******************************************************************************
*
* sysFlashUpdate - update the contents of the flash device
*
* This code allows the user to overwrite the flash device with data from
* a file. It will copy data from "filename" to the flash device.  It first
* erase the device, and it attempts to write data to the flash device 
* via sysFlashWrite ().
*
* The user should pass a binary bootrom file, for example
* to update the bootrom with a VxWorks BSP bootrom image:
* .CS
*       % make bootrom
*         make: `bootrom' is up to date.
*       % elfToBin < bootrom > bootrom.bin
*
* -> sysUpdateFlash ("hostname:/tmp/bootrom.bin")
* The flash has been updated without error.
* .CE
*
* RETURNS: OK, or ERROR: if the file was not accessable, the
* file was too large to fit into the flash, the buffers cannot
* be allocated, sysFlashErase () or sysFlashWrite () returned error.
*
* SEE ALSO: sysFlashErase(), sysFlashGet(), sysFlashTypeGet(), sysFlashWrite()
*
* INTERNAL
* If multiple tasks are calling sysFlashSet() and sysFlashGet(),
* they should use a semaphore to ensure mutually exclusive access to flash
* memory.
*/

STATUS sysFlashUpdate
    (
    UCHAR * filename	/* file to be copied into flash memory */
    )
    {
    UCHAR	* pMemBuf;		/* holds the flash data */
    int		fd;                 	/* the bootrom image fd */
    int		statReturn;             /* return values    */

    /* first step is to open the file */

    if ((fd = open (filename, O_RDONLY, 0444)) == ERROR)
        return (ERROR);

    /* now we read it */

    pMemBuf = calloc (FLASH_MEM_SIZE, 1);
 
    if (pMemBuf == NULL)
        {
        printErr ("sysFlashUpdate: ");
        printErr ("Could not allocate 0x%x bytes.\n", (long) FLASH_MEM_SIZE);
        close (fd);
        return (ERROR);
        }
 
    statReturn = read (fd, pMemBuf, FLASH_MEM_SIZE);
 
    if ((statReturn > FLASH_MEM_SIZE) || (statReturn <= 0)) 
        {
        printErr ("Failed to read from file \n");
        close (fd);
	free (pMemBuf);
        return (ERROR);
	}

    /* see if contents are actually changing */

    if (bcmp ((char *) (FLASH_ADRS), pMemBuf, FLASH_MEM_SIZE) == 0)
	return (OK);

    /* now we can erase the device */

    if (sysFlashErase (0) == ERROR)
        return (ERROR);

    /* write to the device */

    if (sysFlashWrite ((FLASH_DEF *) pMemBuf, statReturn, 0, 0) == ERROR)
	{
        printErr ("Failed to write to the device \n");
        close (fd);
	free (pMemBuf);
        return (ERROR);
	}

    return (OK);
    }
