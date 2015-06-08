/* dhcpsLib.h - DHCP server include file for user interface */

/* Copyright 1984 - 2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,10oct01,rae  merge from truestack (virtual stack support)
01a,25jun01,ppp  fixed the problem involving the dhcp server and client to be
                 built together (SPR #64194)
01f,14jul00,spm  upgraded to RFC 2131 and removed direct link-level access
01e,04dec97,spm  added code review modifications
01d,06oct97,spm  moved default parameters from usrNetwork.c; added prototypes
                 for user-callable routines
01c,06aug97,spm  added definitions for C++ compilation
01b,18apr97,spm  added dhcpsInit() prototype to eliminate compiler warnings
01a,07apr97,spm  created.
*/

#ifndef __INCdhcpsLibh
#define __INCdhcpsLibh

#ifdef __cplusplus
extern "C" {
#endif

#define S_dhcpsLib_NOT_INITIALIZED           (M_dhcpsLib | 1)

#include "ioLib.h"

#define PROTO_ICMP 1                   /* Normally found in /etc/protocols */

#if CPU_FAMILY==I960
#pragma align 1                 /* tell gcc960 not to optimize alignments */
#endif  /* CPU_FAMILY==I960 */

typedef struct leaseDesc
    {
    /* 
     * This structure defines the components of an address pool entry. 
     * The DHCP server issues leases for individual IP addresses from
     * the address pool.
     */

    char *pName; 	/* base name of pool entry */
    char *pStartIp; 	/* first IP address in range */
    char *pEndIp; 	/* last IP address in range */
    char *pParams; 	/* other lease parameters */
    } DHCPS_LEASE_DESC;

typedef struct entryDesc
    {
    /*
     * This structure defines the components of a single lease entry
     * from the address pool which will be offered to DHCP clients.
     */

    char *pName; 	/* unique name of lease entry */
    char *pAddress; 	/* single IP address within range */
    char *pParams; 	/* associated lease parameters */
    } DHCPS_ENTRY_DESC;

#include "ioLib.h"

typedef struct ifDesc
    {
    char ifName[MAX_FILENAME_LENGTH];	/* name of interface to use */
    } DHCPS_IF_DESC;

typedef struct relayDesc
    {
    /*
     * This structure contains information for a relay agent used to 
     * communicate with DHCP clients or servers on other networks.
     */

    char *pAddress; 	/* IP address of relay agent */
    char *pMask; 	/* Subnet mask of supported network */
    } DHCPS_RELAY_DESC;

#include "dhcprLib.h"                /* Relay agent capability. */

#if CPU_FAMILY==I960
#pragma align 0                 /* turn off alignment requirement */
#endif  /* CPU_FAMILY==I960 */

IMPORT DHCPS_LEASE_DESC *      pDhcpsLeasePool;
IMPORT DHCPS_RELAY_DESC *      pDhcpsRelaySourceTbl;

typedef struct dhpcsConfigDesc
    {
    int dhcpMaxHops;			/* max hops */
    int dhcpSPort;			/* server port */
    int dhcpCPort;			/* client port */
    int dhcpMaxMsgSize;			/* max msg size */
    long dhcpsDfltLease;		/* Default for default lease length */
    long dhcpsMaxLease;			/* Default maximum lease length */
    FUNCPTR pDhcpsLeaseFunc;		/* ptr to lease function */
    FUNCPTR pDhcpsAddrFunc;		/* ptr to address function */
    DHCPS_IF_DESC *pDhcpsIfTbl;		/* ptr to device table */
    int numDev;				/* # of devices in table */
    DHCPS_LEASE_DESC *pDhcpsLeaseTbl;	/* lease tbl */
    int dhcpsLeaseTblSize;		/* size of lease table */
    DHCP_TARGET_DESC *pDhcpsTargetTbl;	/* target table */
    int dhcpTargetTblSize;		/* size of target table */
    DHCPS_RELAY_DESC *pDhcpsRelayTbl;	/* relay table */
    int dhcpsRelayTblSize;		/* size of relay table */
    } DHCPS_CFG_PARAMS;

/* User-callable routines */

IMPORT STATUS dhcpsInit (DHCPS_CFG_PARAMS *);
IMPORT STATUS dhcpsLeaseEntryAdd (char *, char *, char *, char *);
IMPORT STATUS dhcpsLeaseHookAdd (FUNCPTR);
IMPORT STATUS dhcpsAddressHookAdd (FUNCPTR);
IMPORT STATUS dhcpsVsInit ();
/* Critical section semaphore. */

IMPORT SEM_ID dhcpsMutexSem;

/* Operations on database cache. */

#define DHCPS_STORAGE_START 0
#define DHCPS_STORAGE_STOP 1
#define DHCPS_STORAGE_WRITE 2
#define DHCPS_STORAGE_READ 3
#define DHCPS_STORAGE_CLEAR 4

/*
 * The following string contains host requirements defaults needed for
 * RFC compliance - DO NOT REMOVE OR MODIFY!!
 */
 
#define DHCPS_DEFAULT_ENTRY "ipfd=false:nlsr=false:mdgs=576:ditl=64:ifmt=576:asnl=false:brda=255.255.255.255:mskd=false:msks=false:rtrd=true:rtsl=255.255.255.255:trlr=false:arpt=60:encp=false:dttl=64:kain=7200:kagb=false"

#define MAX_DHCPFILTERS 23 /* Number of filters as defined below */

#define DHCPS_FILTER_DEFINE { \
BPF_STMT(BPF_LD+BPF_TYPE,0),                /* Save lltype in accumulator */ \
BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, ETHERTYPE_IP, 0, 20),  /* IP packet? */ \
/* \
 * The remaining statements use the (new) BPF_HLEN alias to avoid any \
 * link-layer dependencies. The expected length values are assigned to the \
 * correct values during startup. The expected destination port is also \
 * altered to match the actual value chosen. \
 */ \
BPF_STMT(BPF_LD+BPF_H+BPF_ABS+BPF_HLEN, 6),     /* A <- IP FRAGMENT field */ \
BPF_JUMP(BPF_JMP+BPF_JSET+BPF_K, 0x1fff, 18, 0),          /* OFFSET == 0 ? */ \
BPF_STMT(BPF_LDX+BPF_HLEN, 0),           /* X <- frame data offset */ \
BPF_STMT(BPF_LD+BPF_H+BPF_IND, 2),       /* A <- IP_LEN field */ \
BPF_JUMP(BPF_JMP+BPF_JGE+BPF_K, 0, 0, 15),      /* IP/UDP headers + DHCP? */ \
BPF_STMT(BPF_LD+BPF_B+BPF_IND, 9),      /* A <- IP_PROTO field */ \
BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, IPPROTO_UDP, 0, 13),     /* UDP ? */ \
BPF_STMT(BPF_LD+BPF_HLEN, 0),           /* A <- frame data offset */ \
BPF_STMT(BPF_LDX+BPF_B+BPF_MSH+BPF_HLEN, 0), /* X <- IPHDR LEN field */ \
BPF_STMT(BPF_ALU+BPF_ADD+BPF_X, 0),     /* A <- start of UDP datagram */ \
BPF_STMT(BPF_MISC+BPF_TAX, 0),          /* X <- start of UDP datagram */ \
BPF_STMT(BPF_LD+BPF_H+BPF_IND, 2),      /* A <- UDP DSTPORT */ \
BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, 67, 0, 7), /* check DSTPORT */ \
BPF_STMT(BPF_LD+BPF_H+BPF_IND, 4),      /* A <- UDP LENGTH */ \
BPF_JUMP(BPF_JMP+BPF_JGE+BPF_K, 0, 0, 5), /* UDP header + DHCP? */ \
BPF_STMT(BPF_LD+BPF_B+BPF_IND, 11),      /* A <- DHCP hops field */ \
BPF_JUMP(BPF_JMP+BPF_JGT+BPF_K, -1, 3, 0),   /* -1 replaced with max hops */ \
BPF_STMT(BPF_LD+BPF_W+BPF_IND, 244),    /* A <- DHCP options */ \
BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, 0x63825363, 0, 1), \
BPF_STMT(BPF_RET+BPF_K+BPF_HLEN, DFLTDHCPLEN + UDPHL + IPHL), \
BPF_STMT(BPF_RET+BPF_K, 0)          /* unrecognized message: ignore frame */ }

#ifdef __cplusplus
}
#endif

#endif
