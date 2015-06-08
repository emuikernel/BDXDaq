/* dhcps.h - DHCP server primary include file */

/* Copyright 1984 - 1997 Wind River Systems, Inc. */

/*
modification history
____________________
01b,06aug97,spm  added definitions for C++ compilation
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

#ifndef __INCdhcpsh
#define __INCdhcpsh

#ifdef __cplusplus
extern "C" {
#endif

#define GC_INTERVAL    600      /* interval between garbage collections */

/*
 *  function prototype definition
 */
static void garbage_collect (void);
static void get_cid (struct dhcp *, int, struct client_id *);
static void clean_sbuf (void);
static void construct_msg (u_char, struct dhcp_resource *,
                           u_long, struct if_info *);
static void turnoff_bind (struct dhcp_binding *);
static int nvttostr (char *, char *, int);
static int get_maxoptlen (struct dhcp *, int);
static int get_subnet (struct dhcp *, int, struct in_addr *, struct if_info *);
static int available_res (struct dhcp_resource *, struct client_id *, time_t);
static int choose_lease (int, time_t, struct dhcp_resource *);
static int update_db (int, struct client_id *, struct dhcp_resource *, 
                      u_long, time_t);
static int cidcmp (struct client_id *, struct client_id *);
static int icmp_check (int, struct in_addr *);
static int insert_it (char *);
static int free_bind (struct hash_member *);
static int free_fake (struct hash_member *);
static int send_dhcp (struct if_info *, int);
static int insert_opt (struct dhcp_resource *, u_long, int, char *, char);
#define PASSIVE 0
#define ACTIVE  1
static u_long get_reqlease (struct dhcp *, int);
static struct dhcp_resource *select_wciaddr (struct client_id *, time_t, int *);
static struct dhcp_resource *select_wcid (int, struct client_id *, time_t);
static struct dhcp_resource *select_wreqip (int, struct client_id *, time_t);
static struct dhcp_resource *select_newone (int, struct client_id *, time_t, 
                                            u_long);
static struct dhcp_resource *choose_res (struct client_id *, time_t, u_long);

extern struct hash_member  *reslist;
extern struct hash_member  *bindlist;
extern struct hash_tbl     cidhashtable;
extern struct hash_tbl     iphashtable;
extern struct hash_tbl     relayhashtable;
extern struct hash_tbl     paramhashtable;

extern int bindcidcmp();
extern int paramcidcmp();
extern int resipcmp();
extern int relayipcmp();
extern int open_if();

static int      discover();
static int      request();
static int      decline();
static int      release();

#ifndef NOBOOTP
static void construct_bootp();
static int available_forbootp();
static int send_bootp();
static struct dhcp_resource *choose_forbootp();
static int      bootp();
#endif /* NOBOOTP */

static int (*process_msg[])() = {
#ifdef NOBOOTP
  NULL,
#else
  bootp,
#endif
  discover,
  NULL,
  request,
  decline,
  NULL,
  NULL,
  release
};

static int  ins_ip (struct dhcp_resource *, u_long, int, char *, char);
static int  ins_ips (struct dhcp_resource *, u_long, int, char *, char);
static int  ins_ippairs (struct dhcp_resource *, u_long, int, char *, char);
static int  ins_long (struct dhcp_resource *, u_long, int, char *, char);
static int  ins_short (struct dhcp_resource *, u_long, int, char *, char);
static int  ins_octet (struct dhcp_resource *, u_long, int, char *, char);
static int  ins_str (struct dhcp_resource *, u_long, int, char *, char);
static int  ins_mtpt (struct dhcp_resource *, u_long, int, char *, char);
static int  ins_dht (struct dhcp_resource *, u_long, int, char *, char); 

static int (*ins_opt []) (struct dhcp_resource *, u_long, int, char *, char) = 
    {
  NULL,         /* PAD == 0 */
  ins_ip,       /* SUBNET_MASK */
  ins_long,     /* TIME_OFFSET */
  ins_ips,      /* ROUTER */
  ins_ips,      /* TIME_SERVER */
  ins_ips,      /* NAME_SERVER */
  ins_ips,      /* DNS_SERVER */
  ins_ips,      /* LOG_SERVER */
  ins_ips,      /* COOKIE_SERVER */
  ins_ips,      /* LPR_SERVER */
  ins_ips,      /* IMPRESS_SERVER */
  ins_ips,      /* RLS_SERVER */
  ins_str,      /* HOSTNAME */
  ins_short,    /* BOOTSIZE */
  ins_str,      /* MERIT_DUMP */
  ins_str,      /* DNS_DOMAIN */
  ins_ip,       /* SWAP_SERVER */
  ins_str,      /* ROOT_PATH */
  ins_str,      /* EXTENSIONS_PATH */
  ins_octet,    /* IP_FORWARD */
  ins_octet,    /* NONLOCAL_SRCROUTE */
  ins_ippairs,  /* POLICY_FILTER */
  ins_short,    /* MAX_DGRAM_SIZE */
  ins_octet,    /* DEFAULT_IP_TTL */
  ins_long,     /* MTU_AGING_TIMEOUT */
  ins_mtpt,     /* MTU_PLATEAU_TABLE */
  ins_short,    /* IF_MTU */
  ins_octet,    /* ALL_SUBNET_LOCAL */
  ins_ip,       /* BRDCAST_ADDR */
  ins_octet,    /* MASK_DISCOVER */
  ins_octet,    /* MASK_SUPPLIER */
  ins_octet,    /* ROUTER_DISCOVER */
  ins_ip,       /* ROUTER_SOLICIT */
  ins_ippairs,  /* STATIC_ROUTE */
  ins_octet,    /* TRAILER */
  ins_long,     /* ARP_CACHE_TIMEOUT */
  ins_octet,    /* ETHER_ENCAP */
  ins_octet,    /* DEFAULT_TCP_TTL */
  ins_long,     /* KEEPALIVE_INTER */
  ins_octet,    /* KEEPALIVE_GARBA */
  ins_str,      /* NIS_DOMAIN */
  ins_ips,      /* NIS_SERVER */
  ins_ips,      /* NTP_SERVER */
  NULL,         /* VENDOR_SPEC */
  ins_ips,      /* NBN_SERVER */
  ins_ips,      /* NBDD_SERVER */
  ins_octet,    /* NB_NODETYPE */
  ins_str,      /* NB_SCOPE */
  ins_ips,      /* XFONT_SERVER */
  ins_ips,      /* XDISPLAY_MANAGER */
  NULL,         /* REQUEST_IPADDR */
  NULL,         /* LEASE_TIME */
  NULL,         /* OPT_OVERLOAD */
  NULL,         /* DHCP_MSGTYPE */
  NULL,         /* SERVER_ID */
  NULL,         /* REQ_LIST */
  NULL,         /* DHCP_ERRMSG */
  NULL,         /* DHCP_MAXMSGSIZE */
  ins_dht,      /* DHCP_T1 */
  ins_dht,      /* DHCP_T2 */
  NULL,         /* CLASS_ID */
  NULL,         /* CLIENT_ID */
  NULL,
  NULL,
  ins_str,      /* NISP_DOMAIN */
  ins_ips,      /* NISP_SERVER */
  NULL,         /* TFTP_SERVERNAME */
  NULL,         /* BOOTFILE */
  ins_ips,      /* MOBILEIP_HA */
  ins_ips,      /* SMTP_SERVER */
  ins_ips,      /* POP3_SERVER */
  ins_ips,      /* NNTP_SERVER */
  ins_ips,      /* DFLT_WWW_SERVER */
  ins_ips,      /* DFLT_FINGER_SERVER */
  ins_ips,      /* DFLT_IRC_SERVER */
  ins_ips,      /* STREETTALK_SERVER */
  ins_ips       /* STDA_SERVER */
};

#ifdef __cplusplus
}
#endif

#endif
