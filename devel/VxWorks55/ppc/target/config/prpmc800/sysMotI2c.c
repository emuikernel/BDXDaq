/* sysMotI2c.c - Low-Level Motorola Two-Wire I2C Routines */

/* Copyright 2000 Motorola, Inc., All Rights Reserved */

/*
modification history
--------------------
01g,17jan02,dtr  Removing diab warning.
01f,06nov00,krp  Dual address device give extra delay for I2C write data.
01e,08oct00,dmw  Removed extra delays required for Hawk.
01d,08sep00,dmw  Made sysMotI2cRangeRead dual-address mode capable.
01c,08sep00,dmw  Added dual-address I2C EEPROM accesses.
01b,07sep00,dmw  Updated for common I2C defines.
01a,31aug00,dmw  Written (from version 01b of mcpn765/hawkI2c.c).
*/

/*
DESCRIPTION
This file contains the following system i2c utility routines:

.CS
  sysMotI2cWait      - Waits for a given value in the I2C status register
  sysMotI2cWrite     - Write one or two bytes to a serial eeprom
  sysMotI2cRead      - Reads one or two bytes from a serial eeprom
  sysMotI2cRangeRead - Reads a range of bytes from a serial eeprom
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

/* board dependant I2C defines */

#include "harrierI2c.h"

IMPORT UINT sysMotGetDec(void);

/* forwards */

void   sysMotMsDelay(UINT32);



/*******************************************************************************
*
* sysMotI2cWait - wait for a given value in the i2c status register
*
* This function waits for a given value in the i2c status register.
* If the specified value is never detected, the routine times out
* and returns an error. the routine also returns an error if the error bit
* in the status register is found set.
*
* RETURNS: OK, or ERROR if the expected value does not appear.
*/

STATUS sysMotI2cWait
    (
    UINT32 value    /* expected status value */
    )
    {
    UINT32          count;
    volatile UINT32 status;

    EIEIO_SYNC;				/* eieio, sync */

    for (count=0;;)
        {
        status = *(UINT32 *)SYS_I2C_STATUS_REG;

        if ( (status & value) == value )
           return (OK);

        else
            {

            /* check for timeout or error bit */

            if ( (++count > I2C_TIMEOUT_COUNT) || (status & SYS_I2C_ERR) )
                {

                /*
                 * generate stop condition and return error
                 */

                *(UINT32 *)SYS_I2C_CONTROL_REG = SYS_I2C_STOP|SYS_I2C_ENBL;
                *(UINT32 *)SYS_I2C_TXD_REG = 0;
                return (ERROR);
                }

            sysMotMsDelay(1);   /* delay 1 ms */

            }
        }
    }


/******************************************************************************
*
* sysMotI2cWrite - write a single byte to an i2c device via the harrier
*
* This function writes one or two bytes to an I2C device specified by the 
* passed in parameters. 
*
* RETURNS: OK, or ERROR if transfer failed.
*/

STATUS sysMotI2cWrite
    (
    UCHAR   devAddr,   /* i2c address of target device */
    USHORT  devOffset, /* offset within target device to write */
    UCHAR  *pBfr,      /* pointer to data byte */
    UCHAR   num        /* number of bytes to write */
    )
    {

    /* don't start unless status reg shows complete bit set */

    if ( sysMotI2cWait (SYS_I2C_CMPLT) != OK )
        return (ERROR);  

    /* initiate start sequence */

    *(UINT32 *)SYS_I2C_CONTROL_REG = SYS_I2C_START|SYS_I2C_ENBL;

    /* write addr to transmit data register, bit 0 = 0 = write */

    *(UINT32 *)SYS_I2C_TXD_REG = (UINT32)(devAddr & BYTE_WRITE_MASK);

    /* await complete + acknowledge in status register */

    if ( sysMotI2cWait (SYS_I2C_ACKIN|SYS_I2C_CMPLT) != OK ) 
        return (ERROR);

    /* end of start sequence */

    /* devOffset to transmit data register */

    if (num == I2C_DUAL_ADDRESS)
        {
        *(UINT32 *)SYS_I2C_TXD_REG = (UINT32)(devOffset >> 8);

        /* await complete + acknowledge in status register */

        if ( sysMotI2cWait (SYS_I2C_ACKIN|SYS_I2C_CMPLT) != OK )
            return (ERROR);
        }

    *(UINT32 *)SYS_I2C_TXD_REG = (UINT32)(devOffset & 0x00FF);

    /* await complete + acknowledge in status register */

    if ( sysMotI2cWait (SYS_I2C_ACKIN|SYS_I2C_CMPLT) != OK )
        return (ERROR);

    /* Send data byte to transmit data register */

    *(UINT32 *)SYS_I2C_TXD_REG = (UINT32)*pBfr;

    /* await complete + acknowledge in status register */

    if ( sysMotI2cWait (SYS_I2C_ACKIN|SYS_I2C_CMPLT) != OK ) 
        return (ERROR);

    /* stop sequence */

    *(UINT32 *)SYS_I2C_CONTROL_REG = SYS_I2C_STOP|SYS_I2C_ENBL;

    *(UINT32 *)SYS_I2C_TXD_REG = 0;  /* dummy write */

    /* await complete in status register */

    if ( sysMotI2cWait (SYS_I2C_CMPLT) != OK )
        return (ERROR);

    /* Allow time for EEPROM write to occur  */

    sysMotMsDelay(5);
  
    return (OK);
    }


/*******************************************************************************
*
* sysMotI2cRead - read a single byte to an i2c device 
*
* This function reads one or two 8-bit words from an I2C device specified by 
* the passed in parameters. 
*
* RETURNS: OK, or ERROR if transfer failed.
*/

STATUS sysMotI2cRead
    (
    UCHAR   devAddr,   /* i2c address of target device */
    USHORT  devOffset, /* offset within target device to read */
    UCHAR  *pBfr,      /* pointer to data byte */
    UCHAR   num        /* number of bytes to read */
    )
    {

    /* await complete in status register */

    if ( sysMotI2cWait (SYS_I2C_CMPLT) != OK )
        return (ERROR);

    /* initiate start sequence in write mode */

    *(UINT32 *)SYS_I2C_CONTROL_REG = SYS_I2C_START|SYS_I2C_ENBL;
    *(UINT32 *)SYS_I2C_TXD_REG = (UINT32)(devAddr & BYTE_WRITE_MASK);

    /* await complete + acknowledge in status register */

    if ( sysMotI2cWait (SYS_I2C_ACKIN|SYS_I2C_CMPLT) != OK ) 
        return (ERROR);

    if (num == I2C_DUAL_ADDRESS)
        {
        *(UINT32 *)SYS_I2C_TXD_REG = (UINT32)(devOffset >> 8);

        if ( sysMotI2cWait (SYS_I2C_ACKIN|SYS_I2C_CMPLT) != OK ) 
            return (ERROR);
        }

    *(UINT32 *)SYS_I2C_TXD_REG = (UINT32)(devOffset & 0x00FF);
   
    /* await complete + acknowledge in status register */

    if ( sysMotI2cWait (SYS_I2C_ACKIN|SYS_I2C_CMPLT) != OK ) 
        return (ERROR);

    /* initiate start sequence in read mode */

    *(UINT32 *)SYS_I2C_CONTROL_REG = SYS_I2C_START|SYS_I2C_ENBL;
    *(UINT32 *)SYS_I2C_TXD_REG = (UINT32)(devAddr | BYTE_READ_MASK);

    /* await complete and datain  in status register */

    if ( sysMotI2cWait (SYS_I2C_ACKIN | SYS_I2C_CMPLT) != OK )  
       return (ERROR);

    /* read the byte */

    *(UINT32 *)SYS_I2C_TXD_REG = 0;    /* dummy write to initiate read */   

    /* await complete and datain  in status register */

    if ( sysMotI2cWait (SYS_I2C_CMPLT | SYS_I2C_DATIN) != OK )  
       return (ERROR);

   /* the actual read */

   *pBfr = (UCHAR) (*(UINT32 *)SYS_I2C_RXD_REG);

   /* There is no ack after a single byte read */

   /* stop sequence */

   *(UINT32 *)SYS_I2C_CONTROL_REG = SYS_I2C_STOP|SYS_I2C_ENBL;

   *(UINT32 *)SYS_I2C_TXD_REG = 0;  /* dummy write */

   /* await complete in status register */

   if ( sysMotI2cWait (SYS_I2C_CMPLT) != OK ) 
      return (ERROR);

   return (OK);
   }


/*******************************************************************************
*
* sysMotI2cRangeRead - reads a range of bytes from an I2C serial eeprom (SROM)
*
* This routine simply calls the I2C byte read routine for each requested byte.
* The I2C byte read call is written using a macro to accommodate alternate
* read routines.
*
* RETURNS: OK, or ERROR if the I2C byte read fails.
*
* SEE ALSO: sysI2cSromRangeWrite
*/

STATUS sysMotI2cRangeRead
    (
    UCHAR    devAddr,    /* i2c address of the serial eeprom */
    UCHAR    devOffset,  /* starting offset within the serial eeprom to read */
    UINT16   byteCount,  /* number of bytes to read (one-based) */
    UCHAR   *pBfr,       /* destination buffer */
    UCHAR    num         /* number of bytes to read */
    )
    {

    /* await complete in status register */

    if ( sysMotI2cWait (SYS_I2C_CMPLT) != OK )
        return (ERROR);

    /* initiate start sequence in write mode */

    *(UINT32 *)SYS_I2C_CONTROL_REG = SYS_I2C_START|SYS_I2C_ENBL;
    *(UINT32 *)SYS_I2C_TXD_REG = (UINT32)(devAddr & BYTE_WRITE_MASK);

    /* await complete + acknowledge in status register */

    if ( sysMotI2cWait (SYS_I2C_ACKIN|SYS_I2C_CMPLT) != OK )
        return (ERROR);

    /* devOffset to transmit data register */

    if (num == I2C_DUAL_ADDRESS)
        {
        *(UINT32 *)SYS_I2C_TXD_REG = (UINT32)(devOffset >> 8);

        /* await complete + acknowledge in status register */

        if ( sysMotI2cWait (SYS_I2C_ACKIN|SYS_I2C_CMPLT) != OK )
            return (ERROR);
        }

    *(UINT32 *)SYS_I2C_TXD_REG = (UINT32)(devOffset & 0x00FF);

    /* await complete + acknowledge in status register */

    if ( sysMotI2cWait (SYS_I2C_ACKIN|SYS_I2C_CMPLT) != OK )
       return (ERROR);

    /* initiate start sequence in read mode */

    *(UINT32 *)SYS_I2C_CONTROL_REG = SYS_I2C_START|SYS_I2C_ENBL;
    *(UINT32 *)SYS_I2C_TXD_REG = (UINT32)(devAddr | BYTE_READ_MASK);

    /* await complete + acknowledge in status register */

    if ( sysMotI2cWait (SYS_I2C_ACKIN|SYS_I2C_CMPLT) != OK )
        return (ERROR);

    for ( ; byteCount != 0; --byteCount)
        {

        if (byteCount != 1)
            *(UINT32 *)SYS_I2C_CONTROL_REG = SYS_I2C_ACKOUT|SYS_I2C_ENBL;
        else
            *(UINT32 *)SYS_I2C_CONTROL_REG = SYS_I2C_ENBL;

        /* read the byte */

        *(UINT32 *)SYS_I2C_TXD_REG = 0;    /* dummy write to initiate read */

        /* await complete and datain  in status register */

        if ( sysMotI2cWait (SYS_I2C_CMPLT | SYS_I2C_DATIN) != OK )
            return (ERROR);

        /* the actual read */

        *pBfr++ = (UCHAR) (*(UINT32 *)SYS_I2C_RXD_REG);

        }

    /* stop sequence */

    *(UINT32 *)SYS_I2C_CONTROL_REG = SYS_I2C_STOP|SYS_I2C_ENBL;

    *(UINT32 *)SYS_I2C_TXD_REG = 0;  /* dummy write */

    /* await complete in status register */

    if ( sysMotI2cWait (SYS_I2C_CMPLT) != OK )
       return (ERROR);

    return (OK);
    }


/******************************************************************************
*
* sysMotMsDelay - delay for the specified amount of time (MS)
*
* This routine will delay for the specified amount of time by counting
* decrementer ticks.
*
* This routine is not dependent on a particular rollover value for
* the decrementer, it should work no matter what the rollover
* value is.
*
* A small amount of count may be lost at the rollover point resulting in
* the sysMotMsDelay() causing a slightly longer delay than requested.
*
* This routine will produce incorrect results if the delay time requested
* requires a count larger than 0xffffffff to hold the decrementer
* elapsed tick count.  For a System Bus Speed of 67 MHZ this amounts to
* about 258 seconds.
*
* RETURNS: N/A
*/

void sysMotMsDelay
    (
    UINT        delay                   /* length of time in MS to delay */
    )
    {
    register UINT oldval;	/* decrementer value */
    register UINT newval;       /* decrementer value */
    register UINT totalDelta;           /* Dec. delta for entire delay period */
    register UINT decElapsed;           /* cumulative decrementer ticks */

    /*
     * Calculate delta of decrementer ticks for desired elapsed time.
     */

    totalDelta = ((DEFAULT_BUS_CLOCK / 4) / 1000) * delay;

    /*
     * Now keep grabbing decrementer value and incrementing "decElapsed" until
     * we hit the desired delay value.  Compensate for the fact that we may
     * read the decrementer at 0xffffffff before the interrupt service
     * routine has a chance to set in the rollover value.
     */

    decElapsed = 0;
    oldval = sysMotGetDec();
    while (decElapsed < totalDelta)
        {
        newval = sysMotGetDec();
        if ( DELTA(oldval,newval) < 1000 )
            decElapsed += DELTA(oldval,newval);  /* no rollover */
        else
            if (newval > oldval)
                decElapsed += abs((int)oldval);  /* rollover */
        oldval = newval;
        }
    }



