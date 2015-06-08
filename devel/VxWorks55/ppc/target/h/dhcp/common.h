/* common.h - DHCP server include file for shared data structures */

/* Copyright 1984 - 2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01c,17jan03,wap  Avoid compiler warning due to redefinition of IPHL/UDPHL
                 macros. (SPR #83244)
01b,12dec01,wap  Interface name field of if_info structure is too small for
                 some interface names (SPR #29087)
01a,09oct01,rae  merge from truestack ver 01h, base 01e, minor fixes

01e,14jul00,spm  upgraded to RFC 2131 and removed direct link-level access
01d,06oct97,spm  added support for dynamic driver type detection
01c,06aug97,spm  added definitions for C++ compilation
01b,06may97,spm  changed memory access to align IP header on four byte boundary
01a,07apr97,spm  created by modifying WIDE project DHCP implementation
*/

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

#ifndef __INCcommonh
#define __INCcommonh

#ifdef __cplusplus
extern "C" {
#endif

/* Resources sent to client outside of options field, if at all. */

#define S_TABLE_CONT          0
#define S_SNAME               1
#define S_FILE                2
#define S_SIADDR              3
#define S_ALLOW_BOOTP         4
#define S_IP_ADDR             5
#define S_MAX_LEASE           6
#define S_DEFAULT_LEASE       7
#define S_CLIENT_ID           8
#define S_PARAM_ID            9
#define S_CLASS_ID           10

/* Resources sent to client as RFC 1533 options. */

#define S_SUBNET_MASK        11
#define S_TIME_OFFSET        12
#define S_ROUTER             13
#define S_TIME_SERVER        14
#define S_NAME_SERVER        15
#define S_DNS_SERVER         16
#define S_LOG_SERVER         17
#define S_COOKIE_SERVER      18
#define S_LPR_SERVER         19
#define S_IMPRESS_SERVER     20
#define S_RLS_SERVER         21
#define S_HOSTNAME           22
#define S_BOOTSIZE           23
#define S_MERIT_DUMP         24
#define S_DNS_DOMAIN         25
#define S_SWAP_SERVER        26
#define S_ROOT_PATH          27
#define S_EXTENSIONS_PATH    28
#define S_IP_FORWARD         29
#define S_NONLOCAL_SRCROUTE  30
#define S_POLICY_FILTER      31
#define S_MAX_DGRAM_SIZE     32
#define S_DEFAULT_IP_TTL     33
#define S_MTU_AGING_TIMEOUT  34
#define S_MTU_PLATEAU_TABLE  35
#define S_IF_MTU             36
#define S_ALL_SUBNET_LOCAL   37
#define S_BRDCAST_ADDR       38
#define S_MASK_DISCOVER      39
#define S_MASK_SUPPLIER      40
#define S_ROUTER_DISCOVER    41
#define S_ROUTER_SOLICIT     42
#define S_STATIC_ROUTE       43
#define S_TRAILER            44
#define S_ARP_CACHE_TIMEOUT  45
#define S_ETHER_ENCAP        46
#define S_DEFAULT_TCP_TTL    47
#define S_KEEPALIVE_INTER    48
#define S_KEEPALIVE_GARBA    49
#define S_NIS_DOMAIN         50
#define S_NIS_SERVER         51
#define S_NTP_SERVER         52
#define S_NBN_SERVER         53
#define S_NBDD_SERVER        54
#define S_NB_NODETYPE        55
#define S_NB_SCOPE           56
#define S_XFONT_SERVER       57
#define S_XDISPLAY_MANAGER   58
#define S_DHCP_T1            59
#define S_DHCP_T2            60
#define S_NISP_DOMAIN        61
#define S_NISP_SERVER        62
#define S_MOBILEIP_HA        63
#define S_SMTP_SERVER        64
#define S_POP3_SERVER        65
#define S_NNTP_SERVER        66
#define S_DFLT_WWW_SERVER    67
#define S_DFLT_FINGER_SERVER 68
#define S_DFLT_IRC_SERVER    69
#define S_STREETTALK_SERVER  70
#define S_STDA_SERVER        71

#define S_LAST_OPTION        S_STDA_SERVER
#define VALIDSIZE   ((S_LAST_OPTION / 8) + 1)

/* Set length for entry names. Leave space to append unique value. */

#define BASE_NAME 	8   /* length for user-provided name, excluding '\0' */
#define MAX_NAME 	BASE_NAME + 2 * sizeof (u_long)

#include <netinet/udp.h>
#include <netinet/ip.h>
#include <netinet/if_ether.h>

#define DHCPS_OFF 2      /* 4-byte alignment offset needed by Sun BSP's */
#define ETHERHL  sizeof(struct ether_header)  /* ethernet header length */

#ifndef IPHL
#define IPHL     sizeof(struct ip)            /* IP header length */
#endif

#ifndef UDPHL
#define UDPHL    sizeof(struct udphdr)        /* UDP header length */
#endif

#define WORD     4                            /* word alignment in bits */

#if CPU_FAMILY==I960
#pragma align 1                 /* tell gcc960 not to optimize alignments */
#endif  /* CPU_FAMILY==I960 */

struct msg
{
  struct ether_header *ether;
  struct ip *ip;
  struct udphdr *udp;
  struct dhcp *dhcp;
};

struct if_info {
  int fd;
  char name[IFNAMSIZ];
  short unit;
  int bpfDev;   /* File descriptor for Berkeley Packet Filter */
  int mtuSize;
  unsigned int htype;
  unsigned char hlen;
  char haddr[6];
  struct in_addr ipaddr;
  struct in_addr subnetmask;
  int buf_size;
  char *buf;
  struct if_info *next;	             /* Next entry in null terminated list. */
};

struct relay_acl {
  struct in_addr relay,
                 subnet_mask;
};

struct client_id {
  unsigned char   idtype,
                  idlen;
  char            id [MAXOPT];
  struct in_addr  subnet;
};

struct u_shorts {
  int num;
  unsigned short *shorts;
};

struct in_addrs {
  int num;
  struct in_addr *addr;
};

struct ip_pairs {
  int num;
  struct in_addr *addr1,
                 *addr2;
};

struct chaddr {
  unsigned char  htype,
                 hlen;
  char           haddr [MAX_HLEN];
};

struct dhcp_binding {

  /* external info */
  struct client_id           cid;           /* hashing key. client identifier */
  struct chaddr              haddr;
  time_t                     expire_epoch;  /* lease expiration epoch */
  char                       res_name [MAX_NAME + 1]; /* name of resource */

  /* internal info */
  char                       flag;
#define COMPLETE_ENTRY       0x1         /* complete entry */
#define STATIC_ENTRY         0x2         /* static entry */
#define BOOTP_ENTRY          0x4         /* bootp entry */
  time_t                     temp_epoch;        /* memo expiration epoch */
  struct dhcp_resource       *res;              /* pointer to resource */
};

struct dhcp_resource {

  /* external info */
  char                      allow_bootp,         /* boolean */
                            ip_forward,          /* boolean */
                            nonlocal_srcroute,   /* boolean */
                            all_subnet_local,    /* boolean */
                            mask_discover,       /* boolean */
                            mask_supplier,       /* boolean */
                            router_discover,     /* boolean */
                            trailer,             /* boolean */
                            ether_encap,         /* boolean */
                            keepalive_garba,     /* boolean */
                            entryname [MAX_NAME + 1],  /* tag name of entry */
                            sname [MAX_SNAME + 1], /* server host name */
                            file [MAX_FILE + 1], /* boot file name */
                            hostname [MAXOPT + 1],
                            merit_dump [MAXOPT + 1],
                            dns_domain [MAXOPT + 1],
                            root_path [MAXOPT + 1],
                            extensions_path [MAXOPT + 1],
                            nis_domain [MAXOPT + 1],
                            nb_scope [MAXOPT + 1], /* NetBIOS scope */
                            nisp_domain [MAXOPT + 1]; /* NIS+ domain name */
  unsigned char             default_ip_ttl,       /* time to live (1 - 255) */
                            default_tcp_ttl,
                            nb_nodetype;         /* NetBIOS node type */
#define BNODE   0x1
#define PNODE   0x2
#define MNODE   0x4
#define HNODE   0x8
  unsigned short            bootsize,            /* bootfile size in 16 bits */
                            max_dgram_size,      /* max dgram reassembly */
                            intf_mtu,            /* interface mtu */
                            dhcp_t1,             /* host byte order */
                            dhcp_t2;             /* host byte order */
  long                      time_offset;         /* offset from UTC */
  unsigned long             max_lease,           /* maximum lease duration
						    (host byte order) */
                            default_lease,       /* default lease duration
						    (host byte order) */
                            mtu_aging_timeout,   /* unsigned 32 bits integer */
                            arp_cache_timeout,
                            keepalive_inter;
  struct u_shorts           mtu_plateau_table;   /* array of 16 bit ints */
  struct in_addr            siaddr,              /* (tftp) server */
                            ip_addr,             /* IP address */
                            subnet_mask,
                            swap_server,
                            brdcast_addr,
                            router_solicit;
  struct in_addrs           router,
                            time_server,
                            name_server,
                            dns_server,       /* DNS server */
                            log_server,
                            cookie_server,
                            lpr_server,
                            impress_server,
                            rls_server,       /* Resource Location Server */
                            nis_server,
                            ntp_server,
                            nbn_server,         /* NetBIOS name server */
                            nbdd_server,     /* NetBIOS dgram distrib server */
                            xfont_server,       /* X font erver */
                            xdisplay_manager,   /* X display manager */
                            nisp_server,        /* NIS+ server */
                            mobileip_ha,        /* Mobile IP Home Agent */
                            smtp_server,        /* SMTP server */
                            pop3_server,        /* POP3 server */
                            nntp_server,        /* NNTP server */
                            dflt_www_server,    /* Default WWW server */
                            dflt_finger_server, /* Default Finger server */
                            dflt_irc_server,    /* Default IRC server */
                            streettalk_server,  /* StreetTalk server */
                            stda_server;        /* STDA server */
  struct ip_pairs           policy_filter,   
                            static_route;

  /* internal info */
  char                      valid[VALIDSIZE],  /* indicate which are available */
                            active[VALIDSIZE]; /* indicate which differ from
						  Host Requirements RFC */
  struct dhcp_binding       *binding;            /* pointer to identifier
                                                  * associated with resource. 
                                                  */
};

#if CPU_FAMILY==I960
#pragma align 0                 /* turn off alignment requirement */
#endif  /* CPU_FAMILY==I960 */

#ifndef TRUE
#define TRUE   1
#endif
#ifndef FALSE
#define FALSE  0
#endif
#ifndef GOOD
#define GOOD   1
#endif
#ifndef BAD
#define BAD    0
#endif
#ifndef ADDRPOOL_DUMP
#define ADDRPOOL_DUMP           "/tmp/addrpool_dump"
#endif
#define MAX_NBIND       500   /* maximum number of dhcp_binding
				 ( this is used in garbage_collect(); ) */

#define MAXPATHLEN 64       /* Maximum length of database names. */

#ifndef VIRTUAL_STACK
extern int nbind;
#endif /* VIRTUAL_STACK */

#ifndef sun
extern struct bpf_hdr *rbpf;        /* pointer to bpf header */
#endif
/* extern struct msg rcv; */

int     add_bind();
STATUS  read_bind_db (void);
STATUS  read_addrpool_db (int);
void    read_relay_db (int);
void    dump_bind_db();
void    finish();
char    *haddrtos();
char    *cidtos();
char    *_ctime();

#ifdef __cplusplus
}
#endif

#endif
