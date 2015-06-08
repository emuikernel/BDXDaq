/* bootpLib.h - BOOTP client include file */

/* Copyright 1990 - 2003 Wind River Systems, Inc. */

/*
modification history
--------------------
01q,22jan03,wap  Make init routine backwards compatible
01p,17jan03,wap  Avoid compiler warning due to redefinition of IPHL/UDPHL
                 macros. (SPR #83244)
01o,13jan03,rae  Merged from velocecp branch (SPR #83245)
01n,14jun00,spm  upgraded to RFC 1542 and removed direct link-level access;
                 changed interface to isolate message body values from options
01m,07apr97,spm  changed BOOTP interface to DHCP style: all options supported
01l,26sep96,spm  fixed SPR 7120: added parameter for gateway to bootpParamsGet
01k,22sep92,rrr  added support for c++
01j,04jul92,jcf  cleaned up.
01i,11jun92,elh  modified parameters to bootpParamsGet.
01h,26may92,rrr  the tree shuffle
		  -changed includes to have absolute path from h/
01g,17apr92,elh  moved prototypes icmpLib.
01f,28feb92,elh  ansified.
01e,27aug91,elh  added RFC 1048 stuff, and added errors.
01d,12aug90,dnw  changed retransmission delay parameters
01c,12aug90,hjb  major revision
01b,19apr90,hjb  added VX_LOG_FILE definition
01a,11mar90,hjb  written
*/

#ifndef __INCbootpLibh
#define __INCbootpLibh

#ifdef __cplusplus
extern "C" {
#endif

/* includes */

#include "vwModNum.h"
#include "netinet/in.h"

/* defines */
						/* BOOTP reserved ports */
#define _BOOTP_SPORT            67
#define _BOOTP_CPORT            68

#define BOOTREQUEST             1		/* BOOTP operations	*/
#define BOOTREPLY               2

#define SIZE_HLEN 		16
#define SIZE_SNAME 		64
#define SIZE_VEND		64
#define SIZE_FILE		128

#ifndef UDPHL
#define UDPHL 	sizeof (struct udphdr) 		/* UDP header length */
#endif

#ifndef IPHL
#define IPHL 	sizeof (struct ip) 		/* IP header length */
#endif

/* retransmission delay parameters */

#define INIT_BOOTP_DELAY	4	/* initial retransmit delay (secs) */
#define MAX_BOOTP_DELAY		64 	/* maximum retransmit delay (secs) */

/* Hardware type values from the arp section of the assigned numbers RFC. */

#define ETHER           1       /* Ethernet (10Mb) */
#define EXPETHER        2       /* Experimental Ethernet (3Mb) */
#define AX25            3       /* Amateur Radio AX.25 */
#define PRONET          4       /* Proteon ProNET Token Ring */
#define CHAOS           5       /* Chaos */
#define IEEE802         6       /* IEEE 802 Networks */
#define ARCNET          7       /* ARCNET */
#define HYPERCH         8       /* Hyperchannel */
#define LANSTAR         9       /* Lanstar */
#define AUTONET         10      /* Autonet Short Address */
#define LOCALTALK       11      /* LocalTalk */
#define LOCALNET        12      /* LocalNet */
#define ULTRALINK       13      /* Ultra link */
#define FRAMERELAY      15      /* Frame Relay */
#define SERIAL          20      /* Serial Line */
#define DNS_NAME        128     /* DNS name */

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 1					/* dont optimize alignments */
#endif  /* CPU_FAMILY==I960 */

                                                /* BOOTP message structure */
typedef struct bootp_msg
    {
    unsigned char	bp_op;			/* packet opcode type	*/
    unsigned char	bp_htype;       	/* hardware addr type	*/
    unsigned char	bp_hlen;		/* hardware addr length */
    unsigned char	bp_hops;		/* gateway hops 	*/
    unsigned long	bp_xid;			/* transaction ID 	*/
    unsigned short	bp_secs;		/* seconds since boot 	*/
    unsigned short	bp_unused;
    struct in_addr	bp_ciaddr;		/* client IP address 	*/
    struct in_addr	bp_yiaddr;		/* 'your' IP address 	*/
    struct in_addr	bp_siaddr;		/* server IP address 	*/
    struct in_addr	bp_giaddr;		/* gateway IP address 	*/
    unsigned char	bp_chaddr [SIZE_HLEN]; 	/* client hardware addr	*/
    unsigned char	bp_sname [SIZE_SNAME]; 	/* server host name 	*/
    unsigned char	bp_file [SIZE_FILE];	/* boot file name 	*/
    unsigned char	bp_vend [SIZE_VEND];	/* vendor-specific area */
    } BOOTP_MSG;

#define BOOTPLEN sizeof (struct bootp_msg)

struct in_addr_list
    {
    unsigned char 	num;
    struct in_addr * 	addrlist;
    };

struct ushort_list
    {
    unsigned char 	num;
    unsigned short * 	shortlist;
    };

struct bootpParams 	/* BOOTP options/vendor extensions from RFC 1533 */
    {
    struct in_addr * 		netmask;
    unsigned short *		timeOffset;
    struct in_addr_list * 	routers;
    struct in_addr_list * 	timeServers;
    struct in_addr_list * 	nameServers;
    struct in_addr_list * 	dnsServers;
    struct in_addr_list * 	logServers;
    struct in_addr_list * 	cookieServers;
    struct in_addr_list * 	lprServers;
    struct in_addr_list * 	impressServers;
    struct in_addr_list * 	rlpServers;
    char * 			clientName;
    unsigned short *		filesize;
    char * 			dumpfile; 
    char * 			domainName;
    struct in_addr * 		swapServer;
    char * 			rootPath;
    char * 			extoptPath;
    unsigned char *		ipForward;
    unsigned char *		nonlocalSourceRoute;
    struct in_addr_list * 	policyFilter;
    unsigned short *		maxDgramSize;
    unsigned char *		ipTTL;
    unsigned long *		mtuTimeout;
    struct ushort_list * 	mtuTable;
    unsigned short * 		intfaceMTU;
    unsigned char * 		allSubnetsLocal;
    struct in_addr * 		broadcastAddr;
    unsigned char * 		maskDiscover;
    unsigned char * 		maskSupplier;
    unsigned char * 		routerDiscover;
    struct in_addr * 		routerDiscAddr;
    struct in_addr_list * 	staticRoutes;
    unsigned char * 		arpTrailers;
    unsigned long * 		arpTimeout;
    unsigned char * 		etherPacketType;
    unsigned char * 		tcpTTL;
    unsigned long * 		tcpInterval;
    unsigned char * 		tcpGarbage;
    char * 			nisDomain;
    struct in_addr_list * 	nisServers;
    struct in_addr_list * 	ntpServers;
    char * 			vendString;
    struct in_addr_list * 	nbnServers;
    struct in_addr_list * 	nbddServers;
    unsigned char * 		nbNodeType;
    char * 			nbScope;
    struct in_addr_list * 	xFontServers;
    struct in_addr_list * 	xDisplayManagers; 
    char * 			nispDomain;
    struct in_addr_list * 	nispServers;
    struct in_addr_list * 	ipAgents;
    struct in_addr_list * 	smtpServers;
    struct in_addr_list * 	pop3Servers;
    struct in_addr_list * 	nntpServers;
    struct in_addr_list * 	wwwServers;
    struct in_addr_list * 	fingerServers;
    struct in_addr_list * 	ircServers;
    struct in_addr_list * 	stServers;
    struct in_addr_list * 	stdaServers;
    };

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 0					/* turn off */
#endif  /* CPU_FAMILY==I960 */

#define VM_RFC1048      	{ 99, 130, 83, 99 }


#define TAG_PAD 			0 	/* RFC 1048 vendor tags */
#define TAG_SUBNET_MASK 		1
#define TAG_TIME_OFFSET 		2
#define TAG_GATEWAY 			3
#define TAG_TIME_SERVER 		4
#define TAG_NAME_SERVER 		5
#define TAG_DNS_SERVER 			6
#define TAG_LOG_SERVER 			7
#define TAG_COOKIE_SERVER 		8
#define TAG_LPR_SERVER 			9
#define TAG_IMPRESS_SERVER 		10
#define TAG_RLP_SERVER 			11
#define TAG_HOSTNAME 			12
#define TAG_END                 	255
#define TAG_BOOTSIZE 			13 	/* RFC 1084 vendor tags */
#define TAG_MERIT_DUMP 			14 	/* RFC 1395 vendor tags */
#define TAG_DNS_DOMAIN 			15
#define TAG_SWAP_SERVER 		16
#define TAG_ROOT_PATH 			17
#define TAG_EXTENSIONS_PATH 		18 	/* RFC 1497 vendor tags */
#define TAG_IP_FORWARD 			19 	/* RFC 1533 vendor tags */
#define TAG_NONLOCAL_SRCROUTE 		20
#define TAG_POLICY_FILTER 		21
#define TAG_MAX_DGRAM_SIZE 		22
#define TAG_DEFAULT_IP_TTL 		23
#define TAG_MTU_AGING_TIMEOUT 		24
#define TAG_MTU_PLATEAU_TABLE 		25
#define TAG_IF_MTU 			26
#define TAG_ALL_SUBNET_LOCAL 		27
#define TAG_BRDCAST_ADDR 		28
#define TAG_MASK_DISCOVER 		29
#define TAG_MASK_SUPPLIER 		30
#define TAG_ROUTER_DISCOVER 		31
#define TAG_ROUTER_SOLICIT 		32
#define TAG_STATIC_ROUTE 		33
#define TAG_TRAILER 			34
#define TAG_ARP_CACHE_TIMEOUT 		35
#define TAG_ETHER_ENCAP 		36
#define TAG_DEFAULT_TCP_TTL 		37
#define TAG_KEEPALIVE_INTER 		38
#define TAG_KEEPALIVE_GARBA 		39
#define TAG_NIS_DOMAIN 			40
#define TAG_NIS_SERVER 			41
#define TAG_NTP_SERVER 			42
#define TAG_VENDOR_SPEC 		43
#define TAG_NBN_SERVER 			44
#define TAG_NBDD_SERVER 		45
#define TAG_NB_NODETYPE 		46
#define TAG_NB_SCOPE 			47
#define TAG_XFONT_SERVER 		48
#define TAG_XDISPLAY_MANAGER 		49
#define TAG_NISP_DOMAIN 		64
#define TAG_NISP_SERVER 		65
#define TAG_MOBILEIP_HA 		68
#define TAG_SMTP_SERVER 		69
#define TAG_POP3_SERVER 		70
#define TAG_NNTP_SERVER 		71
#define TAG_WWW_SERVER 			72
#define TAG_FINGER_SERVER 		73
#define TAG_IRC_SERVER 			74
#define TAG_ST_SERVER 			75
#define TAG_STDA_SERVER 		76

/* error values */

#define S_bootpLib_INVALID_ARGUMENT	(M_bootpLib | 1)
#define S_bootpLib_INVALID_COOKIE	(M_bootpLib | 2)
#define S_bootpLib_NO_BROADCASTS	(M_bootpLib | 3)
#define S_bootpLib_PARSE_ERROR		(M_bootpLib | 4)
#define S_bootpLib_INVALID_TAG		(M_bootpLib | 5)
#define S_bootpLib_TIME_OUT		(M_bootpLib | 6)
#define S_bootpLib_MEM_ERROR		(M_bootpLib | 7)
#define S_bootpLib_NOT_INITIALIZED 	(M_bootpLib | 8)
#define S_bootpLib_BAD_DEVICE 		(M_bootpLib | 9)

/* function prototypes */

#if defined(__STDC__) || defined(__cplusplus)
STATUS 	bootpLibInit (int);
STATUS 	bootpLibMultiInit (ULONG, int);
STATUS 	bootpParamsGet (struct ifnet *, u_int, struct in_addr *,
                        struct in_addr *, char *, char *,
                        struct bootpParams *);
STATUS 	bootpMsgGet (struct ifnet *, struct in_addr *, USHORT, USHORT,
                     BOOTP_MSG *, u_int);
#else 	/* __STDC__ */

STATUS 		bootpLibInit ();
STATUS 		bootpLibMultiInit ();
STATUS		bootpParamsGet ();
STATUS		bootpMsgGet ();

#endif 	/* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INCbootpLibh */
