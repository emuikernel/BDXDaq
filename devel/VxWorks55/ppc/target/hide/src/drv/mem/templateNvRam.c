/* templateNvRam.c - template NVRAM driver */

/* Copyright 1984-1997 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
TODO -	Remove the template modification history and begin a new history
	starting with version 01a and growing the history upward with
	each revision.

modification history
--------------------
01a,02sep97,dat  written (from byteNvRam.c, ver 01b)
*/

/*
.SH TODO

 - Replace the documentation for this template driver with documentation
for the driver being written.  

 - Begin with an overview of the complete device.  Indicate if the new driver
only implements a sub-section of the whole device or not.

 - Describe all of the operating modes of the device, and indicate which
ones this driver implements.

 - Document the device initialization steps to be used in the BSP to create
and initialize the device.  Document all the macros that
can be used to customize the driver to a particular hardware environment.

 - Document anything that will help the user to understand how this device
works and interacts with this driver.

DESCRIPTION
This template is for byte-oriented non-volatile RAM (NVRAM).  Typical uses
are EEPROMs, static RAM, and any other byte-oriented device.

This is boilerplate driver code.  It is to be included in source form
into sysLib.c.  Macros are used to provide the actual working elements
while this code provides the generic logic.

The following numeric value macros are required:

  NV_RAM_ADRS	  - address of first non-volatile byte
  NV_RAM_INTRVL	  - address interval between bytes
  NV_RAM_SIZE	  - total number of bytes in device
  NV_BOOT_OFFSET  - Offset to first byte of boot line information

The following procedural macros are used.  If not defined by the
specific BSP, then default procedures assuming static RAM with no
special read/write requirements will be used.

  NV_RAM_READ(x)    - Read and return one byte at offset (x).
  NV_RAM_WRITE(x,y) - Write data (y) at offset (x).
  NV_RAM_WR_ENBL    - procedure to enable writing, if any
  NV_RAM_WR_DSBL    - procedure to disable writing, if any

*/

#include "vxWorks.h"
#include "config.h"

/* default procedures assume static ram with no special r/w routines */
#ifndef NV_RAM_WR_ENBL
#   define NV_RAM_WR_ENBL	/* no write enable procedure */
#endif /*NV_RAM_WR_ENBL*/

#ifndef NV_RAM_WR_DSBL
#   define NV_RAM_WR_DSBL	/* no write disable procedure */
#endif /*NV_RAM_WR_DSBL*/

#ifndef NV_RAM_READ
#   define NV_RAM_READ(x) \
	(*(UCHAR *)((int)NV_RAM_ADRS + ((x) * NV_RAM_INTRVL)))
#endif /*NV_RAM_READ*/

#ifndef NV_RAM_WRITE
#   define NV_RAM_WRITE(x,y) \
	(*(UCHAR *)((int)NV_RAM_ADRS + ((x) * NV_RAM_INTRVL)) = (y))
#endif /*NV_RAM_WRITE*/
	
/******************************************************************************
*
* sysNvRamGet - get the contents of non-volatile RAM
*
* This routine copies the contents of non-volatile memory into a specified
* string.  The string is terminated with an EOS.
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

    if ((offset < 0)
     || (strLen < 0)
     || ((offset + strLen) > NV_RAM_SIZE))
        return (ERROR);

    while (strLen--)
	{
	*string = NV_RAM_READ (offset);
	string++, offset++;
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
*/

STATUS sysNvRamSet
    (
    char *string,     /* string to be copied into non-volatile RAM */
    int strLen,       /* maximum number of bytes to copy           */
    int offset        /* byte offset into non-volatile RAM         */
    )

    {
    STATUS result = OK;

    offset += NV_BOOT_OFFSET;   /* boot line begins at <offset> = 0 */

    if ((offset < 0)
     || (strLen < 0)
     || ((offset + strLen) > NV_RAM_SIZE))
        return ERROR;

    NV_RAM_WR_ENBL;

    while (strLen--)
	{
	char data;

	data = *string; /* avoid any macro side effects */

	NV_RAM_WRITE (offset, data);

	/* verify data */
	if (NV_RAM_READ (offset) != (UCHAR)data)
	    {
	    result = ERROR;
	    goto exit;
	    }

	string++, offset++;
	}
	
exit:
    NV_RAM_WR_DSBL;

    return result;
    }
