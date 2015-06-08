/* sysMotI2c.c - I2C Routines */

/* Copyright 2003 Motorola, Inc., All Rights Reserved */

/*
modification history
--------------------
01a,02apr03,simon  Ported. from ver 01a, mcpm905/sysMotI2c.c.
*/

/*
DESCRIPTION
This file contains the following system I2C utility routines:

.CS
  sysMotI2cWrite     - Write to a serial eeprom
  sysMotI2cRead      - Read from a serial eeprom
.CE

CAVEATS
These routines are needed before the kernel is un-compressed. For proper
operation, this file must be added to the BOOT_EXTRA list in the Makefile to
prevent it from being compressed during kernel generation.

These routines were primarily intended for use during system start-up, before
multi-tasking is started, and are not multi-tasking safe. They are safe for
command-line use during debug, but are not intended to be run-time
user-callable.
*/

/* includes */

#include "vxWorks.h"
#include "config.h"
#include "stdlib.h"
#include "mv64360I2c.h"

/* defines */

#define I2C_ADDR_MUNGE(x,y)	((x) | (((y) & 0x700) >> 7))
#define I2C_BYTE_ADDR(x,y)	(((x) >> ((y)*8)) & 0xFF)
#define LAST_BYTE		TRUE
#define NOT_LAST_BYTE		FALSE

/* typedefs */

/* locals */

/* forward declarations */

STATUS sysMotI2cWrite (UINT32, UINT32, UINT32, UCHAR *, UINT32);
STATUS sysMotI2cRead (UINT32, UINT32, UINT32, UCHAR *, UINT32);

/******************************************************************************
*
* sysMotI2cWrite - write to an I2C device 
*
* This function writes to an I2C device.
*
* RETURNS: OK, or ERROR if transfer failed.
*/

STATUS sysMotI2cWrite
    (
    UINT32 devAddr,	/* I2C address of target device */
    UINT32 devOffset,	/* offset within target device to write */
    UINT32 devAddrBytes,/* number of address bytes of device */
    UCHAR  *pBfr,	/* pointer to data to be written */
    UINT32 numBytes	/* number of bytes to write */
    )
    {
    UINT32 byteCount;	/* byte counter */
    UCHAR addrByte;
    UINT32 base = MV64360_REG_BASE;
    LOCAL UINT32 validAddr [] = { VALID_I2C_WRT_ADDR };
    UINT32 * p = validAddr;
    UINT32 pageBytes;   /* number of bytes per page write access */
    UINT32 x;           /* loop index */
    UINT32 offset = 0x0;

    /* Determine the number of bytes per page write access */

    if (devAddrBytes == I2C_DUAL_BYTE_ADDR)
        {
        pageBytes = 32;
        }
    else
        {

        /* default to 8 bytes per page for all but dual byte address parts */

        pageBytes = 8;
        }

    while (*p != 0xFFFFFFFF)
	{
	if (*p == devAddr)
	    {
            for (x = 0; x <= numBytes / pageBytes; x++)
                {
                offset = ((x * pageBytes) + devOffset);

    	        if (devAddrBytes == I2C_SINGLE_BYTE_ADDR)
		    {
		    if (sysMv64360I2cStart(base) == ERROR) 
	    	        return (ERROR);
		    addrByte = (UCHAR)(I2C_ADDR_MUNGE(devAddr, offset));
		    if (sysMv64360I2cWrite(base,&addrByte) == ERROR)
	    	        return (ERROR);
		    if (sysMv64360I2cAckIn(base) == ERROR)
	    	        return (ERROR);
		    addrByte = (UCHAR)offset;
		    if (sysMv64360I2cWrite(base,&addrByte) == ERROR)
	    	        return (ERROR);
		    if (sysMv64360I2cAckIn(base) == ERROR)
	    	        return (ERROR);
		    }
    	        else if (devAddrBytes == I2C_DUAL_BYTE_ADDR)
		    {
		    if (sysMv64360I2cStart(base) == ERROR) 
	    	        return (ERROR);
		    addrByte = (UCHAR)devAddr;
		    if (sysMv64360I2cWrite(base,&addrByte) == ERROR)
	    	        return (ERROR);
		    if (sysMv64360I2cAckIn(base) == ERROR)
	    	        return (ERROR);
		    addrByte = (UCHAR)(I2C_BYTE_ADDR(offset,1));
		    if (sysMv64360I2cWrite(base,&addrByte) == ERROR)
	    	        return (ERROR);
		    if (sysMv64360I2cAckIn(base) == ERROR)
	    	        return (ERROR);
		    addrByte = (UCHAR)(I2C_BYTE_ADDR(offset,0));
		    if (sysMv64360I2cWrite(base,&addrByte) == ERROR)
	    	        return (ERROR);
		    if (sysMv64360I2cAckIn(base) == ERROR)
	    	        return (ERROR);
		    }
    	        else if (devAddrBytes == I2C_TRIPLE_BYTE_ADDR)
		    {
		    if (sysMv64360I2cStart(base) == ERROR) 
	    	        return (ERROR);
		    addrByte = (UCHAR)devAddr;
		    if (sysMv64360I2cWrite(base,&addrByte) == ERROR)
	    	        return (ERROR);
		    if (sysMv64360I2cAckIn(base) == ERROR)
	    	        return (ERROR);
		    addrByte = (UCHAR)(I2C_BYTE_ADDR(offset,2));
		    if (sysMv64360I2cWrite(base,&addrByte) == ERROR)
	    	        return (ERROR);
		    if (sysMv64360I2cAckIn(base) == ERROR)
	    	        return (ERROR);
		    addrByte = (UCHAR)(I2C_BYTE_ADDR(offset,1));
		    if (sysMv64360I2cWrite(base,&addrByte) == ERROR)
	    	        return (ERROR);
		    if (sysMv64360I2cAckIn(base) == ERROR)
	    	        return (ERROR);
		    addrByte = (UCHAR)(I2C_BYTE_ADDR(offset,0));
		    if (sysMv64360I2cWrite(base,&addrByte) == ERROR)
	    	        return (ERROR);
		    if (sysMv64360I2cAckIn(base) == ERROR)
	    	        return (ERROR);
		    }
    	        else
		    return (ERROR);		/* unsupported device */

    	        /* write data to device */

    	        for (byteCount = 0; byteCount < pageBytes &&
		     (((x * pageBytes) + byteCount) < numBytes); byteCount++)
		    {
        	    if (sysMv64360I2cWrite(base,pBfr) == ERROR)
	   	        return (ERROR);
		    pBfr++;	/* increment pointer to next byte */ 
		    }
        	    
    	        if (sysMv64360I2cAckIn(base) == ERROR)
		    return (ERROR);
    	        if (sysMv64360I2cStop(base)  == ERROR)
		    return (ERROR);

                /*
                 * Delay:
                 * The following delay is required at the end of an I2C write
                 * to allow the EEPROM's internally timed write cycle to
                 * complete.
                 */

                sysMv64360UsDelay(10000);
		}
    	    return (OK);
    	    }
        else
	    p++;	/* advance pointer to next valid I2C address */
        }
    return (ERROR);	/* device is not to be written to */
    }

/*******************************************************************************
*
* sysMotI2cRead - read from an I2C device 
*
* This function reads from an I2C device.
*
* RETURNS: OK, or ERROR if transfer failed.
*/

STATUS sysMotI2cRead
    (
    UINT32 devAddr,	/* I2C address of target device */
    UINT32 devOffset,	/* offset within target device to read */
    UINT32 devAddrBytes,/* number of address bytes of device */ 
    UCHAR  *pBfr,	/* pointer to read buffer */
    UINT32 numBytes	/* number of bytes to read */
    )
    {
    UCHAR addrByte;
    UINT32 base = MV64360_REG_BASE;

    if (devAddrBytes == I2C_SINGLE_BYTE_ADDR)
	{
	if (sysMv64360I2cStart(base) == ERROR) 
	    return (ERROR);
	addrByte = (UCHAR)(I2C_ADDR_MUNGE(devAddr, devOffset));
	if (sysMv64360I2cWrite(base,&addrByte) == ERROR)
	    return (ERROR);
	if (sysMv64360I2cAckIn(base) == ERROR)
	    return (ERROR);
	addrByte = (UCHAR)devOffset;
	if (sysMv64360I2cWrite(base,&addrByte) == ERROR)
	    return (ERROR);
	if (sysMv64360I2cAckIn(base) == ERROR)
	    return (ERROR);
	if (sysMv64360I2cStart(base) == ERROR) 
	    return (ERROR);
	addrByte = (UCHAR)(I2C_ADDR_MUNGE((devAddr | 0x01), devOffset));
	if (sysMv64360I2cWrite(base,&addrByte) == ERROR)
	    return (ERROR);
	if (sysMv64360I2cAckIn(base) == ERROR)
	    return (ERROR);
	}
    else if (devAddrBytes == I2C_DUAL_BYTE_ADDR)
	{
	if (sysMv64360I2cStart(base) == ERROR) 
	    return (ERROR);
	addrByte = (UCHAR)devAddr;
	if (sysMv64360I2cWrite(base,&addrByte) == ERROR)
	    return (ERROR);
	if (sysMv64360I2cAckIn(base) == ERROR)
	    return (ERROR);
	addrByte = (UCHAR)(I2C_BYTE_ADDR(devOffset,1));
	if (sysMv64360I2cWrite(base,&addrByte) == ERROR)
	    return (ERROR);
	if (sysMv64360I2cAckIn(base) == ERROR)
	    return (ERROR);
	addrByte = (UCHAR)(I2C_BYTE_ADDR(devOffset,0));
	if (sysMv64360I2cWrite(base,&addrByte) == ERROR)
	    return (ERROR);
	if (sysMv64360I2cAckIn(base) == ERROR)
	    return (ERROR);
	if (sysMv64360I2cStart(base) == ERROR) 
	    return (ERROR);
	addrByte = (UCHAR)(devAddr | 0x01);
	if (sysMv64360I2cWrite(base,&addrByte) == ERROR)
	    return (ERROR);
	if (sysMv64360I2cAckIn(base) == ERROR)
	    return (ERROR);
	}
    else if (devAddrBytes == I2C_TRIPLE_BYTE_ADDR)
	{
	if (sysMv64360I2cStart(base) == ERROR) 
	    return (ERROR);
	addrByte = (UCHAR)devAddr;
	if (sysMv64360I2cWrite(base,&addrByte) == ERROR)
	    return (ERROR);
	if (sysMv64360I2cAckIn(base) == ERROR)
	    return (ERROR);
	addrByte = (UCHAR)(I2C_BYTE_ADDR(devOffset,2));
	if (sysMv64360I2cWrite(base,&addrByte) == ERROR)
	    return (ERROR);
	if (sysMv64360I2cAckIn(base) == ERROR)
	    return (ERROR);
	addrByte = (UCHAR)(I2C_BYTE_ADDR(devOffset,1));
	if (sysMv64360I2cWrite(base,&addrByte) == ERROR)
	    return (ERROR);
	if (sysMv64360I2cAckIn(base) == ERROR)
	    return (ERROR);
	addrByte = (UCHAR)(I2C_BYTE_ADDR(devOffset,0));
	if (sysMv64360I2cWrite(base,&addrByte) == ERROR)
	    return (ERROR);
	if (sysMv64360I2cAckIn(base) == ERROR)
	    return (ERROR);
	if (sysMv64360I2cStart(base) == ERROR) 
	    return (ERROR);
	addrByte = (UCHAR)(devAddr | 0x01);
	if (sysMv64360I2cWrite(base,&addrByte) == ERROR)
	    return (ERROR);
	if (sysMv64360I2cAckIn(base) == ERROR)
	    return (ERROR);
	}
    else
	return (ERROR);		/* unsupported device */

    /* read the specified number of bytes */

    for ( ; numBytes != 0; --numBytes)
	{
	if (numBytes != 1)
	    {
            if (sysMv64360I2cRead(base,pBfr,NOT_LAST_BYTE) == ERROR)
	    return (ERROR);
	    }
	else
	    {
            if (sysMv64360I2cRead(base,pBfr,LAST_BYTE) == ERROR)
	    return (ERROR);
	    }
	pBfr++;	/* increment pointer to next byte */ 
	}

    if (sysMv64360I2cStop(base) == ERROR)
	return (ERROR);

    return (OK);
    }
