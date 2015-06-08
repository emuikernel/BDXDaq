/* m48t18NvRam.c - non-volatile RAM library for m48t18 */

/* Copyright 1984-1996 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01b,25jan95,vin	 made it generic for both I/O mapped as well as memory mapped.
01a,24jan95,kvk  created from 01d of nvRam.c.
*/

/*
DESCRIPTION
This library contains routines to manipulate non-volatile RAM which is
accessed as normal RAM (e.g. battery backed RAM).  Read and write routines
are included.  All non-volatile RAM accesses are byte wide.

The macro NV_RAM_ADRS must be defined to point to the first byte of
non-volatile memory.  The macro NV_RAM_SIZE must be defined to provide parameter
checking for sysNvRamSet() and sysNvRamGet().
*/


/* local defines */

#define NV_RAM_ADRS_MASK	0x0000FFFF

/* forward declarations */

#ifdef NV_RAM_IO_MAPPED
static void m4818Read (char * string, int strLen, USHORT addr);
static void m4818Write (char * string, int strLen, USHORT addr);
#endif /* NV_RAM_IO_MAPPED */

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

#ifdef NV_RAM_IO_MAPPED	/* nvram is io mapped */

    m4818Read (string, strLen,  (USHORT)((ULONG)(NV_RAM_ADRS + offset) & 
					 NV_RAM_ADRS_MASK));

#else			/* nvram is memory mapped */
    				
    bcopyBytes (NV_RAM_ADRS + offset, string, strLen);

#endif /* NV_RAM_IO_MAPPED */

    string [strLen] = EOS;

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
*/

STATUS sysNvRamSet
    (
    char *string,     /* string to be copied into non-volatile RAM */
    int strLen,       /* maximum number of bytes to copy           */
    int offset        /* byte offset into non-volatile RAM         */
    )

    {
    offset += NV_BOOT_OFFSET;   /* boot line begins at <offset> = 0 */

    if ((offset < 0) || (strLen < 0) || ((offset + strLen) > NV_RAM_SIZE))
        return (ERROR);

#ifdef NV_RAM_IO_MAPPED	/* nvram is iomapped */

    m4818Write (string, strLen, (USHORT)((ULONG)(NV_RAM_ADRS + offset) & 
					 NV_RAM_ADRS_MASK));

#else			/* nvram is memory mapped */

    bcopyBytes (string, NV_RAM_ADRS + offset, strLen);

#endif /* NV_RAM_IO_MAPPED */

    return (OK);
    }

#ifdef NV_RAM_IO_MAPPED

/*******************************************************************************
*
* m4818Read - read from I/O mapped non-volatile RAM
*
* This routine copies non-volatile RAM to a specified string.
*
* RETURNS: N/A
*
* NOMANUAL
*/

static void m4818Read
    (
    char *	string,		/* string to copy to non-volatile RAM    */
    int 	strLen,		/* maximum number of bytes to copy   */
    USHORT 	offset		/* byte offset into non-volatile RAM */
    )
    {

    int 	ix;

    for(ix = 0; ix < strLen; offset++, ix++)
	{
	sysOutByte (NV_RAM_LSB_REG, LSB(offset));
	sysOutByte (NV_RAM_MSB_REG, MSB(offset));
	*string++ = (char)sysInByte (NV_RAM_DAT_REG);
	}
    }

/*******************************************************************************
*
* m4818Write - Write to I/O mapped non-volatile RAM
*
* This routine copies a specified string to non-volatile RAM.
*
* RETURNS: N/A
*
* NOMANUAL
*/

static void m4818Write
    (
    char *	string,		/* string to copy to non-volatile RAM    */
    int 	strLen,		/* maximum number of bytes to copy   */
    USHORT 	offset		/* byte offset into non-volatile RAM */
    )
    {
    int 	ix;

    for(ix = 0; ix < strLen;  string++, offset++, ix++)
	{
	sysOutByte (NV_RAM_LSB_REG, LSB(offset));
	sysOutByte (NV_RAM_MSB_REG, MSB(offset));
	sysOutByte (NV_RAM_DAT_REG, *string);
	}
    }

#endif /* NV_RAM_IO_MAPPED */









