/* if_cpm.c - Motorola CPM core network interface driver */

/* Copyright 1996-1998 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01f,13apr98,map  modified SCC_DEV to SCC_ETHER_DEV.
01e,15jul97,spm  removed driver initialization from ioctl support (SPR #8831);
                 corrected wait in reset routine; 
                 changed interrupt handler to match if_qu driver
01d,07apr97,spm  code cleanup, corrected statistics, and upgraded to BSD 4.4
01c,06nov96,dgp  doc: final formatting
01b,06aug96,dat  fixed scce register handling, and interrupt service rtn
01a,09may96,dzb  derived from v01e of src/drv/netif/if_qu.c.
*/

/*
DESCRIPTION
This module implements the driver for the Motorola CPM core Ethernet network
interface used in the M68EN360 and PPC800-series communications controllers.

The driver is designed to support the Ethernet mode of an SCC residing on the
CPM processor core.  It is generic in the sense that it does not care
which SCC is being used, and it supports up to four individual units per board.

The driver must be given several target-specific parameters, and some external
support routines must be provided.  These parameters, and the mechanisms used
to communicate them to the driver, are detailed below.

This network interface driver does not include support for trailer protocols
or data chaining.  However, buffer loaning has been implemented in an effort to
boost performance. This driver provides support for four individual device 
units.

This driver maintains cache coherency by allocating buffer space using the
cacheDmaMalloc() routine.  It is assumed that cache-safe memory is returned;
this driver does not perform cache flushing and invalidating.

BOARD LAYOUT
This device is on-chip.  No jumpering diagram is necessary.

EXTERNAL INTERFACE
This driver presents the standard WRS network driver API: the device
unit must be attached and initialized with the cpmattach() routine.

The only user-callable routine is cpmattach(), which publishes the `cpm'
interface and initializes the driver structures.

TARGET-SPECIFIC PARAMETERS
These parameters are passed to the driver via cpmattach().

.iP "address of SCC parameter RAM"
This parameter is the address of the parameter RAM used to control the
SCC.  Through this address, and the address of the SCC
registers (see below), different network interface units are able to use
different SCCs without conflict.  This parameter points to the internal
memory of the chip where the SCC physically resides, which may not necessarily
be the master chip on the target board.
.iP "address of SCC registers"
This parameter is the address of the registers used to control the
SCC.  Through this address, and the address of the SCC
parameter RAM (see above), different network interface units are able to use
different SCCs without conflict.  This parameter points to the internal
memory of the chip where the SCC physically resides, which may not necessarily
be the master chip on the target board.
.iP "interrupt-vector offset"
This driver configures the SCC to generate hardware interrupts
for various events within the device.  The interrupt-vector offset
parameter is used to connect the driver's ISR to the interrupt through
a call to intConnect().
.iP "address of transmit and receive buffer descriptors"
These parameters indicate the base locations of the transmit and receive
buffer descriptor (BD) rings.  Each BD takes up 8 bytes of
dual-ported RAM, and it is the user's responsibility to ensure that all
specified BDs will fit within dual-ported RAM.  This includes any other
BDs the target board may be using, including other SCCs, SMCs, and the
SPI device.  There is no default for these parameters; they must be
provided by the user.
.iP "number of transmit and receive buffer descriptors"
The number of transmit and receive buffer descriptors (BDs) used is
configurable by the user upon attaching the driver.  Each buffer
descriptor resides in 8 bytes of the chip's dual-ported RAM space,
and each one points to a 1520-byte buffer in regular RAM.  There must
be a minimum of two transmit and two receive BDs.  There is no maximum
number of buffers, but there is a limit to how much the driver speed increases
as more buffers are added, and dual-ported RAM space is at a premium.
If this parameter is "NULL", a default value of 32 BDs is used.
.iP "base address of buffer pool"
This parameter is used to notify the driver that space for the transmit
and receive buffers need not be allocated, but should be taken from a
cache-coherent private memory space provided by the user at the given
address.  The user should be aware that memory used for buffers must be
4-byte aligned and non-cacheable.  All the buffers
must fit in the given memory space; no checking is performed.
This includes all transmit and receive buffers (see above) and an
additional 16 receive loaner buffers.  If the number of receive
BDs is less than 16, that number of loaner buffers is
used.  Each buffer is 1520 bytes.  If this parameter is "NONE," space
for buffers is obtained by calling cacheDmaMalloc() in cpmattach().
.LP

EXTERNAL SUPPORT REQUIREMENTS
This driver requires seven external support functions:

.iP "STATUS sysCpmEnetEnable (int unit)" "" 9 -1
This routine is expected to perform any target-specific functions required
to enable the Ethernet controller.  These functions typically include
enabling the Transmit Enable signal (TENA) and connecting the transmit
and receive clocks to the SCC.
The driver calls this routine, once per unit, from the cpmInit() routine.
.iP "void sysCpmEnetDisable (int unit)"
This routine is expected to perform any target-specific functions required
to disable the Ethernet controller.  This usually involves disabling the
Transmit Enable (TENA) signal.
The driver calls this routine from the cpmReset() routine each time a unit
is disabled.
.iP "STATUS sysCpmEnetCommand (int unit, UINT16 command)"
This routine is expected to issue a command to the Ethernet interface
controller.  The driver calls this routine to perform basic commands,
such as restarting the transmitter and stopping reception.
.iP "void sysCpmEnetIntEnable (int unit)"
This routine is expected to enable the interrupt for the Ethernet interface
specified by <unit>.
.iP "void sysCpmEnetIntDisable (int unit)"
This routine is expected to disable the interrupt for the Ethernet interface
specified by <unit>.
.iP "void sysCpmEnetIntClear (int unit)"
This routine is expected to clear the interrupt for the Ethernet interface
specified by <unit>.
.iP "STATUS sysCpmEnetAddrGet (int unit, UINT8 * addr)"
The driver expects this routine to provide the 6-byte Ethernet
hardware address that will be used by <unit>.  This routine must copy
the 6-byte address to the space provided by <addr>.  This routine is
expected to return OK on success, or ERROR.
The driver calls this routine, once per unit, from the cpmInit() routine.
.LP

SYSTEM RESOURCE USAGE
This driver requires the following system resources:

    - one mutual exclusion semaphore
    - one interrupt vector
    - 0 bytes in the initialized data section (data)
    - 1272 bytes in the uninitialized data section (BSS)
 
The data and BSS sections are quoted for the CPU32 architecture and may vary
for other architectures.  The code size (text) varies greatly between
architectures, and is therefore not quoted here.

If the driver allocates the memory shared with the Ethernet device unit,
it does so by calling the cacheDmaMalloc() routine.  For the default case
of 32 transmit buffers, 32 receive buffers, and 16 loaner buffers, the total
size requested is 121,600 bytes.  If a non-cacheable memory region is provided
by the user, the size of this region should be this amount, unless the
user has specified a different number of transmit or receive BDs.

This driver can operate only if the shared memory region is non-cacheable,
or if the hardware implements bus snooping.  The driver cannot maintain
cache coherency for the device because the buffers are asynchronously
modified by both the driver and the device, and these fields may share the
same cache line.  Additionally, the chip's dual ported RAM must be
declared as non-cacheable memory where applicable.

SEE ALSO: ifLib,
.I "Motorola MC68EN360 User's Manual",
.I "Motorola MPC860 User's Manual",
.I "Motorola MPC821 User's Manual"

INTERNAL
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
#include "netLib.h"
#include "drv/netif/netifDev.h"
#include "drv/netif/if_cpm.h"

/* defines */

#define MAX_UNITS	4	/* max cpm units */
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

typedef struct drv_ctrl	/* DRV_CTRL */
    {
    IDR		idr;		/* Interface Data Record */
    BOOL	attached;	/* indicates unit is attached */
    BOOL	txStop;		/* emergency stop output */
    int		nLoanRxBd;	/* number of receive buffers left to loan */
    UINT8 *	lPool[L_POOL];	/* receive BD loaner pool */
    UINT8 *	pRefCnt[L_POOL];/* stack of reference count pointers */
    UINT8	refCnt[L_POOL]; /* actual reference count values */
    SCC_ETHER_DEV	ether;		/* Ethernet SCC device */
    } DRV_CTRL;

/* globals */

IMPORT STATUS sysCpmEnetEnable  (int unit);
IMPORT void   sysCpmEnetDisable (int unit);
IMPORT STATUS sysCpmEnetAddrGet (int unit, UINT8 * addr);
IMPORT STATUS sysCpmEnetCommand (int unit, UINT16 command);
IMPORT void   sysCpmEnetIntEnable (int unit);
IMPORT void   sysCpmEnetIntDisable (int unit);
IMPORT void   sysCpmEnetIntClear (int unit);

/* locals */

LOCAL DRV_CTRL drvCtrl[MAX_UNITS];	/* array of driver control */

/* forward declarations */

#ifdef __STDC__

LOCAL STATUS    cpmInit (int unit);
#ifdef BSD43_DRIVER
LOCAL int       cpmOutput (IFNET * pIfNet, MBUF * pMbuf, SOCK * pSock);
LOCAL void      cpmStartOutput (int unit);
#else
LOCAL void      cpmStartOutput (DRV_CTRL * pDrvCtrl);
#endif
LOCAL int       cpmIoctl (IFNET * pIfNet, int cmd, caddr_t data);
LOCAL STATUS    cpmReset (int unit);
LOCAL void      cpmIntr (int unit);
LOCAL void	cpmTxRestart (int unit);
LOCAL void      cpmHandleInt (DRV_CTRL * pDrvCtrl);
LOCAL void      cpmRecv (DRV_CTRL * pDrvCtrl, SCC_BUF * pRxBd);
LOCAL void      cpmLoanFree (DRV_CTRL * pDrvCtrl, UINT8 * pRxBuf, UINT8 * pRef);

#else  /* __STDC__ */

LOCAL STATUS    cpmInit ();
LOCAL int       cpmOutput ();
LOCAL void      cpmStartOutput ();
LOCAL int       cpmIoctl ();
LOCAL STATUS    cpmReset ();
LOCAL void      cpmIntr ();
LOCAL void	cpmTxRestart ();
LOCAL void      cpmHandleInt ();
LOCAL void      cpmRecv (); 
LOCAL void      cpmLoanFree (); 

#endif  /* __STDC__ */

/*******************************************************************************
*
* cpmattach - publish the `cpm' network interface and initialize the driver
*
* The routine publishes the `cpm' interface by filling in a network Interface
* Data Record (IDR) and adding this record to the system's interface list.
*
* The SCC shares a region of memory with the driver.  The caller of this
* routine can specify the address of a shared, non-cacheable memory region
* with <bufBase>.  If this parameter is NONE, the driver obtains this
* memory region by calling cacheDmaMalloc().  Non-cacheable memory space
* is important for cases where the SCC is operating with a processor
* that has a data cache.
*
* Once non-cacheable memory is obtained, this routine divides up the
* memory between the various buffer descriptors (BDs).  The number
* of BDs can be specified by <txBdNum> and <rxBdNum>, or if NULL, a
* default value of 32 BDs will be used.  Additional buffers are
* reserved as receive loaner buffers.  The number of loaner buffers is the
* lesser of <rxBdNum> and a default value of 16.
*
* The user must specify the location of the transmit and receive BDs in
* the CPU's dual-ported RAM.  <txBdBase> and <rxBdBase> give the
* base address of the BD rings.  Each BD uses 8 bytes. Care must be taken so
* that the specified locations for Ethernet BDs do not conflict with other
* dual-ported RAM structures. 
*
* Up to four individual device units are supported by this driver.  Device
* units may reside on different processor chips, or may be on different SCCs
* within a single CPU.
* 
* Before this routine returns, it calls cpmReset() and cpmInit() to 
* configure the Ethernet controller, and connects the interrupt vector <ivec>.
*
* RETURNS: OK or ERROR.
*
* SEE ALSO: ifLib,
* .I "Motorola MC68360 User's Manual",
* .I "Motorola MPC821 and MPC860 User's Manual"
*/

STATUS cpmattach
    (
    int		unit,		/* unit number */
    SCC *	pScc,		/* address of SCC parameter RAM */
    SCC_REG *	pSccReg,	/* address of SCC registers */
    VOIDFUNCPTR * ivec,		/* interrupt vector offset */
    SCC_BUF *	txBdBase,	/* transmit buffer descriptor base address */
    SCC_BUF *	rxBdBase,	/* receive buffer descriptor base address */
    int		txBdNum,	/* number of transmit buffer descriptors */
    int		rxBdNum,	/* number of receive buffer descriptors */
    UINT8 *	bufBase		/* address of memory pool; NONE = malloc it */
    )
    {
    DRV_CTRL *	pDrvCtrl;
    int		counter;
    int		loanNum = L_POOL;
    UINT8 *	txBuf;
    UINT8 *	rxBuf;
    UINT8 *	loanBuf;
     
    if (unit < 0 || unit >= MAX_UNITS)	/* sanity check the unit number */
	 return (ERROR);
		  
    pDrvCtrl = & drvCtrl[unit];
    
    /* ensure single invocation */

    if (pDrvCtrl->attached)
        return (ERROR);

    /* do a quick check of input parameters */

    if (txBdBase == rxBdBase)
	return (ERROR);

    /* publish the Interface Data Record */

#ifdef BSD43_DRIVER
    ether_attach (& pDrvCtrl->idr.ac_if, unit, "cpm", (FUNCPTR) cpmInit,
                  (FUNCPTR) cpmIoctl, (FUNCPTR) cpmOutput, (FUNCPTR) cpmReset);
#else
    ether_attach (
                 &pDrvCtrl->idr.ac_if, 
                 unit, 
                 "cpm", 
                 (FUNCPTR) cpmInit,
                 (FUNCPTR) cpmIoctl, 
                 (FUNCPTR) ether_output, 
                 (FUNCPTR) cpmReset
                 );
    pDrvCtrl->idr.ac_if.if_start = (FUNCPTR)cpmStartOutput;
#endif

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

    pDrvCtrl->nLoanRxBd       = loanNum;
    pDrvCtrl->ether.txBdNum   = txBdNum;
    pDrvCtrl->ether.rxBdNum   = rxBdNum;
    pDrvCtrl->ether.txBdBase  = txBdBase;
    pDrvCtrl->ether.rxBdBase  = rxBdBase;
    pDrvCtrl->ether.txBufSize = FRAME_MAX_AL;
    pDrvCtrl->ether.rxBufSize = FRAME_MAX_AL;
    pDrvCtrl->ether.pScc      = pScc;
    pDrvCtrl->ether.pSccReg   = pSccReg;
    
    /* set up Rx and Tx buffers */

    if (bufBase == (UINT8 *) NONE)
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

	txBuf   = bufBase;
	rxBuf   = (UINT8 *) (txBuf + (txBdNum * pDrvCtrl->ether.txBufSize));
	loanBuf = (UINT8 *) (rxBuf + (rxBdNum * pDrvCtrl->ether.rxBufSize));
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

    if (cpmReset (unit) == ERROR)
        return (ERROR);

    /* connect the interrupt handler cpmIntr() */

    if (intConnect (ivec, (VOIDFUNCPTR) cpmIntr, unit) == ERROR)
        return (ERROR);

    /* Initialize and start controller */

    cpmInit (unit);

    /* mark driver as attached */

    pDrvCtrl->attached = TRUE;

    return (OK);
    }

/*******************************************************************************
*
* cpmInit - network interface initialization routine
*
* This routine marks the interface as down, then configures and initializes
* the chip.  It sets up the transmit and receive buffer descriptor (BD)
* rings, initializes registers, and enables interrupts.  Finally, it marks
* the interface as up and running.  It does not touch the receive loaner
* buffer stack.
*
* The complement of this routine is cpmReset().  Once a unit is reset by
* cpmReset(), it may be re-initialized to a running state by this routine.
*
* RETURNS: OK if successful, otherwise ERROR.
*/

LOCAL STATUS cpmInit
    (
    int		unit		/* unit number */
    )
    {
    DRV_CTRL *	pDrvCtrl;
    UINT8	enetAddr[ENET_ADDR_SIZE];
    int		counter;

    /* sanity check the unit number */

    if (unit < 0 || unit >= MAX_UNITS)
	 return (ERROR);

    pDrvCtrl = & drvCtrl[unit];

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

    if (sysCpmEnetAddrGet (unit, (UINT8 *) & enetAddr) == ERROR)
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

    sysCpmEnetIntEnable (unit);				/* unmask interrupts */

    /* call the BSP to do any other initialization (e.g., connecting clocks) */

    if (sysCpmEnetEnable (unit) == ERROR)
	return (ERROR);

    /* raise the interface flags - mark the device as up */
 
    pDrvCtrl->idr.ac_if.if_flags |= IFF_UP | IFF_RUNNING | IFF_NOTRAILERS;

    /* enable the transmitter */

    pDrvCtrl->ether.pSccReg->gsmrl |= SCC_GSMRL_ENT;

    /* issue the restart transmitter command to the CP */

    (void) sysCpmEnetCommand (unit, CPM_CR_SCC_RESTART);

    /* enable the receiver */

    pDrvCtrl->ether.pSccReg->gsmrl |= SCC_GSMRL_ENR;

    return (OK);
    }

#ifdef BSD43_DRIVER
/*******************************************************************************
*
* cpmOutput - network interface output routine
*
* This routine simply calls ether_output(). ether_output() resolves 
* the hardware addresses and calls cpmStartOutput() with the unit number
* passed as an argument.
*
* RETURNS: OK if successful, otherwise errno.
*/

LOCAL int cpmOutput
    (
    IFNET *	pIfNet,		/* pointer to IFNET structure */
    MBUF *	pMbuf,		/* mbuf chain for output */
    SOCK *	pSock		/* sock ptr for destination */  
    )
    {
    DRV_CTRL * pDrvCtrl = & drvCtrl[pIfNet->if_unit];

    return (ether_output (pIfNet, pMbuf, pSock, (FUNCPTR) cpmStartOutput,
	    & pDrvCtrl->idr));
    }
#endif

/*******************************************************************************
*
* cpmStartOutput - output packet to network interface device
*
* cpmStartOutput() takes a packet from the network interface output queue,
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
* transmission of data. Under BSD 4.3, this will cause cpmOutput() to be
* called, which calls ether_output(), which usually calls this routine. 
* This routine will not be called if ether_output() finds that our interface
* output queue is full. In this very rare case, the outgoing data will be 
* thrown out. BSD 4.4 uses a slightly different model in which the generic
* ether_output() routine is called directly, followed by a call to this
* routine.
*
* The second thread is when a transmitter error occurs that causes a
* TXE event interrupt.  This happens for the following errors: transmitter
* underrun, retry limit reached, late collision, and heartbeat error.
* The ISR sets the txStop flag to stop the transmitter until the errors are
* serviced. These events require a RESTART command of the transmitter, which
* occurs in the cpmTxRestart() routine.  After the transmitter is restarted,
* cpmTxRestart() does a netJobAdd of cpmStartOutput() to send any packets
* left in the interface output queue.  Thus, the second thread executes
* in the context of netTask().
*
* The third, and most unlikely, thread occurs when this routine is executing
* and it runs out of free Tx BDs.  In this case, this routine turns on
* transmit interrupt and exits.  When the next BD is actually sent, an interrupt
* occurs.  The ISR does a netJobAdd of cpmStartOutput() to continue sending
* packets left in the interface output queue.  Once again, we find ourselves
* executing in the context of netTask().
*
* RETURNS: N/A
*/

#ifdef BSD43_DRIVER
LOCAL void cpmStartOutput
    (
    int		unit		/* unit number */
    )
    {
    DRV_CTRL *	pDrvCtrl = & drvCtrl[unit];
#else
LOCAL void cpmStartOutput
    (
    DRV_CTRL * 	pDrvCtrl	/* driver control structure */
    )
    {
#endif
    MBUF *	pMbuf;		/* mbuf pointer */
    int		length;
    SCC_BUF *	pTxBd;
    UINT8 *	pad;

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

            pDrvCtrl->ether.pSccReg->scce = SCC_ETHER_SCCX_TXB;
            pDrvCtrl->ether.pSccReg->sccm |= SCC_ETHER_SCCX_TXB;
	    break;
	    }

        IF_DEQUEUE (& pDrvCtrl->idr.ac_if.if_snd, pMbuf); /* dequeue a packet */

        copy_from_mbufs (pTxBd->dataPointer, pMbuf, length);

        /* padding mechanism in Rev A is buggy - do in software */

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

#ifndef BSD43_DRIVER    /* BSD 4.4 ether_output() doesn't bump statistic. */
            pDrvCtrl->idr.ac_if.if_opackets++;
#endif
            }
        }
    }

/*******************************************************************************
*
* cpmIoctl - network interface control routine
*
* This routine implements the network interface control functions.
* It handles SIOCSIFADDR and SIOCSIFFLAGS commands.
*
* RETURNS: OK if successful, otherwise EINVAL.
*/

LOCAL int cpmIoctl
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
            arpwhohas (pIfNet, &IA_SIN (data)->sin_addr);
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
* cpmReset - network interface reset routine
*
* This routine marks the interface as down and resets the device.  This
* includes disabling interrupts, stopping the transmitter and receiver,
* and calling sysCpmEnetDisable() to do any target specific disabling.
*
* The complement of this rotuine is cpmInit().  Once a unit is reset in this
* routine, it may be re-initialized to a running state by cpmInit().
*
* RETURNS: N/A
*/

LOCAL STATUS cpmReset
    (
    int		unit		/* unit number */
    )
    {
    DRV_CTRL * pDrvCtrl;

    if (unit < 0 || unit >= MAX_UNITS)	/* sanity check the unit number */
        return (ERROR);

    pDrvCtrl = & drvCtrl[unit];

    sysCpmEnetIntDisable (unit);	/* disable the SCC interrupts */

    /* issue the CP graceful stop command to the transmitter if necessary */

    if (pDrvCtrl->ether.pSccReg->gsmrl & SCC_GSMRL_ENT)
	{
	(void) sysCpmEnetCommand (unit, CPM_CR_SCC_GRSTOP);

        /* wait for graceful stop to register */

        /* wait for graceful stop to register */

        taskDelay (sysClkRateGet ());

        if (!(pDrvCtrl->ether.pSccReg->scce & SCC_ETHER_SCCX_GRA))
            return (ERROR);     /* Stop not registered. */
	}

    /* disable the SCC receiver and transmitter */

    pDrvCtrl->ether.pSccReg->gsmrl &= ~(SCC_GSMRL_ENR | SCC_GSMRL_ENT);

    /* call the BSP to do any other disabling (e.g., *TENA) */

    sysCpmEnetDisable (unit);

    /* mark the driver as down */

    pDrvCtrl->idr.ac_if.if_flags &= ~(IFF_UP | IFF_RUNNING);

    return (OK);
    }

/*******************************************************************************
*
* cpmIntr - network interface interrupt handler
*
* This routine gets called at interrupt level. It handles work that 
* requires minimal processing. Interrupt processing that is more 
* extensive gets handled at task level. The network task, netTask(), is 
* provided for this function. Routines get added to the netTask() work 
* queue via the netJobAdd() command.
*
* RETURNS: N/A
*/

LOCAL void cpmIntr
    (
    int		unit		/* unit number */
    )
    {
    DRV_CTRL * pDrvCtrl = & drvCtrl[unit];
    BOOL rxHandle = FALSE;
    BOOL txHandle = FALSE;

    int events;


    /* identify and acknowledge all interrupt events */

    events = (pDrvCtrl->ether.pSccReg->scce & pDrvCtrl->ether.pSccReg->sccm);

    /* check for spurious interrupt */

    if (!pDrvCtrl->attached)
	{
        pDrvCtrl->ether.pSccReg->scce = 0xffff;

	sysCpmEnetIntClear (unit);

	return;
	}

    /* handle receive events */

    if (events & SCC_ETHER_SCCX_RXF)
	{
        (void) netJobAdd ((FUNCPTR) cpmHandleInt, (int) pDrvCtrl, 0, 0, 0, 0); 

	/* turn off receive interrupts for now - cpmHandleInt turns back on */

        pDrvCtrl->ether.pSccReg->sccm &= ~SCC_ETHER_SCCX_RXF;
        rxHandle = TRUE;
        }

    /* check for output errors */

    if (pDrvCtrl->ether.pSccReg->scce & SCC_ETHER_SCCX_TXE)
	{
	/* NOTE: HBC error not supported -> always RESTART Tx here */

        (void) netJobAdd ((FUNCPTR) cpmTxRestart, unit, 0, 0, 0, 0);

	pDrvCtrl->txStop = TRUE;
	}

    /* handle transmitter events - BD full condition -> ever happen ? */

    if (events & SCC_ETHER_SCCX_TXB)
	{
#ifdef BSD43_DRIVER
        (void) netJobAdd ((FUNCPTR) cpmStartOutput, unit, 0, 0, 0, 0); 
#else
        (void) netJobAdd ((FUNCPTR) cpmStartOutput, (int)pDrvCtrl, 0, 0, 0, 0); 
#endif
        pDrvCtrl->ether.pSccReg->sccm &= ~SCC_ETHER_SCCX_TXB;
	}

    /* check for input busy condition, we don't enable this interrupt
       but we check for it with each interrupt. */

    if (pDrvCtrl->ether.pSccReg->scce & SCC_ETHER_SCCX_BSY)
	{
        pDrvCtrl->ether.pSccReg->scce = SCC_ETHER_SCCX_BSY; 

        /* count discarded frames as errors */

        pDrvCtrl->idr.ac_if.if_ierrors +=
	    ((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->disfc;
	((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->disfc = 0;
	}

    /* ignore and reset all other events */

    pDrvCtrl->ether.pSccReg->scce = (pDrvCtrl->ether.pSccReg->scce &
                                     ~(SCC_ETHER_SCCX_RXF |
				       SCC_ETHER_SCCX_TXE |
				       SCC_ETHER_SCCX_TXB |
				       SCC_ETHER_SCCX_BSY));

    if (rxHandle)
        pDrvCtrl->ether.pSccReg->scce = SCC_ETHER_SCCX_RXF;
    if (pDrvCtrl->txStop)
        pDrvCtrl->ether.pSccReg->scce = SCC_ETHER_SCCX_TXE;
    if (txHandle)
        pDrvCtrl->ether.pSccReg->scce = SCC_ETHER_SCCX_TXB;

    sysCpmEnetIntClear (unit);
    }

/*******************************************************************************
*
* cpmTxRestart - issue RESTART Tx command to the CP
*
* This routine issues a RESTART transmitter command to the CP.  It is
* executed by netTask (cpmIntr() did a netJobAdd).  cpmIntr() cannot do
* a RESTART directly because the CPM flag must be taken before a command
* can be written.  After a restart, a netJobAdd of cpmStartOutput() is
* performed in order to send any packets remaining in the interface output
* queue.
*
* RETURNS: N/A
*/

LOCAL void cpmTxRestart
    (
    int		unit		/* unit number */
    )
    {
    DRV_CTRL * pDrvCtrl = & drvCtrl[unit];

    /* update error counter */

    pDrvCtrl->idr.ac_if.if_oerrors++;

    /* correct packet counter */

    pDrvCtrl->idr.ac_if.if_opackets--;

    /* restart the transmitter */

    (void) sysCpmEnetCommand (unit, CPM_CR_SCC_RESTART);

    /* restart transmit routine */

    pDrvCtrl->txStop = FALSE;
#ifdef BSD43_DRIVER
    (void) netJobAdd ((FUNCPTR) cpmStartOutput, unit, 0, 0, 0, 0); 
#else
    (void) netJobAdd ((FUNCPTR) cpmStartOutput, (int)pDrvCtrl, 0, 0, 0, 0); 
#endif
    }

/*******************************************************************************
*
* cpmHandleInt - task-level interrupt handler
*
* This is the task-level interrupt handler, which is called from 
* netTask(). cpmHandleInt() gets input frames from the device and then calls 
* cpmRecv() to process each frame.  cpmRecv() only gets called if no error
* stats were reported in the buffer descriptor.  Data chaining is not
* supported.
*
* This routine should be called with SCC receive interrupts masked so that
* more netJobAdds of this routine are not performed by cpmIntr().
* Receive interrupts are turned back on by this routine before exiting.
*
* RETURNS: N/A
*/

LOCAL void cpmHandleInt
    (
    DRV_CTRL *	pDrvCtrl	/* pointer to DRV_CTRL structure */
    )
    {
    SCC_BUF *	pRxBd = & pDrvCtrl->ether.rxBdBase[pDrvCtrl->ether.rxBdNext];

    while (!(pRxBd->statusMode & SCC_ETHER_RX_BD_E))
        {
        /* data chaining is not supported - check all error conditions */

        if (((pRxBd->statusMode & (SCC_ETHER_RX_BD_F  | SCC_ETHER_RX_BD_L))
	                       == (SCC_ETHER_RX_BD_F  | SCC_ETHER_RX_BD_L))
	                      && !(pRxBd->statusMode & (SCC_ETHER_RX_BD_CL |
		                   SCC_ETHER_RX_BD_OV | SCC_ETHER_RX_BD_CR |
				   SCC_ETHER_RX_BD_SH | SCC_ETHER_RX_BD_NO |
				   SCC_ETHER_RX_BD_LG)))
            cpmRecv (pDrvCtrl, pRxBd);
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
* cpmRecv - process an input frame
*
* This routine processes an input frame, then passes it up to the higher 
* level in a form it expects.  Buffer loaning, promiscuous mode, and
* etherInputHookRtns are all supported.  Trailer protocols is not supported.
*
* RETURNS: N/A
*/

LOCAL void cpmRecv
    (
    DRV_CTRL *	pDrvCtrl,	/* pointer to DRV_CTRL structure */
    SCC_BUF *	pRxBd		/* receive buffer descriptor */
    )
    { 
    MBUF *	pMbuf = NULL;	/* MBUF pointer */
    UINT8 *	pData;
    int		length;
#ifdef BSD43_DRIVER
    UINT16	type;
#endif
    struct ether_header * 	pEh; 

    /* bump input packet counter */

    pDrvCtrl->idr.ac_if.if_ipackets++;

    /* call hook if one is connected */

    if ((etherInputHookRtn != NULL) && ((*etherInputHookRtn)
	(& pDrvCtrl->idr.ac_if, pRxBd->dataPointer, pRxBd->dataLength) != 0))
        return; 

    /* get Ethernet header */

    pEh = (struct ether_header *) pRxBd->dataPointer;

    /* filter frame if controller is in promiscuous mode - M bit is buggy */

    if (pDrvCtrl->ether.pSccReg->psmr & SCC_ETHER_PSMR_PRO)
	{
	if ((bcmp ( (char *) pEh->ether_dhost,
                    (char *) pDrvCtrl->idr.ac_enaddr, ENET_ADDR_SIZE)) &&
	    (bcmp ((char *) pEh->ether_dhost,
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

    /* point to data */

    pData = ((char *)pEh) + SIZEOF_ETHERHEADER;

#ifdef BSD43_DRIVER
    /* save type - build_cluster trashes the type field */

    type = pEh->ether_type;
#endif

    /* OK to loan out buffer ? -> build a mbuf cluster */

    if ((pDrvCtrl->nLoanRxBd > 0) && (USE_CLUSTER (length)))
	pMbuf = build_cluster (pData, length, & pDrvCtrl->idr, MC_CPM,
	    pDrvCtrl->pRefCnt[(pDrvCtrl->nLoanRxBd - 1)], cpmLoanFree,
	    (int) pDrvCtrl, (int) pEh,
            (int) pDrvCtrl->pRefCnt[(pDrvCtrl->nLoanRxBd - 1)]);

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
    do_protocol (pEh, pMbuf, & pDrvCtrl->idr, length);
#endif
    }

/*******************************************************************************
*
* cpmLoanFree - return the given buffer to loaner pool
*
* This routine returns <pRxBuf> to the pool of available loaner buffers.
* It also returns <pRef> to the pool of available loaner reference counters,
* then zeroes the reference count.
*
* RETURNS: N/A
*/

LOCAL void cpmLoanFree
    (
    DRV_CTRL *	pDrvCtrl,
    UINT8 *	pRxBuf,
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
