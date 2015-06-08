/* nvr4101SIUSio.c - NEC VR4101 SIU UART tty driver */

/* Copyright 1984-2002 Wind River Systems, Inc. */

#include "copyright_wrs.h"

/*
modification history
--------------------
01b,24apr02,pmr  SPR 75161: returning int from nvr4101SIUTxStartup() as
                 required.
01a,11jun97,sru  written.
*/

/*
DESCRIPTION
This is the device driver for the nvr4101 UART.

USAGE
A NVR4101_CHAN data structure is used to describe the SIU.

The BSP's sysHwInit() routine typically calls sysSerialHwInit(), which
should pass a pointer to an uninitialized NVR4101_CHAN structure to
nvr4101SIUDevInit().  The BSP's sysHwInit2() routine typically calls
sysSerialHwInit2(), which connects the chip's interrupts via
intConnect().

INCLUDE FILES: drv/sio/nvr4101SIUSio.h

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
#include "drv/sio/nvr4101SIUSio.h"

/* local defines */

#define RX_INT_SOURCES (VR4101_ICU_SIU_RXG |	/* 1-char reception end */ \
			VR4101_ICU_SIU_RXE |	/* rx DMA 2-page end */ \
			VR4101_ICU_SIU_RXI)	/* rx DMA 1-page end */

#define TX_INT_SOURCES (VR4101_ICU_SIU_TXI | 	/* tx DMA 1-page end */ \
			VR4101_ICU_SIU_TXE)	/* tx DMA 2-page end */

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

LOCAL char txrxBuf [3 * DMA_PAGE_SIZE];

/* function prototypes */

LOCAL int nvr4101SIUCallbackInstall (SIO_CHAN *, int, STATUS (*)(), void *);
LOCAL STATUS nvr4101SIUDummyCallback ();
LOCAL void nvr4101SIUInitChannel (NVR4101_SIU_CHAN *);
LOCAL STATUS nvr4101SIUIoctl (NVR4101_SIU_CHAN *, int, int);
LOCAL int nvr4101SIUTxStartup (NVR4101_SIU_CHAN *);
LOCAL int nvr4101SIUPollOutput (NVR4101_SIU_CHAN *, char);
LOCAL int nvr4101SIUPollInput (NVR4101_SIU_CHAN *, char *);
LOCAL void nvr4101SIUIntMask ();
LOCAL void nvr4101SIUIntUnmask ();
LOCAL UINT16 nvr4101SIUCharToTxWord (char outChar);
LOCAL char nvr4101SIURxWordToChar (UINT16 inWord);
LOCAL void nvr4101SIUTxOutput (NVR4101_SIU_CHAN *pChan, char outChar);


/* driver functions */

LOCAL SIO_DRV_FUNCS nvr4101SIUSioDrvFuncs =
    {
    (int (*)())nvr4101SIUIoctl,
    (int (*)())nvr4101SIUTxStartup,
    (int (*)())nvr4101SIUCallbackInstall,
    (int (*)())nvr4101SIUPollInput,
    (int (*)(SIO_CHAN *,char))nvr4101SIUPollOutput
    };

/******************************************************************************
*
* nvr4101SIUDummyCallback - dummy callback routine.
*/

LOCAL STATUS nvr4101SIUDummyCallback (void)
    {
    return (ERROR);
    }

/******************************************************************************
*
* nvr4101SIUDevInit - initialization of the NVR4101SIU SIU.
*
* This routine initializes some SIO_CHAN function pointers and then resets
* the chip in a quiescent state.  No initialization of the NVR4101_SIU_CHAN
* structure is required before this routine is called.
*
* RETURNS: N/A
*/

void nvr4101SIUDevInit
    (
    NVR4101_SIU_CHAN * pChan
    )
    {
    int oldlevel;

    oldlevel = intLock ();

    /* initialize the driver function pointers in the SIO_CHAN's */

    pChan->sio.pDrvFuncs = &nvr4101SIUSioDrvFuncs;
    pChan->getTxChar = nvr4101SIUDummyCallback;
    pChan->putRcvChar = nvr4101SIUDummyCallback;

    /* set the SIU state variables */

    pChan->txActive = FALSE;
    pChan->channelMode = -1;  /* so ioctl will notice first change */

    /* construct pointers to rx/tx buffers aligned with DMA boundaries */

    pChan->pTxBuf = (char *) ROUND_UP ((int) txrxBuf, DMA_PAGE_SIZE);
    pChan->pTxBuf = (char *) K0_TO_K1 (pChan->pTxBuf);
    pChan->pRxBuf = pChan->pTxBuf + DMA_PAGE_SIZE;

    /* construct pointers to the 2nd-to-last word in each buffer */

    pChan->pTxWord = ((UINT16 *) (pChan->pTxBuf + DMA_PAGE_SIZE)) - 2;
    pChan->pRxWord = ((UINT16 *) (pChan->pRxBuf + DMA_PAGE_SIZE)) - 2;

    /* make the last word a word of all stop bits */

    pChan->pTxWord[1] = ~0;
    pChan->pTxWord[1] = ~0;

    /* reset the SIU */

    nvr4101SIUInitChannel (pChan);

    intUnlock (oldlevel);
    }

/*******************************************************************************
*
* nvr4101SIUInitChannel - initialize UART
*
* RETURNS: N/A.
*/

LOCAL void nvr4101SIUInitChannel
    (
    NVR4101_SIU_CHAN *pChan
    )
    {
    UINT32 rxAdr;

    /* enable SIU clock from within the CIU subsystem */

    *VR4101_CMUCLKMSK |= VR4101_MSKSIU;

    /* forcibly reset the SIU */

    *VR4101_SIUCNTREG |= VR4101_SIU_SRST;

    /* 
     * Set transmit length of 1 start bit, 8 data bits, 1 stop bit, 
     * no parity. (1+8+1 = 10).  The receive length is identical,
     * except that the stop bit is not included in the calculation,
     * so the receive length is one less than the transmit length. 
     */

    *VR4101_SIUDLENGTHREG = (10 << VR4101_SIU_TX_CHAR_WIDTH_SHIFT) |
                            (9 << VR4101_SIU_RX_CHAR_WIDTH_SHIFT) |
		            VR4101_SIU_STOP_BITS_1;

    /*
     * Select RS232 mode, and set transmit and receive DMA stop 
     * addresses at page 1 boundary.
     */

    *VR4101_SIUCNTREG = VR4101_SIU_RS232_MODE |
      			VR4101_SIU_RSP | VR4101_SIU_TSP;

    /* Confirm that DTR and RTS are high. */

    *VR4101_SIURS232CREG = VR4101_SIU_DTR | VR4101_SIU_RTS;

    /* Set receive DMA address pointer */

    rxAdr = K0_TO_K1 (pChan->pRxWord);
    *VR4101_SRXDMAADRLREG = (UINT16) rxAdr;
    *VR4101_SRXDMAADRHREG = (UINT16) (rxAdr >> 16);

    /* enable the receiver */

    *VR4101_SIUCNTREG |= VR4101_SIU_RXE;

    /* 
     * unmask the desired interrupts.  Note that the SIU summary interrupt
     * bit within the VR4101_ICU_MSYSINTREG register is _not_ enabled
     * by this driver.  This allows other parts of the BSP to
     * selectively enable or disable the entire SIU using the summary bit, 
     * and hides the details of the specific unmasked SIU interrupts within 
     * this driver. 
     */

    nvr4101SIUIntUnmask ();
    }

/*******************************************************************************
*
* nvr4101SIUIoctl - special device control
*
* RETURNS: OK on success, EIO on device error, ENOSYS on unsupported
*          request.
*/

LOCAL STATUS nvr4101SIUIoctl
    (
    NVR4101_SIU_CHAN *pChan,	/* device to control */
    int        request,		/* request code */
    int        arg		/* some argument */
    )
    {
    int    oldlevel;
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
		    *VR4101_SIUBAUDSELREG = baudTable [i].cntl;
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

		    *VR4101_SIUINTREG = TX_INT_SOURCES | RX_INT_SOURCES;

		    /* mark that we're not awaiting xmit interrupt */

		    pChan->txActive = FALSE;

		    /* Enable appropriate interrupts */

		    nvr4101SIUIntUnmask ();
		    }
		else
		    {
		    /* Disable the interrupts */ 
		    
		    nvr4101SIUIntMask ();
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
* nvr4101SIUInt - interrupt level processing
*
* This routine handles interrupts from the SIU.
*
* RETURNS: N/A
*/

void nvr4101SIUInt
    (
    NVR4101_SIU_CHAN *pChan
    )
    {
    UINT16 	statusReg;
    char	outChar;

    /* get status from the SIU (not ICU) subsystem.  */

    statusReg = *VR4101_SIUINTREG;

    /* write status value back out to clear the various sources */

    *VR4101_SIUINTREG = statusReg;

    /* handle receiver interrupt */

    if (statusReg & RX_INT_SOURCES)
	{
	(*pChan->putRcvChar) (pChan->putRcvArg, 
			      nvr4101SIURxWordToChar (*VR4101_SIURXDATREG));
	}

    /* handle transmitter interrupt */

    if (statusReg & TX_INT_SOURCES)
	{
	/* Disable the sequencer */

	*VR4101_SIUCNTREG &= ~VR4101_SIU_TXE;

	if ((*pChan->getTxChar) (pChan->getTxArg, &outChar) != ERROR)
	    {
	    /* transmit the next character */

	    nvr4101SIUTxOutput (pChan, outChar);
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
* nvr4101SIUTxStartup - transmitter startup routine.
*
* This routine is called to restart data output.  the VR4101 does not
* provide a mechanism to regenerate a transmit interrupt, so this
* function needs to perform the work involved in setting up the data
* transmission.
*
* RETURNS: OK, or ENOSYS if in polled mode.
*/

LOCAL int nvr4101SIUTxStartup
    (
    NVR4101_SIU_CHAN *pChan 
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
	nvr4101SIUTxOutput (pChan, outChar);

	return (OK);
	}
    else
	{
	return (ENOSYS);
	}
    }


/******************************************************************************
*
* nvr4101SIUTxOutput - Transmit requested character.
* 
* This function sets up the transmitter to send the requested character.
*
* RETURNS: N/A
*/

LOCAL void nvr4101SIUTxOutput
    (
    NVR4101_SIU_CHAN *pChan,
    char outChar
    )
    {
    UINT32 txAdr;

    /* encode the character, and place in the output buffer */

    *pChan->pTxWord = nvr4101SIUCharToTxWord (outChar);
	    
    /* write the buffer address to the the DMA address registers. */
    
    txAdr = K0_TO_K1 (pChan->pTxWord);
    *VR4101_STXDMAADRLREG = (UINT16) txAdr;
    *VR4101_STXDMAADRHREG = (UINT16) (txAdr >> 16);

    /* start the transmit sequencer */

    *VR4101_SIUCNTREG |= VR4101_SIU_TXE;
    }


/******************************************************************************
*
* nvr4101SIUPollOutput - output a character in polled mode.
*
* RETURNS: OK if a character arrived, EIO on device error, EAGAIN
*          if the output buffer if full.
*/

LOCAL int nvr4101SIUPollOutput
    (
    NVR4101_SIU_CHAN *  pChan,
    char            outChar
    )
    {
    /* if haven't gotten DMA interrupt, we're still transmitting */

    if (pChan->txActive && 
	((*VR4101_SIUINTREG & TX_INT_SOURCES) == 0))
	return (EAGAIN);

    /* mark that we're starting transmission */

    pChan->txActive = TRUE;

    /* clear the interrupt bits */

    *VR4101_SIUINTREG = TX_INT_SOURCES;

    /* disable the sequencer */

    *VR4101_SIUCNTREG &= ~VR4101_SIU_TXE;

    /* ok to send the character */

    nvr4101SIUTxOutput (pChan, outChar);
    return (OK);
    }


/******************************************************************************
*
* nvr4101SIUPollInput - poll the device for input.
*
* RETURNS: OK if a character arrived, EIO on device error, EAGAIN
*          if the input buffer if empty.
*/

LOCAL int nvr4101SIUPollInput
    (
    NVR4101_SIU_CHAN *  pChan,
    char *          thisChar
    )
    {
    /* if no receiver interrupt, no character */

    if (!(*VR4101_SIUINTREG & RX_INT_SOURCES))
	return (EAGAIN);

    /* clear the interrupt */

    *VR4101_SIUINTREG = RX_INT_SOURCES;

    /* read the character */

    *thisChar = nvr4101SIURxWordToChar (*VR4101_SIURXDATREG);
    return (OK);
    }

/******************************************************************************
*
* nvr4101SIUCallbackInstall - install ISR callbacks to get/put chars.
*/

LOCAL int nvr4101SIUCallbackInstall
    (
    SIO_CHAN *  pSioChan,
    int         callbackType,
    STATUS      (*callback)(),
    void *      callbackArg
    )
    {
    NVR4101_SIU_CHAN * pChan = (NVR4101_SIU_CHAN *) pSioChan;

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
* nvr4101SIUIntMask - Mask interrupts from the SIU.
* 
* This function masks all possible interrupts from the SIU subsystem.
*
* RETURNS: N/A
*/

void nvr4101SIUIntMask()
    {
    int oldLevel;

    oldLevel = intLock ();
    *VR4101_ICU_MSIUINTREG &= ~(RX_INT_SOURCES | TX_INT_SOURCES);
    intUnlock (oldLevel);
    }

/******************************************************************************
*
* nvr4101SIUIntUnmask - Unmask interrupts from the SIU.
* 
* This function unmasks all desired interrupts from the SIU subsystem.
*
* RETURNS: N/A
*/
		
void nvr4101SIUIntUnmask()
    {
    int oldLevel;

    oldLevel = intLock ();
    *VR4101_ICU_MSIUINTREG |= (RX_INT_SOURCES | TX_INT_SOURCES);
    intUnlock (oldLevel);
    }

/******************************************************************************
*
* nvr4101SIUCharToTxWord - Translate character to output word format.
* 
* This routine performs the bit manipulations required to convert 
* a character into the output word format required by the SIU.  
* This routine only supports 8-bit word lengths, two stop bits
* and no parity.
*
* RETURNS: The translated output character.
*/


UINT16 nvr4101SIUCharToTxWord
    (
    char outChar
    )
    {
    /* char --> [1][char][0] */
    return ((0x100 | (UINT16) outChar) << 1);
    }


/******************************************************************************
*
* nvr4101SIURxWordToChar - Extract character from received word.
* 
* This function performs the bit manipulations required to extract
* the received character from its encoded word.  This routine only
* supports 8-bit word lengths, one stop bit and no parity.
*
* RETURNS: The received character.
*/

LOCAL char nvr4101SIURxWordToChar
    (
    UINT16 inWord
    )
    {
    return (char) (inWord >> 8);
    }
