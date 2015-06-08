/* ip_var.h - internet protocol protocol variable header file */

/* Copyright 1984 - 2002 Wind River Systems, Inc. */

/*
 * Copyright (c) 1982, 1986, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)ip_var.h	8.2 (Berkeley) 1/9/95
 */

/*
modification history
--------------------
01h,15apr02,wap  reinstate netJobAdd() removal changes
01g,29mar02,wap  back out the previous change (needs more investigation)
01f,22mar02,rae  ipintr mod helps remove extra netJobAdd (SPR #74604)
01e,10oct01,rae  merge from truestack (ip_mroute update, VIRTUAL)
01d,05oct97,vin  moved ip_moptions to in_pcb.h for multicast hashing changes.
01c,31jan97,vin  changed declaration according to prototype decl in protosw.h
01b,22nov96,vin  modified ipq for new ipReAssembly
01a,03mar96,vin  created from BSD4.4lite2. corrected bit fields according to
		 ansi specfication
*/

#ifndef __INCip_varh
#define __INCip_varh

#ifdef __cplusplus
extern "C" {
#endif

#include "net/if.h"
#include "netinet/in.h"
#include "netinet/in_pcb.h"

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 1                 /* tell gcc960 not to optimize alignments */
#endif	/* CPU_FAMILY==I960 */

/*
 * Overlay for ip header used by other protocols (tcp, udp).
 */
struct ipovly {
	caddr_t	ih_next, ih_prev;	/* for protocol sequence q's */
	u_char	ih_x1;			/* (unused) */
	u_char	ih_pr;			/* protocol */
	short	ih_len;			/* protocol length */
	struct	in_addr ih_src;		/* source internet address */
	struct	in_addr ih_dst;		/* destination internet address */
};

/*
 * Ip reassembly queue structure.  Each fragment
 * being reassembled is attached to one of these structures.
 * They are timed out after ipq_ttl drops to 0, and may also
 * be reclaimed if memory becomes tight.
 */
struct ipq {
	struct	ipq *next,*prev;	/* to other reass headers */
	u_char	ipq_ttl;		/* time for reass q to live */
	u_char	ipq_p;			/* protocol of this fragment */
	u_short	ipq_id;			/* sequence id for reassembly */
    	struct  mbuf * pMbufHdr; 	/* back pointer to mbuf */
    	struct  mbuf * pMbufPkt;	/* pointer to mbuf packet */
					/* to ip headers of fragments */
	struct	in_addr ipq_src,ipq_dst;
};

/*
 * Ip header, when holding a fragment.
 *
 * Note: ipf_next must be at same offset as ipq_next above
 */
struct	ipasfrag {
#if _BYTE_ORDER == _LITTLE_ENDIAN
	u_int	ip_hl:4,
		ip_v:4,
#endif
#if _BYTE_ORDER == _BIG_ENDIAN
	u_int	ip_v:4,
		ip_hl:4,
#endif
		ipf_mff:8,		/* XXX overlays ip_tos: use low bit
					 * to avoid destroying tos;
					 * copied from (ip_off&IP_MF) */
		ip_len:16;
	u_short	ip_id;
	short	ip_off;
	u_char	ip_ttl;
	u_char	ip_p;
	u_short	ip_sum;
	struct	in_addr ip_src,ip_dst;	/* source and dest address */
};

/*
 * Structure stored in mbuf in inpcb.ip_options
 * and passed to ip_output when ip options are in use.
 * The actual length of the options (including ipopt_dst)
 * is in m_len.
 */
#define MAX_IPOPTLEN	40

struct ipoption {
	struct	in_addr ipopt_dst;	/* first-hop dst if source routed */
	char	ipopt_list[MAX_IPOPTLEN];	/* options proper */
};

struct	ipstat {
	u_long	ips_total;		/* total packets received */
	u_long	ips_badsum;		/* checksum bad */
	u_long	ips_tooshort;		/* packet too short */
	u_long	ips_toosmall;		/* not enough data */
	u_long	ips_badhlen;		/* ip header length < data size */
	u_long	ips_badlen;		/* ip length < ip header length */
	u_long	ips_fragments;		/* fragments received */
	u_long	ips_fragdropped;	/* frags dropped (dups, out of space) */
	u_long	ips_fragtimeout;	/* fragments timed out */
	u_long	ips_forward;		/* packets forwarded */
	u_long	ips_cantforward;	/* packets rcvd for unreachable dest */
	u_long	ips_redirectsent;	/* packets forwarded on same net */
	u_long	ips_noproto;		/* unknown or unsupported protocol */
	u_long	ips_delivered;		/* datagrams delivered to upper level*/
	u_long	ips_localout;		/* total ip packets generated here */
	u_long	ips_odropped;		/* lost packets due to nobufs, etc. */
	u_long	ips_reassembled;	/* total packets reassembled ok */
	u_long	ips_fragmented;		/* datagrams sucessfully fragmented */
	u_long	ips_ofragments;		/* output fragments created */
	u_long	ips_cantfrag;		/* don't fragment flag was set, etc. */
	u_long	ips_badoptions;		/* error in option processing */
	u_long	ips_noroute;		/* packets discarded due to no route */
	u_long	ips_badvers;		/* ip version != 4 */
	u_long	ips_rawout;		/* total raw ip packets generated */
};

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 0                 /* turn off alignment requirement */
#endif	/* CPU_FAMILY==I960 */

/* flags passed to ip_output as last parameter */
#define	IP_FORWARDING		0x1		/* most of ip header exists */
#define	IP_RAWOUTPUT		0x2		/* raw ip header exists */
#define	IP_ROUTETOIF		SO_DONTROUTE	/* bypass routing tables */
#define	IP_ALLOWBROADCAST	SO_BROADCAST	/* can send broadcast packets */

#define ipTimeToLive	ip_defttl	/* MIB-II Read Write Variable */

#ifndef VIRTUAL_STACK
extern struct ipstat	ipstat;
extern struct	ipq	ipq;		/* ip reass. queue */
extern u_short		ip_id;		/* ip packet ctr, for ids */
extern int		ip_defttl;	/* default IP ttl */
#endif /* VIRTUAL_STACK */

extern int	 ip_ctloutput (int, struct socket *, int, int, struct mbuf **);
extern void	 ip_deq (struct ipasfrag *);
extern int	 ip_dooptions (struct mbuf *);
extern void	 ip_drain (void);
extern void	 ip_enq (struct ipasfrag *, struct ipasfrag *);
extern void	 ip_forward (struct mbuf *, int);
extern void	 ip_freef (struct ipq *);
extern void	 ip_freemoptions (struct ip_moptions *, struct inpcb *);
extern int	 ip_getmoptions (int, struct ip_moptions *, struct mbuf **);
extern void	 ip_init (void);
extern int	 ip_mforward (struct mbuf *, struct ifnet *, struct ip *, struct ip_moptions*);
extern int	 ip_optcopy (struct ip *, struct ip *);
extern int	 ip_output (struct mbuf *, struct mbuf *, struct route *, 
			    int, struct ip_moptions *);
extern int	 ip_pcbopts (struct mbuf **, struct mbuf *);
extern struct in_ifaddr * ip_rtaddr (struct in_addr);
extern void	 ip_slowtimo (void);
extern struct mbuf * ip_srcroute (void);
extern void	 ip_stripoptions (struct mbuf *, struct mbuf *);
extern int	 ip_sysctl (int *, u_int, void *, size_t *, void *, size_t);
extern void	 ipintr (struct mbuf *m);
extern int	 rip_ctloutput (int, struct socket *, int, int, 
				struct mbuf **);
extern void	 rip_init (void);
extern void	 rip_input (struct mbuf *);
extern int	 rip_output (struct mbuf *, struct socket *, u_long);
extern int	 rip_usrreq (struct socket *, int, struct mbuf *, 
			     struct mbuf *, struct mbuf *);

#ifdef __cplusplus
}
#endif

#endif /* __INCip_varh */
