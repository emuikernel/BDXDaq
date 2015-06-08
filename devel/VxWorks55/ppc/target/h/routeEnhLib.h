/* routeEnhLib.h - definitions for the route interface library */

/* Copyright 1984 - 2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01u,24jan02,niq  fixed call to new address message hook (SPR #71670)
01t,10sep01,niq  Make RIP work on host stack - SPR 70188
01s,03aug01,spm  added flag to prevent incorrect interface reference count
01r,31jul01,spm  fixed return type of callback handler to remove warning
01q,27jul01,spm  removed unused handle from add and delete routines
01p,27jul01,spm  updated registration process (WRN 1012 design, sec. 2.2.3)
01o,25jul01,spm  added routine to reorganize any duplicate routes after
                 weight change with routing sockets 
01n,25jul01,spm  added new messages to report replacement of the primary route
01m,23jul01,niq  Add the prototype for routeTableWalk and removed the 
                 Fastpath kludge.
01l,22jun01,spm  moved additional metric values to provide access with
                 routing sockets; added renamed route classification flag
                 to descriptor structure for callback routines
01m,25jun01,spm  adding new socket messages.
01l,25jun01,spm  added include file for RT_PROTO_SET/GET macro definitions
01k,19jun01,spm  added routing socket messages (WRN 1012 design, sec. 2.2.7.1);
                 removed unused registration handle
01j,19jun01,spm  cleanup: removed excess typecast from macro
01i,12jun01,niq  Add the routeAgeSet routine prototype
01h,09jun01,spm  added route weights (WRN 1012 design, section 2.3.6)
01g,10may01,spm  added new route lookup routine
01f,03may01,niq  Remove ROUTE_MOD stuff. Use ROUTE_CHANGE instead
01e,02may01,niq  Correcting mod history
01d,01may01,niq  Adding definition of ROUTE_MOD needed by Fastpath
01c,01may01,niq  Merging Fastpath changes from tor2_0.open_stack branch
                 version 01h (routeEnhLib.h@@/main/tor2_0.open_stack/7)
01b,01may01,spm  added routing socket messages for unified interface
01a,29mar01,spm  file creation: copied from version 01f of tor2_0.open_stack
                 branch (wpwr VOB) for unified code base
*/

/*
DESCRIPTION
This file includes function prototypes for the routing interface, as well as the
route entry structures.
*/

#ifndef _INCrouteEnhLibh
#define _INCrouteEnhLibh

#ifdef __cplusplus
extern "C" {
#endif

#include "net/socketvar.h"
#include "net/radix.h"
#include "net/mbuf.h"
#include "sys/socket.h"    /* needed for route.h */
#include "net/route.h"
#include "netinet/in.h"    /* RT_PROTO_SET and RT_PROTO_GET */
#include "m2Lib.h"

#define MAX_ROUTE_PROTO 20

/* routeTag values  understood by routing protocols */
#define RFC1745_DEFAULT_ROUTE_TAG 0x0000 /* manual/default route */
#define RFC1745_CONF_EXTERNAL_ROUTE_TAG 0x8000 /* configured external routes
						* IP static to OSPF and BGP 
						*/
#define RFC1745_EGP_ORIGEGP_ROUTE_TAG 0x9000 /* BGP routes reached via OSPF */
#define RFC1745_STUB_ASBR_BGP_ROUTE_TAG 0xa000 /* BGP routes at ASBR to STUB AS */
#define RFC1745_IGP_TO_OSPF_ROUTE_TAG 0xc000   /* IGP dynamic routes */
#define RFC1745_EGP_ORIGIGP_ROUTE_TAG_PER_AS 0xd000 /* BGP routes with orig IGP,
						      * next hop AS
						      */
#define RFC1745_BGP_TO_OSPF_ROUTE_TAG 0xe000 /* BGP origin */

/* Operations performed */
#define ROUTE_ADD 1
#define ROUTE_DEL 2
#define ROUTE_CHANGE 3
#define ROUTE_IF_UP 4
#define ROUTE_IF_DOWN 5
#define ROUTE_IF_NEWADDR 6
#define ROUTE_IF_DELADDR 7
#define ROUTE_CACHE_RESET 8
#define ROUTE_MISS 9    /* internal operation only: not published message */
#define ROUTE_CLONE_FAIL 10
#define ROUTE_SEARCH_FAIL 11
#define ROUTE_GATEWAY_CHANGE 12
#define ROUTE_OLD 13
#define ROUTE_NEW 14

/* special case, when route is not deleted during routeEntryDel() */
#define ROUTE_NOT_DELETED 0

/* 
 * Kernel routing table entry
 *
 * The routeEntry structure is an extension to the rtentry structure defined in
 * route.h. The extension is to support some addtional members defined in RFC 1213
 * and multiple route entries by the same/different routing protocols.
 *
 * Since routeEntry is an extension of rtentry and currently rtrequest() is the only 
 * place where rtentry structure is allocated, the modification for allocating 
 * routeEntry instead of rtentry is binary backward backward compatible.
 *
 */


typedef struct routeNode 
    {
    struct routeEntry* pFrwd;    /* forward pointer : for list traversal */
    struct routeEntry* pBack;    /* backward pointer : for list traversal */
    } ROUTE_NODE;


typedef struct routeEntry 
    {
    struct rtentry rtEntry;     /* defined in net/route.h */
    BOOL primaryRouteFlag;      /* IP forwarding route or alternate? */
    ROUTE_NODE  sameNode;       /*  reference to multiple route entries 
			         *  for the same destination added by 
			         *  the same routing protocol. 
			         */
    ROUTE_NODE  diffNode;       /*  reference to multiple route entries
			         *  for the same destination added by
			         *  different routing protocols.
			         */
    } ROUTE_ENTRY;

/* Macros to access the fields of ROUTE_ENTRY */
 
#define ROUTE_ENTRY_KEY(r) ((struct sockaddr *)(((r)->rtEntry) \
       .rt_nodes[0].rn_u.rn_leaf.rn_Key))
 
#define ROUTE_ENTRY_MASK(r) ((struct sockaddr *)(((r)->rtEntry) \
       .rt_nodes[0].rn_u.rn_leaf.rn_Mask))
 
 
#define ROUTE_ENTRY_GATEWAY(r) ((r)->rtEntry.rt_gateway)
#define ROUTE_ENTRY_FLAGS(r) ((r)->rtEntry.rt_flags)


/* Route description structure
 *
 * This structure contains all the route information that needs to be stored and
 * and retrived from the routing table. This structure is provided by the 
 * routing protocol, while adding (routeEntryADd()) or 
 * searching (routeEntryLookup()) a route.

 * When the lookup routine is called, searching for a specific route, the
 * looked-up routing information is copied on to the following structure.
 * The lookup routine is usually called by the routing protocol to figure out the
 * interface to be used for a particular gateway.
 * IP forwarding uses the internal BSD API and has direct access to ROUTE_ENTRY
 * structure.
 * 
 */

typedef struct routeDesc
    {
    struct sockaddr* pDstAddr; /* destination network/host address */
    struct sockaddr* pNetmask; /* network mask */
    struct sockaddr* pGateway; /* gateway address */
    short flags;               /* up/down, host/net? defined in /h/net/route.h */
    short protoId;             /* routing protocol ID: m2Lib.h */
    struct ifnet* pIf;         /* interface to be used with this route, set to 
                                * NULL for routeEntryAdd().
                                */
    long value1;               /* metrics as defined by RFC 1213 */
    long value2;
    long value3;
    long value4;
    long value5;
    long routeTag;             /* property associated with the route, values are
				* described above 
				*/ 
    u_char weight;

    /* Read-only settings: valid in received messages */

    BOOL primaryRouteFlag;     /* IP forwarding route or alternate? */
    void *pData;               /* additional message data, based on type */
    } ROUTE_DESC;

/* routing system registration structure: see routeStorageBind() routine */

typedef struct routeBind
    {
    NODE    node;     /* internal connection for linked list */
    void (*pRouteMsgRtn) (int type, ROUTE_DESC *pDesc, void *pArg);
    void *  pArg;
    } ROUTE_BINDING;


/* Routing Interface APIs */

/* Initialization */
IMPORT STATUS routeIntInit();

/* Modification APIs */
IMPORT STATUS routeEntryAdd (ROUTE_DESC * pRouteDesc);
IMPORT STATUS routeEntryDel (ROUTE_DESC* pRouteDesc);
IMPORT STATUS routeAgeSet (struct sockaddr *, struct sockaddr *, int, int);
IMPORT STATUS routeMetricSet (struct sockaddr *, struct sockaddr *, int, int);

/* Route lookup */
IMPORT STATUS routeEntryLookup (struct sockaddr *, ULONG *, int,
				ROUTE_DESC *);
IMPORT STATUS routeTableWalk (struct sockaddr *, int, VOIDFUNCPTR, void *);

/* Registration routines */
IMPORT void * routeStorageBind (void (*)(int, ROUTE_DESC *, void *), void *);

IMPORT STATUS routeStorageUnbind (void *); 

/* externs */
IMPORT STATUS rtrequestAddEqui (struct sockaddr *, struct sockaddr *,
                                struct sockaddr *, short, short, UCHAR,
                                BOOL, BOOL, ROUTE_ENTRY **);
IMPORT STATUS rtrequestDelEqui (struct sockaddr *, struct sockaddr *,
                                struct sockaddr *, short, short,
                                BOOL, BOOL, ROUTE_ENTRY **);
IMPORT void routeWeightUpdate (ROUTE_ENTRY *, short, ROUTE_ENTRY *,
                               struct sockaddr *, UCHAR);
IMPORT void routeEntryFree (ROUTE_ENTRY *, BOOL);
IMPORT STATUS routeProtoShow();

/* Interfaces for internal stack events */

extern void (*rtIfaceMsgHook) (struct ifnet *);
extern void (*rtMissMsgHook) (int, struct rt_addrinfo *, int, int); 
#ifdef ROUTER_STACK
extern void (*rtNewAddrMsgHook) (int, struct ifaddr *, struct rtentry *);
#else
extern void (*rtNewAddrMsgHook) (int, struct ifaddr *, int, struct rtentry *);
#endif /* ROUTER_STACK */

#ifdef _cplusplus
}
#endif

#endif /* _INCrouteEnhLibh */
