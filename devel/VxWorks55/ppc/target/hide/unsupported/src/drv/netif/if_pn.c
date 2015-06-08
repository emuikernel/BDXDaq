/* if_pn.c - proNET-80 network interface driver */

/* Copyright 1984,1985,1986,1987,1988,1989 Wind River Systems, Inc. */
extern char copyright_wind_river[]; static char *copyright=copyright_wind_river;

static char sccsid[] = "@(#)if_pn.c	2.1 11/4/86	(c) 1986 Proteon, Inc.";

/*
modification history
--------------------
01k,07aug89,gae  changed iv68k.h to iv.h.
01j,05sep88,gae  got rid of non-functional code.
01i,03sep88,gae  fixed bug with >2 pronets in ring.
01h,20aug88,gae  fixed wire-center/bootrom hang problem by disabling
		 interrupts in pnreset().  ifdef'd out DMA stuff.
		 got rid of gratuitous taskDelay() in pnoutput.
		 called pnprobe() in pnattach().
01g,12jul88,gae  merged in Carl Reed's fixes.  Lint.  Cleanup.
		 fixed pn_softc definition thanks to Phil Wiborg.
01f,07jul88,jcf  lint.
01e,30may88,dnw  changed to v4 names.
01d,10may88,llk  include ioLib.h for READ constants.
		 changed vxTdelay to taskDelay.
01c,08feb88,rdc  fixed several races and bugs.
01b,17nov87,dnw  changed pnattach to take int vec num instead of vec adrs.
01a,06nov87,rdc  written.
*/

/*
DESCRIPTION
ProNET device driver for proNET VMEbus interfaces (p1500 and p1580).

The only user callable routine is:

   pnattach (unit, addr, ivec, ilevel)

BUGS
Only one 'unit' allowed; proNet-80 (not proNet-10) is hard-coded.
*/

#include "vxWorks.h"
#include "iv.h"
#include "ioLib.h"
#include "ioctl.h"
#include "memLib.h"
#include "etherLib.h"

#include "param.h"
#include "mbuf.h"
#include "protosw.h"
#include "socket.h"
#include "errno.h"
#include "if.h"
#include "route.h"

#ifdef	INET
#include "in.h"
#include "in_systm.h"
#include "in_var.h"
#include "ip.h"
#endif	INET

#include "if_pn.h"
#include "if_ether.h"

IMPORT VOID ipintr ();

#define NPN	1		/* maximum number of proNET boards */
#define	IS_80	TRUE		/* TRUE=proNET-80, FALSE=proNET-10 */


/**************************************************************************/
/*    maximum transmission unit defined --                                */
/*        you can set PNMTU at anything from 576 on up, so long as you    */
/*        recongnize the weaknesses of 4.2BSD in assumuing too much about */
/*        the MTU of other TCP/IP implementations. Thus 576 is the most   */
/*        "compatible" value. 1536 is a popular "large" value, because    */
/*        it is a multiple of 512, which the trailer scheme liked.        */
/*        The absolute maximum size is 4096, which is enforced.           */
/**************************************************************************/

/* Adjust the next line for desired MTU (576 <= PNMTU).
 * Acceptable values are 576, 1024, 1536, 2048, 2560, 3072, 3584, 4096
 */
#define PNMTU	4096

/* Do not to touch the next group of lines.
 * They check the value and set other constants based on it.
 */
#if	PNMTU>4096
#undef	PNMTU
#define PNMTU	4096
#endif	PNMTU>4096

#define PNMRU		(PNMTU + 16)
#define PNBUFSIZE	(PNMRU + sizeof(struct pn_header))
#define PNCMDSIZE	4096

/* end of don't touch */


/**************************************************************************/
/*   debugging and tracing stuff                                          */
/**************************************************************************/
int pn_tracehdr  = FALSE;	/* TRUE => trace headers (slowly!!) */
int pn_logerrors = FALSE;	/* TRUE => log all I/O errors */

#define pntracehdr  if (pn_tracehdr) pnprt_hdr
#define	pnprintf    if (pn_logerrors) logMsg

BOOL pn_copy = FALSE;
/* XXX most boards won't allow long word transfers on 'just' even boundaries */
#define	pncopy(x,y,l)	(pn_copy ? bcopy(x,y,l) : bcopyBytes(x,y,l))

LOCAL u_short pn_modes[] =		/* three operating modes */
    {
    PN_RST,				/* digital loopback */
    PN_MEN,				/* analog loopback */
    PN_JNR|PN_MEN			/* network mode */
    };

/**************************************************************************/
/* Software status of each interface.                                     */
/*                                                                        */
/* Each interface is referenced by a network interface structure,         */
/* vs_if, which the routing code uses to locate the interface.            */
/* Structure contains the output queue for the interface, its address, ...*/
/**************************************************************************/
struct pn_softc
    {
    struct	arpcom	vs_ac;	/* common Ethernet structures */

#define	vs_if		vs_ac.ac_if	/* network-visible interface */
#define	vs_enaddr	vs_ac.ac_enaddr	/* hardware ethernet address */

    short	vs_oactive;	/* is output active */
    short	vs_olen;	/* output length */
    u_short	vs_ostrt;	/* output start, for retransmission */
    short	vs_tries;	/* transmit current retry count */
    short	vs_init;	/* number of ring inits */
    short	vs_refused;	/* number of packets refused */
    short	vs_timeouts;	/* number of transmit timeouts */
    short	vs_otimeout;	/* number of output timeouts */
    short	vs_ibadf;	/* number of input bad formats */
    short	vs_parity;	/* number of parity errors */
    u_short	rcvhd;		/* beginning of the rcv blk, for RBUFDMAPTR */
    caddr_t vs_tbuf;		/* xmt buffer */

    int pnIntLevel;           /* VME interrupt level */
    struct pn_regs *pn_regs;
    } pn_softc[NPN];


/* forward declarations */

LOCAL int pnprobe ();
LOCAL VOID pnreset ();
LOCAL VOID pninit ();
LOCAL int pnidentify ();
LOCAL VOID pnstart ();
LOCAL VOID pnxint ();
LOCAL VOID pnwatchdog ();
LOCAL VOID pnrint ();
LOCAL VOID pnoutput ();
LOCAL VOID pnioctl ();
LOCAL VOID pnsetaddr ();
LOCAL VOID pnrcsrintr ();
LOCAL VOID pntcsrintr ();
LOCAL struct mbuf *emptyrbuf ();


/*******************************************************************************
*
* pnattach -
*
* Interface exists: make available by filling in network interface record.
* System will initialize the interface when it is ready to accept packets.
*
* RETURNS: OK or ERROR
*/

STATUS pnattach (unit, addr, ivec, ilevel)
    int unit; 
    FAST struct pn_regs *addr;
    int ivec;
    int ilevel;

    {
    FAST struct pn_softc *vs = &pn_softc[unit];

    if (pnprobe (addr) == 0)
	return (ERROR);

    vs->pnIntLevel = ilevel;
    vs->pn_regs    = addr;

    addr->pn_ctl.rdmaint     = (unsigned short) ((ivec+0) & 0xff);
    addr->pn_ctl.tdmaint_nxm = (unsigned short) ((ivec+1) & 0xff);
    addr->pn_ctl.rcsrint     = (unsigned short) ((ivec+2) & 0xff);
    addr->pn_ctl.tcsrint     = (unsigned short) ((ivec+3) & 0xff);
    addr->pn_ctl.dma_int     = (unsigned short) 0;

    (void) intConnect (INUM_TO_IVEC (ivec+2), pnrcsrintr, unit);
    (void) intConnect (INUM_TO_IVEC (ivec+3), pntcsrintr, unit);

    sysIntEnable (ilevel);

    vs->vs_if.if_unit     = unit;
    vs->vs_if.if_name     = "pn";
    vs->vs_if.if_mtu      = PNMTU;
    vs->vs_if.if_init     = pninit;
    vs->vs_if.if_ioctl    = pnioctl;
    vs->vs_if.if_output   = pnoutput;
    vs->vs_if.if_reset    = pnreset;
    vs->vs_if.if_timer    = 0;
    vs->vs_if.if_watchdog = pnwatchdog; 
    vs->vs_if.if_flags    = IFF_BROADCAST;

    if_attach (&vs->vs_if);

    return (OK);
    }
/*******************************************************************************
*
* pnprobe - probe for presence of hardware
*
* RETURNS: 0 if no device, otherwise non-zero
*/

LOCAL int pnprobe (reg)
    caddr_t reg;

    {
    int dummy;
    FAST struct pn_regs *addr = (struct pn_regs *)reg;
    int rcsrLen = sizeof (addr->pn_ctl.rcsr);
    int tcsrLen = sizeof (addr->pn_ctl.tcsr);

    if (vxMemProbe ((char *)&addr->pn_ctl.rcsr, READ, rcsrLen, (char *)&dummy)
	== ERROR ||
	vxMemProbe ((char *)&addr->pn_ctl.tcsr, READ, tcsrLen, (char *)&dummy)
	 == ERROR)
      {
      return (0);
      }

    /* reset interface, enable, and wait till dust settles */
    /* should try to stay in ring */
    if (addr->pn_ctl.rcsr & PN_JNR)
	addr->pn_ctl.rcsr = PN_RST | PN_JNR;
    else
	addr->pn_ctl.rcsr = PN_RST;

    addr->pn_ctl.tcsr = PN_RST;

    taskDelay (10);

    return (sizeof (struct pn_regs));
    }
/*******************************************************************************
*
* pnreset - reset of interface
*/

LOCAL VOID pnreset (unit)
    int unit;

    {
    FAST struct pn_softc *vs = &pn_softc[unit];
    FAST struct pn_regs *addr = vs->pn_regs; 

    pninit (unit);

    /* XXX need to?
    vs->vs_if.if_flags &= ~IFF_RUNNING;
    */

    /* disable interrupts */

    sysIntDisable (vs->pnIntLevel);

    /* set interface to known state (not interrupting) */

    /* should try to stay in ring */
    if (addr->pn_ctl.rcsr & PN_JNR)
	addr->pn_ctl.rcsr = PN_RST | PN_JNR;
    else
	addr->pn_ctl.rcsr = PN_RST;

    addr->pn_ctl.tcsr = PN_RST;

    taskDelay (10);
    }
/*******************************************************************************
*
* pninit -
*
* Initialization of interface; clear recorded pending
* operations, and start any pending writes.
*/

LOCAL VOID pninit (unit)
    int unit;

    {
    FAST int s;
    FAST struct pn_softc *vs = &pn_softc[unit];
    FAST struct pn_regs *addr = vs->pn_regs; 

    if (vs->vs_tbuf == (caddr_t) 0 &&
	(vs->vs_tbuf = (caddr_t) malloc ((unsigned) PNBUFSIZE)) == (caddr_t) 0)
	{
	logMsg ("pninit: out of memory!");
	return;
	}

    /* Now that the buffers and mapping are set,
     * figure out our address and complete out host address;
     * pnidentify () will do a self-test.
     */

    if (pnidentify (unit, 1) == 0)
	{
	vs->vs_if.if_flags &= ~IFF_UP;
	free((char *)vs->vs_tbuf);
	vs->vs_tbuf = (caddr_t) 0;
	return;
	}
    

    /* Reset the interface, and stay in the ring */
    addr->pn_ctl.tcsr = PN_RST;				/* reset xmt */
    addr->pn_ctl.rcsr = PN_RST | PN_JNR | PN_MEN;	/* reset rcv */

    taskDelay (60);

    vs->vs_init     = 0;				/* clear counters */
    vs->vs_refused  = 0;
    vs->vs_timeouts = 0;
    vs->vs_otimeout = 0;
    vs->vs_ibadf    = 0;
    vs->vs_parity   = 0;

    /* hang receive, start any pending writes by faking a transmit complete */

    s = splimp ();

    vs->rcvhd            = 0;			/* RCOUNTER starts at 0 */
    addr->pn_ctl.rcsr    = PN_IEN | PN_JNR | PN_MEN | PN_CEN;

    vs->vs_oactive       = 1;
    vs->vs_if.if_flags  |= IFF_UP | IFF_RUNNING;

    pnxint (unit);

    splx (s);
    }
/*******************************************************************************
*
* pnidentify -
*
* Do a moderately thorough self-test in all three modes.
* Mostly to keep defective nodes off the ring than to be especially thorough.
*
* RETURNS: host address, or 0 if error
*/

LOCAL int pnidentify (unit, flg)
    int unit;
    int flg;

    {
    FAST struct pn_header *v;
    FAST int ix;
    FAST int successes;
    FAST int failures;
    u_short shost = 0;
    u_short t_strt;    /* t_strt is start of TBUFAD, and refers to lwords */
    u_short *msg;
    u_short *msgStart;
    u_short pktsz;
    int test;
    BOOL ignorexmt            = TRUE;
    FAST struct pn_softc *vs  = &pn_softc[unit];
    FAST struct pn_regs *addr = vs->pn_regs; 

    t_strt = (TBUFSIZE - ((sizeof(struct pn_header) + 3) >> 2)) & 0x3ff;

    if ((msgStart = (u_short *) malloc (PNBUFSIZE)) == NULL)
	{
	logMsg ("pnidentify: out of memory!\n");
	return (0);
	}

    /* Build a multicast address to identify our address.
     * We need do this only once, since nobody else is about to use
     * the intermediate transmit buffer (vs->vs_tbuf) that
     * we snagged in pninit().
     */

    addr->pn_ctl.tbufad = t_strt;	/* reset tbufad to 0? */

    v = (struct pn_header *)(vs->vs_tbuf);
    v->vh_dhost   = PN_BROADCAST;	/* multicast destination address */
    v->vh_shost   = 0;			/* will be overwritten with ours */
    v->vh_version = RING_VERSION;
    v->vh_type    = RING_DIAGNOSTICS;
    v->vh_info    = 0;

    vs->vs_olen = (sizeof(struct pn_header) + 1) >> 1;

    /* For each of the modes (digital, analog, network), go through
     * a self-test that requires me to send PNIDENTSUCC good packets
     * in PNIDENTRETRY attempts. Use broadcast destination to find out
     * who I am, then use this as my address to check my address match
     * logic. Only data checked is the vh_type field.
     */

    ix = (addr->pn_ctl.rcsr & PN_JNR) == PN_JNR ? 2 : 0;

    for (; ix < 4; ix++)
	{
	successes = 0;	/* clear successes for this mode */
	failures  = 0;	/* and clear failures, too */

	/* take over device, and leave ring */
	addr->pn_ctl.tcsr = PN_RST;

	test = (ix < 2) ? ix : 2;
	addr->pn_ctl.rcsr = pn_modes[test];	/* test mode */

	/* retry loop */
	while (successes < PNIDENTSUCC && failures < PNIDENTRETRY)
	    {
	    /* cancel any previous unfinished operations */

	    addr->pn_ctl.rcsr = PN_RST|pn_modes[test];
	    addr->pn_ctl.tcsr = PN_RST;

	    taskDelay (1);	/* let's give it a little more time */

	    /* start a receive */

	    addr->pn_ctl.rcounter   = 0; /* RCNTR after rcv is pkt size */
	    addr->pn_ctl.rcsr       = pn_modes[test]|PN_CEN;	/* do it */

	    /* send our output packet:
	     *   set up TCSR, TBUFAD, and TBUFDA's
	     */
	    addr->pn_ctl.tbufad = t_strt;
	    msg = (u_short *)vs->vs_tbuf;

	    while ((addr->pn_ctl.tbufad & 0x7ff) != TBUFSIZE)
		{
		addr->pn_ctl.tbufda = *msg++;
		addr->pn_ctl.tbufda = *msg++;
		}

	    addr->pn_ctl.tbufad = t_strt;
	    addr->pn_ctl.tcsr   = PN_INI|PN_ORI;

	    taskDelay (1);	/* wait to receive message */

	    if ((addr->pn_ctl.rcsr & PN_RDY) == 0 ||
		(addr->pn_ctl.tcsr & PN_RDY) == 0)
		{
		failures++;
		continue;
		}

	    /* check csr status on this packet */

	    if ((addr->pn_ctl.rstatr & PNRERR) ||
		(addr->pn_ctl.tcsr   & PNTERR))
		{
		failures++;
		continue;
		}

	    if (ignorexmt && test == 0)
		{
		/* Ignore the first xmit message.
		 * There is a problem with Proteon's software address
		 * override so the address returned from the first
		 * broadcast message after powerup may be wrong.
		 */

		ignorexmt = FALSE;
		continue;
		}

	    /* copy out the rcv pkt from RBUFFER to rbuf */
	    msg = msgStart;

	    for (pktsz = 0;
		 pktsz < PNBUFSIZE && pktsz < (addr->pn_ctl.rcntr >> 1);
		 pktsz++)
		{
		*msg++ = addr->pn_rbuffer [pktsz];
		}

	    addr->pn_ctl.rstcntr = 0;	/* clear CNTR */
	    v = (struct pn_header *) msgStart;

	    /* proper message type */
	    if ((v->vh_type & 0xff) == RING_DIAGNOSTICS)
		{
		if (shost == 0)
		    {
		    shost = v->vh_shost & 0xff;
		    ((struct pn_header *)(vs->vs_tbuf))->vh_dhost = shost;
		    }
		  successes++;
		}
	    else
	      failures++;
	    v->vh_type = 0;	/* zap it out so we can check again */
	    }

	if (failures >= PNIDENTRETRY)
	    {
	    if (flg)
		{
		printf("pn%d: failed self-test after %d tries in %s mode\n",
			unit, PNIDENTRETRY, ix == 0 ? "digital loopback" :
			(ix == 1 ? "analog loopback" : "network"));

		printf("pn%d: rstatr = %#x, rcsr = %#x, tcsr = %#x\n",
			unit, 0x7 & addr->pn_ctl.rstatr, /*PN_SBITS,*/
			0xffff & addr->pn_ctl.rcsr/*, PN_RBITS*/,
			0xffff & addr->pn_ctl.tcsr/*, PN_TBITS*/);
		}

	    addr->pn_ctl.rcsr = PN_RST;	/* kill the sick board */
	    addr->pn_ctl.tcsr = PN_RST;

	    free ((char *) msgStart);
	    return (0);
	    }
	}

    free ((char *) msgStart);
    return (shost);
    }
/*******************************************************************************
*
* pnstart - start or restart output on interface
*
* If interface active, then a retransmit, just restuff registers and go.
* If interface not already active, get another datagram off the interface
* queue, copy to buffer and send it.
*/

LOCAL VOID pnstart (unit)
    int unit;

    {
    int s;
    FAST struct mbuf *m;
    FAST struct pn_softc *vs  = &pn_softc[unit];
    FAST struct pn_regs *addr = vs->pn_regs; 

    /* active? */
    if (vs->vs_oactive)
	{
	addr->pn_ctl.tbufad = vs->vs_ostrt;	/* retransmit */
	goto restart;
	}

    /* not active, try dequeueing a new message */

    s = splimp();
    IF_DEQUEUE(&vs->vs_if.if_snd, m);
    splx (s);

    /* any message? */
    if (m == NULL) 
	{
	vs->vs_oactive = 0;
	return;
	}

    /* prepare to send */

    vs->vs_olen = filltbuf (unit, vs->vs_tbuf, m);
    m_freem (m);

restart :

    /* make sure this packet will fit in the interface */

    if (vs->vs_olen > PNBUFSIZE)
	{
	printf ("pn%d: vs_olen: %d > PNBUFSIZE\n", unit, vs->vs_olen);
	panic ("pnstart: vs_olen botch");
	}

    vs->vs_if.if_timer = PNTIMEOUT;
    vs->vs_oactive = 1;

    /* supposed to setup for 82258 transmit dma here */

    if (addr->pn_ctl.tcsr & PN_NOK) 
	vs->vs_init++;				/* count ring inits */

    addr->pn_ctl.tcsr = PN_IEN | PN_INI | PN_ORI;
    }
/*******************************************************************************
*
* pnwatchdog -
*
* Transmit watchdog timer routine. This routine gets called when we lose
* a transmit interrupt. The best we can do is try to restart output.
*/

LOCAL VOID pnwatchdog (unit)
    int unit;

    {
    FAST int s;
    FAST struct pn_softc *vs = &pn_softc[unit];

    pnprintf("pn%d: lost a transmit interrupt.\n", unit);

    vs->vs_timeouts++;

    s = splimp ();
    pnstart (unit);
    splx (s);
    }
/*******************************************************************************
*
* pnxint - transmit interrupt
*
* Start new output if more data available.
*/

LOCAL VOID pnxint (unit)
    int unit;

    {
    FAST struct pn_softc *vs  = &pn_softc[unit];
    FAST struct pn_regs *addr = vs->pn_regs; 
    FAST int tcsr             = addr->pn_ctl.tcsr & 0xffff;

    vs->vs_if.if_timer = 0;

    addr->pn_ctl.tcsr = PN_RTI;		/* clear/reset the interrupt */

    if (vs->vs_oactive == 0) 
	{
	pnprintf("pn%d: stray interrupt tcsr = %#x\n", unit, tcsr/*,PN_TBITS*/);
	return;
	}

    /* retransmit on soft error.
     * TODO: sort retransmits to end of queue if possible
     */
    if (tcsr & (PN_TMO | PN_REF))
	{
	if (vs->vs_tries++ < PNRETRY) 
	    {
	    if (tcsr & PN_TMO)
		vs->vs_otimeout++;

	    if (tcsr & PN_REF)
		{
		vs->vs_if.if_collisions++;
		vs->vs_refused++;
		}

	    pnstart (unit);	/* restart this message */
	    return;
	    }
	}

    vs->vs_if.if_opackets++;
    vs->vs_oactive = 0;
    vs->vs_tries   = 0;

    if (tcsr & PNTERR)
	{
	vs->vs_if.if_oerrors++;
	pnprintf ("pn%d: error tcsr = %#x\n", unit, tcsr/*, PN_TBITS*/);
	}

    pnstart (unit);
    }
/*******************************************************************************
*
* pnrint - receive interrupt
*
* First, stash data on packet just received. Then
* swap receive buffers and start a new receive. Now! Then check error
* status of packet. Then undo trailers, examine the packet, and pass it
* on the the appropriate higher protocol.
*/

LOCAL VOID pnrint (unit, rcsr, rstatr, rcntr)
    int unit;
    FAST int rcsr;		/* this is the current input CSR */
    FAST int rstatr;		/* ditto here as status register */
    int rcntr;

    {
    FAST struct pn_header *pn;
    FAST struct mbuf *m;
    struct ifqueue *inq;
    int len;
    int s;
    FAST int off;
    int type;
    short resid;			/* this is the current residual */
    int index;
    FAST struct pn_softc *vs  = &pn_softc[unit];
    FAST struct pn_regs *addr = vs->pn_regs; 

    vs->vs_if.if_ipackets++;

    /* save device registers, as we are restarting
     * (note rcsr saved it initial interrupt time)
     */

    if (rcntr < vs->rcvhd)
	len = RBUFSIZE - vs->rcvhd + rcntr;
    else
        len = rcntr  - vs->rcvhd;	/* only 14 addr bits */
    len -= sizeof (struct pn_header);	/* but remove local net hdr len */


    /* get a pointer to packet in RBUFFER */
    index = vs->rcvhd;
    pn = (struct pn_header *)&(addr->pn_rbuffer[index >> 1]);

    pntracehdr ("rint", pn, 1);

    vs->rcvhd = (rcntr + 3) & 0x3ffc;   /* blk addr of next pkt */

    if (! (vs->vs_if.if_flags & IFF_UP))
      {
      pnprintf ("pn%d: not up!\n", unit);
      return;
      }

    /* Now that a new receive has been started, do the processing
     * on the old packet (bufno)
     */

    if (rstatr & PNRERR) 
	{
	pnprintf("pn%d: receive error, rcsr = %#x, rstatr = %#x\n",
                  unit, rcsr, rstatr);
	if (rstatr & RBDF)
	    vs->vs_ibadf++;
	goto dropit;
	}

    if (rcsr & PN_PER)
	{
	/* don't have to clear it because the recieve command
	 * above wrote 0 to the parity bit
	 */
	vs->vs_parity++;

	/* Only on 10 megabit proNET is PN_PER an end-to-end parity
	 * bit. On 80 megabit, it returns to the intended use of
	 * node-to-node parity. End-to-end parity errors on 80 megabit
	 * give PN_BDF.
	 */

	if (!IS_80)
	    goto dropit;
	}


    /* fundamental length check */

    if ((rstatr & ROVR) || (len > PNMRU) || (len <= 0))
	{
	pnprintf(
	"pn%d: len too long or short, len = %d, rcsr = %#x, rstatr = %#x\n",
	    unit, len, rcsr, /*PN_RBITS,*/ rstatr/*, PN_SBITS*/);
	goto dropit;
	}

    /* check the protocol header version */
    if (pn->vh_version != RING_VERSION)
	{
	pnprintf("pn%d: bad protocol header version %d\n",
		 unit, pn->vh_version & 0xff);
	goto dropit;
	}

    /* untested trailer handling -- taken straight from VAX code */

    off = 0;

#define	pndataaddr(pn, off, type)	((type)(((caddr_t)((pn)+1)+(off))))

    if (pn->vh_type >= RING_IPTrailer &&
	pn->vh_type < RING_IPTrailer+RING_IPNTrailer) 
	{
	off = (pn->vh_type - RING_IPTrailer) * 512;
	if (off > PNMTU) 
	{
	    pnprintf("pn%d: off > PNMTU, off = %d, rcsr = %#x\n",
		unit, off, rcsr/*, PN_RBITS*/);
	    goto dropit;
	}
	pn->vh_type = ntohs(*pndataaddr(pn, off, u_short *));

	resid = ntohs(*pndataaddr(pn, 2+off, u_short *));
						    /* 2+off -> better code */

	if (off + resid > len) 
	    {
	    pnprintf ("pn%d: trailer packet too short\n", unit);
	    pnprintf ("pn%d: len=%d, off=%d, resid=%d, rcsr=%#x, rstatr=%#x\n",
			unit, len, off, resid,
			rcsr/*, PN_RBITS*/, rstatr/*, PN_SBITS*/);
	    goto dropit;
	    }
	len = off + resid;
	}

    if (len == 0) 
	{
	pnprintf("pn%d: len is zero, rcsr = %#x, rstatr = %#x\n",
                  unit, rcsr/*, PN_RBITS*/, rstatr/*, PN_SBITS*/);
	goto dropit;
	}


    /* save packet type for later demux */
    type = pn->vh_type & 0xff;

    /* Copy buffer into mbuf chain, without net header.
     * Could wait until type is o.k., but almost certainly is so why wait
     * Note that this part of the code runs SLOW, which is why the
     * multiple receive buffers are used. It still must be reckoned
     * that the keyboard is still locked out...
     */
    m = emptyrbuf (unit, addr, index, len, off, sizeof(struct pn_header), 
		  &vs->vs_if);

    if (m == 0)			/* success? */
	goto dropit;

    /* remove trailer header length header */
    if (off)
	{
    	struct ifnet *ifp;

    	ifp = *(mtod (m, struct ifnet **));
    	m->m_off += 2 * sizeof (u_short);
    	m->m_len -= 2 * sizeof (u_short);
    	*(mtod (m, struct ifnet **)) = ifp;
	}

    /* demultiplex on packet type */

    switch (type)
	{
#ifdef INET
	case RING_IP:
	    inq = &ipintrq;

	    s = splimp ();
	    if (IF_QFULL(inq)) 
		{
		IF_DROP(inq);
		m_freem (m);
		}
	    else
		IF_ENQUEUE(inq, m);
	    splx (s);

	    netJobAdd (ipintr, 0);
	    break;
#endif

	default:
	    pnprintf ("pn%d: unknown pkt type %#x\n", unit, type);
	    m_freem (m);
	}

    return;	/* next packet receive is already started */

dropit :	/* drop packet on floor -- count them!! */

    pnprintf ("pn%d: dropped it!\n", unit);
    vs->vs_if.if_ierrors++;
    }
/*******************************************************************************
*
* pnoutput -
*
* RETURNS: 0 if OK, otherwise UNIX errno
*/

LOCAL VOID pnoutput (ifp, m0, dst)
    struct ifnet *ifp;
    struct mbuf *m0;
    struct sockaddr *dst;

    {
    FAST struct pn_header *pn;
    FAST int s;
    int type;
    int dest;
    int error;
    u_short info              = 0;
    FAST struct mbuf *m       = m0;
    FAST int unit             = ifp->if_unit;
    FAST struct pn_softc *vs  = &pn_softc[unit];
    FAST struct pn_regs *addr = vs->pn_regs; 

    switch (dst->sa_family) 
	{
#ifdef INET
	case AF_INET:
	    dest = ((struct sockaddr_in *)dst)->sin_addr.s_addr;
	    /*
	     * Take the local net address part of the address,
	     * and use it as node number. This is not exactly
	     * kosher under some circumstances, but I'll let it fly.
	     */

	    if ((dest = in_lnaof(*((struct in_addr *)&dest))) >= 0x100) 
		{
		pnprintf("pn%d: can't do af%d\n", unit, dst->sa_family);
		error = EPERM;
		goto bad;
		}

	    type = RING_IP;		/* NO TRAILERS OUTBOUND */
	    break;

#endif
	default:
	    pnprintf("pn%d: can't handle af%d\n", unit, dst->sa_family);
	    error = EAFNOSUPPORT;
	    goto bad;
	}

    /* add local net header.  If no space in first mbuf, allocate another */

    if (m->m_off > MMAXOFF || MMINOFF + sizeof (struct pn_header) > m->m_off) 
	{
	m = m_get(M_DONTWAIT, MT_HEADER);
	if (m == (struct mbuf *)0) 
	    {
	    error = ENOBUFS;
	    goto bad;
	    }
	m->m_next = m0;
	m->m_off = MMINOFF;
	m->m_len = sizeof (struct pn_header);
	}
    else 
	{
	m->m_off -= sizeof (struct pn_header);
	m->m_len += sizeof (struct pn_header);
	}

    pn = mtod (m, struct pn_header *);

    pn->vh_shost = in_lnaof ( ((struct arpcom *)ifp)->ac_ipaddr); 
    if (pn->vh_shost == 0)
	{
	panic ("pnoutput: interface not found in in_ifaddr\n");
	error = EPERM;
	goto bad;
	}

    /* map the destination address if it's a broadcast */

    if ((pn->vh_dhost = dest) == INADDR_ANY)
	pn->vh_dhost = PN_BROADCAST;

    pn->vh_version = RING_VERSION;
    pn->vh_type    = type;
    pn->vh_info    = htons (info); /* this gets changed if testing packets */

    /* queue packet, and if interface not active, send */

    s = splimp ();

    if (IF_QFULL(&ifp->if_snd)) 
	{
	IF_DROP(&ifp->if_snd);
	error = ENOBUFS;
	goto qfull;
	}

    IF_ENQUEUE(&ifp->if_snd, m);

    if (vs->vs_oactive == 0)
	pnstart (unit);

    splx (s);

    return (0);

qfull :
    m0 = m;
    splx (s);

bad :
    m_freem (m0);

    return (error);
    }
/*******************************************************************************
*
* pnioctl -
*/

LOCAL VOID pnioctl (ifp, cmd, data)
    FAST struct ifnet *ifp;
    int cmd;
    caddr_t data;
    {
    FAST struct sockaddr *sin;
    FAST struct ifaddr *ifa = (struct ifaddr *)data;
    int error               = 0;
    FAST int s              = splimp();

    switch (cmd) 
	{
	case SIOCSIFADDR:
	    sin = (struct sockaddr *)data;
	    if (sin->sa_family != AF_INET)
		{
		error = EINVAL;
		break;
		}

	    pnsetaddr(ifp, (struct sockaddr_in *)sin);

	    /* store the internet address in arpcom */
	    ((struct arpcom *)ifp)->ac_ipaddr = IA_SIN(ifa)->sin_addr;

	    if (!(ifp->if_flags & IFF_RUNNING))
		{
		pninit (ifp->if_unit);
		if ((ifp->if_flags & IFF_UP) == 0)
		    error = ENETDOWN;	/* self test failed, give error */
		}
	    break;

	case SIOCSIFFLAGS:
	    /* not implemented (set interface up/down) */
	default:
	    error = EINVAL;
	    break;
	}

    splx (s);

    return (error);
    }
/*******************************************************************************
*
* pnsetaddr - set pronet address
*
* set up the address for this interface.  Uses the network number
* from the passed address and an invalid host number.
* pnidentify figures out and inserts real host address later.
*/

LOCAL VOID pnsetaddr (ifp, sin)
    FAST struct ifnet *ifp;
    FAST struct sockaddr_in *sin;

    {
    FAST struct pn_softc *vs  = &pn_softc[ifp->if_unit];
    FAST struct pn_regs *addr = vs->pn_regs; 
    int lna = in_lnaof (((struct sockaddr_in *)sin)->sin_addr);

    /* set the board's host number, overriding the hardware address */

    if (IS_80)
	addr->pn_ctl.rbufdmaptr = 0xc000 | lna;
    }
/*******************************************************************************
*
* pnrcsrintr - rcv csr intr handler
*/

LOCAL VOID pnrcsrintr (unit)
    int unit;

    {
    int csr;
    int rstatr;
    int rcntr;
    FAST struct pn_regs *addr = pn_softc[unit].pn_regs;

    if ((addr->pn_ctl.rcsr & (PN_IRQ|PN_RDY)) != (PN_IRQ|PN_RDY))
	logMsg ("proNET: rcv csr vector number clash!\n");
    else
	{
	csr    = addr->pn_ctl.rcsr & 0xffff;
	rstatr = addr->pn_ctl.rstatr & 0x7;
	rcntr  = addr->pn_ctl.rcntr & 0x3fff; 
	netJobAdd (pnrint, unit, csr, rstatr, rcntr);
	}

    addr->pn_ctl.rcsr =  PN_IEN | PN_MEN | PN_JNR | PN_RTI | PN_CEN;
    addr->pn_ctl.rstcntr = 0;		/* get ready for next pkt */
    }
/*******************************************************************************
*
* pntcsrintr - xmt csr intr handler
*/

LOCAL VOID pntcsrintr (unit)
    FAST int unit;

    {
    FAST struct pn_softc *vs  = &pn_softc[unit];
    FAST struct pn_regs *addr = vs->pn_regs;
    FAST int tcsr             = addr->pn_ctl.tcsr & 0xffff;
    FAST int rfsreg           = addr->pn_ctl.rfsreg & 0xffff;

    sysBusIntAck (vs->pnIntLevel);

    if ((tcsr & (PN_IRQ|PN_RDY)) != (PN_IRQ|PN_RDY))
	{
	pnprintf("proNET: xmt csr vector number clash!\n");
	logMsg ("tcsr = %#x\n", addr->pn_ctl.tcsr );

	if ((addr->pn_ctl.tcsr & (PN_IRQ)) != (PN_IRQ))
	    logMsg ("PN_IRQ not on\n");

	if ((addr->pn_ctl.tcsr & (PN_RDY)) != (PN_RDY))
	    logMsg ("PN_RDY not on\n");

	addr->pn_ctl.tcsr = PN_RTI;		/* clear/reset the interrupt */
	}
    else
	{
	/* should call pnxint() here, but duplicated here for speed */

	vs->vs_if.if_timer = 0;

	addr->pn_ctl.tcsr = PN_RTI;		/* clear/reset the interrupt */

	if (vs->vs_oactive == 0) 
	    {
	    pnprintf("pn%d: stray interrupt tcsr = %#x\n",
		     unit, tcsr/*, PN_TBITS*/);
	    return;
	    }

	/* retransmit on soft error
	 * TODO: sort retransmits to end of queue if possible
	 */

	if (tcsr & PNTERR)
	    {
	    if (vs->vs_tries++ < PNRETRY) 
		{
		/* re-order refused bits and cancel the "any":
		 *  what's wrong here?
		 *
		 * rfsreg = ((rfsreg >> 9) & 0x7f) | ((rfsreg & 0xff) << 7);
		 */

		if (tcsr & PN_TMO)
		    {
		    vs->vs_otimeout++;
		    pnprintf ("pntcsrintr: RESTART (TIMEOUT) %#x\n", rfsreg);
		    }
		if (tcsr & PN_REF)
		    {
		    vs->vs_if.if_collisions++;
		    vs->vs_refused++;
		    pnprintf ("pntcsrintr: RESTART (refused) %#x\n", rfsreg);
		    }
		if (tcsr & PN_BDF)
		    {
		    pnprintf ("pntcsrintr: RESTART (bad form) %#x\n", rfsreg);
		    }

		/* doesn't need to be at task level, 'cuz we're restarting */
		pnstart (unit);
		return;
		}
	    }

	vs->vs_if.if_opackets++;
	vs->vs_oactive = 0;
	vs->vs_tries   = 0;

	if (tcsr & PNTERR)
	    {
	    vs->vs_if.if_oerrors++;
	    pnprintf("pn%d: error tcsr = %#x\n", unit, tcsr/*, PN_TBITS*/);
	    }

	/* let's only call pnstart if there are messages in the output queue */

	if (vs->vs_if.if_snd.ifq_len > 0)  
	    netJobAdd (pnstart, unit);
	}
    }
/*******************************************************************************
*
* filltbuf -
*
* takes a chain of mbufs and copies the chain into a buffer.
* returns the number of bytes put into the buffer.  if board is
* not swabbing it also swabs the bytes. does not free mbuf chain.
*
* RETURNS: number of bytes filled
*/

LOCAL int filltbuf (unit, buf, mchain)
    int unit;
    caddr_t buf;
    struct mbuf *mchain;

    {
    FAST u_long *tbufda;
    FAST struct mbuf *m;
    BOOL odd             = FALSE;
    int len              = 0;
    struct pn_softc *vs  = &pn_softc[unit];
    struct pn_regs *addr = vs->pn_regs; 

    /* sum mbuf chain, and check for odd length mbuf's */

    for (m = mchain; m != NULL; m = m->m_next)
	{
	if (m->m_len & 0x1 || m->m_off & 0x1)
	    odd = TRUE;
	len += m->m_len;
	}

    tbufda = (u_long*)&addr->pn_ctl.tbufda;
    vs->vs_ostrt = (TBUFSIZE - ((len + 3) >> 2)) & 0x3ff;
    addr->pn_ctl.tbufad = vs->vs_ostrt;

    m = mchain;

    if (odd)
	{
	caddr_t bpend;
	FAST caddr_t bp = buf;

	/* double buffering! however this code probably never runs */

	for (; m != NULL; m = m->m_next)
	    {
	    pncopy (mtod (m, caddr_t), bp, m->m_len);
	    bp += m->m_len;
	    }
	bpend = bp;
	for (bp = buf; bp < bpend; bp += 4)
	    *tbufda = *(u_long *)bp;
	}
    else
	{
	int flag = 0;

	for (; m != NULL; m = m->m_next)
	    flag = movep (mtod (m, u_long *), tbufda, m->m_len, flag);
	}

    addr->pn_ctl.tbufad = vs->vs_ostrt;

    return (len);
    }
/*******************************************************************************
*
* emptyrbuf - empty a buffer into a chain of mbufs
*
* This routine is admitted to be hard to understand.  It is doing three
* things at once: copying the buffer, moving IP trailers to the front of
* the packet and skipping the ring header.
*
* NOTE:
* IP trailers part is not tested.
*
* RETURNS: the start of the chain, or 0 if error
*
* ARGSUSED
*/

LOCAL struct mbuf *emptyrbuf (unit, addr, pktad, totlen, off0, skip, ifp)
    int unit;			/* unit number */
    struct pn_regs *addr;	/* device address */
    int pktad;			/* index of pkt in RBUFFER */
    FAST int totlen;		/* bytes to copy */
    int off0;			/* trailer offset */
    int skip;			/* header length */
    struct ifnet *ifp;

    {
    FAST int len;
    FAST struct mbuf *m;
    struct mbuf *top = (struct mbuf *) 0;
    FAST struct mbuf **mp = &top;
    int off          = off0;		/* assuming off = 0, ie. no trailers */
    int index        = ((pktad + skip) >> 1) & 0x1fff;	/* word array index */
    FAST caddr_t bp  = (caddr_t)&addr->pn_rbuffer[index];


    pntracehdr ("input", (struct pn_header *)&(addr->pn_rbuffer[pktad>>1]), 1);

    while (totlen > 0)
	{
	MGET(m,M_DONTWAIT,MT_DATA);
	if (m == (struct mbuf *)0)
	    {
	    if (top != (struct mbuf *)0)
	      m_freem (top);

	    return ((struct mbuf *)0);
	    }

	if (off)
	    {
	    len = totlen - off;
	    index = ((pktad + off + skip) >> 1) & 0x1fff;
	    bp = (caddr_t)&(addr->pn_rbuffer[index]);
	    }
	else
	    len = totlen;

	m->m_off = MMINOFF;
	if (ifp)
	    {
	    /* Leave room for ifp */
	    m->m_len = MIN(MLEN - sizeof(ifp), len);
	    m->m_off += sizeof(ifp);
	    }
	else 
	    m->m_len = MIN(MLEN, len);

	m->m_next = (struct mbuf *)0; 

	if ((index << 1) + m->m_len >= RBUFSIZE)
	    {
	    caddr_t tp = mtod (m, caddr_t);
	    int cut = RBUFSIZE - (index << 1);

	    pncopy(bp,tp,cut);
	    pncopy((caddr_t)addr->pn_rbuffer,tp+cut,m->m_len-cut);
	    index = (m->m_len - cut) >> 1;
	    bp = (caddr_t)&(addr->pn_rbuffer[index]);
	    }
	else
	    {
	    pncopy(bp, mtod (m,caddr_t), m->m_len);
	    index += (m->m_len >> 1);
	    bp += m->m_len;
	    }

	*mp = m;
	mp = &m->m_next;

	if (off)
	    {
	    off += m->m_len;
	    if (off == totlen)
		{
		index = ((pktad + skip) >> 1) & 0x1fff;
		bp = (caddr_t)&(addr->pn_rbuffer[index]);
		off = 0;
		totlen = off0;
		}
	    }
	else
	    totlen -= m->m_len;

	if (ifp) 
	    {
	    /* prepend interface pointer to first mbuf */

	    m->m_len += sizeof(ifp);
	    m->m_off -= sizeof(ifp);
	    *(mtod (m, struct ifnet **)) = ifp;
	    ifp = (struct ifnet *)0;
	    }
      }

    return (top);
    }

/* debugging aid printout routines */

/*******************************************************************************
*
* pnprt_hdr - print the local net header in "v" with title "s"
*/

LOCAL VOID pnprt_hdr (s, v, nl)
    char *s;
    FAST struct pn_header *v;
    int nl;

    {
    printf ("%10s @ %#10x: d=%#x s=%#x v=%#x t=%#x i=%#x",
	s, v,
	0xff & (int)(v->vh_dhost), 0xff & (int)(v->vh_shost),
	0xff & (int)(v->vh_version), 0xff & (int)(v->vh_type),
	0xffff & (int)(v->vh_info));

    if (nl)
	printf("\n");
    }
/*******************************************************************************
*
* pn_errors - print error statistics and zero
*/

VOID pn_errors (unit)

    {
    FAST struct pn_softc *vs = &pn_softc[unit];

    printf ("oerrors = %d", vs->vs_if.if_oerrors);
    vs->vs_if.if_oerrors = 0;

    printf ("otimeout = %d", vs->vs_otimeout);
    vs->vs_otimeout = 0;

    printf ("collisions = %d", vs->vs_if.if_collisions);
    vs->vs_if.if_collisions = 0;

    printf ("refused = %d", vs->vs_refused);
    vs->vs_refused = 0;
    }

BOOL xmovep (from, to, len)
    short *from;
    short *to;
    int len;
    {
    while (len -= 2)
	*to = *from++;
    }
