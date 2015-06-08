/* route.h - routing table header file */

/* Copyright 1984 - 2003 Wind River Systems, Inc. */

/*
 * Copyright (c) 1980, 1986, 1993
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
 *	@(#)route.h	8.3 (Berkeley) 4/19/94
 */

/*
modification history
--------------------
01h,13jan03,rae  Merged from velocecp branch
01g,10oct01,rae  merge from truestack (ROUTER_STACK additions)
01f,07jul97,rjc  revised rt_metrics structure to gain 6 bytes.
01e,02jul97,vin  added declarations for rtxxMsgHooks removed unnessary global
		 declarations
01d,01jul97,rjc  restored old rtentry structure
01c,12feb97,rjc  ospf changes.
01b,05feb97,rjc  tos routing changes.
01a,03mar96,vin  created from BSD4.4 stuff,integrated with 02i.
*/

#ifndef __INCrouteh
#define __INCrouteh

#ifdef __cplusplus
extern "C" {
#endif

#include "net/socketvar.h"
#include "net/radix.h"
#include "net/mbuf.h"

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 1                 /* tell gcc960 not to optimize alignments */
#endif	/* CPU_FAMILY==I960 */

/*
 * Kernel resident routing tables.
 *
 * The routing tables are initialized when interface addresses
 * are set by making entries for all directly connected interfaces.
 */

/*
 * A route consists of a destination address and a reference
 * to a routing entry.  These are often held by protocols
 * in their control blocks, e.g. inpcb.
 */
struct route
    {
    struct	rtentry *ro_rt;
    struct	sockaddr ro_dst;
    };

/*
 * These numbers are used by reliable protocols for determining
 * retransmission behavior and are included in the routing structure.
 */
struct rt_metrics {
	u_long	rmx_locks;	/* Kernel must leave these values alone */
	u_long	rmx_mtu;	/* MTU for this path */
	u_long	rmx_expire;	/* lifetime for route, e.g. redirect */
	u_long	rmx_recvpipe;	/* inbound delay-bandwith product */
	u_long	rmx_sendpipe;	/* outbound delay-bandwith product */
	u_long	rmx_ssthresh;	/* outbound gateway buffer limit */
	u_long	rmx_rtt;	/* estimated round trip time */
	u_long	rmx_rttvar;	/* estimated rtt variance */
	u_short	rmx_hopcount;	/* max hops expected */
        u_short rmx_mss; 	/* TCP maximum segment size from peer */

        /* Additional WRN metrics for routing protocols */

        long 	value1;
        long 	value2;
        long 	value3;
        long 	value4;
        long 	value5;
        long 	routeTag;
        u_char  weight;
};

/*
 * This expanded "gateway" structure contains the address of the next-hop
 * router and all the new metrics for a route, as well as the protocol
 * identifier which created it. The RTM_GETALL routing socket message
 * contains a list of these structures.
 */

struct sockaddr_gate
    {
    u_char         gate_len;
    u_char         gate_family;
    u_char         routeProto;
    u_char         weight;
    u_long         gate_addr;
    long           value1;
    long           value2;
    long           value3;
    long           value4;
    long           value5;
    long           routeTag;
    };

/*
 * rmx_rtt and rmx_rttvar are stored as microseconds;
 * RTTTOPRHZ(rtt) converts to a value suitable for use
 * by a protocol slowtimo counter.
 */
#define	RTM_RTTUNIT	1000000	/* units for rtt, rttvar, as units per sec */
#define	RTTTOPRHZ(r)	((r) / (RTM_RTTUNIT / PR_SLOWHZ))

/*
 * We distinguish between routes to hosts and routes to networks,
 * preferring the former if available.  For each route we infer
 * the interface to use from the gateway address supplied when
 * the route was entered.  Routes that forward packets through
 * gateways are marked so that the output routines know to address the
 * gateway rather than the ultimate destination.
 */
#ifndef RNF_NORMAL
#include <net/radix.h>
#endif
struct rtentry {
	struct	radix_node rt_nodes[2];	/* tree glue, and other values */
#define	rt_key(r)	((struct sockaddr *)((r)->rt_nodes->rn_key))
#define	rt_mask(r)	((struct sockaddr *)((r)->rt_nodes->rn_mask))
	struct	sockaddr *rt_gateway;	/* value */
	short	rt_flags;		/* up/down?, host/net */
	short	rt_refcnt;		/* # held references */
	u_long	rt_use;			/* raw # packets forwarded */
	struct	ifnet *rt_ifp;		/* the answer: interface to use */
	struct	ifaddr *rt_ifa;		/* the answer: interface to use */
	struct	sockaddr *rt_genmask;	/* for generation of cloned routes */
	caddr_t	rt_llinfo;		/* pointer to link level info cache */
	struct	rtentry *rt_gwroute;	/* implied entry for gatewayed routes */
        int	rt_mod;			/* last modified XXX */ 
	struct	rt_metrics rt_rmx;	/* metrics used by rx'ing protocols */
        struct  rtentry *rt_parent;     /* parent of route (if cloned) */
};

/*
 * Following structure necessary for 4.3 compatibility;
 * We should eventually move it to a compat file.
 */
struct ortentry {
	u_long	rt_hash;		/* to speed lookups */
	struct	sockaddr rt_dst;	/* key */
	struct	sockaddr rt_gateway;	/* value */
	short	rt_flags;		/* up/down?, host/net */
	short	rt_refcnt;		/* # held references */
	u_long	rt_use;			/* raw # packets forwarded */
	struct	ifnet *rt_ifp;		/* the answer: interface to use */
        int	rt_mod;			/* last modified */
};

#define	RTF_UP		0x1		/* route usable */
#define	RTF_GATEWAY	0x2		/* destination is a gateway */
#define	RTF_HOST	0x4		/* host entry (net otherwise) */
#define	RTF_REJECT	0x8		/* host or net unreachable */
#define	RTF_DYNAMIC	0x10		/* created dynamically (by redirect) */
#define	RTF_MODIFIED	0x20		/* modified dynamically (by redirect) */
#define RTF_DONE	0x40		/* message confirmed */
#define RTF_MASK	0x80		/* subnet mask present */
#define RTF_CLONING	0x100		/* generate new routes on use */
#define RTF_XRESOLVE	0x200		/* external daemon resolves name */
#define RTF_LLINFO	0x400		/* generated by ARP or ESIS */
#define RTF_STATIC	0x800		/* manually added */
#define RTF_BLACKHOLE	0x1000		/* just discard pkts (during updates) */
#define RTF_PROTO2	0x4000		/* protocol specific routing flag */
#define RTF_PROTO1	0x8000		/* protocol specific routing flag */

#define RTF_MGMT	0x2000		/* modfied by managment proto */

#define RTF_REP_NODE    0x4000         /* Representative Node in the Pat tree */

#define RTF_CLONED RTF_PROTO1     /* Child route holding path MTU results */
#define RTF_DELETE RTF_PROTO2     /* Unused route scheduled for deletion */

/* Routing statistics. */

struct	rtstat
    {
    short	rts_badredirect;	/* bogus redirect calls */
    short	rts_dynamic;		/* routes created by redirects */
    short	rts_newgateway;		/* routes modified by redirects */
    short	rts_unreach;		/* lookups which failed */
    short	rts_wildcard;		/* lookups satisfied by a wildcard */
    };

/*
 * Structures for routing messages.
 */
struct rt_msghdr {
	u_short	rtm_msglen;	/* to skip over non-understood messages */
	u_char	rtm_version;	/* future binary compatibility */
	u_char	rtm_type;	/* message type */
	u_short	rtm_index;	/* index for associated ifp */
	int	rtm_flags;	/* flags, incl. kern & message, e.g. DONE */
	int	rtm_addrs;	/* bitmask identifying sockaddrs in msg */
	pid_t	rtm_pid;	/* identify sender */
	int	rtm_seq;	/* for sender to identify action */
	int	rtm_errno;	/* why failed */
	int	rtm_use;	/* from rtentry */
	u_long	rtm_inits;	/* which metrics we are initializing */
	struct	rt_metrics rtm_rmx; /* metrics themselves */
};

#define RTM_VERSION	3	/* Up the ante and ignore older versions */

#define RTM_ADD		0x1	/* Add Route */
#define RTM_DELETE	0x2	/* Delete Route */
#define RTM_CHANGE	0x3	/* Change Metrics or flags */
#define RTM_GET		0x4	/* Report Metrics */
#define RTM_LOSING	0x5	/* Kernel Suspects Partitioning */
#define RTM_REDIRECT	0x6	/* Told to use different route */
#define RTM_MISS	0x7	/* Lookup failed on this address */
#define RTM_LOCK	0x8	/* fix specified metrics */
#define RTM_OLDADD	0x9	/* caused by SIOCADDRT */
#define RTM_OLDDEL	0xa	/* caused by SIOCDELRT */
#define RTM_RESOLVE	0xb	/* req to resolve dst to LL addr */
#define RTM_NEWADDR	0xc	/* address being added to iface */
#define RTM_DELADDR	0xd	/* address being removed from iface */
#define RTM_IFINFO	0xe	/* iface going up/down etc. */

#ifdef ROUTER_STACK

/* New WRN messages */

#define RTM_ADDEXTRA    0xf     /* Report creation of duplicate route */
#define RTM_DELEXTRA    0x10    /* Report deletion of duplicate route */

#define RTM_NEWCHANGE   0x11    /* Change gateway of duplicate route */
#define RTM_NEWGET      0x12    /* Find any route with gateway */
#define RTM_GETALL 	0x13 	/* Get IP route to dst and any duplicates */

#define RTM_NEWIPROUTE 	0x14 	/* Replacement for deleted primary IP route */
#define RTM_OLDIPROUTE 	0x15 	/* Demoted IP route replaced with new entry */

#endif /* ROUTER_STACK */

#define RTV_MTU		0x1	/* init or lock _mtu */
#define RTV_HOPCOUNT	0x2	/* init or lock _hopcount */
#define RTV_EXPIRE	0x4	/* init or lock _expire */
#define RTV_RPIPE	0x8	/* init or lock _recvpipe */
#define RTV_SPIPE	0x10	/* init or lock _sendpipe */
#define RTV_SSTHRESH	0x20	/* init or lock _ssthresh */
#define RTV_RTT		0x40	/* init or lock _rtt */
#define RTV_RTTVAR	0x80	/* init or lock _rttvar */

#ifdef ROUTER_STACK

    /* New flag values for additional metrics */

#define RTV_VALUE1 	0x100	/* assign or lock first additional metric */
#define RTV_VALUE2  	0x200	/* assign or lock second additional metric */
#define RTV_VALUE3 	0x400 	/* assign or lock third additional metric */
#define RTV_VALUE4 	0x800 	/* assign or lock fourth additional metric */
#define RTV_VALUE5 	0x1000 	/* assign or lock fifth additional metric */
#define RTV_ROUTETAG 	0x2000 	/* assign or lock route tag value */
#define RTV_WEIGHT 	0x4000 	/* assign or lock administrative weight */

#endif /* ROUTER_STACK */

/*
 * Bitmask values for rtm_addr.
 */
#define RTA_DST		0x1	/* destination sockaddr present */
#define RTA_GATEWAY	0x2	/* gateway sockaddr present */
#define RTA_NETMASK	0x4	/* netmask sockaddr present */
#define RTA_GENMASK	0x8	/* cloning mask sockaddr present */
#define RTA_IFP		0x10	/* interface name sockaddr present */
#define RTA_IFA		0x20	/* interface addr sockaddr present */
#define RTA_AUTHOR	0x40	/* sockaddr for author of redirect */
#define RTA_BRD		0x80	/* for NEWADDR, broadcast or p-p dest addr */

/*
 * Index offsets for sockaddr array for alternate internal encoding.
 */
#define RTAX_DST	0	/* destination sockaddr present */
#define RTAX_GATEWAY	1	/* gateway sockaddr present */
#define RTAX_NETMASK	2	/* netmask sockaddr present */
#define RTAX_GENMASK	3	/* cloning mask sockaddr present */
#define RTAX_IFP	4	/* interface name sockaddr present */
#define RTAX_IFA	5	/* interface addr sockaddr present */
#define RTAX_AUTHOR	6	/* sockaddr for author of redirect */
#define RTAX_BRD	7	/* for NEWADDR, broadcast or p-p dest addr */
#define RTAX_MAX	8	/* size of array to allocate */

/* Macro to flush the IP forwarding cache. It must be called at splnet */

#define FORWARDING_CACHE_FLUSH() \
    { \
    if (ipforward_rt.ro_rt) \
        { \
        RTFREE (ipforward_rt.ro_rt); \
        ipforward_rt.ro_rt = 0; \
        } \
    }

struct rt_addrinfo {
	int	rti_addrs;
	struct	sockaddr *rti_info[RTAX_MAX];
};

struct route_cb {
	int	ip_count;
	int	ns_count;
	int	iso_count;
	int	any_count;
};

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 0                 /* turn off alignment requirement */
#endif	/* CPU_FAMILY==I960 */

#define	RTFREE(rt) \
	if ((rt)->rt_refcnt <= 1) \
		rtfree(rt); \
	else \
		(rt)->rt_refcnt--;

extern	struct	rtstat	rtstat;
extern	struct	radix_node_head *rt_tables[];
extern  int	rtmodified; 

#ifdef VIRTUAL_STACK
STATUS 	routeStorageCreate (void);
#else
int	 route_init (void);
#endif
void	 rt_maskedcopy (struct sockaddr *,
	    struct sockaddr *, struct sockaddr *);
int	 rt_setgate (struct rtentry *,
	    struct sockaddr *, struct sockaddr *);
void	 rtable_init (void **);
void	 rtalloc (struct route *);
struct rtentry *
	 rtalloc1 (struct sockaddr *, int, int);
void	 rtfree (struct rtentry *);
int	 rtinit (struct ifaddr *, int, int);
int	 rtioctl (int, caddr_t);
int	 rtredirect (struct sockaddr *, struct sockaddr *,
	    struct sockaddr *, int, struct sockaddr *, struct rtentry **);
int	 rtrequest (int, struct sockaddr *,
	    struct sockaddr *, struct sockaddr *, int, struct rtentry **);

#ifdef __cplusplus
}
#endif

#endif /* __INCrouteh */
