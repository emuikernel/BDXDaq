/* nvRam.c - non-volatile RAM library */

/* Copyright 1984-1996 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01f,18jun96,wlf  doc: cleanup.
01e,24may96,wlf  doc: cleanup.
01d,28jan93,caf  made boot line begin at offset 0 (SPR #1933).
01c,02sep92,caf  changed nvRamLib.c to nvRam.c. changed NV_BOOT_LINE to
		 NV_RAM_ADRS.  tweaked documentation.
01b,29jun92,caf  changed bbRamLib.c to nvRamLib.c. tweaked documentation.
01a,26jun92,caf  derived from sysNvRamSet() and sysNvRamGet() in PortKit.
*/

/*
DESCRIPTION
This library contains routines to manipulate non-volatile RAM which is
accessed as normal RAM (e.g., battery backed RAM).  Read and write routines
are included.  All non-volatile RAM accesses are byte wide.

The macro NV_RAM_ADRS must be defined to point to the first byte of
non-volatile memory.  The macro NV_RAM_SIZE must be defined to provide parameter
checking for sysNvRamSet() and sysNvRamGet().
*/

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

    if ((offset < 0) || (strLen < 0) || ((offset + strLen) > NV_RAM_SIZE))
        return (ERROR);

    bcopyBytes (NV_RAM_ADRS + offset, string, strLen);
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

    bcopyBytes (string, NV_RAM_ADRS + offset, strLen);
    return (OK);
    }
