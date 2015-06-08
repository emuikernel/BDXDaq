/* dhcpcCommonLib.h - common include file for run-time and boot-time client */

/* Copyright 1984 - 2003 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01n,20nov02,wap  allow infinite discover retransmissions (SPR #83246)
01m,08jan02,wap  fix duplicate error codes (SPR #72103)
01l,09oct01,rae  merge from truestack (SPR 69850)
01k,07sep00,spm  moved client-specific transmit routine from shared module
01j,05apr00,spm  added entries for boot-time client shared code
01i,24nov99,spm  upgraded to RFC 2131 and removed direct link-level access
01h,17mar99,spm  enabled dhcpcRelease() routine (SPR #25482)
01g,06oct97,spm  added relative paths to #include statements; removed warnings
01f,06aug97,spm  removed parameters linked list to reduce memory required;
                 added definitions for C++ compilation
01e,10jun97,spm  isolated incoming messages in state machine from input hooks
01d,06may97,spm  added DHCPC_OFF to align IP header on four byte boundary
01c,07apr97,spm  added shutdown return code
01b,29jan97,spm  added little-endian support and modified for coding standards
01a,03oct96,spm  created by modifying WIDE project DHCP implementation
*/

#ifndef __INCdhcpClientCommonh
#define __INCdhcpClientCommonh

#ifdef __cplusplus
extern "C" {
#endif

#include "dhcp/dhcpc.h"
#include "rngLib.h"

#define S_dhcpcLib_NOT_INITIALIZED           (M_dhcpcLib | 1)
#define S_dhcpcLib_BAD_DEVICE                (M_dhcpcLib | 2)
#define S_dhcpcLib_MAX_LEASES_REACHED        (M_dhcpcLib | 3)
#define S_dhcpcLib_MEM_ERROR                 (M_dhcpcLib | 4)
#define S_dhcpcLib_BAD_COOKIE                (M_dhcpcLib | 5)
#define S_dhcpcLib_NOT_BOUND                 (M_dhcpcLib | 6)
#define S_dhcpcLib_BAD_OPTION                (M_dhcpcLib | 7)
#define S_dhcpcLib_OPTION_NOT_PRESENT        (M_dhcpcLib | 8)
#define S_dhcpcLib_TIMER_ERROR               (M_dhcpcLib | 9)
#define S_dhcpcLib_OPTION_NOT_STORED         (M_dhcpcLib | 10)

/* Define sources for events. */

#define DHCP_AUTO_EVENT 0
#define DHCP_USER_EVENT 1

/*  Define types of events. */

#define DHCP_MSG_ARRIVED 0
#define DHCP_TIMEOUT 1
#define DHCP_USER_BIND 2
#define DHCP_USER_VERIFY 3
#define DHCP_USER_RELEASE 4
#define DHCP_USER_SHUTDOWN 5
#define DHCP_USER_INFORM 6

/* State transition definitions (besides OK or ERROR). */

#define DHCPC_DONE 2            /* Lease relinquished or expired - exit. */
#define DHCPC_MORE 3            /* Call a second routine when processing. */
#define DHCPC_STATE_BEGIN 6     /* Initial processing after DHCPC_MORE. */

/* Return codes indicate special situations related to state machine. */

#define  DHCPC_SHUTDOWN 11
#define  DHCPC_ERROR 12

/* Classification of DHCP client leases. */

#define DHCP_MANUAL 0      /* Lease established after system boot. */
#define DHCP_BOOTP 1       /* Lease established using BOOTP reply. */
#define DHCP_NATIVE 2      /* Lease established using DHCP reply. */
#define DHCP_AUTOMATIC 3   /* Bound at boot time - lease still valid. */

/* Internal settings for state machine */

#define FIRSTTIMER          4
#define MAXTIMER           64
#define INIT_WAITING       10
#define DISCOVER_RETRANS    4

#if CPU_FAMILY==I960
#pragma align 1                 /* tell gcc960 not to optimize alignments */
#endif  /* CPU_FAMILY==I960 */

typedef struct leaseData        /* Lease-specific variables. */
    {
    BOOL        initFlag;       /* Lease initialized? */
    BOOL        waitFlag;       /* Synchronous or asynchronous bind attempt? */
    BOOL        autoConfig;     /* Apply parameters to network interface? */
    BOOL 	oldFlag; 	/* Use old (padded) DHCP message formats? */
    SEM_ID      leaseSem;       /* Blocking semaphore for synchronous bind. */
    BOOL        leaseGood;      /* Bind attempt successful? */
    char * 	msgBuffer;
    int         leaseType;      /* Static or dynamic parameters, or BOOTP. */
    int         prevState;      /* Preceding state in state machine. */
    int         currState;      /* Current/next state in state machine. */
    WDOG_ID     timer;          /* Timeout interval for DHCP events */
    time_t      initEpoch;      /* Timestamp of last request. */
    int         timeout;        /* Current timeout interval. */
    int         numRetry;       /* Number of retransmission timeouts. */
    FUNCPTR     eventHookRtn;   /* Event notification hook */
    FUNCPTR     cacheHookRtn;   /* Data storage hook */
    struct dhcp_reqspec leaseReqSpec;   /* Options request list */
    struct if_info ifData;      /* Transmit/receive interface */
    unsigned long       xid;    /* Current transaction ID. */
    struct dhcp_param *dhcpcParam;      /* Current configuration parameters */
    }
LEASE_DATA;

typedef struct messageData 	/* Storage for incoming DHCP messages */
    {
    BOOL writeFlag; 			/* Buffer available? */
    char *msgBuffer;            /* Data buffer contents (copied from BPF) */
    }
MESSAGE_DATA;

typedef struct dhcpcEvent       /* External event descriptor. */
    {
    unsigned char source;       /* Automatic event or manual event? */
    unsigned char type;         /* Manual event - defines user request.
                                 * Automatic - message arrival or timeout.
                                 */
    void * leaseId;             /* Cookie assigned by dhcpcInit (). */
    int slot; 			/* Offset of message buffer in list. */
    char * pMsg; 		/* Start of DHCP message */
    BOOL lastFlag; 		/* Release buffer when finished processing? */
    }
EVENT_DATA;

/* Values for lease established during system boot. */

typedef struct dhcpLeaseData
    {
    struct in_addr yiaddr;          /* assigned IP address */
    unsigned long lease_origin;     /* start time of lease */
    unsigned long lease_duration;   /* length of lease */
    }
DHCP_LEASE_DATA;

struct buffer           /* transmission buffer for DHCP messages */
    {
    char *      buf;
    int         size;
    };

#if CPU_FAMILY==I960
#pragma align 0                 /* turn off alignment requirement */
#endif  /* CPU_FAMILY==I960 */

#define EVENT_RING_SIZE (10 * sizeof (EVENT_DATA))

IMPORT RING_ID  dhcpcEventRing;    /* Ring buffer of DHCP events */
IMPORT SEM_ID   dhcpcEventSem;     /* DHCP event notification */

IMPORT BOOL dhcpcInitialized;
IMPORT DHCP_LEASE_DATA dhcpcBootLease;  /* Settings from boot-time lease. */
IMPORT LEASE_DATA **   dhcpcLeaseList;      /* List of available cookies. */
IMPORT MESSAGE_DATA *  dhcpcMessageList;    /* Incoming DHCP messages. */
IMPORT int     dhcpcMaxLeases;         /* Max. number of simultaneous leases. */
IMPORT int     dhcpcDiscoverRetries;   /* Max. number of DISCOVER retries */
IMPORT void 	dhcpcOptFieldCreate (struct dhcpcOpts *, UCHAR *);
IMPORT void 	dhcpcDefaultsSet (struct dhcp_param *);
IMPORT void 	dhcpcParamsCopy (LEASE_DATA *, struct dhcp_param *);
IMPORT STATUS 	dhcpcArpSend (struct ifnet *, char *, int);
IMPORT STATUS 	dhcpSend (struct ifnet *, struct sockaddr_in *,
                          char *, int, BOOL);
IMPORT STATUS 	dhcpcEventAdd (int, int, void *, BOOL);
IMPORT void 	dhcpcLeaseCleanup (LEASE_DATA *);
#ifdef __cplusplus
}
#endif

#endif
