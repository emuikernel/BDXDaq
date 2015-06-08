#include "sl.h"
#if NSL > 0
/*
 * Copyright (c) 1989, 1990, 1991, 1992, 1993 Regents of the University
 * of California. All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	Van Jacobson (van@ee.lbl.gov), Dec 31, 1989:
 *	- Initial distribution.
 */
#ifndef lint
static char rcsid[] =
    "@(#) $Header: if_sl.c,v 1.24 93/09/14 23:40:05 leres Exp $ (LBL)";
#endif

/*
 * Serial Line interface
 *
 * Originally written by
 *	Rick Adams
 *	Center for Seismic Studies
 *	1300 N 17th Street, Suite 1450
 *	Arlington, Virginia 22209
 *	(703)276-7900
 *	rick@seismo.css.gov
 *	seismo!rick
 *
 * Pounded on heavily by Chris Torek (chris@mimsy.umd.edu, umcp-cs!chris).
 * N.B.: this belongs in netinet, not net, the way it stands now.
 * Should have a link-layer type designation, but wouldn't be
 * backwards-compatible.
 *
 * Converted to 4.3BSD Beta by Chris Torek.
 * Other changes made at Berkeley, based in part on code by Kirk Smith.
 *
 * Hacked almost beyond recognition by Van Jacobson (van@ee.lbl.gov).
 * Added priority queuing for "interactive" traffic; hooks for TCP
 * header compression; ICMP filtering (at 2400 baud, some cretin
 * pinging you can use up all your bandwidth); conditionals for Sun
 * OS 3.x in addition to 4.x BSD.  Made low clist behavior more robust
 * and slightly less likely to hang serial line.  Sped up a bunch of
 * things.
 *
 * The Sun OS4.x STREAMS version of this driver was written by
 * Steve McCanne, Craig Leres and Van Jacobson.  The evolutionary
 * ancestor of this driver was an SGI STREAMS driver written by Sam Leffler
 * (which was derived, in turn, from the 4.x BSD cslip driver described
 * in the previous paragraph).  However this driver bears little or
 * no resemblance to Leffler's.
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/mbuf.h>
#include <sys/protosw.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <sys/debug.h>
#include <sys/signal.h>
#include <sys/user.h>
#include <sys/file.h>
#include <sys/stream.h>
#include <sys/stropts.h>
#include <sys/termio.h>
#include <net/if.h>
#include <net/route.h>
#include <net/netisr.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/in_var.h>
#include <netinet/ip.h>

#ifdef sun
#include <sys/sockio.h>
#endif

#include <net/slcompress.h>
#include <net/if_slvar.h>
#include <net/slip.h>

#include "bpfilter.h"
#if NBPFILTER > 0
#include <net/bpf.h>
#endif

/*
 * The following are patchable defaults for the three options
 * in the interface flags word.  If desired, they should be set
 * by config file options SL_DOCOMPRESS, SL_ALLOWCOMPRESS and
 * SL_NOICMP.
 *
 *   sl_docompress	If = 1, compression for a line will default to "on"
 *
 *   sl_allowcompres	If = 1, compression for a line will default to "off"
 *			but will be turned on if a compressed packet is
 *			received.
 *
 *   sl_noicmp		If = 1, outbound ICMP packets will be discarded.
 *			XXX - shouldn't have to set this but some cretin
 *			pinging us can drive our throughput to zero (not
 *			to mention the raft of quenches we'll get if we're
 *			unlucky enough to have to traverse the milnet.
 */
#ifndef SL_DOCOMPRESS
#define SL_DOCOMPRESS 0
#endif
#ifndef SL_ALLOWCOMPRESS
#define SL_ALLOWCOMPRESS 0
#endif
#ifndef SL_NOICMP
#define SL_NOICMP 0
#endif
int sl_docompress = SL_DOCOMPRESS;
int sl_allowcompress = SL_ALLOWCOMPRESS;
int sl_noicmp = SL_NOICMP;

/*
 * SLMTU is a hard limit on output packet size.  To insure good
 * interactive response, SLMTU wants to be the smallest size that
 * amortizes the header cost.  (Remember that even with
 * type-of-service queuing, we have to wait for any in-progress
 * packet to finish.  I.e., we wait, on the average, 1/2 * mtu /
 * cps, where cps is the line speed in characters per second.
 * E.g., 533ms wait for a 1024 byte MTU on a 9600 baud line.  The
 * average compressed header size is 6-8 bytes so any MTU > 90
 * bytes will give us 90% of the line bandwidth.  A 100ms wait is
 * tolerable (500ms is not), so want an MTU around 296.  (Since TCP
 * will send 256 byte segments (to allow for 40 byte headers), the
 * typical packet size on the wire will be around 260 bytes).  In
 * 4.3tahoe+ systems, we can set an MTU in a route so we do that &
 * leave the interface MTU relatively high (so we don't IP fragment
 * when acting as a gateway to someone using a stupid MTU).
 */
#ifndef SLMTU
#define	SLMTU		(512 + 40)
#endif

/*
 * SLBUFOFFSET is the amount of space we reserve in front of the slip
 * packet. We need room for a pointer to the ifnet struct. If the
 * incoming packet is compressed, we need room to expand the header
 * into a max length tcp/ip header (120 bytes). Finally, if we support
 * BPF, we need an additional 16 bytes for the largest compressed
 * header plus a direction byte.
 */
#if NBPFILTER > 0
#define	SLBUFOFFSET	(sizeof(struct ifnet *) + 120 + SLIP_HDRLEN)
#else
#define	SLBUFOFFSET	(sizeof(struct ifnet *) + 120)
#endif

/*
 * SLLINKHDRSPACE is the space we leave for forwarded packet link level
 * headers. If we forward a slip packet, the output routine will
 * probably have to stick an ethernet (or other link level) header on
 * it so we'll save extra mbuf allocates and copies if we leave room
 * for that header whenever we stick incoming packets into an mbuf. We
 * need at least 16 bytes for that header and, if we make it twenty,
 * the packet data will end up nicely aligned on machines that use 8
 * and 16 byte cache line sizes.
 */
#define	SLLINKHDRSPACE	20

/*
 * SLIP_HIWAT is the hi-water mark. It should be at least 4 Kbytes to
 * handle the default BSD window size and must be a multiple of SLMTU.
 */
#define SLIP_HIWAT	roundup(4096, SLMTU)

/*
 * Setup to receive a new packet.
 * Assumes "m" points to sc->sc_mbuf.
 */
#define SLNEWPACKET { \
	m->m_off = sc->sc_moff + SLBUFOFFSET; \
	sc->sc_mp = mtod(m, u_char *); \
	sc->sc_ep = sc->sc_mp + (MCLBYTES - SLBUFOFFSET); \
	sc->sc_escape = 0; \
}

/* Prototype-foo */
#ifndef __P()
#define __P(protos) ()
#endif

static	struct module_info stm_info = {
	0x6963,
	"slip",				/* module name */
	0,				/* minimum packet size */
	INFPSZ,				/* infinite maximum packet size	*/
	SLIP_HIWAT,			/* hi-water mark */
	0				/* lo-water mark */
};

void	sl_rput __P((queue_t *, mblk_t *));
void	sl_wput __P((queue_t *, mblk_t *));
void	sl_wsrv __P((queue_t *));
void	sl_close __P((queue_t *));
int	sl_open __P((queue_t *, dev_t, int, int));
int	slioctl __P((struct ifnet *, int, caddr_t));
int	sloutput __P((struct ifnet *, struct mbuf *, struct sockaddr *));
void	slinput __P((struct sl_softc *, u_char *, int));

static struct qinit st_rinit = {
	(int (*)())sl_rput,
	NULL,
	sl_open,
	(int (*)())sl_close,
	NULL,
	&stm_info,
	NULL
};
static struct qinit st_winit = {
	(int (*)())sl_wput,
	(int (*)())sl_wsrv,
	NULL,
	NULL,
	NULL,
	&stm_info,
	NULL
};
static char *st_modlist[] = {
	NULL
};
struct streamtab if_slinfo = {
	&st_rinit,
	&st_winit,
	NULL,
	NULL,
	st_modlist
};

/*
 * The following disgusting hack gets around the problem that IP TOS
 * can't be set in BSD/Sun/SGI OS yet.  We want to put "interactive"
 * traffic on a high priority queue.  To decide if traffic is
 * interactive, we check that a) it is TCP and b) one of it's ports
 * if telnet, login, klogin, eklogin or ftp control.
 *
 * (Probably also want X/NeWS-style traffic.)
 */
static	u_short interactive_ports[16] = {
	0,	513,	0,	0,	0,	21,	0,	23,
	0,	2105,	0,	0,	0,	0,	0,	543
};
#define INTERACTIVE(p) (interactive_ports[(p) & 0xf] == (p))

/*
 * Everything there is to know about if_sl interfaces
 */
struct sl_softc sl_softc[NSL];

/*
 * Framing stuff
 */
#define FRAME_END		0xc0	/* Frame End */
#define FRAME_ESCAPE		0xdb	/* Frame Esc */
#define TRANS_FRAME_END		0xdc	/* escaped frame end */
#define TRANS_FRAME_ESCAPE	0xdd	/* escaped frame esc */

void
slattach(unit)
	int unit;
{
	register struct sl_softc *sc;
	register struct ifnet *ifp;

	if (unit >= NSL)
		panic("slattach: too many units");

	sc = &sl_softc[unit];
	sc->sc_fastq.ifq_maxlen = 32;

	ifp = &sc->sc_if;
	ifp->if_name = "sl";
	ifp->if_unit = unit;
	ifp->if_mtu = SLMTU;
	ifp->if_flags = IFF_POINTOPOINT;
	ifp->if_ioctl = slioctl;
	ifp->if_output = sloutput;
	ifp->if_snd.ifq_maxlen = IFQ_MAXLEN;
	if_attach(ifp);
	printf("sl%d: attached\n", unit);
#if NBPFILTER > 0
	bpfattach(&sc->sc_bpf, &sc->sc_if, DLT_SLIP, SLIP_HDRLEN);
#endif
}

int
slinit(sc)
	register struct sl_softc *sc;
{
	register struct mbuf *m;

	/*
	 * Allocate a cluster to hold the input packet.
	 */
	if ((m = sc->sc_mbuf) == NULL) {
		MGET(m, M_DONTWAIT, MT_DATA);
		if (m != NULL) {
			MCLGET(m);
			if (m->m_len != MCLBYTES) {
				m_free(m);
				m = NULL;
			}
		}
		if (m == NULL) {
			printf("sl%d: can't allocate mbuf\n", sc - sl_softc);
			sc->sc_if.if_flags &= ~IFF_UP;
			return (0);
		}
		sc->sc_mbuf = m;
		sc->sc_moff = m->m_off;
	}
	SLNEWPACKET;
	/*
	 * The STREAMS data structures and routines are so abysmally bad
	 * there's no way we can afford to do all the data structure
	 * allocation/deallocation they require per output packet.  So,
	 * since we only send one packet down at a time, we allocate a
	 * buffer & msgblk as big as we could need, set its ref cnt to 1
	 * (so it won't be deallocated), then copy from mbufs into it, dup
	 * it, and send it down for each packet.
	 *
	 * The maximum possible size is twice the MTU (every character
	 * could be escaped) plus two framing bytes (one on each end).
	 * We need two of the damn things because there's no way to
	 * tell the cretinous streams code that we only want to queue
	 * one downstream buffer -- the first one we queue is removed
	 * from the count (and our service procedure is called) even
	 * though it hasn't been output yet.
	 */
	if ((sc->sc_outmb = allocb(2 * SLMTU + 2, BPRI_HI)) == NULL) {
		printf("sl%d: can't allocate out msg buffer\n", sc - sl_softc);
		sc->sc_if.if_flags &= ~IFF_UP;
		return (0);
	}
	if ((sc->sc_pendmb = allocb(2 * SLMTU + 2, BPRI_HI)) == NULL) {
		printf("sl%d: can't allocate pend msg buffer\n", sc - sl_softc);
		sc->sc_if.if_flags &= ~IFF_UP;
		freeb(sc->sc_outmb);
		return (0);
	}

	sc->sc_bytessent = 0;
	sc->sc_bytesrcvd = 0;

	if (sl_docompress)
		sc->sc_if.if_flags |= IFF_LINK0;
	else
		sc->sc_if.if_flags &=~ IFF_LINK0;
	if (sl_allowcompress)
		sc->sc_if.if_flags |= IFF_LINK1;
	else
		sc->sc_if.if_flags &=~ IFF_LINK1;
	if (sl_noicmp)
		sc->sc_if.if_flags |= IFF_LINK2;
	else
		sc->sc_if.if_flags &=~ IFF_LINK2;
	sl_compress_init(&sc->sc_comp);
	sc->sc_if.if_flags |= IFF_RUNNING;
	return (1);
}

/*
 * Process ioctl from the socket side.
 */
int
slioctl(ifp, cmd, data)
	register struct ifnet *ifp;
	register int cmd;
	register caddr_t data;
{
	register struct sl_softc *sc;
	register int error, s;

	error = 0;
	sc = &sl_softc[ifp->if_unit];
	s = splimp();
	switch (cmd) {

	case SIOCSIFADDR: {
		struct ifaddr *ifa = (struct ifaddr *)data;

		if (ifa->ifa_addr.sa_family != AF_INET)
			error = EAFNOSUPPORT;
		else if (sc->sc_rq == NULL)
			error = EINVAL;
		else
			sc->sc_if.if_flags |= IFF_UP;
		break;
	}

	case SIOCSIFDSTADDR: {
		struct ifaddr *ifa = (struct ifaddr *)data;

		if (ifa->ifa_addr.sa_family != AF_INET)
			error = EAFNOSUPPORT;
		break;
	}

	default:
		error = EINVAL;
		break;
	}
	splx(s);
	return (error);
}

/*
 * take an mbuf chain from above and see about sending it down the stream
 */
int
sloutput(ifp, m, dst)
	register struct ifnet *ifp;
	register struct mbuf *m;
	register struct sockaddr *dst;
{
	register struct sl_softc *sc;
	register struct ip *ip;
	register struct ifqueue *ifq;
	register int s;
#if NBPFILTER > 0
	u_char bpfbuf[SLMTU + SLIP_HDRLEN];
	register u_int len;
#endif

	if ((ifp->if_flags & IFF_UP) == 0) {
		m_freem(m);
		return (ENETDOWN);
	}
	if (dst->sa_family != AF_INET) {
		/*
		 * This `cannot happen' (see slioctl).
		 */
		printf("sl%d: af%d not supported\n", ifp->if_unit,
		    dst->sa_family);
		m_freem(m);
		return (EAFNOSUPPORT);
	}
	sc = &sl_softc[ifp->if_unit];

#if NBPFILTER > 0
	if (sc->sc_bpf) {
		/*
		 * We need to save the TCP/IP header before it's compressed.
		 * To avoid complicated code, we just copy the entire packet
		 * into a stack buffer (since this is a serial line, packets
		 * should be short and/or the copy should be negligible cost
		 * compared to the packet transmission time).
		 */
		register struct mbuf *m1;
		register u_char *cp;
		register u_int left, mlen;

		m1 = m;
		cp = bpfbuf + SLIP_HDRLEN;
		left = sizeof(bpfbuf) - SLIP_HDRLEN;
		len = 0;
		do {
			mlen = m1->m_len;
			if (mlen > left)
				mlen = left;
			bcopy(mtod(m1, caddr_t), (caddr_t)cp, mlen);
			cp += mlen;
			len += mlen;
			left -= mlen;
		} while ((m1 = m1->m_next) != NULL && left > 0);
	}
#endif
	ifq = &ifp->if_snd;
	/*
	 * n.b.-- the `40' below checks that we have a least a min length
	 * tcp/ip header contiguous in an mbuf.  We don't do `sizeof' on
	 * some struct because too many compilers add random padding.
	 */
	ip = mtod(m, struct ip *);
	if (ip->ip_p == IPPROTO_TCP && m->m_len >= 40) {
		register int p;

#ifndef SL_NOFASTQ
#ifdef SL_TOS
#ifndef IPTOS_LOWDELAY
#define IPTOS_LOWDELAY 0x10
#endif
		if (ip->ip_tos == IPTOS_LOWDELAY)
			ifq = &sc->sc_fastq;
		else
#endif
		/* Don't need to byte swap ip_off to compare against 0 */
		if (ip->ip_off == 0) {
			/* XXX do TOS queueing based on port number */
			p = ntohl(((int *)ip)[ip->ip_hl]);
			if (INTERACTIVE(p & 0xffff) || INTERACTIVE(p >> 16))
				ifq = &sc->sc_fastq;
		}
#endif
		if (sc->sc_if.if_flags & IFF_LINK0) {
			p = sl_compress_tcp(m, ip, &sc->sc_comp);
			*mtod(m, u_char *) |= p;
		}
	} else if (ip->ip_p == IPPROTO_ICMP &&
	    (sc->sc_if.if_flags & IFF_LINK2)) {
		m_freem(m);
		return (0);
	}
#if NBPFILTER > 0
	if (sc->sc_bpf) {
		/*
		 * Put the SLIP pseudo-"link header" in place.  The compressed
		 * header is now at the beginning of the mbuf.
		 */
		bpfbuf[SLX_DIR] = SLIPDIR_OUT;
		bcopy(mtod(m, caddr_t), (caddr_t)&bpfbuf[SLX_CHDR], CHDR_LEN);
		bpf_tap(sc->sc_bpf, bpfbuf, len + SLIP_HDRLEN);
	}
#endif
	s = splimp();
	if (IF_QFULL(ifq)) {
		IF_DROP(ifq);
		m_freem(m);
		splx(s);
		++sc->sc_if.if_oerrors;
		return (ENOBUFS);
	}
	IF_ENQUEUE(ifq, m);
	splx(s);
	if (sc->sc_outmb->b_datap->db_ref == 1)
		sl_wsrv(sc->sc_wq);
	return (0);
}

/*
 * XXX should probably be in net/if.c.
 */
static void
if_rtdelete(ifp)
	register struct ifnet *ifp;
{
	register struct ifaddr *ifa;
	struct rtentry route;

	for (ifa = ifp->if_addrlist; ifa; ifa = ifa->ifa_next) {
		bzero((caddr_t)&route, sizeof (route));
		route.rt_dst = ifa->ifa_dstaddr;
		route.rt_gateway = ifa->ifa_addr;
		route.rt_flags = RTF_HOST|RTF_UP;
		(void) rtrequest(SIOCDELRT, &route);
	}
}

/*
 * open SLIP STREAMS module
 */
/* ARGSUSED */
int
sl_open(rq, dev, flag, sflag)
	register queue_t *rq;
	register dev_t dev;
	register int flag;
	register int sflag;
{
	register struct sl_softc *sc;
	register queue_t *wq;
	register int nsl, s;

	wq = WR(rq);
	if (MODOPEN != sflag)
		return (OPENFAIL);
	if (rq->q_ptr != NULL)
		return (0);
	/* for now, only root can add links */
	if (!suser())
		return (OPENFAIL);
	s = splstr();
	for (nsl = NSL, sc = sl_softc; --nsl >= 0; ++sc)
		if (sc->sc_rq == NULL) {
			if (!slinit(sc)) {
				u.u_error = ENOBUFS;
				goto bad;
			}
			sc->sc_rq = rq;
			sc->sc_wq = wq;
			rq->q_ptr = (caddr_t)sc;
			wq->q_ptr = (caddr_t)sc;
			splx(s);
			return (0);
		}
	u.u_error = EBUSY;
bad:
	splx(s);
	return (OPENFAIL);
}

/*
 * Flush input or output.
 */
void
sl_flush(op, rq, wq)
	register u_char op;
	register queue_t *rq;
	register queue_t *wq;
{
	register struct sl_softc *sc;
	register int s;
	register struct mbuf *m, *m2;

	sc = (struct sl_softc *)rq->q_ptr;
	if (op & FLUSHW) {
		/* flush outbound packets */
		register struct ifqueue *q;
		register struct mbuf *m;

		s = splimp();
		q = &sc->sc_fastq;
		while (1) {
			IF_DEQUEUE(q, m);
			if (m == NULL)
				break;
			IF_DROP(q);
			m_freem(m);
		}
		q = &sc->sc_if.if_snd;
		while (1) {
			IF_DEQUEUE(q, m);
			if (m == NULL)
				break;
			IF_DROP(q);
			m_freem(m);
		}
		splx(s);
		s = splstr();
		flushq(wq, FLUSHALL);
		splx(s);
	}
	if (op & FLUSHR) {
		/* flush partially received packet */
		s = splstr();
		flushq(rq, FLUSHALL);
		m = sc->sc_mbuf;
		if ((m2 = m->m_next) != NULL) {
			/* Free second cluster and mbuf */
			(void) m_free(m2);
			m->m_next = NULL;
		}
		SLNEWPACKET;
		splx(s);
	}
}

void
sl_close(rq)
	register queue_t *rq;
{
	register struct sl_softc *sc;
	register int s;

	sc = (struct sl_softc *)rq->q_ptr;
	sl_flush(FLUSHRW, rq, sc->sc_wq);
	s = splimp();
	if_rtdelete(&sc->sc_if);
	if_down(&sc->sc_if);
	sc->sc_wq->q_ptr = NULL;
	rq->q_ptr = NULL;
	sc->sc_rq = NULL;
	sc->sc_wq = NULL;
	if (sc->sc_outmb)
		freeb(sc->sc_outmb);
	if (sc->sc_pendmb)
		freeb(sc->sc_pendmb);
	if (sc->sc_mbuf)
		m_freem(sc->sc_mbuf);
	sc->sc_mbuf = NULL;
	sc->sc_moff = 0;
	sc->sc_ep = NULL;
	sc->sc_mp = NULL;
	sc->sc_if.if_flags &=~ IFF_RUNNING;
	splx(s);
}

/*
 * handle streams packets from the stream head.  They should be only IOCTLs.
 */
void
sl_wput(wq, bp)
	register queue_t *wq;
	register mblk_t *bp;
{
	register struct sl_softc *sc;

	sc = (struct sl_softc*)wq->q_ptr;
	switch (bp->b_datap->db_type) {

	case M_DATA:			/* data does not come this way */
	case M_DELAY:			/* do not allow random controls */
		return;

	case M_FLUSH:
		sl_flush(*bp->b_rptr, sc->sc_rq, wq);
		break;

	case M_IOCTL:
		if (((struct iocblk *)bp->b_rptr)->ioc_cmd == SLIOGUNIT) {
			register struct iocblk *ioc;

			ioc = (struct iocblk *)bp->b_rptr;
			bp->b_datap->db_type = M_IOCACK;
			if (bp->b_cont)
				freemsg(bp->b_cont);
			bp->b_cont = allocb(sizeof(int), BPRI_HI);
			*(int *)bp->b_cont->b_wptr = sc->sc_if.if_unit;
			bp->b_cont->b_wptr += sizeof(int);
			ioc->ioc_count = sizeof(int);
			ioc->ioc_error = 0;
			qreply(wq, bp);
			return;
		}
		break;
	}
	putnext(wq, bp);
}

/*
 * Accept a new STREAMS message from the serial line.
 */
void
sl_rput(rq, bp)
	register queue_t *rq;
	register mblk_t *bp;
{
	register struct sl_softc *sc;

	sc = (struct sl_softc*)rq->q_ptr;
	switch (bp->b_datap->db_type) {

	case M_DATA:
		if ((sc->sc_if.if_flags & IFF_UP) == 0)
			freemsg(bp);
		else {
			register mblk_t *tp;
			register u_char *buf;

			buf = bp->b_rptr;
			do {
				slinput(sc, buf, bp->b_wptr - buf);
				tp = bp->b_cont;
				freeb(bp);
			} while ((bp = tp) != NULL);
		}
		return;

	case M_FLUSH:
		sl_flush(*bp->b_rptr, rq, sc->sc_wq);
		break;
	}
	putnext(rq, bp);
}

/*
 * Possibly swap mbuf's; add ifnet pointer.
 */
struct mbuf *
sl_btom(sc)
	register struct sl_softc *sc;
{
	register u_int len;
	register struct mbuf *m, *m2;

	MGET(m2, M_DONTWAIT, MT_DATA);
	if (m2 == NULL)
		return (NULL);
	/*
	 * If we have more than MLEN bytes, it's cheaper to
	 * queue the cluster we just filled & allocate a new one
	 * for the input buffer.  Otherwise, fill the mbuf we
	 * allocated above.  Note that code in the input routine
	 * guarantees that packet + ifp will fit in a cluster and
	 * initial setup left room for interface pointer.
	 */
	m = sc->sc_mbuf;
	len = m->m_len;
	if (len > MLEN - SLLINKHDRSPACE) {
		MCLGET(m2);
		if (m2->m_len != MCLBYTES) {
			m_free(m2);
			return (NULL);
		}
		sc->sc_mbuf = m2;
		sc->sc_moff = m2->m_off;
	} else {
		m2->m_len = len;
		m2->m_off += SLLINKHDRSPACE;
		bcopy(mtod(m, caddr_t), mtod(m2, caddr_t), len);
		m = m2;
	}
	m->m_len += sizeof(struct ifnet *);
	m->m_off -= sizeof(struct ifnet *);
	*mtod(m, struct ifnet **) = &sc->sc_if;
	return (m);
}

/*
 * convert buffer of data into packets.
 */
void
slinput(sc, buf, buflen)
	register struct sl_softc *sc;
	register u_char *buf;
	register int buflen;
{
	register struct mbuf *m, *m2;
	register int len, i, s, c;
#if NBPFILTER > 0
	u_char chdr[CHDR_LEN];
#endif

	while (--buflen >= 0) {
		++sc->sc_bytesrcvd;
		c = *buf++;
		switch (c) {

		case TRANS_FRAME_ESCAPE:
			if (sc->sc_escape)
				c = FRAME_ESCAPE;
			break;

		case TRANS_FRAME_END:
			if (sc->sc_escape)
				c = FRAME_END;
			break;

		case FRAME_ESCAPE:
			sc->sc_escape = 1;
			continue;

		case FRAME_END:
			m = sc->sc_mbuf;
			if ((m2 = m->m_next) != NULL)
				len = m->m_len;
			else {
				m2 = m;
				len = 0;
			}
			m2->m_len = sc->sc_mp - mtod(m2, u_char *);
			len += m2->m_len;
			if (len < 3)
				/* less than min length packet - ignore */
				goto newpack;
#if NBPFILTER > 0
			if (sc->sc_bpf)
				/*
				 * Save the compressed header, so we can
				 * tack it on later.  Note that we just
				 * copy the maximum number of bytes (16) --
				 * we will end up copying garbage in some
				 * cases but this is okay.  We remember
				 * where the buffer started so we can
				 * compute the new header length.
				 */
				bcopy(mtod(m, caddr_t), (caddr_t)chdr,
				    CHDR_LEN);
#endif
			c = (*mtod(m, char *) & 0xf0);
			if (c != (IPVERSION << 4)) {
				if (c & 0x80)
					c = TYPE_COMPRESSED_TCP;
				else if (c == TYPE_UNCOMPRESSED_TCP)
					*mtod(m, char *) &= 0x4f;
				/*
				 * we've got something that's not an IP packet.
				 * If compression is enabled, try to uncompress
				 * it.  Otherwise, if `auto-enable' compression
				 * is on and it's a reasonable packet,
				 * uncompress it then enable compression.
				 * Otherwise, drop it.
				 */
				if (sc->sc_if.if_flags & IFF_LINK0) {
					i = sl_uncompress_tcp(m, len,
					    (u_int)c, &sc->sc_comp);
					if (i <= 0)
						goto error;
				} else if ((sc->sc_if.if_flags & IFF_LINK1) &&
					   c == TYPE_UNCOMPRESSED_TCP &&
					   m->m_len >= 40) {
					i = sl_uncompress_tcp(m, len,
					     (u_int)c, &sc->sc_comp);
					if (i <= 0)
						goto error;
					sc->sc_if.if_flags |= IFF_LINK0;
				} else
					goto error;
			}
#if NBPFILTER > 0
			if (sc->sc_bpf) {
				/*
				 * Put the SLIP pseudo-"link header" in place.
				 * We couldn't do this any earlier since
				 * decompression probably moved the buffer
				 * pointer.  Then, invoke BPF.
				 */
				register u_char *hp;

				m->m_len += SLIP_HDRLEN;
				m->m_off -= SLIP_HDRLEN;
				hp = mtod(m, u_char *);
				hp[SLX_DIR] = SLIPDIR_IN;
				bcopy((caddr_t)chdr, (caddr_t)&hp[SLX_CHDR],
				    CHDR_LEN);
				bpf_mtap(sc->sc_bpf, m);
				m->m_len -= SLIP_HDRLEN;
				m->m_off += SLIP_HDRLEN;
			}
#endif
			m = sl_btom(sc);
			if (m == NULL)
				goto error;

			++sc->sc_if.if_ipackets;
			s = splimp();
			if (IF_QFULL(&ipintrq)) {
				IF_DROP(&ipintrq);
				++sc->sc_if.if_ierrors;
				m_freem(m);
			} else {
				IF_ENQUEUE(&ipintrq, m);
				schednetisr(NETISR_IP);
			}
			splx(s);
			goto newpack;
		}
		if (sc->sc_mp < sc->sc_ep) {
			*sc->sc_mp++ = c;
			sc->sc_escape = 0;
			continue;
		}

		/* Allocate second mbuf and cluster (but not third!) */
		m = sc->sc_mbuf;
		if (m->m_next == NULL) {
			MGET(m2, M_DONTWAIT, MT_DATA);
			if (m2) {
				MCLGET(m2);
				if (m2->m_len == MCLBYTES) {
					m->m_len = MCLBYTES - SLBUFOFFSET;

					sc->sc_mp = mtod(m2, u_char *);
					sc->sc_ep = sc->sc_mp + MCLBYTES;
					m->m_next = m2;

					/* Copy character in */
					*sc->sc_mp++ = c;
					sc->sc_escape = 0;
					continue;
				}
				m_free(m2);
			}
		}
		/*
		 * Because we use at most two clusters and must reserve
		 * some space at the front of the first one, we cannot
		 * accept more than (2 * MCLBYTES) - SLBUFOFFSET input
		 * bytes. Notice that if SLMTU is larger than this, we
		 * will be able to send larger packets than we can
		 * receive (and won't be able to talk to a similarly
		 * configured system).
		 */

error:
		++sc->sc_if.if_ierrors;
newpack:
		m = sc->sc_mbuf;
		if (m->m_next) {
			/* Free second cluster and mbuf */
			m_free(m->m_next);
			m->m_next = NULL;
		}
		SLNEWPACKET;
	}
}

/*
 * Copy exactly one packet to the stream below us (any more and our
 * type-of-service queuing turns into a nop).
 */
void
sl_wsrv(wq)
	register queue_t *wq;
{
	register struct sl_softc *sc;
	register struct mbuf *m;
	register u_char *op;
	register u_char *basep;
	register int s;
	register int len;

	sc = (struct sl_softc *)wq->q_ptr;
	s = splstr();
	if (sc->sc_outmb->b_datap->db_ref != 1) {
		wq->q_next->q_flag |= QWANTW;
		splx(s);
		return;
	}

	/*
	 * Get a packet and send it to the interface.
	 */
	(void) splimp();
	IF_DEQUEUE(&sc->sc_fastq, m);
	if (m == NULL)
		IF_DEQUEUE(&sc->sc_if.if_snd, m);
	splx(s);
	if (m == NULL)
		return;

	/*
	 * The extra FRAME_END will start up a new packet, and thus
	 * will flush any accumulated garbage.
	 */
	op = basep = sc->sc_outmb->b_datap->db_base;
	*op++ = FRAME_END;

	while (m) {
		register u_char *cp;
		register u_char *ep;
		register u_char c;
		register struct mbuf *m2;

		cp = mtod(m, u_char *);
		ep = cp + m->m_len;
		while (cp < ep) {
			switch (c = *cp++) {

			case FRAME_ESCAPE:
				*op++ = FRAME_ESCAPE;
				c = TRANS_FRAME_ESCAPE;
				break;

			case FRAME_END:
				*op++ = FRAME_ESCAPE;
				c = TRANS_FRAME_END;
			}
			*op++ = c;
		}
		MFREE(m, m2);
		m = m2;
	}
	*op++ = FRAME_END;
	len = op - basep;
	sc->sc_bytessent += len;
	++sc->sc_if.if_opackets;
	{
		register mblk_t *bp;

		/* swap current & pending output buffers */
		bp = sc->sc_outmb;
		sc->sc_outmb = sc->sc_pendmb;
		sc->sc_pendmb = bp;

		bp = dupb(bp);
		if (bp == NULL) {
			/*
			 * out of buffers -- flush everything so the
			 * next packet will force us to try again or
			 * we'll hang this line.
			 */
			sl_flush(FLUSHW, sc->sc_rq, wq);
			++sc->sc_if.if_collisions;
		} else {
			/*
			 * send block downstream, trying to make sure
			 * that we get a callback when it has been picked off.
			 */
			bp->b_wptr += len;
			s = splstr();
			wq->q_next->q_flag |= QWANTW;
			splx(s);
			putnext(wq, bp);
		}
	}
}
#endif
