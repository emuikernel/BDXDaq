/* hawkI2c.c - Low-Level Hawk I2C Routines */

/* Copyright 1998-2001 Wind River Systems, Inc. */
/* Copyright 1998-2001 Motorola, Inc., All Rights Reserved */

/*
modification history
--------------------
01d,14nov01,kab  Fixed compiler warning
01c,16sep01,dat  removed inline assembly, used vxDecGet()
01b,07mar01,rhk  WRS coding standards cleanup.
01a,03mar00,dmw  Written (from version 01a of mcpn765/hawkI2c.c).
*/

/*
DESCRIPTION
This file contains the following hawk i2c utility routines:

.CS
  sysHawkI2cWait      - Waits for a given value in the I2C status register
  sysHawkI2CByteWrite - Write a single byte to a serial eeprom
  sysHawkI2cByteRead  - Reads a single byte from a serial eeprom
  sysHawkI2cRangeRead - Reads a range of bytes from a serial eeprom
.CE

CAVEATS
These routines are needed before the kernel is un-compressed. For proper
operation, this file must be added to the BOOT_EXTRA list in the Makefile to
prevent it from being compressed during kernel generation.

These routines were primarily intended for use during system start-up, before
multi-tasking is started, and are not multi-tasking safe. They are safe for
command-line use during debug, but were not intended to be run-time
user-callable.
*/

#include "vxWorks.h"
#include "config.h"
#include "stdlib.h"
#include "hawkI2c.h"                /* i2c defines */

/* forwards */

void sysHawkMsDelay(UINT32);
LOCAL int  sysHawkGetDec();

/* externals */
IMPORT	UINT32	vxDecGet(void);


/*******************************************************************************
*
* sysHawkI2cWait - wait for a given value in the i2c status register
*
* This function waits for a given value in the i2c status register.
* If the specified value is never detected, the routine times out
* and returns an error. the routine also returns an error if the error bit
* in the status register is found set.
*
* RETURNS: OK, or ERROR if the expected value does not appear.
*/

STATUS sysHawkI2cWait
    (
    UINT32 value    /* expected status value */
    )
    {
    UINT32          count;
    volatile UINT32 status;

    EIEIO_SYNC;				/* eieio, sync */

    for (count=0;;)
        {
        status = *(UINT32 *)I2C_HAWK_STATUS_REG;

        if ( (status & value) == value )
           return (OK);

        else
            {

            /* check for timeout or error bit */

            if ( (++count > I2C_TIMEOUT_COUNT) || (status & I2C_HAWK_ERR) )
                {

                /* generate stop condition and return error */

                *(UINT32 *)I2C_HAWK_CONTROL_REG = I2C_HAWK_STOP|I2C_HAWK_ENBL;
                *(UINT32 *)I2C_HAWK_TXD_REG = 0;
                return (ERROR);
                }

            sysHawkMsDelay(1);   /* delay 1 ms */

            }
        }
    }


/*******************************************************************************
*
* sysHawkI2cByteWrite - write a single byte to an i2c device via the hawk
*
* This function writes one 8-bit word to an I2C device specified by the passed
* in parameters. The access is specifically tailored for the Hawk I2C 
* controller.
*
* RETURNS: OK, or ERROR if transfer failed.
*/

STATUS sysHawkI2cByteWrite
    (
    UCHAR   devAddr,   /* i2c address of target device */
    UCHAR   devOffset, /* offset within target device to write */
    UCHAR * pBfr       /* pointer to data byte */
    )
    {

    /* don't start unless status reg shows complete bit set */

    if ( sysHawkI2cWait (I2C_HAWK_CMPLT) != OK )
        return (ERROR);  

    /* initiate start sequence */

    *(UINT32 *)I2C_HAWK_CONTROL_REG = I2C_HAWK_START|I2C_HAWK_ENBL;

    /* write addr to transmit data register, bit 0 = 0 = write */

    *(UINT32 *)I2C_HAWK_TXD_REG = (UINT32)(devAddr & BYTE_WRITE_MASK);

    /* await complete + acknowledge in status register */

    if ( sysHawkI2cWait (I2C_HAWK_ACKIN|I2C_HAWK_CMPLT) != OK ) 
        return (ERROR);

    /* 
     * end of start sequence
     *
     * devOffset to transmit data register 
     */

    *(UINT32 *)I2C_HAWK_TXD_REG = (UINT32)devOffset;

    /* await complete + acknowledge in status register */

    if ( sysHawkI2cWait (I2C_HAWK_ACKIN|I2C_HAWK_CMPLT) != OK )
        return (ERROR);

    /* Send data byte to transmit data register */

    *(UINT32 *)I2C_HAWK_TXD_REG = (UINT32)*pBfr;

    /* await complete + acknowledge in status register */

    if ( sysHawkI2cWait (I2C_HAWK_ACKIN|I2C_HAWK_CMPLT) != OK ) 
        return (ERROR);

    /* stop sequence */

    *(UINT32 *)I2C_HAWK_CONTROL_REG = I2C_HAWK_STOP|I2C_HAWK_ENBL;

    *(UINT32 *)I2C_HAWK_TXD_REG = 0;  /* dummy write */

    sysHawkMsDelay(1);                /* delay for ASIC errata. */
   
    /* await complete in status register */

    if ( sysHawkI2cWait (I2C_HAWK_CMPLT) != OK )
        return (ERROR);

    /* allow time for EEPROM write to occur. */

    sysHawkMsDelay(1);
   
    return (OK);
    }


/*******************************************************************************
*
* sysHawkI2cByteRead - read a single byte to an i2c device via the Hawk
*
* This function reads one 8-bit word from an I2C device specified by the passed
* in parameters. The access is specifically tailored for the Hawk I2C 
* controller.
*
* RETURNS: OK, or ERROR if transfer failed.
*/

STATUS sysHawkI2cByteRead
    (
    UCHAR   devAddr,   /* i2c address of target device */
    UCHAR   devOffset, /* offset within target device to read */
    UCHAR * pBfr       /* pointer to data byte */
    )
    {

    /* await complete in status register */

    if ( sysHawkI2cWait (I2C_HAWK_CMPLT) != OK )
        return (ERROR);

    /* initiate start sequence in write mode */

    *(UINT32 *)I2C_HAWK_CONTROL_REG = I2C_HAWK_START|I2C_HAWK_ENBL;
    *(UINT32 *)I2C_HAWK_TXD_REG = (UINT32)(devAddr & BYTE_WRITE_MASK);
   
    /* await complete + acknowledge in status register */

    if ( sysHawkI2cWait (I2C_HAWK_ACKIN|I2C_HAWK_CMPLT) != OK ) 
        return (ERROR);

    /* devOffset to transmit data register */

    *(UINT32 *)I2C_HAWK_TXD_REG = (UINT32)devOffset;

    /* await complete + acknowledge in status register */

    if ( sysHawkI2cWait (I2C_HAWK_ACKIN|I2C_HAWK_CMPLT) != OK ) 
       return (ERROR);

    /* initiate start sequence in read mode */

    *(UINT32 *)I2C_HAWK_CONTROL_REG = I2C_HAWK_START|I2C_HAWK_ENBL;
    *(UINT32 *)I2C_HAWK_TXD_REG = (UINT32)(devAddr | BYTE_READ_MASK);
   
    /* await complete + acknowledge in status register */

    if ( sysHawkI2cWait (I2C_HAWK_ACKIN|I2C_HAWK_CMPLT) != OK ) 
        return (ERROR);

    /* read the byte */

    *(UINT32 *)I2C_HAWK_TXD_REG = 0;    /* dummy write to initiate read */   

    sysHawkMsDelay(1);                  /* delay for ASIC errata. */

    /* await complete and datain  in status register */

    if ( sysHawkI2cWait (I2C_HAWK_CMPLT | I2C_HAWK_DATIN) != OK )  
       return (ERROR);

   /* the actual read */

   *pBfr = (UCHAR) (*(UINT32 *)I2C_HAWK_RXD_REG);

   /* 
    * There is no ack after a single byte read
    *
    * stop sequence 
    */

   *(UINT32 *)I2C_HAWK_CONTROL_REG = I2C_HAWK_STOP|I2C_HAWK_ENBL;

   *(UINT32 *)I2C_HAWK_TXD_REG = 0;  /* dummy write */
   sysHawkMsDelay(1);                /* delay for ASIC errata. */
   
   /* await complete in status register */

   if ( sysHawkI2cWait (I2C_HAWK_CMPLT) != OK ) 
      return (ERROR);

   return (OK);
   }


/*******************************************************************************
*
* sysHawkI2cRangeRead - reads a range of bytes from an I2C serial eeprom (SROM)
*
* This routine simply calls the I2C byte read routine for each requested byte.
* The I2C byte read call is written using a macro to accommodate alternate
* read routines.
*
* RETURNS: OK, or ERROR if the I2C byte read fails.
*
* SEE ALSO: sysI2cSromRangeWrite
*/

STATUS sysHawkI2cRangeRead
    (
    UCHAR    devAddr,    /* i2c address of the serial eeprom */
    UCHAR    devOffset,  /* starting offset within the serial eeprom to read */
    UINT16   byteCount,  /* number of bytes to read (one-based) */
    UCHAR *  pBfr        /* destination buffer */
    )
    {

    /* await complete in status register */

    if ( sysHawkI2cWait (I2C_HAWK_CMPLT) != OK )
        return (ERROR);

    /* initiate start sequence in write mode */

    *(UINT32 *)I2C_HAWK_CONTROL_REG = I2C_HAWK_START|I2C_HAWK_ENBL;
    *(UINT32 *)I2C_HAWK_TXD_REG = (UINT32)(devAddr & BYTE_WRITE_MASK);

    /* await complete + acknowledge in status register */

    if ( sysHawkI2cWait (I2C_HAWK_ACKIN|I2C_HAWK_CMPLT) != OK )
        return (ERROR);

    /* devOffset to transmit data register */

    *(UINT32 *)I2C_HAWK_TXD_REG = (UINT32)devOffset;

    /* await complete + acknowledge in status register */

    if ( sysHawkI2cWait (I2C_HAWK_ACKIN|I2C_HAWK_CMPLT) != OK )
       return (ERROR);

    /* initiate start sequence in read mode */

    *(UINT32 *)I2C_HAWK_CONTROL_REG = I2C_HAWK_START|I2C_HAWK_ENBL;
    *(UINT32 *)I2C_HAWK_TXD_REG = (UINT32)(devAddr | BYTE_READ_MASK);

    /* await complete + acknowledge in status register */

    if ( sysHawkI2cWait (I2C_HAWK_ACKIN|I2C_HAWK_CMPLT) != OK )
        return (ERROR);

    for ( ; byteCount != 0; --byteCount)
        {

        if (byteCount != 1)
            *(UINT32 *)I2C_HAWK_CONTROL_REG = I2C_HAWK_ACKOUT|I2C_HAWK_ENBL;
        else
            *(UINT32 *)I2C_HAWK_CONTROL_REG = I2C_HAWK_ENBL;

        /* read the byte */

        *(UINT32 *)I2C_HAWK_TXD_REG = 0;    /* dummy write to initiate read */

        sysHawkMsDelay(1);                  /* delay for ASIC errata. */

        /* await complete and datain  in status register */

        if ( sysHawkI2cWait (I2C_HAWK_CMPLT | I2C_HAWK_DATIN) != OK )
            return (ERROR);

        /* the actual read */

        *pBfr++ = (UCHAR) (*(UINT32 *)I2C_HAWK_RXD_REG);

        }

    /* stop sequence */

    *(UINT32 *)I2C_HAWK_CONTROL_REG = I2C_HAWK_STOP|I2C_HAWK_ENBL;

    *(UINT32 *)I2C_HAWK_TXD_REG = 0;  /* dummy write */
    sysHawkMsDelay(1);                /* delay for ASIC errata. */

    /* await complete in status register */

    if ( sysHawkI2cWait (I2C_HAWK_CMPLT) != OK )
       return (ERROR);

    return (OK);
    }


/******************************************************************************
*
* sysHawkMsDelay - delay for the specified amount of time (MS)
*
* This routine will delay for the specified amount of time by counting
* decrementer ticks.
*
* This routine is not dependent on a particular rollover value for
* the decrementer, it should work no matter what the rollover
* value is.
*
* A small amount of count may be lost at the rollover point resulting in
* the sysHawkMsDelay() causing a slightly longer delay than requested.
*
* This routine will produce incorrect results if the delay time requested
* requires a count larger than 0xffffffff to hold the decrementer
* elapsed tick count.  For a System Bus Speed of 67 MHZ this amounts to
* about 258 seconds.
*
* RETURNS: N/A
*/

void sysHawkMsDelay
    (
    UINT        delay                   /* length of time in MS to delay */
    )
    {
    register UINT oldval;	/* decrementer value */
    register UINT newval;       /* decrementer value */
    register UINT totalDelta;   /* Dec. delta for entire delay period */
    register UINT decElapsed;   /* cumulative decrementer ticks */

    /* Calculate delta of decrementer ticks for desired elapsed time. */

    totalDelta = ((DEFAULT_BUS_CLOCK / 4) / 1000) * delay;

    /*
     * Now keep grabbing decrementer value and incrementing "decElapsed" until
     * we hit the desired delay value.  Compensate for the fact that we may
     * read the decrementer at 0xffffffff before the interrupt service
     * routine has a chance to set in the rollover value.
     */

    decElapsed = 0;
    oldval = sysHawkGetDec();
    while (decElapsed < totalDelta)
        {
        newval = sysHawkGetDec();
        if ( DELTA(oldval,newval) < 1000 )
            decElapsed += DELTA(oldval,newval);  /* no rollover */
        else
            if (newval > oldval)
                decElapsed += abs((int)oldval);  /* rollover */
        oldval = newval;
        }
    }


/******************************************************************************
*
* sysHawkGetDec - read from the Decrementer register SPR22.
*
* This routine will read the contents the decrementer (SPR22)
*
* RETURNS: value of SPR22 (in r3)
*/

LOCAL int sysHawkGetDec(void)
    {
    return vxDecGet();
    }

