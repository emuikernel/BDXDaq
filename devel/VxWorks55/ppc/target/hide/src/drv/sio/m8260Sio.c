/* m8260Sio.c - Motorola MPC8260 SCC UART serial driver */

/* Copyright 1984-2002 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01o,24apr02,pmr  SPR 75161: returning int as required from m8260SioStartup().
01n,30nov01,rcs  allow pSccChan->pRBASE and pSccChan->pTBASE to be passed in
                 SPR# 34524
01m,22may01,rcs  changed m8260SioIoctl SIO_BAUD_SET to use M8260_BRGC_DIVISOR
01l,15may01,rcs  changes m8260SioIoctl SIO_BAUD_SET to correctly calculate 
                 baudrate, added include "drv/sio/m8260Brg.h SPR#66989
01k,03oct99,ms_  roll back to circa 01g to fix hang
01j,16sep99,ms_  some include files come from drv/sio
01i,16sep99,ms_  rename m8260Int.h to m8260IntrCtl.h
01h,15jul99,ms_  make compliant with our coding standards
		 use macros to access hardware registers
01g,21may99,ms_  set a number of "not used" fields as indicated by user's
                 manual
01f,19apr99,ms_  add documentation to functions with none
01e,17apr99,ms_  final EAR cleanup
01d,14apr99,ms_  interrupt numbers are defined, not vectors
01c,08apr99,ms_  upgrade to multiple channels
01b,06apr99,ms_  try using claudios buffer scheme
01a,08mar99,ms_	 adapted from src/drv/m8260Sio.c version 01b
*/

/*
DESCRIPTION
This is the driver for the SCCs in the internal Communications Processor (CP)
of the Motorola MPC8260. This driver only supports the SCCs in 
asynchronous UART mode.

USAGE
An M8260_SIO_CHAN structure is used to describe the chip.
The BSP's sysHwInit() routine typically calls sysSerialHwInit(),
which initializes all the values in the M8260_SCC_CHAN structure (except
the SIO_DRV_FUNCS) before calling m8260SioDevInit(). 

This driver requires that the BSP create and initialize the global variable
baudRateGenClk. This global variable must contain the value of the 
Baud Rate Generator Input Clock. 

The value should be calculated as:
 
                    VCO_OUT/SCCR[DFBRG].  

Typically this will be: 

		(CPM clock * 2) / 16

This driver requires the BSP Parameter M8260_BRGC_DIVISOR. This Parameter
must be initialized according to the system oscillator present on the board.
For the ads8260 BSP this is automatic. The Parameter will be initialzed to
M8260_BRGC_DIV1 for a 40 MHz system oscillator and M8260_BRGC_DIV16 for 33 MHz
and 66 MHz.

The BSP's sysHwInit2() routine typically calls sysSerialHwInit2() which
connects the chip's interrupts via intConnect().

INCLUDE FILES: drv/sio/m8260Sio.h drv/sio/m8260Cp.h drv/intrCtl/m8260IntrCtl.h
*/

/* includes */

#include "vxWorks.h"
#include "intLib.h"
#include "errno.h"
#include "sioLib.h"
#include "drv/sio/m8260Sio.h"
#include "drv/sio/m8260Cp.h"
#include "drv/sio/m8260Brg.h"
#include "drv/intrCtl/m8260IntrCtl.h"

/* defines */

#define DEFAULT_BAUD 9600

/* local forward declarations */

static STATUS m8260SioIoctl (M8260_SCC_CHAN *pSccChan,int request,int arg);
static int    m8260SioPollOutput (SIO_CHAN *pSioChan,char);
static int    m8260SioPollInput (SIO_CHAN *pSioChan,char *);
static int    m8260SioStartup (M8260_SCC_CHAN *pSccChan);
static int    m8260SioCallbackInstall (SIO_CHAN *pSioChan, int, STATUS (*)(), void *);

/* global forward declarations */

void   m8260SioResetChannel (M8260_SCC_CHAN *pSccChan);

/* local driver function table */

static SIO_DRV_FUNCS m8260SioDrvFuncs =
    {
    (int (*)())			m8260SioIoctl,
    (int (*)())			m8260SioStartup,
    (int (*)())			m8260SioCallbackInstall,
    (int (*)())			m8260SioPollInput,
    (int (*)(SIO_CHAN *,char))	m8260SioPollOutput
    };

/*******************************************************************************
*
* m8260SioDevInit - initialize the SCC
*
* This routine is called to initialize the chip to a quiescent state.
* Note that the `sccNum' field of M8260_SCC_CHAN must be less than 
* or equal to the
* maximum number of SCC channels to configure as SIOs, as defined in 
* ads8260.h
*/

void m8260SioDevInit
    (
    M8260_SCC_CHAN *pSccChan
    )
    {
    UINT32  immrVal = pSccChan->immrVal;    /* holder for the immr value */
    UINT8   sccNum = pSccChan->sccNum;      /* holder for the fcc number */
    UINT8   scc = sccNum - 1;            	/* a convenience */
    VINT16 *pSCCM = (VINT16 *) (immrVal + M8260_SCC_BASE + M8260_SCCM_OFFSET +
	                       (scc * M8260_SCC_OFFSET_NEXT_SCC));

    if (sccNum > N_SIO_CHANNELS)	
	return;

    /*
     * Disable receive and transmit interrupts.
     * Always flush cache pipe before first read
     */

    CACHE_PIPE_FLUSH ();
	
    *pSCCM &= ~(M8260_SCC_UART_SCCX_RX | M8260_SCC_UART_SCCX_TX);

    pSccChan->baudRate  = DEFAULT_BAUD;
    pSccChan->pDrvFuncs = &m8260SioDrvFuncs;
    }

/*******************************************************************************
*
* m8260SioResetChannel - initialize an SCC channel
*
* This routines initializes an SCC channel. Initialized are:
* 
* .CS
*        CPCR
*        BRGC
*        Baud rate
*        Buffer Descriptors
*        RBASE field in SCC Parameter RAM
*        TBASE field in SCC Parameter RAM
*        RFCR field in SCC Parameter RAM
*        TFCR field in SCC Parameter RAM
*        MRBLR field in SCC Parameter RAM
*        
*        SCC mode registers: GSMR_L, GSMR_H, AND PSMR
*        transmit and receive interrupts are enabled in SCCM 
*
* RETURNS: NA
*
* .CE
*/

void m8260SioResetChannel 
    (
    M8260_SCC_CHAN *pSccChan
    )
    {
    UINT32  immrVal = pSccChan->immrVal;    /* holder for the immr value */
    UINT8   sccNum = pSccChan->sccNum;      /* holder for the fcc number */
    UINT8   scc = sccNum - 1;            	/* a convenience */
    VINT32  cpcrVal = 0;         		/* a convenience */

    VINT32 *pBRGC = (VINT32 *) (immrVal + M8260_BRGC_BASE + 
			        (scc * M8260_BRGC_OFFSET_NEXT_BRGC));

    VINT32 *pGSMR_L = (VINT32 *) (immrVal + M8260_SCC_BASE + 
                                  M8260_GSMR_L_OFFSET +
				  (scc * M8260_SCC_OFFSET_NEXT_SCC));

    VINT32 *pGSMR_H = (VINT32 *) (immrVal + M8260_SCC_BASE + 
                                  M8260_GSMR_H_OFFSET +
				  (scc * M8260_SCC_OFFSET_NEXT_SCC));

    VINT16 *pPSMR = (VINT16 *) (immrVal + M8260_SCC_BASE + 
		      	        (scc * M8260_SCC_OFFSET_NEXT_SCC) +
			        M8260_PSMR_OFFSET);

    VINT16 *pSCCM = (VINT16 *) (immrVal + M8260_SCC_BASE + M8260_SCCM_OFFSET +
		      	        (scc * M8260_SCC_OFFSET_NEXT_SCC));

    VINT8 * pRFCR = (VINT8 *) (immrVal + M8260_SCC_PRAM_BASE +
                               M8260_SCC_PRAM_RFCR +
                               (scc * M8260_SCC_PRAM_OFFSET_NEXT_PRAM));

    VINT8 * pTFCR = (VINT8 *) (immrVal + M8260_SCC_PRAM_BASE +
                               M8260_SCC_PRAM_TFCR +
                               (scc * M8260_SCC_PRAM_OFFSET_NEXT_PRAM));

    VINT16 *pMRBLR = (VINT16 *) (immrVal + M8260_SCC_PRAM_BASE +
                                 M8260_SCC_PRAM_MRBLR +
                                 (scc * M8260_SCC_PRAM_OFFSET_NEXT_PRAM));

    VINT16 *pMAX_IDL = (VINT16 *) (immrVal + M8260_SCC_PRAM_BASE +
                                 0x38 +
                                 (scc * M8260_SCC_PRAM_OFFSET_NEXT_PRAM));

    VINT16 *pBRKCR = (VINT16 *) (immrVal + M8260_SCC_PRAM_BASE +
                                 0x3C +
                                 (scc * M8260_SCC_PRAM_OFFSET_NEXT_PRAM));

    VINT16 *pPAREC = (VINT16 *) (immrVal + M8260_SCC_PRAM_BASE +
                                 M8260_SCC_UART_PRAM_PAREC +
                                 (scc * M8260_SCC_PRAM_OFFSET_NEXT_PRAM));

    VINT16 *pFRMEC = (VINT16 *) (immrVal + M8260_SCC_PRAM_BASE +
                                 M8260_SCC_UART_PRAM_FRMEC +
                                 (scc * M8260_SCC_PRAM_OFFSET_NEXT_PRAM));

    VINT16 *pNOSEC = (VINT16 *) (immrVal + M8260_SCC_PRAM_BASE +
                                 M8260_SCC_UART_PRAM_NOSEC +
                                 (scc * M8260_SCC_PRAM_OFFSET_NEXT_PRAM));

    VINT16 *pBRKEC = (VINT16 *) (immrVal + M8260_SCC_PRAM_BASE +
                                 M8260_SCC_UART_PRAM_BRKEC +
                                 (scc * M8260_SCC_PRAM_OFFSET_NEXT_PRAM));

    VINT16 *pUADDR1 = (VINT16 *) (immrVal + M8260_SCC_PRAM_BASE +
                                 0x48 +
                                 (scc * M8260_SCC_PRAM_OFFSET_NEXT_PRAM));

    VINT16 *pUADDR2 = (VINT16 *) (immrVal + M8260_SCC_PRAM_BASE +
                                 0x4A +
                                 (scc * M8260_SCC_PRAM_OFFSET_NEXT_PRAM));

    VINT16 *pTOSEQ = (VINT16 *) (immrVal + M8260_SCC_PRAM_BASE +
                                 0x4E +
                                 (scc * M8260_SCC_PRAM_OFFSET_NEXT_PRAM));

    VINT16 *pCHARACTER1 = (VINT16 *) (immrVal + M8260_SCC_PRAM_BASE +
                                 0x50 +
                                 (scc * M8260_SCC_PRAM_OFFSET_NEXT_PRAM));

    VINT16 *pCHARACTER2 = (VINT16 *) (immrVal + M8260_SCC_PRAM_BASE +
                                 0x52 +
                                 (scc * M8260_SCC_PRAM_OFFSET_NEXT_PRAM));

    VINT16 *pCHARACTER3 = (VINT16 *) (immrVal + M8260_SCC_PRAM_BASE +
                                 0x54 +
                                 (scc * M8260_SCC_PRAM_OFFSET_NEXT_PRAM));

    VINT16 *pCHARACTER4 = (VINT16 *) (immrVal + M8260_SCC_PRAM_BASE +
                                 0x56 +
                                 (scc * M8260_SCC_PRAM_OFFSET_NEXT_PRAM));

    VINT16 *pCHARACTER5 = (VINT16 *) (immrVal + M8260_SCC_PRAM_BASE +
                                 0x58 +
                                 (scc * M8260_SCC_PRAM_OFFSET_NEXT_PRAM));

    VINT16 *pCHARACTER6 = (VINT16 *) (immrVal + M8260_SCC_PRAM_BASE +
                                 0x5A +
                                 (scc * M8260_SCC_PRAM_OFFSET_NEXT_PRAM));

    VINT16 *pCHARACTER7 = (VINT16 *) (immrVal + M8260_SCC_PRAM_BASE +
                                 0x5C +
                                 (scc * M8260_SCC_PRAM_OFFSET_NEXT_PRAM));

    VINT16 *pCHARACTER8 = (VINT16 *) (immrVal + M8260_SCC_PRAM_BASE +
                                 0x5E +
                                 (scc * M8260_SCC_PRAM_OFFSET_NEXT_PRAM));

    VINT16 *pRCCM = (VINT16 *) (immrVal + M8260_SCC_PRAM_BASE +
                                 0x60 +
                                 (scc * M8260_SCC_PRAM_OFFSET_NEXT_PRAM));
    int ix = 0;
    
    int oldlevel = intLock ();	/* lock interrupts */ 

    if (pSccChan->pRBASE == NULL)
        {
        pSccChan->pRBASE = (VINT16 *) (immrVal + M8260_SCC_PRAM_BASE +
                                       M8260_SCC_PRAM_RBASE +
                                       (scc * M8260_SCC_PRAM_OFFSET_NEXT_PRAM));
        } 
    if (pSccChan->pTBASE == NULL)
        {
        pSccChan->pTBASE = (VINT16 *) (immrVal + M8260_SCC_PRAM_BASE +
                                       M8260_SCC_PRAM_TBASE +
                                       (scc * M8260_SCC_PRAM_OFFSET_NEXT_PRAM));
        }

    CACHE_PIPE_FLUSH ();

    /* wait until the CP is clear */

    do
        {
	M8260_SCC_32_RD((M8260_CPCR (immrVal)), cpcrVal);

        if (ix++ == M8260_CPCR_LATENCY)
            break;
        } while (cpcrVal & M8260_CPCR_FLG) 
	    ;

    if (ix >= M8260_CPCR_LATENCY)
        {

        /* what to do, other than log an error? */

        }

    /* Stop transmitting on SCC, if doing so */

    cpcrVal = (M8260_CPCR_OP (M8260_CPCR_TX_STOP)
               | M8260_CPCR_SBC (M8260_CPCR_SBC_SCC1 | (scc * 0x1))
               | M8260_CPCR_PAGE (M8260_CPCR_PAGE_SCC1 | (scc * 0x1))
               | M8260_CPCR_FLG);

    M8260_SCC_32_WR (M8260_CPCR (immrVal), cpcrVal);

    /* flush cache pipe when switching from write to read */

    CACHE_PIPE_FLUSH ();

   /* wait until the CP is clear */

    ix = 0;
    do
        {
        M8260_SCC_32_RD((M8260_CPCR (immrVal)), cpcrVal);

        if (ix++ == M8260_CPCR_LATENCY)
            break;
        } while (cpcrVal & M8260_CPCR_FLG) ;

    if (ix >= M8260_CPCR_LATENCY)
        {

        /* what to do, other than log an error? */

        }

    /* set up SCC as NMSI, select Baud Rate Generator */

    /* reset baud rate generator, wait for reset to clear... */
 
    * pBRGC |= M8260_BRGC_RST;

    /* flush cache pipe when switching from write to read */

    CACHE_PIPE_FLUSH ();

    while ((*pBRGC) & M8260_BRGC_RST);

    m8260SioIoctl (pSccChan, SIO_BAUD_SET, pSccChan->baudRate);

    /* set up the RECEIVE buffer descriptors */

    /* buffer status/control */
    M8260_SCC_16_WR((pSccChan->pBdBase + 
                     M8260_SCC_RCV_BD_OFF + 
                     M8260_SCC_BD_STAT_OFF), 0xB000);
    CACHE_PIPE_FLUSH ();

    /* buffer length */
    M8260_SCC_16_WR((pSccChan->pBdBase + 
                     M8260_SCC_RCV_BD_OFF + 
                     M8260_SCC_BD_LEN_OFF), 0x0001);
    CACHE_PIPE_FLUSH ();

    /* buffer address */

    M8260_SCC_32_WR((pSccChan->pBdBase + 
                     M8260_SCC_RCV_BD_OFF + 
                     M8260_SCC_BD_ADDR_OFF), pSccChan->rcvBufferAddr);

    /* set the SCC parameter ram field RBASE */

    * pSccChan->pRBASE = 0x00 + (0x100 * scc);

    /* set up the TRANSMIT buffer descriptors */

    /* 
     * buffer status/control;
     * not ready, Wrap, Bit Clear-to-send_report, Interrupt 
     */

    M8260_SCC_16_WR((pSccChan->pBdBase + 
                     M8260_SCC_TX_BD_OFF + 
                     M8260_SCC_BD_STAT_OFF), 0x3800);

    /* buffer length */

    M8260_SCC_16_WR((pSccChan->pBdBase + 
                     M8260_SCC_TX_BD_OFF + 
                     M8260_SCC_BD_LEN_OFF), 0x0001);

    /* buffer address */

    M8260_SCC_32_WR((pSccChan->pBdBase + 
                     M8260_SCC_TX_BD_OFF + 
                     M8260_SCC_BD_ADDR_OFF), pSccChan->txBufferAddr);

    /* set the SCC parameter ram field TBASE */

    * pSccChan->pTBASE = 0x08 + (0x100 * scc);

    /* disable transmit and receive interrupts */

    * pSCCM &= ~(M8260_SCC_UART_SCCX_RX | M8260_SCC_UART_SCCX_TX);

    /* program the three SCC mode registers: gsmrl, gsmrh, and psmr */

    /* 
     * Protocol Specific Mode Register for SCC
     * Normal CTS, 1 stop bit, 8 data bits 
     */

    * pPSMR   = M8260_SCC_UART_PSMR_FLC  | M8260_SCC_UART_PSMR_CL_8BIT;


    /* 
     * General Serial Mode Register for SCC, high word
     * xmit fifo is 1 byte, receive fifo 8 bits 
     */

    * pGSMR_H = M8260_SCC_GSMRH_RFW      | M8260_SCC_GSMRH_TFL;

    /* 
     * General Serial Mode Register for SCC, low word
     * set SCC attributes to standard UART mode, disable xmit and rcv
     */

    * pGSMR_L = (M8260_SCC_GSMRL_RDCR_X16 | 
		 M8260_SCC_GSMRL_TDCR_X16 | 
		 M8260_SCC_GSMRL_UART) 
		 & ~
		 (M8260_SCC_GSMRL_ENT | M8260_SCC_GSMRL_ENR);

    /* initialize parameter RAM area for this SCC */

    * pRFCR = 0x18;	/* supervisor data access */

    * pTFCR = 0x18;	/* supervisor data access */

    * pMRBLR = 1;

    /* initialize some unused parameters in SCC parameter RAM */

    * pMAX_IDL = 0x0;
    * pBRKCR = 0x0001;
    * pPAREC = 0x0;
    * pFRMEC = 0x0;
    * pNOSEC = 0x0;
    * pBRKEC = 0x0;
    * pUADDR1 = 0x0;
    * pUADDR2 = 0x0;
    * pTOSEQ = 0x0;
    * pCHARACTER1 = 0x8000;
    * pCHARACTER2 = 0x8000;
    * pCHARACTER3 = 0x8000;
    * pCHARACTER4 = 0x8000;
    * pCHARACTER5 = 0x8000;
    * pCHARACTER6 = 0x8000;
    * pCHARACTER7 = 0x8000;
    * pCHARACTER8 = 0x8000;
    * pRCCM = 0x0C0FF;

    CACHE_PIPE_FLUSH ();

   /* wait until the CP is clear */
    ix = 0;
    do
        {
        M8260_SCC_32_RD((M8260_CPCR (immrVal)), cpcrVal);

        if (ix++ == M8260_CPCR_LATENCY)
            break;
        } while (cpcrVal & M8260_CPCR_FLG) ;

    if (ix >= M8260_CPCR_LATENCY)
        {

        /* what to do, other than log an error? */

        }

    /* Tell CP to initialize tx and rx parameters for SCC */

    cpcrVal = (M8260_CPCR_OP (M8260_CPCR_RT_INIT)
               | M8260_CPCR_SBC (M8260_CPCR_SBC_SCC1 | (scc * 0x1))
               | M8260_CPCR_PAGE (M8260_CPCR_PAGE_SCC1 | (scc * 0x1))
               | M8260_CPCR_MCN (M8260_CPCR_MCN_ETH)
               | M8260_CPCR_FLG);

    M8260_SCC_32_WR (M8260_CPCR (immrVal), cpcrVal);

    CACHE_PIPE_FLUSH ();

    /* wait until the CP is clear */

    ix = 0;
    do
        {
        M8260_SCC_32_RD((M8260_CPCR (immrVal)), cpcrVal);

        if (ix++ == M8260_CPCR_LATENCY)
            break;
        } while (cpcrVal & M8260_CPCR_FLG) ;

    if (ix >= M8260_CPCR_LATENCY)
        {

        /* what to do, other than log an error? */

        }

    CACHE_PIPE_FLUSH ();

    /* lastly, enable the transmitter and receiver  */

    * pGSMR_L |= (M8260_SCC_GSMRL_ENT | M8260_SCC_GSMRL_ENR);

    CACHE_PIPE_FLUSH ();

    intUnlock (oldlevel);			/* UNLOCK INTERRUPTS */
    }

/*******************************************************************************
*
* m8260SioIoctl - special device control
*
* Allows the caller to get and set the buad rate; to get and set the mode;
* and to get the allowable modes.
*
* RETURNS: OK on success, EIO on device error, ENOSYS on unsupported
*          request.
*
*/

LOCAL STATUS m8260SioIoctl
    (
    M8260_SCC_CHAN *	pSccChan,	/* device to control */
    int			request,	/* request code */
    int			arg		/* some argument */
    )
    {
    int 	oldlevel;
    STATUS 	status = OK;
    UINT8       sccNum = pSccChan->sccNum;      /* holder for the fcc number */
    UINT8       scc = sccNum - 1;            	/* a convenience */
    UINT32      immrVal = pSccChan->immrVal;    /* holder for the immr value */
    int         baudRate;
    int         baud;

    VINT32 *pBRGC = (VINT32 *) (immrVal + M8260_BRGC_BASE + 
			        (scc * M8260_BRGC_OFFSET_NEXT_BRGC));


    VINT16 *pSCCM = (VINT16 *) (immrVal + M8260_SCC_BASE + M8260_SCCM_OFFSET +
		      	        (scc * M8260_SCC_OFFSET_NEXT_SCC));

    switch (request)
	{
	case SIO_BAUD_SET:
	    if (arg >=  300 && arg <= 38400)     /* could go higher... */
                {
                baudRate = (baudRateGenClk + (8 * arg)) / (16 * arg);

                if (--baudRate > 0xfff)
                    baud = (M8260_BRGC_CD_MASK &
                   (((baudRate + 8) / 16) << M8260_BRGC_CD_SHIFT)) |\
                    M8260_BRGC_EN | M8260_BRGC_DIVISOR ;
                else
                    baud = ( M8260_BRGC_CD_MASK &
                        (baudRate << 1)) | M8260_BRGC_EN;

                *pBRGC = baud;

                pSccChan->baudRate = arg;
                }
            else
	        status = EIO;
	    break;
    
	case SIO_BAUD_GET:
	    * (int *) arg = pSccChan->baudRate;
	    break;

	case SIO_MODE_SET:
            if (!((int) arg == SIO_MODE_POLL || (int) arg == SIO_MODE_INT))
                {
                status = EIO;
                break;
                }

            /* lock interrupt  */

            oldlevel = intLock();

            /* initialize channel on first MODE_SET */

            if (!pSccChan->channelMode)
                m8260SioResetChannel(pSccChan);

            if (arg == SIO_MODE_INT)
		{

		/* enable SCC interrupts at the SIU Interrupt Controller */

		if (scc == 0) 
		    {
                    m8260IntEnable(INUM_SCC1);
		    }
		else if (scc == 1) 
		    {
                    m8260IntEnable(INUM_SCC2);
		    }

		/* enable receive and transmit interrupts at the scc */

		* pSCCM |= (M8260_SCC_UART_SCCX_RX | M8260_SCC_UART_SCCX_TX);

		CACHE_PIPE_FLUSH ();

		}
            else
		{

		/* disable transmit and receive interrupts */

		* pSCCM &= ~(M8260_SCC_UART_SCCX_RX | M8260_SCC_UART_SCCX_TX);

		CACHE_PIPE_FLUSH ();

		/* mask off this SCC's interrupt */ 

		if (scc == 0) 
                    m8260IntDisable(INUM_SCC1);
		else if (scc == 1) 
                    m8260IntDisable(INUM_SCC2);
		CACHE_PIPE_FLUSH ();

                }

            pSccChan->channelMode = arg;

            intUnlock(oldlevel);

            break;

        case SIO_MODE_GET:
            * (int *) arg = pSccChan->channelMode;
	    break;

        case SIO_AVAIL_MODES_GET:
            *(int *)arg = SIO_MODE_INT | SIO_MODE_POLL;
	    break;

	default:
	    status = ENOSYS;
	}

    return (status);
    }

/*******************************************************************************
*
* m8260SioInt - handle an SCC interrupt
*
* This routine is called to handle SCC interrupts.
*
* RETURNS: NA
*/

void m8260SioInt
    (
    M8260_SCC_CHAN *pSccChan
    )
    {
    char		outChar;
    VINT16              bdStatus;      /* holder for the BD status */
    UINT8       sccNum = pSccChan->sccNum;      /* holder for the fcc number */
    UINT8       scc = sccNum - 1;            	/* a convenience */
    UINT32      immrVal = pSccChan->immrVal;    /* holder for the immr value */

    VINT16 *pSCCE = (VINT16 *) (immrVal + M8260_SCC_BASE + M8260_SCCE_OFFSET +
		      	        (scc * M8260_SCC_OFFSET_NEXT_SCC));

    CACHE_PIPE_FLUSH ();

    /* check for a receive event */

    if (* pSCCE & M8260_SCC_UART_SCCX_RX )
	{

	/*
	 * clear receive event bit by setting the bit in the event register.
	 * This also clears the bit in SIPNR
	 */

        * pSCCE = M8260_SCC_UART_SCCX_RX; 
	CACHE_PIPE_FLUSH ();

	/* 
	 * as long as there is a character: 
	 * Inspect bit 0 of the status word which is the empty flag:
	 * 0 if buffer is full or if there was an error
	 * 1 if buffer is not full
	 */

	M8260_SCC_16_RD((pSccChan->pBdBase +
			 M8260_SCC_RCV_BD_OFF +
			 M8260_SCC_BD_STAT_OFF), bdStatus);

        /* if bit is set there is nothing */

	while (!(bdStatus & M8260_SCC_UART_RX_BD_EMPTY)) 
	    {
	    M8260_SCC_8_RD(pSccChan->rcvBufferAddr, outChar);

            /*
             * indicate that we've processed this buffer; set empty
             * flag to 1 indicating that the buffer is empty
             */

	    M8260_SCC_16_WR((pSccChan->pBdBase +
                     M8260_SCC_RCV_BD_OFF +
                     M8260_SCC_BD_STAT_OFF), 
                     bdStatus |= M8260_SCC_UART_RX_BD_EMPTY);

            /* necessary when switching from write to read */

	    CACHE_PIPE_FLUSH ();

            /* pass the received character up to the tty driver */

	    (*pSccChan->putRcvChar) (pSccChan->putRcvArg,outChar);

            /* if driver is in polled mode, we're done */

	    if (pSccChan->channelMode == SIO_MODE_POLL)
		break;

	    /*
             * If interrupt mode, read the status again;
             * it's possible a new char has arrived
             */

	    M8260_SCC_16_RD((pSccChan->pBdBase +
			     M8260_SCC_RCV_BD_OFF +
			     M8260_SCC_BD_STAT_OFF), bdStatus);
	    }
	}

    /* check for a transmit event and if a character needs to be output */

    /* transmit event */

    CACHE_PIPE_FLUSH ();

    if ((* pSCCE & M8260_SCC_UART_SCCX_TX ) && 
        (pSccChan->channelMode != SIO_MODE_POLL)) /* ...and we're not polling */
	{

	/*
         * clear transmit event bit by setting the bit in the event register.
         * This also clears the bit in SIPNR
         */

        * pSCCE = M8260_SCC_UART_SCCX_TX; 

	CACHE_PIPE_FLUSH ();
    
        if ((*pSccChan->getTxChar) (pSccChan->getTxArg, &outChar) == OK)
            {
	    CACHE_PIPE_FLUSH ();

	    /* wait for the ready bit to be 0 meaning the buffer is free */

	    do
		{
		M8260_SCC_16_RD((pSccChan->pBdBase +
				 M8260_SCC_TX_BD_OFF +
				 M8260_SCC_BD_STAT_OFF), bdStatus);
		} while (bdStatus & M8260_SCC_UART_TX_BD_READY);

	    M8260_SCC_8_WR(pSccChan->txBufferAddr, outChar); /* write char */

	    /* set buffer length */

	    M8260_SCC_16_WR((pSccChan->pBdBase +
			     M8260_SCC_TX_BD_OFF +
			     M8260_SCC_BD_LEN_OFF), 0x0001);

	                /* set ready bit so CPM will process buffer */

	    M8260_SCC_16_WR((pSccChan->pBdBase +
			     M8260_SCC_TX_BD_OFF +
			     M8260_SCC_BD_STAT_OFF), 
                             bdStatus |= M8260_SCC_UART_TX_BD_READY);

	    CACHE_PIPE_FLUSH ();
	    }
        }

    /*
     * acknowledge all other interrupts - ignore events. This also clears
     * the bit in SIPNR
     */

    * pSCCE &= ~(M8260_SCC_UART_SCCX_TX | M8260_SCC_UART_SCCX_RX);

    CACHE_PIPE_FLUSH ();

   }

/*******************************************************************************
*
* m8260SioStartup - transmitter startup routine
* 
* Starts transmission on the indicated channel
*
* RETURNS: OK, or ENOSYS if in polled mode.
*/
LOCAL int m8260SioStartup
    (
    M8260_SCC_CHAN *pSccChan		/* ty device to start up */
    )
    {
    char outChar;
    UINT16              bdStatus;      /* holder for the BD status */

    if (pSccChan->channelMode == SIO_MODE_POLL)
	return (ENOSYS);

    /* 
     * check if a transmit buffer is ready and if a character needs to be 
     * output 
     */

    CACHE_PIPE_FLUSH ();                /* before first read */

    M8260_SCC_16_RD((pSccChan->pBdBase +
		     M8260_SCC_TX_BD_OFF +
		     M8260_SCC_BD_STAT_OFF), bdStatus);

    if (!(bdStatus & M8260_SCC_UART_TX_BD_READY))
	if ((*pSccChan->getTxChar) (pSccChan->getTxArg, &outChar) == OK)
	    {

            /* write char; set length; flag buffer as not empty */

	    M8260_SCC_8_WR(pSccChan->txBufferAddr, outChar); /* write char */

	    /* set buffer length */
	    M8260_SCC_16_WR((pSccChan->pBdBase +
			     M8260_SCC_TX_BD_OFF +
			     M8260_SCC_BD_LEN_OFF), 0x0001);

	    /* flag buffer as not empty */

	    M8260_SCC_16_WR((pSccChan->pBdBase +
			     M8260_SCC_TX_BD_OFF +
			     M8260_SCC_BD_STAT_OFF), bdStatus |= 0x8000);

	    }
    CACHE_PIPE_FLUSH ();

    return (OK);
    }

/******************************************************************************
*
* m8260SioPollInput - poll the device for input.
*
* Poll the indicated device for input characters
*
* RETURNS: OK if a character arrived, ERROR on device error, EAGAIN
*          if the input buffer is empty.
*/

LOCAL int m8260SioPollInput
    (
    SIO_CHAN *	pSioChan,
    char *	thisChar
    )
    {
    M8260_SCC_CHAN * pSccChan = (M8260_SCC_CHAN *) pSioChan;
    UINT16              bdStatus;      /* holder for the BD status */
    UINT8       sccNum = pSccChan->sccNum;      /* holder for the fcc number */
    UINT8       scc = sccNum - 1;            	/* a convenience */
    UINT32      immrVal = pSccChan->immrVal;    /* holder for the immr value */

    VINT16 *pSCCE = (VINT16 *) (immrVal + M8260_SCC_BASE + M8260_SCCE_OFFSET +
		      	        (scc * M8260_SCC_OFFSET_NEXT_SCC));

    CACHE_PIPE_FLUSH ();

    /* is there a receive event? */

    if (!(* pSCCE & M8260_SCC_UART_SCCX_RX ))
        return (EAGAIN); 

    /* is the buffer empty? */

    M8260_SCC_16_RD((pSccChan->pBdBase +
		     M8260_SCC_RCV_BD_OFF +
		     M8260_SCC_BD_STAT_OFF), bdStatus);

    /* if bit is high there is nothing */

    if (bdStatus & M8260_SCC_UART_RX_BD_EMPTY) 
	return (EAGAIN);

    /* get a character */

    M8260_SCC_8_RD(pSccChan->rcvBufferAddr, * thisChar);

    /* set the empty bit */

    M8260_SCC_16_WR((pSccChan->pBdBase +
	     M8260_SCC_RCV_BD_OFF +
	     M8260_SCC_BD_STAT_OFF), bdStatus |= M8260_SCC_UART_RX_BD_EMPTY);

    CACHE_PIPE_FLUSH ();

    /* only clear RX event if no more characters are ready */

    M8260_SCC_16_RD((pSccChan->pBdBase +
		     M8260_SCC_RCV_BD_OFF +
		     M8260_SCC_BD_STAT_OFF), bdStatus);
    CACHE_PIPE_FLUSH ();

    /* if bit is high there is nothing */

    if (bdStatus & M8260_SCC_UART_RX_BD_EMPTY) 
	*pSCCE = M8260_SCC_UART_SCCX_RX; /* clear rx event bit */

    CACHE_PIPE_FLUSH ();

    return (OK);
    }

/******************************************************************************
*
* m8260SioPollOutput - output a character in polled mode.
*
* Transmit a character in polled mode
*
* RETURNS: OK if a character is sent, ERROR on device error, EAGAIN
*          if the output buffer if full.
*/

static int m8260SioPollOutput
    (
    SIO_CHAN *	pSioChan,
    char	outChar
    )
    {
    M8260_SCC_CHAN *	pSccChan = (M8260_SCC_CHAN *) pSioChan;
    int			i;
    UINT16              bdStatus;      /* holder for the BD status */
    UINT8       sccNum = pSccChan->sccNum;      /* holder for the fcc number */
    UINT8       scc = sccNum - 1;            	/* a convenience */
    UINT32      immrVal = pSccChan->immrVal;    /* holder for the immr value */

    VINT16 *pSCCE = (VINT16 *) (immrVal + M8260_SCC_BASE + M8260_SCCE_OFFSET +
		      	        (scc * M8260_SCC_OFFSET_NEXT_SCC));

    CACHE_PIPE_FLUSH ();

    /*
     * wait a bit for the last character to get out
     * because the PPC603 is a very fast processor
     */

    for (i=0; i<M8260_SCC_POLL_OUT_DELAY; i++)
        ;
	
    /*
     * is the transmitter ready yet to accept a character?
     * if still not, we have a problem
     */

    M8260_SCC_16_RD((pSccChan->pBdBase +
		     M8260_SCC_TX_BD_OFF +
		     M8260_SCC_BD_STAT_OFF), bdStatus);

    if (!(bdStatus & 0x8000))
	return(EAGAIN);

    /* reset the transmitter status bit */

    /*
     * clear transmit event bit by setting the bit in the event register.
     * This also clears the bit in SIPNR
     */

    * pSCCE = M8260_SCC_UART_SCCX_TX; 

    /* write char; set length; flag buffer as not empty */

    M8260_SCC_8_WR(pSccChan->txBufferAddr, outChar); /* write char */

    /* set buffer length */

    M8260_SCC_16_WR((pSccChan->pBdBase +
		     M8260_SCC_TX_BD_OFF +
		     M8260_SCC_BD_LEN_OFF), 0x0001);

    /* flag buffer as not empty */

    M8260_SCC_16_WR((pSccChan->pBdBase +
		     M8260_SCC_TX_BD_OFF +
		     M8260_SCC_BD_STAT_OFF), 
                     bdStatus |= M8260_SCC_UART_TX_BD_READY);
    CACHE_PIPE_FLUSH ();

    return (OK);
    }

/******************************************************************************
*
* m8260SioCallbackInstall - install ISR callbacks to get and put chars.
*
* Install the indicated ISR callback functions that are used to get and
* put characters
*
* RETURNS: NA
*
*/

static int m8260SioCallbackInstall
    (
    SIO_CHAN *	pSioChan,
    int		callbackType,
    STATUS	(* callback)(),
    void *	callbackArg
    )
    {
    M8260_SCC_CHAN * pSccChan = (M8260_SCC_CHAN *) pSioChan;
    CACHE_PIPE_FLUSH ();

    switch (callbackType)
        {
        case SIO_CALLBACK_GET_TX_CHAR:
            pSccChan->getTxChar    = callback;
            pSccChan->getTxArg     = callbackArg;
            return (OK);
	    break;

        case SIO_CALLBACK_PUT_RCV_CHAR:
            pSccChan->putRcvChar   = callback;
            pSccChan->putRcvArg    = callbackArg;
            return (OK);
	    break;

        default:
            return (ENOSYS);
        }
    }
