/* dhcp.h - Common DHCP include file for server, relay agent, and client */

/* Copyright 1984 - 2001 Wind River Systems, Inc. */

/*
modification history
---------------------
01j,23apr02,wap  Add prototype for dhcpTime() (SPR #68900)
01i,27jun01,ppp  removed MIN LEASE since it is not supported any more (SPR
                 #34799)
01h,07sep00,spm  moved client-specific transmit routine to private module
01g,05apr00,spm  added entries for boot-time client shared code
01f,24nov99,spm  upgraded to RFC 2131 and removed direct link-level access
01e,04dec97,spm  added code review modifications
01d,06aug97,spm  added definitions for C++ compilation
01c,02jun97,spm  changed DHCP option tags to prevent name conflicts (SPR #8667)
01b,29jan97,spm  added little-endian support and additional DHCP defines.
01a,03oct96,spm  created by modifying WIDE project DHCP Implementation.
*/

#ifndef __INCdhcph
#define __INCdhcph

#ifdef __cplusplus
extern "C" {
#endif

/*
 * WIDE Project DHCP Implementation
 * Copyright (c) 1995 Akihiro Tominaga
 * Copyright (c) 1995 WIDE Project
 * All rights reserved.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided only with the following
 * conditions are satisfied:
 *
 * 1. Both the copyright notice and this permission notice appear in
 *    all copies of the software, derivative works or modified versions,
 *    and any portions thereof, and that both notices appear in
 *    supporting documentation.
 * 2. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by WIDE Project and
 *      its contributors.
 * 3. Neither the name of WIDE Project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE DEVELOPER ``AS IS'' AND WIDE
 * PROJECT DISCLAIMS ANY LIABILITY OF ANY KIND FOR ANY DAMAGES
 * WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE. ALSO, THERE
 * IS NO WARRANTY IMPLIED OR OTHERWISE, NOR IS SUPPORT PROVIDED.
 *
 * Feedback of the results generated from any improvements or
 * extensions made to this software would be much appreciated.
 * Any such feedback should be sent to:
 * 
 *  Akihiro Tominaga
 *  WIDE Project
 *  Keio University, Endo 5322, Kanagawa, Japan
 *  (E-mail: dhcp-dist@wide.ad.jp)
 *
 * WIDE project has the rights to redistribute these changes.
 */

#include "netinet/in.h"

/* Tag values for DHCP options. */

#define _DHCP_PAD_TAG 			((char)   0)
#define _DHCP_SUBNET_MASK_TAG 		((char)   1)
#define _DHCP_TIME_OFFSET_TAG 		((char)   2)
#define _DHCP_ROUTER_TAG 		((char)   3)
#define _DHCP_TIME_SERVER_TAG 		((char)   4)
#define _DHCP_NAME_SERVER_TAG 		((char)   5)
#define _DHCP_DNS_SERVER_TAG 		((char)   6)
#define _DHCP_LOG_SERVER_TAG 		((char)   7)
#define _DHCP_COOKIE_SERVER_TAG 	((char)   8)
#define _DHCP_LPR_SERVER_TAG		((char)   9)
#define _DHCP_IMPRESS_SERVER_TAG 	((char)  10)
#define _DHCP_RLS_SERVER_TAG 		((char)  11)
#define _DHCP_HOSTNAME_TAG              ((char)  12)
#define _DHCP_BOOTSIZE_TAG 		((char)  13)
#define _DHCP_MERIT_DUMP_TAG 		((char)  14)
#define _DHCP_DNS_DOMAIN_TAG 		((char)  15)
#define _DHCP_SWAP_SERVER_TAG 		((char)  16)
#define _DHCP_ROOT_PATH_TAG 		((char)  17)
#define _DHCP_EXTENSIONS_PATH_TAG 	((char)  18)
#define _DHCP_IP_FORWARD_TAG 		((char)  19)
#define _DHCP_NONLOCAL_SRCROUTE_TAG 	((char)  20)
#define _DHCP_POLICY_FILTER_TAG 	((char)  21)
#define _DHCP_MAX_DGRAM_SIZE_TAG 	((char)  22)
#define _DHCP_DEFAULT_IP_TTL_TAG 	((char)  23)
#define _DHCP_MTU_AGING_TIMEOUT_TAG 	((char)  24)
#define _DHCP_MTU_PLATEAU_TABLE_TAG 	((char)  25)
#define _DHCP_IF_MTU_TAG 		((char)  26)
#define _DHCP_ALL_SUBNET_LOCAL_TAG 	((char)  27)
#define _DHCP_BRDCAST_ADDR_TAG 		((char)  28)
#define _DHCP_MASK_DISCOVER_TAG 	((char)  29)
#define _DHCP_MASK_SUPPLIER_TAG 	((char)  30)
#define _DHCP_ROUTER_DISCOVER_TAG 	((char)  31)
#define _DHCP_ROUTER_SOLICIT_TAG 	((char)  32)
#define _DHCP_STATIC_ROUTE_TAG 		((char)  33)
#define _DHCP_TRAILER_TAG 		((char)  34)
#define _DHCP_ARP_CACHE_TIMEOUT_TAG 	((char)  35)
#define _DHCP_ETHER_ENCAP_TAG 		((char)  36)
#define _DHCP_DEFAULT_TCP_TTL_TAG 	((char)  37)
#define _DHCP_KEEPALIVE_INTERVAL_TAG 	((char)  38)
#define _DHCP_KEEPALIVE_GARBAGE_TAG 	((char)  39)
#define _DHCP_NIS_DOMAIN_TAG 		((char)  40)
#define _DHCP_NIS_SERVER_TAG 		((char)  41)
#define _DHCP_NTP_SERVER_TAG 		((char)  42)
#define _DHCP_VENDOR_SPEC_TAG 		((char)  43)
#define _DHCP_NBN_SERVER_TAG 		((char)  44)
#define _DHCP_NBDD_SERVER_TAG 		((char)  45)
#define _DHCP_NB_NODETYPE_TAG 		((char)  46)
#define _DHCP_NB_SCOPE_TAG 		((char)  47)
#define _DHCP_XFONT_SERVER_TAG 		((char)  48)
#define _DHCP_XDISPLAY_MANAGER_TAG 	((char)  49)
#define _DHCP_REQUEST_IPADDR_TAG 	((char)  50)
#define _DHCP_LEASE_TIME_TAG 		((char)  51)
#define _DHCP_OPT_OVERLOAD_TAG 		((char)  52)
#define _DHCP_MSGTYPE_TAG 		((char)  53)
#define _DHCP_SERVER_ID_TAG 		((char)  54)
#define _DHCP_REQ_LIST_TAG 		((char)  55)
#define _DHCP_ERRMSG_TAG 		((char)  56)
#define _DHCP_MAXMSGSIZE_TAG 		((char)  57)
#define _DHCP_T1_TAG 			((char)  58) 
#define _DHCP_T2_TAG 			((char)  59)
#define _DHCP_CLASS_ID_TAG 		((char)  60)
#define _DHCP_CLIENT_ID_TAG 		((char)  61)
#define _DHCP_NISP_DOMAIN_TAG 		((char)  64)
#define _DHCP_NISP_SERVER_TAG 		((char)  65)
#define _DHCP_TFTP_SERVERNAME_TAG 	((char)  66)
#define _DHCP_BOOTFILE_TAG 		((char)  67)
#define _DHCP_MOBILEIP_HA_TAG 		((char)  68)
#define _DHCP_SMTP_SERVER_TAG 		((char)  69)
#define _DHCP_POP3_SERVER_TAG 		((char)  70)
#define _DHCP_NNTP_SERVER_TAG 		((char)  71)
#define _DHCP_DFLT_WWW_SERVER_TAG 	((char)  72)
#define _DHCP_DFLT_FINGER_SERVER_TAG 	((char)  73)
#define _DHCP_DFLT_IRC_SERVER_TAG 	((char)  74)
#define _DHCP_STREETTALK_SERVER_TAG 	((char)  75)
#define _DHCP_STDA_SERVER_TAG 		((char)  76)
#define _DHCP_END_TAG 			((char) 255)

#define _DHCP_LAST_OPTION 		_DHCP_STDA_SERVER_TAG
#define _DHCP_MAX_OPTLEN 		255

/* 
 * Hardware type values from the arp section of the assigned numbers RFC. 
 * (Frequently used as client identifier types).
 */

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
#define ULTRALINK 	13 	/* Ultra link */
#define FRAMERELAY 	15 	/* Frame Relay */
#define SERIAL 		20 	/* Serial Line */
#define DNS_NAME        128     /* DNS name */

/* Definitions for DHCP message format. */

#define MAX_HLEN      16    /* maximum length of haddr field */
#define MAX_SNAME     64
#define MAX_FILE     128
#define DFLTOPTLEN   312
#define BOOTPOPTLEN   64
#define MAXOPT      0xff
#define INFINITY    0xffffffff

/* Definitions for ARP messages. */

#define MAX_ARPLEN    (sizeof (struct arphdr) + 2 * (MAX_HLEN + 4))
#define ARPHL    (sizeof (struct arphdr))

#if CPU_FAMILY==I960
#pragma align 1                 /* tell gcc960 not to optimize alignments */
#endif  /* CPU_FAMILY==I960 */

/* DHCP message format */

struct dhcp
    {
    unsigned char       op;             /* packet type */
    unsigned char       htype;          /* Link-level interface address type */
    unsigned char       hlen;   /* Link-level interface address length */
    unsigned char       hops;           /* Hops through DHCP relay agents */
    unsigned long       xid;            /* transaction ID */
    unsigned short      secs;   /* seconds since lease negotiation started */
    unsigned short      flags;
    struct in_addr      ciaddr;         /* Client's current IP address */
    struct in_addr      yiaddr;         /* Client's assigned IP address */
    struct in_addr      siaddr;         /* Address of (tftp) server */
    struct in_addr      giaddr;         /* Address of relay agent */
    char                chaddr [MAX_HLEN];      /* Client's hardware address */
    char                sname [MAX_SNAME];      /* Host name of DHCP server */
    char                file [MAX_FILE];        /* File name of boot image */
    char                options [DFLTOPTLEN];   /* DHCP message options */
    };

struct ps_udph          /* pseudo udp header for checksum */
    {
    struct in_addr      srcip;
    struct in_addr      dstip;
    char                zero;
    char                prto;
    short               ulen;
    };

#if CPU_FAMILY==I960
#pragma align 0                 /* turn off alignment requirement */
#endif  /* CPU_FAMILY==I960 */

/* Definitions for processing incoming DHCP messages */

#define DHCP_MSG_SIZE (DFLTDHCPLEN + UDPHL + IPHL)
#define MESSAGE_RING_SIZE (10 * DHCP_MSG_SIZE)
#define DFLTDHCPLEN   sizeof (struct dhcp)   /* default DHCP message size */
#define DFLTBOOTPLEN  (DFLTDHCPLEN - DFLTOPTLEN + BOOTPOPTLEN)
#define DHCPLEN(UDP)  (ntohs(UDP->uh_ulen) - UDPHL)  /* get DHCP msg size */
                                                     /* from UDP header */
#define OPTBODY(TAGP)  ( ( (char *)TAGP) + 2)   /* get DHCP option contents */
#define DHCPOPTLEN(TAGP) ((UINT8) * ( ((char *)TAGP) + 1)) /* option length */

/* handle the word alignment */
#define   GETHS(PTR)  (*((u_char *)PTR)*256 + *(((u_char *)PTR)+1))
#define   GETHL(PTR)  (*((u_char *)PTR)*256*256*256 + *(((u_char *)PTR)+1)*256*256 +\
                       *(((u_char *)PTR)+2)*256 + *(((u_char *)PTR)+3))

/* Bit operators for options flags. */

#define SETBIT(a,i)   ((a)[(i) / 8] |= (1 << (7 - (i) % 8)))
#define CLRBIT(a,i)   ((a)[(i) / 8] &= ~(1 << (7 - (i) % 8)))
#define ISSET(a,i)    ((a)[(i) / 8] & (1 << (7 - (i) % 8)))
#define ISCLR(a,i)    (((a)[(i) / 8] & (1 << (7 - (i) % 8))) == 0)

#define BOOTREQUEST		1
#define BOOTREPLY		2


/*
 * DHCP/BOOTP magic cookie defined in RFC 1048
 */

#define RFC1048_MAGIC { 99, 130, 83, 99 }
#define MAGIC_LEN  4

/* DHCP message types */

#define DHCPDISCOVER           1
#define DHCPOFFER              2
#define DHCPREQUEST            3
#define DHCPDECLINE            4
#define DHCPACK                5
#define DHCPNAK                6
#define DHCPRELEASE            7
#define DHCPINFORM             8
#define BOOTP                  0

/* Classification of DHCP message fields */

#define FILE_ISOPT             1
#define SNAME_ISOPT            2
#define BOTH_AREOPT            FILE_ISOPT + SNAME_ISOPT

/* Common routines for all DHCP components */

IMPORT int dhcpConvert (int);
IMPORT char * pickup_opt (struct dhcp *, int, char);
IMPORT u_short udp_cksum (struct ps_udph *, char *, int);
IMPORT time_t dhcpTime (time_t *);

#ifdef __cplusplus
}
#endif

#endif
