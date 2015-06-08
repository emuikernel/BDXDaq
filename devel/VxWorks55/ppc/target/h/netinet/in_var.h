/* in_var.h - internet interface variable header file */

/* Copyright 1984 - 2001 Wind River Systems, Inc. */

/*
 * Copyright (c) 1985, 1986, 1993
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
 *	@(#)in_var.h	8.2 (Berkeley) 1/9/95
 */

/*
modification history
--------------------
01d,04dec01,rae  cleanup
01c,10oct01,rae  merge from truestack (IGMPv2, VIRTUAL)
01b,05oct97,vin  changed in_ifaddr for fast multicasting.
01a,03mar96,vin  created from BSD4.4lite2.
*/

#ifndef __INCin_varh
#define __INCin_varh

#ifdef __cplusplus
extern "C" {
#endif

#include "net/if.h"
#include "netinet/in_pcb.h"

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 1                 /* tell gcc960 not to optimize alignments */
#endif	/* CPU_FAMILY==I960 */

/*
 * Interface address, Internet version.  One of these structures
 * is allocated for each interface with an Internet address.
 * The ifaddr structure contains the protocol-independent part
 * of the structure and is assumed to be first.
 */
struct in_ifaddr {
	struct	ifaddr ia_ifa;		/* protocol-independent info */
#define	ia_ifp		ia_ifa.ifa_ifp
#define ia_flags	ia_ifa.ifa_flags
					/* ia_{,sub}net{,mask} in host order */
	u_long	ia_net;			/* network number of interface */
	u_long	ia_netmask;		/* mask of net part */
	u_long	ia_subnet;		/* subnet number, including net */
	u_long	ia_subnetmask;		/* mask of subnet part */
	struct	in_addr ia_netbroadcast; /* to recognize net broadcasts */
	struct	in_ifaddr *ia_next;	/* next in list of internet addresses */
	struct	sockaddr_in ia_addr;	/* reserve space for interface name */
	struct	sockaddr_in ia_dstaddr; /* reserve space for broadcast addr */
#define	ia_broadaddr	ia_dstaddr
	struct	sockaddr_in ia_sockmask; /* reserve space for general netmask */
};

struct	in_aliasreq {
	char	ifra_name[IFNAMSIZ];		/* if name, e.g. "en0" */
	struct	sockaddr_in ifra_addr;
	struct	sockaddr_in ifra_broadaddr;
#define ifra_dstaddr ifra_broadaddr
	struct	sockaddr_in ifra_mask;
};
/*
 * Given a pointer to an in_ifaddr (ifaddr),
 * return a pointer to the addr as a sockaddr_in.
 */
#define	IA_SIN(ia) (&(((struct in_ifaddr *)(ia))->ia_addr))

#define IN_LNAOF(in, ifa) \
	((ntohl((in).s_addr) & ~((struct in_ifaddr *)(ifa)->ia_subnetmask))
			

#ifndef VIRTUAL_STACK
extern	struct	in_ifaddr *in_ifaddr;
#endif /* VIRTUAL_STACK */

extern	struct	ifqueue	ipintrq;		/* ip packet input queue */

extern u_char 	inetctlerrmap[];

extern void	in_socktrim (struct sockaddr_in *);


/*
 * Macro for finding the interface (ifnet structure) corresponding to one
 * of our IP addresses.
 */
#ifdef VIRTUAL_STACK

#define INADDR_TO_IFP(addr, ifp) \
	/* struct in_addr addr; */ \
	/* struct ifnet *ifp; */ \
{ \
	register struct in_ifaddr *ia; \
\
	for (ia = _in_ifaddr; \
	    ia != NULL && IA_SIN(ia)->sin_addr.s_addr != (addr).s_addr; \
	    ia = ia->ia_next) \
		 continue; \
	(ifp) = (ia == NULL) ? NULL : ia->ia_ifp; \
}

#else

#define INADDR_TO_IFP(addr, ifp) \
	/* struct in_addr addr; */ \
	/* struct ifnet *ifp; */ \
{ \
	register struct in_ifaddr *ia; \
\
	for (ia = in_ifaddr; \
	    ia != NULL && IA_SIN(ia)->sin_addr.s_addr != (addr).s_addr; \
	    ia = ia->ia_next) \
		 continue; \
	(ifp) = (ia == NULL) ? NULL : ia->ia_ifp; \
}

#endif /* VIRTUAL_STACK */

/*
 * Macro for finding the internet address structure (in_ifaddr) corresponding
 * to a given interface (ifnet structure).
 */
#ifdef VIRTUAL_STACK

#define IFP_TO_IA(ifp, ia) \
	/* struct ifnet *ifp; */ \
	/* struct in_ifaddr *ia; */ \
{ \
	for ((ia) = _in_ifaddr; \
	    (ia) != NULL && (ia)->ia_ifp != (ifp); \
	    (ia) = (ia)->ia_next) \
		continue; \
}

#else

#define IFP_TO_IA(ifp, ia) \
	/* struct ifnet *ifp; */ \
	/* struct in_ifaddr *ia; */ \
{ \
	for ((ia) = in_ifaddr; \
	    (ia) != NULL && (ia)->ia_ifp != (ifp); \
	    (ia) = (ia)->ia_next) \
		continue; \
}

#endif /* VIRTUAL_STACK */

LIST_HEAD(inMultiHead, in_multi); 


/*
 * This information should be part of the ifnet structure but we don't wish
 * to change that - as it might break a number of things
 */

struct router_info {
	struct ifnet *rti_ifp;
	int    rti_type; /* type of router which is querier on this interface */
	int    rti_time; /* # of slow timeouts since last old query */
	struct router_info *rti_next;
};

/*
 * Internet multicast address structure.  There is one of these for each IP
 * multicast group to which this host belongs on a given network interface.
 * They are kept in a linked list, rooted in the interface's in_ifaddr
 * structure. (now it is a "hash list")
 */


struct in_multi {
	LIST_ENTRY(in_multi) inm_hash;	/* hash list */
	struct	in_addr inm_addr;	/* IP multicast address */
	struct	ifnet *inm_ifp;		/* back pointer to ifnet */
	u_int	inm_refcount;		/* no. membership claims by sockets */
        u_int	inm_timer;		/* IGMP membership report timer */
        u_int	inm_state;		/*  state of the membership */
        struct	router_info *inm_rti;	/* router info*/
      	struct  mBlk * pInPcbMblk; 	/* chain of pcbs */ 
};

struct mcastHashInfo
    {
    struct inMultiHead *	hashBase;
    ULONG			hashMask;
    };

typedef struct in_multi		IN_MULTI;
typedef struct mcastHashInfo	M_CAST_HASH_INFO;
typedef struct inMultiHead	IN_MULTI_HEAD;
extern struct mcastHashInfo	mCastHashInfo;

/* necessary macros */
#define MCAST_HASH(mCastAddr, ifp, mask) \
	(((mCastAddr) ^ ((mCastAddr) >> 16) ^ (int)(ifp)) & (mask))

/*
 * Macro for looking up the in_multi record for a given IP multicast address
 * on a given interface.  If no matching record is found, "inm" returns NULL.
 */
#define IN_LOOKUP_MULTI(addr, ifp, inm)  				\
    {									\
    (inm) = mcastHashTblLookup ((addr).s_addr, (ifp));			\
    }

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 0			/* turn off alignment requirement */
#endif	/* CPU_FAMILY==I960 */

extern int	in_ifinit (struct ifnet *,
                           struct in_ifaddr *, struct sockaddr_in *, int);
extern struct mBlk * in_addmulti (struct in_addr *, struct ifnet *,
                                  struct inpcb *);
extern int	in_delmulti (struct mBlk *, struct inpcb *);
extern void	in_ifscrub (struct ifnet *, struct in_ifaddr *);
extern int	in_control (struct socket *, u_long, caddr_t, struct ifnet *);
IMPORT STATUS 	mcastHashTblInsert (struct in_multi * 	pInMulti);
IMPORT STATUS 	mcastHashTblDelete (struct in_multi * pInMulti);
IMPORT IN_MULTI * mcastHashTblLookup (int mcastAddr, struct ifnet * pIf);

#ifdef __cplusplus
}
#endif

#endif /* __INCin_varh */
