/* nvr4101DSIUSio.c - NEC VR4101 DSIU UART tty driver */

/* Copyright 1984-2002 Wind River Systems, Inc. */

#include "copyright_wrs.h"

/*
modification history
--------------------
01b,24apr02,pmr  SPR 75161: returning int from nvr4101DSIUTxStartup() as
                 required.
01a,23jun97,sru  written.
*/

/*
DESCRIPTION
This is the device driver for the nvr4101 DSIU UART.

USAGE
An NVR4101_DSIU_CHAN data structure is used to describe the DSIU.

The BSP's sysHwInit() routine typically calls sysSerialHwInit(), which
should pass a pointer to an uninitialized NVR4101_DSIU_CHAN structure to
nvr4101DSIUDevInit().  The BSP's sysHwInit2() routine typically calls
sysSerialHwInit2(), which connects the chip's interrupts via
intConnect().

INCLUDE FILES: drv/sio/nvr4101DSIUSio.h

*/

#include "vxWorks.h"
#include "intLib.h"
#include "logLib.h"
#include "errnoLib.h"
#include "errno.h"
#include "sioLib.h"
#include "memLib.h"
#include "stdlib.h"
#include "drv/multi/nvr4101.h"
#include "drv/sio/nvr4101DSIUSio.h"

/* local defines */

#define RX_DSIU_INT_SOURCES VR4101_INTSR0
#define TX_DSIU_INT_SOURCES VR4101_INTST0

#define RX_ICU_INT_SOURCES  VR4101_ICU_SIU_INTSR0
#define TX_ICU_INT_SOURCES  VR4101_ICU_SIU_INTST0

/* min/max baud rate */

#define NVR4101_MIN_RATE 1200
#define NVR4101_MAX_RATE 115200

typedef struct /* BAUD */
    {
    int rate;        /* baud rate */
    int cntl;        /* control bits */
    } BAUD;

LOCAL BAUD baudTable [] =
    {
    {   1200, 0},
    {   2400, 1},
    {   4800, 2},
    {   9600, 3},
    {  19200, 4},
    {  38400, 5},
    {  57600, 6},
    { 115200, 7}
    };

/* function prototypes */

LOCAL int nvr4101DSIUCallbackInstall (SIO_CHAN *, int, STATUS (*)(), void *);
LOCAL STATUS nvr4101DSIUDummyCallback ();
LOCAL void nvr4101DSIUInitChannel (NVR4101_DSIU_CHAN *);
LOCAL STATUS nvr4101DSIUIoctl (NVR4101_DSIU_CHAN *, int, int);
LOCAL int nvr4101DSIUTxStartup (NVR4101_DSIU_CHAN *);
LOCAL int nvr4101DSIUPollOutput (NVR4101_DSIU_CHAN *, char);
LOCAL int nvr4101DSIUPollInput (NVR4101_DSIU_CHAN *, char *);
LOCAL void nvr4101DSIUIntMask ();
LOCAL void nvr4101DSIUIntUnmask ();


/* driver functions */

LOCAL SIO_DRV_FUNCS nvr4101DSIUSioDrvFuncs =
    {
    (int (*)())nvr4101DSIUIoctl,
    (int (*)())nvr4101DSIUTxStartup,
    (int (*)())nvr4101DSIUCallbackInstall,
    (int (*)())nvr4101DSIUPollInput,
    (int (*)(SIO_CHAN *,char))nvr4101DSIUPollOutput
    };

/******************************************************************************
*
* nvr4101DSIUDummyCallback - dummy callback routine.
*/

LOCAL STATUS nvr4101DSIUDummyCallback (void)
    {
    return (ERROR);
    }

/******************************************************************************
*
* nvr4101DSIUDevInit - initialization of the NVR4101DSIU DSIU.
*
* This routine initializes some SIO_CHAN function pointers and then resets
* the chip in a quiescent state.  The caller needs to initialize the
* channel structure with the requested word length and parity.
*
* RETURNS: N/A
*/

void nvr4101DSIUDevInit
    (
    NVR4101_DSIU_CHAN * pChan
    )
    {
    int oldlevel;

    oldlevel = intLock ();

    /* initialize the driver function pointers in the SIO_CHAN's */

    pChan->sio.pDrvFuncs = &nvr4101DSIUSioDrvFuncs;
    pChan->getTxChar = nvr4101DSIUDummyCallback;
    pChan->putRcvChar = nvr4101DSIUDummyCallback;

    /* set the DSIU state variables */

    pChan->txActive = FALSE;
    pChan->channelMode = -1;  /* so ioctl will notice first change */

    /* reset the DSIU */

    nvr4101DSIUInitChannel (pChan);

    intUnlock (oldlevel);
    }

/*******************************************************************************
*
* nvr4101DSIUInitChannel - initialize UART
*
* RETURNS: N/A.
*/

LOCAL void nvr4101DSIUInitChannel
    (
    NVR4101_DSIU_CHAN *pChan
    )
    {
    /* forcibly reset the DSIU */

    *VR4101_DSIURESETREG = VR4101_DSIURST;

    /*
     * Enable data reception, 1 stop bit, user-selected parity and 
     * user-selected word length.
     */

    *VR4101_ASIM00REG = VR4101_ASIM00REG_RESERVED | /* mandatory "1" bit */
      			VR4101_RXE0 |		    /* enable reception */
      			VR4101_DSIU_STOPBITS_1 |    /* 1 stop bit */
#if 1
      			(pChan->parityStyle << VR4101_DSIU_PAR_SHIFT) |
      			((pChan->wordLength == 7) ? VR4101_DSIU_CHARLEN_7 :
      						    VR4101_DSIU_CHARLEN_8);
#else
      			VR4101_DSIU_PAR_EVEN |
			VR4101_DSIU_CHARLEN_7;
#endif

    /*
     * Unmask the desired interrupts.  Note that the (D)SIU summary interrupt
     * bit within the VR4101_ICU_MSYSINTREG register is _not_ enabled
     * by this driver.  This allows other parts of the BSP to
     * selectively enable or disable the entire (D)SIU using the summary bit, 
     * and hides the details of the specific unmasked (D)SIU interrupts within 
     * this driver. 
     */

    nvr4101DSIUIntUnmask ();
    }

/*******************************************************************************
*
* nvr4101DSIUIoctl - special device control
*
* RETURNS: OK on success, EIO on device error, ENOSYS on unsupported
*          request.
*/

LOCAL STATUS nvr4101DSIUIoctl
    (
    NVR4101_DSIU_CHAN *pChan,	/* device to control */
    int        request,		/* request code */
    int        arg		/* some argument */
    )
    {
    int oldlevel;
    STATUS status;
    int	i;

    status = OK;

    switch (request)
	{
	case SIO_BAUD_SET:
            status = EIO;  /* assume baud rate out of range */
            for (i = 0; i < NELEMENTS (baudTable); i++)
                {
                if (baudTable [i].rate == (int)arg)
                    {
		    pChan->baudRate = arg;
		    oldlevel = intLock ();
		    *VR4101_BPRM0REG = baudTable [i].cntl | VR4101_BRCE0;
		    intUnlock (oldlevel);
                    status = OK;
                    break;
                    }
	        }
	    break;

        case SIO_BAUD_GET:
            *(int *)arg = pChan->baudRate;
            break; 

        case SIO_MODE_SET:
            if ((arg != SIO_MODE_POLL) && (arg != SIO_MODE_INT))
                {
                status = EIO;
                break;
                }
           
	    if (arg != pChan->channelMode)
		{
		if (arg == SIO_MODE_INT)
		    {
		    /* clear all pending interrupts */

		    *VR4101_INTR0REG = TX_DSIU_INT_SOURCES | 
				       RX_DSIU_INT_SOURCES;

		    /* mark that we're not awaiting xmit interrupt */

		    pChan->txActive = FALSE;

		    /* Enable appropriate interrupts */

		    nvr4101DSIUIntUnmask ();
		    }
		else
		    {
		    /* Disable the interrupts */ 
		    
		    nvr4101DSIUIntMask ();
		    }
		pChan->channelMode = arg;
		}
            break;          

        case SIO_MODE_GET:
            *(int *)arg = pChan->channelMode;
            break;

        case SIO_AVAIL_MODES_GET:
            *(int *)arg = SIO_MODE_INT | SIO_MODE_POLL;
            break;

        case SIO_HW_OPTS_SET:
        case SIO_HW_OPTS_GET:
        default:
            status = ENOSYS;
	}
    return (status);
    }

/********************************************************************************
* nvr4101DSIUInt - interrupt level processing
*
* This routine handles interrupts from the DSIU.
*
* RETURNS: N/A
*/

void nvr4101DSIUInt
    (
    NVR4101_DSIU_CHAN *pChan
    )
    {
    UINT16 	statusReg;
    char	outChar;

    /* get status from the DSIU (not ICU) subsystem.  */

    statusReg = *VR4101_INTR0REG;

    /* write status value back out to clear the various sources */

    *VR4101_INTR0REG = statusReg;

    /* handle receiver interrupt */

    if (statusReg & RX_DSIU_INT_SOURCES)
	{
	(*pChan->putRcvChar) (pChan->putRcvArg, (char) *VR4101_RXB0LREG);
			      
	}

    /* handle transmitter interrupt */

    if (statusReg & TX_DSIU_INT_SOURCES)
	{
	if ((*pChan->getTxChar) (pChan->getTxArg, &outChar) != ERROR)
	    {
	    /* transmit the next character */

	    *VR4101_TXS0LREG = (UINT16) outChar;
	    }
	else
	    {
	    /* note that TxStartup will need to generate next character */

	    pChan->txActive = FALSE;
	    }
	}

    }

/*******************************************************************************
*
* nvr4101DSIUTxStartup - transmitter startup routine.
*
* This routine is called to restart data output.  the VR4101 does not
* provide a mechanism to regenerate a transmit interrupt, so this
* function needs to perform the work involved in setting up the data
* transmission.
*
* RETURNS: OK, or ENOSYS if in polled mode.
*/

LOCAL int nvr4101DSIUTxStartup
    (
    NVR4101_DSIU_CHAN *pChan 
    )
    {
    char outChar;
    int oldLevel;

    if (pChan->channelMode == SIO_MODE_INT)
	{
	oldLevel = intLock ();

	/* ignore request if expecting transmitter interrupt */

	if (pChan->txActive)
	    {
	    intUnlock (oldLevel);
	    return (OK);
	    }

	/* mark that we've started transmission */

	pChan->txActive = TRUE;
	intUnlock (oldLevel);

	/* initiate transmission of the next character */

	(void) (*pChan->getTxChar) (pChan->getTxArg, &outChar);
	*VR4101_TXS0LREG = (UINT16) outChar;

	return (OK);
	}
    else 
	{
	return (ENOSYS);
	}
    }


/******************************************************************************
*
* nvr4101DSIUPollOutput - output a character in polled mode.
*
* RETURNS: OK if a character arrived, EIO on device error, EAGAIN
*          if the output buffer if full.
*/

LOCAL int nvr4101DSIUPollOutput
    (
    NVR4101_DSIU_CHAN *  pChan,
    char            outChar
    )
    {
    /* if transmitting, come back later */

    if (*VR4101_ASIS0REG & VR4101_SOT0)
	return (EAGAIN);

    /* transmit the character */

    *VR4101_TXS0LREG = (UINT16) outChar;
    return (OK);
    }

/******************************************************************************
*
* nvr4101DSIUPollInput - poll the device for input.
*
* RETURNS: OK if a character arrived, EIO on device error, EAGAIN
*          if the input buffer if empty.
*/

LOCAL int nvr4101DSIUPollInput
    (
    NVR4101_DSIU_CHAN *  pChan,
    char *          thisChar
    )
    {
    /* if no receive interrupt, come back later */

    if (! (*VR4101_INTR0REG & RX_DSIU_INT_SOURCES))
	return (EAGAIN);

    /* receive the character */

    *thisChar = (char) *VR4101_RXB0LREG;

    /* clear the interrupt */

    *VR4101_INTR0REG = RX_DSIU_INT_SOURCES;
    return (OK);
    }

/******************************************************************************
*
* nvr4101DSIUCallbackInstall - install ISR callbacks to get/put chars.
*/

LOCAL int nvr4101DSIUCallbackInstall
    (
    SIO_CHAN *  pSioChan,
    int         callbackType,
    STATUS      (*callback)(),
    void *      callbackArg
    )
    {
    NVR4101_DSIU_CHAN * pChan = (NVR4101_DSIU_CHAN *) pSioChan;

    switch (callbackType)
        {
        case SIO_CALLBACK_GET_TX_CHAR:
            pChan->getTxChar    = callback;
            pChan->getTxArg     = callbackArg;
            return (OK);
        case SIO_CALLBACK_PUT_RCV_CHAR:
            pChan->putRcvChar   = callback;
            pChan->putRcvArg    = callbackArg;
            return (OK);
        default:
            return (ENOSYS);
        }

    }


/******************************************************************************
*
* nvr4101DSIUIntMask - Mask interrupts from the DSIU.
* 
* This function masks all possible interrupts from the DSIU subsystem.
*
* RETURNS: N/A
*/

void nvr4101DSIUIntMask()
    {
    int oldLevel = intLock ();
    *VR4101_ICU_MSIUINTREG &= ~(RX_ICU_INT_SOURCES | TX_ICU_INT_SOURCES);
    intUnlock (oldLevel);
    }

/******************************************************************************
*
* nvr4101DSIUIntUnmask - Unmask interrupts from the DSIU.
* 
* This function unmasks all desired interrupts from the DSIU subsystem.
*
* RETURNS: N/A
*/
		
void nvr4101DSIUIntUnmask()
    {
    int oldLevel = intLock ();
    *VR4101_ICU_MSIUINTREG |= (RX_ICU_INT_SOURCES | TX_ICU_INT_SOURCES);
    intUnlock (oldLevel);
    }
