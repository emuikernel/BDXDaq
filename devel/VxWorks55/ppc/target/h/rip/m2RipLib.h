/* m2RipLib.h - VxWorks MIB-II interface for RIP to SNMP Agent */

/* Copyright 1984 - 2003 Wind River Systems, Inc. */

/*
modification history
--------------------
01f,13jan03,rae  Merged from velocecp branch
01e,28apr02,rae  added extern C jazz (SPR #76303)
01d,12oct01,rae  merge from truestack (M2_rip2IfConfAuthType_md5)
01c,30jun98,spm  corrected size of storage for authentication key
01b,17apr97,gnn  changed variables to follow coding convention.
                 added varToSet defines.
01a,01apr97,gnn	 written.

*/
 
#ifndef __INCm2RipLibh
#define __INCm2RipLibh

#ifdef __cplusplus
extern "C" {
#endif


/* defines for local structures */
#define AUTHKEYLEN 16

/* defines for errors */
#define S_m2RipLib_IFACE_NOT_FOUND            (M_m2RipLib | 1)

/* defines for enumerated types */

/* enumerated values for rip2IfStatStatus */
#define M2_rip2IfStatStatus_valid       1
#define M2_rip2IfStatStatus_invalid     2

/* enumerated values for rip2IfConfAuthType */
#define M2_rip2IfConfAuthType_noAuthentication 1
#define M2_rip2IfConfAuthType_simplePassword 2
#define M2_rip2IfConfAuthType_md5 3

/* enumerated values for rip2IfConfSend */
#define M2_rip2IfConfSend_doNotSend     1
#define M2_rip2IfConfSend_ripVersion1   2
#define M2_rip2IfConfSend_rip1Compatible 3
#define M2_rip2IfConfSend_ripVersion2   4

/* enumerated values for rip2IfConfReceive */
#define M2_rip2IfConfReceive_rip1       1
#define M2_rip2IfConfReceive_rip2       2
#define M2_rip2IfConfReceive_rip1OrRip2 3
#define M2_rip2IfConfReceive_doNotReceive 4

/* enumerated values for rip2IfConfStatus */
#define M2_rip2IfConfStatus_valid       1
#define M2_rip2IfConfStatus_invalid     2

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 1                 /* tell gcc960 not to optimize alignments */
#endif  /* CPU_FAMILY==I960 */

/*
 * The rip2GlobalGroup family. 
 */


typedef struct
    {
    unsigned long   rip2GlobalRouteChanges;
    unsigned long   rip2GlobalQueries;
    } M2_RIP2_GLOBAL_GROUP;

/*
 * The rip2IfStatEntry family. 
 */


typedef struct
    {
    unsigned long   rip2IfStatAddress;
    unsigned long   rip2IfStatRcvBadPackets;
    unsigned long   rip2IfStatRcvBadRoutes;
    unsigned long   rip2IfStatSentUpdates;
    long            rip2IfStatStatus;
    } M2_RIP2_IFSTAT_ENTRY;

/*
 * The rip2IfConfEntry family. 
 */


/* Defines for the fields that can be set. */

#define M2_RIP2_IF_CONF_DOMAIN 0x1
#define M2_RIP2_IF_CONF_AUTH_TYPE 0x2
#define M2_RIP2_IF_CONF_AUTH_KEY 0x4
#define M2_RIP2_IF_CONF_SEND 0x8
#define M2_RIP2_IF_CONF_RECEIVE 0x10
#define M2_RIP2_IF_CONF_DEFAULT_METRIC 0x20
#define M2_RIP2_IF_CONF_STATUS 0x40

typedef struct

    {
    unsigned long   rip2IfConfAddress;
    char            rip2IfConfDomain[2];
    long            rip2IfConfAuthType;
    char            rip2IfConfAuthKey[AUTHKEYLEN];
    long            rip2IfConfSend;
    long            rip2IfConfReceive;
    long            rip2IfConfDefaultMetric;
    long            rip2IfConfStatus;
    } M2_RIP2_IFCONF_ENTRY;

/*
 * The rip2PeerEntry family. 
 */


typedef struct
    {
    unsigned long   rip2PeerAddress;
    char*           rip2PeerDomain;
    unsigned long   rip2PeerLastUpdate;
    long            rip2PeerVersion;
    unsigned long   rip2PeerRcvBadPackets;
    unsigned long   rip2PeerRcvBadRoutes;
    } M2_RIP2_PEER_ENTRY;

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 0                 /* turn off alignment requirement */
#endif  /* CPU_FAMILY==I960 */

/* function declarations */

STATUS m2RipInit (void);
STATUS m2RipDelete (void);
STATUS m2RipGlobalCountersGet(M2_RIP2_GLOBAL_GROUP* pRipGlobal);
STATUS m2RipIfStatEntryGet(int search, M2_RIP2_IFSTAT_ENTRY* pRipIfStat);
STATUS m2RipIfConfEntryGet(int search, M2_RIP2_IFCONF_ENTRY* pRipIfConf);
STATUS m2RipIfConfEntrySet(unsigned int varToSet,
                           M2_RIP2_IFCONF_ENTRY* pRipIfConf);
STATUS m2RipTreeAdd (void);

#ifdef __cplusplus
}
#endif
#endif /* INCm2RipLibh */
