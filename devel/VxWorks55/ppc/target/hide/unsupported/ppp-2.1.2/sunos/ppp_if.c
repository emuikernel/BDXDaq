/*
  ppp_if.c - Streams PPP interface module

  top level module handles if_ and packetizing PPP packets.

  Copyright (C) 1990  Brad K. Clements, All Rights Reserved
  See copyright notice in NOTES

  $Id: ppp_if.c,v 1.9 1994/06/09 01:39:47 paulus Exp $
*/

#define	VJC	1
#include <sys/types.h>

#ifndef PPP_VD
#include "ppp.h"
#endif

#if NPPP > 0

#define	STREAMS	1

#define	PPP_STATS	1	/* keep statistics */
#define	PPP_SNIT	1	/* pass packets to Streams Network Interface Tap */
#define	DEBUGS		1

#include <sys/param.h>
#include <sys/stream.h>
#include <sys/stropts.h>

#include <sys/user.h>
#include <sys/systm.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <sys/ioctl.h>
#include <sys/file.h>
#include <sys/uio.h>
#include <net/if.h>
#include <net/route.h>
#include <net/netisr.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/in_var.h>
#include <netinet/ip.h>

#ifdef	PPP_STATS
#define	INCR(comp)	++p->pii_stats.comp
#else
#define	INCR(comp)
#endif

#ifdef	VJC
#include <net/slcompress.h>
#endif

#include <net/ppp_str.h>

#define MAX_PKTSIZE	4096	/* max packet size including framing */
#define PPP_FRAMING	6	/* 4-byte header + 2-byte FCS */
#define MAX_IPHDR	128	/* max TCP/IP header size */
#define MAX_VJHDR	20	/* max VJ compressed header size (?) */

#ifdef	DEBUGS
#include <sys/syslog.h>
#define	DLOG(s,a)	if (p->pii_flags&PII_FLAGS_DEBUG) log(LOG_DEBUG, s, a)
#else
#define	DLOG(s)	{}
#endif

#ifdef PPP_SNIT
#include <net/nit_if.h>
#include <netinet/if_ether.h>
/* Use a fake link level header to make etherfind and tcpdump happy. */
static struct ether_header header = {{1}, {2}, ETHERTYPE_IP};
static struct nit_if nif = {(caddr_t)&header, sizeof(header), 0, 0};
#endif

static	int	ppp_if_open(), ppp_if_close(), ppp_if_rput(), ppp_if_wput(),
		ppp_if_wsrv(), ppp_if_rsrv();

static 	struct	module_info	minfo ={
	0xbad,"ppp_if",0, INFPSZ, 16384, 4096
};

static	struct	qinit	r_init = {
	ppp_if_rput, ppp_if_rsrv, ppp_if_open, ppp_if_close, NULL, &minfo, NULL
};
static	struct	qinit	w_init = {
	ppp_if_wput, ppp_if_wsrv, ppp_if_open, ppp_if_close, NULL, &minfo, NULL
};
struct	streamtab	ppp_ifinfo = {
	&r_init, &w_init, NULL, NULL, NULL
};

typedef	struct ppp_if_info	PII;

PII	pii[NPPP];

int	ppp_output(), ppp_ioctl();


int
ppp_attach(unit)
    int	unit;
{
    register struct ifnet *ifp = &pii[unit].pii_ifnet;

    ifp->if_name = "ppp";
    ifp->if_mtu  = PPP_MTU;
    ifp->if_flags = IFF_POINTOPOINT;
    ifp->if_unit  = unit;
    ifp->if_ioctl = ppp_ioctl;
    ifp->if_output = ppp_output;
    ifp->if_snd.ifq_maxlen = IFQ_MAXLEN;
    if_attach(ifp);
    pii[unit].pii_flags |= PII_FLAGS_ATTACHED;
}


int
ppp_unattach(unit)
    int	unit;
{
    register struct ifnet *ifp = &pii[unit].pii_ifnet;
    register struct ifnet **p;
    int s;

    if (!(pii[unit].pii_flags & PII_FLAGS_ATTACHED))
	return 0;

    /* remove interface from interface list */
    s = splimp();
    for (p = &ifnet; *p; p = &((*p)->if_next)) {
	if (*p == ifp) {
	    *p = (*p)->if_next;

	    /* mark it down and flush it's que */
	    if_down(ifp);

	    /* free any addresses hanging off the intf */
	    if_release_addrs(ifp);

	    pii[unit].pii_flags &= ~PII_FLAGS_ATTACHED;

	    (void)splx(s);
	    return 0;
	}
    }

    (void)splx(s);
    return -1;
}


static int	/* should be void */
if_release_addrs(ifp)
register struct ifnet *ifp;
{
    register struct in_ifaddr **addr;
    register struct ifaddr *ifa, *ifanxt;
    register int s;
 
    if_delete_route(ifp);
 
    for (addr = &in_ifaddr; *addr; ) {
	if ((*addr)->ia_ifp == ifp)
	    *addr = (*addr)->ia_next;
	else
	    addr = &((*addr)->ia_next);
    }
 
    /*
     * Free all mbufs holding down this interface's address(es).
     */
    for (ifa = ifp->if_addrlist; ifa; ifa = ifanxt) {
	ifanxt = ifa->ifa_next;
	m_free(dtom(ifa));
    }
    ifp->if_addrlist = 0;
}

/*
 * Delete routes to the specified interface.
 * Hacked from rtrequest().
 */
static int	/* should be void */
if_delete_route(ifp)
struct ifnet *ifp;
{
    extern int rttrash;		/* routes not in table but not freed */
    register struct mbuf **mprev, *m;
    register struct rtentry *route;
    register int i;
 
    /* search host rt tbl */
    for (i = 0; i < RTHASHSIZ; i++) {
	mprev = &rthost[i];
	while (m = *mprev) {
	    route = mtod(m, struct rtentry *);
	    if (route->rt_ifp == ifp) {
		*mprev = m->m_next;
		if (route->rt_refcnt > 0) {
		    route->rt_flags &= ~RTF_UP;
		    rttrash++;
		    m->m_next = 0;
		} else {
		    m_free(m);
		}
	    } else
		mprev = &m->m_next;
	}
    }
 
    /* search net rt tbl */
    for (i = 0; i < RTHASHSIZ; i++) {
	mprev = &rtnet[i];
	while (m = *mprev) {
	    route = mtod(m, struct rtentry *);
	    if (route->rt_ifp == ifp) {
		*mprev = m->m_next;
		if (route->rt_refcnt > 0) {
		    route->rt_flags &= ~RTF_UP;
		    rttrash++;
		    m->m_next = 0;
		} else {
		    m_free(m);
		}
	    } else
		mprev = &m->m_next;
	}
    }
} 

int
ppp_busy()
{
    int x;

    for (x = 0; x < NPPP; x++) {
	if (pii[x].pii_flags & PII_FLAGS_INUSE)
	    return 1;
    }
    return 0;
}

static int
ppp_if_open(q, dev, flag, sflag)
    queue_t	*q;
    dev_t	dev;
    int		flag, sflag;

{
    register PII	*p;
    register int	x;
    int	s;

    if (!suser()) {
	u.u_error = EPERM;
	return (OPENFAIL);
    }

#if 0
    if (q->q_ptr) {
	u.u_error = EBUSY;
	return (OPENFAIL);
    }
#endif

    s = splstr();
    p = (PII *) q->q_ptr;
    if (!q->q_ptr) {
	for (x = 0; x < NPPP; x++)
	    if (!(pii[x].pii_flags & PII_FLAGS_INUSE))
		break;
	if (x == NPPP) {		/* all buffers in use */
	    splx(s);			/* restore processor state */
	    u.u_error = ENOBUFS;
	    return (OPENFAIL);
	}

	p = &pii[x];
#ifdef	VJC
	sl_compress_init(&p->pii_sc_comp);
#endif
#ifdef	PPP_STATS
	bzero(&p->pii_stats, sizeof(p->pii_stats));
#endif
	if (!(p->pii_flags & PII_FLAGS_ATTACHED))
	    ppp_attach(x);			/* attach it */
	else
	    p->pii_ifnet.if_mtu = PPP_MTU;
	p->pii_writeq = WR(q);
	/* set write Q and read Q to point here */
	WR(q)->q_ptr = q->q_ptr = (caddr_t) p;
	p->pii_ifnet.if_flags |= IFF_RUNNING;
	p->pii_flags |= PII_FLAGS_INUSE;
	DLOG("ppp_if%d: open\n", x);
    }

    splx(s);
    return (0);
}

static int
ppp_if_close(q)
    queue_t	*q;			/* queue info */
{
    PII	*p = (PII *) q->q_ptr;
    int	s;

    s = splimp();
    p->pii_flags &= ~PII_FLAGS_INUSE;
    if_down(&p->pii_ifnet);
    p->pii_ifnet.if_flags &= ~IFF_RUNNING;
    q->q_ptr = NULL;
    DLOG("ppp_if%d: closed\n", p - pii);
    splx(s);
    return(0);			/* no work to be done */
}


static int
ppp_if_wput(q, mp)
    queue_t  *q;
    register mblk_t *mp;
{
    register struct iocblk	*i;
    register PII	*p;
    int bits, flags;

    switch (mp->b_datap->db_type) {

    case M_FLUSH :
	if (*mp->b_rptr & FLUSHW)
	    flushq(q, FLUSHDATA);
	putnext(q, mp);		/* send it along too */
	break;

    case M_DATA :
	putq(q, mp);	/* queue it for my service routine */
	break;

    case M_IOCTL :
	i = (struct iocblk *) mp->b_rptr;
	p = (PII *) q->q_ptr;
	switch (i->ioc_cmd) {

	case SIOCSIFVJCOMP:	/* enable or disable VJ compression */
#ifdef	VJC
	    if (i->ioc_count != sizeof(u_char)) {
		putnext(q, mp);
		break;
	    }
	    bits = *(u_char *) mp->b_cont->b_rptr;
	    DLOG("ppp_if: SIFVJCOMP %d\n", bits);
	    if (bits & 1) 
		p->pii_flags |= PII_FLAGS_VJC_ON;
	    else
		p->pii_flags &= ~PII_FLAGS_VJC_ON;
	    if (bits & 2)
		p->pii_flags |= PII_FLAGS_VJC_NOCCID;
	    else
		p->pii_flags &= ~PII_FLAGS_VJC_NOCCID;
	    if (bits & 4)
		p->pii_flags |= PII_FLAGS_VJC_REJ;
	    else
		p->pii_flags &= ~PII_FLAGS_VJC_REJ;
	    bits >>= 4;		/* now max conn id. */
	    if (bits)
		sl_compress_setup(&p->pii_sc_comp, bits);
	    mp->b_datap->db_type = M_IOCACK;
#else
	    mp->b_datap->db_type = M_IOCNAK;
	    i->ioc_error = EINVAL;
	    i->ioc_count = 0;
#endif
	    qreply(q,mp);
	    break;

	case SIOCGETU :	/* get unit number */
	    if ((mp->b_cont = allocb(sizeof(int), BPRI_MED)) != NULL) {
		*(int *) mp->b_cont->b_wptr = p->pii_ifnet.if_unit;
		mp->b_cont->b_wptr += i->ioc_count  = sizeof(int);
		mp->b_datap->db_type = M_IOCACK;
	    } else {
		i->ioc_error = ENOSR;
		i->ioc_count = 0;
		mp->b_datap->db_type = M_IOCNAK;
	    }
	    qreply(q,mp);
	    break;

	case SIOCSIFDEBUG :
	    /* catch it on the way past to set our debug flag as well */
	    if (i->ioc_count == sizeof(int)) {
		flags = *(int *)mp->b_cont->b_rptr;
		if (flags & 1)
		    p->pii_flags |= PII_FLAGS_DEBUG;
		else
		    p->pii_flags &= ~PII_FLAGS_DEBUG;
	    }
	    putnext(q, mp);
	    break;

	default:		/* unknown IOCTL call */
	    putnext(q,mp);	/* pass it along */
	}
	break;

    default:
	putnext(q,mp);	/* don't know what to do with this, so send it along*/
    }
}

static int
ppp_if_wsrv(q)
    queue_t	*q;
{
    register mblk_t *mp;
    register PII *p;

    p = (PII *) q->q_ptr;

    while ((mp = getq(q)) != NULL) {
	/*
	 * we can only get M_DATA types into our Queue,
	 * due to our Put function
	 */
	if (!canput(q->q_next)) {
	    putbq(q, mp);
	    return;
	}

	/* increment count of outgoing packets */
	p->pii_ifnet.if_opackets++;
	INCR(ppp_opackets);

	/* just pass it along, nothing to do in this direction */
	putnext(q, mp);
    }	/* end while */
}


static int
ppp_if_rput(q, mp)
    queue_t *q;
    register mblk_t *mp;
{
    register u_char	*c;
    register PII	*p;

    switch (mp->b_datap->db_type) {

    case M_FLUSH :
	if (*mp->b_rptr & FLUSHR)
	    flushq(q, FLUSHDATA);
	putnext(q, mp);		/* send it along too */
	break;

    case M_DATA :
	putq(q, mp);	/* queue it for my service routine */
	break;

    case M_CTL :		/* could be a message from below */
	p = (PII *) q->q_ptr;
	c = (u_char *) mp->b_rptr;
	switch (c)	{
	case IF_INPUT_ERROR :
	    p->pii_ifnet.if_ierrors++;
	    INCR(ppp_ierrors);
	    DLOG("ppp_if: input error inc to %d\n",
		 p->pii_ifnet.if_ierrors);
	    break;
	case IF_OUTPUT_ERROR :
	    p->pii_ifnet.if_oerrors++;
	    INCR(ppp_oerrors);
	    DLOG("ppp_if: output error inc to %d\n",
		 p->pii_ifnet.if_oerrors);
	    break;
	}
	freemsg(mp);
	break;

    default :
	/* don't know what to do with this, so send it along*/
	putnext(q,mp);
    }
}

static int
ppp_if_rsrv(q)
    queue_t	*q;
{
    register mblk_t *mp,*m0;
#ifdef	VJC
    register mblk_t *mvjc;
    unsigned char *cp;
    int dlen;
#endif
    register PII *p;
    struct mbuf	*mb1, *mb2, *mbtail;
    int	len, xlen, count, s;
    u_char *rptr;
    int address, control;

    p = (PII *) q->q_ptr;

    while ((mp = getq(q)) != NULL) {
	/*
	 * we can only get M_DATA types into our Queue,
	 * due to our Put function
	 */
	len = msgdsize(mp);
#ifdef	PPP_STATS
	p->pii_stats.ppp_ibytes += len;
#endif

	/* make sure ppp_header is completely in first block */
	if (mp->b_wptr - mp->b_rptr < PPP_HDRLEN
	    && !pullupmsg(mp, PPP_HDRLEN)) {
	    DLOG("pullupmsg failed!\n", 0);
	    freemsg(mp);
	    p->pii_ifnet.if_ierrors++;
	    continue;
	}
	m0 = mp;	/* remember first message block */

#ifdef	VJC
	switch (PPP_PROTOCOL(mp->b_rptr)) {
	case PPP_VJC_COMP :
	    if (p->pii_flags & PII_FLAGS_VJC_REJ) {
		DLOG("VJC rejected\n", 0);
		freemsg(mp);
		continue;				
	    }
	    address = PPP_ADDRESS(mp->b_rptr);
	    control = PPP_CONTROL(mp->b_rptr);

	    xlen = MIN(len, MAX_VJHDR + PPP_HDRLEN);

	    /* get a buffer for the IP header */
	    if (!(mvjc = allocb(xlen + MAX_IPHDR, BPRI_MED))) {
		DLOG("allocb mvjc failed (%d)\n", xlen + MAX_IPHDR);
		putbq(q, mp);
		qenable(q);
		return;
	    }
	    mvjc->b_wptr += MAX_IPHDR;
	    linkb(mvjc, mp);
	    if (!pullupmsg(mvjc, xlen + MAX_IPHDR)) {
		DLOG("pullupmsg mvjc %d failed\n", xlen + MAX_IPHDR);
		freemsg(mvjc);
		continue;
	    }
	    m0 = mp = mvjc;

	    cp = mvjc->b_rptr + MAX_IPHDR + PPP_HDRLEN;
	    xlen = mp->b_wptr - cp;
	    dlen = sl_uncompress_tcp_part(&cp, xlen, len - PPP_HDRLEN,
					  TYPE_COMPRESSED_TCP,
					  &p->pii_sc_comp);
	    if (dlen == 0) {
		DLOG("ppp: sl_uncompress failed on type Compressed\n",0);
		freemsg(mp);
		continue;
	    }
	    mp->b_rptr = cp - PPP_HDRLEN;
	    mp->b_wptr = cp + xlen + dlen - len + PPP_HDRLEN;

	    /*
	     * Set up the protocol field, in case this message
	     * gets put back on the queue.
	     */
	    mp->b_rptr[0] = address;
	    mp->b_rptr[1] = control;
	    mp->b_rptr[2] = 0;
	    mp->b_rptr[3] = PPP_IP;
	    DLOG("ppp_if%d: VJC decompressed\n", p - pii);
	    break;

	case PPP_VJC_UNCOMP :
	    if (p->pii_flags & PII_FLAGS_VJC_REJ) {
		DLOG("VJU rejected\n", 0);
		freemsg(mp);
		continue;
	    }

	    cp = (unsigned char *) mp->b_rptr + PPP_HDRLEN;
	    if (!sl_uncompress_tcp(&cp, 1, TYPE_UNCOMPRESSED_TCP,
				   &p->pii_sc_comp)) {
		DLOG("ppp: sl_uncompress failed on type Uncompresed\n",0);
		freemsg(mp);
		continue;
	    }

	    /*
	     * Set up the protocol field, in case this message
	     * gets put back on the queue.
	     */
	    mp->b_rptr[3] = PPP_IP;
	    DLOG("ppp_if%d: VJU decompressed\n", p - pii);
	    break;
	}
#endif

	switch (PPP_PROTOCOL(mp->b_rptr)) {
	default:
	    if (!canput(q->q_next)) {
		putbq(q, mp);
		return;
	    }
	    INCR(ppp_ipackets);
	    p->pii_ifnet.if_ipackets++;
	    putnext(q, mp);
	    continue;

	case PPP_IP:
	    /*
	     * Don't let packets through until IPCP is up.
	     */
	    INCR(ppp_ipackets);
	    p->pii_ifnet.if_ipackets++;

	    if (!(p->pii_ifnet.if_flags & IFF_UP)) {
		DLOG("pkt ignored - IP down\n", 0);
		freemsg(mp);
		continue;
	    }

	    /*
	     * Get the first mbuf and put the struct ifnet * in.
	     */
	    MGET(mb1, M_DONTWAIT, MT_DATA);
	    if (mb1 == NULL) {
		p->pii_ifnet.if_ierrors++;
		freemsg(m0);
		continue;
	    }
	    *mtod(mb1, struct ifnet **) =  &p->pii_ifnet;
	    len = MLEN - sizeof(struct ifnet *);
	    mbtail = mb2 = mb1;
	    mb1->m_len = sizeof(struct ifnet *);

	    rptr = mp->b_rptr + PPP_HDRLEN;
	    xlen = mp->b_wptr - rptr;
	    for(;;) {
		if (xlen == 0) {	/* move to the next mblk */
		    mp = mp->b_cont;
		    if (mp == NULL)
			break;
		    xlen = mp->b_wptr - (rptr = mp->b_rptr);
		}
		if (len == 0) {
		    MGET(mb2, M_DONTWAIT, MT_DATA);
		    if (!mb2) {
			/* if we couldn't get a buffer, drop the packet */
			p->pii_ifnet.if_ierrors++;
			m_freem(mb1);	/* discard what we've used already */
			mb1 = NULL;
			break;
		    }
		    len = MLEN;
		    mb2->m_len = 0;
		    mbtail->m_next = mb2;
		    mbtail = mb2;
		}
		count = MIN(xlen, len);
		bcopy((char *) rptr, mtod(mb2, char *) + mb2->m_len, count);
		rptr += count;
		len -= count;
		xlen -= count;
		mb2->m_len += count;
	    }

	    freemsg(m0);
	    if (mb1 == NULL)
		continue;

#ifdef PPP_SNIT
	    if (p->pii_ifnet.if_flags & IFF_PROMISC) {
		struct mbuf *m = mb1;

		len = 0;
		do {
		    len += m->m_len;
		} while (m = m->m_next);
		nif.nif_bodylen = len - sizeof(struct ifnet *);
		mb1->m_off += sizeof(struct ifnet *);
		snit_intr(&p->pii_ifnet, mb1, &nif);
		mb1->m_off -= sizeof(struct ifnet *);
	    }
#endif
	    s = splimp();
	    if (IF_QFULL(&ipintrq)) {
		IF_DROP(&ipintrq);
		p->pii_ifnet.if_ierrors++;
		m_freem(mb1);
	    }
	    else {
		IF_ENQUEUE(&ipintrq, mb1);
		schednetisr(NETISR_IP);
	    }
	    splx(s);
	}
    }	/* end while */
}

/* ifp output procedure */
int
ppp_output(ifp, m0, dst)
    struct ifnet *ifp;
    struct mbuf *m0;
    struct sockaddr *dst;
{
    register PII	*p = &pii[ifp->if_unit];
    struct mbuf	*m1;
    int	error,s, len;
    u_short	protocol;
#ifdef	VJC
    u_char	type;
#endif
    mblk_t	*mp;

    error = 0;
    if (!(ifp->if_flags & IFF_UP)) {
	error = ENETDOWN;
	goto getout;
    }

    switch (dst->sa_family) {
#ifdef	INET
    case AF_INET :
#ifdef PPP_SNIT
	if (ifp->if_flags & IFF_PROMISC) {
	    struct mbuf *m = m0;

	    len = 0;
	    do {
		len += m->m_len;
	    } while (m = m->m_next);
	    nif.nif_bodylen = len;
	    snit_intr(ifp, m0, &nif);
	}
#endif
	protocol = PPP_IP;
	break;
#endif
#ifdef	NS
    case AF_NS :
	protocol = PPP_XNS;
	break;
#endif
    default :
	DLOG("ppp: af%d not supported\n", dst->sa_family);
	error = EAFNOSUPPORT;
	goto getout;
    }

    if (!p->pii_writeq) {
	error = EHOSTUNREACH;
	goto getout;
    }

#ifdef	VJC
    if ((protocol == PPP_IP) && (p->pii_flags & PII_FLAGS_VJC_ON)) {
	register struct ip *ip;
	ip = mtod(m0, struct ip *);
	if (ip->ip_p == IPPROTO_TCP) {
	    type = sl_compress_tcp(m0, ip, &p->pii_sc_comp,
				   !(p->pii_flags & PII_FLAGS_VJC_NOCCID));
	    switch (type) {
	    case TYPE_UNCOMPRESSED_TCP :
		protocol = PPP_VJC_UNCOMP;
		break;
	    case TYPE_COMPRESSED_TCP :
		protocol = PPP_VJC_COMP;
		break;	
	    }
	}
    }
#endif

    len = PPP_HDRLEN;
    for (m1 = m0; m1; m1 = m1->m_next) 
	len += m1->m_len;

    if (!(mp = allocb(len, BPRI_MED))) {
	error = ENOBUFS;
	goto getout;
    }

#ifdef	PPP_STATS
    p->pii_stats.ppp_obytes += len;
#endif

    *mp->b_wptr++ = PPP_ALLSTATIONS;
    *mp->b_wptr++ = PPP_UI;
    *mp->b_wptr++ = 0;
    *mp->b_wptr++ = protocol;
    for (m1 = m0; m1; m1 = m1->m_next) {	/* copy all data */
	bcopy(mtod(m1, char *), (char *) mp->b_wptr, m1->m_len);
	mp->b_wptr += m1->m_len;
    }

    s = splstr();
    putq(p->pii_writeq, mp);
    splx(s);

    p->pii_ifnet.if_opackets++;
    INCR(ppp_opackets);

 getout:
    m_freem(m0);
    if (error) {
	INCR(ppp_oerrors);
	p->pii_ifnet.if_oerrors++;
    }
    return (error);		/* gads, streams are great */
}

/*
 * if_ ioctl requests 
*/
ppp_ioctl(ifp, cmd, data)
    register struct ifnet *ifp;
    int	cmd;
    caddr_t	data;
{
    register struct ifaddr *ifa = (struct ifaddr *) data;
    register struct ifreq *ifr = (struct ifreq *) data;
    int	s = splimp(), error = 0;

    switch (cmd) {
    case SIOCSIFFLAGS :
	/* This happens every time IFF_PROMISC has been changed. */
	if (!ifr)
	    break;
	if (!suser()) {
	    error = EPERM;
	    break;
	}

	/* clear the flags that can be cleared */
	ifp->if_flags &= (IFF_CANTCHANGE);	
	/* or in the flags that can be changed */
	ifp->if_flags |= (ifr->ifr_flags & ~IFF_CANTCHANGE);
	break;

    case SIOCGIFFLAGS :
	ifr->ifr_flags = ifp->if_flags;
	break;

    case SIOCSIFADDR :
	if( ifa->ifa_addr.sa_family != AF_INET) 
	    error = EAFNOSUPPORT;
	break;

    case SIOCSIFDSTADDR :
	if (ifa->ifa_addr.sa_family != AF_INET)
	    error = EAFNOSUPPORT;
	break;

    case SIOCSIFMTU :
	if (!suser()) {
	    error = EPERM;
	    break;
	}
	if (ifr->ifr_mtu > MAX_PKTSIZE - PPP_FRAMING) {
	    error = EINVAL;
	    break;
	}
	ifp->if_mtu = ifr->ifr_mtu;
	break;

    case SIOCGIFMTU :
	ifr->ifr_mtu = ifp->if_mtu;
	break;

    default :
	error = EINVAL;
	break;
    }
    splx(s);
    return(error);
}

#endif
