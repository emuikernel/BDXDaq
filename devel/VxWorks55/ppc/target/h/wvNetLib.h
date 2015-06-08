/* wvNetLib.h - header file for WindView networking events */

/* Copyright 1984 - 2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,20feb01,ham  added declaration of wvNetInit() (SPR #63241).
01a,12jan00,spm  written
*/

#ifndef __INCwvNetLibh
#define __INCwvNetLibh

#ifdef __cplusplus
extern "C" {
#endif

#include "private/eventP.h" 	/* Macros for testing WindView status. */
#include "private/wvNetEventP.h"  /* Macros to generate networking events. */

typedef struct eventmask
    {
    UCHAR bitmap [8];     /* Map indicating selection status of events. */
    }
EVENT_MASK;

IMPORT UCHAR wvNetLevel;
IMPORT UCHAR wvNetFilterId;
IMPORT EVENT_MASK * pWvNetEventMap;

#define WV_NET_ID 	0x30 	/*  
                                 * Arbitrary 8-bit prefix for 
                                 * network subsystem events.
                                 */

/* 
 * Class definitions - move to private/eventP.h when WindView 2.0 changes
 * are visible with  tor2_0_x cspec. 
 */

#define WV_NET_CORE_CLASS 0x00000008 	/* Core network events */
#define WV_NET_AUX_CLASS 0x00000010 	/* Auxiliary network events */

#define NET_CORE_EVENT WV_NET_CORE_CLASS
#define NET_AUX_EVENT WV_NET_AUX_CLASS

/* 
 * The WV_BLOCK_START macro prevents wasted cycles by determining if
 * WindView is running and if the given class has been selected. It
 * provides a header before any code which reports network events to
 * WindView. The classId is either NET_CORE_EVENT or NET_AUX_EVENT.
 */

#define WV_BLOCK_START(classId)    \
    if (ACTION_IS_SET)    \
        {    \
        if (WV_EVTCLASS_IS_SET (classId | WV_ON))    \
            {

/*
 * The WV_BLOCK_END provides a footer for all code which reports network
 * events to WindView. It closes the open scopes caused by WV_BLOCK_START.
 * The classId parameter is not used, but is included for symmetry.
 */

#define WV_BLOCK_END(classId)    \
            }    \
        }

/*
 * The WV_NET_ADDR_FILTER_TEST and WV_NET_PORT_FILTER_TEST macros will check 
 * the given data against predefined address or port numbers by calling the
 * appropriate routine. Each macro is only valid within a test conditional.
 */

#define WV_NET_ADDR_FILTER_TEST(filterType,targetType,srcAddr,dstAddr) \
    wvNetAddressFilterTest (filterType, targetType, srcAddr, dstAddr)

#define WV_NET_PORT_FILTER_TEST(filterType, srcPort, dstPort) \
    wvNetPortFilterTest (filterType, srcPort, dstPort)

/* 
 * The WV_NET_EVENT_SET macro constructs a WindView event identifier using 
 * the following structure:
 *
 * <Component ID>:<Module ID>:<Level ID>:<Status>:<Filter ID>:<Event Tag>
 *    (8 bits)      (8 bits)   (4 bits)  (2 bits)   (2 bits)   (8 bits)
 *
 * The first 20 bits are intended for use when instrumenting any VxWorks
 * module. The three fields indicate the VxWorks subsystem, library, and 
 * event priority values. The component ID is a fixed value for all networking 
 * code, and the remaining two fields use the constants defined below. The 
 * final 12 bits apply specifically to networking events. The first two fields
 * encode the transmission direction and packet filter for the event, if 
 * applicable. The packet filter type is assigned using a local variable
 * defined within each module. The final field is a unique identifier for 
 * each event within a given module. Currently, this organization of event 
 * identifiers is ignored by the host-based parser, but it may be used for 
 * refined event filtering someday.
 *
 * The resulting event identifier is stored in the ULONG variable wvNetEventId.
 * It is only valid in a scope where that variable exists. The macro also 
 * references variables for the "Module ID" and "Filter ID" fields so that the 
 * parameters only supply values for the event priority, transmission 
 * direction and event tag, all of which change frequently.
 */

#define WV_NET_EVENT_SET(eventLevel,status,tag)    \
    wvNetEventId = (WV_NET_ID << 24) | (wvNetModuleId << 16) \
                   | (eventLevel << 12) | (status << 10) \
                   | (wvNetLocalFilter << 8) | tag

/* 
 * The WV_NET_MARKER_SET macro constructs identifiers for events which do
 * not involve data transfer. It is a special case of the WV_NET_EVENT_SET
 * macro which omits one parameter since the transmission direction (i.e. - 
 * status) field is always 0.
 */

#define WV_NET_MARKER_SET(eventLevel,tag)    \
    wvNetEventId = (WV_NET_ID << 24) | (wvNetModuleId << 16) \
                   | (eventLevel << 12) | (wvNetLocalFilter << 8) | tag

#define WV_NET_NONE 	0 	/* Placeholder for any unused event fields. */

/* 8-bit component identifiers (unused alternatives to WV_NET_ID). */

#define WV_NET_CORE_BLOCK 	0 	/* Unscalable network processing. */
#define WV_NET_ICMP_BLOCK 	1 	/* ICMP protocol */
#define WV_NET_IGMP_BLOCK 	2 	/* IGMP protocol */
#define WV_NET_UDP_BLOCK 	3 	/* UDP protocol */
#define WV_NET_TCP_BLOCK 	4 	/* TCP protocol */

/* 8-bit module identifiers */

    /* module identifiers for netinet directory */

#define WV_NET_IF_MODULE 	0 	/* if.c source file */
#define WV_NET_IFETHER_MODULE 	1 	/* if_ether.c source file */
#define WV_NET_IFSUBR_MODULE 	2 	/* if_subr.c source file */
#define WV_NET_IGMP_MODULE 	3 	/* igmp.c source file */
#define WV_NET_IN_MODULE 	4 	/* in.c source file */
#define WV_NET_INCKSUM_MODULE 	5 	/* in_cksum.c source file */
#define WV_NET_INPCB_MODULE 	6 	/* in_pcb.c source file */
#define WV_NET_INPROTO_MODULE 	7 	/* in_proto.c source file */
#define WV_NET_IPICMP_MODULE 	8 	/* ip_icmp.c source file */
#define WV_NET_IPINPUT_MODULE 	9 	/* ip_input.c source file */
#define WV_NET_IPMROUTE_MODULE 	10 	/* ip_mroute.c source file */
#define WV_NET_IPOUTPUT_MODULE 	11 	/* ip_output.c source file */
#define WV_NET_RADIX_MODULE 	12 	/* radix.c source file */
#define WV_NET_RAWCB_MODULE 	13 	/* raw_cb.c source file */
#define WV_NET_RAWIP_MODULE 	14 	/* raw_ip.c source file */
#define WV_NET_RAWREQ_MODULE 	15 	/* raw_usrreq.c source file */
#define WV_NET_ROUTE_MODULE 	16 	/* route.c source file */
#define WV_NET_RTSOCK_MODULE 	17 	/* rtsock.c source file */
#define WV_NET_SLCOMP_MODULE 	18 	/* sl_compress.c source file */
#define WV_NET_SYSSOCK_MODULE 	19 	/* sys_socket.c source file */
#define WV_NET_TCPDEBUG_MODULE 	20 	/* tcp_debug.c source file */
#define WV_NET_TCPINPUT_MODULE 	21 	/* tcp_input.c source file */
#define WV_NET_TCPOUT_MODULE 	22 	/* tcp_output.c source file */
#define WV_NET_TCPSUBR_MODULE 	23 	/* tcp_subr.c source file */
#define WV_NET_TCPTIMER_MODULE 	24 	/* tcp_timer.c source file */
#define WV_NET_TCPREQ_MODULE 	25 	/* tcp_usrreq.c source file */
#define WV_NET_UDPREQ_MODULE 	26 	/* udp_usrreq.c source file */
#define WV_NET_UIDOM_MODULE 	27 	/* uipc_dom.c source file */
#define WV_NET_UIMBUF_MODULE 	28 	/* uipc_mbuf.c source file */
#define WV_NET_UISOCK_MODULE 	29 	/* uipc_sock.c source file */
#define WV_NET_UISOCK2_MODULE 	30 	/* uipc_sock2.c source file */
#define WV_NET_UNIXLIB_MODULE 	31 	/* unixLib.c source file */

    /* module identifiers for netwrs directory */

#define WV_NET_IPPROTO_MODULE 	32 	/* ipProto.c source file */

/* 
 * 4-bit level identifiers for event priority. The user can block
 * all event reporting even if WindView is active and a network class
 * is set by specifying the WV_NET_NONE constant in a call to the
 * wvNetLevelSet() routine. That "priority" is not used by any events.
 */

#define WV_NET_EMERGENCY 	1
#define WV_NET_ALERT 		2
#define WV_NET_CRITICAL 	3
#define WV_NET_ERROR 		4
#define WV_NET_WARNING 		5
#define WV_NET_NOTICE 		6
#define WV_NET_INFO 		7
#define WV_NET_VERBOSE 		8

/* Data transfer status (WV_NET_NONE is used if not applicable). */

#define WV_NET_SEND 		1
#define WV_NET_RECV 		2

/* OSI protocol layer for event (WV_NET_NONE is used for other processing). */

#define WV_NET_NETWORK 		1
#define WV_NET_TRANSPORT 	2
#define WV_NET_SESSION 		3
#define WV_NET_APP 		4

/* Packet filter for event (or WV_NET_NONE if unfiltered). */

#define WV_NET_SADDR_FILTER 	1	/* Source address comparison */
#define WV_NET_DADDR_FILTER 	2 	/* Destination address comparison */
#define WV_NET_SPORT_FILTER 	3 	/* Source port comparison */
#define WV_NET_DPORT_FILTER 	4 	/* Destination port comparison */
#define WV_NET_SOCK_FILTER 	5 	/* Socket value comparison */

/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

extern void	wvNetInit (void);
extern void 	wvNetEnable (int);
extern void 	wvNetDisable (void);

#else	/* __STDC__ */

extern void	wvNetInit ();
extern void 	wvNetEnable ();
extern void 	wvNetDisable ();

#endif	/* __STDC__ */


/* 
 * Individual Events. All events contain the integer parameter 
 * wvNetEventId as their first argument. Additional parameters, 
 * if any, are listed with each event.
 */

/* The following events are defined in if.c */

#define WV_NETEVENT_IFINIT_START 20000      /* ID: 0x30008008 */

#define WV_NETEVENT_IFRESET_START 20001     /* ID: 0x30008009 */

#define WV_NETEVENT_IFATTACH_START 20002    /* ID: 0x3000800a */
    /* Param:
     *   struct ifnet * pIf
     */

#define WV_NETEVENT_IFDETACH_START 20003    /* ID: 0x3000800b */
    /* Param:
     *   struct ifnet * pIf
     */

#define WV_NETEVENT_IFAFREE_PANIC 20004    /* 0x30001001 */

#define WV_NETEVENT_LINKRTREQ_FAIL 20005 /* ID: 0x30005005 */

#define WV_NETEVENT_IFDOWN_START 20006   /* ID: 0x3000800c */
    /* UINT32 pIf */

#define WV_NETEVENT_IFUP_START 20007     /* ID: 0x3000800d */
    /* UINT32 pIf */

#define WV_NETEVENT_IFWATCHDOG 20008     /* 0x30007006 */
    /* UINT32 pIf */

#define WV_NETEVENT_IFIOCTL_START 20009 /* 0x30007007 */
    /* UINT32 sockFd
    UINT32 command */

#define WV_NETEVENT_IFIOCTL_BADIFNAME 20010 /* 0x30004003 */
    /* UINT32 sockFd
    UINT32 command */

#define WV_NETEVENT_IFIOCTL_NOPROTO 20011   /* 0x30004004 */
    /* UINT32 sockFd
    UINT32 command */

#define WV_NETEVENT_IFPROMISC_START 20012   /* 0x3000800e */
    /* UINT32 pIf
    UINT32 setting */

#define WV_NETEVENT_IFPROMISC_FAIL 20013    /* 0x30003002 */
    /* UINT32 pIf
    UINT32 setting */

/* The following events are defined in if_ether.c */

#define WV_NETEVENT_ARPTIMER_FREE 20014     /* 0x3001700f */

#define WV_NETEVENT_ARPRTREQ_START 20015    /* 0x30018011 */
    /* UINT32 request */

#define WV_NETEVENT_ARPRTREQ_BADGATE 20016  /* 0x30015008 */
    /* UINT32 gateFamily
    UINT32 gateLength */

#define WV_NETEVENT_ARPRTREQ_FAIL 20017     /* 0x30011001 */

#define WV_NETEVENT_ARPREQ_SEND 20018      /* 0x30018012 */

#define WV_NETEVENT_ARPREQ_FAIL 20019       /* 0x30011402 */

#define WV_NETEVENT_ARPRESOLV_START 20020   /* 0x30018013 */
    /* UINT32 destAddr */

#define WV_NETEVENT_ARPLOOK_FAIL 20021      /* 0x30011003 */
    /* UINT32 destAddr */

#define WV_NETEVENT_ARPINTR_START 20022     /* 0x30018014 */

#define WV_NETEVENT_ARPINTR_FAIL 20023      /* 0x300111004 */

#define WV_NETEVENT_ARPIN_START 20024       /* 0x3001680c */

#define WV_NETEVENT_ARPIN_BADADDR 20025     /* 0x30015809 */
    /* UINT32 srcAddr */

#define WV_NETEVENT_ARPIN_BADADDR2 20026    /* 0x3001580a */
    /* UINT32 srcAddr */

#define WV_NETEVENT_ARPIN_BADADDR3 20027    /* 0x3001580b */
    /* UINT32 srcAddr */
   
#define WV_NETEVENT_ARPIN_DELAYEDSEND 20028  /* 0x3001640d */
    /* UINT32 dstAddr */

#define WV_NETEVENT_ARPIN_REPLYSEND 20029    /* 0x3001640e */
    /* UINT32 srcAddr
    UINT32 dstAddr */

#define WV_NETEVENT_ARPTFREE_PANIC 20030     /* 0x30011005 */

#define WV_NETEVENT_ARPIOCTL_NOTSUPP 20031   /* 0x30014007 */
    /* UINT32 wvNetEventId
    UINT32 reqProtoFamily
    UINT32 reqHwFamily */

#define WV_NETEVENT_ARPIOCTL_START 20032     /* 0x30017010 */
    /* UINT32 request */

#define WV_NETEVENT_ARPIOCTL_SEARCHFAIL 20033  /* 0x30013006 */
    /* UINT32 request
    UINT32 ipAddr */

/* The following events are defined in if_subr.c */

#define WV_NETEVENT_ETHEROUT_IFDOWN 20034    /* 0x30023003 */
    /* UINT32 pIf */

#define WV_NETEVENT_ETHEROUT_NOROUTE 20035   /* 0x30023005 */
    /* UINT32 ipAddr */

#define WV_NETEVENT_ETHEROUT_RTREJECT 20036  /* 0x30023006 */

#define WV_NETEVENT_ETHEROUT_NOBUFS 20037    /* 0x30023004 */

#define WV_NETEVENT_ETHEROUT_AFNOTSUPP 20038  /* 0x30024007 */
    /* UINT32 addrFamily */

#define WV_NETEVENT_ETHEROUT_FINISH 20039    /* 0x3002600c */
    /* UINT32 pIf
    UINT32 result */

#define WV_NETEVENT_ETHERIN_START 20040      /* 0x3002600d */
    /* UINT32 pIf */

#define WV_NETEVENT_ETHERIFATTACH_START 20041  /* 0x3002800e */
    /* UINT32 pIf */

#define WV_NETEVENT_ADDMULT_START 20042    /* 0x3002800f */
    /* UINT32 pIf */

#define WV_NETEVENT_ADDMULT_AFNOTSUPP 20043   /* 0x30024008 */
    /* UINT32 addrFamily */

#define WV_NETEVENT_ADDMULT_BADADDR 20044   /* 0x30024009 */
    /* UINT32 pIf */

#define WV_NETEVENT_ADDMULT_NOBUFS 20045    /*  0x30021001 */

#define WV_NETEVENT_DELMULT_START 20046   /* 0x30028010 */
    /* UINT32 pIf */

#define WV_NETEVENT_DELMULT_AFNOTSUPP 20047 /* 0x3002400a */
    /* UINT32 addrFamily */

#define WV_NETEVENT_DELMULT_BADADDR 20048 /* 0x3002400b */
    /* UINT32 pIf */

#define WV_NETEVENT_ETHERATTACH_START 20049  /* 0x30028011 */
    /* UINT32 pIf */

#define WV_NETEVENT_TYPEADD_FAIL 20050 /* 0x30021002 */
    /* UINT32 etherType */

/* The following events are defined in igmp.c */

#define WV_NETEVENT_IGMPINIT_START 20051 /* 0x30038008 */

#define WV_NETEVENT_IGMPIN_SHORTMSG 20052 /* 0x30033802 */

#define WV_NETEVENT_IGMPIN_BADSUM 20053 /* 0x30033803 */

#define WV_NETEVENT_IGMPIN_BADADDR 20054 /* 0x30035804 */
    /* UINT32 igmpType
    UINT32 ipAddr */

#define WV_NETEVENT_IGMPIN_FINISH 20055   /* 0x30036805 */
    /* UINT32 srcAddr
    UINT32 dstAddr */

#define WV_NETEVENT_IGMPJOIN_START 20056  /* 0x30038009 */

#define WV_NETEVENT_IGMPLEAVE_START 20057  /* 0x3003800a */

#define WV_NETEVENT_IGMPFASTTIMER_START 20058 /* 0x30037007 */

#define WV_NETEVENT_IGMPSENDREP_NOBUFS 20059  /* 0x30031001 */

#define WV_NETEVENT_IGMPSENDREP_FINISH 20060  /* 0x30036406 */
    /* UINT32 dstAddr */

/* The following events are defined in in.c */

#define WV_NETEVENT_INCTRL_START 20061  /* 0x30048007 */
    /* UINT32 request */
    
#define WV_NETEVENT_INCTRL_SEARCHFAIL 20062 /* 0x30044004 */
    /* UINT32 request
    UINT32 pIf */

#define WV_NETEVENT_INCTRL_BADSOCK 20063  /* 0x30045006 */
    /* UINT32 request */

#define WV_NETEVENT_INCTRL_PANIC 20064  /* 0x30041001 */
    /* UINT32 request */

#define WV_NETEVENT_INCTRL_NOBUFS 20065 /* 0x30041002 */
    /* UINT32 request */

#define WV_NETEVENT_INCTRL_BADFLAGS 20066 /* 0x30044005 */
    /* UINT32 request
    UINT32 pIf */

#define WV_NETEVENT_INROUTEDEL_START 20067 /* 0x30048008 */
    /* UINT32 pIf */

#define WV_NETEVENT_INIFINIT_START 20068 /* 0x30048009 */
    /* UINT32 pIf */

#define WV_NETEVENT_INIFADDRDEL_START 20069 /* 0x3004800a */

#define WV_NETEVENT_INADDMULT_START 20070   /* 0x3004800b */
    /* UINT32 ipAddr
    UINT32 pIf */

#define WV_NETEVENT_INADDMULT_NOBUFS 20071 /* 0x30041003 */
    /* UINT32 ipAddr
    UINT32 pIf */

#define WV_NETEVENT_INDELMULT_START 20072 /* 0x3004800c */

/* The following events are defined in in_pcb.c */

#define WV_NETEVENT_PCBALLOC_NOBUFS 20073 /* 0x30061001 */
    /* UINT32 sockNum */

#define WV_NETEVENT_PCBBIND_NOADDR 20074 /* 0x30064002 */
    /* UINT32 sockNum */

#define WV_NETEVENT_PCBBIND_BADSOCK 20075 /* 0x30064003 */
    /* UINT32 sockNum
    UINT32 localAddr
    UINT32 localPort */

#define WV_NETEVENT_PCBBIND_BADADDRLEN 20076 /* 0x30064004 */
    /* UINT32 sockNum
    UINT32 addrLen */

#define WV_NETEVENT_PCBBIND_BADADDR 20077 /* 0x30064005 */
    /* UINT32 sockNum
    UINT32 destAddr */

#define WV_NETEVENT_PCBBIND_BADPORT 20078 /* 0x30064006 */
    /* UINT32 sockNum
    UINT32 localPort */

#define WV_NETEVENT_PCBLADDR_BADADDR 20079 /* 0x30064007 */
    /* UINT32 sockNum
    UINT32 addrLen
    UINT32 addrFam
    UINT32 addrPort */

#define WV_NETEVENT_PCBLADDR_BADIF 20080  /* 0x30064008 */
    /* UINT32 sockNum */

#define WV_NETEVENT_PCBCONNECT_BADADDR 20081 /* 0x30064009 */
    /* UINT32 sockNum
    UINT32 destAddr
    UINT32 localAddr
    UINT32 localPort */

#define WV_NETEVENT_TCPTIMER_ROUTEDROP 20082  /* 0x3006500a */

#define WV_NETEVENT_PCBBIND_START 20083  /* 0x3006800b */
    /* UINT32 sockNum */

#define WV_NETEVENT_PCBCONNECT_START 20084 /* 0x3006800c */
    /* UINT32 sockNum */

#define WV_NETEVENT_PCBDISCONN_START 20085  /* 0x3006800d */
    /* UINT32 sockNum */

#define WV_NETEVENT_PCBNOTIFY_START 20086 /* 0x3006800e */
    /* UINT32 request
    UINT32 localAddr
    UINT32 destAddr */

/* The following events are defined in ip_icmp.c */

/*
 * The icmp_error routine might be classified as an emergency event,
 * depending on the type of error detected. ICMP errors which are not 
 * emergencies use the critical category for the identical event.
 */ 

#define WV_NETEVENT_ICMPERR_START 20087    /* 0x30081002 OR 0x30083002 */
    /* UINT32 errType
    UINT32 errCode */

#define WV_NETEVENT_ICMPERR_PANIC 20088    /* 0x30081001 */
    /* UINT32 errType */

#define WV_NETEVENT_ICMPIN_SHORTMSG 20089  /* 0x30083803 */

#define WV_NETEVENT_ICMPIN_BADSUM 20090  /* 0x30083804 */

#define WV_NETEVENT_ICMPIN_BADLEN 20091  /* 0x30083805 */
    /* UINT32 msgLen
    UINT32 ipHdrLen */

#define WV_NETEVENT_ICMPCTRL_BADLEN 20092 /* 0x30084006 */
    /* UINT32 namelen */

#define WV_NETEVENT_ICMPCTRL_BADCMD 20093 /* 0x30084007 */
    /* UINT32 ctrlType */

#define WV_NETEVENT_ICMPIN_START 20094  /* 0x30086808 */

#define WV_NETEVENT_ICMPREFLECT_START 20095 /* 0x30086409 */

#define WV_NETEVENT_ICMPSEND_START 20096 /* 0x3008640a */

#define WV_NETEVENT_ICMPCTRL_START 20097 /* 0x3008700b */
    /* UINT32 ctrlType */

/* The following events are defined in ip_input.c */

#define WV_NETEVENT_IPINIT_PANIC 20098 /* 0x30091001 */

#define WV_NETEVENT_IPIN_PANIC 20099  /* 0x30091802 */

#define WV_NETEVENT_IPIN_SHORTMSG 20100 /* 0x30093803 */

#define WV_NETEVENT_IPIN_BADVERS 20101 /* 0x30093804 */
    /* UINT32 version */

#define WV_NETEVENT_IPIN_BADHLEN 20102 /* 0x30093805 */
    /* UINT32 hdrLength */

#define WV_NETEVENT_IPIN_BADSUM 20103 /* 0x30093806 */

#define WV_NETEVENT_IPIN_BADLEN 20104 /* 0x30093807 */
    /* UINT32 ipLength */

#define WV_NETEVENT_IPIN_BADMBLK 20105 /* 0x30093808 */
    /* UINT32 mblkLength */

#define WV_NETEVENT_IPIN_BADOPT 20106 /* 0x30093809 */
    /* UINT32 errType
    UINT32 errCode */

#define WV_NETEVENT_IPFWD_BADADDR 20107 /* 0x3009380a */
    /* UINT32 dstAddr */

#define WV_NETEVENT_IPCTRL_BADCMDLEN 20108 /* 0x3009400b */
    /* UINT32 namelen */

#define WV_NETEVENT_IPCTRL_BADCMD 20109 /* 0x3009400c */
    /* UINT32 ctrlType */

#define WV_NETEVENT_IPIN_FRAGDROP 20110 /* 0x3009580d */
    /* UINT32 overlapFlag - address of fragment, or 0 if out of memory. */

#define WV_NETEVENT_IPIN_FINISH 20111 /* 0x3009580e */

#define WV_NETEVENT_IPFWD_STATUS 20112 /* 0x3009640f */
    /* UINT32 result */

#define WV_NETEVENT_IPIN_GOODMBLK 20113 /* 0x30097810 */

#define WV_NETEVENT_IPFRAGFREE_START 20114 /* 0x30097011 */

#define WV_NETEVENT_IPTIMER_FRAGFREE 20115 /* 0x30097012 */

#define WV_NETEVENT_IPCTRL_START 20116 /* 0x30097013 */
    /* UINT32 ctrlType */

#define WV_NETEVENT_IPINIT_START 20117 /* 0x30098014 */

/* The following events are defined in ip_mroute.c */

#define WV_NETEVENT_ADDMRT_NOBUFS 20118  /* 0x300a1001 */

#define WV_NETEVENT_IPMRT_NOTINIT 20119 /* 0x300a4002 */

#define WV_NETEVENT_IPMRT_BADCMD 20120  /* 0x300a4003 */
    /* UINT32 request */

#define WV_NETEVENT_ADDVIF_BADINDEX 20121 /* 0x300a4004 */

#define WV_NETEVENT_ADDVIF_BADENTRY 20122 /* 0x300a4005 */
    /* UINT32 localAddr */

#define WV_NETEVENT_ADDVIF_SEARCHFAIL 20123 /* 0x300a4006 */
    /* UINT32 sockNum
    UINT32 destAddr */

#define WV_NETEVENT_ADDVIF_BADFLAGS 20124 /* 0x300a4007 */
    /* UINT32 pIf */

#define WV_NETEVENT_DELVIF_BADINDEX 20125   /* 0x300a4008 */

#define WV_NETEVENT_DELVIF_BADENTRY 20126 /* 0x300a4009 */
    /* UINT32 index */

#define WV_NETEVENT_TUNNEL_STATUS 20127 /* 0x300a640a */
    /* UINT32 result */

#define WV_NETEVENT_PHYINT_STATUS 20128 /* 0x300a640b */
    /* UINT32 result */

#define WV_NETEVENT_IPMRT_START 20129 /* 0x300a700c */
    /* UINT32 request */

/* The following events are defined in ip_output.c */

#define WV_NETEVENT_IPOUT_BADADDR 20130 /* 0x300b3401 */
    /* UINT32 dstAddr */

#define WV_NETEVENT_IPOUT_BADFLAGS 20131 /* 0x300b3402 */
    /* UINT32 dstAddr */

#define WV_NETEVENT_IPOUT_CANTFRAG 20132 /* 0x300b3403 */
    /* UINT32 dstAddr
       UINT32 pIf */

#define WV_NETEVENT_IPOUT_SHORTMSG 20133 /* 0x300b3404 */
    /* UINT32 dstAddr
       UINT32 pIf */

#define WV_NETEVENT_IPOUTCTRL_BADCMD 20134 /* 0x300b3405 */
    /* UINT32 sockNum
       UINT32 request */

#define WV_NETEVENT_IPOUTCTRL_START 20135 /* 0x300b7006 */
    /* UINT32 sockNum
       UINT32 request */

#define WV_NETEVENT_IPMLOOP_START 20136 /* 0x300b7807 */

#define WV_NETEVENT_IPOUT_CACHEMISS 20294 /* 0x300b8408 */

#define WV_NETEVENT_IPOUT_FINISH 20295 /* 0x300b4409 */

#define WV_NETEVENT_IPOUT_SENDALL 20296 /* 0x300b740a */

#define WV_NETEVENT_IPOUT_SENDFRAG 20297 /* 0x300b740b */

/* The following events are defined in radix.c */

#define WV_NETEVENT_RNINIT_PANIC 20137 /* ID: 0x300c1001 */

#define WV_NETEVENT_RNADD_BADMASK 20138 /* ID: 0x300c2002 */

#define WV_NETEVENT_RNADD_NOMASK 20139 /* ID: 0x300c2003 */

#define WV_NETEVENT_RNADD_BADROUTE 20140 /* ID: 0x300c2004 */

#define WV_NETEVENT_RNDEL_BADREFCNT 20141 /* ID: 0x300c2005 */

#define WV_NETEVENT_RNDEL_BADMASK 20142 /* ID: 0x300c2006 */

#define WV_NETEVENT_RNDEL_SEARCHFAIL 20143 /* ID: 0x300c2007 */

#define WV_NETEVENT_RNDEL_KEYSEARCHFAIL 20144 /* ID: 0x300c2008 */

#define WV_NETEVENT_RNDEL_EXTRAMASK 20145 /* ID: 0x300c2009 */

/* The following events are defined in raw_cb.c */

#define WV_NETEVENT_RAWATTACH_NOPCBMEM 20146 /* 0x300d3001 */

#define WV_NETEVENT_RAWATTACH_NOSOCKBUFMEM 20147 /* 0x300d3002 */

#define WV_NETEVENT_RAWBIND_NOIF 20148 /* 0x300d4003 */

#define WV_NETEVENT_RAWATTACH_START 20149 /* 0x300d8004 */

#define WV_NETEVENT_RAWDETACH_START 20150 /* 0x300d8005 */

#define WV_NETEVENT_RAWBIND_START 20151   /* 0x300d8006 */

/* The following events are defined in raw_ip.c */

#define WV_NETEVENT_RAWIPREQ_PANIC 20152 /* 0x300e1001 */

#define WV_NETEVENT_RAWCTLOUT_BADLEVEL 20153 /* 0x300e4002 */

#define WV_NETEVENT_RAWCTLOUT_BADMEM 20154   /* 0x300e4003 */

#define WV_NETEVENT_RAWIPREQ_FAIL 20155   /* 0x300e4004 */

#define WV_NETEVENT_RAWIPIN_NOSOCK 20156 /* 0x300e4805 */

#define WV_NETEVENT_RAWIPIN_LOST 20157 /* 0x300e5806 */

#define WV_NETEVENT_RAWIPIN_START 20158 /* 0x300e6807 */

#define WV_NETEVENT_RAWIPOUT_FINISH 20159   /* 0x300e6408 */

#define WV_NETEVENT_RAWCTLOUT_START 20160  /* 0x300e7009 */

#define WV_NETEVENT_RAWIPREQ_START 20161  /* 0x300e700a */

#define WV_NETEVENT_RAWIPINIT_START 20162 /* 0x300e800b */

/* The following events are defined in raw_usrreq.c */

#define WV_NETEVENT_RAWREQ_PANIC 20163   /* 0x300f1001 */

#define WV_NETEVENT_RAWREQ_FAIL 20164 /* 0x300f4002 */

#define WV_NETEVENT_RAWIN_LOST 20165 /* 0x300f5803 */

#define WV_NETEVENT_RAWIN_START 20166 /* 0x300f6804 */

#define WV_NETEVENT_RAWCTLIN_START 20167 /* 0x300f7005 */

#define WV_NETEVENT_RAWREQ_START 20168 /* 0x300f7006 */

#define WV_NETEVENT_RAWINIT_START 20169 /* 0x300f8007 */

/* The following events are defined in route.c */

#define WV_NETEVENT_RTFREE_PANIC 20170 /* 0x30101001 */

#define WV_NETEVENT_RTREQ_PANIC 20171 /* 0x30101002 */

#define WV_NETEVENT_RTSET_NOBUFS 20172 /* 0x30101003 */

#define WV_NETEVENT_RTFREE_BADREFCNT 20173 /* 0x30102004 */

#define WV_NETEVENT_RTALLOC_SEARCHFAIL 20174 /* 0x30103005 */

#define WV_NETEVENT_RTRESET_BADDEST 20175 /* 0x30103006 */

#define WV_NETEVENT_RTREQ_BADROUTE 20176 /* 0x30103007 */

#define WV_NETEVENT_RTIOCTL_BADREQ 20177 /* 0x30104008 */

#define WV_NETEVENT_RTALLOC_CLONE 20178 /* 0x30107009 */

#define WV_NETEVENT_RTALLOC_SEARCHGOOD 20179 /* 0x3010700a */

#define WV_NETEVENT_RTFREE_START 20180 /* 0x3010700b */

#define WV_NETEVENT_RTIOCTL_START 20181 /* 0x3010700c */

#define WV_NETEVENT_ROUTEINIT_START 20182 /* 0x3010800d */

#define WV_NETEVENT_RTRESET_STATUS 20183 /* 0x3010800e */

#define WV_NETEVENT_RTINIT_START 20184 /* 0x3010800f */

/* The following events are defined in rtsock.c */

#define WV_NETEVENT_RTOUT_BADMSG 20185 /* 0x30111401 */

#define WV_NETEVENT_RTOUT_PANIC 20186 /* 0x30111402 */

#define WV_NETEVENT_COPYDATA_PANIC 20187 /* 0x30111003 */

#define WV_NETEVENT_RTOUTPUT_FAIL 20188 /* 0x30113404 */

#define WV_NETEVENT_RTOUTPUT_START 20189 /* 0x30116405 */

#define WV_NETEVENT_ROUTEREQ_START 20190 /* 0x30117006 */

#define WV_NETEVENT_ROUTEWALK_START 20191 /* 0x30117007 */

#define WV_NETEVENT_RTSOCKINIT_START 20192 /* 0x30118008 */

/* The following events are defined in sys_socket.c */

#define WV_NETEVENT_SOCKIOCTL_START 20193 /* 0x30137001 */

#define WV_NETEVENT_SOCKSELECT_START 20194 /* 0x30137002 */

#define WV_NETEVENT_SOCKUNSELECT_START 20195 /* 0x30137003 */

/* The following events are defined in tcp_input.c */

#define WV_NETEVENT_TCPOOB_PANIC 20196 /* 0x30151801 */

#define WV_NETEVENT_TCPIN_BADSUM 20197 /* 0x30153802 */

#define WV_NETEVENT_TCPIN_BADSEG 20198 /* 0x30153803 */

#define WV_NETEVENT_TCPIN_SEARCHFAIL 20199 /* 0x30155804 */

#define WV_NETEVENT_TCPIN_BADSYNC 20200 /* 0x30155805 */

#define WV_NETEVENT_TCPIN_SENDFAIL 20201 /* 0x30155806 */

#define WV_NETEVENT_TCPIN_START 20202 /* 0x30156807 */

#define WV_NETEVENT_TCPIN_DUPSEG 20203 /* 0x30157808 */

#define WV_NETEVENT_TCPIN_PUREACK 20204 /* 0x30157809 */

#define WV_NETEVENT_TCPIN_PUREDATA 20205 /* 0x3015780a */

#define WV_NETEVENT_TCPIN_ACCEPT 20206 /* 0x3015780b */

#define WV_NETEVENT_TCPIN_CONNECT 20207 /* 0x3015780c */

#define WV_NETEVENT_TCPIN_DATACLOSED 20208 /* 0x3015780d */

#define WV_NETEVENT_TCPIN_RESET 20209 /* 0x3015780e */

#define WV_NETEVENT_TCPIN_SENDCLOSED 20210 /* 0x3015780f */

#define WV_NETEVENT_TCPIN_CLOSEWAIT 20211 /* 0x30157810 */

#define WV_NETEVENT_TCPIN_RECVCLOSED 20212 /* 0x30157811 */

#define WV_NETEVENT_TCPIN_CLOSING 20213 /* 0x30157812 */

#define WV_NETEVENT_TCPIN_CLOSE_RESTART 20214 /* 0x30157813 */

#define WV_NETEVENT_TCPIN_DUPDATA 20215 /* 0x30158814 */

#define WV_NETEVENT_TCPIN_SHORTSEG 20216 /* 0x30158815 */

/* The following events are defined in tcp_output.c */

#define WV_NETEVENT_TCPOUT_HDRPANIC 20217 /* 0x30161401 */

#define WV_NETEVENT_TCPOUT_PANIC 20218 /* 0x30161402 */

#define WV_NETEVENT_TCPOUT_FINISH 20219 /* 0x30166403 */

/* The following events are defined in tcp_subr.c */

#define WV_NETEVENT_TCPINIT_HDRPANIC 20220 /* 0x30171001 */

#define WV_NETEVENT_TCPRESPOND_START 20221 /* 0x30176402 */

#define WV_NETEVENT_TCPDROP_START 20222 /* 0x30177003 */

#define WV_NETEVENT_TCPCLOSE_START 20223 /* 0x30177004 */

#define WV_NETEVENT_TCPNOTIFY_IGNORE 20224 /* 0x30177005 */

#define WV_NETEVENT_TCPNOTIFY_KILL 20225 /* 0x30177006 */ 

#define WV_NETEVENT_TCPNOTIFY_ERROR 20226 /* 0x30177005 */

#define WV_NETEVENT_TCPCTLIN_START 20227 /* 0x30177008 */

#define WV_NETEVENT_TCPINIT_START 20228 /* 0x30178009 */ 

/* The following events are defined in tcp_subr.c */

#define WV_NETEVENT_TCPFASTTIMER_START 20229 /* 0x30177001 */ 

#define WV_NETEVENT_TCPSLOWTIMER_START 20230 /* 0x30177002 */ 

#define WV_NETEVENT_TCPTIMER_START 20231 /* 0x30188003 */ 

/* The following events are defined in tcp_usrreq.c */

#define WV_NETEVENT_TCPREQ_PANIC 20232 /* 0x30191001 */ 

#define WV_NETEVENT_TCPCTLOUT_NOPCB 20233 /* 0x30193002 */ 

#define WV_NETEVENT_TCPATTACH_NOBUFS 20234 /* 0x30193003 */ 

#define WV_NETEVENT_TCPREQ_BADMEM 20235 /* 0x30194004 */ 

#define WV_NETEVENT_TCPREQ_NOPCB 20236 /* 0x30194005 */ 

#define WV_NETEVENT_TCPREQ_FAIL 20237 /* 0x30194006 */ 

#define WV_NETEVENT_TCPCTLOUT_START 20238 /* 0x30197007 */ 

#define WV_NETEVENT_TCPREQ_START 20239 /* 0x30197008 */ 

#define WV_NETEVENT_TCPATTACH_START 20240 /* 0x30198009 */ 

#define WV_NETEVENT_TCPDISCONN_START 20241 /* 0x3019800a */ 

/* The following events are defined in udp_usrreq.c */

#define WV_NETEVENT_UDPREQ_PANIC 20242 /* 0x301a1001 */ 

#define WV_NETEVENT_UDPOUT_FAIL 20243 /* 0x301a4402 */ 

#define WV_NETEVENT_UDPREQ_NOPCB 20244 /* 0x301a4003 */ 

#define WV_NETEVENT_UDPIN_SHORTPKTHDR 20245 /* 0x301a5804 */ 

#define WV_NETEVENT_UDPIN_BADLEN 20246 /* 0x301a5805 */ 

#define WV_NETEVENT_UDPIN_BADSUM 20247 /* 0x301a5806 */ 

#define WV_NETEVENT_UDPIN_FULLSOCK 20248 /* 0x301a5807 */ 

#define WV_NETEVENT_UDPIN_SEARCHFAIL 20249 /* 0x301a5808 */ 

#define WV_NETEVENT_UDPIN_NOPORT 20250 /* 0x301a5809 */ 

#define WV_NETEVENT_UDPIN_START 20251 /* 0x301a680a */ 

#define WV_NETEVENT_UDPOUT_FINISH 20252 /* 0x301a640b */ 

#define WV_NETEVENT_UDPNOTIFY_START 20253 /* 0x301a700c */ 

#define WV_NETEVENT_UDPCTLIN_START 20254 /* 0x301a700d */ 

#define WV_NETEVENT_UDPREQ_START 20255 /* 0x301a700e */ 

#define WV_NETEVENT_UDPCTRL_START 20256 /* 0x301a700f */ 

#define WV_NETEVENT_UDPINIT_START 20257 /* 0x301a8010 */ 

#define WV_NETEVENT_UDPIN_CACHEMISS 20258 /* 0x301a8811 */ 

#define WV_NETEVENT_UDPIN_PCBGOOD 20293 /* 0x301a7812 */ 

/* The following events are defined in uipc_mbuf.c */

#define WV_NETEVENT_POOLINIT_PANIC 20259 /* 0x301c1001 */ 

#define WV_NETEVENT_MEMINIT_PANIC 20260 /* 0x301c1002 */ 

#define WV_NETEVENT_DEVMEM_PANIC 20261 /* 0x301c1003 */ 

#define WV_NETEVENT_MEMCOPY_PANIC 20262 /* 0x301c1004 */ 

#define WV_NETEVENT_MEMFREE_PANIC 20263 /* 0x301c1005 */ 

/* The following events are defined in uipc_sock.c */

#define WV_NETEVENT_SOFREE_PANIC 20264 /* 0x301d1001 */ 

#define WV_NETEVENT_SOCLOSE_PANIC 20265 /* 0x301d1002 */ 

#define WV_NETEVENT_SOACCEPT_PANIC 20266 /* 0x301d1003 */ 

#define WV_NETEVENT_SOSEND_FAIL 20267 /* 0x301d3404 */ 

#define WV_NETEVENT_SORECV_FAIL 20268 /* 0x301d3805 */ 

#define WV_NETEVENT_SOCREATE_SEARCHFAIL 20269 /* 0x301d4006 */ 

#define WV_NETEVENT_SOCREATE_BADTYPE 20270 /* 0x301d4007 */ 

#define WV_NETEVENT_SOCONNECT_BADSOCK 20271 /* 0x301d4008 */ 

#define WV_NETEVENT_SODISCONN_STATUS 20272 /* 0x301d4009 */ 

#define WV_NETEVENT_SOCLOSE_WAIT 20273 /* 0x301d700a */ 

#define WV_NETEVENT_SOCREATE_START 20274 /* 0x301d800b */ 

#define WV_NETEVENT_SOBIND_START 20275 /* 0x301d800c */ 

#define WV_NETEVENT_SOLISTEN_START 20276 /* 0x301d800d */ 

#define WV_NETEVENT_SOFREE_START 20277 /* 0x301d800e */ 

#define WV_NETEVENT_SOCLOSE_START 20278 /* 0x301d800f */ 

#define WV_NETEVENT_SOABORT_START 20279 /* 0x301d8010 */ 

#define WV_NETEVENT_SOACCEPT_START 20280 /* 0x301d8011 */ 

#define WV_NETEVENT_SOCONNECT_START 20281 /* 0x301d8012 */ 

#define WV_NETEVENT_SOCONNECT2_START 20282 /* 0x301d8013 */ 

#define WV_NETEVENT_SOSEND_FINISH 20283 /* 0x301d8014 */ 

#define WV_NETEVENT_SORECV_FINISH 20284 /* 0x301d8015 */ 

#define WV_NETEVENT_SOSHUTDOWN_START 20285 /* 0x301d8016 */ 

/* The following events are defined in uipc_sock2.c */

#define WV_NETEVENT_SBAPPENDADDR_PANIC 20286 /* 0x301e1801 */ 

#define WV_NETEVENT_SBAPPENDCTRL_PANIC 20287 /* 0x301e1002 */ 

#define WV_NETEVENT_SBFLUSH_PANIC 20288 /* 0x301e1003 */ 

#define WV_NETEVENT_SBDROP_PANIC 20289 /* 0x301e1004 */ 

#define WV_NETEVENT_SBWAIT_SLEEP 20290 /* 0x301e7005 */ 

#define WV_NETEVENT_SBWAKEUP_START 20291 /* 0x301e7006 */ 

/* The following events are defined in unixLib.c */

#define WV_NETEVENT_HASHINIT_PANIC 20292 /* 0x301f1001 */ 

/* end of VxWorks networking events */

#ifdef __cplusplus
}
#endif

#endif /* __INCwvNetLibh */
