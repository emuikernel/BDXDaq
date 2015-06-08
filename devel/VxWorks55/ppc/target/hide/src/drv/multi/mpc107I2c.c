/* mpc107I2c.c - MPC107 I2C support */

/* Copyright 1996-2001 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01d,15sep01,dat  Use of vxDecGet routine
01c,11sep00,rcs  fix include paths
01b,22jun00,bri  updated to WindRiver coding standards.
01a,18jun00,bri  created - based on  Kahlua I2C driver.
*/

/*
DESCRIPTION
This module contains routines for the I2C  interface of MPC107.

The I2C interface is a two-wire, bidirectional serial bus that provides
a simple, efficient way to exchange data between integrated circuit(IC) devices.
The I2C interface allows the MPC107 to exchange data with other I 2 C devices
such as microcontrollers, EEPROMs, real-time clock devices, A/D converters,
and LCDs.

This module provides the routines  for writing/reading  data to/from
the I2C interface with MPC107 as a "master"  device .

The routine mpc107i2cWrite() is used for writing  data to an EEPROM type
slave device .
The routine mpc107i2cRead() is used for reading data from an EEPROM type
slave device .

Support for other I2C slave devices can be added easily by using the
routines provided in this module .

Reading/writing data from/to the I2C interface with MPC107 as a "slave" device
are not supported in this module .

However, additional I2C bus controllers can be easily be added as required.

.SH INITIALIZATION
The routine mpc107I2cInit() should be called to  initialize the
IIC interface.

The user has to specify the the following parameters for
programming the I2C Frequency Divider Register (I2CFDR):
.IP DFFSR
Specifies the Digital filter frequency sampling rate of the I2C interface.
DFFSR can be changed by changing MPC107_DFFSR_DATA in the header file.
.IP FDR
Specifies the Frequency divider ratio.FDR can be changed by changing
MPC107_FDR_DATA in the header file .
.LP

The user has to specify the the following parameters for
programming the I2C Address Register (I2CADR) :
.IP ADDR
Specifies the  Slave address used by the I2C interface when MPC107 is configured
as a slave.ADDR can be changed by changing MPC107_ADR_DATA in the header file
.LP

Refer to MPC107 users manual for further details about the values
to be programmed in the I2C registers .
*/

/* includes */

#include "vxWorks.h"
#include "sysLib.h"
#include "stdlib.h"
#include "drv/multi/mpc107.h"
#include "drv/multi/mpc107I2c.h"
#include "drv/intrCtl/mpc107Epic.h"

/* globals  */

/* static file scope locals */

BOOL i2cDoRepeatStart = FALSE;    /* Repeat start */

/* forward Declarations */

LOCAL INT32	mpc107i2cCycleStart (void);
LOCAL void	mpc107i2cCycleStop (void);
LOCAL void 	mpc107i2cCycleRead (UCHAR* data);
LOCAL void 	mpc107i2cCycleWrite (UCHAR data);
LOCAL INT32 	mpc107i2cCycleAckIn (void);
LOCAL void 	mpc107I2cCycleDelay (UINT milliSec);
LOCAL UINT32 	mpc107GetDec (void);
LOCAL UINT32 	mpc107I2cRegRdWrMod (UINT32 operation , UINT32 address ,
                                     UINT32 data1, UINT32 data2);
LOCAL VOID 	mpc107I2cDoOperation (UINT32 deviceAddress,
                                      MPC107_I2C_CMD_PCKT * pI2cCmdPacket);

/***************************************************************************
*
* mpc107I2cInit - initialize the IIC interface
*
* This routine initializes the IIC Interface.
*
* The  "Frequency Divider Register" is initialized depending on
* the clock frequency.The slave address is also programmed. The IIC
* interface is programmed to be in Master/Receive Mode with interrupts disabled.
* After all the initialization is done  the IIC interface is enabled.

* This routine must be called before the user can use the IIC interface to
* read /write data.
*
* RETURNS: N/A
*/

void mpc107I2cInit (void)
    {

    /* Update the frequency divider register */

    mpc107I2cRegRdWrMod(MPC107_I2C_WRITE, MPC107_I2C_I2CFDR,
                           MPC107_I2CFDR_DEFAULT, 0);
    SYNC;

    /* Update the I2CADR to define the slave address for this device */

    mpc107I2cRegRdWrMod(MPC107_I2C_WRITE, MPC107_I2C_I2CADR,
                            MPC107_I2CADR_DEFAULT, 0);
    SYNC;

    /*
     * Update  control register to select Master
     * Receive mode  and interrupt-disabled
     */

    mpc107I2cRegRdWrMod(MPC107_I2C_WRITE, MPC107_I2C_I2CCR,
                            MPC107_I2CCR_DEFAULT,0);
    SYNC;

    /* Enable the I2C interface     */

    mpc107I2cRegRdWrMod(MPC107_I2C_READ_OR_WRITE, MPC107_I2C_I2CCR,
                        MPC107_I2CCR_MEN,0);
    SYNC;

    }


/***************************************************************************
*
* mpc107i2cRead -  read  from the slave device on IIC bus
*
* This routine  reads the specified number of bytes from the specified slave
* device with MPC107 as the master  .
*
* RETURNS: OK, or Error on a bad request
*/

INT32 mpc107i2cRead
    (
    UINT32 	deviceAddress,	/* Device I2C  bus address */
    UINT32 	numBytes,	/* number of Bytes to read */
    char *	pBuf		/* pointer to buffer to receive data */
    )

    {
    MPC107_I2C_CMD_PCKT 	i2cCmdPacket;	/* command packet */

    /* Check for a bad request. */

    if (numBytes == 0)
        {
        return (ERROR);
        }

    /* Build command packet. */

    i2cCmdPacket.command = MPC107_I2C_READOP;
    i2cCmdPacket.status = 0;
    i2cCmdPacket.memoryAddress = (UINT32)pBuf;
    i2cCmdPacket.nBytes = numBytes;
    mpc107I2cDoOperation(deviceAddress, (MPC107_I2C_CMD_PCKT *)&i2cCmdPacket);

    /* Return the appropriate status. */

    return (i2cCmdPacket.status);
    }


/***************************************************************************
*
* mpc107i2cWrite - write to the slave device on IIC bus
*
* This routine is used to write the specified number of
* bytes to the specified slave device with  MPC 107 as a master
*
* RETURNS: OK, or ERROR on a bad request.
*/

INT32 mpc107i2cWrite
    (
    UINT32 	deviceAddress,	/* Devices I2C bus address */
    UINT32 	numBytes,	/* number of bytes to write */
    char *	pBuf		/* pointer to buffer of send data */
    )
    {
    MPC107_I2C_CMD_PCKT 	i2cCmdPacket;	/* command packet */

    /* Check for a NOP request. */

    if (numBytes == 0)
        {
        return (ERROR);
        }

        /* Build command packet. */

    i2cCmdPacket.command = MPC107_I2C_WRITOP;
    i2cCmdPacket.status = 0;
    i2cCmdPacket.memoryAddress = (UINT32)pBuf;
    i2cCmdPacket.nBytes = numBytes;

    /* Take ownership, call driver, release ownership. */

     mpc107I2cDoOperation(deviceAddress, (MPC107_I2C_CMD_PCKT *)&i2cCmdPacket);

    /* Return the appropriate status. */

    return (i2cCmdPacket.status);

    }



/***************************************************************************
*
* mpc107i2cCycleRead - perform I2C "read" cycle
*
* This routine is used to  perform a read from the I2C bus .
*
* RETURNS: N/A
*/

LOCAL void mpc107i2cCycleRead
    (
    UCHAR *	pReadDataBuf	/* pointer to read data buffer */
    )
    {
    UINT32 readData = 0;

    mpc107I2cCycleDelay (1);

    /*
     * Configure the I2C interface into receive mode(MTX=0) and set
     * the interface  to NOT acknowledge(TXAK=1) the incoming data on
     * the 9th clock cycle.
     * This is required when doing random reads of a I2C device.
     */

    mpc107I2cRegRdWrMod(MPC107_I2C_READ_ANDOR_WRITE,
                        MPC107_I2C_I2CCR,~(MPC107_I2CCR_MTX),
                        MPC107_I2CCR_TXAK);
    SYNC;

    mpc107I2cCycleDelay (1);

    /* Perform  a  "dummy read". This latches the data off the I2C  bus. */

    mpc107I2cRegRdWrMod(MPC107_I2C_READ,MPC107_I2C_I2CDR, 0, 0);
    SYNC;

    mpc107I2cCycleDelay (1);

    /* Perform the actual read */

    readData = mpc107I2cRegRdWrMod(MPC107_I2C_READ, MPC107_I2C_I2CDR, 0, 0);
    SYNC;

    *pReadDataBuf = (UCHAR)readData; /* copy the read data */

    return;

    }


/***************************************************************************
*
* mpc107i2cCycleWrite - perform I2C "write" cycle
*
* This routine is used is to perform a write to the I2C bus.
* Data is written to the I2C interface .
*
* RETURNS: N/A
*/

LOCAL void mpc107i2cCycleWrite
    (
    UCHAR writeData	/* character to write */
    )
    {

    mpc107I2cCycleDelay (1);

    /*
     * Write the requested data to the data register, which will cause
     * it to be transmitted on the I2C bus.
     */

    mpc107I2cRegRdWrMod(MPC107_I2C_WRITE, MPC107_I2C_I2CDR, writeData, 0);
    SYNC;

    mpc107I2cCycleDelay (1);

    return;

    }


/***************************************************************************
*
* mpc107i2cCycleAckIn - perform I2C "acknowledge-in" cycle
*
* This routine is used to perform an I2C acknowledge-in  on the bus.
*
* RETURNS: OK, or ERROR if timed out waiting for an ACK .
*/

LOCAL STATUS mpc107i2cCycleAckIn (void)
    {
    UINT32 	statusReg = 0;
    UINT32 	timeOutCount;

    /*
     * wait until an *internal* device interrupt has been generated, then
     * clear it.  if it is not received, return with an error.
     * we are polling, so NO processor interrupt is generated.
     */

    for (timeOutCount = 100; timeOutCount; timeOutCount--)
        {
        mpc107I2cCycleDelay (1);

        statusReg = mpc107I2cRegRdWrMod(MPC107_I2C_READ, MPC107_I2C_I2CSR,0,0);
        SYNC;

        if ((statusReg & MPC107_I2CSR_MIF) != 0)
            {
            mpc107I2cRegRdWrMod(MPC107_I2C_READ_AND_WRITE,
                                MPC107_I2C_I2CSR,~(MPC107_I2CSR_MIF),0);
            SYNC;
            break;
            }
        }

    if (timeOutCount == 0)
        {
        return (ERROR);
        }

    return (OK);

    }


/***************************************************************************
*
* mpc107i2cCycleStart - perform I2C "start" cycle
*
* This routine is used is to perform an I2C start cycle.
*
* RETURNS: OK, or ERROR if timed out waiting for the IIC bus to be free.
*/

LOCAL STATUS mpc107i2cCycleStart (void)
    {
    UINT32 	timeOutCount;
    UINT32 	statusReg = 0;

    /*
     * if this is a repeat start, then set the required bits and return.
     * this driver ONLY supports one repeat start between the start cycle and
     * stop cycles.
     */

    if(i2cDoRepeatStart)  /* If a repeat start */
        {
        mpc107I2cCycleDelay (1);

        mpc107I2cRegRdWrMod(MPC107_I2C_READ_OR_WRITE, MPC107_I2C_I2CCR,
                    (MPC107_I2CCR_RSTA | MPC107_I2CCR_MSTA |
                     MPC107_I2CCR_MTX),0);
        SYNC;
        mpc107I2cCycleDelay (1);
        i2cDoRepeatStart = FALSE;  /* one repeat start only, so clear this */
        return(OK);
        }

    /*
     * wait until the I2C bus is free.  if it doesn't become free
     * within a *reasonable* amount of time, exit with an error.
     */

    for (timeOutCount = 10; timeOutCount; timeOutCount--)
        {
        mpc107I2cCycleDelay (1);

        statusReg = mpc107I2cRegRdWrMod(MPC107_I2C_READ,
                                        MPC107_I2C_I2CSR, 0, 0);
        SYNC;

        if ((statusReg & MPC107_I2CSR_MBB) == 0)
            {
            break;
            }
        }

    if (timeOutCount == 0)
        {
        return (ERROR);
        }

    /*
     * since this is the first time through, generate a START(MSTA) and
     * place the I2C interface into a master transmitter mode(MTX).
     */

    mpc107I2cRegRdWrMod(MPC107_I2C_READ_OR_WRITE, MPC107_I2C_I2CCR,
                       (MPC107_I2CCR_MTX | MPC107_I2CCR_MSTA),0);
    SYNC;
    mpc107I2cCycleDelay (1);

    /*
     * The first time through, set "i2cDoRepeatStart".  If this routine
     * is called again BEFORE a STOP is sent, then we are doing a
     * "dummy write", which sets the devices internal byte pointer
     * to the byte we intend to read.
     */

    i2cDoRepeatStart = TRUE;

    return (OK);

    }


/***************************************************************************
*
* mpc107i2cCycleStop - perform I2C "stop" cycle
*
* This routine is used is to perform an I2C stop cycle.
*
* RETURNS: N/A
*/

LOCAL void mpc107i2cCycleStop (void)
    {

    mpc107I2cCycleDelay (1);

    /*
     * turn off MSTA bit(which will generate a STOP bus cycle)
     * turn off MTX bit(which places the I2C interface into receive mode
     * turn off TXAK bit(which allows 9th clock cycle acknowledges)
     */

    mpc107I2cRegRdWrMod(MPC107_I2C_READ_AND_WRITE, MPC107_I2C_I2CCR,
                       (~(MPC107_I2CCR_MTX  | MPC107_I2CCR_MSTA |
                          MPC107_I2CCR_TXAK)), 0);
    SYNC;
    mpc107I2cCycleDelay (1);

    /* Clear the global I2C "Repeat Start" flag.*/

    i2cDoRepeatStart = FALSE;

    return ;

    }


/***************************************************************************
*
* mpc107i2cRegRdWrMod - i2c Registers In/OutLong and/or-ing wrapper.
*
* The purpose of this routine is to perform AND, OR, and
* AND/OR  or In/Out operations with syncronization.
*
* RETURNS: The data read for read operations.
*/

LOCAL UINT32 mpc107I2cRegRdWrMod
    (
    UINT32 	ioControlFlag,  /* input/ouput control flag         */
                                /* 0, write                         */
                                /* 1, read                          */
                                /* 2, read/modify/write (ORing)     */
                                /* 3, read/modify/write (ANDing)    */
                                /* 4, read/modify/write (AND/ORing) */
    UINT32 	address,        /* address of device register       */
    UINT32 	wdata1,         /* data item 1 for read/write operation */
    UINT32 	wdata2          /* data item 2 for read/write operation */
    )
    {
    UINT32 i2cTempData = 0;

    if (ioControlFlag == MPC107_I2C_WRITE)  /* Write */
        {

        /*
         * Data <wdata1> is to be written in
         * the register specified by <address>
         */

       MPC107EUMBBARWRITE(address, wdata1);


        }
    else if (ioControlFlag == MPC107_I2C_READ) /* Read */
        {

        /* Data is read from the register  specified by <address> */

        i2cTempData =MPC107EUMBBARREAD(address);


        }
    else if (ioControlFlag == MPC107_I2C_READ_OR_WRITE) /* Read OR Write */
        {

        /*
         * Data <wdata1> is bitwise ORed with the data read
         * from the register specified by <address>  and the
         * resultant  data is written to the register
         */

        i2cTempData =MPC107EUMBBARREAD(address) | wdata1 ;
        SYNC;
        MPC107EUMBBARWRITE(address, i2cTempData);
        }

    else if (ioControlFlag == MPC107_I2C_READ_AND_WRITE)  /* Read AND Write */
        {

        /*
         * Data <wdata1> is bitwise ANDed with the data read
         * from the register specified by address  and the
         * resultant  data is written to the  register
         */

        i2cTempData =MPC107EUMBBARREAD(address) & wdata1;
        SYNC;
        MPC107EUMBBARWRITE(address, i2cTempData);

        }

    else if (ioControlFlag == MPC107_I2C_READ_ANDOR_WRITE)/* Read ANDOR write */
        {

        /*
         * Data <wdata1> is bitwise ANDed with the data read
         * from the register specified by <address> and  data
         * <wdata2> is bitwise ORed, and the resultant  data
         * is written to the register
         */


        i2cTempData =MPC107EUMBBARREAD(address);
        SYNC;
        i2cTempData &= wdata1;
        i2cTempData |= wdata2;
        MPC107EUMBBARWRITE(address, i2cTempData);

        }

    SYNC;

    return (i2cTempData);

    }


/***************************************************************************
*
* mpc107I2cDoOperation - i2c do operation
*
* The  purpose of this routine is to execute the operation as specified
* by the passed command packet.
*
* RETURNS: N/A
*/

LOCAL VOID mpc107I2cDoOperation
    (
    UINT32 		deviceAddress,	/* device I2C bus address */
    MPC107_I2C_CMD_PCKT * pI2cCmdPacket	/* pointer to command packet */
    )
    {
    INT32 	byteCount;	/* byte counter */
    INT32 	statusVariable;	/* local status variable */
    UCHAR *	pWriteData;	/* pointer to write data buffer */

    /* Command interface to stop.  This is for previous error exits. */

    mpc107i2cCycleStop ();

    if (pI2cCmdPacket->command == MPC107_I2C_READOP) /* read operation  */
        {

        statusVariable = 0;

        for (byteCount = 0; byteCount < pI2cCmdPacket->nBytes; byteCount++)
            {

            /* Start cycle */

            if ((mpc107i2cCycleStart ()) != 0)
                {
                statusVariable = MPC107_I2C_ERR_CYCLE_START;
                break;
	        }

            /* Write the device address */

            mpc107i2cCycleWrite (deviceAddress);


            if ((mpc107i2cCycleAckIn ()) != 0)
                {
                statusVariable = MPC107_I2C_ERR_CYCLE_ACKIN;
                break;
	        }

            /* Write the byte offset */

            mpc107i2cCycleWrite (byteCount);

            if ((mpc107i2cCycleAckIn ()) != 0)
                {
                statusVariable = MPC107_I2C_ERR_CYCLE_ACKIN;
                break;
	        }

            /* Write the device address */

            mpc107i2cCycleWrite (deviceAddress |
                                    MPC107_I2C_DEVICE_READ_ENABLE);

	    if ((mpc107i2cCycleAckIn ()) != 0)
                {
                statusVariable = MPC107_I2C_ERR_CYCLE_ACKIN;
                break;
	        }

            /* Read the data from the IIC interface */

	    mpc107i2cCycleRead ((UCHAR *)
                               (pI2cCmdPacket->memoryAddress + byteCount));
                {
                statusVariable = MPC107_I2C_ERR_CYCLE_READ;
                break;
	        }

            /* Generate the Stop Cycle */

	    mpc107i2cCycleStop () ;

            }


        /* update the caller's command packet with status of the operation */

        pI2cCmdPacket->status = statusVariable;
        }

    /*
     * write operation for each byte
     * perform the byte write operation, a delay must be
     * exercised following each byte write
     */

    if (pI2cCmdPacket->command == MPC107_I2C_WRITOP)
        {

        /* Initialize pointer to caller's write data buffer. */

        pWriteData = (UCHAR *)pI2cCmdPacket->memoryAddress;

        /* Write the specified number of bytes from the EEPROM. */

        statusVariable = 0;

        for (byteCount = 0; byteCount < pI2cCmdPacket->nBytes; byteCount++)
            {

            /* Generate the Start Cycle */

            if ((mpc107i2cCycleStart ()) != 0)
                {
                statusVariable = MPC107_I2C_ERR_CYCLE_START;
                break;
	        }

            /* Write the device address */

            mpc107i2cCycleWrite (deviceAddress);

            if ((mpc107i2cCycleAckIn ()) != 0)
               {
               statusVariable = MPC107_I2C_ERR_CYCLE_ACKIN;
               break;
	       }

            /* Write the byte offset */

            mpc107i2cCycleWrite (byteCount);

            if ((mpc107i2cCycleAckIn()) != 0)
               {
               statusVariable = MPC107_I2C_ERR_CYCLE_ACKIN;
               break;
	       }

            /* Write the device address */

            mpc107i2cCycleWrite (deviceAddress);

	    if ((mpc107i2cCycleAckIn ()) != 0)
                {
                statusVariable = MPC107_I2C_ERR_CYCLE_ACKIN;
                break;
	        }

            /* Write the data */

	    mpc107i2cCycleWrite (*(pWriteData + byteCount));

	    if ((mpc107i2cCycleAckIn ()) != 0)
                {
                statusVariable = MPC107_I2C_ERR_CYCLE_ACKIN;
                break;
	        }

            /* Generate the Stop Cycle */

            mpc107i2cCycleStop ();

            }


        /* update the caller's command packet with status of the operation */

        pI2cCmdPacket->status = statusVariable;
        }

    }


/***************************************************************************
*
* mpc107I2cCycleDelay() - delay for the specified amount of time (MS)
*
* This routine will delay for the specified amount of time by counting
* decrementer ticks.
*
* This routine is not dependent on a particular rollover value for
* the decrementer, it should work no matter what the rollover
* value is.
*
* A small amount of count may be lost at the rollover point resulting in
* the mpc107I2cCycleDelay() causing a slightly longer delay than requested.
*
* This routine will produce incorrect results if the delay time requested
* requires a count larger than 0xffffffff to hold the decrementer
* elapsed tick count.  For a System Bus Speed of 67 MHZ this amounts to
* about 258 seconds.
*
* RETURNS: N/A
*/

LOCAL void mpc107I2cCycleDelay
    (
    UINT        delay            /* length of time in MS to delay */
    )
    {
    register UINT32 	oldval;      /* decrementer value */
    register UINT32 	newval;      /* decrementer value */
    register UINT32 	totalDelta;  /* Dec. delta for entire delay period */
    register UINT32 	decElapsed;  /* cumulative decrementer ticks */

    /* Calculate delta of decrementer ticks for desired elapsed time. */

    totalDelta = ((MPC107_DEFAULT_BUS_CLOCK / 4) / 1000) * delay;

    /*
     * Now keep grabbing decrementer value and incrementing "decElapsed" until
     * we hit the desired delay value.  Compensate for the fact that we may
     * read the decrementer at 0xffffffff before the interrupt service
     * routine has a chance to set in the rollover value.
     */

    decElapsed = 0;
    oldval = mpc107GetDec ();
    while (decElapsed < totalDelta)
        {
        newval = mpc107GetDec();
        if ( MPC107_DELTA(oldval,newval) < 1000 )
            decElapsed += MPC107_DELTA(oldval,newval);  /* no rollover */
        else
            if (newval > oldval)
                decElapsed += abs((INT32)oldval);  /* rollover */
        oldval = newval;
        }
  }


/***************************************************************************
*
* mpc107GetDec - read from the Decrementer register SPR22.
*
* This routine will read the contents the decrementer (SPR22)
*
* From a C point of view, the routine is defined as follows:
*
* UINT32 mpc107GetDec()
*
* RETURNS: value of SPR22 (in r3)
*/

LOCAL UINT32 mpc107GetDec(void)
    {
    return vxDecGet ();
    }

