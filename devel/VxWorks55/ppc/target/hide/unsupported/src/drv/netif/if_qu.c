/* if_qu.c - Motorola MC68EN360 QUICC network interface driver */

/* Copyright 1994-1997 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01k,31aug98,fle  doc : replaced simple quotes before 040 with 68 name to avoid
                 bolded text problems
01j,18jul97,spm  removed incorrect attachment test from driver initialization
01i,15jul97,spm  removed driver initialization from ioctl support (SPR #8831)
01h,14jul97,vin  fixed SPR# 8919.
01g,15may97,map  fixed intr event ack (scce) handling (SPR# 8580).
01f,03jan97,vin  modified for BSD44.
01f,15may97,map  fixed intr event register ack [SPR# 8580]
01e,08jul94,dzb  Added promisc code into quIoctl() code.
01d,30jun94,dzb  removed relevant quIoctl() code (SPR #3437).
01c,21apr94,dzb  fixed quReset() so doesn't reset all IFF flags (SPR #3239).
01b,12nov93,dzb  fixed event race condition in quIntr() (SPR #2632).
01a,18sep93,dzb  written.
*/

/*
This module implements the Motorola MC68EN360 QUICC Ethernet network interface
driver.

This driver is designed to support the Ethernet mode of a SCC residing on the
MC68EN360 processor chip.  It is generic in the sense that it does not care
which SCC is being used, and it supports up to four individual units per board.
To achieve this goal, the driver must be given several target-specific
parameters, and some external support routines must be provided.  These
parameters, and the mechanisms used to communicate them to the driver, are
detailed below.

This network interface driver does not include support for trailer protocols
or data chaining.  However, buffer loaning has been implemented in an effort to
boost performance. Support for four individual device units was designed
into this driver.

This driver maintains cache coherency by allocating buffer
space using the cacheDmaMalloc() routine.  This is provided for boards that
use the MC68EN360 in 68040 companion mode where it is attached to processors
with data cache space.

Due to a lack of suitable hardware, the multiple unit support and 68040
companion mode support have not been tested.

BOARD LAYOUT
This device is on-chip.  No jumpering diagram is necessary.

EXTERNAL INTERFACE
This driver provides the standard external interface; the device unit is 
attached by the quattach() routine, which uses quInit() to initialize the 
driver.

The only user-callable routine is quattach(), which publishes the `qu'
interface and initializes the driver structures.

TARGET-SPECIFIC PARAMETERS
.iP "base address of MC68EN360 internal memory"
This parameter is passed to the driver by quattach().

This parameter indicates the address at which the MC68EN360 presents
its internal memory (also known as the dual ported RAM base address).
With this address, and the SCC number (see below), the driver is able to
compute the location of the SCC parameter RAM and the SCC register map.
This parameter should point to the internal memory of the QUICC chip
where the SCC physically resides, which may not necessarily be the master
QUICC on the target board.

.iP "interrupt vector"
This parameter is passed to the driver by quattach().

This driver configures the MC68EN360 device to generate hardware interrupts
for various events within the device.  Therefore, this driver contains
an interrupt handler routine.  This driver will call the VxWorks system
function intConnect() to connect its interrupt handler to the interrupt
vector generated as a result of the MC68EN360 interrupt.

.iP "SCC number used"
This parameter is passed to the driver by quattach().

This driver is written to support four individual device units.
At the time that this driver was written, Motorola had released
information stating that any of the SCCs on the MC68EN360 may be used
in Ethernet mode.  Thus, the multiple units supported by this driver
may reside on different MC68EN360 chips, or may be on different SCCs
within a single MC68EN360.  This parameter is used to explicitly state
which SCC is being used (SCC1 is most commonly used, thus this parameter
most often equals "1").

.iP "number of transmit and receive buffer descriptors"
These parameters are passed to the driver by quattach().

The number of transmit and receive buffer descriptors (BDs) used is
configurable by the user upon attaching the driver.  Each buffer
descriptor resides in 8 bytes of the MC68EN360's dual ported RAM space,
and each one points to a 1520 byte buffer in regular RAM.  There must
be a minimum of two transmit and two receive BDs, and there is no
maximum, although over a certain amount will not speed up the driver and
will waste valuable dual ported RAM space.  If this parameter is "NULL"
a default value of "32" BDs will be used.

.iP "offset of transmit and receive buffer descriptors"
These parameters are passed to the driver by quattach().

These parameters indicate the base location of the transmit and receive
buffer descriptors (BDs).  They are offsets in bytes from the base address
of MC68EN360 internal memory (see above).  Each BD takes up 8 bytes of
dual ported RAM, and it is the user's responsibility to ensure that all
specified BDs will fit within dual ported RAM.  This includes any other
BDs the target board may be using, including other SCCs, SMCs, and the
SPI device.  There is no default for these parameters; they must be
provided by the user.

.iP "base address of buffer pool"
This parameter is passed to the driver by quattach().

This parameter is used to notify the driver that space for the transmit
and receive buffers need not be allocated, but should be taken from a
cache-coherent private memory space provided by the user at the given
address.  The user should be aware that memory used for buffers must be
4-byte aligned and non-cacheable.  All the buffers
*must* fit in the given memory space; no checking will be performed.
This includes all transmit and receive buffers (see above) and an
additional 16 receive loaner buffers, unless the number of receive
BDs is less than 16, in which case that number of loaner buffers will be
used.  Each buffer is 1520 bytes.  If this parameter is "NONE", space
for buffers will be obtained by calling cacheDmaMalloc() in quattach().
.LP

EXTERNAL SUPPORT REQUIREMENTS
This driver requires three external support functions:
.iP "STATUS sys360EnetEnable (int unit, UINT32 regBase)" "" 9 -1
This routine is expected to perform any target specific functions required
to enable the Ethernet controller.  These functions typically include
enabling the Transmit Enable signal (TENA) and connecting the transmit
and receive clocks to the SCC.
The driver calls this routine, once per unit, from the quInit() routine.
.iP "void sys360EnetDisable (int unit, UINT32 regBase)"
This routine is expected to perform any target specific functions required
to disable the Ethernet controller.  This usually involves disabling the
Transmit Enable (TENA) signal.
The driver calls this routine from the quReset() routine each time a unit
is disabled.
.iP "STATUS sys360EnetAddrGet (int unit, u_char * addr)"
The driver expects this routine to provide the six byte Ethernet
hardware address that will be used by this unit.  This routine must copy
the six byte address to the space provided by <addr>.  This routine is
expected to return OK on success, or ERROR.
The driver calls this routine, once per unit, from the quInit() routine.
.LP

SYSTEM RESOURCE USAGE
When implemented, this driver requires the following system resources:

    - one mutual exclusion semaphore
    - one interrupt vector
    - 0 bytes in the initialized data section (data)
    - 1272 bytes in the uninitialized data section (BSS)
 
The data and BSS sections are quoted for the CPU32 architecture and may vary
for other architectures.  The code size (text) will varies greatly between
architectures, and is therefore not quoted here.

If the driver allocates the memory to share with the Ethernet device unit,
it does so by calling the cacheDmaMalloc() routine.  For the default case
of 32 transmit buffers, 32 receive buffers, and 16 loaner buffers, the total
size requested is 121,600 bytes.  If a non-cacheable memory region is provided
by the user, the size of this region should be this amount, unless the
user has specified a different number of transmit or receive BDs.

This driver can only operate if this memory region is non-cacheable,
or if the hardware implements bus snooping.  The driver cannot maintain
cache coherency for the device because the buffers are asynchronously
modified by both the driver and the device, and these fields may share the
same cache line.  Additionally, the MC68EN360 dual ported RAM must be
declared as non-cacheable memory where applicable (e.g., when attached
to a 68040 processor).

SEE ALSO: ifLib,
.I "Motorola MC68360 User's Manual"

INTERNAL
Future optimization: Transmit directly from mbufs/clusters instead of calling
                     copy_from_mbufs(). Turn on transmit interrupts to know
		     when to call free routine.
*/

/* includes */

#include "vxWorks.h"
#include "iv.h"
#include "taskLib.h"
#include "memLib.h"
#include "ioctl.h"
#include "etherLib.h"
#include "net/mbuf.h"
#include "net/protosw.h"
#include "socket.h"
#include "errno.h"
#include "net/if.h"
#include "net/unixLib.h"
#include "net/route.h"
#include "netinet/in.h"
#include "netinet/in_systm.h"
#include "netinet/in_var.h"
#include "netinet/ip.h"
#include "netinet/if_ether.h"
#include "net/if_subr.h"
#include "cacheLib.h"
#include "stdio.h"
#include "intLib.h"
#include "logLib.h"
#include "netLib.h"
#include "drv/netif/netifDev.h"
#include "drv/multi/m68360.h"

/* defines */

#define MAX_UNITS	4	/* max qu units */
#define ENET_ADDR_SIZE	0x6	/* size of Ethernet src/dest addresses */
#define TX_BD_MIN	2	/* minimum number of Tx buffer descriptors */
#define RX_BD_MIN	2	/* minimum number of Rx buffer descriptors */
#define TX_BD_DEFAULT	0x20	/* default number of Tx buffer descriptors */
#define RX_BD_DEFAULT	0x20	/* default number of Rx buffer descriptors */
#define FRAME_MAX	0x05ee	/* maximum frame size */
#define FRAME_MAX_AL	0x05f0	/* maximum frame size, 4 byte alligned */
#define FRAME_MIN	0x0040	/* minimum frame size */
#define L_POOL		0x10	/* number of Rx loaner buffers in pool */

/* typedefs */

typedef struct mbuf MBUF;
typedef struct arpcom IDR;	/* Interface Data Record */
typedef struct ifnet IFNET;
typedef struct sockaddr SOCK;
typedef struct ether_header ETH_HDR; 		

typedef struct drv_ctrl	/* DRV_CTRL */
    {
    IDR		idr;		/* Interface Data Record */
    BOOL	attached;	/* indicates unit is attached */
    BOOL	initialized;	/* indicates unit is initialized */
    BOOL	txStop;		/* emergency stop output */
    int		nLoanRxBd;	/* number of receive buffers left to loan */
    u_char *	lPool[L_POOL];	/* receive BD loaner pool */
    UINT8 *	pRefCnt[L_POOL];/* stack of reference count pointers */
    UINT8	refCnt[L_POOL]; /* actual reference count values */
    UINT32	regBase;	/* register/DPR base address */
    SCC_DEV	ether;		/* ETHERNET SCC device */
    } DRV_CTRL;

/* globals */

IMPORT STATUS sys360EnetEnable  (int unit, UINT32 regBase); /* enable ctrl */
IMPORT void   sys360EnetDisable (int unit, UINT32 regBase); /* disable ctrl */
IMPORT STATUS sys360EnetAddrGet (int unit, u_char * addr);  /* get enet addr */

/* locals */

LOCAL DRV_CTRL drvCtrl[MAX_UNITS];	/* array of driver control */

/* forward declarations */

#ifdef __STDC__

LOCAL STATUS    quInit (int unit);
#ifdef BSD43_DRIVER
LOCAL int       quOutput (IFNET * pIfNet, MBUF * pMbuf, SOCK * pSock);
LOCAL void      quStartOutput (int unit);
#else
LOCAL void      quStartOutput (DRV_CTRL * pDrvCtrl); 
#endif
LOCAL int       quIoctl (IFNET * pIfNet, int cmd, caddr_t data);
LOCAL void      quReset (int unit);
LOCAL void      quIntr (int unit);
LOCAL void	quTxRestart (DRV_CTRL *	pDrvCtrl);
LOCAL void      quHandleInt (DRV_CTRL * pDrvCtrl);
LOCAL void      quRecv (DRV_CTRL * pDrvCtrl, SCC_BUF * pRxBd);
LOCAL void      quLoanFree (DRV_CTRL * pDrvCtrl, u_char * pRxBuf, UINT8 * pRef);

#else  /* __STDC__ */

LOCAL STATUS    quInit ();
LOCAL int       quOutput ();
LOCAL void      quStartOutput ();
LOCAL int       quIoctl ();
LOCAL void      quReset ();
LOCAL void      quIntr ();
LOCAL void	quTxRestart ();
LOCAL void      quHandleInt ();
LOCAL void      quRecv (); 
LOCAL void      quLoanFree (); 

#endif  /* __STDC__ */

/*******************************************************************************
*
* quattach - publish the `qu' network interface and initialize driver structures
*
* The routine publishes the `qu' interface by filling in a network Interface
* Data Record (IDR) and adding this record to the system list.
*
* The MC68EN360 shares a region of memory with the driver.  The caller of this
* routine can specify the address of a non-cacheable memory region with
* <bufBase>, or if this parameter is "NONE" the driver will obtain this
* memory region from calls to cacheDmaMalloc().  Non-cacheable memory space
* is important for cases where the MC68EN360 is operating in companion mode,
* and is attached to a processor with cache memory.
*
* Once non-cacheable memory is obtained, this routine will divide up the
* memory between the various buffer descriptors (BDs).  The number
* of BDs can be specified by <txBdNum> and <rxBdNum>, or if "NULL", a
* default value of 32 BDs will be used.  An additional number of buffers are
* reserved as receive loaner buffers.  The number of loaner buffers is the
* lesser of <rxBdNum> and a default number of 16.
*
* The user must specify the location of the transmit and receive BDs in
* the 68EN360's dual ported RAM.  <txBdBase> and <rxBdBase> give the
* offsets from <quAddr> for the base of the BD rings.  Each BD uses 8 bytes.
* Care must be taken so that the specified locations for Ethernet BDs
* do not conflict with other dual ported RAM structures. 
*
* Up to four individual device units are supported by this driver.  Device
* units may reside on different MC68EN360 chips, or may be on different SCCs
* within a single MC68EN360.  The <sccNum> parameter is used to explicitly
* state which SCC is being used. SCC1 is most commonly used, thus this
* parameter most often equals "1".
* 
* Before this routine returns, it calls quReset() to put the Ethernet
* controller in a quiescent state, and connects up the interrupt vector
* <ivec>.
*
* RETURNS: OK or ERROR.
*
* SEE ALSO: ifLib,
* .I "Motorola MC68360 User's Manual"
*/

STATUS quattach
    (
    int		unit,		/* unit number */
    UINT32	quAddr,		/* base address of QUICC chip internal mem */
    int		ivec,		/* interrupt vector */
    int		sccNum,		/* SCC number used */
    int		txBdNum,	/* number of transmit buffer descriptors */
    int		rxBdNum,	/* number of receive buffer descriptors */
    UINT32	txBdBase,	/* transmit buffer descriptor offset */
    UINT32	rxBdBase,	/* receive buffer descriptor offset */
    UINT32	bufBase		/* address of memory pool; NONE = malloc it */
    )
    {
    DRV_CTRL *	pDrvCtrl;
    int		counter;
    int         scc = sccNum - 1;
    int		loanNum = L_POOL;
    u_char *	txBuf;
    u_char *	rxBuf;
    u_char *	loanBuf;
     
    if (unit < 0 || unit >= MAX_UNITS)	/* sanity check the unit number */
	 return (ERROR);
		  
    pDrvCtrl = & drvCtrl[unit];
    
    /* ensure single invocation */

    if (pDrvCtrl->attached)
        return (ERROR);

    /* initialize flag(s) */

    pDrvCtrl->initialized = FALSE;

    /* do a quick check of input parameters */

    if ((sccNum < 1) || (sccNum > 4))
	return (ERROR);

    if (txBdBase == rxBdBase)
	return (ERROR);

    /* publish the Interface Data Record */
#ifdef BSD43_DRIVER
    ether_attach (& pDrvCtrl->idr.ac_if, unit, "qu", (FUNCPTR) quInit,
                  (FUNCPTR) quIoctl, (FUNCPTR) quOutput, (FUNCPTR) quReset);
#else
    ether_attach    (
                    (IFNET *) & pDrvCtrl->idr,
                    unit,
                    "qu",
                    (FUNCPTR) quInit,
                    (FUNCPTR) quIoctl,
                    (FUNCPTR) ether_output, /* generic ether_output */
                    (FUNCPTR) quReset
                    );

    pDrvCtrl->idr.ac_if.if_start = (FUNCPTR) quStartOutput;
#endif /* BSD43_DRIVER */

    /* use default number of buffer descriptors if user did not specify */

    if (txBdNum == NULL)
	txBdNum = TX_BD_DEFAULT;
    if (rxBdNum == NULL)
	rxBdNum = RX_BD_DEFAULT;

    /* must be at least two transmit and receive buffer descriptors */

    txBdNum = max (TX_BD_MIN, txBdNum);
    rxBdNum = max (RX_BD_MIN, rxBdNum);
    loanNum = min (rxBdNum, loanNum); 

    /* fill in driver control structure for <unit> */

    pDrvCtrl->regBase         = quAddr;
    pDrvCtrl->ether.sccNum    = sccNum;
    pDrvCtrl->ether.txBdNum   = txBdNum;
    pDrvCtrl->ether.rxBdNum   = rxBdNum;
    pDrvCtrl->nLoanRxBd       = loanNum;
    pDrvCtrl->ether.txBdBase  = (SCC_BUF *) (quAddr + (txBdBase & 0xfff));
    pDrvCtrl->ether.rxBdBase  = (SCC_BUF *) (quAddr + (rxBdBase & 0xfff));
    pDrvCtrl->ether.txBufSize = FRAME_MAX_AL;
    pDrvCtrl->ether.rxBufSize = FRAME_MAX_AL;

    /* derive SCC dependent variables */

    pDrvCtrl->ether.pScc    = (SCC *) ((UINT32) M360_DPR_SCC1(quAddr) +
                                       (scc * 0x100));
    pDrvCtrl->ether.pSccReg = (SCC_REG *) ((UINT32) M360_CPM_GSMR_L1(quAddr) +
                                           (scc * 0x20));
    pDrvCtrl->ether.intMask = CPIC_CIXR_SCC4 << (3 - scc);
    
    /* set up Rx and Tx buffers */

    if (bufBase == NONE)
	{
        /* allocate memory pools */

	txBuf = cacheDmaMalloc (txBdNum * pDrvCtrl->ether.txBufSize);
        if (txBuf == NULL)
	    return (ERROR);

	rxBuf = cacheDmaMalloc (rxBdNum * pDrvCtrl->ether.rxBufSize);
        if (rxBuf == NULL)
	    {
	    free (txBuf);
	    return (ERROR);
	    }

        loanBuf = cacheDmaMalloc (loanNum * pDrvCtrl->ether.rxBufSize);
	if (loanBuf == NULL)
            pDrvCtrl->nLoanRxBd = 0;
        }
    else
	{
	/* use passed in address as memory pool - assume OK */

	txBuf   = (u_char *) bufBase;
	rxBuf   = (u_char *) (txBuf + (txBdNum * pDrvCtrl->ether.txBufSize));
	loanBuf = (u_char *) (rxBuf + (rxBdNum * pDrvCtrl->ether.rxBufSize));
	}

    /* assign buffer addresses */

    for (counter = 0; counter < txBdNum; counter++)
        pDrvCtrl->ether.txBdBase[counter].dataPointer = (txBuf + (counter *
	    pDrvCtrl->ether.txBufSize));

    for (counter = 0; counter < rxBdNum; counter++)
        pDrvCtrl->ether.rxBdBase[counter].dataPointer = (rxBuf + (counter *
	    pDrvCtrl->ether.rxBufSize));

    for (counter = 0; counter < pDrvCtrl->nLoanRxBd; counter++)
        {
        pDrvCtrl->lPool[counter] = (loanBuf + (counter *
				    pDrvCtrl->ether.rxBufSize));
        pDrvCtrl->refCnt[counter] = 0;
	pDrvCtrl->pRefCnt[counter] = & pDrvCtrl->refCnt[counter];
        }

    /* reset the chip */

    quReset (unit);

    /* connect the interrupt handler quIntr() */

    if (intConnect (INUM_TO_IVEC (ivec), (VOIDFUNCPTR) quIntr, unit) == ERROR)
        return (ERROR);

    quInit (unit);

    /* mark driver as attached */

    pDrvCtrl->attached = TRUE;

    return (OK);
    }

/*******************************************************************************
*
* quInit - network interface initialization routine
*
* This routine marks the interface as down, then configures and initializes
* the chip.   It sets up the transmit and receive buffer descriptor (BD)
* rings, initializes registers, and enables interrupts.  Finally, it marks
* the interface as up and running.  It does not touch the receive loaner
* buffer stack.
*
* This routine is called from the driver attach routine at boot time.
*
* The complement of this rotuine is quReset().  Once a unit is reset by
* quReset(), it may be re-initialized to a running state by this routine.
*
* RETURNS: OK if successful, otherwise ERROR.
*/

LOCAL STATUS quInit
    (
    int		unit		/* unit number */
    )
    {
    DRV_CTRL *	pDrvCtrl;
    u_char	enetAddr[ENET_ADDR_SIZE];
    int		counter;

    /* sanity check the unit number */

    if (unit < 0 || unit >= MAX_UNITS)
	 return (ERROR);
		  
    pDrvCtrl = & drvCtrl[unit];

    /* ensure single invocation */

    if (pDrvCtrl->initialized)
        return (OK);

    /* mark the device as down */
 
    pDrvCtrl->idr.ac_if.if_flags &= ~(IFF_UP | IFF_RUNNING);

    /* initialize flag(s) */

    pDrvCtrl->txStop = FALSE;

    /* set up transmit buffer descriptors */

    pDrvCtrl->ether.pScc->param.tbase = (UINT16)
	((UINT32) pDrvCtrl->ether.txBdBase & 0xfff);
    pDrvCtrl->ether.pScc->param.tbptr = (UINT16)
	((UINT32) pDrvCtrl->ether.txBdBase & 0xfff);

    pDrvCtrl->ether.txBdNext  = 0;

    /* initialize each transmit buffer descriptor */
     
    for (counter = 0; counter < pDrvCtrl->ether.txBdNum; counter++)
        pDrvCtrl->ether.txBdBase[counter].statusMode = SCC_ETHER_TX_BD_I |
                                                       SCC_ETHER_TX_BD_PAD |
                                                       SCC_ETHER_TX_BD_L |
                                                       SCC_ETHER_TX_BD_TC;

    /* set the last BD to wrap to the first */

    pDrvCtrl->ether.txBdBase[(counter - 1)].statusMode |= SCC_ETHER_TX_BD_W;

    /* set up receive buffer descriptors */

    pDrvCtrl->ether.pScc->param.rbase = (UINT16)
	((UINT32) pDrvCtrl->ether.rxBdBase & 0xfff);
    pDrvCtrl->ether.pScc->param.rbptr = (UINT16)
	((UINT32) pDrvCtrl->ether.rxBdBase & 0xfff);

    pDrvCtrl->ether.rxBdNext  = 0;

    /* initialize each receive buffer descriptor */

    for (counter = 0; counter < pDrvCtrl->ether.rxBdNum; counter++)
        pDrvCtrl->ether.rxBdBase[counter].statusMode = SCC_ETHER_RX_BD_I |
        					       SCC_ETHER_RX_BD_E;

    /* set the last BD to wrap to the first */

    pDrvCtrl->ether.rxBdBase[(counter - 1)].statusMode |= SCC_ETHER_RX_BD_W;

    /* set SCC attributes to Ethernet mode */
 
    pDrvCtrl->ether.pSccReg->gsmrl    = SCC_GSMRL_ETHERNET | SCC_GSMRL_TPP_10 |
                                        SCC_GSMRL_TPL_48   | SCC_GSMRL_TCI;
    pDrvCtrl->ether.pSccReg->gsmrh    = 0x0;

    pDrvCtrl->ether.pSccReg->psmr     = SCC_ETHER_PSMR_CRC |
					SCC_ETHER_PSMR_NIB_22;
 
    if (pDrvCtrl->idr.ac_if.if_flags & IFF_PROMISC)
        pDrvCtrl->ether.pSccReg->psmr |= SCC_ETHER_PSMR_PRO;
 
    pDrvCtrl->ether.pSccReg->dsr      = 0xd555;
    pDrvCtrl->ether.pScc->param.rfcr  = 0x18;      /* supervisor data access */
    pDrvCtrl->ether.pScc->param.tfcr  = 0x18;      /* supervisor data access */
    pDrvCtrl->ether.pScc->param.mrblr = FRAME_MAX_AL;  /* max rx buffer size */

    /* initialize parameter the SCC RAM */
 
    ((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->c_pres	= 0xffffffff;
    ((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->c_mask	= 0xdebb20e3;
    ((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->crcec	= 0x00000000;
    ((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->alec	= 0x00000000;
    ((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->disfc	= 0x00000000;
    ((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->pads	= 0x8888;
    ((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->ret_lim	= 0x000f;
    ((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->mflr	= FRAME_MAX;
    ((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->minflr	= FRAME_MIN;
    ((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->maxd1	= FRAME_MAX;
    ((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->maxd2	= FRAME_MAX;
    ((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->gaddr1	= 0x0000;
    ((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->gaddr2	= 0x0000;
    ((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->gaddr3	= 0x0000;
    ((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->gaddr4	= 0x0000;
    ((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->p_per	= 0x0000;
    ((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->iaddr1	= 0x0000;
    ((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->iaddr2	= 0x0000;
    ((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->iaddr3	= 0x0000;
    ((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->iaddr4	= 0x0000;
    ((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->taddr_h	= 0x0000;
    ((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->taddr_m	= 0x0000;
    ((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->taddr_l	= 0x0000;

    /* set the hardware Ethernet address of the board */

    if (sys360EnetAddrGet (unit, (u_char *) & enetAddr) == ERROR)
	return (ERROR);

    ((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->paddr1_h =
        (enetAddr[5] << 8) + enetAddr[4];
    ((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->paddr1_m =
        (enetAddr[3] << 8) + enetAddr[2];
    ((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->paddr1_l =
        (enetAddr[1] << 8) + enetAddr[0];

    bcopy ((char *) enetAddr, (char *) pDrvCtrl->idr.ac_enaddr, ENET_ADDR_SIZE);

    /* enable Ethernet interrupts */

    pDrvCtrl->ether.pSccReg->scce = 0xffff;		/* clr events */

    pDrvCtrl->ether.pSccReg->sccm = SCC_ETHER_SCCX_RXF | SCC_ETHER_SCCX_TXE;

    *M360_CPM_CIMR(pDrvCtrl->regBase) |= pDrvCtrl->ether.intMask;

    /* call the BSP to do any other initialization (e.g., connecting clocks) */

    if (sys360EnetEnable (unit, pDrvCtrl->regBase) == ERROR)
	return (ERROR);

    /* raise the interface flags - mark the device as up */
 
    pDrvCtrl->idr.ac_if.if_flags |= IFF_UP | IFF_RUNNING | IFF_NOTRAILERS;

    /* mark driver as initialized */

    pDrvCtrl->initialized = TRUE;

    /* enable the transmitter */

    pDrvCtrl->ether.pSccReg->gsmrl |= SCC_GSMRL_ENT;

    /* issue the restart transmitter command to the CP */

    while (*M360_CPM_CR(pDrvCtrl->regBase) & CPM_CR_FLG);
    *M360_CPM_CR(pDrvCtrl->regBase) = ((pDrvCtrl->ether.sccNum - 1) << 6) |
                                      CPM_CR_SCC_RESTART | CPM_CR_FLG;
    while (*M360_CPM_CR(pDrvCtrl->regBase) & CPM_CR_FLG);

    /* enable the receiver */

    pDrvCtrl->ether.pSccReg->gsmrl |= SCC_GSMRL_ENR;

    return (OK);
    }
#ifdef BSD43_DRIVER
/*******************************************************************************
*
* quOutput - network interface output routine
*
* This routine simply calls ether_output(). ether_output() resolves 
* the hardware addresses and calls quStartOutput() with the unit number
* passed as an argument.
*
* RETURNS: OK if successful, otherwise errno.
*/

LOCAL int quOutput
    (
    IFNET *	pIfNet,		/* pointer to IFNET structure */
    MBUF *	pMbuf,		/* mbuf chain for output */
    SOCK *	pSock		/* sock ptr for destination */  
    )
    {
    DRV_CTRL * pDrvCtrl = & drvCtrl[pIfNet->if_unit];

    return (ether_output (pIfNet, pMbuf, pSock, (FUNCPTR) quStartOutput,
	    & pDrvCtrl->idr));
    }

/*******************************************************************************
*
* quStartOutput - output packet to network interface device
*
* quStartOutput() takes a packet from the network interface output queue,
* copies the mbuf chain into an interface buffer, and sends the packet over
* the interface.  etherOutputHookRtns are supported.
*
* Collision stats are collected in this routine from previously sent BDs.
* These BDs will not be examined until after the transmitter has cycled the
* ring, coming upon the BD after it has been sent. Thus, collision stat
* collection will be delayed a full cycle through the Tx ring.
*
* This routine is called from several possible threads.  Each one will be
* described below.
*
* The first, and most common thread, is when a user task requests the
* transmission of data. This will cause quOutput() to be called, which calls
* ether_output(), which usually calls this routine. This routine will not be
* called if ether_output() finds that our interface output queue is full.
* In this case, the outgoing data will be thrown out (very rare case).
*
* The second thread is when a transmitter error occurs that causes a
* TXE event interrupt.  This happens for the following errors: transmitter
* underrun, retry limit reached, late collision, and heartbeat error.
* The ISR sets the txStop flag to stop the transmitter until the errors are
* serviced. These events require a RESTART command of the transmitter, which
* occurs in the quTxRestart() routine.  After the transmitter is restarted,
* quTxRestart() does a netJobAdd of quStartOutput() to send any packets
* left in the interface output queue.  Thus, the second thread executes
* in the context of netTask().
*
* The third, and most unlikely, thread occurs when this routine is executing
* and it runs out of free Tx BDs.  In this case, this routine turns on
* transmit interrupt and exits.  When the next BD is actually sent, an interrupt
* occurs.  The ISR does a netJobAdd of quStartOutput() to continue sending
* packets left in the interface output queue.  Once again, we find ourselves
* executing in the context of netTask().
*
* RETURNS: N/A
*/

LOCAL void quStartOutput
    (
    int		unit		/* unit number */
    )
    {
    DRV_CTRL *	pDrvCtrl = & drvCtrl[unit];
#else /* BSD43_DRIVER */
LOCAL void quStartOutput
    (
    DRV_CTRL *	pDrvCtrl		/* unit number */
    )
    {
#endif /* BSD43_DRIVER */
    MBUF *	pMbuf;		/* mbuf pointer */
    int		length;
    SCC_BUF *	pTxBd;
    u_char *	pad;

    /*
     * Loop until:
     *     a) there are no more packets ready to send
     *     b) we have insufficient resources left to send another one
     *     c) a fatal transmitter error occurred, thus txStop was set
     */

    while (!pDrvCtrl->txStop && (pDrvCtrl->idr.ac_if.if_snd.ifq_head != NULL))
        {
	pTxBd = & pDrvCtrl->ether.txBdBase[pDrvCtrl->ether.txBdNext];

	/* check if a transmit buffer descriptor is available */

        if (pTxBd->statusMode & SCC_ETHER_TX_BD_R)
	    {
	    /* if no BD available, enable interrupt to see when one is free */

            pDrvCtrl->ether.pSccReg->scce  = SCC_ETHER_SCCX_TXB;
            pDrvCtrl->ether.pSccReg->sccm |= SCC_ETHER_SCCX_TXB;
	    break;
	    }

        IF_DEQUEUE (& pDrvCtrl->idr.ac_if.if_snd, pMbuf); /* dequeue a packet */

        copy_from_mbufs (pTxBd->dataPointer,pMbuf,length);

        /* QUICC padding mechanism in Rev A is buggy - do in software */

        if (length < FRAME_MIN)
            {
            pad = pTxBd->dataPointer + length;
            for (; length != FRAME_MIN; length++, pad++)
                *pad = 0x88;
            }

        /* call hook if one is connected */

        if ((etherOutputHookRtn == NULL) || ((* etherOutputHookRtn)
	    (& pDrvCtrl->idr.ac_if, pTxBd->dataPointer, length) == 0))
            {
	    /* add in collision stats from previously sent BDs */

            pDrvCtrl->idr.ac_if.if_collisions += ((pTxBd->statusMode &
					          SCC_ETHER_TX_BD_RC) >> 2);

	    /* if hook did not grab the frame, send packet */

            pTxBd->dataLength = length;
            if (pTxBd->statusMode & SCC_ETHER_TX_BD_W)
                pTxBd->statusMode = SCC_ETHER_TX_BD_I | SCC_ETHER_TX_BD_PAD |
                                    SCC_ETHER_TX_BD_L | SCC_ETHER_TX_BD_TC  |
                                    SCC_ETHER_TX_BD_W | SCC_ETHER_TX_BD_R;
	    else
                pTxBd->statusMode = SCC_ETHER_TX_BD_I | SCC_ETHER_TX_BD_PAD |
                                    SCC_ETHER_TX_BD_L | SCC_ETHER_TX_BD_TC  |
                                    SCC_ETHER_TX_BD_R;

            /* incr BD count */

	    pDrvCtrl->ether.txBdNext = (pDrvCtrl->ether.txBdNext + 1) %
				        pDrvCtrl->ether.txBdNum;
            }
        }
    }

/*******************************************************************************
*
* quIoctl - network interface control routine
*
* This routine implements the network interface control functions.
* It handles SIOCSIFADDR and SIOCSIFFLAGS commands.
*
* RETURNS: OK if successful, otherwise EINVAL.
*/

LOCAL int quIoctl
    (
    IFNET *	pIfNet,		/* pointer to IFNET structure */
    int		cmd,		/* command to process */
    caddr_t	data		/* pointer to data */ 
    )
    {
    DRV_CTRL * pDrvCtrl = & drvCtrl[pIfNet->if_unit];
    int         error   = 0;            /* error value */
    int         s       = splimp ();    /* sem not taken in calling functions */

    switch (cmd)
        {
        case SIOCSIFADDR:       /* set interface address */
            ((struct arpcom *)pIfNet)->ac_ipaddr = IA_SIN (data)->sin_addr;
            arpwhohas ((struct arpcom *)pIfNet, &IA_SIN (data)->sin_addr);
            break;
    
        case SIOCSIFFLAGS:
            /* set promiscuous bit according to flags */

	    if (pDrvCtrl->idr.ac_if.if_flags & IFF_PROMISC)
                pDrvCtrl->ether.pSccReg->psmr |= SCC_ETHER_PSMR_PRO;
            else
                pDrvCtrl->ether.pSccReg->psmr &= ~SCC_ETHER_PSMR_PRO;

            break;

        default:
            error = EINVAL;
        }

    splx (s);
    return (error);
    }

/*******************************************************************************
*
* quReset - network interface reset routine
*
* This routine marks the interface as down and resets the device.  This
* includes disabling interrupts, stopping the transmitter and receiver,
* and calling sys360EnetDisable() to do any target specific disabling.
*
* The complement of this rotuine is quInit().  Once a unit is reset in this
* routine, it may be re-initialized to a running state by quInit().
*
* RETURNS: N/A
*/

LOCAL void quReset
    (
    int		unit		/* unit number */
    )
    {
    DRV_CTRL * pDrvCtrl;
    int counter = 0xffff;

    if (unit < 0 || unit >= MAX_UNITS)	/* sanity check the unit number */
        return;
		  
    pDrvCtrl = & drvCtrl[unit];

    /* disable the SCC interrupts */

    *M360_CPM_CIMR(pDrvCtrl->regBase) &= ~pDrvCtrl->ether.intMask;

    /* issue the CP graceful stop command to the transmitter if necessary */

    if (pDrvCtrl->ether.pSccReg->gsmrl & SCC_GSMRL_ENT)
	{
        while (*M360_CPM_CR(pDrvCtrl->regBase) & CPM_CR_FLG);
        *M360_CPM_CR(pDrvCtrl->regBase) = ((pDrvCtrl->ether.sccNum - 1) << 6) |
                                          CPM_CR_SCC_GRSTOP | CPM_CR_FLG;
        while (*M360_CPM_CR(pDrvCtrl->regBase) & CPM_CR_FLG);

        /* wait for graceful stop to register */

        while ((counter--) && (!(pDrvCtrl->ether.pSccReg->scce &
			         SCC_ETHER_SCCX_GRA)));
	}

    /* disable the SCC receiver and transmitter */

    pDrvCtrl->ether.pSccReg->gsmrl &= ~(SCC_GSMRL_ENR | SCC_GSMRL_ENT);

    /* call the BSP to do any other disabling (e.g., *TENA) */

    sys360EnetDisable (unit, pDrvCtrl->regBase);

    /* mark the driver as down */

    pDrvCtrl->idr.ac_if.if_flags &= ~(IFF_UP | IFF_RUNNING);

    /* mark the driver as no longer initialized */

    pDrvCtrl->initialized = FALSE;
    }

/*******************************************************************************
*
* quIntr - network interface interrupt handler
*
* This routine gets called at interrupt level. It handles work that 
* requires minimal processing. Interrupt processing that is more 
* extensive gets handled at task level. The network task, netTask(), is 
* provided for this function. Routines get added to the netTask() work 
* queue via the netJobAdd() command.
*
* RETURNS: N/A
*/

LOCAL void quIntr
    (
    int		unit		/* unit number */
    )
    {
    DRV_CTRL * pDrvCtrl = & drvCtrl[unit];
    BOOL rxHandle = FALSE;
    BOOL txbHandle = FALSE;

    /* check for spurious interrupt -> attached & initialized ? */

    if ((!pDrvCtrl->attached) || (!pDrvCtrl->initialized))
	{
        pDrvCtrl->ether.pSccReg->scce = 0xffff;

        *M360_CPM_CISR(pDrvCtrl->regBase) = pDrvCtrl->ether.intMask;
	return;
	}

    /* handle receive events */

    if ((pDrvCtrl->ether.pSccReg->sccm & SCC_ETHER_SCCX_RXF) &&
        (pDrvCtrl->ether.pSccReg->scce & SCC_ETHER_SCCX_RXF))
	{
        (void) netJobAdd ((FUNCPTR) quHandleInt, (int) pDrvCtrl, 0, 0, 0, 0); 

	/* turn off receive interrupts for now - quHandleInt turns back on */

        pDrvCtrl->ether.pSccReg->sccm &= ~SCC_ETHER_SCCX_RXF;
	rxHandle = TRUE;
        }

    /* check for output errors */

    if (pDrvCtrl->ether.pSccReg->scce & SCC_ETHER_SCCX_TXE)
	{
	/* NOTE: HBC error not supported -> always RESTART Tx here */

        (void) netJobAdd ((FUNCPTR) quTxRestart, (int) pDrvCtrl, 0, 0, 0, 0);

	pDrvCtrl->txStop = TRUE;
	}

    /* handle transmitter events - BD full condition -> ever happen ? */

    if ((pDrvCtrl->ether.pSccReg->sccm & SCC_ETHER_SCCX_TXB) &&
        (pDrvCtrl->ether.pSccReg->scce & SCC_ETHER_SCCX_TXB))
	{
#ifdef BSD43_DRIVER
        (void) netJobAdd ((FUNCPTR) quStartOutput, unit, 0, 0, 0, 0); 
#else
        (void) netJobAdd ((FUNCPTR) quStartOutput, (int)pDrvCtrl, 0, 0, 0, 0); 
#endif /* BSD43_DRIVER */

        pDrvCtrl->ether.pSccReg->sccm &= ~SCC_ETHER_SCCX_TXB;
	txbHandle = TRUE;
	}

    /* check for input busy condition */

    if (pDrvCtrl->ether.pSccReg->scce & SCC_ETHER_SCCX_BSY)
	{
        pDrvCtrl->ether.pSccReg->scce = SCC_ETHER_SCCX_BSY; 

        /* count discarded frames as errors */

        pDrvCtrl->idr.ac_if.if_ierrors +=
	    ((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->disfc;
	((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->disfc = 0;
	}

    /* acknowledge all other interrupts - ignore events */

    pDrvCtrl->ether.pSccReg->scce = (pDrvCtrl->ether.pSccReg->scce &
				    ~(SCC_ETHER_SCCX_RXF |
				      SCC_ETHER_SCCX_TXE |
				      SCC_ETHER_SCCX_TXB |
				      SCC_ETHER_SCCX_BSY));

    /* acknowledge interrupts */

    if (rxHandle)
        pDrvCtrl->ether.pSccReg->scce = SCC_ETHER_SCCX_RXF; 
    if (pDrvCtrl->txStop)
        pDrvCtrl->ether.pSccReg->scce = SCC_ETHER_SCCX_TXE; 
    if (txbHandle)
        pDrvCtrl->ether.pSccReg->scce = SCC_ETHER_SCCX_TXB; 

    *M360_CPM_CISR(pDrvCtrl->regBase) = pDrvCtrl->ether.intMask;
    }

/*******************************************************************************
*
* quTxRestart - issue RESTART Tx command to the CP
*
* This routine issues a RESTART transmitter command to the CP.  It is
* executed by netTask (quIntr() did a netJobAdd).  quIntr() cannot do
* a RESTART directly because the CPM flag must be taken before a command
* can be written.  After a restart, a netJobAdd of quStartOutput() is
* performed in order to send any packets remaining in the interface output
* queue.
*
* RETURNS: N/A
*/

LOCAL void quTxRestart
    (
    DRV_CTRL *	pDrvCtrl	/* pointer to DRV_CTRL structure */
    )
    {
    /* update error counter */

    pDrvCtrl->idr.ac_if.if_oerrors++;

    /* restart transmitter */

    while (*M360_CPM_CR(pDrvCtrl->regBase) & CPM_CR_FLG);
    *M360_CPM_CR(pDrvCtrl->regBase) = ((pDrvCtrl->ether.sccNum - 1) << 6) |
                                      CPM_CR_SCC_RESTART | CPM_CR_FLG;
    while (*M360_CPM_CR(pDrvCtrl->regBase) & CPM_CR_FLG);

    /* restart transmit routine */

    pDrvCtrl->txStop = FALSE;
#ifdef 	BSD43_DRIVER
    (void) netJobAdd ((FUNCPTR) quStartOutput, pDrvCtrl->idr.ac_if.if_unit,
		      0, 0, 0, 0); 
#else
    (void) netJobAdd ((FUNCPTR) quStartOutput, (int)pDrvCtrl, 0, 0, 0, 0); 
#endif	/* BSD43_DRIVER */

    }

/*******************************************************************************
*
* quHandleInt - task-level interrupt handler
*
* This is the task-level interrupt handler, which is called from 
* netTask(). quHandleInt() gets input frames from the device and then calls 
* quRecv() to process each frame.  quRecv() only gets called if no error
* stats were reported in the buffer descriptor.  Data chaining is not
* supported.
*
* This routine should be called with SCC receive interrupts masked so that
* more netJobAdds of this routine are not performed by quIntr().
* Receive interrupts are turned back on by this routine before exiting.
*
* RETURNS: N/A
*/

LOCAL void quHandleInt
    (
    DRV_CTRL *	pDrvCtrl	/* pointer to DRV_CTRL structure */
    )
    {
    SCC_BUF *	pRxBd = & pDrvCtrl->ether.rxBdBase[pDrvCtrl->ether.rxBdNext];

    while (!(pRxBd->statusMode & SCC_ETHER_RX_BD_E))
        {
        /* data chaining is not supported  - check all error conditions */

        if (((pRxBd->statusMode & (SCC_ETHER_RX_BD_F  | SCC_ETHER_RX_BD_L))
	                       == (SCC_ETHER_RX_BD_F  | SCC_ETHER_RX_BD_L))
	                      && !(pRxBd->statusMode & (SCC_ETHER_RX_BD_CL |
		                   SCC_ETHER_RX_BD_OV | SCC_ETHER_RX_BD_CR |
				   SCC_ETHER_RX_BD_SH | SCC_ETHER_RX_BD_NO |
				   SCC_ETHER_RX_BD_LG)))
            quRecv (pDrvCtrl, pRxBd);
        else
            pDrvCtrl->idr.ac_if.if_ierrors++;

        /* reset buffer descriptor as empty */

	if (pRxBd->statusMode & SCC_ETHER_RX_BD_W)
            pRxBd->statusMode = SCC_ETHER_RX_BD_E | SCC_ETHER_RX_BD_I |
                                SCC_ETHER_RX_BD_W;
        else
            pRxBd->statusMode = SCC_ETHER_RX_BD_E | SCC_ETHER_RX_BD_I;

        /* incr BD count */

        pDrvCtrl->ether.rxBdNext = (pDrvCtrl->ether.rxBdNext + 1) %
				    pDrvCtrl->ether.rxBdNum;

        pRxBd = & pDrvCtrl->ether.rxBdBase[pDrvCtrl->ether.rxBdNext];

        /* clear Rx events */

        pDrvCtrl->ether.pSccReg->scce = SCC_ETHER_SCCX_RXF;
        }

    /* re-enable Rx interrupts */

    pDrvCtrl->ether.pSccReg->sccm |= SCC_ETHER_SCCX_RXF;
    }

/*******************************************************************************
*
* quRecv - process an input frame
*
* This routine processes an input frame, then passes it up to the higher 
* level in a form it expects.  Buffer loaning, promiscuous mode, and
* etherInputHookRtns are all supported.  Trailer protocols is not supported.
*
* RETURNS: N/A
*/

LOCAL void quRecv
    (
    DRV_CTRL *	pDrvCtrl,	/* pointer to DRV_CTRL structure */
    SCC_BUF *	pRxBd		/* receive buffer descriptor */
    )
    { 
    FAST ETH_HDR *	pEtherHdr;	/* pointer to the ethernet header */
    MBUF *		pMbuf = NULL;	/* MBUF pointer */
    u_char *		pData;
    int			length;
#ifdef BSD43_DRIVER
    UINT16		type;
#endif /* BSD43_DRIVER */

    /* call hook if one is connected */

    if ((etherInputHookRtn != NULL) && ((*etherInputHookRtn)
	(& pDrvCtrl->idr.ac_if, pRxBd->dataPointer, pRxBd->dataLength) != 0))
        return; 

    /* filter frame if controller is in promiscuous mode - M bit is buggy */

    if (pDrvCtrl->ether.pSccReg->psmr & SCC_ETHER_PSMR_PRO)
	{
	if ((bcmp ((char *) ((ETH_HDR *) pRxBd->dataPointer)->ether_dhost,
	     (char *) pDrvCtrl->idr.ac_enaddr, ENET_ADDR_SIZE)) &&
	    (bcmp ((char *) ((ETH_HDR *) pRxBd->dataPointer)->ether_dhost,
	     (char *) etherbroadcastaddr, ENET_ADDR_SIZE)))
           return;
	}

    /* adjust length to data only */

    length = pRxBd->dataLength - SIZEOF_ETHERHEADER;
    if (length <= 0)
	{ 
        pDrvCtrl->idr.ac_if.if_ierrors++;
	return;
	}

    /* point to ethernet header */
    
    pEtherHdr = (ETH_HDR *) pRxBd->dataPointer; 

    /* point to data */
    
    pData = (u_char *)pEtherHdr + SIZEOF_ETHERHEADER;

#ifdef BSD43_DRIVER
    /* save type - build_cluster trashes the type field */

    type = ((ETH_HDR *) pRxBd->dataPointer)->ether_type;
#endif /* BSD43_DRIVER */
    
    /* OK to loan out buffer ? -> build a mbuf cluster */

    if ((pDrvCtrl->nLoanRxBd > 0) && (USE_CLUSTER (length)))
	pMbuf = build_cluster (
        		       pData,
                               length,
                               & pDrvCtrl->idr.ac_if,
                               MC_QU,
                               pDrvCtrl->pRefCnt [(pDrvCtrl->nLoanRxBd - 1)],
                               (FUNCPTR)quLoanFree,
                               (int) pDrvCtrl,
                               (int) pRxBd->dataPointer,
                               (int) pDrvCtrl->pRefCnt [(pDrvCtrl->nLoanRxBd -
                                                         1)]
                               );

    /* if buffer was successfully turned into mbuf cluster */

    if (pMbuf != NULL)
	pRxBd->dataPointer = pDrvCtrl->lPool[--pDrvCtrl->nLoanRxBd];
    else
        {
	/* else do same ol' copy to mbuf */

        pMbuf = copy_to_mbufs (pData, length, 0, & pDrvCtrl->idr.ac_if);
        if (pMbuf == NULL)
	    {
            pDrvCtrl->idr.ac_if.if_ierrors++;
            return;
	    }
        }

    /* send up to protocol */
#ifdef BSD43_DRIVER
    do_protocol_with_type (type, pMbuf, & pDrvCtrl->idr, length);
#else
    do_protocol (pEtherHdr, pMbuf, &pDrvCtrl->idr, length);
#endif

    /* bump input packet counter */

    pDrvCtrl->idr.ac_if.if_ipackets++;
    }

/*******************************************************************************
*
* quLoanFree - return the given buffer to loaner pool
*
* This routine returns <pRxBuf> to the pool of available loaner buffers.
* It also returns <pRef> to the pool of available loaner reference counters,
* then zeroes the reference count.
*
* RETURNS: N/A
*/

LOCAL void quLoanFree
    (
    DRV_CTRL *	pDrvCtrl,
    u_char *	pRxBuf,
    UINT8 *	pRef
    )
    {
    /* return loaned buffer to pool */

    pDrvCtrl->lPool[pDrvCtrl->nLoanRxBd] = pRxBuf;

    /* return loaned reference count to pool */

    pDrvCtrl->pRefCnt[pDrvCtrl->nLoanRxBd++] = pRef;

    /* reset reference count - should have been done from above, but... */

    *pRef = 0;
    }
