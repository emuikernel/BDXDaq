/* if_elc.c - SMC 8013WC Ethernet network interface driver */

/* Copyright 1984-1999 Wind River Systems, Inc. */

#include "copyright_wrs.h"

/*
modification history
--------------------
02b,11jan99,aeg  fixed multiple semaphore inits in elcattach ().
02a,15jul97,spm  removed driver initialization from ioctl support (SPR #8831)
01z,15may97,spm  reverted to bcopy routines for mbufs in BSD 4.4
01y,07apr97,spm  code cleanup, corrected statistics, and upgraded to BSD 4.4
01x,14jun95,hdn  removed function declarations defined in sysLib.h.
01w,01feb95,jdi  doc cleanup; made elcShow() NOMANUAL.
01v,29nov94,hdn  added a semaphore in ultraPut().
01u,12nov94,tmk  removed some IMPORTed function decs and included unixLib.h
01t,03oct94,hdn  implemented two transmit buffers.
		 enabled getting OUT2 bit of IRR in EEROM.
		 deleted while loop in ultraPut().
01s,03dec93,hdn  added documentation.
01r,29nov93,hdn  removed netJobAdd (elcPut, ..) in elcIntr().
01q,28nov93,hdn  changed NELC to 1.
01p,24nov93,hdn  removed TXING and txreq flags.
		 added checking TXP bit before the transmission. 
01o,23nov93,hdn  removed cacheInvalidate(). 
01n,22nov93,hdn  added elcdetach () for the warm start.
01m,15nov93,hdn  added support for Thin Ethernet(10Base2, BNC)
01l,05nov93,hdn  added cacheFlush() and cacheInvalidate().
01k,04nov93,hdn  cleaned up.
01j,03nov93,hdn  made RAM size 16Kbytes. cleaned up.
01i,05sep93,hdn  added sysIntEnablePIC ().
01h,02sep93,hdn  added macros about PIC.
01g,11aug93,hdn  changed a way of setting a handler for stray interrupt IRQ7.
01f,23jun93,hdn  made address, irq software configuarable.
01e,17jun93,hdn  updated to 5.1.
01d,28may93,hdn  deleted a zero wait bit. fixed a race condition.
01c,23apr93,hdn  fixed a bug which appears when a packet is wrapped.
		 fixed a stray interrupt 15.
		 moved a interrupt masking to the top of elcGet.
01b,03jan93,hdn  fixed bugs and cleaned up.
01a,09jun92,hdn  written.
*/

/*
DESCRIPTION
This module implements the SMC 8013WC network interface driver.

BOARD LAYOUT
The W1 jumper should be set in position SOFT.
The W2 jumper should be set in position NONE/SOFT.

CONFIGURATION
The I/O address, RAM address, RAM size, and IRQ levels are defined in config.h.
The I/O address must match the one stored in EEROM.  The configuration 
software supplied by the manufacturer should be used to set the I/O address. 

IRQ levels 2,3,4,5,7,9,10,11,15 are supported.  Thick Ethernet (AUI) and
Thin Ethernet (BNC) are configurable by changing the macro CONFIG_ELC
in config.h.

EXTERNAL INTERFACE
The only user-callable routines are elcattach() and elcShow():
.iP elcattach() 14
publishes the `elc' interface and initializes the driver and device.
.iP elcShow()
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
#include "drv/intrCtl/i8259a.h"
#include "drv/netif/if_elc.h"

/*
 * Ethernet software status per interface.
 *
 * Each interface is referenced by a network interface structure,
 * es_if, which the routing code uses to locate the interface.
 * This structure contains the output queue for the interface, its address, ...
 */

#define	NELC	1	/* max number of ELC controllers */

LOCAL ELC_SOFTC elc_softc [NELC];
LOCAL IRQ_TABLE irqTable [] =
    {
    {0xff,  0xff},		/* IRQ0  not supported  */
    {0xff,  0xff},		/* IRQ1  not supported  */
    {0x0,   0x0},		/* IRQ2  		*/
    {0x0,   0x1},		/* IRQ3  		*/
    {0x1,   0x3},		/* IRQ4  		*/
    {0x0,   0x2},		/* IRQ5  		*/
    {0xff,  0xff},		/* IRQ6  not supported  */
    {0x0,   0x3},		/* IRQ7  		*/
    {0xff,  0xff},		/* IRQ8  not supported  */
    {0x0,   0x0},		/* IRQ9  		*/
    {0x1,   0x0},		/* IRQ10 		*/
    {0x1,   0x1},		/* IRQ11 		*/
    {0xff,  0xff},		/* IRQ12 not supported  */
    {0xff,  0xff},		/* IRQ13 not supported  */
    {0xff,  0xff},		/* IRQ14 not supported  */
    {0x1,   0x2},		/* IRQ15 		*/
    {0, 0},
    };
LOCAL BOOL elcAttached = FALSE;
LOCAL SEMAPHORE elcSyncSem;		/* syncronization semaphore */


/* forward static functions */

static void	elcreset (int unit);
static int	elcinit (int unit);
static void	elcintr (int unit);
static int	elcioctl (struct ifnet *ifp, int cmd, caddr_t data);
#ifdef BSD43_DRIVER
static int	elcoutput (struct ifnet *ifp, struct mbuf *m0, struct sockaddr
		*dst);
#endif
static void	elcGet (int unit);
#ifdef BSD43_DRIVER
static void	elcPut (int unit);
#else
static void	elcPut (ELC_SOFTC *sc);
#endif
static void	elcReset (int unit);
static void	elcGetAddr (int unit);
static void	bicInit (int unit);
static void	elcInit (int unit);
static UCHAR	elcGetCurr (int unit);
       void	elcShow (int unit, BOOL zap);
       void	elcdetach (int unit);


/*******************************************************************************
*
* elcattach - publish the `elc' network interface and initialize the driver and device
*
* This routine attaches an `elc' Ethernet interface to the network if the
* device exists.  It makes the interface available by filling in the network
* interface record.  The system will initialize the interface when it is
* ready to accept packets.
*
* RETURNS: OK or ERROR.
*
* SEE ALSO: ifLib, netShow
*/

STATUS elcattach 
    (
    int unit,           /* unit number */
    int ioAddr,         /* address of elc's shared memory */
    int ivec,           /* interrupt vector to connect to */
    int ilevel,         /* interrupt level */
    int memAddr,        /* address of elc's shared memory */
    int memSize,        /* size of elc's shared memory */
    int config		/* 0: RJ45 + AUI(Thick)  1: RJ45 + BNC(Thin) */
    )
    {
    FAST ELC_SOFTC *sc = &elc_softc[unit];

    if ((UINT)unit >= NELC)
	{
	errnoSet (S_iosLib_CONTROLLER_NOT_PRESENT);
	return (ERROR);
	}

    sc->bicAddr		= ioAddr;
    sc->elcAddr		= ioAddr + 0x10;
    sc->memAddr		= memAddr;
    sc->memSize		= memSize;
    if (ilevel == 0  || ilevel == 1  || ilevel == 6 || ilevel == 8 ||
	ilevel == 12 || ilevel == 13 || ilevel == 14)
	ilevel = 3;                     /* default is IRQ3 */
    sc->intLevel	= ilevel;
    sc->intVec		= ivec;
    sc->config		= config;
    sc->transmitPage[0]	= SMC8013WC_TSTART0;
    sc->transmitPage[1]	= SMC8013WC_TSTART1;
    sc->transmitCnt	= 0;

    /* synchronization semaphore must be initialized only once */

    if (!elcAttached)
	semBInit (&elcSyncSem, SEM_Q_FIFO, SEM_FULL);

    bicInit (unit);			/* initialize BIC */

    elcInit (unit);			/* initialize ELC */

    elcGetAddr (unit);			/* get ethernet address */

    (void) intConnect ((VOIDFUNCPTR *)INUM_TO_IVEC (ivec), (VOIDFUNCPTR)elcintr, unit);

    /* enable interrupt */

    sysOutByte (ELC_INTMASK (sc->elcAddr), 
	IM_OVWE | IM_TXEE | IM_PTXE | IM_PRXE);

    sysIntEnablePIC (ilevel);

#ifdef BSD43_DRIVER
    ether_attach (&sc->es_if, unit, "elc", (FUNCPTR)elcinit, (FUNCPTR)elcioctl,
		  (FUNCPTR)elcoutput, (FUNCPTR)elcreset);
#else
    ether_attach (
                 &sc->es_if, 
                 unit, 
                 "elc", 
                 (FUNCPTR)elcinit, 
                 (FUNCPTR)elcioctl,
		 (FUNCPTR)ether_output, 
                 (FUNCPTR)elcreset
                 );
    sc->es_if.if_start = (FUNCPTR)elcPut;
#endif

    elcinit (unit);

    elcAttached = TRUE;

    return (OK);
    }

/*******************************************************************************
*
* elcreset - reset the interface
*/

LOCAL void elcreset 
    (
    int unit 
    )
    {
    elcReset (unit);
    }

/*******************************************************************************
*
* elcinit - initialize elc
*
* Restart the link level software on the board and mark the interface up.
*
* RETURNS: 0
*/

LOCAL int elcinit 
    (
    int unit 
    )
    {
    FAST ELC_SOFTC *sc = &elc_softc[unit];

    sc->es_if.if_flags |= IFF_UP|IFF_RUNNING; /* open for business*/

    return (0);
    }

/*******************************************************************************
*
* elcintr - Ethernet interface interrupt
*
* Hand off reading of packets from the interface to task level.
*/

LOCAL void elcintr 
    (
    int unit 
    )
    {
    FAST ELC_SOFTC *sc = &elc_softc[unit];

    sc->elcStat.interrupts++;
    sysBusIntAck (sc->intLevel);			/* acknowledge int */

    /* acknowledge interrupt, get Transmit/Receive status */

    sc->istat = sysInByte (ELC_INTSTAT (sc->elcAddr));
    sysOutByte (ELC_INTSTAT (sc->elcAddr), sc->istat);
    sc->tstat = sysInByte (ELC_TSTAT (sc->elcAddr));
    sc->rstat = sysInByte (ELC_RSTAT (sc->elcAddr));

    /* get xxxCount Reg that is cleared after read access */

    sc->elcStat.collisions += sysInByte (ELC_COLCNT (sc->elcAddr));
    sc->elcStat.aligns += sysInByte (ELC_ALICNT (sc->elcAddr));
    sc->elcStat.crcs += sysInByte (ELC_CRCCNT (sc->elcAddr));
    sc->elcStat.missed += sysInByte (ELC_MPCNT (sc->elcAddr));


    if (sc->istat & ISTAT_OVW) /* Receive buffer ring overwrite */
	{
        sysOutByte (ELC_BOUND(sc->elcAddr), sysInByte (ELC_BOUND(sc->elcAddr)));
	sc->elcStat.overwrite++;
        sc->es_if.if_ierrors++;
	}
    if (sc->istat & ISTAT_RXE) /* Receive-error */
	{
        sc->es_if.if_ierrors++;
	sc->elcStat.rerror++;
	if (sc->rstat & RSTAT_OVER)
	    sc->elcStat.overruns++;
	if (sc->rstat & RSTAT_DIS)
	    sc->elcStat.disabled++;
	if (sc->rstat & RSTAT_DFR)
	    sc->elcStat.deferring++;
	}
    if (sc->istat & ISTAT_PRX) /* Receive */
	{
        sc->current = elcGetCurr (unit);
	if ((sc->flags & RXING) == 0)
	    {
            sc->flags |= RXING;
	    (void) netJobAdd ((FUNCPTR)elcGet, unit, 0,0,0,0);
	    }
	if (sc->rstat & RSTAT_PRX)
	    sc->elcStat.rnoerror++;
	}

    if (sc->istat & (ISTAT_TXE | ISTAT_PTX)) /* Transmit-error, Transmit */
	{
	semGive (&elcSyncSem);
        if (sc->istat & ISTAT_TXE)
            {
            sc->es_if.if_oerrors++;
            sc->es_if.if_opackets--;
	    sc->elcStat.terror++;
            }
	if (sc->tstat & TSTAT_ABORT)
	    {
	    sc->elcStat.aborts++;
	    sc->elcStat.collisions += 16;
	    }
	if (sc->tstat & TSTAT_UNDER)
	    sc->elcStat.underruns++;
	if (sc->tstat & TSTAT_CDH)
	    sc->elcStat.heartbeats++;
	if (sc->tstat & TSTAT_OWC)
	    sc->elcStat.outofwindow++;
	if (sc->tstat & TSTAT_PTX)
	    sc->elcStat.tnoerror++;
	}
    }

/*******************************************************************************
*
* elcGet - read a packet off the interface
*
* Copy packets from a RING into an mbuf and hand it to the next higher layer.
*/

LOCAL void elcGet 
    (
    int unit
    )
    {
    FAST ELC_SOFTC *sc = &elc_softc[unit];
    FAST ELC_HEADER *pH;
    FAST struct ether_header *eh;
    FAST struct mbuf *m;
    int len;
#ifdef BSD43_DRIVER
    int off;
#endif
    int ix = 0;
    USHORT wrapSize;
    USHORT packetSize;
    char *pSrc;
    char *pDst;
    UCHAR *pData;
    UCHAR uppByteCnt;

unlucky:
    ix = 0;

    while ((sc->next != sc->current) && (ix++ < SMC8013WC_PSTOP))
	{
	/* Invalidate cache */

        cacheDrvInvalidate (&cacheDmaFuncs, 
		            (UCHAR *)(sc->memAddr + (SMC8013WC_PSTART << 8)),
		            (sc->memSize - (SMC8013WC_PSTART << 8)));

	pH = (ELC_HEADER *)(sc->memAddr + 
			   (((UINT)sc->next << 8) & 0x0000ffff));

	/* sanity check */

	if ((pH->next < SMC8013WC_PSTART) || (pH->next >= SMC8013WC_PSTOP))
	    {
	    sc->elcStat.badPacket++;
	    pH->next = sc->next + sc->uppByteCnt + 1;
	    if (pH->next >= SMC8013WC_PSTOP)
		pH->next = SMC8013WC_PSTART + (pH->next - SMC8013WC_PSTOP);
	    }

	if (*(USHORT *)(&pH->lowByteCnt) < 64)
	    {
	    sc->elcStat.shortPacket++;
	    goto doneGet;
	    }

        sc->es_if.if_ipackets++;        /* bump statistic */

	/* get uppByteCnt from pH->next because of the bug of a chip */

	if (pH->next > sc->next)
	    {
	    wrapSize = 0;
	    uppByteCnt = pH->next - sc->next;
	    }
	else
	    {
	    wrapSize = (SMC8013WC_PSTOP - sc->next) << 8;
	    uppByteCnt = (SMC8013WC_PSTOP - sc->next) + 
			 (pH->next - SMC8013WC_PSTART);
	    }
	if (pH->lowByteCnt > 0xfc)
	    uppByteCnt -= 2;
	else
	    uppByteCnt -= 1;
	packetSize = (uppByteCnt << 8) | pH->lowByteCnt;
	pH->uppByteCnt = uppByteCnt;
	if (wrapSize > packetSize)
	    wrapSize = 0;

	/* copy separated frame to a temporary buffer */

	if (wrapSize)
	    {
	    sc->elcStat.wrapped++;
	    pSrc = (char *)pH;
	    pDst = (char *)sc->receiveBuf;
	    bcopy (pSrc, pDst, wrapSize);
	    pSrc = (char *)(sc->memAddr + (SMC8013WC_PSTART << 8));
	    pDst += wrapSize;
	    len = packetSize - wrapSize;
	    if (len)
		bcopy (pSrc, pDst, len);
	    }

        len = packetSize - 4;			/* remove CRCs 4 bytes */

        if (wrapSize)
	    eh = (struct ether_header *)((UINT)sc->receiveBuf + 
		sizeof (ELC_HEADER));
        else
	    eh = (struct ether_header *)((UINT)pH + sizeof (ELC_HEADER));

        /* call input hook if any */

        if ((etherInputHookRtn != NULL) &&
	    (* etherInputHookRtn) (&sc->es_if, (char *) eh, len))
	    {
	    goto doneGet;
	    }

        if (len >= SIZEOF_ETHERHEADER)
            len -= SIZEOF_ETHERHEADER;
        else
            {
            sc->es_if.if_ierrors++;         /* bump error statistic */
	    goto doneGet;
            }

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

    if (sc->next <= SMC8013WC_PSTART)
	sysOutByte (ELC_BOUND (sc->elcAddr), SMC8013WC_PSTOP - 1);
    else
	sysOutByte (ELC_BOUND (sc->elcAddr), sc->next - 1);

    sc->flags &= ~RXING;

    /* go back if we received an interrupt and a new packet */

    if (sc->next != sc->current)
	goto unlucky;
    }

#ifdef BSD43_DRIVER
/*******************************************************************************
*
* elcoutput - ethernet output routine
*
* Encapsulate a packet of type family for the local net.
* Use trailer local net encapsulation if enough data in first
* packet leaves a multiple of 512 bytes of data in remainder.
* If destination is this address or broadcast, send packet to
* loop device to kludge around the fact that 3com interfaces can't
* talk to themselves.
*
* RETURNS: ?
*/

LOCAL int elcoutput 
    (
    struct ifnet *ifp,
    struct mbuf *m0,
    struct sockaddr *dst 
    )
    {
    return (ether_output (ifp, m0, dst, (FUNCPTR)elcPut,
			  &elc_softc [ifp->if_unit].es_ac));
    }
#endif

/*******************************************************************************
*
* elcPut - copy a packet to the interface.
*
* Copy from mbuf chain to transmitter buffer in shared memory.
*
*/

#ifdef BSD43_DRIVER
LOCAL void elcPut 
    (
    int unit
    )
    {
    FAST ELC_SOFTC *sc   = &elc_softc [unit];
#else
LOCAL void elcPut 
    (
    ELC_SOFTC *sc
    )
    {
#endif
    FAST struct mbuf *m;
    FAST int len;
    UINT transmitPage = sc->transmitPage[sc->transmitCnt & 1];
    UCHAR *pBuf = (UCHAR *)(sc->memAddr + (transmitPage << 8));
#ifdef BSD43_DRIVER
    int s = splnet ();

    if (sc->es_if.if_snd.ifq_head != NULL)
#else
    while (sc->es_if.if_snd.ifq_head != NULL)
#endif
	{
	semTake (&elcSyncSem, sysClkRateGet());

	IF_DEQUEUE (&sc->es_if.if_snd, m);

	copy_from_mbufs (pBuf, m, len);
	len = max (ETHERSMALL, len);

	cacheDrvFlush (&cacheDmaFuncs, pBuf, len);

	/* call output hook if any */

	if ((etherOutputHookRtn != NULL) &&
	    (* etherOutputHookRtn) (&sc->es_if, pBuf, len))
#ifdef BSD43_DRIVER
	    goto donePut;
#else
            continue;
#endif

	/* kick Transmitter */

        sysOutByte (ELC_INTMASK (sc->elcAddr), 0x00);
	sysOutByte (ELC_TSTART (sc->elcAddr), transmitPage);
	sysOutByte (ELC_TCNTH (sc->elcAddr), len >> 8);
	sysOutByte (ELC_TCNTL (sc->elcAddr), len & 0xff);
	sysOutByte (ELC_CMD (sc->elcAddr), CMD_TXP);
	sc->transmitCnt++;
        sysOutByte (ELC_INTMASK (sc->elcAddr), 
	    IM_OVWE | IM_TXEE | IM_PTXE | IM_PRXE);
#ifndef BSD43_DRIVER
        sc->es_if.if_opackets++;
#endif
	}

#ifdef BSD43_DRIVER
donePut:
    splx (s);
#endif
    }

/*******************************************************************************
*
* elcReset - reset the chip
*
* Reset the chip.
*
*/

LOCAL void elcReset 
    (
    int unit
    )
    {
    FAST ELC_SOFTC *sc   = &elc_softc [unit];
    int s = splnet ();

    /* mask interrupt */
    sysOutByte (ELC_INTMASK (sc->elcAddr), 0x00);

    bicInit (unit);
    elcInit (unit);

    /* unmask interrupt */
    sysOutByte (ELC_INTMASK (sc->elcAddr), 
	IM_OVWE | IM_TXEE | IM_PTXE | IM_PRXE);

    sc->elcStat.reset++;
    splx (s);
    }

/*******************************************************************************
*
* elcioctl - ioctl for interface
*
* RETURNS: ?
*/

LOCAL int elcioctl 
    (
    FAST struct ifnet *ifp,
    int cmd,
    caddr_t data 
    )
    {
    int error = 0;
    int s;

    s = splimp();

    switch (cmd)
	{
	case SIOCSIFADDR:
            ((struct arpcom *)ifp)->ac_ipaddr = IA_SIN (data)->sin_addr;
            arpwhohas (ifp, &IA_SIN (data)->sin_addr);
	    break;

	case SIOCSIFFLAGS:
            /* Handled outside module - nothing more to do. */
	    break;

	default:
	    error = EINVAL;
	}

    splx (s);
    return (error);
    }

/*******************************************************************************
*
* bicInit - initialize SMC83c584 BIC (Bus Interface Controller)
*
* RETURNS: N/A
*
* NOMANUAL
*/

LOCAL void bicInit 
    (
    int unit
    )
    {
    FAST ELC_SOFTC *sc	= &elc_softc[unit];
    IRQ_TABLE *p	= &irqTable[sc->intLevel];
    char laar = (sc->memAddr & 0xf80000) >> 19;
    char msr  = (sc->memAddr & 0x07e000) >> 13;
    char iar  = (sc->bicAddr & 0xe000) >> 8 | (sc->bicAddr & 0x03e0) >> 5;
    char irr;
    char icr;
    char eeromIrr;

    /* get IRQ level bits */

    if (p->ir2 == 0xff)
        p = &irqTable[3];		/* default is IRQ3 */

    irr  = p->ir01 << 5;
    icr  = p->ir2 << 2;

    /* Reset and Recall */

    sysOutByte (BIC_MSR (sc->bicAddr), MSR_RST);
    sysOutByte (BIC_MSR (sc->bicAddr), 0);
    taskDelay (sysClkRateGet () >> 2);
    sysOutByte (BIC_ICR (sc->bicAddr), ICR_RLA | ICR_RX7);
    while (sysInByte (BIC_ICR (sc->bicAddr)) & (ICR_RLA | ICR_RX7))
	;

    /* get values set by EEROM */

    eeromIrr = sysInByte (BIC_IRR (sc->bicAddr));

    /* Memory Select Reg: Enable, 0x??000. */

    sysOutByte (BIC_MSR (sc->bicAddr), MSR_MEN | msr);

    /* Interface Config Reg: 16Kbytes, 16Bit. */

    sysOutByte (BIC_ICR (sc->bicAddr), ICR_BIT16 | icr);

    /* IO Address Reg: 0x??0 */

    sysOutByte (BIC_IAR (sc->bicAddr), iar);

    /* BIOS ROM Address Reg: Disable */

    sysOutByte (BIC_BIO (sc->bicAddr), 0x00);

    /* Interrupt Request Reg: Enable, IRQ? */

    if (sc->config == 1)
        sysOutByte (BIC_IRR (sc->bicAddr), IRR_IEN | irr | IRR_OUT1);
    else if (sc->config == 2)
        sysOutByte (BIC_IRR (sc->bicAddr), IRR_IEN | irr | IRR_OUT1 | IRR_OUT2);
    else
        sysOutByte (BIC_IRR (sc->bicAddr), IRR_IEN | irr | IRR_OUT1 |
		    (eeromIrr & 0x0e));

    /* LA Address Reg: 16Bit-memory, 16Bit-lan, 0x??000 */

    sysOutByte (BIC_LAAR (sc->bicAddr), LAAR_M16EN | LAAR_L16EN | laar);

    }

/*******************************************************************************
*
* elcInit - initialize SMC83c690 ELC (Ethernet LAN Controller)
*
* RETURNS: N/A
*
* NOMANUAL
*/

LOCAL void elcInit 
    (
    int unit
    )
    {
    FAST ELC_SOFTC *sc          = &elc_softc[unit];

    /* 1. program Command Register for page 0 */

    sysOutByte (ELC_CMD (sc->elcAddr), CMD_CMD5 | CMD_STP);
    while ((sysInByte (ELC_INTSTAT (sc->elcAddr)) & ISTAT_RST) != ISTAT_RST)
	;

    /* 2. initialize Data Configuration Register */

    sysOutByte (ELC_DCON (sc->elcAddr), DCON_BSIZE1 | DCON_BUS16 | 0x08);

    /* 3. clear Remote Byte Count Register */

    sysOutByte (ELC_RBCR0 (sc->elcAddr), 0x00);
    sysOutByte (ELC_RBCR1 (sc->elcAddr), 0x00);

    /* 4. initialize Receive Configuration Register */

    sysOutByte (ELC_RCON (sc->elcAddr), 0x04);

    /* 5. place the ELC in LOOPBACK mode 1 or 2 */

    sysOutByte (ELC_TCON (sc->elcAddr), TCON_LB1);

    /* 6. initialize Receive Buffer Ring */

    sysOutByte (ELC_RSTART (sc->elcAddr), SMC8013WC_PSTART);
    sysOutByte (ELC_RSTOP (sc->elcAddr), SMC8013WC_PSTOP);
    sysOutByte (ELC_BOUND (sc->elcAddr), SMC8013WC_PSTART);

    /* 7. clear Interrupt Status Register */

    sysOutByte (ELC_INTSTAT (sc->elcAddr), 0xff);

    /* 8. initialize Interrupt Mask Register */

    sysOutByte (ELC_INTMASK (sc->elcAddr), 0x00);

    /* 9. program Command Register for page 1 */

    sysOutByte (ELC_CMD (sc->elcAddr), CMD_PS0 | CMD_STP);
    sysOutByte (ELC_STA0 (sc->elcAddr), sysInByte (BIC_LAR0 (sc->bicAddr)));
    sysOutByte (ELC_STA1 (sc->elcAddr), sysInByte (BIC_LAR1 (sc->bicAddr)));
    sysOutByte (ELC_STA2 (sc->elcAddr), sysInByte (BIC_LAR2 (sc->bicAddr)));
    sysOutByte (ELC_STA3 (sc->elcAddr), sysInByte (BIC_LAR3 (sc->bicAddr)));
    sysOutByte (ELC_STA4 (sc->elcAddr), sysInByte (BIC_LAR4 (sc->bicAddr)));
    sysOutByte (ELC_STA5 (sc->elcAddr), sysInByte (BIC_LAR5 (sc->bicAddr)));
    sysOutByte (ELC_MAR0 (sc->elcAddr), 0);
    sysOutByte (ELC_MAR1 (sc->elcAddr), 0);
    sysOutByte (ELC_MAR2 (sc->elcAddr), 0);
    sysOutByte (ELC_MAR3 (sc->elcAddr), 0);
    sysOutByte (ELC_MAR4 (sc->elcAddr), 0);
    sysOutByte (ELC_MAR5 (sc->elcAddr), 0);
    sysOutByte (ELC_MAR6 (sc->elcAddr), 0);
    sysOutByte (ELC_MAR7 (sc->elcAddr), 0);
    sc->next = SMC8013WC_PSTART + 1;
    sysOutByte (ELC_CURR (sc->elcAddr), sc->next);

    sysOutByte (ELC_CMD (sc->elcAddr), CMD_PS1 | CMD_STP);
    sysOutByte (ELC_ENH (sc->elcAddr), 0x00);	/* 0 wait states */
    sysOutByte (ELC_BLOCK (sc->elcAddr), 0x00);	/* 0x00xxxx */

    /* 10. put the ELC in START mode */

    sysOutByte (ELC_CMD (sc->elcAddr), CMD_STA);

    /* 11. initialize Transmit Configuration Register */

    sysOutByte (ELC_TCON (sc->elcAddr), 0x00);

    }

/*******************************************************************************
*
* elcGetAddr - get hardwired ethernet address.
*
* Read the ethernet address off the board, one byte at a time.
*
*/

LOCAL void elcGetAddr 
    (
    int unit 
    )
    {
    FAST ELC_SOFTC *sc          = &elc_softc[unit];
    UCHAR *pAddr		= (UCHAR *)sc->es_enaddr;

    *pAddr++ = sysInByte (BIC_LAR0 (sc->bicAddr));
    *pAddr++ = sysInByte (BIC_LAR1 (sc->bicAddr));
    *pAddr++ = sysInByte (BIC_LAR2 (sc->bicAddr));
    *pAddr++ = sysInByte (BIC_LAR3 (sc->bicAddr));
    *pAddr++ = sysInByte (BIC_LAR4 (sc->bicAddr));
    *pAddr   = sysInByte (BIC_LAR5 (sc->bicAddr));
    }

/*******************************************************************************
*
* elcGetCurr - get current page
*
* RETURNS: current page that ELC is working on
*
* NOMANUAL
*/

LOCAL UCHAR elcGetCurr 
    (
    int unit
    )
    {
    FAST ELC_SOFTC *sc          = &elc_softc[unit];
    UCHAR curr;

    /* get CURR register */

    sysOutByte (ELC_CMD (sc->elcAddr), CMD_PS0);
    curr = sysInByte (ELC_CURR (sc->elcAddr));
    sysOutByte (ELC_CMD (sc->elcAddr), 0);

    return (curr);
    }

/*******************************************************************************
*
* elcShow - display statistics for the SMC 8013WC `elc' network interface
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

void elcShow 
    (
    int unit,   /* interface unit */
    BOOL zap    /* 1 = zero totals */
    )
    {
    FAST ELC_SOFTC *sc          = &elc_softc[unit];
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
	"t-no-error",
	"r-no-error",
	"t-error",
	"r-error",
	"over-write",
	"wrapped",
	"interrupts",
	"reset",
	"stray-int"};

    for (ix = 0; ix < NELEMENTS(e_message); ix++)
	{
	printf ("    %-30.30s  %4d\n", e_message [ix], sc->elcStat.stat [ix]);
	if (zap)
	    sc->elcStat.stat [ix] = 0;
	}
    printf ("    %-30.30s  0x%2x\n", "flags", sc->flags);
    }

/*******************************************************************************
*
* elcdetach - detach the card from the bus.
*
* Dettach the card from the bus for reset.
*
* RETURNS: N/A
*
* NOMANUAL
*/

void elcdetach 
    (
    int unit
    )
    {
    FAST ELC_SOFTC *sc	= &elc_softc[unit];

    if (elcAttached)
	{
        sysOutByte (ELC_INTMASK (sc->elcAddr), 0x00);
        sysOutByte (ELC_CMD (sc->elcAddr), CMD_CMD5 | CMD_STP);
        while ((sysInByte (ELC_INTSTAT (sc->elcAddr)) & ISTAT_RST) != ISTAT_RST)
	    ;
        sysOutByte (ELC_RCON (sc->elcAddr), 0x00);
        sysOutByte (ELC_TCON (sc->elcAddr), TCON_LB1);
        sysIntDisablePIC (sc->intLevel);
        sysOutByte (BIC_MSR (sc->bicAddr), MSR_RST);
        taskDelay (sysClkRateGet () >> 1);
        sysOutByte (BIC_MSR (sc->bicAddr), 0x00);
        taskDelay (sysClkRateGet () >> 1);
	}
    }

