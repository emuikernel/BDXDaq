/* z8530Sio.c - Z8530 SCC Serial Communications Controller driver */

/* Copyright 1995-1996 Wind River Systems, Inc. */

/*
modification history
--------------------
01j,09jun97,mas  now uses BSP-defined macros for register access and reset
		 delay loop/count (SPR 8566); updated documentation.
01i,05jun97,db   fixed warning messages.
01h,23may97,db   added hardware options and modem control flow(SPR #1037).
01g,27dec96,dat  fixed spr 7683, int ack in wrong place (z8530IntRd).
01f,11jun96,dbt  fixed spr 6325. replaced everywhere *cr = 0 with *cr = zero.
		 Update Copyright.
01g,13oct95,ms   initialize all the non-BSP specific fields in z8530DevInit()
01f,21sep95,myz  fixed the interrupt race problem
01e,03aug95,myz  fixed the warning messages
01d,20jun95,ms   fixed comments for mangen
01c,15jun95,ms	 updated for new driver structure
01b,28apr95,ms   z8530IntRd now only reads the data register once.
01a,21dec94,ms   written (using z8530Serial.c + the VxMon polled driver).
*/

/*
DESCRIPTION
This is the driver for the Z8530 SCC (Serial Communications Controller).
It uses the SCCs in asynchronous mode only.

USAGE
A Z8530_DUSART structure is used to describe the chip. This data structure
contains two Z8530_CHAN structures which describe the chip's two serial
channels.  Supported baud rates range from 50 to 38400.  The default baud
rate is Z8530_DEFAULT_BAUD (9600).  The BSP may redefine this.

The BSP's sysHwInit() routine typically calls sysSerialHwInit()
which initializes all the values in the Z8530_DUSART structure (except
the SIO_DRV_FUNCS) before calling z8530DevInit().

The BSP's sysHwInit2() routine typically calls sysSerialHwInit2() which
connects the chips interrupts via intConnect() (either the single
interrupt z8530Int or the three interrupts z8530IntWr, z8530IntRd,
and z8530IntEx).

This driver handles setting of hardware options such as parity(odd, even) and
number of data bits(5, 6, 7, 8).  Hardware flow control is provided with the
signals CTS on transmit and DSR on read.  Refer to the target documentation
for the RS232 port configuration.  The function HUPCL(hang up on last close) is
supported.  Default hardware options are defined by Z8530_DEFAULT_OPTIONS.
The BSP may redefine them.

All device registers are accessed via BSP-defined macros so that memory-
mapped as well as I/O space accesses can be supported.  The BSP may re-
define the REG_8530_READ and REG_8530_WRITE macros as needed.  By default,
they are defined as simple memory-mapped accesses.

The BSP may define DATA_REG_8530_DIRECT to cause direct access to the Z8530
data register, where hardware permits it.  By default, it is not defined.

The BSP may redefine the macro for the channel reset delay Z8530_RESET_DELAY
as well as the channel reset delay counter value Z8530_RESET_DELAY_COUNT as
required.  The delay is defined as the minimum time between successive chip
accesses (6 PCLKs + 200 nSec for a Z8530, 4 PCLKs for a Z85C30 or Z85230)
plus an additional 4 PCLKs.  At a typical PCLK frequency of 10 MHz, each PCLK
is 100 nSec, giving a minimum reset delay of:

Z8530:  10 PCLKs + 200 nSec = 1200 nSec = 1.2 uSec

Z85x30:  8 PCLKs            =  800 nSec = 0.8 uSec

INCLUDE FILES: drv/sio/z8530Sio.h
*/

#include "vxWorks.h"
#include "sioLib.h"
#include "intLib.h"
#include "errno.h"
#include "drv/sio/z8530Sio.h"

#ifndef SIO_HUP
#   define SIO_OPEN	0x100A
#   define SIO_HUP	0x100B
#endif

/* forward static declarations */

static void z8530InitChannel (Z8530_CHAN *);
static int z8530Ioctl (SIO_CHAN *, int, void *);
static int z8530TxStartup (SIO_CHAN *);
static int z8530CallbackInstall (SIO_CHAN *, int, STATUS (*)(), void *);
static int z8530PollInput (SIO_CHAN *, char *);
static int z8530PollOutput (SIO_CHAN *, char);
LOCAL   STATUS  z8530OptsSet (Z8530_CHAN *, UINT);
LOCAL   STATUS  z8530Open (Z8530_CHAN * pChan );
LOCAL   STATUS  z8530Hup (Z8530_CHAN * pChan );

/* driver functions */

static SIO_DRV_FUNCS z8530SioDrvFuncs = 
    {
    (int (*)())z8530Ioctl,
    (int (*)())z8530TxStartup,
    z8530CallbackInstall,
    (int (*)())z8530PollInput,
    (int (*)(SIO_CHAN *,char))z8530PollOutput
    };

/* defines */

#ifndef Z8530_DEFAULT_BAUD
#       define  Z8530_DEFAULT_BAUD      9600
#endif

#ifndef Z8530_DEFAULT_OPTIONS
    /* no handshake, rcvr enabled, 8 data bits, 1 stop bit, no parity */
#   define Z8530_DEFAULT_OPTIONS (CLOCAL | CREAD | CS8)
#endif

#ifndef REG_8530_READ
#  define	REG_8530_READ(reg,pVal)	*(pVal) = (*(reg))
#endif  /* REG_8530_READ */

#ifndef REG_8530_WRITE
#  define	REG_8530_WRITE(reg,val)	(*(reg) = (val))
#endif  /* REG_8530_WRITE */

#ifndef Z8530_RESET_DELAY_COUNT
#  define	Z8530_RESET_DELAY_COUNT	1000
#endif  /* Z8530_REST_DELAY_COUNT */

#ifndef Z8530_RESET_DELAY
#  define Z8530_RESET_DELAY \
        { \
        int i; \
        for (i = 0; i < Z8530_RESET_DELAY_COUNT; i++) \
                ; /* do nothing */ \
        }
#endif  /* Z8530_REST_DELAY */


/******************************************************************************
*
* z8530DummyCallback - dummy callback routine.
*
* This routine is used as an intialization default only.
*
* RETURNS: ERROR always
*/ 

static STATUS z8530DummyCallback (void)
    {
    return (ERROR);
    }

/******************************************************************************
*
* z8530TxStartup - start the interrupt transmitter.
*
* This routine starts the transmitter if it isn't already transmitting.
* This routine should only be called in interrupt mode.
*
* RETURNS: OK always
*/

static int z8530TxStartup
    (
    SIO_CHAN * pSioChan
    )
    {
    Z8530_CHAN * pChan = (Z8530_CHAN *)pSioChan;
    char         outChar;
    int          lvl;

    if ((*pChan->getTxChar) (pChan->getTxArg, &outChar) != ERROR)
	{
	lvl = intLock ();
#ifndef DATA_REG_8530_DIRECT
	REG_8530_WRITE(pChan->cr, SCC_WR0_SEL_WR8); /* select data register */
	REG_8530_WRITE(pChan->cr, outChar);
#else
	REG_8530_WRITE(pChan->dr, outChar);
#endif  /* DATA_REG_8530_DIRECT */
	intUnlock (lvl);
	}

    return (OK);
    }

/******************************************************************************
*
* z8530CallbackInstall - install ISR callbacks to get/put chars.
*
* This routine installs the specified type of callback routine and its
* associated argument for use in read/write character buffer management
* as defined in Stream I/O (SIO).
*
* RETURNS: OK or ENOSYS if unknown callback type.
*/ 

static int z8530CallbackInstall
    (
    SIO_CHAN *	pSioChan,
    int		callbackType,
    STATUS	(*callback)(),
    void *      callbackArg
    )
    {
    Z8530_CHAN * pChan = (Z8530_CHAN *)pSioChan;

    switch (callbackType)
	{
	case SIO_CALLBACK_GET_TX_CHAR:
	    pChan->getTxChar	= callback;
	    pChan->getTxArg	= callbackArg;
	    return (OK);
	case SIO_CALLBACK_PUT_RCV_CHAR:
	    pChan->putRcvChar	= callback;
	    pChan->putRcvArg	= callbackArg;
	    return (OK);
	default:
	    return (ENOSYS);
	}

    }

/******************************************************************************
*
* z8530PollOutput - output a character in polled mode.
*
* This routine is called to transmit a single character when the driver is
* set to polled mode (no interrupts).
*
* RETURNS: OK if a character arrived, EIO on device error, EAGAIN
*	   if the output buffer if full.
*/

static int z8530PollOutput
    (
    SIO_CHAN *	pSioChan,
    char	outChar
    )
    {
    Z8530_CHAN *  pChan = (Z8530_CHAN *)pSioChan;
    volatile char ch;

    /* is the transitter ready to accept a character? */

    REG_8530_READ(pChan->cr, &ch);
    if ((ch & SCC_RR0_TX_EMPTY) == 0x00)
	return (EAGAIN);

    /* write out the character */

#ifndef DATA_REG_8530_DIRECT
    REG_8530_WRITE(pChan->cr, SCC_WR0_SEL_WR8); /* select data register */
    REG_8530_WRITE(pChan->cr, outChar);
#else
    REG_8530_WRITE(pChan->dr, outChar);
#endif  /* DATA_REG_8530_DIRECT */

    return (OK);
    }

/******************************************************************************
*
* z8530PollInput - poll the device for input.
*
* This routine is called to receive a single character when the driver is
* set to polled mode (no interrupts).
*
* RETURNS: OK if a character arrived, EIO on device error, EAGAIN
*	   if the input buffer if empty.
*/

static int z8530PollInput
    (
    SIO_CHAN *	pSioChan,
    char *	thisChar
    )
    {
    Z8530_CHAN *  pChan = (Z8530_CHAN *)pSioChan;
    volatile char ch;

    /* is a character available? */

    REG_8530_READ(pChan->cr, &ch);
    if ((ch & SCC_RR0_RX_AVAIL) == 0x00)
	return (EAGAIN);

    /* yes, get a character */

#ifndef DATA_REG_8530_DIRECT
    REG_8530_WRITE(pChan->cr, SCC_WR0_SEL_WR8); /* select data register */
    REG_8530_READ(pChan->cr, thisChar);
#else
    REG_8530_READ(pChan->dr, thisChar);
#endif  /* DATA_REG_8530_DIRECT */

    /* check for errors */

    REG_8530_WRITE(pChan->cr, SCC_WR0_SEL_WR1);	/* read reg 1 */
    REG_8530_READ(pChan->cr, &ch);
    if ((ch & 0x70) != 0)				/* reset if errors */
        REG_8530_WRITE(pChan->cr, SCC_WR0_ERR_RST);

    return (OK);
    }

/******************************************************************************
*
* z8530ModeSet - set driver mode
*
* This routine sets the driver SIO mode.  Currently there are only two:
* polled and interrupt.
*
* RETURNS: OK    if successful
*          ERROR if illegal mode specified
*/

static STATUS z8530ModeSet
    (
    Z8530_CHAN * pChan,
    uint_t	 newMode
    )
    {
    int             oldlevel;
    int             zero = 0;
    volatile char * cr = pChan->cr;    /* SCC control reg adr */

    if ((newMode != SIO_MODE_POLL) && (newMode != SIO_MODE_INT))
	return (ERROR);

    oldlevel = intLock ();

    pChan->mode = newMode;

    if (pChan->mode == SIO_MODE_INT)
        {
        REG_8530_WRITE(cr, SCC_WR0_SEL_WR1);	/* int and xfer mode */
        REG_8530_WRITE(cr, SCC_WR1_INT_ALL_RX | SCC_WR1_TX_INT_EN);
        }
    else
        {
        REG_8530_WRITE(cr, SCC_WR0_SEL_WR1);	/* no interrupts */
        REG_8530_WRITE(cr, zero);
        }

    intUnlock(oldlevel);
   
    return (OK);
    }

/*******************************************************************************
*
* z8530InitChannel - initialize a single channel
*
* This routine initializes the specified Z8530 channel to a known and
* quiescent state at a default baud rate, no handshake signals.  It is
* called by z8530DevInit().
*
* RETURNS: N/A
*/

static void z8530InitChannel
    (
    Z8530_CHAN * pChan
    )
    {
    int             baudConstant;
    int             zero = 0;
    volatile char * cr = pChan->cr;    /* SCC control reg adr */

    /* SCC_WR0_NULL_CODE sync the state machine */

    REG_8530_WRITE(cr, zero);
    REG_8530_WRITE(cr, zero);

    /* reset any error or interrupts */

    REG_8530_WRITE(cr, SCC_WR0_ERR_RST);
    REG_8530_WRITE(cr, SCC_WR0_RST_INT);

    /* write reg 9 - master interrupt control and reset */

    REG_8530_WRITE(cr, SCC_WR0_SEL_WR9);

    /* reset specified channel */

    if (pChan->channel == SCC_CHANNEL_A)
        REG_8530_WRITE(cr, SCC_WR9_CH_A_RST);      /* reset channel A */
    else
        REG_8530_WRITE(cr, SCC_WR9_CH_B_RST);      /* reset channel B */

    /* Delay for about 1 uSec (typically) before further chip accesses */

    Z8530_RESET_DELAY


    /* initialize registers */

    /* write reg 4 - misc parms & modes */

    REG_8530_WRITE(cr, SCC_WR0_SEL_WR4);
    REG_8530_WRITE(cr, SCC_WR4_1_STOP | SCC_WR4_16_CLOCK);

    /* write reg 3 - receive params */

    REG_8530_WRITE(cr, SCC_WR0_SEL_WR3);
    REG_8530_WRITE(cr, (unsigned char)SCC_WR3_RX_8_BITS);

    /* write reg 5 - tx params */

    REG_8530_WRITE(cr, SCC_WR0_SEL_WR5);
    REG_8530_WRITE(cr, (UINT8)(SCC_WR5_TX_8_BITS | SCC_WR5_DTR | SCC_WR5_RTS));

    /* write reg 10 - misc tx/rx control */

    REG_8530_WRITE(cr, SCC_WR0_SEL_WR10);
    REG_8530_WRITE(cr, zero);             /* clear sync, loop, poll */

    /* write reg 11 - clock mode */

    REG_8530_WRITE(cr, SCC_WR0_SEL_WR11);
    REG_8530_WRITE(cr, pChan->writeReg11);

    /* write reg 15 - disable external/status interrupts */

    REG_8530_WRITE(cr, SCC_WR0_SEL_WR15);
    REG_8530_WRITE(cr, zero);

    /*
     * Calculate the baud rate constant for the default baud rate
     * from the input clock (PCLK) frequency.  This assumes that
     * the divide-by-16 bit is set (done in WR4 above).
     */

    baudConstant = ((pChan->baudFreq / 32) / pChan->baudRate) - 2;

    /* write reg 12,13 - set baud rate */

    REG_8530_WRITE(cr, SCC_WR0_SEL_WR12);     /* LSB of baud constant */
    REG_8530_WRITE(cr, (char)baudConstant);
    REG_8530_WRITE(cr, SCC_WR0_SEL_WR13);     /* MSB of baud constant */
    REG_8530_WRITE(cr, (char)(baudConstant >> 8));

    /* write reg 14 - misc control bits */

    REG_8530_WRITE(cr, SCC_WR0_SEL_WR14);
    REG_8530_WRITE(cr, pChan->writeReg14);

    /* reset external interrupts and errors */

    REG_8530_WRITE(cr, SCC_WR0_RST_INT);
    REG_8530_WRITE(cr, SCC_WR0_ERR_RST);

    /* write reg 3 - receive params */

    REG_8530_WRITE(cr, SCC_WR0_SEL_WR3);
    REG_8530_WRITE(cr, (unsigned char)(SCC_WR3_RX_8_BITS | SCC_WR3_RX_EN));

    /* write reg 5 - tx params */

    REG_8530_WRITE(cr, SCC_WR0_SEL_WR5);
    REG_8530_WRITE(cr, (unsigned char)(SCC_WR5_TX_8_BITS | SCC_WR5_TX_EN |
				       SCC_WR5_DTR       | SCC_WR5_RTS    ));

    /* write reg 2 - interrupt vector */

    REG_8530_WRITE(cr, SCC_WR0_SEL_WR2);
    REG_8530_WRITE(cr, pChan->intVec);

    /* write reg 1 - disable interrupts and xfer mode */

    REG_8530_WRITE(cr, SCC_WR0_SEL_WR1);
    REG_8530_WRITE(cr, zero);

    /* write reg 9 - enable master interrupt control */

    REG_8530_WRITE(cr, SCC_WR0_SEL_WR9);
    REG_8530_WRITE(cr, SCC_WR9_MIE | pChan->intType);

    /* reset Tx CRC generator and any pending ext/status interrupts */

    REG_8530_WRITE(cr, (UINT8)(SCC_WR0_RST_TX_CRC));
    REG_8530_WRITE(cr, SCC_WR0_RST_INT);
    REG_8530_WRITE(cr, SCC_WR0_RST_INT);

    /* reset SCC register counter */

    REG_8530_WRITE(cr, zero);
    }

/*******************************************************************************
*
* z8530Hup - hang up the modem control lines 
*
* Resets the RTS and DTR signals.
*
* RETURNS: OK
*/

LOCAL STATUS z8530Hup
    (
    Z8530_CHAN * pChan          /* pointer to channel */
    )
    {
    volatile    char *cr = pChan->cr;    /* SCC control reg adr */
    FAST        int     oldlevel;       /* current interrupt level mask */

    pChan->writeReg5 &= (~(SCC_WR5_DTR | SCC_WR5_RTS));

    oldlevel = intLock ();

    REG_8530_WRITE(cr, SCC_WR0_SEL_WR5);     
    REG_8530_WRITE(cr, pChan->writeReg5);

    intUnlock (oldlevel);

    return (OK);

    }    

/*******************************************************************************
*
* z8530Open - Set the modem control lines 
*
* Set the modem control lines(RTS, DTR) TRUE if not already set.  
* It also clears the receiver, transmitter and enables the fifo. 
*
* RETURNS: OK
*/

LOCAL STATUS z8530Open
    (
    Z8530_CHAN * pChan          /* pointer to channel */
    )
    {
    volatile    char *cr = pChan->cr;   /* SCC control reg adr */
    FAST int     oldlevel;              /* current interrupt level mask */
    char mask;

    mask = pChan->writeReg5 & (SCC_WR5_DTR | SCC_WR5_RTS);

    if (mask != (char)(SCC_WR5_DTR | SCC_WR5_RTS)) 
        {
        /* RTS and DTR not set yet */

        pChan->writeReg5 |= (SCC_WR5_DTR | SCC_WR5_RTS);

        oldlevel = intLock ();

        /* set RTS and DTR TRUE */

        REG_8530_WRITE(cr, SCC_WR0_SEL_WR5);     
        REG_8530_WRITE(cr, pChan->writeReg5);

        intUnlock (oldlevel);
        }

    return (OK);
    }

/*******************************************************************************
*
* z8530OptsSet - set the serial options
*
* Set the channel operating mode to that specified.  All sioLib options
* are supported: CLOCAL, HUPCL, CREAD, CSIZE, PARENB, and PARODD.
*
* RETURNS:
* Returns OK to indicate success, otherwise ERROR is returned
*/

LOCAL STATUS z8530OptsSet
    (
    Z8530_CHAN * pChan,                 /* pointer to channel */
    UINT options                        /* new hardware options */
    )
    {
    FAST int    oldlevel;               /* current interrupt level mask */
    volatile    char *cr = pChan->cr;   /* SCC control reg adr */
    UINT8       wreg3, wreg4, wreg5;

    wreg3 = wreg4 = wreg5 = 0;
    
    if (pChan == NULL || options & 0xffffff00)
        return ERROR;

    switch (options & CSIZE)
        {
        case CS5:
            wreg3 = SCC_WR3_RX_5_BITS; /* select receive 5 bit data size */
            wreg5 = SCC_WR5_TX_5_BITS; /* select transmit 5 bit data size */
            break;
        case CS6:
            wreg3 = SCC_WR3_RX_6_BITS; /* select receive 6 bit data size */
            wreg5 = SCC_WR5_TX_6_BITS; /* select transmit 6 bit data size */
            break;
        case CS7:
            wreg3 = SCC_WR3_RX_7_BITS; /* select receive 7 bit data size */
            wreg5 = SCC_WR5_TX_7_BITS; /* select transmit 7 bit data size */
            break;
        default:
        case CS8:
            wreg3 = SCC_WR3_RX_8_BITS; /* select receive 8 bit data size */ 
            wreg5 = SCC_WR5_TX_8_BITS; /* select transmit 8 bit data size */
            break;
        }

    if (options & STOPB)
        wreg4 = SCC_WR4_2_STOP;         /* select 2 stop bits */
    else
        wreg4 = SCC_WR4_1_STOP;         /* select one stop bit */
    
    switch (options & (PARENB | PARODD))
        {
        case PARENB|PARODD:
            wreg4 |= SCC_WR4_PAR_EN;
            break;
        case PARENB:
            wreg4 |= (SCC_WR4_PAR_EN | SCC_WR4_PAR_EVEN);
            break;
        default:
        case 0:
            wreg4 &= (~SCC_WR4_PAR_EN);
            break;
        }

    if (!(options & CLOCAL))
        {
        /* !clocal enables hardware flow control(DTR/DSR) */

        wreg3 |= SCC_WR3_AUTO_EN;
        wreg5 |= (unsigned char)(SCC_WR5_DTR | SCC_WR5_RTS);
        }

    if (options & CREAD)  
        wreg3 |= SCC_WR3_RX_EN;

    oldlevel = intLock ();

    REG_8530_WRITE(cr, SCC_WR0_SEL_WR1); /* no interrupts */
    REG_8530_WRITE(cr, 0);

    REG_8530_WRITE(cr, SCC_WR0_ERR_RST);
    REG_8530_WRITE(cr, SCC_WR0_RST_INT);

    REG_8530_WRITE(cr, SCC_WR0_SEL_WR9); /* write register 9 - mstr int ctrl */
    REG_8530_WRITE(cr, ((pChan->channel == SCC_CHANNEL_A) ?
           SCC_WR9_CH_A_RST :           /* reset channel A */
           SCC_WR9_CH_B_RST));          /* reset channel B */

    REG_8530_WRITE(cr, SCC_WR0_SEL_WR4); /* write reg 4 - misc parms & modes */
    REG_8530_WRITE(cr, (wreg4 | SCC_WR4_16_CLOCK));

    wreg5 |= SCC_WR5_TX_EN;

    REG_8530_WRITE(cr, SCC_WR0_SEL_WR5); /* tx params */
    REG_8530_WRITE(cr, wreg5);

    if (pChan->mode == SIO_MODE_INT)
        {
        REG_8530_WRITE(cr, SCC_WR0_SEL_WR1); /* int and xfer mode */
        REG_8530_WRITE(cr, (SCC_WR1_INT_ALL_RX | SCC_WR1_TX_INT_EN));

        /* re-enable interrupts */

        REG_8530_WRITE(cr, SCC_WR0_SEL_WR9); /* master interrupt control */
        REG_8530_WRITE(cr, (SCC_WR9_MIE | pChan->intType));
        }

    REG_8530_WRITE(cr, SCC_WR0_SEL_WR3); /* write reg 3 - receive params */
    REG_8530_WRITE(cr, (unsigned char)wreg3);

    intUnlock (oldlevel);

    pChan->options   = options;           /* save new options */
    pChan->writeReg5 = wreg5;

    return OK;
    }

/*******************************************************************************
*
* z8530Ioctl - special device control
*
* This is a general access routine to special device- and driver-specific
* functions and services.
*
* RETURNS: OK on success, EIO on device error, ENOSYS on unsupported request.
*/

static int z8530Ioctl
    (
    SIO_CHAN *	pSioChan,		/* device to control */
    int		request,		/* request code */
    void *	someArg			/* some argument */
    )
    {
    Z8530_CHAN *    pChan = (Z8530_CHAN *) pSioChan;
    int             oldlevel;		/* current interrupt level mask */
    int             baudConstant;
    volatile char * cr;			/* SCC control reg adr */
    int             arg = (int)someArg;

    switch (request)
	{
	case SIO_BAUD_SET:

	    if (arg < 50 || arg > 38400)  /* illegal baud rate */
	        {
		return (EIO);
	        }

            if (pChan->baudRate == arg)   /* current baud = requested baud */
                {
                return (OK);
                }

	    /*
	     * Calculate the baud rate constant for the new baud rate
	     * from the input clock (PCLK) frequency.  This assumes that the
	     * divide-by-16 bit is set in the Z8530 WR4 register (done in
	     * tyCoInitChannel).
	     */

	    baudConstant = ((pChan->baudFreq / 32) / arg) - 2;

	    cr = pChan->cr;

	    /* disable interrupts during chip access */

	    oldlevel = intLock ();

	    /* set the new baud rate */

	    REG_8530_WRITE(cr, SCC_WR0_SEL_WR12); /* LSB of baud constant */
	    REG_8530_WRITE(cr, (char)baudConstant);
	    REG_8530_WRITE(cr, SCC_WR0_SEL_WR13); /* MSB of baud constant */
	    REG_8530_WRITE(cr, (char)(baudConstant >> 8));

	    /* record new baud rate in channel control block */

	    pChan->baudRate = arg;

	    /* re-enable interrupts */

	    intUnlock (oldlevel);

	    return (OK);

	case SIO_BAUD_GET:
	    *(int *)arg = pChan->baudRate;  /* return current baud rate */
	    return (OK);

	case SIO_MODE_SET:
	    return (z8530ModeSet (pChan, arg) == OK ? OK : EIO);

	case SIO_MODE_GET:
	    *(int *)arg = pChan->mode;      /* return current SIO mode */
	    return (OK);

	case SIO_AVAIL_MODES_GET:
	    *(int *)arg = SIO_MODE_INT | SIO_MODE_POLL;
	    return (OK);

	case SIO_HW_OPTS_SET:
            return ((z8530OptsSet (pChan, arg) == OK) ? OK : EIO);
            break;

	case SIO_HW_OPTS_GET:
            *(int *)arg = pChan->options;
            break;

        case SIO_HUP:
            /* check if hupcl option is enabled */

            if (pChan->options & HUPCL) 
                return (z8530Hup (pChan));
            break;

        case SIO_OPEN:
            /* check if hupcl option is enabled */

            if (pChan->options & HUPCL) 
                return (z8530Open (pChan));
            break;

	default:
	    return (ENOSYS);
	}

	return (OK);
    }

/******************************************************************************
*
* z8530DevInit - intialize a Z8530_DUSART
*
* The BSP must have already initialized all the device addresses, etc in
* Z8530_DUSART structure.  This routine initializes some SIO_CHAN
* function pointers and then resets the chip to a quiescent state.
*
* RETURNS: N/A
*/

void z8530DevInit
    (
    Z8530_DUSART * pDusart
    )
    {
    /* disable interrupts during initialization */

    int oldlevel = intLock();

    /* initialize the driver function pointers in the SIO_CHAN's */

    pDusart->portA.pDrvFuncs	= &z8530SioDrvFuncs;
    pDusart->portB.pDrvFuncs	= &z8530SioDrvFuncs;

    /* set the non BSP-specific constants */

    pDusart->portA.baudRate	= Z8530_DEFAULT_BAUD;
    pDusart->portA.channel	= SCC_CHANNEL_A;
    pDusart->portA.getTxChar	= z8530DummyCallback;
    pDusart->portA.putRcvChar	= z8530DummyCallback;
    pDusart->portA.mode		= 0;	/* undefined */
    pDusart->portA.options	= Z8530_DEFAULT_OPTIONS;

    pDusart->portB.baudRate	= Z8530_DEFAULT_BAUD;
    pDusart->portB.channel	= SCC_CHANNEL_B;
    pDusart->portB.getTxChar	= z8530DummyCallback;
    pDusart->portB.putRcvChar	= z8530DummyCallback;
    pDusart->portB.mode		= 0;	/* undefined */
    pDusart->portB.options	= Z8530_DEFAULT_OPTIONS;

    /* reset the chip */

    z8530InitChannel(&pDusart->portA);
    z8530InitChannel(&pDusart->portB);

    intUnlock(oldlevel);
    }

/*******************************************************************************
*
* z8530IntWr - handle a transmitter interrupt
*
* This routine handles write interrupts from the SCC.
*
* RETURNS: N/A
*/

void z8530IntWr
    (
    Z8530_CHAN * pChan
    )
    {
    char            outChar;
    volatile char * cr = pChan->cr;
#ifdef DATA_REG_8530_DIRECT
    volatile char * dr = pChan->dr;
#endif  /* DATA_REG_8530_DIRECT */

    /* if a char is available, transmit it */

    if ((*pChan->getTxChar) (pChan->getTxArg, &outChar) != ERROR)
	{
#ifndef DATA_REG_8530_DIRECT
	REG_8530_WRITE(cr, SCC_WR0_SEL_WR8); /* select data register */
	REG_8530_WRITE(cr, outChar);
#else
	REG_8530_WRITE(dr, outChar);
#endif  /* DATA_REG_8530_DIRECT */
	}
	
    /* else, reset tx interrupts */

    else
	REG_8530_WRITE(cr, SCC_WR0_RST_TX_INT);

    /* reset the interrupt */

    REG_8530_WRITE(cr, SCC_WR0_RST_HI_IUS);
    }

/*****************************************************************************
*
* z8530IntRd - handle a reciever interrupt
*
* This routine handles read interrupts from the SCC.
*
* RETURNS: N/A
*/

void z8530IntRd
    (
    Z8530_CHAN * pChan
    )
    {
    volatile char * cr = pChan->cr;
    volatile char   inchar;
#ifdef DATA_REG_8530_DIRECT
    volatile char * dr = pChan->dr;
#endif  /* DATA_REG_8530_DIRECT */
    BOOL            rx = FALSE;

    /* reset the interrupt in the Z8530 */

    REG_8530_WRITE(cr, SCC_WR0_RST_HI_IUS);

    /*
     * Receiver is FIFO based so there may be more than one char to read.
     * Loop here until all chars are read.
     */
    do
        {
        /* see if character is valid, if not just read and discard it */

        REG_8530_WRITE(cr, SCC_WR0_SEL_WR1); /* read register 1 */
        REG_8530_READ(cr, &inchar);
        if ((inchar & (SCC_RR1_CRC_ERR   |  /* framing error */
		       SCC_RR1_RX_OV_ERR |  /* receive overrun */
		       SCC_RR1_PAR_ERR))    /* parity error */
	    == 0)
            rx = TRUE;
        else
            rx = FALSE;

        /* read the received character */

#ifndef DATA_REG_8530_DIRECT
        REG_8530_WRITE(cr, SCC_WR0_SEL_WR8); /* select data register */
        REG_8530_READ(cr, &inchar);
#else
        REG_8530_READ(dr, &inchar);
#endif  /* DATA_REG_8530_DIRECT */

        /* do callback if valid char */

        if (rx)
	    (*pChan->putRcvChar) (pChan->putRcvArg, inchar);

        /* See if more chars available */

        REG_8530_READ(cr, &inchar);
        } while (inchar & SCC_RR0_RX_AVAIL);
    }

/**********************************************************************
*
* z8530IntEx - handle error interrupts
*
* This routine handles miscellaneous interrupts on the SCC.
*
* RETURNS: N/A
*/

void z8530IntEx
    (
    Z8530_CHAN * pChan
    )
    {
    volatile char * cr = pChan->cr;
    char            charStatus;

    /* if Tx Underrun, reset it */

    REG_8530_WRITE(cr, SCC_WR0_SEL_WR0); /* read register 0 */
    REG_8530_READ(cr, &charStatus);
    if (charStatus & SCC_RR0_TX_UND)
	REG_8530_WRITE(cr, SCC_WR0_RST_TX_UND);

    REG_8530_WRITE(cr, SCC_WR0_ERR_RST);	/* reset errors */
    REG_8530_WRITE(cr, SCC_WR0_RST_INT);	/* reset ext/status interrupt */
    REG_8530_WRITE(cr, SCC_WR0_RST_HI_IUS);	/* reset the interrupt */
    }

/********************************************************************************
* z8530Int - handle all interrupts in one vector
*
* On some boards, all SCC interrupts for both ports share a single interrupt
* vector. This is the ISR for such boards.
* We determine from the parameter which SCC interrupted, then look at
* the code to find out which channel and what kind of interrupt.
*
* RETURNS: N/A
*/

void z8530Int
    (
    Z8530_DUSART * pDusart
    )
    {
    volatile char * crA;
    volatile char * crB;
    Z8530_CHAN *    pChanA;
    Z8530_CHAN *    pChanB;
    char            intStatus;
    char            charStatus;
    char            thisCharA;
    char            thisCharB;
    BOOL            rxA = FALSE;
    BOOL            rxB = FALSE;
#ifdef DATA_REG_8530_DIRECT
    volatile char * drA;
    volatile char * drB;
#endif  /* DATA_REG_8530_DIRECT */

    /*
     * We need to find out if the SCC interrupted.  We need to read
     * the A channel of the SCC to find out if there are any pending
     * interrupts.  Note that things are set up so that the A
     * channel is channel 0, even though on the chip it is the one with
     * the higher address.
     */  

    pChanA = &pDusart->portA;		/* Load ch A control reg address */
    crA    = pChanA->cr;

    /* get pending interrupt flags from Z8530 (found only in ch A) */

    REG_8530_WRITE(crA, SCC_WR0_SEL_WR3); /* read reg 3 */
    REG_8530_READ(crA, &intStatus);

    /* If no interrupt pending, interrupt was not from us; return immediately */

    if (intStatus == 0)
      return;

    /* our interrupt; handle all pending interrupts */

    pChanB = &pDusart->portB;		/* Load ch B control reg address */
    crB    = pChanB->cr;
#ifdef DATA_REG_8530_DIRECT
    drA = pChanA->dr;
    drB = pChanB->dr;
#endif  /* DATA_REG_8530_DIRECT */

    /*
     * service the interrupt by type with Tx ints being highest priority
     * and ch A having priority over B.  Ignore Special Receive Conditions.
     */

    /* ch A Tx READY */

    if ((intStatus & SCC_RR3_A_TX_IP) != 0)
        {
	/* Either output the next character, */

	if ((*pChanA->getTxChar) (pChanA->getTxArg, &thisCharA) != ERROR)
	    {
#ifndef DATA_REG_8530_DIRECT
	    REG_8530_WRITE(crA, SCC_WR0_SEL_WR8); /* select data register */
            REG_8530_WRITE(crA, thisCharA);
#else
            REG_8530_WRITE(drA, thisCharA);
#endif  /* DATA_REG_8530_DIRECT */
	    }

	/* or reset/stop the tx. */

        else
	    REG_8530_WRITE(crA, SCC_WR0_RST_TX_INT);

	/* update interrupt status */

	intStatus &= ~SCC_RR3_A_TX_IP;
        }
 
    /* ch B Tx READY */

    if ((intStatus & SCC_RR3_B_TX_IP) != 0)
        {
	/* Either output the next character, */

	if ((*pChanB->getTxChar) (pChanB->getTxArg, &thisCharB) != ERROR)
	    {
#ifndef DATA_REG_8530_DIRECT
	    REG_8530_WRITE(crB, SCC_WR0_SEL_WR8); /* select data register */
            REG_8530_WRITE(crB, thisCharB);
#else
            REG_8530_WRITE(drB, thisCharB);
#endif  /* DATA_REG_8530_DIRECT */
	    }

	/* or reset/stop the tx. */

        else
	    REG_8530_WRITE(crB, SCC_WR0_RST_TX_INT);

	/* update interrupt status */

	intStatus &= ~SCC_RR3_B_TX_IP;
        }
 
    /* ch A Rx CHAR AVAILABLE */

    if ((intStatus & SCC_RR3_A_RX_IP) != 0)
        {
	/*
	 * Receiver is FIFO based so there may be more than one char to read.
	 * Loop here until all chars are read.
	 */
	do
	    {
	    /* see if character is valid, if not just read and discard it */

	    REG_8530_WRITE(crA, SCC_WR0_SEL_WR1); /* read register 1 */
	    REG_8530_READ(crA, &charStatus);
	    if ((charStatus & (SCC_RR1_CRC_ERR   |  /* framing error */
			       SCC_RR1_RX_OV_ERR |  /* receive overrun */
			       SCC_RR1_PAR_ERR))    /* parity error */
	        == 0)
	        rxA = TRUE;
	    else
		{
		rxA = FALSE;
                REG_8530_WRITE(crA, SCC_WR0_ERR_RST);  /* reset error */
		}

	    /* read the received character */

#ifndef DATA_REG_8530_DIRECT
	    REG_8530_WRITE(crA, SCC_WR0_SEL_WR8); /* select data register */
	    REG_8530_READ(crA, &thisCharA);
#else
	    REG_8530_READ(drA, &thisCharA);
#endif  /* DATA_REG_8530_DIRECT */

	    /* do callback if valid char */

	    if (rxA)
		(*pChanA->putRcvChar) (pChanA->putRcvArg, thisCharA);

	    /* See if more chars available */

	    REG_8530_READ(crA, &charStatus);
	    } while (charStatus & SCC_RR0_RX_AVAIL);

	/* update interrupt status */

	intStatus &= ~SCC_RR3_A_RX_IP;
        }
 
    /* ch B Rx CHAR AVAILABLE */

    if ((intStatus & SCC_RR3_B_RX_IP) != 0)
        {
	/*
	 * Receiver is FIFO based so there may be more than one char to read.
	 * Loop here until all chars are read.
	 */
	do
	    {
	    /* see if character is valid, if not just read and discard it */

	    REG_8530_WRITE(crB, SCC_WR0_SEL_WR1); /* read register 1 */
	    REG_8530_READ(crB, &charStatus);
	    if ((charStatus & (SCC_RR1_CRC_ERR   |  /* framing error */
			       SCC_RR1_RX_OV_ERR |  /* receive overrun */
			       SCC_RR1_PAR_ERR))    /* parity error */
	        == 0)
	        rxB = TRUE;
	    else
		{
		rxB = FALSE;
                REG_8530_WRITE(crB, SCC_WR0_ERR_RST);  /* reset error */
		}

	    /* read the received character */

#ifndef DATA_REG_8530_DIRECT
	    REG_8530_WRITE(crB, SCC_WR0_SEL_WR8); /* select data register */
	    REG_8530_READ(crB, &thisCharB);
#else
	    REG_8530_READ(drB, &thisCharB);
#endif  /* DATA_REG_8530_DIRECT */

	    /* do callback if valid char */

	    if (rxB)
		(*pChanB->putRcvChar) (pChanB->putRcvArg, thisCharB);

	    /* See if more chars available */

	    REG_8530_READ(crB, &charStatus);
	    } while (charStatus & SCC_RR0_RX_AVAIL);

	/* update interrupt status */

	intStatus &= ~SCC_RR3_B_RX_IP;
        }
 
    /* ch A EXTERNAL STATUS CHANGE */

    if ((intStatus & SCC_RR3_A_EXT_IP) != 0)
        {
	/* if Tx Underrun, reset it */

        REG_8530_WRITE(crA, SCC_WR0_SEL_WR0); /* read register 0 */
        REG_8530_READ(crA, &charStatus);
	if (charStatus & SCC_RR0_TX_UND)
	    REG_8530_WRITE(crA, SCC_WR0_RST_TX_UND);

	/* reset ext/status interrupt */

        REG_8530_WRITE(crA, SCC_WR0_RST_INT);
        }
 
    /* ch B EXTERNAL STATUS CHANGE */

    if ((intStatus & SCC_RR3_B_EXT_IP) != 0)
        {
	/* if Tx Underrun, reset it */

        REG_8530_WRITE(crB, SCC_WR0_SEL_WR0); /* read register 0 */
        REG_8530_READ(crB, &charStatus);
	if (charStatus & SCC_RR0_TX_UND)
	    REG_8530_WRITE(crB, SCC_WR0_RST_TX_UND);

	/* reset ext/status interrupt */

        REG_8530_WRITE(crB, SCC_WR0_RST_INT);
        }
 
    /* reset any error condition */

    REG_8530_WRITE(crA, SCC_WR0_ERR_RST);

    /* Reset the interrupt in the Z8530 */

    REG_8530_WRITE(crA, SCC_WR0_RST_HI_IUS);

    }

