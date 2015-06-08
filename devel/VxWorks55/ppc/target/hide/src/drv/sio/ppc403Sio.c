/* ppc403Sio.c - ppc403GA serial driver */

/* Copyright 1984-2001 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01g,18oct01,dat  Documentation fixes
01f,30aug01,pch  Remove #include "arch/ppc/ppc403.h".  For PPC403, it is
		 brought in by vxWorks.h.  ppc403Sio.c is not built for
		 other CPU types, but the #include causes warnings during
		 dependency generation.
01e,07aug97,tam	 removed clearing of EXIRS[SRI] bit in ppc403IntRd() (SPR 9114)
01d,06nov96,dgp  doc: final formatting
01c,28oct96,tam  fixed typo in ppc403DummyCallback to build man pages.
01b,12jul96,tam  corrected code to avoid getting spurious interrupt when 
		 interrupt nesting's enabled.
01a,13feb96,tam  written (using evb403_diab/tyCoDrv.c version 01a).
*/

/*

DESCRIPTION
This is the driver for PPC403GA serial port on the on-chip peripheral bus.
The SPU (serial port unit) consists of three main elements: receiver,
transmitter, and baud-rate generator.  For details, refer to the 
.I PPC403GA Embedded Controller User's Manual.

USAGE
A PPC403_CHAN structure is used to describe the chip. This data structure
contains the single serial channel.
The BSP's sysHwInit() routine typically calls sysSerialHwInit()
which initializes all the values in the PPC403_CHAN structure (except
the SIO_DRV_FUNCS) before calling ppc403DevInit().
The BSP's sysHwInit2() routine typically calls sysSerialHwInit2() which
connects the chip interrupt routines ppc403IntWr() and ppc403IntRd() via 
intConnect().

IOCTL FUNCTIONS
This driver responds to the same ioctl() codes as other SIO drivers; for 
more information, see sioLib.h.

INCLUDE FILES: drv/sio/ppc403Sio.h

*/

#include "vxWorks.h"
#include "iv.h"
#include "intLib.h"
#include "errnoLib.h"
#include "vxLib.h"
#include "drv/sio/ppc403Sio.h"


/* forward declarations */

static void ppc403InitChannel (PPC403_CHAN *);
static int ppc403Ioctl(SIO_CHAN *,int,int);
static int ppc403Startup(SIO_CHAN *pSioChan);
static int ppc403CallbackInstall(SIO_CHAN *,int,STATUS (*callback)(),void *);
static int ppc403PRxChar (SIO_CHAN *pSioChan,char *ch);
static int ppc403PTxChar(SIO_CHAN *pSioChan,char ch);


/* driver functions */

static SIO_DRV_FUNCS ppc403SioDrvFuncs =
    {
    (int (*)())ppc403Ioctl,
    (int (*)())ppc403Startup,
    ppc403CallbackInstall,
    (int (*)())ppc403PRxChar,
    (int (*)(SIO_CHAN *,char))ppc403PTxChar
    };

/******************************************************************************
*
* ppc403DummyCallback - dummy callback routine
*
* RETURNS: ERROR (always).
*/ 

STATUS ppc403DummyCallback (void)
    {
    return (ERROR);
    }

/******************************************************************************
*
* ppc403CallbackInstall - install callbacks to get/put chars
*
* RETURNS:
* Returns OK, or ENOSYS for an unsupported callback type.
*/

static int ppc403CallbackInstall
    (
    SIO_CHAN *  pSioChan,
    int         callbackType,
    STATUS      (*callback)(),
    void *      callbackArg
    )
    {
    PPC403_CHAN * pChan = (PPC403_CHAN *)pSioChan;
    
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
* ppc403DevInit - initialize the serial port unit
*
* The BSP must already have initialized all the device addresses in the
* PPC403_CHAN structure. This routine initializes some SIO_CHAN
* function pointers and then resets the chip in a quiescent state.
*
* RETURNS: N/A.
*/

void ppc403DevInit
    (
    PPC403_CHAN * pChan
    )
    {
    /* initialize the driver function pointers in the SIO_CHAN's */

    pChan->pDrvFuncs    = &ppc403SioDrvFuncs;

    /* set the non BSP-specific constants */

    pChan->baudRate     = DEFAULT_BAUD;
    pChan->getTxChar    = ppc403DummyCallback;
    pChan->putRcvChar   = ppc403DummyCallback;
    pChan->mode         = 0;       			/* undefined */

    /* reset the chip */

    ppc403InitChannel(pChan);
    }

/******************************************************************************
*
* ppc403ModeSet - change mode of device
*
* Implements the MODE_SET ioctl.
*
* RETURNS:
* Returns OK, or ERROR for an invalid mode type.
*/

static STATUS ppc403ModeSet
    (
    PPC403_CHAN * pChan,
    uint_t      newMode
    )
    {
    int oldlevel;
    int oldExier;

    if ((newMode != SIO_MODE_POLL) && (newMode != SIO_MODE_INT))
        return (ERROR);

    oldlevel = intLock ();

    pChan->mode = newMode;

    if (pChan->mode == SIO_MODE_INT)
        {
	/* Program chip in interrupt mode */

	oldExier = vxExierDisable (_PPC403_EXI_STI); 
			/* disable transmitter interrupt - EXIER level */
	*pChan->sptc &= ~ _SPTC_TIE_MASK & ~_SPTC_EIE_MASK & ~_SPTC_DME_TBR;
			/* disable transmitter interrupts - SPTC level */
	*pChan->sprc |= _SPRC_EIE_MASK | _SPRC_DME_RXRDY;
			/* enable Receiver interrupts - SPRC level */
	vxExierEnable (_PPC403_EXI_SRI); 
			/* enable Receiver interrupt - EXIER level */
        }
    else
        {
	/* Program chip in polling mode */

	oldExier = vxExierDisable (_PPC403_EXI_SRI); 
			/* disable Receiver interrupt - EXIER level   */
	oldExier = vxExierDisable (_PPC403_EXI_STI); 
			/* disable Transmitter interrupt - EXIER level */
	*pChan->sptc &= ~ _SPTC_TIE_MASK & ~_SPTC_EIE_MASK & ~_SPTC_DME_TBR;
			/* disable Transmitter interrupts - SPTC level */
	*pChan->sprc &= ~_SPRC_EIE_MASK & ~_SPRC_DME_RXRDY;
			/* disable Receiver interrupts - SPRC level */
	*pChan->sptc  = _SPTC_ET_ENABLE;	/* enable transmiter */
	*pChan->sprc  = _SPRC_ER_ENABLE;	/* enable receiver */
        }

    intUnlock(oldlevel);

    return (OK);
    }

/*******************************************************************************
*
* ppc403InitChannel  - initialize a single channel
*
* RETURNS: N/A.
*/

static void ppc403InitChannel
    (
    PPC403_CHAN *pChan
    )
    {
    char inchar;
    int  oldLevel = intLock ();

    /*
     * Receiver Command Register: Receiver enabled & DMA + RBR interrupt +
     * Receiver error interrupt disabled & RTS controlled by software.
     */

    *pChan->sprc  = _SPRC_DME_DISABLE | _SPRC_ER_ENABLE;

    /* 
     * SPU Control Register: Normal Operation & 8 Data Bits & RTS + DTR
     * active & 1 stop bit & no parity.
     */

    *pChan->spctl = _SPCTL_LM_NORM | _SPCTL_DB_8_BITS | _SPCTL_RTS_ACTIVE |
                    _SPCTL_DTR_ACTIVE | _SPCTL_SB_1_BIT;

    /* 
     * Transmitter Command Register: Transmitter enabled & DMA + TBR interrupt
     * + Transmitter Empty interrupt + Transmitter error interrupt disabled & 
     * Stop mode when CTS active enabled & Transmit Break + Pattern Generation
     * mode disabled.
     */

    *pChan->sptc  = _SPTC_DME_DISABLE | _SPTC_SPE_MASK | _SPTC_ET_ENABLE;

    /* read Receive Buffer */

    inchar = *pChan->sprb;		/* clear the port */

    *pChan->spls = 0xf8;		/* reset bits 0-4 of SPLS */

    vxExisrClear (_PPC403_EXI_SRI); 	/* clear Receiver interrupt Status */ 

    /* enable receiver interrupts */

    *pChan->sprc |= _SPRC_DME_RXRDY | _SPRC_EIE_MASK;
				/* enable Receiver RBR and error interrupts */
    vxExierEnable (_PPC403_EXI_SRI);	/* enable Receiver  interrupt */

    intUnlock (oldLevel);
    }

/*******************************************************************************
*
* ppc403Ioctl - special device control
*
* RETURNS: 
* Returns OK on success, EIO on device error, ENOSYS on unsupported
* request.
*/

static int ppc403Ioctl
    (
    SIO_CHAN * pSioChan,	/* device to control */
    int request,		/* request code */
    int arg			/* some argument */
    )
    {
    PPC403_CHAN * pChan = (PPC403_CHAN *) pSioChan;
    int status = OK;
    int baud;

    switch (request)
        {
        case SIO_BAUD_SET:
            if (arg <= 0)
                status = EIO;
            else
                {
                baud = ((pChan->clkFreq / arg) >> 4) - 1;
                *(char *) _PPC403GA_BRDH = (baud & 0xff00) >> 8;
                *(char *) _PPC403GA_BRDL = baud & 0x00ff;
		pChan->baudRate = arg;
		status = OK;
                }
            break;

	case SIO_BAUD_GET:
	    *(int *)arg = pChan->baudRate;
	    return (OK);

	case SIO_MODE_SET:
            if (!(arg == SIO_MODE_POLL || arg == SIO_MODE_INT))
                {
                status = EIO;
                break;
                }
	    status = (ppc403ModeSet (pChan, arg) == OK ? OK : EIO);
	    break;

	case SIO_MODE_GET:
	    *(int *)arg = pChan->mode;
	    return (OK);

	case SIO_AVAIL_MODES_GET:
	    *(int *)arg = SIO_MODE_INT | SIO_MODE_POLL;
	    return (OK);

        case SIO_HW_OPTS_SET:
        case SIO_HW_OPTS_GET:
        default:
            status = ENOSYS;
        }
    return (status);
    }

/*******************************************************************************
*
* ppc403IntWr - handle a transmitter interrupt
*
* This routine handles write interrupts from the serial communication 
* controller.
*
* RETURNS: N/A
*/

void ppc403IntWr 
    (
    PPC403_CHAN * pChan
    )
    {
    unsigned char dummy;
    unsigned char outchar;

    dummy = *pChan->spls;
    dummy &= _SPLS_RX_ERR;

    if (dummy != 0)				/* any error detected */
        {
        *pChan->spls = _SPLS_RX_ERR;		/* reset error status bits */

        return;
        }

    dummy = *pChan->sphs;

    if (dummy & (_SPHS_DIS_MASK | _SPHS_CS_MASK))
        {
        *pChan->sphs = _SPHS_DIS_MASK | _SPHS_CS_MASK; 
						/* reset DIS & CS bits */
        return;
        }

    /* output character or disable transmitter intr if no more characters*/

    if ((*pChan->getTxChar) (pChan->getTxArg, &outchar) != ERROR)
        *pChan->sptb = outchar;
    else
        *pChan->sptc &= ~ _SPTC_TIE_MASK & ~_SPTC_EIE_MASK & ~_SPTC_DME_TBR;
    }

/*****************************************************************************
*
* ppc403IntRd - handle a receiver interrupt
*
* This routine handles read interrupts from the serial commonication 
* controller.
*
* RETURNS: N/A
*/

void ppc403IntRd
    (
    PPC403_CHAN * pChan
    )
    {
    unsigned char dummy;
    unsigned char inchar;
    int status = OK;

    dummy = *pChan->spls;
    dummy &= _SPLS_RX_ERR;
    while (dummy != 0)				/* any error detected */
        {
	*pChan->spls = _SPLS_RX_ERR;		/* reset error status bits */
        status = ERROR;
	dummy = *pChan->spls & _SPLS_RX_ERR; 
        }

    inchar = *pChan->sprb;			/* read input char */

    *pChan->spls = _SPLS_RBR_MASK; 	/* reset Receive buffer ready bit */

    /*
     * NOTE: the receiver interrupt status bit EXIRS[SRI] has already been
     * cleared in the generic interrupt handler sysPpc403IntHandler() 
     * (ppc403Intr.c), so there's no need to do it here.
     */

    if (status == OK)
	{
        (*pChan->putRcvChar) (pChan->putRcvArg, inchar);
	}
    }

/**********************************************************************
*
* ppc403IntEx - handle error interrupts
*
* This routine handles miscellaneous interrupts on the seial communication
* controller.
*
* RETURNS: N/A
*/

void ppc403IntEx
    (
    PPC403_CHAN * pChan
    )
    {
    }

/*******************************************************************************
*
* ppc403Startup - transmitter startup routine
*
* Call interrupt level character output routine.
*
* RETURNS:
* Returns OK, always.
*/

static int ppc403Startup
    (
    SIO_CHAN *pSioChan		/* device to start up */
    )
    {
    PPC403_CHAN * pChan = (PPC403_CHAN *) pSioChan;
    int lock;

    /* enable the transmitter and it should interrupt to write the next char */
    
    if (((PPC403_CHAN *)pSioChan)->mode != SIO_MODE_POLL)
    	{
    	lock = intLock ();

	/* enable Transmit empty & Transmit error interrupts */

    	*pChan->sptc |= _SPTC_TIE_MASK|_SPTC_EIE_MASK;

    	vxExierEnable (_PPC403_EXI_STI);     /* enable Transmitter interrupt */

    	intUnlock (lock);
    	}
    return (OK);
    }

/******************************************************************************
*
* ppc403PRxChar - poll the device for input
*
* RETURNS:
* Returns OK if a character arrived, ERROR on device error, EAGAIN
* if the input buffer if empty.
*/

static int ppc403PRxChar
    (
    SIO_CHAN *pSioChan,
    char *ch
    )
    {
    PPC403_CHAN * pChan = (PPC403_CHAN *) pSioChan;

    /* wait for RBR full */

    if (((*pChan->spls) & _SPLS_RBR_MASK) != _SPLS_RBR_FULL) return (EAGAIN);

    *ch = *(pChan->sprb); 

    return(OK);
    }

/******************************************************************************
*
* ppc403PTxChar - output a character in polled mode
*
* RETURNS:
* Returns OK if a character arrived, ERROR on device error, EAGAIN
* if the output buffer if full.
*/

static int ppc403PTxChar
    (
    SIO_CHAN *pSioChan,
    char ch                   /* character to output */
    )
    {
    PPC403_CHAN * pChan = (PPC403_CHAN *) pSioChan;

    /* wait for TBR empty */

    if (((*pChan->spls) & _SPLS_TBR_MASK) != _SPLS_TBR_EMPTY) return (EAGAIN);

    *(pChan->sptb) = ch; 

    return(OK);
    }

