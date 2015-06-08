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
    "@(#) $Header: if_sl.c,v 1.21 93/09/04 22:13:11 leres Exp $ (LBL)";
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
 */

/* originally from if_sl.c,v 1.11 84/10/04 12:54:47 rick Exp */

#include "sl.h"
#if NSL > 0
#ifndef LBL
#define LBL
#endif

#include "param.h"
#include "mbuf.h"
#include "buf.h"
#include "dk.h"
#include "socket.h"
#include "ioctl.h"
#include "file.h"
#include "tty.h"
#include "errno.h"

#include "if.h"
#include "netisr.h"
#include "route.h"
#if INET
#include "../netinet/in.h"
#include "../netinet/in_systm.h"
#ifndef sun
#include "../netinet/in_var.h"
#include "../netinet/ip.h"
#else
#include "../netinet/ip.h"
#include "../netinet/ip_var.h"
#include "protosw.h"
#endif
#endif

#ifndef sun
#include "machine/mtpr.h"
#endif

#include "slcompress.h"
#include "if_slvar.h"
#include "slip.h"

#include "bpfilter.h"
#if NBPFILTER > 0
#include <sys/time.h>
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
#define	SLMTU		576
#endif

#undef SLMTU
#define SLMTU (3 * 1024)	/* XXX testing */

/*
 * SLBUFOFFSET is the amount of space we reserve in front of the slip
 * packet. We need room for a pointer to the ifnet struct. If the
 * incoming packet is compressed, we need room to expand the header
 * into a max length tcp/ip header (120 bytes). Finally, if we support
 * BPF, we need an additional 16 bytes for the largest compressed
 * header plus a direction byte.
 */
#if NBPFILTER > 0
#define	SLBUFOFFSET	(sizeof(struct ifnet *) + 128 + SLIP_HDRLEN)
#else
#define	SLBUFOFFSET	(sizeof(struct ifnet *) + 128)
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
#define SLLINKHDRSPACE	20

/*
 * SLIP_HIWAT is the amount of data that will be queued 'downstream' of
 * us (i.e., in clists waiting to be picked up by the tty output
 * interrupt). If we queue a lot of data downstream, it's immune to our
 * t.o.s. queuing. E.g., if SLIP_HIWAT is 1024, the interactive traffic
 * in mixed telnet/ftp will see a 1 sec wait, independent of the mtu
 * (the wait is dependent on the ftp window size but that's typically
 * 1k - 4k). So, we want SLIP_HIWAT just big enough to amortize the
 * cost (in idle time on the wire) of the tty driver running off the
 * end of its clists & having to call back slstart for a new packet.
 * For a tty interface with any buffering at all, this cost will be
 * zero. Even with a totally brain dead interface (like the one on a
 * typical workstation), the cost will be <= 1 character time. So,
 * setting SLIP_HIWAT to ~100 guarantees that we'll lose at most 1%
 * while maintaining good interactive response.
 */
#define	SLIP_HIWAT	roundup(50, CBSIZE)
#define	CLISTRESERVE	1024		/* can't let clists get too low */

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
#ifndef __P
#ifdef __STDC__
#define __P(protos)	protos		/* full-blown ANSI C */
#else
#define __P(protos)	()		/* traditional C preprocessor */
#endif
#endif

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

#ifdef sun
#define t_sc t_linep
#else
#define t_sc T_LINEP
#endif

#if defined(SL_NIT) && defined(NIT)
#include "../h/time.h"
#include "../net/nit.h"
#include "../netinet/if_ether.h"
extern struct nit_cb nitcb;
int sl_donit = 0;
#endif

/* Compatibility with 4.2 BSD (and variants) */
#ifndef MCLBYTES
#define MCLBYTES CLBYTES
#endif
/* Sun OS3.x doesn't have a MCLGET.
 * The code below should work on either SunOS (this driver, however
 * will *NOT* work on a stock SunOS 4 system because of the slow,
 * broken, worthless, System-V "streams" tty driver).
 *
 * All BSD systems since 4.1c have MCLGET.
 * However 4.2bsd had a second, unused parameter.  Some versions of cpp
 * may complain if you compile this routine under a stock 4.2bsd.  For
 * reasons I have never understood, various vendors chose to break BSD
 * in mysterious ways (e.g., DEC's Ultrix changed the definition of
 * m_off to make mbuf cluster use a factor of two more costly -- no
 * doubt this was done to encourage the sale of faster processors since
 * there is no technical justification whatsoever for the change).  If
 * you are stuck with one of these abortions and, for reasons best
 * known to yourself, don't want to upgrade to the BSD network code
 * freely available via anonymous ftp from ucbarpa.berkeley.edu, the
 * following macros might give you a prototype to work from.  Then
 * again, they might not. Good luck.
 */
#ifdef sun
#ifndef MCLGET
#define MCLGET(m) { int ms = splimp(); (void)mclget(m); (void) splx(ms); }
#endif
#endif

int	slioctl __P((struct ifnet *, int, caddr_t));
int	slopen __P((dev_t, struct tty *));
int	sloutput __P((struct ifnet *, struct mbuf *, struct sockaddr *));
int	sltioctl __P((struct tty *, int, caddr_t, int));
static	int slinit __P((register struct sl_softc *));
static	struct mbuf *sl_btom __P((struct sl_softc *));
static	void if_down __P((struct ifnet *));
static	void if_rtdelete __P((struct ifnet *));
void	slattach __P((void));
void	slclose __P((struct tty *));
void	slinput __P((int, struct tty *));
void	slstart __P((struct tty *));

/*
 * Called from boot code to establish sl interfaces.
 */
void
slattach()
{
	register struct sl_softc *sc;
	register int nsl;

	if (sl_softc[0].sc_if.if_name == NULL)
		for (nsl = 0, sc = sl_softc; nsl < NSL; ++nsl, ++sc) {
			sc->sc_if.if_name = "sl";
			sc->sc_if.if_unit = nsl;
			sc->sc_if.if_mtu = SLMTU;
			sc->sc_if.if_flags = IFF_POINTOPOINT;
			sc->sc_if.if_ioctl = slioctl;
			sc->sc_if.if_output = sloutput;
			sc->sc_if.if_snd.ifq_maxlen = 50;
			sc->sc_fastq.ifq_maxlen = 32;
			if_attach(&sc->sc_if);
#if NBPFILTER > 0
			bpfattach(&sc->sc_bpf, &sc->sc_if, DLT_SLIP,
				  SLIP_HDRLEN);
#endif
		}
}

static int
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
	return (1);
}

/*
 * Line specific open routine.
 * Attach the given tty to the first available sl unit.
 */
/* ARGSUSED */
int
slopen(dev, tp)
	dev_t dev;
	register struct tty *tp;
{
	register struct sl_softc *sc;
	register int nsl;

	if (!suser())
		return (EPERM);
	if (tp->t_line == SLIPDISC)
		return (EBUSY);

	for (nsl = NSL, sc = sl_softc; --nsl >= 0; ++sc)
		if (sc->sc_ttyp == NULL) {
			if (!slinit(sc))
				return (ENOBUFS);
			tp->t_sc = (caddr_t)sc;
			sc->sc_ttyp = tp;
			ttyflush(tp, FREAD | FWRITE);
			return (0);
		}
	return (ENXIO);
}

#ifdef sun
/* XXX - Sun OS3 is missing these 4bsd routines.
 *
 * Mark an interface down and notify protocols of
 * the transition.
 */
static void
if_down(ifp)
	register struct ifnet *ifp;
{
	register struct ifqueue *ifq = &ifp->if_snd;
	register struct mbuf *m, *n;

	ifp->if_flags &=~ IFF_UP;
	pfctlinput(PRC_IFDOWN, &ifp->if_addr);

	/* Flush the interface queue. */
	n = ifq->ifq_head;
	while (m = n) {
		n = m->m_act;
		m_freem(m);
	}
	ifq->ifq_head = NULL;
	ifq->ifq_tail = NULL;
	ifq->ifq_len = 0;
}

static void
if_rtdelete(ifp)
	register struct ifnet *ifp;
{
	static struct rtentry route;

	if (ifp->if_flags & IFF_ROUTE) {
		route.rt_dst = ifp->if_dstaddr;
		route.rt_gateway = ifp->if_addr;
		route.rt_flags = RTF_HOST|RTF_UP;
		(void) rtrequest(SIOCDELRT, &route);
		ifp->if_flags &=~ IFF_ROUTE;
	}
}
#endif

/*
 * Line specific close routine.
 * Detach the tty from the sl unit.
 * Mimics part of ttyclose().
 */
void
slclose(tp)
	struct tty *tp;
{
	register struct sl_softc *sc;
	int s;

	ttywflush(tp);
	tp->t_line = NULL;
	s = splimp();
	sc = (struct sl_softc *)tp->t_sc;
	if (sc != NULL) {
#ifdef sun
		if_rtdelete(&sc->sc_if);
#endif
		if_down(&sc->sc_if);
		sc->sc_ttyp = NULL;
		tp->t_sc = NULL;
		if (sc->sc_mbuf)
			m_freem(sc->sc_mbuf);
		sc->sc_mbuf = NULL;
		sc->sc_moff = 0;
		sc->sc_ep = NULL;
		sc->sc_mp = NULL;
	}
	splx(s);
}

/*
 * Line specific (tty) ioctl routine.
 * Provide a way to get the sl unit number.
 */
/* ARGSUSED */
int
sltioctl(tp, cmd, data, flag)
	struct tty *tp;
	int cmd;
	caddr_t data;
	int flag;
{

	if (cmd == TIOCGETD) {
		*(int *)data = ((struct sl_softc *)tp->t_sc)->sc_if.if_unit;
		return (0);
	}
	return (-1);
}

/*
 * Queue a packet.  Start transmission if not active.
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

	/*
	 * `Cannot happen' (see slioctl).  Someday we will extend
	 * the line protocol to support other address families.
	 */
	if (dst->sa_family != AF_INET) {
		printf("sl%d: af%d not supported\n", ifp->if_unit,
			dst->sa_family);
		m_freem(m);
		return (EAFNOSUPPORT);
	}

	sc = &sl_softc[ifp->if_unit];
	if (sc->sc_ttyp == NULL) {
		m_freem(m);
		return (ENETDOWN);	/* sort of */
	}
	if ((sc->sc_ttyp->t_state & TS_CARR_ON) == 0) {
		m_freem(m);
		return (EHOSTUNREACH);
	}
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
			/* do TOS queueing based on port number XXX */
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
	if (sc->sc_ttyp->t_outq.c_cc == 0)
		slstart(sc->sc_ttyp);
	splx(s);
	return (0);
}

/*
 * Start output on interface.  Get another datagram
 * to send from the interface queue and map it to
 * the interface before starting output.
 */
void
slstart(tp)
	register struct tty *tp;
{
	register struct sl_softc *sc = (struct sl_softc *)tp->t_sc;
	register struct mbuf *m;
	register u_char *cp;
	int s;
	struct mbuf *m2;
	extern int cfreecount;

	for (;;) {
		/*
		 * If there is more in the output queue, just send it now.
		 * We are being called in lieu of ttstart and must do what
		 * it would.
		 */
		if (tp->t_outq.c_cc != 0) {
			(*tp->t_oproc)(tp);
			if (tp->t_outq.c_cc > SLIP_HIWAT)
				return;
		}
		/*
		 * This happens briefly when the line shuts down.
		 */
		if (sc == NULL)
			return;

		/*
		 * Get a packet and send it to the interface.
		 */
		s = splimp();
		IF_DEQUEUE(&sc->sc_fastq, m);
		if (m == NULL)
			IF_DEQUEUE(&sc->sc_if.if_snd, m);
#if defined(SL_NIT) && defined(NIT)
		if (m && sl_donit && nitcb.ncb_next != &nitcb) {
			/* do nit processing if there's anyone listening */
			static struct ether_header oheader = { { 1 }, { 2 } };
			struct nit_ii nii;
			int len = 0;

			m2 = m;
			do {
				len += m2->m_len;
			} while (m2 = m2->m_next);

			oheader.ether_type = *mtod(m, u_char *);
			nii.nii_header = (caddr_t)&oheader;
			nii.nii_hdrlen = sizeof(oheader);
			nii.nii_type = oheader.ether_type;
			nii.nii_datalen = len;
			nii.nii_promisc = 0;
			nit_tap(&sc->sc_if, m, &nii);
		}
#endif
		splx(s);
		if (m == NULL)
			return;
		/*
		 * If system is getting low on clists, just flush our
		 * output queue (if the stuff was important, it'll get
		 * retransmitted).
		 */
		if (cfreecount < CLISTRESERVE + SLMTU) {
			m_freem(m);
			++sc->sc_if.if_collisions;
			continue;
		}

		/*
		 * The extra FRAME_END will start up a new packet, and thus
		 * will flush any accumulated garbage.  We do this whenever
		 * the line may have been idle for some time.
		 */
		if (tp->t_outq.c_cc == 0) {
			++sc->sc_bytessent;
			(void) putc(FRAME_END, &tp->t_outq);
		}

		while (m) {
			register u_char *ep;

			cp = mtod(m, u_char *); ep = cp + m->m_len;
			while (cp < ep) {
				/*
				 * Find out how many bytes in the string we can
				 * handle without doing something special.
				 */
				register u_char *bp = cp;

				while (cp < ep) {
					switch (*cp++) {

					case FRAME_ESCAPE:
					case FRAME_END:
						--cp;
						goto out;
					}
				}
				out:
				if (cp > bp) {
					/*
					 * Put n characters at once
					 * into the tty output queue.
					 */
					if (b_to_q((char *)bp, cp - bp,
					    &tp->t_outq))
						break;
					sc->sc_bytessent += cp - bp;
				}
				/*
				 * If there are characters left in the mbuf,
				 * the first one must be special..
				 * Put it out in a different form.
				 */
				if (cp < ep) {
					if (putc(FRAME_ESCAPE, &tp->t_outq))
						break;
					if (putc(*cp++ == FRAME_ESCAPE ?
					   TRANS_FRAME_ESCAPE : TRANS_FRAME_END,
					   &tp->t_outq)) {
						(void) unputc(&tp->t_outq);
						break;
					}
					sc->sc_bytessent += 2;
				}
			}
			MFREE(m, m2);
			m = m2;
		}

		if (putc(FRAME_END, &tp->t_outq)) {
			/*
			 * Not enough room.  Remove a char to make room
			 * and end the packet normally.
			 * If you get many collisions (more than one or two
			 * a day) you probably do not have enough clists
			 * and you should increase "nclist" in param.c.
			 */
			(void) unputc(&tp->t_outq);
			(void) putc(FRAME_END, &tp->t_outq);
			++sc->sc_if.if_collisions;
		} else {
			++sc->sc_bytessent;
			++sc->sc_if.if_opackets;
		}
	}
}

/*
 * Possibly swap mbuf's; add ifnet pointer.
 */
static struct mbuf *
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
#if BSD == 43
	m->m_len += sizeof(struct ifnet *);
	m->m_off -= sizeof(struct ifnet *);
	*mtod(m, struct ifnet **) = &sc->sc_if;
#endif
	return (m);
}

/*
 * tty interface receiver interrupt.
 */
void
slinput(c, tp)
	register int c;
	register struct tty *tp;
{
	register struct sl_softc *sc;
	register struct mbuf *m, *m2;
	register int len, i, s;
#if NBPFILTER > 0
	u_char chdr[CHDR_LEN];
#endif
	++tk_nin;
	sc = (struct sl_softc *)tp->t_sc;
	if (sc == NULL)
		return;

	++sc->sc_bytesrcvd;
	c &= 0xff;
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
		return;

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

#if defined(SL_NIT) && defined(NIT)
		if (sl_donit && nitcb.ncb_next != &nitcb) {
			/* do nit processing if there's anyone listening */
			static struct ether_header iheader = { { 2 }, { 1 } };
			static struct mbuf mb;
			struct nit_ii nii;

			/* XXX broken! */
			m = &mb;
			iheader.ether_type = *sc->sc_buf;
			nii.nii_header = (caddr_t)&iheader;
			nii.nii_hdrlen = sizeof(iheader);
			nii.nii_type = iheader.ether_type;
			nii.nii_datalen = len;
			nii.nii_promisc = 0;
			nit_tap(&sc->sc_if, m, &nii);
		}
#endif
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
			bcopy(mtod(m, caddr_t), (caddr_t)chdr, CHDR_LEN);
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
			 * it.	Otherwise, if `auto-enable' compression
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
		return;
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
				return;
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

/*
 * Process an ioctl request.
 */
int
slioctl(ifp, cmd, data)
	register struct ifnet *ifp;
	int cmd;
	caddr_t data;
{
	int s = splimp(), error = 0;
#ifndef sun
	register struct ifaddr *ifa = (struct ifaddr *)data;

	switch (cmd) {

	case SIOCSIFADDR:
		if (ifa->ifa_addr.sa_family == AF_INET)
			ifp->if_flags |= IFF_UP;
		else
			error = EAFNOSUPPORT;
		break;

	case SIOCSIFDSTADDR:
		if (ifa->ifa_addr.sa_family != AF_INET)
			error = EAFNOSUPPORT;
		break;

	default:
		error = EINVAL;
	}
#else
	switch (cmd) {

	case SIOCSIFADDR:
#ifdef LBL
		{
		register u_long addr;

		if_rtdelete(ifp);
		ifp->if_addr = *(struct sockaddr *)data;
		addr = ((struct sockaddr_in *)data)->sin_addr.s_addr;
		if (IN_CLASSA(addr))
			ifp->if_netmask = IN_CLASSA_NET;
		else if (IN_CLASSB(addr))
			ifp->if_netmask = IN_CLASSB_NET;
		else
			ifp->if_netmask = IN_CLASSC_NET;
		ifp->if_net = addr & ifp->if_netmask;
		ifp->if_host[0] = addr &~ ifp->if_netmask;
		ifp->if_subnetmask |= ifp->if_netmask;
		ifp->if_subnet = addr & ifp->if_subnetmask;
		ifp->if_flags |= IFF_UP|IFF_RUNNING;
		ifp->if_flags &=~ IFF_BROADCAST;
		/* set up routing table entry */
		error = rtinit(&ifp->if_dstaddr, &ifp->if_addr,
				RTF_HOST|RTF_UP);
		ifp->if_flags |= IFF_ROUTE;
		}
#else
		if_rtdelete(ifp);
		ifp->if_addr = *(struct sockaddr *)data;
		ifp->if_net = in_netof(((struct sockaddr_in *)data)->sin_addr);
		ifp->if_flags |= IFF_UP|IFF_RUNNING;
		ifp->if_flags &=~ IFF_BROADCAST;
		/* set up routing table entry */
		error = rtinit(&ifp->if_dstaddr, &ifp->if_addr,
				RTF_HOST|RTF_UP);
		ifp->if_flags |= IFF_ROUTE;
#endif
		break;

	case SIOCSIFDSTADDR:
		/* all the real work is done for us in ../net/if.c */
		break;

	default:
		error = EINVAL;
	}
#endif
	splx(s);
	return (error);
}
#endif
