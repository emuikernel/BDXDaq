/* database.h -  DHCP server include file for database functions */

/* Copyright 1984 - 2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01d,09oct01,rae  merge from truestack (remove some variables) 
01c,06aug97,spm  added definitions for C++ compilation
01b,09may97,spm  corrected declarations of hooks for storage routines
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

#ifndef __INCdatabaseh
#define __INCdatabaseh

#ifdef __cplusplus
extern "C" {
#endif

/*
 * functions prototype definition
 */
void dump_bind_entry();
static time_t strtotime();
int bindcidcmp();
int paramcidcmp();
int resipcmp();
int relayipcmp();
time_t _mktime();

static void read_entry();
int process_entry();
void set_default(struct dhcp_resource *);
static void adjust();
static void get_string();
int eval_symbol();
static void eat_whitespace();
static long get_integer();
static int prs_inaddr();
static STATUS get_ip();
static void default_netmask();
int resnmcmp();
static int read_idtype();
static int read_cid();
static int read_haddr();
static int read_subnet();

int proc_sname();
int proc_file();
int proc_tblc();
int proc_mtpt();
int proc_clid();
int proc_class();
int proc_ip();
int proc_ips();
int proc_ippairs();
int proc_hl();
int proc_hs();
int proc_nl();
int proc_ns();
int proc_octet();
int proc_str();
int proc_bool();

#define SUCCESS	 		  0
#define E_END_OF_ENTRY		  1

#define MAXENTRYLEN             4096
#define MAXSTRINGLEN             260
#define MAX_MTUPLTSZ             127
#define MAX_IPS                   63
#define MAX_IPPAIR                31

#define OP_ADDITION		  1	/* Operations on tags */
#define OP_DELETION		  2

#ifndef BUFSIZ
#define BUFSIZ                  1024
#endif

IMPORT FUNCPTR dhcpsLeaseHookRtn;
IMPORT FUNCPTR dhcpsAddressHookRtn;

struct symbolmap {
    char *symbol;
    int code;
    int (*func)();
};

static struct symbolmap symbol_list[] = {
  { "tblc", S_TABLE_CONT, proc_tblc },
  { "snam", S_SNAME, proc_sname },
  { "file", S_FILE, proc_file },
  { "siad", S_SIADDR, proc_ip },
  { "albp", S_ALLOW_BOOTP, proc_bool },
  { "ipad", S_IP_ADDR, proc_ip },
  { "maxl", S_MAX_LEASE, proc_hl },
  { "dfll", S_DEFAULT_LEASE, proc_hl },
  { "clid", S_CLIENT_ID, proc_clid },
  { "pmid", S_PARAM_ID, proc_clid},
  { "clas", S_CLASS_ID, proc_class},
  { "snmk", S_SUBNET_MASK, proc_ip },
  { "tmof", S_TIME_OFFSET, proc_nl },
  { "rout", S_ROUTER, proc_ips },
  { "tmsv", S_TIME_SERVER, proc_ips },
  { "nmsv", S_NAME_SERVER, proc_ips },
  { "dnsv", S_DNS_SERVER, proc_ips },
  { "lgsv", S_LOG_SERVER, proc_ips },
  { "cksv", S_COOKIE_SERVER, proc_ips },
  { "lpsv", S_LPR_SERVER, proc_ips },
  { "imsv", S_IMPRESS_SERVER, proc_ips },
  { "rlsv", S_RLS_SERVER, proc_ips },
  { "hstn", S_HOSTNAME, proc_str },
  { "btsz", S_BOOTSIZE, proc_ns },
  { "mdmp", S_MERIT_DUMP, proc_str },
  { "dnsd", S_DNS_DOMAIN, proc_str },
  { "swsv", S_SWAP_SERVER, proc_ip },
  { "rpth", S_ROOT_PATH, proc_str },
  { "epth", S_EXTENSIONS_PATH, proc_str },
  { "ipfd", S_IP_FORWARD, proc_bool },
  { "nlsr", S_NONLOCAL_SRCROUTE, proc_bool },
  { "plcy", S_POLICY_FILTER, proc_ippairs },
  { "mdgs", S_MAX_DGRAM_SIZE, proc_ns },
  { "ditl", S_DEFAULT_IP_TTL, proc_octet },
  { "mtat", S_MTU_AGING_TIMEOUT, proc_nl },
  { "mtpt", S_MTU_PLATEAU_TABLE, proc_mtpt },
  { "ifmt", S_IF_MTU, proc_ns },
  { "asnl", S_ALL_SUBNET_LOCAL, proc_bool },
  { "brda", S_BRDCAST_ADDR, proc_ip },
  { "mskd", S_MASK_DISCOVER, proc_bool },
  { "msks", S_MASK_SUPPLIER, proc_bool },
  { "rtrd", S_ROUTER_DISCOVER, proc_bool },
  { "rtsl", S_ROUTER_SOLICIT, proc_ip },
  { "strt", S_STATIC_ROUTE, proc_ippairs },
  { "trlr", S_TRAILER, proc_bool },
  { "arpt", S_ARP_CACHE_TIMEOUT, proc_nl },
  { "encp", S_ETHER_ENCAP, proc_bool },
  { "dttl", S_DEFAULT_TCP_TTL, proc_octet },
  { "kain", S_KEEPALIVE_INTER, proc_nl },
  { "kagb", S_KEEPALIVE_GARBA, proc_bool },
  { "nisd", S_NIS_DOMAIN, proc_str },
  { "nisv", S_NIS_SERVER, proc_ips },
  { "ntsv", S_NTP_SERVER, proc_ips },
  { "nnsv", S_NBN_SERVER, proc_ips },
  { "ndsv", S_NBDD_SERVER, proc_ips },
  { "nbnt", S_NB_NODETYPE, proc_octet },
  { "nbsc", S_NB_SCOPE, proc_str },
  { "xfsv", S_XFONT_SERVER, proc_ips },
  { "xdmn", S_XDISPLAY_MANAGER, proc_ips },
  { "dht1", S_DHCP_T1, proc_hs },
  { "dht2", S_DHCP_T2, proc_hs },
  { "nspd", S_NISP_DOMAIN, proc_str },
  { "nsps", S_NISP_SERVER, proc_ips },
  { "miph", S_MOBILEIP_HA, proc_ips },
  { "smtp", S_SMTP_SERVER, proc_ips },
  { "pops", S_POP3_SERVER, proc_ips },
  { "nntp", S_NNTP_SERVER, proc_ips },
  { "wwws", S_DFLT_WWW_SERVER, proc_ips },
  { "fngs", S_DFLT_FINGER_SERVER, proc_ips },
  { "ircs", S_DFLT_IRC_SERVER, proc_ips },
  { "stsv", S_STREETTALK_SERVER, proc_ips },
  { "stda", S_STDA_SERVER, proc_ips }
};

#ifdef __cplusplus
}
#endif

#endif
