/* if_ultra.c - SMC Elite Ultra Ethernet network interface driver */

/* Copyright 1984-1999 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01u,11jan99,aeg  fixed multiple semaphore inits in ultraattach ().
01t,29jun98,dat  SPR 21211. fixes to Interrupt loop. 
01s,11jun98,hdn  fixed typo in ultraIntr().
01r,22apr98,hdn  set FINE16 bit in ultraInit(). 
		 added a loop to ack pending interrupts in untraIntr().
01q,15jul97,spm  removed driver initialization from ioctl support (SPR #8831)
01p,15may97,spm  reverted to bcopy routines for mbufs in BSD 4.4
01o,07apr97,spm  code cleanup and corrected counter for incoming packets
01n,17mar97,spm  upgraded to BSD 4.4 and corrected statistics counters.
01m,14jun95,hdn  removed function declarations defined in sysLib.h.
01l,01feb95,jdi  doc cleanup.
01k,29nov94,hdn  added a semaphore in ultraPut().
01j,12nov94,tmk  removed some IMPORTed function decs and included unixLib.h
01i,30sep94,hdn  implemented two transmit buffers.
		 enabled getting GPOUT and ZWSEN bit of GCR in EEROM.
		 deleted while loop in ultraPut().
01h,29nov93,hdn  disabled the interrupt in ultraPut().
		 removed netJobAdd (ultraPut, ..) in ultraIntr().
01g,28nov93,hdn  changed NULTRA to 1.
01f,23nov93,hdn  removed cacheInvalidate(). 
		 added checking TXP bit before the transmission. 
01e,22nov93,hdn  added elcdetach () for the warm start.
01d,15nov93,hdn  added support for Thin Ethernet(10Base2, BNC)
01c,05nov93,hdn  added cacheFlush() and cacheInvalidate().
01b,04nov93,hdn  cleaned up.
01a,30oct93,hdn  written.
*/

/*
DESCRIPTION
This module implements the SMC Elite Ultra Ethernet network interface driver.

This driver supports single transmission and multiple reception.  The
Current register is a write pointer to the ring.  The Bound register is a
read pointer from the ring.  This driver gets the Current register at the
interrupt level and sets the Bound register at the task level.  The
interrupt is never masked at the task level.

CONFIGURATION
The W1 jumper should be set in the position of "Software Configuration".
The defined I/O address in config.h must match the one stored in EEROM. 
The RAM address, the RAM size, and the IRQ level are defined in config.h. 
IRQ levels 2,3,5,7,10,11,15 are supported.

EXTERNAL INTERFACE
The only user-callable routines are ultraattach() and ultraShow():
.iP ultraattach() 14
publishes the `ultra' interface and initializes the driver and device.
.iP ultraShow()
displays statistics that are collected in the interrupt handler.
.LP
*/

#include "vxWorks.h"
#include "net/mbuf.h"
#include "net/protosw.h"
#include "net/unixLib.h"
#include "ioctl.h"
#include "socket.h"
#include "errnoLib.h"
#include "net/uio.h"
#include "net/if.h"
#include "net/route.h"
#include "netinet/in.h"
#include "netinet/in_systm.h"
#include "netinet/ip.h"
#include "netinet/ip_var.h"
#include "netinet/in_var.h"
#include "netinet/if_ether.h"
#include "etherLib.h"
#include "vme.h"
#include "iv.h"
#include "iosLib.h"
#include "ioLib.h"
#include "memLib.h"
#include "net/systm.h"
#include "net/if_subr.h"
#include "sysLib.h"
#include "vxLib.h"
#include "stdio.h"
#include "intLib.h"
#include "logLib.h"
#include "string.h"
#include "netLib.h"
#include "stdlib.h"
#include "semLib.h"
#include "cacheLib.h"
#include "drv/netif/if_ultra.h"

/* defines */

#define	NULTRA	1	/* max number of ULTRA controllers */

/* typedefs */

typedef struct ifnet IFNET;                 /* real Interface Data Record */

/* globals */


/* locals */

LOCAL ULTRA_SOFTC ultra_softc [NULTRA];
LOCAL IRQ_TABLE irqTable [] = 
    {
    { 2, 0x15},    { 2, 0x15},    { 2, 0x15},    { 3, 0x19},
    { 3, 0x19},    { 5, 0x1d},    { 5, 0x1d},    { 7, 0x51},
    { 7, 0x51},    { 7, 0x51},    {10, 0x55},    {11, 0x59},
    {11, 0x59},    {11, 0x59},    {11, 0x59},    {15, 0x5d}
    };
LOCAL BOOL ultraAttached = FALSE;
LOCAL SEMAPHORE  ultraSyncSem;           /* syncronization semaphore */


/* forward static functions */

       void ultraShow	(int unit, BOOL zap);
       void ultradetach (int unit);
LOCAL  int  ultrainit	(int unit);
LOCAL  void ultrareset	(int unit);
LOCAL  int  ultraioctl	(struct ifnet *ifp, int cmd, caddr_t data);
#ifdef BSD43_DRIVER
LOCAL  int  ultraoutput (struct ifnet *ifp, struct mbuf *m0, struct sockaddr
			 *dst);
#endif
LOCAL  void ultraInit	(int unit);
LOCAL  void ultraIntr	(int unit);
LOCAL  void ultraGet	(int unit);
#ifdef BSD43_DRIVER
LOCAL  void ultraPut	(int unit);
#else
LOCAL  void ultraPut 	(ULTRA_SOFTC *sc);
#endif

/*******************************************************************************
*
* ultraattach - publish `ultra' interface and initialize device
*
* This routine attaches an `ultra' Ethernet interface to the network if the
* device exists.  It makes the interface available by filling in the network
* interface record.  The system will initialize the interface when it is ready
* to accept packets.
*
* RETURNS: OK or ERROR.
*
* SEE ALSO: ifLib, netShow
*/

STATUS ultraattach 
    (
    int unit,           /* unit number */
    int ioAddr,         /* address of ultra's shared memory */
    int ivec,           /* interrupt vector to connect to */
    int ilevel,         /* interrupt level */
    int memAddr,        /* address of ultra's shared memory */
    int memSize,        /* size of ultra's shared memory */
    int config		/* 0: RJ45 + AUI(Thick)  1: RJ45 + BNC(Thin) */
    )
    {
    FAST ULTRA_SOFTC *sc = &ultra_softc[unit];

    if ((UINT)unit >= NULTRA)
	{
        errnoSet (S_iosLib_CONTROLLER_NOT_PRESENT);
	return (ERROR);
	}

    sc->ioAddr		= ioAddr;
    sc->memAddr		= memAddr;
    sc->memSize		= memSize;
    sc->intLevel	= irqTable[ilevel].irq;
    sc->intVec		= ivec;
    sc->config		= config;
    sc->transmitPage[0]	= ULTRA_TSTART0;
    sc->transmitPage[1]	= ULTRA_TSTART1;
    sc->transmitCnt	= 0;

    /* synchronization semaphore must be initialized only once */

    if (!ultraAttached)
	semBInit (&ultraSyncSem, SEM_Q_FIFO, SEM_FULL);

    (void) intConnect ((VOIDFUNCPTR *)INUM_TO_IVEC (ivec), 
		       (VOIDFUNCPTR)ultraIntr, unit);

#ifdef BSD43_DRIVER
    ether_attach (&sc->es_if, unit, "ultra", (FUNCPTR)ultrainit, 
	  (FUNCPTR)ultraioctl, (FUNCPTR)ultraoutput, (FUNCPTR)ultrareset);
#else
    ether_attach (
                 (IFNET *) & sc->es_ac,
                 unit,
                 "ultra",
                 (FUNCPTR) ultrainit,
                 (FUNCPTR) ultraioctl,
                 (FUNCPTR) ether_output,    /* generic ether_output */ 
                 (FUNCPTR) ultrareset
                 );
    sc->es_if.if_start = (FUNCPTR) ultraPut;
#endif 

    ultrainit (unit);

    ultraAttached = TRUE;

    return (OK);
    }

/*******************************************************************************
*
* ultrareset - reset the interface
*
* RETURNS: N/A
*/

LOCAL void ultrareset 
    (
    int unit 	/* device unit number */
    )
    {
    FAST ULTRA_SOFTC *sc   = &ultra_softc [unit];
    int s = splnet ();

    sc->stat.reset++;
    sysIntDisablePIC (sc->intLevel);
    sc->es_if.if_flags = 0;

    splx (s);
    }

/*******************************************************************************
*
* ultrainit - initialize ultra
*
* Restart the link level software on the board and mark the interface up.
*
* RETURNS: 0
*/

LOCAL int ultrainit 
    (
    int unit 	/* device unit number */
    )
    {
    FAST ULTRA_SOFTC *sc = &ultra_softc[unit];
    int s = splnet ();

    ultraInit (unit);
    sysIntEnablePIC (sc->intLevel);
    sc->es_if.if_flags |= IFF_UP|IFF_RUNNING;	/* open for business*/

    splx (s);
    return (0);
    }

#ifdef BSD43_DRIVER
/*******************************************************************************
*
* ultraoutput - ethernet output routine
*
* Encapsulate a packet of type family for the local net.
* Use trailer local net encapsulation if enough data in first
* packet leaves a multiple of 512 bytes of data in remainder.
* If destination is this address or broadcast, send packet to
* loop device to kludge around the fact that 3com interfaces can't
* talk to themselves.
*
* RETURNS:  OK if successful, otherwise errno.
*/

LOCAL int ultraoutput 
    (
    struct ifnet *ifp,		/* device structure */
    struct mbuf *m0,		/* mbuf containing packet */
    struct sockaddr *dst 	/* destination address */
    )
    {
    return (ether_output (ifp, m0, dst, (FUNCPTR)ultraPut,
			  &ultra_softc [ifp->if_unit].es_ac));
    }
#endif

/*******************************************************************************
*
* ultraioctl - ioctl for interface
*
* RETURNS: OK if successful, otherwise errno.
*/

LOCAL int ultraioctl 
    (
    FAST struct ifnet *ifp,	/* device structure */
    int cmd,			/* ioctl cmd code */
    caddr_t data 		/* ioctl argument */
    )
    {
    int status = OK;
    int s = splimp();
    short flags;

    switch (cmd)
	{
	case SIOCSIFADDR:
            ((struct arpcom *)ifp)->ac_ipaddr = IA_SIN (data)->sin_addr;
            arpwhohas ((struct arpcom *)ifp, &IA_SIN (data)->sin_addr);
	    break;

	case SIOCSIFFLAGS:
            flags = ifp->if_flags;

	    if (ifp->if_flags & IFF_UP)
                ifp->if_flags = flags | (IFF_UP | IFF_RUNNING);
            else
                ifp->if_flags = flags & ~(IFF_UP|IFF_RUNNING);
	    break;

	default:
	    status = EINVAL;
	}

    splx (s);
    return (status);
    }

/*******************************************************************************
*
* ultraIntr - Ethernet interface interrupt
*
* Hand off reading of packets from the interface to task level.
*
* RETURNS: N/A
*/

LOCAL void ultraIntr 
    (
    int unit 	/* device unit number */
    )
    {
    FAST ULTRA_SOFTC *sc = &ultra_softc[unit];
    UCHAR istat;
    int   nCnt = 0;

    /* Acknowledge interrupt, get Transmit/Receive status */

    sc->istat = 0;
    do  {
        istat = sysInByte (LAN_INTSTAT (sc->ioAddr));
        sysOutByte (LAN_INTSTAT (sc->ioAddr), istat);
        sc->istat |= istat;
        nCnt ++;
        }
    while (istat != 0);
    sc->tstat = sysInByte (LAN_TSTAT (sc->ioAddr));
    sc->rstat = sysInByte (LAN_RSTAT (sc->ioAddr));

    /* get xxxCount Reg that is cleared after read access */

    sc->stat.interrupts++;
    sc->stat.collisions += sysInByte (LAN_COLCNT (sc->ioAddr));
    sc->stat.aligns += sysInByte (LAN_ALICNT (sc->ioAddr));
    sc->stat.crcs += sysInByte (LAN_CRCCNT (sc->ioAddr));
    sc->stat.missed += sysInByte (LAN_MPCNT (sc->ioAddr));

    if (sc->istat & ISTAT_RXE) /* Receive-error */
	{
        sc->es_if.if_ierrors++;
	sc->stat.rerror++;
	if (sc->rstat & RSTAT_OVER)
	    sc->stat.overruns++;
	if (sc->rstat & RSTAT_DIS)
	    sc->stat.disabled++;
	if (sc->rstat & RSTAT_DFR)
	    sc->stat.deferring++;
	}
    if (sc->istat & (ISTAT_PRX | ISTAT_OVW)) /* Receive, Overwrite */
	{
        if (sc->istat & ISTAT_OVW)
            {
	    sc->stat.overwrite++;
            sc->es_if.if_ierrors++;
            }
        sysOutByte (LAN_CMD (sc->ioAddr), CMD_PS0);
        sc->current = sysInByte (LAN_CURR (sc->ioAddr));
        sysOutByte (LAN_CMD (sc->ioAddr), 0);
	if ((sc->flags & RXING) == 0)
	    {
            sc->flags |= RXING;
	    (void) netJobAdd ((FUNCPTR)ultraGet, unit, 0,0,0,0);
	    }
	if (sc->rstat & RSTAT_PRX)
	    sc->stat.rnoerror++;
	}
    if (sc->istat & (ISTAT_TXE | ISTAT_PTX)) /* Transmit-error, Transmit */
	{
	semGive (&ultraSyncSem);

#if FALSE  /* BEGIN TEST CODE */
	if (sc->es_if.if_snd.ifq_head != NULL)
	    {
#ifdef BSD43_DRIVER
	    netJobAdd ((FUNCPTR) ultraPut, unit, 0,0,0,0);
#else
	    netJobAdd ((FUNCPTR) ultraPut, (int)sc, 0,0,0,0);
#endif
	    }
#endif /* END TEST CODE */

        if (sc->istat & ISTAT_TXE)
            {
	    sc->stat.terror++;
            sc->es_if.if_oerrors++;
            sc->es_if.if_opackets--;
            }
	if (sc->tstat & TSTAT_ABORT)
	    {
	    sc->stat.aborts++;
	    sc->stat.collisions += 16;
	    }
	if (sc->tstat & TSTAT_UNDER)
	    sc->stat.underruns++;
	if (sc->tstat & TSTAT_CDH)
	    sc->stat.heartbeats++;
	if (sc->tstat & TSTAT_OWC)
	    sc->stat.outofwindow++;
	if (sc->tstat & TSTAT_PTX)
	    sc->stat.tnoerror++;
	}
    }

/*******************************************************************************
*
* ultraGet - read a packet off the interface
*
* Copy packets from a RING into an mbuf and hand it to the next higher layer.
*
* RETURNS: N/A
*/

LOCAL void ultraGet 
    (
    int unit	/* device unit number */
    )
    {
    FAST ULTRA_SOFTC *sc = &ultra_softc[unit];
    FAST ULTRA_HEADER *pH;
    FAST struct ether_header *eh;
    FAST struct mbuf *m;
    int len;
#ifdef BSD43_DRIVER
    int off;
#endif
    int ix;
    USHORT wrapSize;
    USHORT packetSize;
    char *pSrc;
    char *pDst;
    UCHAR *pData;


unlucky:
    ix = 0;

    while ((sc->next != sc->current) && (ix++ < ULTRA_PSTOP))
	{
	/* Invalidate cache */
        cacheDrvInvalidate (&cacheDmaFuncs, 
		            (UCHAR *)(sc->memAddr + (ULTRA_PSTART << 8)),
		            (sc->memSize - (ULTRA_PSTART << 8)));

	pH = (ULTRA_HEADER *)(sc->memAddr + (sc->next << 8));

	/* sanity check */

	if ((pH->next < ULTRA_PSTART) || (pH->next >= ULTRA_PSTOP))
	    {
	    sc->stat.badPacket++;
	    pH->next = sc->next + sc->uppByteCnt + 1;
	    if (pH->next >= ULTRA_PSTOP)
		pH->next = ULTRA_PSTART + (pH->next - ULTRA_PSTOP);
	    }

	packetSize = *(USHORT *)(&pH->lowByteCnt);
	if (packetSize < (ULTRA_MIN_SIZE))
	    {
	    sc->stat.shortPacket++;
	    goto doneGet;
	    }
	if (packetSize > (ULTRA_MAX_SIZE))
	    {
	    sc->stat.longPacket++;
	    goto doneGet;
	    }

        sc->es_if.if_ipackets++;        /* bump statistic */

	if (pH->next > sc->next)
	    wrapSize = 0;
	else
	    wrapSize = min (packetSize, ((ULTRA_PSTOP - sc->next) << 8));

	/* copy separated frame to a temporary buffer */

	if (wrapSize)
	    {
	    sc->stat.wrapped++;
	    pSrc = (char *)pH;
	    pDst = (char *)sc->receiveBuf;
	    bcopy (pSrc, pDst, wrapSize);
	    pSrc = (char *)(sc->memAddr + (ULTRA_PSTART << 8));
	    pDst += wrapSize;
	    len = packetSize - wrapSize;
	    if (len)
		bcopy (pSrc, pDst, len);
	    }

        len = packetSize - 4;			/* remove CRCs 4 bytes */

        if (wrapSize)
	    eh = (struct ether_header *)((UINT)sc->receiveBuf + 
		sizeof (ULTRA_HEADER));
        else
	    eh = (struct ether_header *)((UINT)pH + sizeof (ULTRA_HEADER));

        /* call input hook if any */

        if ((etherInputHookRtn != NULL) &&
	    (* etherInputHookRtn) (&sc->es_if, (char *) eh, len))
	    {
	    goto doneGet;
	    }

        if (len >= SIZEOF_ETHERHEADER)
            len -= SIZEOF_ETHERHEADER;
        else
	    goto doneGet;

        pData = ((unsigned char *) eh) + SIZEOF_ETHERHEADER;

#ifdef BSD43_DRIVER
        check_trailer (eh, pData, &len, &off, &sc->es_if);

        /* copy data from the ring buffer to mbuf a long-word at a time */

        m = bcopy_to_mbufs (pData, len, off, (struct ifnet *) &sc->es_if, 2);

        if (m == NULL)
            {
            sc->es_if.if_ierrors++;         /* bump error statistic */
	    goto doneGet;
            }

        do_protocol_with_type (eh->ether_type, m, &sc->es_ac, len);
#else

        /* copy data from the ring buffer to mbuf a long-word at a time */

        m = bcopy_to_mbufs (pData, len, 0, (struct ifnet *) &sc->es_if, 2);

        if (m == NULL)
            {
            sc->es_if.if_ierrors++;         /* bump error statistic */
	    goto doneGet;
            }

        do_protocol (eh, m, &sc->es_ac, len);

#endif

doneGet:
	sc->next = pH->next;
	sc->uppByteCnt = pH->uppByteCnt;
	}

    /* update BOUND Register */

    if ((sc->next < ULTRA_PSTART) || (sc->next >= ULTRA_PSTOP))
	sysOutByte (LAN_BOUND (sc->ioAddr), ULTRA_PSTART);
    else
	sysOutByte (LAN_BOUND (sc->ioAddr), sc->next);

    sc->flags &= ~RXING;

    /* go back if we received an interrupt and a new packet */

    if (sc->next != sc->current)
	goto unlucky;
    }

/*******************************************************************************
*
* ultraPut - copy a packet to the interface.
*
* Copy from mbuf chain to transmitter buffer in shared memory.
*
* RETURNS: N/A
*/

#ifdef BSD43_DRIVER
LOCAL void ultraPut 
    (
    int unit	/* device unit number */
    )
    {
    FAST ULTRA_SOFTC *sc   = &ultra_softc [unit];
#else
LOCAL void ultraPut
    (
    ULTRA_SOFTC *sc	/* device context */
    )
    {
#endif
    FAST struct mbuf *m;
    FAST int len;
    UINT transmitPage = sc->transmitPage[sc->transmitCnt & 1];
    UCHAR *pBuf = (UCHAR *)(sc->memAddr + (transmitPage << 8));
#ifdef BSD43_DRIVER
    int s = splnet ();
#endif

    while (sc->es_if.if_snd.ifq_head != NULL)
	{
	semTake (&ultraSyncSem, sysClkRateGet ());

	IF_DEQUEUE (&sc->es_if.if_snd, m);

	if (m != NULL)
	    {
	    copy_from_mbufs (pBuf, m, len);
	    len = max (ETHERSMALL, len);

	    cacheDrvFlush (&cacheDmaFuncs, pBuf, len);

	    /* call output hook if any */

	    if ((etherOutputHookRtn != NULL) &&
		(* etherOutputHookRtn) (&sc->es_if, pBuf, len))
		continue;

	    /* kick Transmitter */

	    sysOutByte (LAN_INTMASK (sc->ioAddr), 0x00);
	    sysOutByte (LAN_TSTART (sc->ioAddr), transmitPage);
	    sysOutByte (LAN_TCNTH (sc->ioAddr), len >> 8);
	    sysOutByte (LAN_TCNTL (sc->ioAddr), len & 0xff);
	    sysOutByte (LAN_CMD (sc->ioAddr), CMD_TXP);
	    sc->transmitCnt++;
	    sysOutByte (LAN_INTMASK (sc->ioAddr), 
			IM_OVWE | IM_TXEE | IM_PTXE | IM_PRXE);
#ifndef BSD43_DRIVER
	    sc->es_if.if_opackets++;
#endif
	    }
	}

#ifdef BSD43_DRIVER
    splx (s);
#endif
    }

/*******************************************************************************
*
* ultraInit - initialize Elite Ultra (Ethernet LAN Controller)
*
* RETURNS: N/A
*/

LOCAL void ultraInit 
    (
    int unit	/* device unit number */
    )
    {
    FAST ULTRA_SOFTC *sc = &ultra_softc[unit];
    UCHAR *pAddr	 = (UCHAR *)sc->es_enaddr;
    UCHAR memAddr;
    UCHAR intLevel;
    UCHAR ioAddr;
    UCHAR eeromGcon;

    /* mask interrupt */

    sysOutByte (LAN_INTMASK (sc->ioAddr), 0x00);

    /* reset */

    sysOutByte (CTRL_CON (sc->ioAddr), CON_RESET);
    sysOutByte (CTRL_CON (sc->ioAddr), 0x00);
    sysOutByte (CTRL_HARD (sc->ioAddr), 0x08);
    taskDelay (sysClkRateGet () >> 2);
    sysOutByte (CTRL_CON (sc->ioAddr), CON_MENABLE);

    /* get values set by EEROM */

    sysOutByte (CTRL_HARD (sc->ioAddr), 0x80);
    eeromGcon = sysInByte (CTRL_GCON(sc->ioAddr));

    /* IO address, Memory address, Interrupt request */

    memAddr = ((sc->memAddr & 0x1e000) >> 13) | ((sc->memAddr & 0x20000) >> 10);
    intLevel = ((sc->intLevel & 0x03) << 2) | ((sc->intLevel & 0x04) << 6);
    ioAddr = ((sc->ioAddr & 0xe000) >> 8) | ((sc->ioAddr & 0x01e0) >> 4);
    sysOutByte (CTRL_HARD (sc->ioAddr), 0x80);
    sysOutByte (CTRL_IOADDR (sc->ioAddr), ioAddr);
    sysOutByte (CTRL_MEMADDR (sc->ioAddr), memAddr | 0x10);
    if (sc->config == 1)
        sysOutByte (CTRL_GCON (sc->ioAddr), irqTable[sc->intLevel].reg |
		    (eeromGcon & 0x20));
    else if (sc->config == 2)
        sysOutByte (CTRL_GCON (sc->ioAddr), irqTable[sc->intLevel].reg | 0x02 |
		    (eeromGcon & 0x20));
    else
        sysOutByte (CTRL_GCON (sc->ioAddr), irqTable[sc->intLevel].reg | 
		    (eeromGcon & 0x22));

    /* set FINE16 bit in BIO register to get finer resolution for M16CS decode */

    sysOutByte (CTRL_BIO (sc->ioAddr), sysInByte (CTRL_BIO (sc->ioAddr) | 0x80)); 
    sysOutByte (CTRL_HARD (sc->ioAddr), 0x00);

    /* 16 bit enable */

    sysOutByte (CTRL_BIOS (sc->ioAddr), BIOS_M16EN);

    /* program Command Register for page 0 */

    sysOutByte (LAN_CMD (sc->ioAddr), CMD_STP);
    while ((sysInByte (LAN_INTSTAT (sc->ioAddr)) & ISTAT_RST) != ISTAT_RST)
	;

    /* initialize Data Configuration Register */

    sysOutByte (LAN_DCON (sc->ioAddr), DCON_BSIZE1 | DCON_BUS16);

    /* initialize Receive Configuration Register */

    sysOutByte (LAN_RCON (sc->ioAddr), 0x04);

    /* place the Ultra in LOOPBACK mode 1 or 2 */

    sysOutByte (LAN_TCON (sc->ioAddr), TCON_LB1);

    /* initialize Receive Buffer Ring */

    sysOutByte (LAN_RSTART (sc->ioAddr), ULTRA_PSTART);
    sysOutByte (LAN_RSTOP (sc->ioAddr), ULTRA_PSTOP);
    sysOutByte (LAN_BOUND (sc->ioAddr), ULTRA_PSTART);

    /* clear Interrupt Status Register */

    sysOutByte (LAN_INTSTAT (sc->ioAddr), 0xff);

    /* initialize Interrupt Mask Register */

    sysOutByte (LAN_INTMASK (sc->ioAddr), 0x00);

    /* program Command Register for page 1 */

    sysOutByte (LAN_CMD (sc->ioAddr), CMD_PS0 | CMD_STP);
    sysOutByte (LAN_STA0 (sc->ioAddr), sysInByte (CTRL_LAN0 (sc->ioAddr)));
    sysOutByte (LAN_STA1 (sc->ioAddr), sysInByte (CTRL_LAN1 (sc->ioAddr)));
    sysOutByte (LAN_STA2 (sc->ioAddr), sysInByte (CTRL_LAN2 (sc->ioAddr)));
    sysOutByte (LAN_STA3 (sc->ioAddr), sysInByte (CTRL_LAN3 (sc->ioAddr)));
    sysOutByte (LAN_STA4 (sc->ioAddr), sysInByte (CTRL_LAN4 (sc->ioAddr)));
    sysOutByte (LAN_STA5 (sc->ioAddr), sysInByte (CTRL_LAN5 (sc->ioAddr)));
    *pAddr++ = sysInByte (CTRL_LAN0 (sc->ioAddr));
    *pAddr++ = sysInByte (CTRL_LAN1 (sc->ioAddr));
    *pAddr++ = sysInByte (CTRL_LAN2 (sc->ioAddr));
    *pAddr++ = sysInByte (CTRL_LAN3 (sc->ioAddr));
    *pAddr++ = sysInByte (CTRL_LAN4 (sc->ioAddr));
    *pAddr   = sysInByte (CTRL_LAN5 (sc->ioAddr));
    sysOutByte (LAN_MAR0 (sc->ioAddr), 0);
    sysOutByte (LAN_MAR1 (sc->ioAddr), 0);
    sysOutByte (LAN_MAR2 (sc->ioAddr), 0);
    sysOutByte (LAN_MAR3 (sc->ioAddr), 0);
    sysOutByte (LAN_MAR4 (sc->ioAddr), 0);
    sysOutByte (LAN_MAR5 (sc->ioAddr), 0);
    sysOutByte (LAN_MAR6 (sc->ioAddr), 0);
    sysOutByte (LAN_MAR7 (sc->ioAddr), 0);
    sc->next = ULTRA_PSTART + 1;
    sysOutByte (LAN_CURR (sc->ioAddr), sc->next);

    sysOutByte (LAN_CMD (sc->ioAddr), CMD_PS1 | CMD_STP);
    sysOutByte (LAN_ENH (sc->ioAddr), 0x00);	/* 0 wait states */

    /* put the Ultra in START mode */

    sysOutByte (LAN_CMD (sc->ioAddr), CMD_STA);

    /* initialize Transmit Configuration Register */

    sysOutByte (LAN_TCON (sc->ioAddr), 0x00);

    /* enable interrupt */

    sysOutByte (CTRL_INT (sc->ioAddr), INT_ENABLE);
    sysOutByte (LAN_INTMASK (sc->ioAddr),
		IM_OVWE | IM_TXEE | IM_PTXE | IM_PRXE);
    }

/*******************************************************************************
*
* ultraShow - display statistics for the `ultra' network interface
*
* This routine displays statistics about the `elc' Ethernet network interface.
* It has two parameters:
* .iP <unit>
* interface unit; should be 0.
* .iP <zap>
* if 1, all collected statistics are cleared to zero.
* .LP
*
* RETURNS: N/A
*/

void ultraShow 
    (
    int unit,   /* interface unit */
    BOOL zap    /* zero totals */
    )
    {
    FAST ULTRA_SOFTC *sc          = &ultra_softc[unit];
    FAST int ix;
    static char *e_message [] = {
	"collisions",
	"crcs",
	"aligns",
	"missed",
	"over-runs",
	"disabled",
	"deferring",
	"under-run",
	"aborts",
	"out-of-window",
	"heart-beats",
	"bad-packet",
	"short-packet",
	"long-packet",
	"t-no-error",
	"r-no-error",
	"t-error",
	"r-error",
	"over-write",
	"wrapped",
	"interrupts",
	"reset",
	"stray-int"};

    printf ("ioAddr=0x%x memAddr=0x%x memSize=0x%x vector=0x%x level=0x%x\n",
	sc->ioAddr, sc->memAddr, sc->memSize, sc->intVec, sc->intLevel);
    for (ix = 0; ix < NELEMENTS(e_message); ix++)
	{
	printf ("    %-30.30s  %4d\n", e_message [ix], sc->stat.stat [ix]);
	if (zap)
	    sc->stat.stat [ix] = 0;
	}
    printf ("    %-30.30s  0x%2x\n", "flags", sc->flags);
    }

/*******************************************************************************
*
* ultradetach - detach the card from the bus.
*
* Dettach the card from the bus for reset.
*
* RETURNS: N/A
*
* NOMANUAL
*/

void ultradetach 
    (
    int unit	/* device unit number */
    )
    {
    FAST ULTRA_SOFTC *sc = &ultra_softc[unit];

    if (ultraAttached)
	{
        sysOutByte (LAN_INTMASK (sc->ioAddr), 0x00);
        sysOutByte (LAN_CMD (sc->ioAddr), CMD_STP);
        while ((sysInByte (LAN_INTSTAT (sc->ioAddr)) & ISTAT_RST) != ISTAT_RST)
	    ;
        sysOutByte (LAN_RCON (sc->ioAddr), 0x00);
        sysOutByte (LAN_TCON (sc->ioAddr), TCON_LB1);
        sysIntDisablePIC (sc->intLevel);
        sysOutByte (CTRL_CON (sc->ioAddr), CON_RESET);
        taskDelay (sysClkRateGet () >> 1);
        sysOutByte (CTRL_CON (sc->ioAddr), 0x00);
        taskDelay (sysClkRateGet () >> 1);
	}
    }

