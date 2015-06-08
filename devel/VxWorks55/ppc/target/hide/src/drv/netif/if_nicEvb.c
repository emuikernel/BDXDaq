/* if_nicEvb.c - National Semiconductor ST-NIC Chip network interface driver */

/* Copyright 1984-1997 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
03i,05may97,tam  re-work driver to improve reliability and performances:
                 modified receive interrupt processing and error handling,
                 fixed bug in nicTxStartup(), added function nicPagePtrUpdate()
		 and nicResetDelay().
03h,21apr97,mkw  reset count to 0 before timeout check in nicTransmit and
		 set TRANS_TIMOUT value to 400.
03g,18apr97,mkw  Fixed nicTransmit to use count value.
03g,15jul97,spm  added ARP request to SIOCSIFADDR ioctl handler
03f,05mar97,map  added sysEnetAddrGet() call to fetch MAC address (SPR# 7745)
03f,07apr97,spm  code cleanup, corrected statistics, and upgraded to BSD 4.4
03e,10dec96,tpr  removed intEnable() prototype. Defined in intLib.h now.
03d,18sep96,p_m  added SYNC definition for CPUs other than PPC
03c,08aug96,tam  added code to synchronize i/o acess in nicWritePort().
03b,03mar96,tam  replaced sysIntEnablePIC by intEnable().
03a,02feb96,dzb  re-worked many parts of driver. implemented Rx buffer-loaning.
                 fixed nicRestart to recover gracefully from DMA overflow.
		 re-worked Tx code to send directly from mbuf chain.
		 changed to turn off Rx interrupts, instead of an active flag.
		 fixed nicRead() to flush extra DMA-byte on Rx error.
02d,11jul95,amp  used intLock(), implemented nicEnetAddrGet(), cleanup.
02c,29jun95,caf  changed name to "nicEvb".
02b,27jun95,amp  improved reliability.
02a,16jun95,amp  modified if_nic.c for EVB403.
01z,11aug93,jmm  Changed ioctl.h and socket.h to sys/ioctl.h and sys/socket.h
01y,19feb93,jdi  documentation cleanup.
01x,15oct92,rfs  Added documentation.
01w,02oct92,rfs  Made multiple attach calls per unit return OK.
01v,09sep92,gae  documentation tweaks.
01u,02sep92,ajm  added conditional compilation for __STDC__ (DEC)
01t,18aug92,rfs  Revised to follow style of other drivers.
                 Added cache support.  Reworked OVW error recovery.
01s,23jun92,ajm  fixed ansi warnings for pktBufRead calls
                 went back and fixed all 68k ansi warnings
01r,26may92,rrr  the tree shuffle
                 -changed includes to have absolute path from h/
01q,26may92,ajm  got rid of HOST_DEC def's (new compiler)
                 updated copyright
01p,10oct91,rfs  modified pktBufRead() & pktBufWrite() to return if len is zero
01o,04oct91,rrr  passed through the ansification filter
                 -changed functions to ansi style
                 -changed includes to have absolute path from h/
                 -changed VOID to void
                 -changed copyright notice
01n,28sep91,ajm  ifdef'd HOST_DEC for compiler problem
01m,02Aug91,rfs  cleanup of transmit design, int handler, NIC command register
                 was being handled incorrectly, DMA operations required
                 semaphoring and error checking, changed names, deleted
                 nicdelay(), deleted excessive include files. Provided correct
                 OVW recovery.  Your basic rewrite.
01l,01may91,elh  fixed transmit buffering problems, added changes from
                 heurikon, removed DMAState, XmtByteCount.
01k,20sep90,dab  made nicInit() return int.
01j,10aug90,dnw  added forward declarations of void routines.
                 added include of if_subr.h.
01i,11jul90,hjb  removed code that uses gotIp.  made nicRead() a void function.
                 changed references to do_protocol() to use
                 do_protocol_with_type().
01h,19apr90,hjb  deleted param.h, de-linted.
01g,18mar90,hjb  reduction of redundant code and addition of cluster support.
                 added error recording (TXE, RXE), removed unnecessary
                 error handler which didn't do anything.  delinted.
01f,18sep89,cwp  added multiple transmit buffer support
01e,14sep89,dab  changed iv68k.h to iv.h.
01d,15jun89,cwp  fixed ring buffer overflow and transmit errors.
01c,04apr89,cwp  first working version.
01b,25feb89,cwp  added changes from unix driver.
01a,12jan89,cwp  written.
*/

/*
This module implements the National Semiconductor 83902A ST-NIC Ethernet
network interface driver.

This driver is non-generic and is for use on the IBM EVB403 board.  
Only unit number zero is supported.  The driver must be given several 
target-specific parameters.  These parameters, and the mechanisms 
used to communicate them to the driver, are detailed below.

BOARD LAYOUT
This device is on-board.  No jumpering diagram is necessary.

EXTERNAL INTERFACE
This driver provides the standard external interface with the following
exceptions.  All initialization is performed within the attach routine; there
is no separate initialization routine.  Therefore, in the global interface
structure, the function pointer to the initialization routine is NULL.

The only user-callable routine is nicEvbattach(), which publishes the `nicEvb'
interface and initializes the driver and device.

TARGET-SPECIFIC PARAMETERS
.iP "device I/O address"
This parameter is passed to the driver by nicEvbattach().
It specifies the base address of the device's I/O register
set.
.iP "interrupt vector"
This parameter is passed to the driver by nicEvbattach().
It specifies the interrupt vector to be used by the driver
to service an interrupt from the ST-NIC device.  The driver will connect
the interrupt handler to this vector by calling intConnect().
.iP "device restart/reset delay"
The global variable nicRestartDelay (UINT32), defined in this file, 
should be initialized in the BSP sysHwInit() routine. nicRestartDelay is 
used only with PowerPC platform and is equal to the number of time base 
increments which makes for 1.6 msec. This corresponds to the delay necessary 
to respect when restarting or resetting the device.
.LP

EXTERNAL SUPPORT REQUIREMENTS

The driver requires the following support functions:

.iP "STATUS sysEnetAddrGet (int unit, UINT8 * addr)" "" 9 -1
The driver expects this routine to provide the six-byte Ethernet hardware
address that will be used by <unit>.  This routine must copy the six-byte
address to the space provided by <addr>.  This routine is expected to return
OK on success, or ERROR.  The driver calls this routine, during device
initialization, from the nicEnetAddrGet() routine.
.LP

SYSTEM RESOURCE USAGE
When implemented, this driver requires the following system resources:

    - one mutual exclusion semaphore
    - one interrupt vector

SEE ALSO: ifLib
*/

/* includes */

#include "vxWorks.h"
#include "iv.h"
#include "stdlib.h"
#include "intLib.h"
#include "netLib.h"

#ifdef NIC_DEBUG
#include "logLib.h"
#endif

#include "taskLib.h"
#include "logLib.h"
#include "net/mbuf.h"
#include "sys/ioctl.h"
#include "errno.h"
#include "net/if.h"
#include "net/unixLib.h"
#include "net/if_subr.h"
#include "netinet/in_var.h"
#include "netinet/if_ether.h"
#include "etherLib.h"
#include "semLib.h"
#include "drv/netif/netifDev.h"
#include "drv/netif/if_nicEvb.h"
#include "vxLib.h"
#include "private/funcBindP.h"

/* defines */

#if	(CPU_FAMILY == PPC)
#define	NIC_USE_PPCTIMEBASE
#else   /* CPU_FAMILY == PPC */
#undef	NIC_USE_PPCTIMEBASE
#endif	/* CPU_FAMILY == PPC */

#define MAX_UNITS		1	/* max number of units supported */
#define TRANS_TIMEOUT		400	/* times to poll for end of Tx */
#define L_POOL          	0x4	/* Rx loan buffers in pool */

/* debug macros */

#undef	NIC_INSTRUMENT			/* instrument the driver */
#undef	NIC_DEBUG			/* log debug messages */

/* 
 * NIC_FASTER, if defined, improved the driver throuput, however the driver
 * seems to be less reliable. Left undefined for now.
 */

#undef	NIC_FASTER			/* improve throuput but less stable */

/* typedefs */

typedef struct mbuf MBUF;
typedef struct arpcom IDR;		/* Interface Data Record wrapper */
typedef struct ifnet IFNET;		/* real Interface Data Record */
typedef struct sockaddr SOCK;
typedef struct ether_header ETH_HDR;

typedef struct rx_hdr
    {
    UINT8		status;		/* status of packet */
    UINT8		nextRxPage;	/* page next pkt starts at */
    UINT8		cntL;		/* low byte of frame length */
    UINT8		cntH;		/* high byte of frame length */
    } RX_HDR;

#define RX_HDR_SIZ		sizeof(RX_HDR)

typedef struct rx_frame
    {
    UINT16		pad1;
    RX_HDR		rxHdr;		/* receive status header */
    ETH_HDR		enetHdr;	/* ethernet header */
    UINT8		data [ETHERMTU];/* frame data */
    UINT32		fcs;		/* frame check sequence */
    UINT8		refCnt;		/* loaner reference count */
    UINT8		pad2;
    } RX_FRAME;

#define RX_FRAME_SIZ		sizeof(RX_FRAME)

typedef struct drv_ctrl			/* driver control structure */
    {
    IDR                 idr;		/* interface data record */
    BOOL                attached;	/* indicates unit is attached */
    int                 nicIntVec;	/* interrupt vector */
    SEM_ID              dmaSem;		/* exclusive use of remote DMA */
    UINT8		nextPkt;	/* NIC buf page for next pkt */
    NIC_DEVICE *	nicAddr;	/* address of NIC chip */
    RX_FRAME *		pRxFrame;	/* Rx frame memory */
    RX_FRAME *		lPool [L_POOL];	/* Rx loaner pool */
    int			lIndex;		/* Rx loaner pool index */
    } DRV_CTRL;

/* globals */

UINT32  nicRestartDelay = 30000;/* number of time base ticks to wait for */
				/* when resetting the chip */
#ifdef	NIC_INSTRUMENT
UINT32	nicRestartNb = 0;	/* number of restart due to ring overflow */
UINT32	nicIntNb = 0;		/* number of receive interrupt */
UINT32	nicLen = 0;		/* lenght of the current received packet */
UINT32	nicHdrStat = 0;		/* status byte of the current received packet */
UINT32	nicNextPage = 0;	/* page pointer to the next received packet */
UINT32	nicCurrentPage = 0;	/* start page of the current packet */
UINT32	nicTxNb = 0;		/* number of transmitted packets */
UINT32	nicTxTimeout = 0;	/* number of transmit time-out */
UINT32	nicInitNb = 0;		/* number of time device is re-initialized */
#endif

IMPORT STATUS	sysEnetAddrGet (int unit, UINT8 * addr);

/* locals */

LOCAL DRV_CTRL drvCtrl [MAX_UNITS];

/* forward declarations */

#ifdef __STDC__

LOCAL void	nicReset (int unit);
LOCAL void	nicIntr (int unit);
LOCAL void	nicRestart (DRV_CTRL *pDrvCtrl, UINT8 cr);
LOCAL void	nicHandleInt (DRV_CTRL *pDrvCtrl);
LOCAL BOOL	nicRead (DRV_CTRL *pDrvCtrl);
#ifdef BSD43_DRIVER
LOCAL int	nicOutput (IDR *pIDR, MBUF *pMbuf, SOCK *pDest);
#endif
LOCAL int	nicIoctl (IDR *pIDR, int cmd, caddr_t data);
LOCAL void	nicEnetAddrGet (int unit);
LOCAL void	nicConfig (int unit);
LOCAL STATUS	nicPktBufRead (DRV_CTRL *pDrvCtrl, UINT32 nicBufAddr,
		    UINT32 len, char *pData);
LOCAL void	nicWriteCr (NIC_DEVICE *pNic, UINT8 value);
LOCAL void	nicWriteReg (NIC_DEVICE *pNic, volatile UINT8 *pReg,
		    UINT8 value, UINT8 page);
LOCAL UINT8	nicReadReg (NIC_DEVICE *pNic, volatile UINT8 *pReg, char page);
LOCAL STATUS	nicTransmit (DRV_CTRL *pDrvCtrl, MBUF *pMbuf, int len,
		    BOOL waitFlag);
LOCAL void	nicWritePort (UINT8 value);
LOCAL STATUS	nicReceive (DRV_CTRL *pDrvCtrl, UINT32 nicBufAddr, char *pData,
		    int len);
#ifdef BSD43_DRIVER
LOCAL STATUS	nicTxStartup(int unit);
#else
LOCAL STATUS	nicTxStartup(DRV_CTRL *pDrvCtrl);
#endif
LOCAL UINT8	nicReadPort (void);
LOCAL void	nicLoanFree (DRV_CTRL *pDrvCtrl, RX_FRAME *pRx);
LOCAL BOOL	nicPagePtrUpdate (DRV_CTRL * pDrvCtrl);
LOCAL void	nicResetDelay (void);

#else  /* __STDC__ */

LOCAL void	nicReset ();
LOCAL void	nicIntr ();
LOCAL void	nicRestart ();
LOCAL void	nicHandleInt ();
LOCAL BOOL	nicRead ();
LOCAL int	nicOutput ();
LOCAL int	nicIoctl ();
LOCAL void	nicEnetAddrGet ();
LOCAL void	nicConfig ();
LOCAL STATUS	nicPktBufRead ();
LOCAL void	nicWriteCr ();
LOCAL void	nicWriteReg ();
LOCAL UINT8	nicReadReg ();
LOCAL STATUS	nicTransmit ();
LOCAL void	nicWritePort ();
LOCAL STATUS	nicReceive ();
LOCAL STATUS	nicTxStartup();
LOCAL UINT8	nicReadPort ();
LOCAL void	nicNicFree ();
LOCAL BOOL	nicPagePtrUpdate ();
LOCAL void	nicResetDelay () ;

#endif  /* __STDC__ */

/*******************************************************************************
*
* nicEvbattach - publish and initialize the `nicEvb' network interface driver
*
* This routine publishes the `nicEvb' interface by filling in a network
* interface record and adding this record to the system list.  It also
* initializes the driver and the device to the operational state.
*
* RETURNS: OK, or ERROR if the receive buffer memory could not be allocated.
*/

STATUS nicEvbattach
    (
    int			unit,			/* unit number */
    NIC_DEVICE *	pNic,			/* address of NIC chip */
    int			ivec			/* interrupt vector to use */
    )
    {
    DRV_CTRL *		pDrvCtrl;		/* driver control struct */
    RX_FRAME *		pPool;			/* receive memory pool */
    int			ix;			/* counter */

    if (unit < 0 || unit >= MAX_UNITS)	/* sanity check the unit number */
        return (ERROR);

    pDrvCtrl = &drvCtrl [unit];

    if (pDrvCtrl->attached)		/* ensure single invocation */
        return (OK);

    if ((pPool = (RX_FRAME *) malloc ((L_POOL + 1) * RX_FRAME_SIZ)) == NULL)
        return (ERROR);

    for (ix = 0; ix < L_POOL; ix++)	/* assign addresses of Rx pool */
	pDrvCtrl->lPool [ix] = pPool++;

    pDrvCtrl->lIndex = L_POOL;		/* initialize loaner pool index */
    pDrvCtrl->pRxFrame = pPool;		/* link in primary buffer */
    pDrvCtrl->nicAddr = pNic;		/* save NIC address */
    pDrvCtrl->nicIntVec = ivec;		/* save int vector number */
    pDrvCtrl->nextPkt = CURR;		/* reset to initial value */

    pDrvCtrl->dmaSem = semMCreate (SEM_Q_PRIORITY);

    /* publish the interface data record */

#ifdef BSD43_DRIVER
    ether_attach (&pDrvCtrl->idr.ac_if, unit, "nicEvb", (FUNCPTR) NULL,
        (FUNCPTR) nicIoctl, (FUNCPTR) nicOutput, (FUNCPTR) nicReset);
#else
    ether_attach (
                 &pDrvCtrl->idr.ac_if, 
                 unit, 
                 "nicEvb", 
                 (FUNCPTR) NULL,
                 (FUNCPTR) nicIoctl, 
                 (FUNCPTR) ether_output, 
                 (FUNCPTR) nicReset
                 );
    pDrvCtrl->idr.ac_if.if_start = (FUNCPTR)nicTxStartup;
#endif
 
    /* connect the interrupt handler */

    (void) intConnect (INUM_TO_IVEC (ivec), nicIntr, 0); 
    intEnable (ivec); 

    nicConfig (unit);			/* configure the device */

    /* raise the interface flags */

    pDrvCtrl->idr.ac_if.if_flags |= IFF_UP | IFF_RUNNING | IFF_NOTRAILERS;

    pDrvCtrl->attached = TRUE;

    return (OK);
    }

/*******************************************************************************
*
* nicReset - reset of interface
*
* This routine performs a software reset of the ST-NIC device.
*
* RETURNS: N/A.
*
* NOMANUAL
*/

void nicReset
    (
    int			unit			/* device unit to reset */
    )
    {
    DRV_CTRL *		pDrvCtrl = &drvCtrl [unit];
    NIC_DEVICE *	pNic = pDrvCtrl->nicAddr;

    pDrvCtrl->idr.ac_if.if_flags = 0;
    nicWriteCr (pNic, STP | ABORT | RPAGE0);

    taskDelay (2);			/* wait at least 1.6 mSec */
    }

/*******************************************************************************
*
* nicIntr - The driver's interrupt handler
*
* This function clears the cause of the device interrupt(s) and then acts
* on the individual possible causes.  The primary goal of this routine is to
* minimize the time spent in it.  This is accomplished by deferring processing
* to the netTask via the netJobAdd() function.
*
* Note that in case the receiver overruns, we promptly mark the interface as
* "down" and leave error handling to task-level.   This is in case netTask
* is in the midst of DMA activity, we must allow it to complete.  The receive
* handler will give up when it discovers the interface is down, which will
* then allow netTask to run our OVW handler.  This provides a nice orderly
* error recovery.
*
* RETURNS: N/A.
*/

LOCAL void nicIntr
    (
    int			unit
    )
    {
    DRV_CTRL *		pDrvCtrl = &drvCtrl [unit];
    NIC_DEVICE *	pNic = pDrvCtrl->nicAddr;	/* NIC registers */
    UINT8		isr;				/* copy of ISR */
    UINT8		cr;				/* copy of CR */

#ifdef	NIC_INSTRUMENT
    nicIntNb++;
#endif
    isr = nicReadReg(pNic, &pNic->Isr, RPAGE0);
    nicWriteReg(pNic, &pNic->Isr, isr, RPAGE0);

    /* handle receiver overrun */

    if  ((isr & OVW) && (pDrvCtrl->idr.ac_if.if_flags & (IFF_UP | IFF_RUNNING)))
	{
	cr = nicReadReg (pNic, &pNic->Cr, RPAGE0);

	nicWriteReg(pNic, &pNic->Imr, 0, RPAGE0); /* disable all interrupts */

        pDrvCtrl->idr.ac_if.if_flags &= ~(IFF_UP | IFF_RUNNING);

	netJobAdd ((FUNCPTR) nicRestart, (int) pDrvCtrl, cr, 0, 0, 0);
        return;
	}

    /* handle packet received */

    if  ((isr & PRX) && (pDrvCtrl->idr.ac_if.if_flags & (IFF_UP | IFF_RUNNING)))
	netJobAdd ((FUNCPTR) nicHandleInt, (int) pDrvCtrl, 0, 0, 0, 0);
    }

/*******************************************************************************
*
* nicRestart - restart chip after receive ring buffer overflow
*
* This routine is the task-level handler that deals with a receive DMA
* overflow condition.  It gets access to the remote DMA, cleans up NIC
* registers, empties the receive ring buffers, and then resends any
* packet that was in the midst of transmission when the overflow hit.
*
* RETURNS: N/A.
*/

LOCAL void nicRestart
    (
    DRV_CTRL *		pDrvCtrl,
    UINT8		cr
    )
    {
    NIC_DEVICE *	pNic = pDrvCtrl->nicAddr;	/* NIC registers */
    BOOL		resend = FALSE;

#ifdef	NIC_INSTRUMENT
    nicRestartNb++;
#endif
    nicWriteCr (pNic, STP);

    nicResetDelay ();				/* wait at least 1.6 mSec */

    if (semTake(pDrvCtrl->dmaSem, 100) == ERROR)
	{
#ifdef NIC_DEBUG
	if (_func_logMsg != NULL)
	    _func_logMsg ("nicRestart: can't obtain dmaSem\n", 0, 0, 0, 0, 0,0);

#endif
        semTake (pDrvCtrl->dmaSem, WAIT_FOREVER);
        }

    nicWriteReg (pNic, &pNic->Rbcr0, 0, RPAGE0);
    nicWriteReg (pNic, &pNic->Rbcr1, 0, RPAGE0);

    if ((cr & TXP) && (!(nicReadReg (pNic, &pNic->Isr, RPAGE0) & (TXE | PTX))))
	resend = TRUE;

    nicWriteReg (pNic, &pNic->Tcr, MODE1, RPAGE0);
    nicWriteCr (pNic, RPAGE0 | ABORT | STA);		/* back to page 0 */

    pDrvCtrl->idr.ac_if.if_flags |= IFF_UP | IFF_RUNNING | IFF_NOTRAILERS;

    while (nicRead (pDrvCtrl))
	;

    nicWriteReg(pNic, &pNic->Isr, OVW, RPAGE0);
    nicWriteReg (pNic, &pNic->Tcr, MODE0, RPAGE0);

    if (resend)
        nicWriteCr (pNic, RPAGE0 | STA | TXP | ABORT);

    semGive(pDrvCtrl->dmaSem);

    nicWriteReg (pNic, &pNic->Imr, PRXE | OVWE, RPAGE0);
    }

/*******************************************************************************
*
* nicHandleInt - deferred receive interrupt handler
*
* This function handles the received frames from the device.  It runs in the
* context of the netTask, which was triggered by a received packet interrupt.
* Actual processing of the packet is done by calling nicRead().
*
* RETURNS: N/A.
*/

LOCAL void nicHandleInt
    (
    DRV_CTRL *		pDrvCtrl
    )
    {
    /* empties the receive ring buffer of its packets */

    while (nicRead (pDrvCtrl));
    }

/*******************************************************************************
*
* nicRead - read a packet off the interface ring buffer
*
* nicRead copies packets from local memory into an mbuf and hands it to
* the next higher layer (IP or etherInputHook).
*
* RETURNS: TRUE, or FALSE if the packet reception encountered errors.
*/

LOCAL BOOL nicRead
    (
    DRV_CTRL *		pDrvCtrl
    )
    {
    NIC_DEVICE *	pNic = pDrvCtrl->nicAddr;/* NIC registers */
    RX_FRAME *		pRx = pDrvCtrl->pRxFrame;/* free Rx buffer */
    MBUF *		pMbuf = NULL;
    UINT32		len;			/* len of Rx pkt */
#ifdef BSD43_DRIVER
    UINT16		type;			/* packet type - saved */
#endif
    int			cur;

    if (!(pDrvCtrl->idr.ac_if.if_flags & (IFF_UP | IFF_RUNNING)) ||
        (pDrvCtrl->nextPkt == (cur = nicReadReg (pNic, &pNic->Curr, RPAGE1))))
	return (FALSE);

    /*
     * OK, there is work to be done.
     * First we copy the NIC receive status header from the NIC buffer
     * into our local area. This is done so that we can obtain the length
     * of the packet before copying out the rest of it. Note that the length
     * field in the NIC header includes the Ethernet header, the data, and
     * the 4 byte FCS field.
     */

    if (nicPktBufRead (pDrvCtrl, pDrvCtrl->nextPkt << 8, (RX_HDR_SIZ),
	(char *) &pRx->rxHdr) == ERROR)
        {
#ifdef	NIC_DEBUG
	if (_func_logMsg != NULL)
	    _func_logMsg("nicRead could not read packet header\n",0,0,0,0,0,0);
#endif
	return (FALSE);
	}

    len = pRx->rxHdr.cntL + (pRx->rxHdr.cntH << 8) - 4 /* FCS */;

    /* valid frame checks */

    /*
     * NOTE: if the packet's receive status byte indicates an error
     * the packet is discarded and the receive page pointers are updated to
     * point to the next packet.
     */

#ifdef	NIC_INSTRUMENT
    nicLen = len;
    nicHdrStat = pRx->rxHdr.status;
    nicNextPage = pRx->rxHdr.nextRxPage;
    nicCurrentPage = cur;
#endif

    if ((len < 60) || (len > 1514) || ((pRx->rxHdr.status & ~PHY) != PRX))
	{
	pDrvCtrl->idr.ac_if.if_ierrors++;

#ifdef	NIC_DEBUG
	if (_func_logMsg != NULL)
	    _func_logMsg ("nicRead receive error: statusHeader=0x%x nextRxPage=%d currentPage=%d len=%d IntNb=%d\n",pRx->rxHdr.status, pRx->rxHdr.nextRxPage, cur,len,nicIntNb);
#endif  
	if (nicPagePtrUpdate (pDrvCtrl))
	    return (TRUE);
	else
	    return (FALSE);
	}

    /* copy Ethernet packet section of the frame */

    if (nicPktBufRead(pDrvCtrl, (pDrvCtrl->nextPkt << 8) + RX_HDR_SIZ, len,
	(char *) &pRx->enetHdr) == ERROR)
	{
#ifdef	NIC_DEBUG
	if (_func_logMsg != NULL)
	    _func_logMsg ("nicRead: Could not read packet data\n",0,0,0,0,0,0);
#endif
	if (nicPagePtrUpdate(pDrvCtrl))
	    return (TRUE);
	else
	    return (FALSE);
	}

    /* update ring buffer/page pointers */

    if (!nicPagePtrUpdate (pDrvCtrl))
	{
	return (FALSE);
	}
    pDrvCtrl->idr.ac_if.if_ipackets++;


    if ((etherInputHookRtn != NULL) &&		/* call hook if installed */
        ((* etherInputHookRtn) (&pDrvCtrl->idr.ac_if, (char *) &pRx->enetHdr,
        len)))
        return (TRUE);

    len -= sizeof (ETH_HDR);            /* now equals length of data only */
#ifdef BSD43_DRIVER
    type = pRx->enetHdr.ether_type;	/* build_cluster trashes type field */
#endif

    if ((pDrvCtrl->lIndex) && (USE_CLUSTER (len)))	/* use loaning ? */
	{
	pRx->refCnt = 0;
        pMbuf = build_cluster (pRx->data, len, &pDrvCtrl->idr, MC_NIC_EVB,
            &pRx->refCnt, nicLoanFree, (int) pDrvCtrl, (int) pRx, 0);
	}

    if (pMbuf != NULL)					/* loaning worked ? */
        pDrvCtrl->pRxFrame = pDrvCtrl->lPool [--pDrvCtrl->lIndex];
    else
        {
        pMbuf = copy_to_mbufs (pRx->data, len, 0, &pDrvCtrl->idr.ac_if);
        if (pMbuf == NULL)
            {
            pDrvCtrl->idr.ac_if.if_ierrors++;
            return (TRUE);
            }
        }

#ifdef BSD43_DRIVER
    do_protocol_with_type (type, pMbuf, &pDrvCtrl->idr, len);	/* up to IP */
#else
    do_protocol (&pRx->enetHdr, pMbuf, &pDrvCtrl->idr, len);
#endif

    return (TRUE);
    }

#ifdef BSD43_DRIVER
/*******************************************************************************
*
* nicOutput - the driver's output routine
*
* RETURNS: N/A.
*/

LOCAL int nicOutput
    (
    IDR  *		pIDR,
    MBUF *		pMbuf,
    SOCK *		pDest
    )
    {
    return ether_output ((IFNET *) pIDR, pMbuf, pDest, (FUNCPTR) nicTxStartup,
	pIDR);
    }
#endif

/*******************************************************************************
*
* nicTxStartup - the driver's actual output routine
*
* This routine accepts outgoing packets from the if_snd queue, and then 
* gains exclusive access to the DMA (through a mutex semaphore),
* then calls nicTransmit() to send the packet out onto the interface.
*
* RETURNS: OK, or ERROR if the packet could not be transmitted.
*/

#ifdef BSD43_DRIVER
LOCAL STATUS nicTxStartup
    (
    int			unit
    )
    {
    DRV_CTRL * 		pDrvCtrl = &drvCtrl [unit];
#else
LOCAL STATUS nicTxStartup
    (
    DRV_CTRL * 	pDrvCtrl
    )
    {
#endif
    MBUF *		pMbuf;
    MBUF *		mbuf;
    int			status = OK;
    int			length = 0;

    while (pDrvCtrl->idr.ac_if.if_snd.ifq_head)
	{
        IF_DEQUEUE (&pDrvCtrl->idr.ac_if.if_snd, pMbuf);/* dequeue a packet */

        if ((pDrvCtrl->idr.ac_if.if_flags & (IFF_UP | IFF_RUNNING)) !=
            (IFF_UP | IFF_RUNNING))
            return (ERROR);

        if (semTake (pDrvCtrl->dmaSem, 100) == ERROR)	/* get DMA access */
            {
	    if (_func_logMsg != NULL)
		_func_logMsg("nicTxStartup: can't obtain dmaSem\n",0,0,0,0,0,0);
            semTake (pDrvCtrl->dmaSem, WAIT_FOREVER);
            }

	for (mbuf = pMbuf; mbuf != NULL; mbuf = mbuf->m_next)
	    length += mbuf->m_len;		/* find length of chain */

        if (length == 0)
	    {
	    m_freem (pMbuf);			/* empty mbuf chain */
	    semGive (pDrvCtrl->dmaSem);
	    continue;
	    }

#ifndef BSD43_DRIVER    /* BSD 4.4 ether_output() doesn't bump statistic */
        pDrvCtrl->idr.ac_if.if_opackets++;
#endif
#ifdef	NIC_INSTRUMENT
	nicTxNb++;
#endif

	/* send packet out over interface */

        if ( (status = nicTransmit (pDrvCtrl, pMbuf, length, TRUE)) == ERROR)
            {
            pDrvCtrl->idr.ac_if.if_opackets--;
            pDrvCtrl->idr.ac_if.if_oerrors++;
            }

        semGive (pDrvCtrl->dmaSem);
	}

    return status;
    }

/*******************************************************************************
*
* nicIoctl - the driver's I/O control routine
*
* Perform device-specific commands.
*
* RETURNS: 0, or EINVAL if the command 'cmd' is not supported.
*/

LOCAL int nicIoctl
    (
    IDR *		pIDR,
    int			cmd,			/* command to execute */
    caddr_t		data			/* command-specific data */
    )
    {
    int			error = 0;
    int			s = splimp ();

    switch (cmd)
	{
        case SIOCSIFADDR:
            ((struct arpcom *)pIDR)->ac_ipaddr = IA_SIN (data)->sin_addr;
            arpwhohas (pIDR, &IA_SIN (data)->sin_addr);
            break;

        case SIOCSIFFLAGS:	/* no driver-dependent flags */
            break;

        default:
            error = EINVAL;
	}

    splx (s);
    return (error);
    }

/*******************************************************************************
*
* nicEnetAddrGet - get the Ethernet address.
*
* Get ethernet address from the BSP.
*
* RETURNS: N/A.
*/

LOCAL void nicEnetAddrGet
    (
    int			unit
    )
    {
    DRV_CTRL *		pDrvCtrl = & drvCtrl [unit];
    UINT8		enetAdrs[6];
    UINT8		count;

    sysEnetAddrGet (unit, enetAdrs);
    for (count=0; count<6; count++)
        pDrvCtrl->idr.ac_enaddr[count] = enetAdrs[5-count];
        
    }

/*******************************************************************************
*
* nicConfig - configure the NIC chip and program address
*
* This routine follows the algorythm in the ST-NIC manual for enabling
* a NIC device on an active network.  Essentially, this routine initializes
* the NIC device.
*
* RETURNS: N/A.
*/

LOCAL void nicConfig
    (
    int			unit
    )
    {
    DRV_CTRL *		pDrvCtrl = &drvCtrl [unit];
    NIC_DEVICE *	pNic= pDrvCtrl->nicAddr;
    
    nicWriteCr (pNic, RPAGE0 | STP | ABORT);

    nicResetDelay ();

    nicWriteReg (pNic, &pNic->Dcr, NOTLS | FIFO8, RPAGE0);

    /* clear remote DMA byte count registers */

    nicWriteReg (pNic, &pNic->Rbcr0, 0, RPAGE0);             
    nicWriteReg (pNic, &pNic->Rbcr1, 0, RPAGE0);

    /* accept broadcast, but not runt or multicast */

    nicWriteReg (pNic, &pNic->Rcr, AB, RPAGE0);         
    nicWriteReg (pNic, &pNic->Tcr, MODE1, RPAGE0);	/* int loopback mode */

    nicWriteReg (pNic, &pNic->Pstart, PSTART, RPAGE0);
    nicWriteReg (pNic, &pNic->Pstop, PSTOP, RPAGE0);
    nicWriteReg (pNic, &pNic->Bnry, BNRY, RPAGE0);

    nicWriteReg (pNic, &pNic->Isr, 0xff, RPAGE0);	/* clr pending ints */
    nicWriteReg (pNic, &pNic->Imr, PRXE | OVWE, RPAGE0); /* enable int */

    nicEnetAddrGet (unit);			/* get enet address */

    /* set up page 1 registers */

    nicWriteReg (pNic, &pNic->Par0, pDrvCtrl->idr.ac_enaddr [0], RPAGE1);
    nicWriteReg (pNic, &pNic->Par1, pDrvCtrl->idr.ac_enaddr [1], RPAGE1);
    nicWriteReg (pNic, &pNic->Par2, pDrvCtrl->idr.ac_enaddr [2], RPAGE1);
    nicWriteReg (pNic, &pNic->Par3, pDrvCtrl->idr.ac_enaddr [3], RPAGE1);
    nicWriteReg (pNic, &pNic->Par4, pDrvCtrl->idr.ac_enaddr [4], RPAGE1);
    nicWriteReg (pNic, &pNic->Par5, pDrvCtrl->idr.ac_enaddr [5], RPAGE1);
    nicWriteReg (pNic, &pNic->nic_pg1.mar0, 0xff, RPAGE1);
    nicWriteReg (pNic, &pNic->nic_pg1.mar1, 0xff, RPAGE1);
    nicWriteReg (pNic, &pNic->nic_pg1.mar2, 0xff, RPAGE1);
    nicWriteReg (pNic, &pNic->nic_pg1.mar3, 0xff, RPAGE1);
    nicWriteReg (pNic, &pNic->nic_pg1.mar4, 0xff, RPAGE1);
    nicWriteReg (pNic, &pNic->nic_pg1.mar5, 0xff, RPAGE1);
    nicWriteReg (pNic, &pNic->nic_pg1.mar6, 0xff, RPAGE1);
    nicWriteReg (pNic, &pNic->nic_pg1.mar7, 0xff, RPAGE1);
    nicWriteReg (pNic, &pNic->Curr, CURR, RPAGE1);

    nicWriteCr (pNic, RPAGE0 | ABORT | STA);		/* back to page 0 */
    nicWriteReg (pNic, &pNic->Tcr, MODE0, RPAGE0);	/* Tx normal mode */
    }

/*******************************************************************************
*
* nicPktBufRead - read data from the NIC receive ring buffer
*
* This routine gets exclusive access to the remote DMA, and calls
* nicReceive() to get data from the NIC's receive ring buffer.
*
* RETURNS: OK, or ERROR if obtaining the requested bytes encountered an error.
*/

LOCAL STATUS nicPktBufRead
    (
    DRV_CTRL *		pDrvCtrl,
    UINT32		nicBufAddr,
    UINT32		len,
    char *		pData
    )
    {
    STATUS		status = OK;

    /* avoid starting DMA if device is down to to fatal error */

    if ((pDrvCtrl->idr.ac_if.if_flags & (IFF_UP | IFF_RUNNING)) !=
        (IFF_UP | IFF_RUNNING))
        return (ERROR);

    if (semTake (pDrvCtrl->dmaSem, 100) == ERROR)	/* get DMA */
	{
	if (_func_logMsg != NULL)
	    _func_logMsg ("nicPktBufRead: can't obtain dmaSem\n",0,0,0,0,0,0);
        semTake (pDrvCtrl->dmaSem, WAIT_FOREVER);
	}

    status = nicReceive (pDrvCtrl, nicBufAddr, pData, len);	/* goto ring */
    semGive(pDrvCtrl->dmaSem);

    return (status);
    }

/*******************************************************************************
*
* nicWriteCr - write to the NIC command register
*
* RETURNS: N/A.
*/

LOCAL void nicWriteCr
    (
    NIC_DEVICE		*pNic,
    UINT8		value
    )
    {
    int			level = intLock ();

    do {
        pNic->Cr = value;
        } while	(!((*(volatile UINT8 *)NIC_DMA) & ACC_MASK));

    intUnlock (level);
    }

/*******************************************************************************
*
* nicWriteReg - write to a NIC register
*
* RETURNS: N/A.
*/

LOCAL void nicWriteReg 
    (
    NIC_DEVICE *	pNic,
    volatile UINT8 *	pReg,
    UINT8		value,
    UINT8		page
    )
    {
    volatile UINT8 *	nicDma = (volatile UINT8 *) NIC_DMA;
    UINT8		cr;
    int			level = intLock ();

    do {
	cr = pNic->Cr;
	} while (!((*nicDma) & ACC_MASK));

    if ((cr & PBMASK) != page)
	do {
	    pNic->Cr = (cr & 0x3f) | page;
	    } while (!((*nicDma) & ACC_MASK));

    do {
	*pReg = value;
	} while (!((*nicDma) & ACC_MASK));

    intUnlock (level);
    }

/*******************************************************************************
*
* nicReadReg - read from a NIC register
*
* RETURNS: the register value.
*/

LOCAL UINT8 nicReadReg 
    (
    NIC_DEVICE *	pNic,
    volatile UINT8 *	pReg,
    char		page
    )
    {
    volatile UINT8 *	nicDma = (volatile UINT8 *) NIC_DMA;
    UINT8		cr;
    int			level = intLock ();

    do {
	cr = pNic->Cr;
	} while (!((*nicDma) & ACC_MASK));

    if ((cr & PBMASK) != page)
	do {
	    pNic->Cr = (cr & 0x3f) | page;
	    } while (!((*nicDma) & ACC_MASK));

    do {
	cr = *pReg;
	} while (!((*nicDma) & ACC_MASK));

    intUnlock (level);
    return (cr);
    }

/*******************************************************************************
*
* nicWritePort - write to the DMA port
*
* RETURNS: N/A.
*/

LOCAL void nicWritePort
    (
    UINT8		value
    )
    {
#ifndef	NIC_FASTER
    int			level = intLock ();
#endif

    do 
	{
	*(volatile char *) NIC_PORT = value;
        }
    while (!((*(volatile UINT8 *) NIC_DMA) & ACC_MASK));
#ifndef	NIC_FASTER
    intUnlock (level);
#endif
    }

/*******************************************************************************
*
* nicReadPort - read from the DMA port
*
* RETURNS: the DMA port value.
*/

LOCAL UINT8 nicReadPort (void)
    {
    UINT8		value;
#ifndef	NIC_FASTER
    int			level = intLock ();
#endif

    do
	{
	value = *(volatile UINT8 *) NIC_PORT;
        }
    while (!((*(volatile UINT8 *) NIC_DMA) & ACC_MASK));

#ifndef	NIC_FASTER
    intUnlock (level);
#endif
    return (value);
    }

/*******************************************************************************
*
* nicTransmit - send data over the NIC network interface
*
* This routine transfers data to the NIC device via the remote DMA, and
* then signal for a transmission.
*
* RETURNS: OK, or ERROR if the transmitter signalled an error.
*/

LOCAL STATUS nicTransmit
    (
    DRV_CTRL *		pDrvCtrl,
    MBUF *		pMbuf,
    int			len,
    BOOL		waitFlag
    )
    {
    NIC_DEVICE *	pNic = pDrvCtrl->nicAddr;
    UINT8		cr;
    UINT8		isr;
    UINT8 *		buf;
    int			status = OK;
    int			count;
    int			ix;
    int			tranxLen = max (len, MINPKTSIZE);

#ifdef	NIC_FASTER
    nicWriteReg(pNic, &pNic->Imr, 0, RPAGE0);   /* disable all interrupts */
#endif

    nicWriteReg (pNic, &pNic->Rbcr0, 0x10, RPAGE0);
    nicWriteCr (pNic, RREAD);

    nicWriteReg (pNic, &pNic->Rbcr0, (tranxLen & 0x00ff), RPAGE0);
    nicWriteReg (pNic, &pNic->Rbcr1, (tranxLen & 0xff00) >> 8, RPAGE0);
    nicWriteReg (pNic, &pNic->Rsar0, 0x00, RPAGE0);
    nicWriteReg (pNic, &pNic->Rsar1, 0x00, RPAGE0);
    nicWriteCr (pNic, RWRITE);
    count = 0;
    for (; pMbuf != NULL; pMbuf = m_free (pMbuf))
	for (ix = 0, buf = mtod (pMbuf, UINT8 *); ix < pMbuf->m_len; ix++)
	    { 
	    count++;
	    nicWritePort (*buf++);
	    }

    for (; count < tranxLen; count ++)		/* pad out if too short */
	nicWritePort (0);

    nicWriteReg (pNic, &pNic->Tpsr, 0x00, RPAGE0);
    nicWriteReg (pNic, &pNic->Tbcr0, (tranxLen & 0x000000ff), RPAGE0);
    nicWriteReg (pNic, &pNic->Tbcr1, ((tranxLen & 0x0000ff00) >> 8), RPAGE0);

    cr = nicReadReg (pNic, &pNic->Cr, RPAGE0);
    nicWriteReg(pNic, &pNic->Isr, (PTX | TXE), RPAGE0);
    nicWriteCr (pNic, TXP | (cr & (RWRITE | RREAD)));	/* start Tx */

    count = 0;
    if (waitFlag)				/* wait for Tx to end ? */
	{
        while (count++ < TRANS_TIMEOUT)		/* only poll a few times */
	    {
            isr = nicReadReg (pNic, &pNic->Isr, RPAGE0);

	    if (isr & TXE)			/* error encountered */
	        {
                status = ERROR;
	        break;
	        }

	    if (isr & PTX)			/* Tx done */
	        break;
            }

        if (count >= TRANS_TIMEOUT)		/* error ? */
	    {
            status = ERROR;
#ifdef	NIC_INSTRUMENT
	    nicTxTimeout++;
#endif
#ifdef	NIC_DEBUG
	    if (_func_logMsg != NULL)
		_func_logMsg("nicTransmit TRANS_TIMEOUT %d\n",
			     nicTxTimeout,0,0,0,0,0);
#endif
	    }
        }

#ifdef	NIC_FASTER
    nicWriteReg(pNic, &pNic->Imr, PRXE | OVWE, RPAGE0);	/* re-enable intr. */
#endif
    return (status);
    }

/*******************************************************************************
*
* nicReceive - receive data from the NIC network interface
*
* This routine transfers data from the NIC device ring buffers via the
* remote DMA.
*
* RETURNS: OK, always.
*/

LOCAL STATUS nicReceive 
    (
    DRV_CTRL *		pDrvCtrl,
    UINT32		nicBufAddr, 
    char *		pData, 
    int			len
    )
    {
    NIC_DEVICE *	pNic = pDrvCtrl->nicAddr;
    int			residual;
    int			dmaCnt;
    int			ix;

#ifdef	NIC_FASTER
    nicWriteReg(pNic, &pNic->Imr, 0, RPAGE0);	/* disable all interrupts */
#endif
    if ((nicBufAddr + len) < (PSTOP * PACKET_SIZE))
	{
	nicWriteReg (pNic, &pNic->Rbcr0, (len & 0xff), RPAGE0);
	nicWriteReg (pNic, &pNic->Rbcr1, ((len & 0xff00) >> 8), RPAGE0);
	nicWriteReg (pNic, &pNic->Rsar0, (nicBufAddr & 0xff), RPAGE0);
	nicWriteReg (pNic, &pNic->Rsar1, (((nicBufAddr & 0xff00)>> 8)), RPAGE0);
	nicWriteCr (pNic, RREAD);

	for (ix = 0; ix < len; ix ++)
	    *pData++ = nicReadPort ();
	}
    else
	{
	residual = (nicBufAddr + len) - (PSTOP * PACKET_SIZE);
	dmaCnt = len - residual;

	nicWriteReg (pNic, &pNic->Rbcr0, (dmaCnt & 0xff), RPAGE0);
	nicWriteReg (pNic, &pNic->Rbcr1, ((dmaCnt & 0xff00) >> 8), RPAGE0);
	nicWriteReg (pNic, &pNic->Rsar0, (nicBufAddr & 0xff), RPAGE0);
	nicWriteReg (pNic, &pNic->Rsar1, (((nicBufAddr & 0xff00)>> 8)), RPAGE0);
	nicWriteCr (pNic, RREAD);

	for (ix = 0; ix < dmaCnt; ix++)
	    *pData++ = nicReadPort ();

	nicWriteReg (pNic, &pNic->Rbcr0, (residual & 0xff), RPAGE0);
	nicWriteReg (pNic, &pNic->Rbcr1, ((residual & 0xff00) >> 8), RPAGE0);
	nicWriteReg (pNic, &pNic->Rsar0, 0x00, RPAGE0);
	nicWriteReg (pNic, &pNic->Rsar1, PSTART, RPAGE0);
	nicWriteCr (pNic, RREAD);

	for (ix = 0; ix < residual; ix++)
	    *pData++ = nicReadPort ();
	}

#ifdef	NIC_FASTER
    nicWriteReg(pNic, &pNic->Imr, PRXE | OVWE, RPAGE0);	/* re-enable intr. */
#endif
    return (OK);
    } 

/*******************************************************************************
*
* nicLoanFree - return a buffer to the Rx loaner pool
*
* RETURNS: N/A.
*/

LOCAL void nicLoanFree
    (
    DRV_CTRL *		pDrvCtrl,
    RX_FRAME *		pRx
    )
    {
    pDrvCtrl->lPool [pDrvCtrl->lIndex++] = pRx;
    }

/*******************************************************************************
*
* nicPagePtrUpdate - updates receive buffer/page pointers
*
* This routine updates the receive buffer/page pointer and the receive
* boundary register (BNRY). The chip is re-initialized if the receive next 
* packet pointer recorded in the current packet header is out of range.
*
* RETURNS: TRUE, or FALSE if next packet pointer is out of range.
*/
 
LOCAL BOOL nicPagePtrUpdate
    (
    DRV_CTRL *          pDrvCtrl
    )
    {
    RX_FRAME *          pRx = pDrvCtrl->pRxFrame;/* free Rx buffer */
    NIC_DEVICE *        pNic= pDrvCtrl->nicAddr;

    /* update ring buffer/page pointers */

    if ((pRx->rxHdr.nextRxPage < PSTART)||(pRx->rxHdr.nextRxPage >= PSTOP))
        {
#ifdef	NIC_DEBUG
        if (_func_logMsg != NULL)
	    _func_logMsg ("nicPagePtrUpdate: statusHeader=0x%x nextRxPage=%d IntNb=%d\n",pRx->rxHdr.status, pRx->rxHdr.nextRxPage, nicIntNb, 0,0);
#endif
#ifdef	NIC_INSTRUMENT
    	nicInitNb++;
#endif
        pDrvCtrl->idr.ac_if.if_ierrors++;

	/* restart the chip - we should never end up here - */

    	nicConfig (0);
    	pDrvCtrl->nextPkt = CURR;
    	pDrvCtrl->idr.ac_if.if_flags |= IFF_UP | IFF_RUNNING | IFF_NOTRAILERS;

        return (FALSE);
        }

    /* updates the Boundary pointer register (BNRY) */

    if ((pDrvCtrl->nextPkt = pRx->rxHdr.nextRxPage) == PSTART)
        nicWriteReg (pNic, &pNic->Bnry, PSTOP - 1, RPAGE0);
    else
        nicWriteReg (pNic, &pNic->Bnry, pDrvCtrl->nextPkt - 1, RPAGE0);

    return (TRUE);
    }

/*******************************************************************************
*
* nicResetDelay - performs the delay required before resetting the chip 
*
* This routine performs a 1.6 ms delay for PowerPC architecture if 
* using the internal PPC time base (the number of time base increment to
* count for 1.6 msec is defined by the value nicRestartDelay which must 
* be set in the BSP), or waits for one system clock tick otherwise.
*
* RETURNS: N/A.
*/
 
LOCAL void nicResetDelay (void)
    {
#ifdef  NIC_USE_PPCTIMEBASE
    UINT32 tbu1, tbu2;
    UINT32 tbl1, tbl2;
#endif

#ifdef  NIC_USE_PPCTIMEBASE
    vxTimeBaseGet (&tbu1, &tbl1);
    while (1)  {
        vxTimeBaseGet (&tbu2, &tbl2);
        if ((tbu2-tbu1) != 0)
            vxTimeBaseGet (&tbu1, &tbl1);
        else
	    if ((tbl2 - tbl1) > nicRestartDelay)
            	break;
        }
#else
    taskDelay (1);                      /* wait at least 1.6 mSec */
#endif
    }
