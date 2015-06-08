/* m2LibP.h - VxWorks MIB-II interface to SNMP Agent */

/* Copyright 1984 - 2002 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01d,25apr02,vvv  removed unused ifAttachChange (SPR #74391)
01c,10oct01,rae  merge from truestack (VIRTUAL)
01b,25oct00,niq  Merge RFC2233 changes from open_stack-p1-r1 branch
01a,08dec93,jag  written
*/

#ifndef __INCm2LibPh
#define __INCm2LibPh

#ifdef __cplusplus
extern "C" {
#endif

#include "socket.h"
#include "net/if_arp.h"
#include "net/if.h"
#include "netinet/if_ether.h"
#include "avlLib.h"

#ifdef VIRTUAL_STACK
#include "netinet/vsLib.h"
#endif    /* VIRTUAL_STACK */

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 1                 /* tell gcc960 not to optimize alignments */
#endif  /* CPU_FAMILY==I960 */

/* This structure is used to keep track of the  Network Interfaces */

typedef struct
    {
    int              netIfIndex;      /* I/F index returned to SNMP */
    long             netIfType;       /* I/F type from MIB-II specifications */
    unsigned long    netIfSpeed;      /* I/F Speed from MIB-II specifications */
    unsigned long    netIfAdminStatus;  /* I/F old status used for traps */
    unsigned long    netIfLastChange;   /* I/F Time of Interface last change */
    M2_OBJECTID      netIfOid;          /* I/F Object ID */
    struct arpcom  * pNetIfDrv;         /* Pointer to BSD Network Driver I/F */
 
    } NET_IF_TBL;

/*
 *  These IOCTL commands are used exclusively by the mib2Lib.c module to retrive
 *  MIB-II parameters from network drivers.
 */
 
#define SIOCGMIB2CNFG   _IOR('m',  0, int)            /* Get Configuration */
#define SIOCGMIB2CNTRS  _IOR('m',  1, int)            /* Get driver counters */
#define SIOCGMIB2233    _IOR('m',  2, M2_ID **)       /* Get rfc2233 data */
#define SIOCSMIB2233    _IOW('m',  3, IF_SETENTRY)    /* Set rfc2233 data */
#define SIOCGMCASTLIST  _IOR('m',  4, LIST **)        /* Get mcast list */

/*
 * This structure is used to obtain the configuration information from the
 * network drivers.  This information is static and does not change through the
 * life time of the driver. 
 */
 
typedef struct
    {
    long            ifType;
    M2_OBJECTID     ifSpecific;
 
    } M2_NETDRVCNFG;
 
/*
 * This structure is used to retrive counters from the network driver.  The
 * variable ifSpeed is included here to support devices that can compute the
 * nominal bandwidth.
 */
 
typedef struct
    {
    unsigned long   ifSpeed;
    unsigned long   ifInOctets;
    unsigned long   ifInNUcastPkts;
    unsigned long   ifInDiscards;
    unsigned long   ifInUnknownProtos;
    unsigned long   ifOutOctets;
    unsigned long   ifOutNUcastPkts;
    unsigned long   ifOutDiscards;
 
    } M2_NETDRVCNTRS;

  
 
#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 0                 /* turn off alignment requirement */
#endif  /* CPU_FAMILY==I960 */
 
/* extern NET_IF_TBL * pm2IfTable;  Network I/F table (Allocated Dynamically) */

#ifndef VIRTUAL_STACK

extern int 	     m2IfCount;
extern AVL_TREE     pM2IfRoot;    /* The root node of the if tree */
extern M2_IFINDEX * pm2IfTable;  /* Network I/F table (Allocated Dynamically) */
extern int          m2IfCount; 

#endif    /* VIRTUAL_STACK */

extern M2_OBJECTID zeroObjectId;

#if defined(__STDC__) || defined(__cplusplus)

extern int m2NetIfTableRead (void);
extern int nextIndex (void *, GENERIC_ARGUMENT);
extern void ifWalk (int);
extern void testTree (void);

#else   /* __STDC__ */

extern int m2NetIfTableRead ();

#endif  /* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INCm2LibPh */
