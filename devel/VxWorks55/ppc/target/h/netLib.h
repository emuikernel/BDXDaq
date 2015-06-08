/* netLib.h - header file for netLib.c */

/* Copyright 1984-2003 Wind River Systems, Inc. */

/*
modification history
--------------------
01p,22jan03,wap  maintain compatibility with old ipLib API
01o,13jan03,rae  Merged from velocecp branch (SPRs 84069, 83252, 82107)
01n,10oct01,rae  merge from truestack ver 01q base 01m
01m,24mar99,ann  protected some #defines within #ifndefs to avoid conflicts
                 in project tool. (only for those defined in 00network.cdf)
01l,01jul97,vin  added routeSockLibInit().
01k,17apr97,vin  added ipFilterLibInit().
01i,08apr97,vin  added mCastRouteLibInit().
01j,31jan97,vin  added default configuration macros.
01h,20jan97,vin  added declarations for protocol initialization functions,
                 added configuration parameters, added interrupt masks.
01g,26aug96,vin  added new declaration for schednetisr().
01f,22sep92,rrr  added support for c++
01e,04jul92,jcf  cleaned up.
01d,26may92,rrr  the tree shuffle
01c,04oct91,rrr  passed through the ansification filter
                  -fixed #else and #endif
                  -changed VOID to void
                  -changed copyright notice
01b,05oct90,dnw deleted private routine.
01a,05oct90,shl created.
*/

#ifndef __INCnetLibh
#define __INCnetLibh

#ifdef __cplusplus
extern "C" {
#endif

/* wrs specific network isr codes */

#define NETISR_IP       1               /* for IP packets */
#define NETISR_ARP      2               /* for arp packets */
#define NETISR_RARP     3               /* for rarp packets */
#define NETISR_IMP      4               /* same as AF_IMPLINK */
#define NETISR_NS       5               /* same as AF_NS */
#define NETISR_ISO      6               /* same as AF_ISO */
#define NETISR_CCITT    7               /* same as AF_CCITT */

/* wrs network isr masks */

#define IP_INTR_MASK    (1 << NETISR_IP)
#define ARP_INTR_MASK   (1 << NETISR_ARP)
#define RARP_INTR_MASK  (1 << NETISR_RARP)
#define IMP_INTR_MASK   (1 << NETISR_IMP)
#define NS_INTR_MASK    (1 << NETISR_NS)
#define ISO_INTR_MASK   (1 << NETISR_ISO)
#define CCITT_INTR_MASK (1 << NETISR_CCITT)

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 1                 /* tell gcc960 not to optimize alignments */
#endif  /* CPU_FAMILY==I960 */

/* ip configuration parameters */

typedef struct ipcfgparams
    {
    int         ipCfgFlags;             /* ip configuration flags */
    int         ipDefTtl;               /* ip packet default time to live */
    int         ipIntrQueueLen;         /* ip interrupt queue length */
    int         ipFragTtl;              /* ip fragment time to live */
    } IP_CFG_PARAMS;

/* icmp configuration parameters */

typedef struct icmpcfgparams
    {
    int         icmpCfgFlags;           /* icmp configuration flags */
    } ICMP_CFG_PARAMS;

/* udp configuration parameters */

typedef struct udpcfgparams
    {
    int         udpCfgFlags;            /* udp configuration flags */
    int         udpSndSpace;            /* udp send space */
    int         udpRcvSpace;            /* udp receive space */
    } UDP_CFG_PARAMS;

/* tcp configuration parameters */

typedef struct tcpcfgparams
    {
    int         tcpCfgFlags;            /* tcp configuration flags */
    int         tcpSndSpace;            /* tcp send space */
    int         tcpRcvSpace;            /* tcp receive space */
    int         tcpConnectTime;         /* tcp connect time out */
    int         tcpReTxThresh;          /* tcp no. dup ACK to trig fast reTx */
    int         tcpMssDflt;             /* tcp default maximum segment size */
    int         tcpRttDflt;             /* tcp default RTT if no data */
    int         tcpKeepIdle;            /* tcp idle time before first probe */
    int         tcpKeepCnt;             /* tcp max probes before dropping */
    FUNCPTR	pTcpRandFunc;		/* tcp random function for tcp_init */
    UINT32      tcpMsl;                 /* tcp max segment lifetime */
    } TCP_CFG_PARAMS;

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 0                 /* turn off alignment requirement */
#endif  /* CPU_FAMILY==I960 */

#define IP_PROTO_NUM_MAX        16       /* maximum size of protocol switch */

/* ip configuration defines */
#define IP_NO_FORWARDING        0x000000000     /* no ip forwarding of pkts */
#define IP_DO_FORWARDING        0x000000001     /* do ip forwarding of pkts */
#define IP_DO_REDIRECT          0x000000002     /* do ip redirecting of pkts */
#define IP_DO_LARGE_BCAST       0x000000004     /* do broadcasting pkt > MTU */
#define IP_DO_CHECKSUM_SND      0x000000008     /* calculate ip checksum */
#define IP_DO_CHECKSUM_RCV      0x000000010
#define IP_DO_ARP_ON_FLAGCHG	0x000000020	/* send gratuitous ARP whenever
                                                 * interface status changes
                                                 * (up/down) instead of just
                                                 * when address is configured.
                                                 */
#define IP_DO_IF_STATUS_CHECK   0x000000040     /* check status of interface */
						/* associated with dest addr */
						/* for incoming packet       */

/* default ip configurations */
#ifndef IP_FLAGS_DFLT
#define IP_FLAGS_DFLT           (IP_DO_FORWARDING | IP_DO_REDIRECT | \
                                 IP_DO_CHECKSUM_SND | IP_DO_CHECKSUM_RCV)
#endif  /* IP_FLAGS_DFLT */

#ifndef IP_TTL_DFLT
#define IP_TTL_DFLT             64              /* default ip queue length */
#endif  /* IP_TTL_DFLT */

#ifndef IP_QLEN_DFLT
#define IP_QLEN_DFLT            50
#endif  /* IP_QLEN_DFLT */

#ifndef IP_FRAG_TTL_DFLT
#define IP_FRAG_TTL_DFLT        60              /* 60 slow time outs, 30secs */
#endif  /* IP_FRAG_TTL_DFLT */

/* udp configuration defines */
#define UDP_DO_NO_CKSUM         0x00000000
#define UDP_DO_CKSUM_SND        0x00000001      /* do check sum on sends */
#define UDP_DO_CKSUM_RCV        0x00000002      /* do check sum on recvs */

/* default udp configurations */

#ifndef UDP_FLAGS_DFLT
#define UDP_FLAGS_DFLT          (UDP_DO_CKSUM_SND | UDP_DO_CKSUM_RCV)
#endif  /* UDP_FLAGS_DFLT */

#ifndef UDP_SND_SIZE_DFLT
#define UDP_SND_SIZE_DFLT       9216            /* default send buffer size */
#endif  /* UDP_SND_SIZE_DFLT */

#ifndef UDP_RCV_SIZE_DFLT
#define UDP_RCV_SIZE_DFLT       41600           /* default recv buffer size */
#endif  /* UDP_RCV_SIZE_DFLT */

/* tcp configuration defines */

#define TCP_NO_RFC1323          0x00000000      /* tcp no RFC 1323 support */
#define TCP_DO_RFC1323          0x00000001      /* tcp RFC 1323 support*/

/* default tcp configurations */

#ifndef TCP_FLAGS_DFLT
#define TCP_FLAGS_DFLT          (TCP_DO_RFC1323)
#endif  /* TCP_FLAGS_DFLT */

#ifndef TCP_SND_SIZE_DFLT
#define TCP_SND_SIZE_DFLT       8192            /* default send buffer size */
#endif  /* TCP_SND_SIZE_DFLT */

#ifndef TCP_RCV_SIZE_DFLT
#define TCP_RCV_SIZE_DFLT       8192            /* default recv buffer size */
#endif  /* TCP_RCV_SIZE_DFLT */

#ifndef TCP_CON_TIMEO_DFLT
#define TCP_CON_TIMEO_DFLT      150             /* 75 secs */
#endif  /* TCP_CON_TIMEO_DFLT */

#ifndef TCP_REXMT_THLD_DFLT
#define TCP_REXMT_THLD_DFLT     3               /* 3 times */
#endif  /* TCP_REXMT_THLD_DFLT */

#ifndef TCP_MSS_DFLT
#define TCP_MSS_DFLT            512             /* default max segment size */
#endif  /* TCP_MSS_DFLT */

#ifndef TCP_RND_TRIP_DFLT
#define TCP_RND_TRIP_DFLT       3               /* 3 secs if no data avail */
#endif  /* TCP_RND_TRIP_DFLT */

#ifndef TCP_IDLE_TIMEO_DFLT
#define TCP_IDLE_TIMEO_DFLT     14400           /* idle timeouts, 2 hrs */
#endif  /* TCP_IDLE_TIMEO_DFLT */

#ifndef TCP_MAX_PROBE_DFLT
#define TCP_MAX_PROBE_DFLT      8               /* probes before dropping */
#endif  /* TCP_MAX_PROBE_DFLT */

#ifndef TCP_RAND_FUNC
#define TCP_RAND_FUNC           (FUNCPTR)random /* TCP random function */
#endif  /* TCP_RAND_FUNC */

#ifndef TCP_MSL_CFG
#define TCP_MSL_CFG             30              /* max. segment lifetime */
#endif

/* icmp configuration defines */
#define ICMP_NO_MASK_REPLY      0x000000000     /* no mask reply support */
#define ICMP_DO_MASK_REPLY      0x000000001     /* do icmp mask reply */

/* default icmp configurations */

#ifndef ICMP_FLAGS_DFLT
#define ICMP_FLAGS_DFLT         (ICMP_NO_MASK_REPLY)
#endif  /* ICMP_FLAGS_DFLT */


/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

extern STATUS   netJobAdd (FUNCPTR routine, int param1, int param2, int param3,
                           int param4, int param5);
extern STATUS   netLibInit (void);
extern void     netErrnoSet (int status);
extern STATUS	netLibGeneralInit (void);
extern void     netTask (void);
extern void     schednetisr (int isrType);
extern STATUS   ipLibInit (IP_CFG_PARAMS * ipCfg);
extern STATUS   ipLibMultiInit (IP_CFG_PARAMS * ipCfg, int maxUnits);
extern STATUS   rawIpLibInit (void);
extern STATUS   rawLibInit (void);
extern STATUS   udpLibInit (UDP_CFG_PARAMS * udpCfg);
extern STATUS   tcpLibInit (TCP_CFG_PARAMS * tcpCfg);
extern STATUS   icmpLibInit (ICMP_CFG_PARAMS * icmpCfg);
extern STATUS   igmpLibInit (void);
extern STATUS   mCastRouteLibInit (void);
extern void     ipFilterLibInit (void);
extern STATUS   routeSockLibInit (void);
extern void     splSemInit (void);
extern STATUS   mbinit (void);
extern void     domaininit (void);

#else   /* __STDC__ */

extern STATUS   netJobAdd ();
extern STATUS   netLibInit ();
extern void     netErrnoSet ();
extern STATUS	netLibGeneralInit ();
extern void     netTask ();
extern void     schednetisr ();
extern STATUS   ipLibInit ();
extern STATUS   ipLibMultiInit ();
extern STATUS   rawIpLibInit ();
extern STATUS   rawLibInit ();
extern STATUS   udpLibInit ();
extern STATUS   tcpLibInit ();
extern STATUS   icmpLibInit ();
extern STATUS   igmpLibInit ();
extern STATUS   mCastRouteLibInit ();
extern void     ipFilterLibInit ();
extern STATUS   routeSockLibInit ();
extern void     splSemInit ();
extern STATUS   mbinit ();
extern void     domaininit ();

#endif  /* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INCnetLibh */

