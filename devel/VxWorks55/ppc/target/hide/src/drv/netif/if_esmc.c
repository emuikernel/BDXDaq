/* if_esmc.c - Ampro Ethernet2 SMC-91c9x Ethernet network interface driver */

/* Copyright 1984-1997 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01d,15jul97,spm  removed driver initialization from ioctl support (SPR #8831)
01c,15may97,spm  reverted to bcopy routines for mbufs in BSD 4.4
01b,07apr97,spm  code cleanup, corrected statistics, and upgraded to BSD 4.4
01a,07may96,hdn  written.
*/

/*
DESCRIPTION
This module implements the Ampro Ethernet2 SMC-91c9x Ethernet network
interface driver.

CONFIGURATION
The W3 and W4 jumper should be set for IO address and IRQ.
The defined I/O address and IRQ in config.h must match the one stored
in EEROM and the jumper setting. 

BOARD LAYOUT
The diagram below shows the relevant jumpers for VxWorks configuration.

.bS
        ___________________________________
        |             * * * *             |
        |  ______                         |
        |  |    |                         |
        |  | U1 | W1  W3                  |
        |  |PROM| X   "                   |
        |  |    | .   -                   |
        |  |    |     -                   |
        |  |    |     -                   |
        |  |____|                         |
        |                                 |
        |                W4               |
        |                "                |
        |                "                |
        |                -                |
        |                -                |
        |_________________________________|    

      W1:  Boot PROM Size
      W3:  IO-address, IRQ, Media
      W4:  IRQ Group Selection
.bE

EXTERNAL INTERFACE
The only user-callable routines are esmcattach():
.iP esmcattach() 14
publishes the `esmc' interface and initializes the driver and device.
.LP
The last parameter of esmcattach(), <mode>, is a receive mode.
If it is 0, a packet is received in the interrupt level.  If it is 1,
a packet is received in the task level.  Receiving packets in the interrupt
level requires about 10K bytes of memory, but minimize a risk of dropping 
packets.  Receiving packets in the task level doesn't require extra
memory, but might have a risk of dropping packets.

INTERNAL
A driver implementation for this board faces two conflicting network
interface design goals:  (1) minimizing the time spent at interrupt level,
and (2) servicing the board's input without dropping packets.
Receive overrun affects significantly for the performance.  It depends
on CPU power and network traffic of your hardware and software.
And also it depends on the memory size of the chip, 91cxx series.
Importance of performance penalty for dropping packets by the receive
overrun and time spent at the interrupt level is different for each person.  
So there is a configurable flag to decide a receive mode which is the
last parameter of esmcattach() function.

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
#include "tickLib.h"
#include "drv/netif/if_esmc.h"

#undef	ESMC_DEBUG    /* define to include debug messages */

#define	NESMC			2	/* max number of ESMC controllers */
#define	ESMC_SHOW		TRUE	/* show routine */
#define	ESMC_USE_LONG		TRUE	/* use long word IO access */
#define ATTACHMENT_DEFAULT      0       /* use card as configured */
#define ATTACHMENT_AUI          1       /* AUI  (thick, DIX, DB-15) */
#define ATTACHMENT_BNC          2       /* BNC  (thin, 10BASE-2) */
#define ATTACHMENT_RJ45         3       /* RJ-45 (twisted pair, TPE, 10BASE-T)*/
#define RX_MODE_INTERRUPT	0	/* receive in interrupt level */
#define RX_MODE_TASK		1	/* receive in task level */


/* globals */


/* locals */

LOCAL ESMC_SOFTC esmc_softc [NESMC];
LOCAL char * pName[12] =
    {
    NULL,       NULL,       NULL, "SMC91C90/91C92",
    "SMC91C94",	"SMC91C95", NULL, "SMC91C100", 
    NULL,       NULL,       NULL, NULL
    };  


/* forward static functions */

LOCAL int    esmcInit		(int unit);
LOCAL void   esmcReset		(int unit);
LOCAL int    esmcIoctl		(struct ifnet *ifp, int cmd, caddr_t data);
#ifdef BSD43_DRIVER
LOCAL int    esmcOutput		(struct ifnet *ifp, struct mbuf *m0, 
				 struct sockaddr *dst);
#endif
LOCAL void   esmcIntr		(int unit);
LOCAL void   esmcGet		(int unit);
LOCAL void   esmcGetInt		(int unit);
LOCAL void   esmcGetTask	(int unit);
#ifdef BSD43_DRIVER
LOCAL void   esmcPut		(int unit);
#else
LOCAL void   esmcPut		(ESMC_SOFTC *sc);
#endif
LOCAL STATUS esmcChipInit	(int unit);
LOCAL void   esmcChipReset	(int unit);


/*******************************************************************************
*
* esmcattach - publish the `esmc' network interface and initialize the driver.
*
* This routine attaches an `esmc' Ethernet interface to the network if the
* device exists.  It makes the interface available by filling in the network
* interface record.  The system will initialize the interface when it is ready
* to accept packets.
*
* RETURNS: OK or ERROR.
*
* SEE ALSO: ifLib, netShow
*/

STATUS esmcattach 
    (
    int unit,           /* unit number */
    int ioAddr,         /* address of esmc's shared memory */
    int intVec,         /* interrupt vector to connect to */
    int intLevel,       /* interrupt level */
    int config,		/* 0: Autodetect  1: AUI  2: BNC  3: RJ45 */
    int mode		/* 0: rx in interrupt   1: rx in task(netTask) */
    )
    {
    FAST ESMC_SOFTC *sc = &esmc_softc[unit];

    if ((UINT)unit >= NESMC)
	{
        errnoSet (S_iosLib_CONTROLLER_NOT_PRESENT);
	return (ERROR);
	}

    sc->ioAddr		= ioAddr;
    sc->intVec		= intVec;
    sc->intLevel	= intLevel;
    sc->config		= config;
    sc->flags		= 0;
    sc->mode		= mode;
    sc->pBuft		= (char *)(((u_int)sc->buft + 3) & ~0x03);

    if (sc->mode == RX_MODE_INTERRUPT)			/* RX int */
	{
        sc->bufi	= malloc (ESMC_BUFSIZE_INT + 8);
        sc->pBufi	= (char *)(((u_int)sc->bufi + 3) & ~0x03);
        sc->pStart	= sc->pBufi;			/* start pointer */
        sc->pEnd	= sc->pBufi + ESMC_BUFSIZE_INT;	/* end pointer */
        sc->pIn		= sc->pBufi;			/* input pointer */
        sc->pOut	= sc->pBufi;			/* output pointer */
        sc->indexIn	= 0;				/* input index */
        sc->indexOut	= 0;				/* output index */
        sc->nIndex	= ESMC_INDEXES;			/* indexes */

        if (sc->bufi == NULL)
	    return (ERROR);

        bzero (sc->pBufi, ESMC_BUFSIZE_INT);
	}

    semBInit (&sc->esmcSyncSem, SEM_Q_FIFO, SEM_EMPTY);

    (void) intConnect ((VOIDFUNCPTR *)INUM_TO_IVEC (intVec), 
		       (VOIDFUNCPTR)esmcIntr, unit);

#ifdef BSD43_DRIVER
    ether_attach (&sc->es_if, unit, "esmc", (FUNCPTR)esmcInit, 
	  (FUNCPTR)esmcIoctl, (FUNCPTR)esmcOutput, (FUNCPTR)esmcReset);
#else
    ether_attach (
                 &sc->es_if, 
                 unit, 
                 "esmc", 
                 (FUNCPTR)esmcInit, 
	         (FUNCPTR)esmcIoctl, 
                 (FUNCPTR)ether_output, 
                 (FUNCPTR)esmcReset
                 );
    sc->es_if.if_start = (FUNCPTR)esmcPut;
#endif

    esmcInit (unit);

    return (OK);
    }
/*******************************************************************************
*
* esmcReset - reset the interface
*
* RETURNS: N/A
*/

LOCAL void esmcReset 
    (
    int unit 
    )
    {
    FAST ESMC_SOFTC *sc   = &esmc_softc [unit];
    int s = splnet ();

    sysIntDisablePIC (sc->intLevel);
    sc->es_if.if_flags = 0;

    splx (s);
    }
/*******************************************************************************
*
* esmcInit - initialize esmc
*
* Restart the link level software on the board and mark the interface up.
*
* RETURNS: 0
*/

LOCAL int esmcInit 
    (
    int unit 
    )
    {
    FAST ESMC_SOFTC *sc = &esmc_softc[unit];
    int s = splnet ();

    esmcChipInit (unit);
    sysIntEnablePIC (sc->intLevel);
    sc->es_if.if_flags |= IFF_UP|IFF_RUNNING;	/* open for business*/

    splx (s);
    return (0);
    }

#ifdef BSD43_DRIVER
/*******************************************************************************
*
* esmcOutput - ethernet output routine
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

LOCAL int esmcOutput 
    (
    struct ifnet *ifp,
    struct mbuf *m0,
    struct sockaddr *dst 
    )
    {
    return (ether_output (ifp, m0, dst, (FUNCPTR)esmcPut,
			  &esmc_softc [ifp->if_unit].es_ac));
    }
#endif

/*******************************************************************************
*
* esmcIoctl - ioctl for interface
*
* RETURNS: OK if successful, otherwise errno.
*/

LOCAL int esmcIoctl 
    (
    FAST struct ifnet *ifp,
    int cmd,
    caddr_t data 
    )
    {
    FAST ESMC_SOFTC *sc = &esmc_softc[ifp->if_unit];
    int status = OK;
    int s = splimp();
    short flags;

    switch (cmd)
	{
	case SIOCSIFADDR:
            ((struct arpcom *)ifp)->ac_ipaddr = IA_SIN (data)->sin_addr;
            arpwhohas (ifp, &IA_SIN (data)->sin_addr);
	    break;

	case SIOCSIFFLAGS:
            flags = ifp->if_flags;

	    if (ifp->if_flags & IFF_UP)
                ifp->if_flags = flags | (IFF_UP|IFF_RUNNING);
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
* esmcIntr - Ethernet interface interrupt
*
* Hand off reading of packets from the interface to task level.
*
* RETURNS: N/A
*/

LOCAL void esmcIntr 
    (
    int unit 
    )
    {
    FAST ESMC_SOFTC *sc = &esmc_softc[unit];
    FAST int ioaddr     = sc->ioAddr;
    int timeout         = ESMC_INTR_TIMEOUT;
    u_short bank;
    u_short pointerSave;
    u_short statusTx;
    u_short statusCard;
    u_short packetFail;
    u_char packetSave;
    u_char mask;
    u_char status;

    sc->interrupt++;
    bank = sysInWord (ioaddr + ESMC_BANK_SELECT);	/* save bank */
    ESMC_SWITCH_BANK (2);
    mask = sysInByte (ioaddr + ESMC_INT_MASK);		/* save mask */

    while ((status = sysInByte (ioaddr + ESMC_INT_STAT) & mask) && (timeout--))
	{
	if (status & ESMC_INT_RCV)			/* RX done */
	    {
            sc->intRcv++;
	    if (sc->mode == RX_MODE_INTERRUPT)		/* RX in int */
		{
		esmcGetInt (unit);
	        if ((sc->flags & ESMC_RXING) == 0)
		    {
		    sc->flags |= ESMC_RXING;
		    (void) netJobAdd ((FUNCPTR)esmcGetTask, unit, 0, 0, 0, 0);
		    }
		}
	    else if ((sc->flags & ESMC_RXING) == 0)	/* RX in task */
		{
		sc->flags |= ESMC_RXING;
		(void) netJobAdd ((FUNCPTR)esmcGet, unit, 0, 0, 0, 0);
		}
	    }
	
	if (status & ESMC_INT_TX)			/* TX error */
	    {
            sc->es_if.if_oerrors++;
            sc->es_if.if_opackets--;
            sc->intTx++;
    	    pointerSave = sysInWord (ioaddr + ESMC_PTR); /* save pointer */
	    packetSave  = sysInByte (ioaddr + ESMC_PNR); /* save packet */

	    packetFail  = sysInWord (ioaddr + ESMC_FIFO) & 0x7f;
	    sysOutByte (ioaddr + ESMC_PNR, packetFail);
	    sysOutWord (ioaddr + ESMC_PTR, ESMC_PTR_AUTOINC | ESMC_PTR_READ);
	    statusTx = sysInWord (ioaddr + ESMC_DATA_1);
	    if (statusTx & ESMC_TS_LOST_CARR)
		sc->lostCarr++;
	    if (statusTx & ESMC_TS_LATCOL)
		sc->es_if.if_collisions++;
	    
	    sysOutWord (ioaddr + ESMC_MMU, ESMC_MMU_TX_RELEASE); /* release */
	    sysOutByte (ioaddr + ESMC_INT_ACK, ESMC_INT_TX);	 /* int ack */
	    ESMC_SWITCH_BANK (0);
	    sysOutWord (ioaddr + ESMC_TCR, 
			sysInWord (ioaddr + ESMC_TCR) | ESMC_TCR_TXEN);
	    ESMC_SWITCH_BANK (2);

	    sysOutByte (ioaddr + ESMC_PNR, packetSave);	 /* restore packet */
            sysOutWord (ioaddr + ESMC_PTR, pointerSave); /* restore pointer */
	    }

	if (status & ESMC_INT_TX_EMPTY)			/* TX done */
	    {
            sc->intTxempty++;
	    sysOutByte (ioaddr + ESMC_INT_ACK, ESMC_INT_TX_EMPTY); /* int ack */

	    ESMC_SWITCH_BANK (0);
	    statusCard = sysInWord (ioaddr + ESMC_COUNTER);
    	    ESMC_SWITCH_BANK (2);
	    sc->es_if.if_collisions += statusCard & 0x0f;
	    statusCard >>= 4;
	    sc->es_if.if_collisions += statusCard & 0x0f;

	    mask &= ~ESMC_INT_TX_EMPTY;
	    if (sc->es_if.if_snd.ifq_head != NULL)
#ifdef BSD43_DRIVER
		(void) netJobAdd ((FUNCPTR)esmcPut, unit, 0, 0, 0, 0);
#else
		(void) netJobAdd ((FUNCPTR)esmcPut, (int)sc, 0, 0, 0, 0);
#endif
	    }
	
	if (status & ESMC_INT_ALLOC)			/* ALLOC done */
	    {
            sc->intAlloc++;
	    mask &= ~ESMC_INT_ALLOC;
	    semGive (&sc->esmcSyncSem);
	    }

	if (status & ESMC_INT_RX_OVRN)			/* RX over run */
	    {
            sc->es_if.if_ierrors++;
            sc->intOverrun++;
            sysOutByte (ioaddr + ESMC_INT_ACK, ESMC_INT_RX_OVRN); /* int ack */
	    if (sc->mode == RX_MODE_INTERRUPT)		/* RX in int */
		{
		esmcGetInt (unit);
	        if ((sc->flags & ESMC_RXING) == 0)
		    {
		    sc->flags |= ESMC_RXING;
		    (void) netJobAdd ((FUNCPTR)esmcGetTask, unit, 0, 0, 0, 0);
		    }
		}
	    else if ((sc->flags & ESMC_RXING) == 0)	/* RX in task */
		{
		sc->flags |= ESMC_RXING;
		(void) netJobAdd ((FUNCPTR)esmcGet, unit, 0, 0, 0, 0);
		}
	    }
	
	if (status & ESMC_INT_EPH)			/* EPH */
            sc->intEph++;

	if (status & ESMC_INT_ERCV)			/* early receive */
            sc->intErcv++;
	}
    
    sysOutByte (ioaddr + ESMC_INT_MASK, mask);		/* restore the mask */
    ESMC_SWITCH_BANK (bank);				/* restore the bank */

    return;
    }
/*******************************************************************************
*
* esmcGet - read a packet off the interface(Task Level: netTask)
*
* Copy packets from a RING into an mbuf and hand it to the next higher layer.
*
* RETURNS: N/A
*/

LOCAL void esmcGet 
    (
    int unit
    )
    {
    FAST ESMC_SOFTC *sc = &esmc_softc[unit];
    FAST int ioaddr     = sc->ioAddr;
    FAST struct ether_header *eh;
    FAST struct mbuf *m;
    int off;
    int len;
    u_short packetNo;
    u_short statusRx;
    u_char * pData;
    u_char mask;

    ESMC_SWITCH_BANK (2);
    mask = sysInByte (ioaddr + ESMC_INT_MASK);		/* save mask */

unlucky:
    while (((packetNo = sysInWord (ioaddr + ESMC_FIFO)) & ESMC_FP_RXEMPTY) == 0)
	{
        sysOutByte (ioaddr + ESMC_INT_MASK, 0);		/* lock INT */

	sysOutWord (ioaddr + ESMC_PTR, 
		    ESMC_PTR_READ | ESMC_PTR_RCV | ESMC_PTR_AUTOINC);

	statusRx = sysInWord (ioaddr + ESMC_DATA_1);	/* status */
	len	 = sysInWord (ioaddr + ESMC_DATA_1);	/* byte count */
	len	 = (len & 0x07ff) - 6;			/* subtract extra */

	if (statusRx & ESMC_RS_ERROR_MASK)
	    {
	    ++sc->es_if.if_ierrors;
	    while (sysInWord (ioaddr + ESMC_MMU) & 0x0001) /* check busy bit */
		;
	    sysOutWord (ioaddr + ESMC_MMU, ESMC_MMU_RX_RELEASE); /* release */
            sysOutByte (ioaddr + ESMC_INT_MASK, mask);	/* unlock INT */
	    }
	else
	    {
#if	ESMC_USE_LONG
	    sysInLongString (ioaddr + ESMC_DATA_1, (long *)sc->pBuft, len >> 2);
	    if (len & 0x02)
	        *(short *)&sc->pBuft[len & ~0x03] = 
		                              sysInWord (ioaddr + ESMC_DATA_1);
	    *(short *)&sc->pBuft[len & ~0x01] = 
		                              sysInWord (ioaddr + ESMC_DATA_1);
#else
	    sysInWordString (ioaddr + ESMC_DATA_1, (short *)sc->pBuft,
		             (len>>1) + 1);
#endif	/* ESMC_USE_LONG */

	    if (statusRx & ESMC_RS_ODDFRM)
		len++;
	    while (sysInWord (ioaddr + ESMC_MMU) & 0x0001) /* check busy bit */
		;
	    sysOutWord (ioaddr + ESMC_MMU, ESMC_MMU_RX_RELEASE); /* release */
            sysOutByte (ioaddr + ESMC_INT_MASK, mask);	/* unlock INT */

#ifdef ESMC_DEBUG
	    {
	    int ix;
	    int iy;
	    printf ("packet=0x%x, status=0x%x, len=%d\n", 
		    (packetNo >> 8) & 0x1f, statusRx, len);
	    for (iy = 0; iy < 8; iy++)
	        {
                for (ix = 0; ix < 16; ix++)
	            printf ("%2.2x ", (u_char)sc->pBuft[iy * 16 + ix]);
	        printf ("\n");
	        }
	    }
#endif	/* ESMC_DEBUG */

            sc->es_if.if_ipackets++;        /* bump statistic */

	    eh = (struct ether_header *)(sc->pBuft);

            /* call input hook if any */

            if ((etherInputHookRtn != NULL) &&
	        (* etherInputHookRtn) (&sc->es_if, (char *) eh, len))
	        {
	        continue;
	        }

            if (len >= SIZEOF_ETHERHEADER)
                len -= SIZEOF_ETHERHEADER;
            else
                {
                sc->es_if.if_ierrors++;         /* bump error statistic */
	        continue;
                }

            pData = ((unsigned char *) eh) + SIZEOF_ETHERHEADER;

#ifdef BSD43_DRIVER
            check_trailer (eh, pData, &len, &off, &sc->es_if);

            /* copy data from the ring buffer to mbuf a long-word at a time */

            m = bcopy_to_mbufs (pData, len, off, (struct ifnet *)&sc->es_if, 2);

            if (m == NULL)
                {
                sc->es_if.if_ierrors++;        /* bump error statistic */
	        continue;
                }

            do_protocol_with_type (eh->ether_type, m, &sc->es_ac, len);
#else
            /* copy data from the ring buffer to mbuf */

            m = bcopy_to_mbufs (pData, len, off, (struct ifnet *)&sc->es_if, 2);

            if (m == NULL)
                {
                sc->es_if.if_ierrors++;        /* bump error statistic */
	        continue;
                }

            do_protocol (eh, m, &sc->es_ac, len);
#endif
	    }
        }

    sc->flags &= ~ESMC_RXING;

    /* go back if we got an interrupt and a new packet */

    if ((sysInByte (ioaddr + ESMC_INT_STAT) & (ESMC_INT_RCV | ESMC_INT_RX_OVRN))
	 || (((sysInWord (ioaddr + ESMC_FIFO)) & ESMC_FP_RXEMPTY) == 0))
	goto unlucky;
    }

/*******************************************************************************
*
* esmcGetInt - read a packet off the interface(Interrupt Level)
*
* Copy packets from a RING into an mbuf and hand it to the next higher layer.
*
* RETURNS: N/A
*/

LOCAL void esmcGetInt 
    (
    int unit
    )
    {
    FAST ESMC_SOFTC *sc = &esmc_softc[unit];
    FAST int ioaddr     = sc->ioAddr;
    int len;
    int bufLen;
    u_short statusRx;
    u_short packetNo;


    if ((sc->pIn > sc->pOut) && ((sc->pEnd - sc->pIn) < ESMC_BUFSIZE))
	sc->pIn = sc->pStart;

    if (((sc->pOut > sc->pIn) && ((sc->pOut - sc->pIn) < ESMC_BUFSIZE)) ||
        (sc->packetLen[sc->indexIn] != 0))
	return;

    while (((packetNo = sysInWord (ioaddr + ESMC_FIFO)) & ESMC_FP_RXEMPTY) == 0)
	{
	sysOutWord (ioaddr + ESMC_PTR, 
		    ESMC_PTR_READ | ESMC_PTR_RCV | ESMC_PTR_AUTOINC);

	statusRx = sysInWord (ioaddr + ESMC_DATA_1);	/* status */
	len	 = sysInWord (ioaddr + ESMC_DATA_1);	/* byte count */
	len	 = (len & 0x07ff) - 6;			/* subtract extra */

	if (statusRx & ESMC_RS_ERROR_MASK)
	    {
	    ++sc->es_if.if_ierrors;
	    while (sysInWord (ioaddr + ESMC_MMU) & 0x0001) /* check busy bit */
		;
	    sysOutWord (ioaddr + ESMC_MMU, ESMC_MMU_RX_RELEASE); /* release */
	    }
	else
	    {
#if	ESMC_USE_LONG
	    sysInLongString (ioaddr + ESMC_DATA_1, (long *)sc->pIn, len >> 2);
	    if (len & 0x02)
	        *(short *)&sc->pIn[len & ~0x03] = 
					      sysInWord (ioaddr + ESMC_DATA_1);
	    *(short *)&sc->pIn[len & ~0x01] = sysInWord (ioaddr + ESMC_DATA_1);
#else
	    sysInWordString (ioaddr + ESMC_DATA_1, (short *)sc->pIn, 
			     (len>>1) + 1);
#endif	/* ESMC_USE_LONG */

	    if (statusRx & ESMC_RS_ODDFRM)
		len++;
	    while (sysInWord (ioaddr + ESMC_MMU) & 0x0001) /* check busy bit */
		;
	    sysOutWord (ioaddr + ESMC_MMU, ESMC_MMU_RX_RELEASE); /* release */

	    if (len & 0x01)				/* buffer length */
		bufLen = len + 1;
	    else
		bufLen = len + 2;
	    sc->packetAddr[sc->indexIn] = sc->pIn;	/* packet address */
	    sc->packetLen[sc->indexIn] = len;		/* packet length */
	    sc->pIn += bufLen;				/* increment */
	    sc->indexIn = (sc->indexIn + 1) % sc->nIndex;

            if ((sc->pIn > sc->pOut) && ((sc->pEnd - sc->pIn) < ESMC_BUFSIZE))
	        sc->pIn = sc->pStart;			/* wrap around */

            if (((sc->pOut > sc->pIn) && ((sc->pOut - sc->pIn) < ESMC_BUFSIZE))
		|| (sc->packetLen[sc->indexIn] != 0))	/* no space */
	        break;
	    }
        }
    }

/*******************************************************************************
*
* esmcGetTask - read a packet off the interface(Task Level)
*
* Copy packets from a RING into an mbuf and hand it to the next higher layer.
*
* RETURNS: N/A
*/

LOCAL void esmcGetTask 
    (
    int unit
    )
    {
    FAST ESMC_SOFTC *sc = &esmc_softc[unit];
    FAST struct ether_header *eh;
    FAST struct mbuf *m;
    int off;
    int len;
    int bufLen;
    u_char * pData;

    while ((len = sc->packetLen[sc->indexOut]) != 0)
	{
	eh = (struct ether_header *)(sc->packetAddr[sc->indexOut]);

        sc->es_if.if_ipackets++;        /* bump statistic */

        /* call input hook if any */

        if ((etherInputHookRtn != NULL) &&
	    (* etherInputHookRtn) (&sc->es_if, (char *) eh, len))
	    continue;

        if (len >= SIZEOF_ETHERHEADER)
            len -= SIZEOF_ETHERHEADER;
        else
            {
            sc->es_if.if_ierrors++;         /* bump error statistic */
	    continue;
            }

        pData = ((unsigned char *) eh) + SIZEOF_ETHERHEADER;

#ifdef BSD43_DRIVER
        check_trailer (eh, pData, &len, &off, &sc->es_if);

        /* copy data from the ring buffer to mbuf a long-word at a time */

        m = bcopy_to_mbufs (pData, len, off, (struct ifnet *)&sc->es_if, 2);

        if (m == NULL)
            {
            sc->es_if.if_ierrors++;         /* bump error statistic */
	    continue;
            }

        do_protocol_with_type (eh->ether_type, m, &sc->es_ac, len);
#else
        /* copy data from the ring buffer to mbuf */

        m = bcopy_to_mbufs (pData, len, off, (struct ifnet *)&sc->es_if, 2);

        if (m == NULL)
            {
            sc->es_if.if_ierrors++;         /* bump error statistic */
	    continue;
            }

        do_protocol (eh, m, &sc->es_ac, len);
#endif

	if (len & 0x01)					/* buffer length */
	    bufLen = len + 1;
	else
	    bufLen = len + 2;
	sc->pOut = sc->packetAddr[sc->indexOut] + bufLen;
	sc->packetLen[sc->indexOut] = 0;		/* mark it empty */
	sc->indexOut = (sc->indexOut + 1) % sc->nIndex;	/* increment */
	}

    sc->flags &= ~ESMC_RXING;
    }

/*******************************************************************************
*
* esmcPut - copy a packet to the interface.
*
* Copy from mbuf chain to transmitter buffer in shared memory.
*
* RETURNS: N/A
*/

#ifdef BSD43_DRIVER
LOCAL void esmcPut 
    (
    int unit
    )
    {
    FAST ESMC_SOFTC *sc = &esmc_softc [unit];
#else
LOCAL void esmcPut 
    (
    ESMC_SOFTC *sc
    )
    {
#endif

    FAST int ioaddr     = sc->ioAddr;
    FAST struct mbuf *m;
    int timeout         = ESMC_ALLOC_TIMEOUT;
    int len;
    int pages;
    u_char mask;
    u_char packetNo;
    int s = splnet ();

    if (sc->es_if.if_snd.ifq_head != NULL)
	{
	IF_DEQUEUE (&sc->es_if.if_snd, m);

	copy_from_mbufs (sc->pBuft, m, len);
	len = max (ETHERSMALL, len);

	/* call output hook if any */

	if ((etherOutputHookRtn != NULL) &&
	    (* etherOutputHookRtn) (&sc->es_if, sc->pBuft, len))
	    {
	    goto donePut;
	    }

#ifndef BSD43_DRIVER
        sc->es_if.if_opackets++;        /* bump statistic. */
#endif

	/* allocate transmit memory and transmit */

	pages = len / 256;
	ESMC_SWITCH_BANK (2);
	mask = sysInByte (ioaddr + ESMC_INT_MASK);	/* save mask */

	sysOutWord (ioaddr + ESMC_MMU, ESMC_MMU_ALLOC | pages);	/* alloc mem */
	while (((sysInByte (ioaddr + ESMC_INT_STAT) & ESMC_INT_ALLOC) == 0) &&
	       (--timeout))
	    ;
	if (timeout == 0)				/* take sem & wait */
	    {
	    sysOutByte (ioaddr + ESMC_INT_MASK, mask | ESMC_INT_ALLOC);
	    if (semTake (&sc->esmcSyncSem, sysClkRateGet () >> 2) == ERROR)
		{
	        sc->allocTimeout++;
	        goto donePut;
	        }
	    }

	if ((packetNo = sysInByte (ioaddr + ESMC_ARR)) & 0x80) /* check it */
	    {
	    sc->allocFailed++;
	    goto donePut;
	    }

#ifdef ESMC_DEBUG
	{
	int ix;
	int iy;
	printf ("pages=0x%x, packetNo=0x%x, len=%d\n", pages, packetNo, len);
	for (iy = 0; iy < 8; iy++)
	    {
            for (ix = 0; ix < 16; ix++)
	        printf ("%2.2x ", (u_char)sc->pBuft[iy * 16 + ix]);
	    printf ("\n");
	    }
	}
#endif	/* ESMC_DEBUG */

	sysOutByte (ioaddr + ESMC_INT_MASK, 0);		/* lock INT */
	sysOutByte (ioaddr + ESMC_PNR, packetNo);	/* set the tx packet */
	sysOutWord (ioaddr + ESMC_PTR, ESMC_PTR_AUTOINC); /* set the pointer */

#if	ESMC_USE_LONG
	sysOutLong (ioaddr + ESMC_DATA_1, (len + 6) << 16); /* status, count */
	sysOutLongString (ioaddr + ESMC_DATA_1, (long *)sc->pBuft, len >> 2);
	if (len & 0x02)
	    sysOutWord (ioaddr + ESMC_DATA_1, *(short *)&sc->pBuft[len & ~0x03]);
#else
	sysOutWord (ioaddr + ESMC_DATA_1, 0);		/* status */
	sysOutWord (ioaddr + ESMC_DATA_1, (len + 6));	/* byte count */
	sysOutWordString (ioaddr + ESMC_DATA_1, (short *)sc->pBuft, len >> 1);
#endif	/* ESMC_USE_LONG */

	if (len & 0x01)					/* odd byte count */
	    {
	    sysOutByte (ioaddr + ESMC_DATA_1, sc->pBuft[len - 1]);
	    sysOutByte (ioaddr + ESMC_DATA_1, 0x20);
	    }
	else						/* even byte count */
	    sysOutWord (ioaddr + ESMC_DATA_1, 0);
	
	sysOutWord (ioaddr + ESMC_MMU, ESMC_MMU_ENQUEUE); /* send it */
	sysOutByte (ioaddr + ESMC_INT_MASK, 
		    mask | ESMC_INT_TX | ESMC_INT_TX_EMPTY);
	}

donePut:
    splx (s);
    }
/*******************************************************************************
*
* esmcChipInit - initialize SMC91c9x chip
*
* RETURNS: N/A
*/

LOCAL STATUS esmcChipInit 
    (
    int unit
    )
    {
    FAST ESMC_SOFTC *sc = &esmc_softc[unit];
    FAST int ioaddr     = sc->ioAddr;
    int memSize;
    int ix;
    u_short bank;
    u_short config;
    u_short addr;
    u_short memInfo;
    u_short memConfig;
    char revision;
    char * pRevision;

    sysOutWord (ioaddr + ESMC_BANK_SELECT, 0x00);
    bank = sysInWord (ioaddr + ESMC_BANK_SELECT);
    if ((bank & 0xff00) != 0x3300)
	{
#ifdef ESMC_DEBUG
        printf ("esmc: error - bank select 0x%x\n", bank & 0xff00);
#endif
	return (ERROR);
        }

    ESMC_SWITCH_BANK (1);
    addr = sysInWord (ioaddr + ESMC_BASE);
    if (ioaddr != ((addr >> 3) & 0x3e0))
	{
#ifdef ESMC_DEBUG
        printf ("esmc: error - ioaddr=0x%x\n", (addr >> 3) & 0x3e0);
#endif
	return (ERROR);
        }
    
    ESMC_SWITCH_BANK (3);
    revision = sysInByte (ioaddr + ESMC_REVISION);
    if ((pRevision = pName[(revision >> 4) & 0x0f]) == NULL)
	{
#ifdef ESMC_DEBUG
        printf ("esmc: error - revision=0x%x\n", (revision >> 4) & 0x0f);
#endif
	return (ERROR);
        }
    
    ESMC_SWITCH_BANK (0);
    memInfo = sysInWord (ioaddr + ESMC_MIR);
    memConfig = sysInWord (ioaddr + ESMC_MCR);
    memSize = (memConfig >> 9) & 0x07;
    memSize *= (memInfo & 0xff) * 256;

    esmcChipReset (unit);				/* reset the chip */

    ESMC_SWITCH_BANK (1);
    config = sysInWord (ioaddr + ESMC_CONFIG);
    if (sc->config == ATTACHMENT_DEFAULT)		/* EEPROM */
	{
	if (config & ESMC_CFG_AUI_SELECT)		/* EEPROM - AUI */
	    sc->config = ATTACHMENT_AUI;
	else						/* EEPROM - RJ45 */
	    {
	    sc->config = ATTACHMENT_RJ45;
	    sysOutByte (ioaddr + ESMC_CONFIG, config);
	    }
	}
    else if (sc->config == ATTACHMENT_AUI)		/* AUI */
	sysOutByte (ioaddr + ESMC_CONFIG, config | ESMC_CFG_AUI_SELECT);
    else if (sc->config == ATTACHMENT_RJ45)		/* RJ45 */
	sysOutByte (ioaddr + ESMC_CONFIG, config & ~ESMC_CFG_AUI_SELECT);
    
    for (ix = 0; ix < 6; ix++)				/* ethernet addres */
	sc->es_enaddr[ix] = sysInByte (ioaddr + ESMC_ADDR_0 + ix);
    
    ESMC_SWITCH_BANK (0);
    sysOutWord (ioaddr + ESMC_MCR, 0x0006);  		/* 1532 bytes */ 
    sysOutWord (ioaddr + ESMC_TCR, ESMC_TCR_PAD | ESMC_TCR_TXEN); /* TXenable */
    sysOutWord (ioaddr + ESMC_RCR, ESMC_RCR_RXEN);	/* RX enable */

    ESMC_SWITCH_BANK (2);				/* int enable */
    sysOutByte (ioaddr + ESMC_INT_MASK, ESMC_INT_RX_OVRN | ESMC_INT_RCV); 

#if	ESMC_DEBUG
    printf ("ESMC: %s(rev=%d) at %#3x IRQ=%d IF=%s MEM=%d ",
	    pRevision, revision & 0x0f, ioaddr, sc->intLevel,
	    (sc->config == 1)? "TP" : "AUI", memSize);
    printf ("(");
    for (ix = 0; ix < 5; ix++)
	printf ("%2.2x:", (u_char)sc->es_enaddr[ix]);
    printf ("%2.2x)\n", (u_char)sc->es_enaddr[ix]);
#endif	/* ESMC_DEBUG */

    return (OK);
    }
/*******************************************************************************
*
* esmcChipReset - reset SMC91c9x chip
*
* RETURNS: N/A
*/

LOCAL void esmcChipReset 
    (
    int unit
    )
    {
    FAST ESMC_SOFTC *sc = &esmc_softc[unit];
    FAST int ioaddr     = sc->ioAddr;

    ESMC_SWITCH_BANK (2);
    sysOutByte (ioaddr + ESMC_INT_MASK, 0);		/* lock INT */

    ESMC_SWITCH_BANK (0);
    sysOutWord (ioaddr + ESMC_RCR, ESMC_RCR_EPH_RST);	/* software reset */
    taskDelay (sysClkRateGet () >> 3);

    sysOutWord (ioaddr + ESMC_RCR, 0x0000);		/* RX disable */
    sysOutWord (ioaddr + ESMC_TCR, 0x0000);		/* TX disable */

    ESMC_SWITCH_BANK (1);
    sysOutWord (ioaddr + ESMC_CONTROL, 			/* TX auto release */
		sysInWord (ioaddr + ESMC_CONTROL) | ESMC_CTL_AUTO_RELEASE);

    ESMC_SWITCH_BANK (2);
    sysOutWord (ioaddr + ESMC_MMU, ESMC_MMU_RESET);	/* MMU reset */
    taskDelay (sysClkRateGet () >> 3);
    }

#if	ESMC_SHOW
/*******************************************************************************
*
* esmcShow - display statistics for the `esmc' network interface
*
* This routine displays statistics about the `esmc' Ethernet network interface.
* It has two parameters:
* .iP <unit>
* interface unit; should be 0.
* .iP <zap>
* if 1, all collected statistics are cleared to zero.
* .LP
*
* RETURNS: N/A
*/

void esmcShow 
    (
    int unit,   /* interface unit */
    BOOL zap    /* zero totals */
    )
    {
    FAST ESMC_SOFTC *sc = &esmc_softc[unit];
    FAST int ioaddr     = sc->ioAddr;
    FAST u_int * pUint  = &sc->interrupt;
    FAST int ix;
    u_short bank;
    u_char mask;
    static char *pMessage [] =
	{
	"interrupt",
	"interrupt rcv",
	"interrupt tx",
	"interrupt txempty",
	"interrupt alloc",
	"interrupt overrun",
	"interrupt eph",
	"interrupt ercv",
	"lost carr",
	"alloc timeout",
	"alloc failed",
	};

    printf ("esmc: ioAddr=0x%x intLevel=0x%x\n", sc->ioAddr, sc->intLevel);
    for (ix = 0; ix < NELEMENTS(pMessage); ix++)
	{
	printf ("    %-30.30s  %4d\n", pMessage[ix], pUint[ix]);
	if (zap)
	    pUint[ix] = 0;
	}
    printf ("    %-30.30s  0x%2x\n", "flags", sc->flags);

    bank = sysInWord (ioaddr + ESMC_BANK_SELECT);
    ESMC_SWITCH_BANK (2);
    mask = sysInByte (ioaddr + ESMC_INT_MASK);
    sysOutByte (ioaddr + ESMC_INT_MASK, 0);		/* lock INT */

    ESMC_SWITCH_BANK (0);
    printf ("bank0-TCR:        0x%x\n", sysInWord (ioaddr + ESMC_TCR));
    printf ("bank0-EPH:        0x%x\n", sysInWord (ioaddr + ESMC_EPH));
    printf ("bank0-RCR:        0x%x\n", sysInWord (ioaddr + ESMC_RCR));
    printf ("bank0-COUNTER:    0x%x\n", sysInWord (ioaddr + ESMC_COUNTER));
    printf ("bank0-MIR:        0x%x\n", sysInWord (ioaddr + ESMC_MIR));
    printf ("bank0-MCR:        0x%x\n", sysInWord (ioaddr + ESMC_MCR));
    ESMC_SWITCH_BANK (1);
    printf ("bank1-CONFIG:     0x%x\n", sysInWord (ioaddr + ESMC_CONFIG));
    printf ("bank1-CONTROL:    0x%x\n", sysInWord (ioaddr + ESMC_CONTROL));
    ESMC_SWITCH_BANK (2);
    printf ("bank2-MMU:        0x%x\n", sysInWord (ioaddr + ESMC_MMU));
    printf ("bank2-PNR:        0x%x\n", sysInByte (ioaddr + ESMC_PNR));
    printf ("bank2-ARR:        0x%x\n", sysInByte (ioaddr + ESMC_ARR));
    printf ("bank2-FIFO:       0x%x\n", sysInWord (ioaddr + ESMC_FIFO));
    printf ("bank2-PTR:        0x%x\n", sysInWord (ioaddr + ESMC_PTR));
    printf ("bank2-INT_STAT:   0x%x\n", sysInByte (ioaddr + ESMC_INT_STAT));
    printf ("bank2-INT_MASK:   0x%x\n", mask);
    ESMC_SWITCH_BANK (3);
    printf ("bank3-MGMT:       0x%x\n", sysInWord (ioaddr + ESMC_MGMT));
    printf ("bank3-REVISION:   0x%x\n", sysInWord (ioaddr + ESMC_REVISION));
    printf ("bank3-ERCV:       0x%x\n", sysInWord (ioaddr + ESMC_ERCV));

    ESMC_SWITCH_BANK (bank);
    sysOutByte (ioaddr + ESMC_INT_MASK, mask);	/* unlock INT */
    }
#endif	/* ESMC_SHOW */
