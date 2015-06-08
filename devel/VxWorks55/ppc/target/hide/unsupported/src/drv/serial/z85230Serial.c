/* z85230Serial.c - Zilog z85230 ESCC tty handler (same as Intel i82530) */

/* Copyright 1984-1993 Wind River Systems, Inc. */
extern char copyright_wind_river[]; static char *copyright=copyright_wind_river;

/*
modification history
--------------------
01a,08aug93,jpb  written based on sun3/tyCoDrv.c (z8530) version 01e.
*/

/*
DESCRIPTION
This is the driver for the Zilog z85230 ESCC serial device.

USER CALLABLE ROUTINES
Most of the routines in this driver are accessible only through the I/O
system.  Two routines, however, must be called directly, tyCoDrv to
initialize the driver, and tyCoDevCreate to create devices.

TYCODRV
Before using the driver, it must be initialized by calling the routine:
.CS
    tyCoDrv ()
.CE
This routine should be called exactly once, before any reads, writes, or
tyCoDevCreates.  Normally, it is called from usrRoot.

CREATING TERMINAL DEVICES
Before a terminal can be used, it must be created.  This is done
with the tyCoDevCreate call.
Each port to be used should have exactly one device associated with it,
by calling this routine.

.CS
    STATUS tyCoDevCreate (name, channel, rdBufSize, wrtBufSize)
	char *name;	/* Name to use for this device *
	int channel;	/* Physical channel for this device, 0 or 1 *
	int rdBufSize;	/* Read buffer size, in bytes *
	int wrtBufSize;	/* Write buffer size, in bytes *
.CE

For instance, to create the device "/tyCo/0", with buffer sizes of 512 bytes,
the proper call would be:
.CS
   tyCoDevCreate ("/tyCo/0", 0, 512, 512);
.CE
IOCTL
This driver responds to all the same ioctl codes as a normal ty driver.
The DUARTs have countdown clocks from a base frequency of 4.9152 MHz to
set the baud rates, so all baud rates from 50 to 38400 are selectable.
*/

#include "vxWorks.h"
#include "ioLib.h"
#include "tyLib.h"
#include "config.h"

#define DEFAULT_BAUD 9600

/* CONSOLE_TTY is 13 */

/*
 * Accesses to the 85230 must be at least 4 * 1/BAUD_CLK_RATE seconds apart.
 * with a baud clock rate of 9.6 MHz, two consecutive instructions which 
 * access the chip, violate this constraint.  These macros are used to ensure 
 * a delay between access.  They should be used for all reads and writes to 
 * the 85230's.
 */

#define DELAY asm("nop")
#define WRITE_REG(addr,reg,value) *(addr) = (reg); DELAY; *(addr) = (value); DELAY;
#define WRITE_REG_0(addr,value) *(addr) = (value); DELAY;
#define WRITE_DATA(addr,value) value = *(addr); DELAY;

IMPORT FUNCPTR sysAuxClkRoutine;
IMPORT int sysAuxClkArg;

typedef struct			/* TY_CO_DEV */
    {
    TY_DEV tyDev;
    BOOL created;	/* true if this device has really been created */
    int channel;	/* channel number */
    char vector;	/* vector base */
    char *cp;		/* control port */
    char *dp;		/* data port */
    RIND_ID rdBuf;	/* receive ring buffer */
    RIND_ID wrBuf;	/* transmit ring buffer */
    } TY_CO_DEV;

LOCAL TY_CO_DEV tyCoDv [N_USART_CHANNELS] =	/* device descriptors */
    {
    {{{{NULL}}}, FALSE,  0, 0x40, SBPM_SCCA, SBPM_SCDA, NULL, NULL}, /* SCC 1 */
    {{{{NULL}}}, FALSE,  1, 0x40, SBPM_SCCB, SBPM_SCDB, NULL, NULL},
    {{{{NULL}}}, FALSE,  2, 0x50, SBPM_SCCC, SBPM_SCDC, NULL, NULL}, /* SCC 2 */
    {{{{NULL}}}, FALSE,  3, 0x50, SBPM_SCCD, SBPM_SCDD, NULL, NULL},
    {{{{NULL}}}, FALSE,  4, 0x60, SBPM_SCCE, SBPM_SCDE, NULL, NULL}, /* SCC 3 */
    {{{{NULL}}}, FALSE,  5, 0x60, SBPM_SCCF, SBPM_SCDF, NULL, NULL},
    {{{{NULL}}}, FALSE,  6, 0x70, SBPM_SCCG, SBPM_SCDG, NULL, NULL}, /* SCC 4 */
    {{{{NULL}}}, FALSE,  7, 0x70, SBPM_SCCH, SBPM_SCDH, NULL, NULL},
    {{{{NULL}}}, FALSE,  8, 0x80, SBPM_SCCI, SBPM_SCDI, NULL, NULL}, /* SCC 5 */
    {{{{NULL}}}, FALSE,  9, 0x80, SBPM_SCCJ, SBPM_SCDJ, NULL, NULL},
    {{{{NULL}}}, FALSE, 10, 0x90, SBPM_SCCK, SBPM_SCDK, NULL, NULL}, /* SCC 6 */
    {{{{NULL}}}, FALSE, 11, 0x90, SBPM_SCCL, SBPM_SCDL, NULL, NULL},
    {{{{NULL}}}, FALSE, 12, 0xA0, SBPM_SCCM, SBPM_SCDM, NULL, NULL}, /* SCC 7 */
    {{{{NULL}}}, FALSE, 13, 0xA0, SBPM_SCCN, SBPM_SCDN, NULL, NULL}
    };

LOCAL int tyCoDrvNum;		/* driver number assigned to this driver */
LOCAL receiveEnabled[N_USART_CHANNELS]; /* flag to indicate ok to receive data */

/* forward declarations */

LOCAL int tyCoOpen ();
LOCAL STATUS tyCoIoctl ();
LOCAL VOID tyCoStartup ();
LOCAL VOID tyCoHrdInit ();
LOCAL VOID asynchInit ();
LOCAL VOID synchInit ();
LOCAL int tyCoRead ();
LOCAL int tyCoWrite ();

LOCAL VOID scc1aTxInt();
LOCAL VOID scc1aRxInt();
LOCAL VOID scc1aExInt();
LOCAL VOID scc1bTxInt();
LOCAL VOID scc1bRxInt();
LOCAL VOID scc1bExInt();

LOCAL VOID scc2aTxInt();
LOCAL VOID scc2aRxInt();
LOCAL VOID scc2aExInt();
LOCAL VOID scc2bTxInt();
LOCAL VOID scc2bRxInt();
LOCAL VOID scc2bExInt();

LOCAL VOID scc3aTxInt();
LOCAL VOID scc3aRxInt();
LOCAL VOID scc3aExInt();
LOCAL VOID scc3bTxInt();
LOCAL VOID scc3bRxInt();
LOCAL VOID scc3bExInt();

LOCAL VOID scc4aTxInt();
LOCAL VOID scc4aRxInt();
LOCAL VOID scc4aExInt();
LOCAL VOID scc4bTxInt();
LOCAL VOID scc4bRxInt();
LOCAL VOID scc4bExInt();

LOCAL VOID scc5aTxInt();
LOCAL VOID scc5aRxInt();
LOCAL VOID scc5aExInt();
LOCAL VOID scc5bTxInt();
LOCAL VOID scc5bRxInt();
LOCAL VOID scc5bExInt();

LOCAL VOID scc6aTxInt();
LOCAL VOID scc6aRxInt();
LOCAL VOID scc6aExInt();
LOCAL VOID scc6bTxInt();
LOCAL VOID scc6bRxInt();
LOCAL VOID scc6bExInt();

LOCAL VOID scc7aTxInt();
LOCAL VOID scc7aRxInt();
LOCAL VOID scc7aExInt();
LOCAL VOID scc7bTxInt();
LOCAL VOID scc7bRxInt();
LOCAL VOID scc7bExInt();

LOCAL VOID tyTxInt ();
LOCAL VOID tyRxInt ();
LOCAL VOID tyExInt ();

/*******************************************************************************
*
* tyCoDrv - ty driver initialization routine
*
* This routine initializes the serial driver, sets up interrupt vectors,
* and performs hardware initialization of the serial ports.
*
* RETURNS: OK or ERROR if unable to install driver.
*/

STATUS tyCoDrv ()

    {
    /* check if driver already installed */

    if (tyCoDrvNum > 0)
	return (OK);

    /* SCC 1 interrupt connections */
    (void) intConnect (INUM_TO_IVEC (INT_VEC_1A_TX), scc1aTxInt, NULL);
    (void) intConnect (INUM_TO_IVEC (INT_VEC_1A_RX), scc1aRxInt, NULL);
    (void) intConnect (INUM_TO_IVEC (INT_VEC_1A_EX), scc1aExInt, NULL);
    (void) intConnect (INUM_TO_IVEC (INT_VEC_1B_TX), scc1bTxInt, NULL);
    (void) intConnect (INUM_TO_IVEC (INT_VEC_1B_RX), scc1bRxInt, NULL);
    (void) intConnect (INUM_TO_IVEC (INT_VEC_1B_EX), scc1bExInt, NULL);

    /* SCC 2 interrupt connections */
    (void) intConnect (INUM_TO_IVEC (INT_VEC_2A_TX), scc2aTxInt, NULL);
    (void) intConnect (INUM_TO_IVEC (INT_VEC_2A_RX), scc2aRxInt, NULL);
    (void) intConnect (INUM_TO_IVEC (INT_VEC_2A_EX), scc2aExInt, NULL);
    (void) intConnect (INUM_TO_IVEC (INT_VEC_2B_TX), scc2bTxInt, NULL);
    (void) intConnect (INUM_TO_IVEC (INT_VEC_2B_RX), scc2bRxInt, NULL);
    (void) intConnect (INUM_TO_IVEC (INT_VEC_2B_EX), scc2bExInt, NULL);

    /* SCC 3 interrupt connections */
    (void) intConnect (INUM_TO_IVEC (INT_VEC_3A_TX), scc3aTxInt, NULL);
    (void) intConnect (INUM_TO_IVEC (INT_VEC_3A_RX), scc3aRxInt, NULL);
    (void) intConnect (INUM_TO_IVEC (INT_VEC_3A_EX), scc3aExInt, NULL);
    (void) intConnect (INUM_TO_IVEC (INT_VEC_3B_TX), scc3bTxInt, NULL);
    (void) intConnect (INUM_TO_IVEC (INT_VEC_3B_RX), scc3bRxInt, NULL);
    (void) intConnect (INUM_TO_IVEC (INT_VEC_3B_EX), scc3bExInt, NULL);

    /* SCC 4 interrupt connections */
    (void) intConnect (INUM_TO_IVEC (INT_VEC_4A_TX), scc4aTxInt, NULL);
    (void) intConnect (INUM_TO_IVEC (INT_VEC_4A_RX), scc4aRxInt, NULL);
    (void) intConnect (INUM_TO_IVEC (INT_VEC_4A_EX), scc4aExInt, NULL);
    (void) intConnect (INUM_TO_IVEC (INT_VEC_4B_TX), scc4bTxInt, NULL);
    (void) intConnect (INUM_TO_IVEC (INT_VEC_4B_RX), scc4bRxInt, NULL);
    (void) intConnect (INUM_TO_IVEC (INT_VEC_4B_EX), scc4bExInt, NULL);

    /* SCC 5 interrupt connections */
    (void) intConnect (INUM_TO_IVEC (INT_VEC_5A_TX), scc5aTxInt, NULL);
    (void) intConnect (INUM_TO_IVEC (INT_VEC_5A_RX), scc5aRxInt, NULL);
    (void) intConnect (INUM_TO_IVEC (INT_VEC_5A_EX), scc5aExInt, NULL);
    (void) intConnect (INUM_TO_IVEC (INT_VEC_5B_TX), scc5bTxInt, NULL);
    (void) intConnect (INUM_TO_IVEC (INT_VEC_5B_RX), scc5bRxInt, NULL);
    (void) intConnect (INUM_TO_IVEC (INT_VEC_5B_EX), scc5bExInt, NULL);

    /* SCC 6 interrupt connections */
    (void) intConnect (INUM_TO_IVEC (INT_VEC_6A_TX), scc6aTxInt, NULL);
    (void) intConnect (INUM_TO_IVEC (INT_VEC_6A_RX), scc6aRxInt, NULL);
    (void) intConnect (INUM_TO_IVEC (INT_VEC_6A_EX), scc6aExInt, NULL);
    (void) intConnect (INUM_TO_IVEC (INT_VEC_6B_TX), scc6bTxInt, NULL);
    (void) intConnect (INUM_TO_IVEC (INT_VEC_6B_RX), scc6bRxInt, NULL);
    (void) intConnect (INUM_TO_IVEC (INT_VEC_6B_EX), scc6bExInt, NULL);

    /* SCC 7 interrupt connections */
    (void) intConnect (INUM_TO_IVEC (INT_VEC_7A_TX), scc7aTxInt, NULL);
    (void) intConnect (INUM_TO_IVEC (INT_VEC_7A_RX), scc7aRxInt, NULL);
    (void) intConnect (INUM_TO_IVEC (INT_VEC_7A_EX), scc7aExInt, NULL);
    (void) intConnect (INUM_TO_IVEC (INT_VEC_7B_TX), scc7bTxInt, NULL);
    (void) intConnect (INUM_TO_IVEC (INT_VEC_7B_RX), scc7bRxInt, NULL);
    (void) intConnect (INUM_TO_IVEC (INT_VEC_7B_EX), scc7bExInt, NULL);

    tyCoHrdInit ();

    tyCoDrvNum = iosDrvInstall (tyCoOpen, (FUNCPTR) NULL, tyCoOpen,
				(FUNCPTR) NULL, tyRead, tyWrite, tyCoIoctl);

    return (tyCoDrvNum == ERROR ? ERROR : OK);
    }
/*******************************************************************************
*
* tyCoDevCreate - create a device for the onboard ports
*
* This routine creates a device on one of the serial ports.  Each port
* to be used should have exactly one device associated with it, by calling
* this routine.
*
* RETURNS: OK or ERROR if driver not installed or invalid channel.
*/

STATUS tyCoDevCreate (name, channel, rdBufSize, wrtBufSize)
    char *name;		/* Name to use for this device */
    int channel;	/* Physical channel for this device (0 or 1) */
    int rdBufSize;	/* Read buffer size, in bytes */
    int wrtBufSize;	/* Write buffer size, in bytes */

    {
    if (tyCoDrvNum <= 0)
	{
	errnoSet (S_ioLib_NO_DRIVER);
	return (ERROR);
	}

    if (channel < 0 || channel > N_USART_CHANNELS - 1)
	return (ERROR);

    /* if this device already exists, don't create it */

    if (tyCoDv [channel].created)
	return (ERROR);

    /*
     * initialize the ty descriptor, and turn on the bit for this
     * receiver and transmitter in the interrupt mask
     */

    if ((channel >= 0) && (channel <= NUM_TTY) && (channel != CONSOLE_TTY)) 
	{
	bzero ((char *)&tyCoDv[channel].tyDev, sizeof(TY_DEV));

	if ((tyCoDv[channel].rdBuf = rngCreate(rdBufSize)) == NULL)
	    return (ERROR);
	if ((tyCoDv[channel].wrBuf = rngCreate(wrtBufSize)) == NULL)
	    return (ERROR);
	
	rngFlush (tyCoDv[channel].rdBuf);
	rngFlush (tyCoDv[channel].wrBuf);
	}
    else 
	{
	if (tyDevInit (&tyCoDv[channel].tyDev,
	  rdBufSize, wrtBufSize, tyCoStartup) != OK)
	    return (ERROR);
	}

    /* mark the device as created, and add the device to the I/O system */

    tyCoDv [channel].created = TRUE;

    return (iosDevAdd (&tyCoDv [channel].tyDev.devHdr, name, tyCoDrvNum));
    }
/**********************************************************************
*
* tyCoHrdInit - initialize the USARTs
*/

LOCAL VOID tyCoHrdInit ()

    {
    int ix;
    int delay;		/* delay for reset */
    char *cp;
    int oldLevel;	/* current interrupt level mask */
    char zero = 0;

    oldLevel = intLock ();	/* disable interrupts during init */

    /*  reset all SCC's */
     
    for (ix = 0; ix < N_USART_CHANNELS; ix++)
	{
	cp = tyCoDv [ix].cp;
	WRITE_REG(cp, SCC_WR0_SEL_WR9, 0);	/* disable interrupts */
	}
	
    for (ix = 0; ix < N_USART_CHANNELS; ix += 2)
	{
	cp = tyCoDv [ix].cp;
	WRITE_REG(cp, SCC_WR0_SEL_WR9, SCC_WR9_HDWR_RST); /* master int cntrl */
	}
	
    /*
     * setup console for asynch all others for synch {TBD}
     */

    for (ix = 0; ix <= NUM_TTY; ++ix)
	if (ix == CONSOLE_TTY)
	    asynchInit (CONSOLE_TTY);
	else
	    synchInit (ix);
    
    intUnlock (oldLevel);
    } 

/*******************************************************************************
*
* asynchInit - initialize channel for asynchronous operation.
*
* ARGSUSED1
*/

LOCAL void asynchInit (channel)
    int channel;
    {
    int delay;
    char *cp;
    int baudConstand;

    cp = tyCoDv[channel].cp;

    /* setup for asynchronous operation */

    WRITE_REG(cp, SCC_WR0_SEL_WR4, SCC_WR4_EXT_SYNC|SCC_WR4_32_CLOCK); /* misc parms */
    WRITE_REG(cp, SCC_WR0_SEL_WR2, tyCoDv[channel].vector); /* interrupt vec */
    WRITE_REG(cp, SCC_WR0_SEL_WR3, SCC_WR3_RX_8_BITS);	/* RX params */
    WRITE_REG(cp, SCC_WR0_SEL_WR5, SCC_WR5_TX_8_BITS);	/* TX params */
    WRITE_REG(cp, SCC_WR0_SEL_WR9, SCC_WR9_NO_RST);	/* master int */
    WRITE_REG(cp, SCC_WR0_SEL_WR1, 0);			/* TX/RX int */
    WRITE_REG(cp, SCC_WR0_SEL_WR10, 0);		/* misc TX/RX */

    /* setup baud rate generator */
    
    baudConstant = ((BAUD_CLK_FREQ / 64) / DEFAULT_BAUD) - 2;
    WRITE_REG(cp, SCC_WR0_SEL_WR11, SCC_WR11_RX_BR_GEN | SCC_WR11_TX_BR_GEN |
	       SCC_WR11_TRXC_OI | SCC_WR11_OUT_BR_GEN);     /* clock */
    WRITE_REG(cp, SCC_WR0_SEL_WR12, (char)baudConstant);      /* baud lsb */
    WRITE_REG(cp, SCC_WR0_SEL_WR13, (char)(baudConstant>>8)); /* baud msb */
    WRITE_REG(cp, SCC_WR0_SEL_WR14, SCC_WR14_RX_BR_SRC | SCC_WR14_RST_CLK); /* misc */
    WRITE_REG(cp, SCC_WR0_SEL_WR14, SCC_WR14_BR_SRC | SCC_WR14_RST_CLK |
				     SCC_WR14_BR_EN);

    /*
     * enable receivers and transmitters on both channels
     */

    WRITE_REG(cp,SCC_WR0_SEL_WR3,SCC_WR3_RX_8_BITS | SCC_WR3_RX_EN); /* enable RX */
    WRITE_REG(cp,SCC_WR0_SEL_WR5,SCC_WR5_TX_8_BITS | SCC_WR5_TX_EN); /* enable TX */
    WRITE_REG(cp,SCC_WR0_SEL_WR1,SCC_WR1_INT_ALL_RX | SCC_WR1_TX_INT_EN); /* enable RX */

    WRITE_REG(cp, SCC_WR0_SEL_WR9,SCC_WR9_MIE | SCC_WR9_VIS); /* enable interrupts */

    }
/*******************************************************************************
*
* synchInit - initialize channel for asynchronous operation.
*
* ARGSUSED1
*/

LOCAL void synchInit (channel)
    int channel;
    {
    int delay;
    char *cp;
    int baudConstand;

    cp = tyCoDv[channel].cp;

    /* setup for synchronous operation */

    WRITE_REG(cp, SCC_WR0_SEL_WR4, SCC_WR4_EXT_SYNC|SCC_WR4_1_CLOCK);
    WRITE_REG(cp, SCC_WR0_SEL_WR2, tyCoDv[channel].vector); /* interrupt vec */
    WRITE_REG(cp, SCC_WR0_SEL_WR3, SCC_WR3_RX_8_BITS);	/* RX params */
    WRITE_REG(cp, SCC_WR0_SEL_WR5, SCC_WR5_TX_8_BITS);	/* TX params */
    WRITE_REG(cp, SCC_WR0_SEL_WR9, SCC_WR9_NO_RST);	/* master int */
    WRITE_REG(cp, SCC_WR0_SEL_WR1, 0);			/* TX/RX int */
    WRITE_REG(cp, SCC_WR0_SEL_WR10, 0);		/* misc TX/RX */

    /* setup baud rate generator */
    
    baudConstant = ((BAUD_CLK_FREQ / 2) / DEFAULT_BAUD) - 2;
    WRITE_REG(cp, SCC_WR0_SEL_WR11, SCC_WR11_RX_BR_GEN | SCC_WR11_TX_BR_GEN |
	       SCC_WR11_TRXC_OI | SCC_WR11_OUT_BR_GEN);     /* clock */
    WRITE_REG(cp, SCC_WR0_SEL_WR12, (char)baudConstant);      /* baud lsb */
    WRITE_REG(cp, SCC_WR0_SEL_WR13, (char)(baudConstant>>8)); /* baud msb */
    WRITE_REG(cp, SCC_WR0_SEL_WR14, SCC_WR14_BR_SRC | SCC_WR14_RST_CLK); /* misc */
    WRITE_REG(cp, SCC_WR0_SEL_WR14, SCC_WR14_BR_SRC | SCC_WR14_RST_CLK |
				     SCC_WR14_BR_EN);

    /*
     * enable extended read feature and CD interrupt */
     */

    WRITE_REG(cp, SCC_WR0_SEL_WR15,SCC_WR15_CD_IE | SCC_WR15_WR7_PRIME);
    WRITE_REG(cp, SCC_WR0_SEL_WR7,SCC_WR7_PRIME_EXT_READ);

    /*
     * enable receivers on both channels, transmitters not enabled until
     * data is available to transmit
     */

    WRITE_REG_0(cp, SCC_WR0_RST_TX_INT);
    WRITE_REG(cp,SCC_WR0_SEL_WR3,SCC_WR3_RX_8_BITS | SCC_WR3_RX_EN); /* enable RX */
    WRITE_REG(cp,SCC_WR0_SEL_WR1,SCC_WR1_INT_ALL_RX | SCC_WR1_TX_INT_EN |
				 SCC_WR1_EXT_INT_EN);            /* enable RX */

    /* reset external/status interrupts */

    WRITE_REG_0(cp, SCC_WR0_RST_EXT_INT);
    WRITE_REG_0(cp, SCC_WR0_RST_HI_IUS);
    WRITE_REG(cp, SCC_WR0_SEL_WR9,SCC_WR9_MIE | SCC_WR9_VIS); /* enable interrupts */

    /* turn-off CD, CTS, and TX */

    CTS_Off(channel);
    CD_Off(channel);
    TX_Off(channel);
    receiveEnabled[channel] = 0;

    }

/* routines provided to I/O system */

/*******************************************************************************
*
* tyCoOpen - open file to USART
*
* ARGSUSED1
*/

LOCAL int tyCoOpen (pTyCoDv, name, mode)
    TY_CO_DEV *pTyCoDv;
    char *name;
    int mode;

    {
    return ((int) pTyCoDv);
    }

/*******************************************************************************
*
* tyCoRead - serial interface read routine
*
* This driver is called by vxWorks whenever a read call is made on one
* of the serial ports.  
*
* ARGSUSED3
*/

LOCAL int tyCoRead (devPtr, buffer, maxbytes)
    TY_CO_DEV *devPtr;
    char *buffer;
    int maxbytes;

    {

    if (devPtr->channel == CONSOLE_TTY)
	return (tyRead ((TY_DEV_ID) devPtr, buffer, maxbytes));

    else
	return (rndBufGet (devPtr->rdBuf,buffer,maxbytes));

    }

/*******************************************************************************
*
* tyCoWrite - serial interface write routine
*
* This driver is called by vxWorks whenever a write call is made on one
* of the serial ports.
*
* ARGSUSED3
*/

LOCAL int tyCoWrite (devPtr, buffer, nbytes)
    TY_CO_DEV *devPtr;
    char *buffer;
    int nbytes;
    {
    char msgId;
    int len;
    int bytes;
    int key;

    if (devPtr->channel == CONSOLE_TTY)
	return (tyWrite ((TY_DEV_ID) devPtr, buffer, nbytes));

    else
	{
	if (rngIsEmpty(devPtr->wrBuf))
	    {
	    bytes = rngBufPut(devPtr->wrBuf, &buffer[1], nbytes-1);
	    key = intLock();
	    CD_On(devPtr->channel); /* turn-on signal acquired */
	    WRITE_DATA(devPtr->dp,buffer[0]);
	    TX_On(devPtr->channel);
	    intUnlock(key);
	    }
	else
	    {
	    bytes = rngBufPut(devPtr->wrBuf,buffer,nbytes);
	    }
	return bytes;
	}

    }

/*******************************************************************************
*
* tyCoIoctl - special device control
*
* This routine handles baud rate requests, and passes all others to tyIoctl(2).
*
* RETURNS: OK or ERROR if invalid baud rate, or whatever tyIoctl returns.
*/

LOCAL STATUS tyCoIoctl (pTyCoDv, request, arg)
    TY_CO_DEV *pTyCoDv;	/* device to control */
    int request;	/* request code */
    int arg;		/* some argument */

    {
    STATUS status;
    int oldLevel;
    int baudConstant;
    char *cp;

#define	MAX_BAUD	38400

    switch (request)
	{
	case FIOBAUDRATE:

	    if (arg < 50 || arg > MAX_BAUD)
		{
		status = ERROR; /* baud rate out-of-range */
		break;
		}

	    /* 
	     * calculate the baud rate constant for the new baud rate
	     * from the input clock frequency. x32 mode for asynchronous
	     * channels, x1 mode for synchronous (user data) channels
	     */

	    if (pTyCoDv->channel == CONSOLE_TTY)
		baudConstant = ((BAUD_CLK_FREQ / 64) / arg) - 2;
	    else
		baudConstant = ((BAUD_CLK_FREQ / 2) / arg) - 2;

	    cp = pTyCoDv->cp;

	    oldLevel = intLock();

            WRITE_REG(cp, SCC_WR0_SEL_WR14,SCC_WR14_BR_SRC | SCC_WR14_RST_CLK); /* turn-off baud clk */
            WRITE_REG(cp, SCC_WR0_SEL_WR12,(char)baudConstant); /* baud rate lsb */
            WRITE_REG(cp, SCC_WR0_SEL_WR13,(char)(baudConstant>>8)); /* baud rate msb */
            WRITE_REG(cp, SCC_WR0_SEL_WR14,SCC_WR14_BR_SRC | SCC_WR14_RST_CLK |
					   SCC_WR14_BR_EN); /* enable bud clk */
	    
	    intUnlock (oldLevel);

	    status = OK;

	    break;

	default:
	    status = tyIoctl (&pTyCoDv->tyDev, request, arg);
	    break;
	}

    return (status);
    }

/*******************************************************************************
*
* tyRxInt - handle a receiver interrupt
*
*/

LOCAL VOID tyRxInt (channel)
    FAST int channel;

    {
    char ch;
    FAST int temp;

    READ_DATA(tyCoDv[channel].dp,ch);

    if (channel != CONSOLE_TTY)
	{
	if (RTS_On(channel) && receiveEnabled[channel])
	    {
	    RNG_ELEM_PUT(tyCoDv[channel].rdBuf,ch,temp);
	    }
	else
	    CTS_Off(channel);

	return;
	}

    tyIRd (&tyCoDv[channel].tyDev, ch);
    WRITE_REG_0(tyCoDv[channel].cp, SCC_WR0_RST_HI_IUS);

    }
/*******************************************************************************
*
* tyTxInt - handle a transmitter interrupt
*
* If there is another character to be transmitted, it sends it.  If
* not, or if a device has never been created for this channel, we just
* disable the interrupt.
*/

LOCAL VOID tyTxInt (channel)
    FAST int channel;

    {
    char outChar;
    FAST int temp;

    if (!tyCoDv[channel].created)
	return;

    if (channel != CONSOLE_TTY)
	{
	if (RNG_ELEM_GET(tyCoDv[channel.wrBuf,&outChar,temp))
	    {
            WRITE_DATA(tyCoDv[channel].dp,outChar)
	    }
	else
	    {
	    WRITE_REG_0(tyCoDv[channel].cp,SCC_WR0_RST_TX_INT);
	    CD_Off(channel);
	    TX_Off(channel);
	    }
	}
    else
	{
	if (tyITx (&tyCoDv[channel.tyDev, &outChar) == OK)
	    {
	    WRITE_DATA(tyCoDv[channel].dp, outChar);
	    }
	else
	    {
	    WRITE_REG_0(tyCoDv[channel].cp,SCC_WR0_RST_TX_INT);
	    }
	}

    WRITE_REG_0(tyCoDv[channel].cp,SCC_WR0_RST_HI_IUS);

    }
/*******************************************************************************
*
* tyExInt - handle an error interrupt
*
*/

LOCAL VOID tyExInt (channel)
    FAST int channel;
    
    {
    /* 
     * if RTS on, enable CTS, else disable CTS
     */

    if (RTS_On(channel))
	CTS_On(channel);
    else
	CTS_Off(channel);

    /*
     * reset external/status interrupt (spec says to do it twice)
     */

    WRITE_REG_0(tyCoDv[channel].cp,SCC_WR0_RST_EXT_INT);
    WRITE_REG_0(tyCoDv[channel].cp,SCC_WR0_RST_HI_IUS);

    }

/*******************************************************************************
*
* enableReceive - enable the receiver
*
*/

LOCAL VOID enableReceive (channel)
    FAST int channel;
    
    {
    if (channel != CONSOLE_TTY)
	receiveEnabled[channel] = 1;
    else
	printf ("enableReceive: Invalid channel number %d\n",channel);
    }

/*******************************************************************************
*
* disableReceive - disable the receiver
*
*/

LOCAL VOID disableReceive (channel)
    FAST int channel;
    
    {
    if (channel != CONSOLE_TTY)
	receiveEnabled[channel] = 0;
    else
	printf ("disableReceive: Invalid channel number %d\n",channel);
    }

/*******************************************************************************
*
* tyCoStartup - transmitter startup routine
*
* Call interrupt level character output routine.
*/

LOCAL VOID tyCoStartup (pTyCoDv)
    TY_CO_DEV *pTyCoDv;		/* ty device to start up */

    {
    char outChar;
    unsigned char byte;
    int key;

    if (!pTyCoDv->created)
	return;

    if (tyITx (&pTyCoDv->tyDev, &outChar) == OK)
	{
        WRITE_DATA(pTyCoDv->dp,outChar);
	if (pTyCoDv->channel != CONSOLE_TTY)
	    TX_On(pTyCoDv->channel); /* enable TX */
	}
    else
	{
        WRITE_REG_0(pTyCoDv->cp,SCC_WR0_RST_TX_INT); /* send EOI */
	}
    }

/*******************************************************************************
*	interrupt routines for SCC 1
*******************************************************************************/

LOCAL VOID scc1aTxInt()
    {
    tyTxInt(0);
    }

LOCAL VOID scc1aRxInt()
    {
    tyRxInt(0);
    }

LOCAL VOID scc1aExInt()
    {
    tyExInt(0);
    }

LOCAL VOID scc1bTxInt()
    {
    tyTxInt(1);
    }

LOCAL VOID scc1bRxInt()
    {
    tyRxInt(1);
    }

LOCAL VOID scc1bExInt()
    {
    tyExInt(1);
    }

/*******************************************************************************
*	interrupt routines for SCC 2
*******************************************************************************/

LOCAL VOID scc2aTxInt()
    {
    tyTxInt(2);
    }

LOCAL VOID scc2aRxInt()
    {
    tyRxInt(2);
    }

LOCAL VOID scc2aExInt()
    {
    tyExInt(2);
    }

LOCAL VOID scc2bTxInt()
    {
    tyTxInt(3);
    }

LOCAL VOID scc2bRxInt()
    {
    tyRxInt(3);
    }

LOCAL VOID scc2bExInt()
    {
    tyExInt(3);
    }

/*******************************************************************************
*	interrupt routines for SCC 3
*******************************************************************************/

LOCAL VOID scc3aTxInt()
    {
    tyTxInt(4);
    }

LOCAL VOID scc3aRxInt()
    {
    tyRxInt(4);
    }

LOCAL VOID scc3aExInt()
    {
    tyExInt(4);
    }

LOCAL VOID scc3bTxInt()
    {
    tyTxInt(5);
    }

LOCAL VOID scc3bRxInt()
    {
    tyRxInt(5);
    }

LOCAL VOID scc3bExInt()
    {
    tyExInt(5);
    }

/*******************************************************************************
*	interrupt routines for SCC 4
*******************************************************************************/

LOCAL VOID scc4aTxInt()
    {
    tyTxInt(6);
    }

LOCAL VOID scc4aRxInt()
    {
    tyRxInt(6);
    }

LOCAL VOID scc4aExInt()
    {
    tyExInt(6);
    }

LOCAL VOID scc4bTxInt()
    {
    tyTxInt(7);
    }

LOCAL VOID scc4bRxInt()
    {
    tyRxInt(7);
    }

LOCAL VOID scc4bExInt()
    {
    tyExInt(7);
    }

/*******************************************************************************
*	interrupt routines for SCC 5
*******************************************************************************/

LOCAL VOID scc5aTxInt()
    {
    tyTxInt(8);
    }

LOCAL VOID scc5aRxInt()
    {
    tyRxInt(8);
    }

LOCAL VOID scc5aExInt()
    {
    tyExInt(8);
    }

LOCAL VOID scc5bTxInt()
    {
    tyTxInt(9);
    }

LOCAL VOID scc5bRxInt()
    {
    tyRxInt(9);
    }

LOCAL VOID scc5bExInt()
    {
    tyExInt(9);
    }

/*******************************************************************************
*	interrupt routines for SCC 6
*******************************************************************************/

LOCAL VOID scc6aTxInt()
    {
    tyTxInt(10);
    }

LOCAL VOID scc6aRxInt()
    {
    tyRxInt(10);
    }

LOCAL VOID scc6aExInt()
    {
    tyExInt(10);
    }

LOCAL VOID scc6bTxInt()
    {
    tyTxInt(11);
    }

LOCAL VOID scc6bRxInt()
    {
    tyRxInt(11);
    }

LOCAL VOID scc6bExInt()
    {
    tyExInt(11);
    }

/*******************************************************************************
*	interrupt routines for SCC 7
*******************************************************************************/

LOCAL VOID scc7aTxInt()
    {
    tyTxInt(12);
    }

LOCAL VOID scc7aRxInt()
    {
    tyRxInt(12);
    }

LOCAL VOID scc7aExInt()
    {
    tyExInt(12);
    }

LOCAL VOID scc7bTxInt()
    {
    tyTxInt(13);
    }

LOCAL VOID scc7bRxInt()
    {
    tyRxInt(13);
    }

LOCAL VOID scc7bExInt()
    {
    tyExInt(13);
    }

/******************************************************************************
* macros to read/modify/write WR5
******************************************************************************/

#define WR5_ON(cp,bit) { \
		       READ_REG(cp,SCC_WR0_SEL_WR5,byte); \
		       byte = byte | (unsigned char)bit; \
		       WRITE_REG(cp,SCC_WR0_SEL_WR5,byte); \
		       }

#define WR5_OFF(cp,bit) { \
		       READ_REG(cp,SCC_WR0_SEL_WR5,byte); \
		       byte = byte & ~(unsigned char)bit; \
		       WRITE_REG(cp,SCC_WR0_SEL_WR5,byte); \
		       }

/* turn carrier detect on */
void CD_On (channel)
    FAST int channel;
    
    {
    unsigned char byte;
    int key;

    key = intLock();
    WR5_OFF(tyCoDv[channel].cp,SCC_WR5_DTR);
    intUnlock(key);
    }

/* turn carrier detect off */
void CD_Off (channel)
    FAST int channel;
    
    {
    unsigned char byte;
    int key;

    key = intLock();
    WR5_ON(tyCoDv[channel].cp,SCC_WR5_DTR);
    intUnlock(key);
    }

/* turn CTS on */
void CTS_On (channel)
    FAST int channel;
    
    {
    unsigned char byte;
    int key;

    key = intLock();
    WR5_ON(tyCoDv[channel].cp,SCC_WR5_CTS);
    intUnlock(key);
    }

/* turn CTS off */
void CTS_Off (channel)
    FAST int channel;
    
    {
    int key;
    unsigned char byte;

    key = intLock();
    WR5_OFF(tyCoDv[channel].cp,SCC_WR5_CTS);
    intUnlock(key);
    }

/* turn TX on */
void TX_On (channel)
    FAST int channel;
    
    {
    unsigned char byte;
    int key;

    key = intLock();
    WR5_ON(tyCoDv[channel].cp,SCC_WR5_TX_EN);
    intUnlock(key);
    }

/* turn TX off */
void TX_Off (channel)
    FAST int channel;
    
    {
    int key;
    unsigned char byte;

    key = intLock();
    WR5_OFF(tyCoDv[channel].cp,SCC_WR5_TX_EN);
    intUnlock(key);
    }

/* check if RTS on */
/* this routine should either return unsigned char or define result as int */

int RTS_On (channel)
    FAST int channel;
    
    {
    unsigned char result;
    int key;

    key = intLock();
    READ_REG(tyCoDv[channel].cp,SCC_WR0_SEL_WR0,result); \
    result = (result & SCC_RR0_CD) |= SCC_RR0_CD;
    intUnlock(key);
    return result;
    }
