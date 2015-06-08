/* in_pcb.h - internet protocol control block header file */

/* Copyright 1984-1996 Wind River Systems, Inc. */

/*
 * Copyright (c) 1982, 1986, 1990, 1993
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
 *	@(#)in_pcb.h	8.1 (Berkeley) 6/10/93
 */

/*
modification history
--------------------
01d,10oct01,rae  merge from truestack, add multicast vif to ip_moptions
01c,05oct97,vin  moved ip_moptions field from ip_var.h
01b,31mar97,vin  added pcb hashing functions.
01a,03mar96,vin  created from BSD4.4 stuff,integrated with 02h of in_pcb.h
*/

#ifndef __INCin_pcbh
#define __INCin_pcbh

#ifdef __cplusplus
extern "C" {
#endif

#include "netinet/ip.h"
#include "net/unixLib.h"

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 1                 /* tell gcc960 not to optimize alignments */
#endif	/* CPU_FAMILY==I960 */

/*
 * Structure attached to inpcb.ip_moptions and
 * passed to ip_output when IP multicast options are in use.
 */
struct ip_moptions {
	struct	ifnet *imo_multicast_ifp; /* ifp for outgoing multicasts */
	u_char	imo_multicast_ttl;	/* TTL for outgoing multicasts */
	u_char	imo_multicast_loop;	/* 1 => hear sends if a member */
	u_short	imo_num_memberships;/* no. memberships this socket */
	struct  mBlk *	pInmMblk; 	/* chain of multicast addrs */
	u_long imo_multicast_vif;   /* the multicast vif to send on */
};

/*
 * Common structure pcb for internet protocol implementation.
 * Here are stored pointers to local and foreign host table
 * entries, local and foreign socket numbers, and pointers
 * up (to a socket structure) and down (to a protocol-specific)
 * control block.
 */

LIST_HEAD(inpcbhead, inpcb);

struct inpcb {
	LIST_ENTRY(inpcb) inp_list;	/* list for all PCBs of this proto */
	LIST_ENTRY(inpcb) inp_hash;	/* hash list */
	struct	inpcbinfo *inp_pcbinfo;
	struct	in_addr inp_faddr;	/* foreign host table entry */
	u_short	inp_fport;		/* foreign port */
	struct	in_addr inp_laddr;	/* local host table entry */
	u_short	inp_lport;		/* local port */
	struct	socket *inp_socket;	/* back pointer to socket */
	caddr_t	inp_ppcb;		/* pointer to per-protocol pcb */
	struct	route inp_route;	/* placeholder for routing entry */
	int	inp_flags;		/* generic IP/datagram flags */
	struct	ip inp_ip;		/* header prototype; should have more */
	struct	mbuf *inp_options;	/* IP options */
	struct	ip_moptions *inp_moptions; /* IP multicast options */
};

struct inpcbinfo {
	struct inpcbhead *listhead;
	struct inpcbhead *hashbase;
	unsigned long hashmask;
	unsigned short lastport;
};

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 0                 /* turn off alignment requirement */
#endif  /* CPU_FAMILY==I960 */

#define INP_PCBHASH(faddr, lport, fport, mask) \
	(((faddr) ^ ((faddr) >> 16) ^ (lport) ^ (fport)) & (mask))

/* flags in inp_flags: */
#define	INP_RECVOPTS		0x01	/* receive incoming IP options */
#define	INP_RECVRETOPTS		0x02	/* receive IP options for reply */
#define	INP_RECVDSTADDR		0x04	/* receive IP dst address */
#define	INP_CONTROLOPTS		(INP_RECVOPTS|INP_RECVRETOPTS|INP_RECVDSTADDR)
#define	INP_HDRINCL		0x08	/* user supplies entire IP header */

#define	INPLOOKUP_WILDCARD	1

#define	sotoinpcb(so)	((struct inpcb *)(so)->so_pcb)

extern void in_losing (struct inpcb *);
extern int in_pcballoc (struct socket *, struct inpcbinfo *);
extern int in_pcbbind (struct inpcb *, struct mbuf *);
extern int in_pcbconnect (struct inpcb *, struct mbuf *);
extern void in_pcbdetach (struct inpcb *);
extern void in_pcbdisconnect (struct inpcb *);
extern void in_pcbinshash (struct inpcb *);
extern int in_pcbladdr (struct inpcb *, struct mbuf *, struct sockaddr_in **);
extern struct inpcb * in_pcblookup (struct inpcbinfo *, struct in_addr, u_int,
                                    struct in_addr, u_int, int);
extern struct inpcb * in_pcblookuphash (struct inpcbinfo *, struct in_addr,
                                        u_int, struct in_addr, u_int, int);
extern void in_pcbnotify (struct inpcbhead *, struct sockaddr *, u_int,
                          struct in_addr, u_int, int,
                          void (*)(struct inpcb *, int));
extern void in_pcbrehash (struct inpcb *);
extern void in_rtchange (struct inpcb *, int);
extern void in_setpeeraddr (struct inpcb *, struct mbuf *);
extern void in_setsockaddr (struct inpcb *, struct mbuf *);

#ifdef __cplusplus
}
#endif

#endif /* __INCin_pcbh */
