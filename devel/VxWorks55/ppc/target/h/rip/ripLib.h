/* ripLib.h - primary include file for Routing Information Protocol (RIP) */

/* Copyright 1996 - 2003 Wind River Systems, Inc. */

/*-
 * Copyright (c) 1983, 1989, 1993
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
 *	@(#)routed.h	8.1 (Berkeley) 6/2/93
 */

/*
modification history
--------------------
01g,13jan03,rae  Merged from velocecp branch (SPR #83603)
01f,22mar02,niq  Merged from Synth view, tor3_x.synth branch, ver 01h
01e,12oct01,rae  merge from truestack
01d,11sep98,spm  updated contents to prevent compilation problems (SPR #22352);
                 removed references to bloated trace commands (SPR #22350)
01c,12mar97,gnn  added multicast support
                 added timer variables
01b,24feb97,gnn  Changed include protection.
                 Added prototype for ripLibInit.
                 Put in WRS proper typedefs.
01a,26nov96,gnn  created from BSD4.4 routed
*/


#ifndef __INCripLibh
#define __INCripLibh

#ifdef __cplusplus
extern "C" {
#endif

#include "sys/socket.h"
#include "net/route.h"
#include "net/if.h"
#include "rip/table.h"
#include "rip/interface.h"
#include "rip/rip2.h"

/*
 * Routing Information Protocol
 *
 * Derived from Xerox NS Routing Information Protocol
 * by changing 32-bit net numbers to sockaddr's and
 * padding stuff to 32-bit boundaries.
 */

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 1                 /* tell gcc960 not to optimize alignments */
#endif  /* CPU_FAMILY==I960 */

typedef struct route_info 
    {
    struct rt_msghdr	rtm;			/* Route message header */
    struct sockaddr 	addrs [RTAX_MAX];	/* Array of sockaddrs */
    } ROUTE_INFO;


typedef struct netinfo {
	struct	sockaddr rip_dst;	/* destination net/host */
	int	rip_metric;		/* cost of route */
} NETINFO;

typedef struct rip_pkt {
	u_char	rip_cmd;		/* request/response */
	u_char	rip_vers;		/* protocol version # */
	u_char	rip_domain[2];		/* Version 2 routing domain. */
	union {
		struct	netinfo ru_nets[1];	/* variable length... */
	} ripun;
#define	rip_nets	ripun.ru_nets
} RIP_PKT;

typedef struct rt_iflist
    {
    NODE	node;
    char 	rtif_name [IFNAMSIZ];	
    u_long      rtif_ipaddr;
    u_long      rtif_subnetmask;
    } RT_IFLIST;

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 0                 /* turn off alignment requirement */
#endif  /* CPU_FAMILY==I960 */
 
/*
 * Packet types.
 */
#define	RIPCMD_REQUEST		1	/* want info */
#define	RIPCMD_RESPONSE		2	/* responding to request */
#define	RIPCMD_TRACEON		3	/* obsolete: turn tracing on */
#define	RIPCMD_TRACEOFF		4	/* obsolete: turn tracing off */

#define	RIPCMD_MAX		5

#define	HOPCNT_INFINITY		16	/* per Xerox NS */
#define	MAXPACKETSIZE		512	/* max broadcast size */

/*
 * Flag values for the route hook routine
 */

#define RIP_REDIRECT_RECD	0x1	/* Received redirect message */
#define RIP_ROUTE_CHANGE_RECD	0x2	/* Received route change message */

/*
* Timer values used in managing the routing table. Complete tables are 
* broadcast at periodic intervals (30 seconds by default). If changes occur 
* between updates, triggered updates containing the changed entries may be 
* sent to speed convergence of the routing tables. The MIN_WAITTIME and 
* MAX_WAITTIME constants control the scheduling of these updates. No 
* triggered update will occur within MAX_WAITTIME of a regular update. 
* After the update is sent, further triggered updates will be supressed 
* for a random interval between MIN_WAITTIME and MAX_WAITTIME to avoid 
* flooding the network.
*/

#define	MIN_WAITTIME		1	/* min. interval to broadcast changes */
#define	MAX_WAITTIME		5	/* max. time to delay changes */

/* Forward declarations for user callable routines. */

STATUS ripLibInit (BOOL, BOOL, BOOL, int, int, int, int, int, int);
void ripRouteShow (void);
STATUS ripAuthHookAdd (char *, FUNCPTR);
IMPORT STATUS ripAuthHook (char *, RIP_PKT *);
STATUS ripAuthHookDelete (char *);
STATUS ripLeakHookAdd (char *, FUNCPTR);
STATUS ripLeakHookDelete (char *);
STATUS ripSendHookAdd (char *, BOOL (*sendHook) (struct rt_entry *));
STATUS ripSendHookDelete (char *);
void ripIfSearch (void);
STATUS ripIfReset (char *);
STATUS ripIfAddrReset (char *, char *, u_long);
void ripFilterEnable (void);
void ripFilterDisable (void);
STATUS ripShutdown (void);
void ripDebugLevelSet (int);
void ripAuthKeyShow (UINT);
STATUS ripAuthKeyAdd (char *, UINT16, char *, UINT, UINT, ULONG);
STATUS ripAuthKeyDelete (char *, UINT16);
STATUS ripAuthKeyFind (struct interface *, UINT16, RIP_AUTH_KEY **);
STATUS ripAuthKeyFindFirst (struct interface *, RIP_AUTH_KEY **);
STATUS ripAuthKeyInMD5 (struct interface *, RIP_PKT *, UINT);
STATUS ripAuthKeyOut1MD5 (struct interface *, struct netinfo *,
                          RIP2_AUTH_PKT_HDR **, RIP_AUTH_KEY **);
void ripAuthKeyOut2MD5 (RIP_PKT *, UINT *, struct netinfo *,
                        RIP2_AUTH_PKT_HDR *, RIP_AUTH_KEY *);
STATUS ripRouteHookAdd (FUNCPTR);
STATUS ripRouteHookDelete (void);
STATUS ripRouteHook (ROUTE_INFO *, int, BOOL, int);
void ripAddrsXtract (ROUTE_INFO *, struct sockaddr **, struct sockaddr **, 
		     struct sockaddr **, struct sockaddr **);
STATUS ripIfExcludeListAdd (char *);
STATUS ripIfExcludeListDelete (char *);
STATUS ripIfExcludeListCheck (char *, u_long, u_long);
void ripIfExcludeListShow (void);
STATUS ripIfAddrExcludeListAdd (char *, char *, u_long);
STATUS ripIfAddrExcludeListDelete (char *, u_long);

#ifdef __cplusplus
}
#endif

#endif /* __INCripLibh */
