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

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <syslog.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/param.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "dhcp.h"
#include "common.h"
#include "hash.h"
#include "database.h"

#ifdef solaris              /* Bit map macros */
#include <sys/fs/ufs_fs.h>
#endif

void
dump_bind_db()
{
  char *newdbname;
  struct hash_member *bindptr = NULL;

  if ((newdbname =
       calloc(1, strlen(binding_db) + sizeof(".bak") + 1)) == NULL) {
    syslog(LOG_WARNING, "calloc error in dump_bind_db()");
    return;
  }
  sprintf(newdbname, "%s.bak", binding_db);
  fclose(binding_dbfp);
  if (rename(binding_db, newdbname) != 0)
    syslog(LOG_WARNING, "rename error in dump_bind_db()");
  free(newdbname);
  if ((binding_dbfp = fopen(binding_db, "w+")) == NULL) {
    syslog(LOG_WARNING, "Cannot open the binding database \"%s\"", binding_db);
    return;
  }

  bindptr = bindlist;
  while (bindptr != NULL) {
    dump_bind_entry(bindptr->data);
    bindptr = bindptr->next;
  }
  fflush(binding_dbfp);
  return;
}


void
dump_bind_entry(bp)
  struct dhcp_binding *bp;
{
  char *tmp = NULL;

  /* non-complete/static entry should not dumped */
  if ((bp->flag & COMPLETE_ENTRY) == 0 || (bp->flag & STATIC_ENTRY) != 0)
    return;

  fprintf(binding_dbfp, "%s:", cidtos(&bp->cid, 1));
  fprintf(binding_dbfp, "%s:", haddrtos(&bp->haddr));
  if (bp->expire_epoch == 0xffffffff) {
    if (bp->flag & BOOTP_ENTRY)
      fprintf(binding_dbfp, "\"bootp\":");
    else
      fprintf(binding_dbfp, "\"infinity\":");
  }
  else if (bp->expire_epoch == 0){
    fprintf(binding_dbfp, "\"\":");
  } else {
#ifdef solaris
    tmp = (char *) ctime(&bp->expire_epoch);
#else
    tmp = (char *) _ctime(&bp->expire_epoch);
#endif
    tmp[strlen(tmp) - 1] = '\0';
    fprintf(binding_dbfp, "\"%s\":", tmp);
  }
  fprintf(binding_dbfp, "%s", bp->res_name);
  fprintf(binding_dbfp, "\n");

  fflush(binding_dbfp);
  return;
}


void
dump_addrpool_db()
{
  struct hash_member *resptr = NULL;

  if ((dump_fp = freopen(ADDRPOOL_DUMP, "w+", dump_fp)) == NULL) {
    syslog(LOG_WARNING,
	   "Cannot reopen the address pool dump file \"%s\"", ADDRPOOL_DUMP);
    return;
  }

  resptr = reslist;
  while (resptr != NULL) {
    dump_addrpool_entry(resptr->data);
    resptr = resptr->next;
  }
  fflush(dump_fp);
  return;
}


/*
 * print out true/false to the dump of the address pool database.
 */
static void
print_bool(symbol, bool)
  char *symbol;
  char bool;
{
  fprintf(dump_fp, "%s=%s:", symbol, (bool == TRUE) ? "true" : "false");  
}


/*
 * print out the IP address to the dump of the address pool database.
 */
static void
print_ip(symbol, addr)
  char *symbol;
  struct in_addr *addr;
{
  fprintf(dump_fp, "%s=%s:", symbol, inet_ntoa(*addr));
}


/*
 * print out the IP addresses to the dump of the address pool database.
 */
static void
print_ips(symbol, ips)
  char *symbol;
  struct in_addrs *ips;
{
  int i;

  fprintf(dump_fp, "%s=", symbol);
  for (i = 0; i < ips->num; i++) {
    fprintf(dump_fp, "%s", inet_ntoa(ips->addr[i]));
    if (i != ips->num - 1) fprintf(dump_fp, " ");
  }
  fprintf(dump_fp, ":");
}


/*
 * print out the IP addresses to the dump of the address pool database.
 */
static void
print_ippairs(symbol, pair)
  char *symbol;
  struct ip_pairs *pair;
{
  int i;

  fprintf(dump_fp, "%s=", symbol);
  for (i = 0; i < pair->num; i++) {
    fprintf(dump_fp, "%s ", inet_ntoa(pair->addr1[i]));
    fprintf(dump_fp, "%s", inet_ntoa(pair->addr2[i]));
    if (i != pair->num - 1) fprintf(dump_fp, " ");
  }
  fprintf(dump_fp, ":");
}


/*
 * print out the IP address to the dump of the address pool database.
 */
static void
print_str(symbol, str)
  char *symbol;
  char *str;
{
  fprintf(dump_fp, "%s=%s:", symbol, str);
}


void
dump_addrpool_entry(rp)
  struct dhcp_resource *rp;
{
  int i = 0;

  if (rp == NULL) {
    return;
  }

  fprintf(dump_fp, "%s:\t:", rp->entryname);
  if (isset(rp->active, S_SNAME)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_SNAME)) fprintf(dump_fp, "snam=%s:", rp->sname);
  if (isset(rp->active, S_FILE)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_FILE)) fprintf(dump_fp, "file=%s:", rp->file);
  if (isset(rp->active, S_SIADDR)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_SIADDR)) print_ip("siad", rp->siaddr);
  if (isset(rp->active, S_ALLOW_BOOTP)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_ALLOW_BOOTP)) print_bool("albp", rp->allow_bootp);
  if (isset(rp->active, S_IP_ADDR)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_IP_ADDR)) print_ip("ipad", rp->ip_addr);
  if (isset(rp->active, S_MAX_LEASE)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_MAX_LEASE)) fprintf(dump_fp, "maxl=%lu:", rp->max_lease);
  if (isset(rp->active, S_DEFAULT_LEASE)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_DEFAULT_LEASE))
      fprintf(dump_fp, "dfll=%lu:", rp->default_lease);
  if (rp->binding != NULL) {
    if (isset(rp->active, S_CLIENT_ID)) fprintf(dump_fp, "!");
    if (isset(rp->valid, S_CLIENT_ID))
      fprintf(dump_fp, "clid=\"%s\":", cidtos(&rp->binding->cid, 0));
  }
  if (isset(rp->active, S_SUBNET_MASK)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_SUBNET_MASK)) print_ip("snmk", rp->subnet_mask);
  if (isset(rp->active, S_TIME_OFFSET)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_TIME_OFFSET))
      fprintf(dump_fp, "tmof=%ld:", ntohl(rp->time_offset));
  if (isset(rp->active, S_ROUTER)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_ROUTER)) print_ips("rout", rp->router);
  if (isset(rp->active, S_TIME_SERVER)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_TIME_SERVER)) print_ips("tmsv", rp->time_server);
  if (isset(rp->active, S_NAME_SERVER)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_NAME_SERVER)) print_ips("nmsv", rp->name_server);
  if (isset(rp->active, S_DNS_SERVER)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_DNS_SERVER)) print_ips("dnsv", rp->dns_server);
  if (isset(rp->active, S_LOG_SERVER)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_LOG_SERVER)) print_ips("lgsv", rp->log_server);
  if (isset(rp->active, S_COOKIE_SERVER)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_COOKIE_SERVER)) print_ips("cksv", rp->cookie_server);
  if (isset(rp->active, S_LPR_SERVER)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_LPR_SERVER)) print_ips("lpsv", rp->lpr_server);
  if (isset(rp->active, S_IMPRESS_SERVER)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_IMPRESS_SERVER)) print_ips("imsv", rp->impress_server);
  if (isset(rp->active, S_RLS_SERVER)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_RLS_SERVER)) print_ips("rlsv", rp->rls_server);
  if (isset(rp->active, S_HOSTNAME)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_HOSTNAME)) print_str("hstn", rp->hostname);
  if (isset(rp->active, S_BOOTSIZE)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_BOOTSIZE))
      fprintf(dump_fp, "btsz=%u:", ntohs(rp->bootsize));
  if (isset(rp->active, S_MERIT_DUMP)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_MERIT_DUMP)) print_str("mdmp", rp->merit_dump);
  if (isset(rp->active, S_DNS_DOMAIN)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_DNS_DOMAIN)) print_str("dnsd", rp->dns_domain);
  if (isset(rp->active, S_SWAP_SERVER)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_SWAP_SERVER)) print_ip("swsv", rp->swap_server);
  if (isset(rp->active, S_ROOT_PATH)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_ROOT_PATH)) print_str("rpth", rp->root_path);
  if (isset(rp->active, S_EXTENSIONS_PATH)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_EXTENSIONS_PATH)) print_str("epth", rp->extensions_path);
  if (isset(rp->active, S_IP_FORWARD)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_IP_FORWARD)) print_bool("ipfd", rp->ip_forward);
  if (isset(rp->active, S_NONLOCAL_SRCROUTE)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_NONLOCAL_SRCROUTE))
      print_bool("nlsr", rp->nonlocal_srcroute);
  if (isset(rp->active, S_POLICY_FILTER)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_POLICY_FILTER)) print_ippairs("plcy", rp->policy_filter);
  if (isset(rp->active, S_MAX_DGRAM_SIZE)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_MAX_DGRAM_SIZE))
      fprintf(dump_fp, "mdgs=%u:", ntohs(rp->max_dgram_size));
  if (isset(rp->active, S_DEFAULT_IP_TTL)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_DEFAULT_IP_TTL))
      fprintf(dump_fp, "ditl=%u:", rp->default_ip_ttl);
  if (isset(rp->active, S_MTU_AGING_TIMEOUT)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_MTU_AGING_TIMEOUT))
      fprintf(dump_fp, "mtat=%lu:", ntohl(rp->mtu_aging_timeout));
  if (isset(rp->active, S_MTU_PLATEAU_TABLE)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_MTU_PLATEAU_TABLE)) {
    fprintf(dump_fp, "mtpt=");
    for (i = 0; i < rp->mtu_plateau_table->num; i++) {
      fprintf(dump_fp, "%u", ntohs(rp->mtu_plateau_table->shorts[i]));
      if (i != rp->mtu_plateau_table->num - 1) fprintf(dump_fp, " ");
    }
    fprintf(dump_fp, ":");
  }
  if (isset(rp->active, S_IF_MTU)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_IF_MTU)) fprintf(dump_fp, "ifmt=%u:", ntohs(rp->intf_mtu));
  if (isset(rp->active, S_ALL_SUBNET_LOCAL)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_ALL_SUBNET_LOCAL)) print_bool("asnl", rp->all_subnet_local);
  if (isset(rp->active, S_BRDCAST_ADDR)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_BRDCAST_ADDR)) print_ip("brda", rp->brdcast_addr);
  if (isset(rp->active, S_MASK_DISCOVER)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_MASK_DISCOVER)) print_bool("mskd", rp->mask_discover);
  if (isset(rp->active, S_MASK_SUPPLIER)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_MASK_SUPPLIER)) print_bool("msks", rp->mask_supplier);
  if (isset(rp->active, S_ROUTER_DISCOVER)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_ROUTER_DISCOVER)) print_bool("rtrd", rp->router_discover);
  if (isset(rp->active, S_ROUTER_SOLICIT)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_ROUTER_SOLICIT)) print_ip("rtsl", rp->router_solicit);
  if (isset(rp->active, S_STATIC_ROUTE)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_STATIC_ROUTE)) print_ippairs("strt", rp->static_route);
  if (isset(rp->active, S_TRAILER)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_TRAILER)) print_bool("trlr", rp->trailer);
  if (isset(rp->active, S_ARP_CACHE_TIMEOUT)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_ARP_CACHE_TIMEOUT))
      fprintf(dump_fp, "arpt=%lu:", ntohl(rp->arp_cache_timeout));
  if (isset(rp->active, S_ETHER_ENCAP)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_ETHER_ENCAP)) print_bool("encp", rp->ether_encap);
  if (isset(rp->active, S_DEFAULT_TCP_TTL)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_DEFAULT_TCP_TTL))
      fprintf(dump_fp, "dttl=%u:", rp->default_tcp_ttl);
  if (isset(rp->active, S_KEEPALIVE_INTER)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_KEEPALIVE_INTER))
      fprintf(dump_fp, "kain=%lu:", ntohl(rp->keepalive_inter));
  if (isset(rp->active, S_KEEPALIVE_GARBA)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_KEEPALIVE_GARBA)) print_bool("kagb", rp->keepalive_garba);
  if (isset(rp->active, S_NIS_DOMAIN)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_NIS_DOMAIN)) print_str("nisd", rp->nis_domain);
  if (isset(rp->active, S_NIS_SERVER)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_NIS_SERVER)) print_ips("nisv", rp->nis_server);
  if (isset(rp->active, S_NTP_SERVER)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_NTP_SERVER)) print_ips("ntsv", rp->ntp_server);
  if (isset(rp->active, S_NBN_SERVER)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_NBN_SERVER)) print_ips("nnsv", rp->nbn_server);
  if (isset(rp->active, S_NBDD_SERVER)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_NBDD_SERVER)) print_ips("ndsv", rp->nbdd_server);
  if (isset(rp->active, S_NB_NODETYPE)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_NB_NODETYPE))
      fprintf(dump_fp, "nbnt=0x%x:", rp->nb_nodetype);
  if (isset(rp->active, S_NB_SCOPE)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_NB_SCOPE)) print_str("nbsc", rp->nb_scope);
  if (isset(rp->active, S_XFONT_SERVER)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_XFONT_SERVER)) print_ips("xfsv", rp->xfont_server);
  if (isset(rp->active, S_XDISPLAY_MANAGER)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_XDISPLAY_MANAGER)) print_ips("xdmn", rp->xdisplay_manager);
  if (isset(rp->active, S_DHCP_T1)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_DHCP_T1)) fprintf(dump_fp, "dht1=%u:", rp->dhcp_t1);
  if (isset(rp->active, S_DHCP_T2)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_DHCP_T2)) fprintf(dump_fp, "dht2=%u:", rp->dhcp_t2);
  if (isset(rp->active, S_NISP_DOMAIN)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_NISP_DOMAIN)) print_str("nspd", rp->nisp_domain);
  if (isset(rp->active, S_NISP_SERVER)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_NISP_SERVER)) print_ips("nsps", rp->nisp_server);
  if (isset(rp->active, S_MOBILEIP_HA)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_MOBILEIP_HA)) print_ips("miph", rp->mobileip_ha);
  if (isset(rp->active, S_SMTP_SERVER)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_SMTP_SERVER)) print_ips("smtp", rp->smtp_server);
  if (isset(rp->active, S_POP3_SERVER)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_POP3_SERVER)) print_ips("pops", rp->pop3_server);
  if (isset(rp->active, S_NNTP_SERVER)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_NNTP_SERVER)) print_ips("nntp", rp->nntp_server);
  if (isset(rp->active, S_DFLT_WWW_SERVER)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_DFLT_WWW_SERVER))
    print_ips("wwws", rp->dflt_www_server);
  if (isset(rp->active, S_DFLT_FINGER_SERVER)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_DFLT_FINGER_SERVER))
    print_ips("fngs", rp->dflt_finger_server);
  if (isset(rp->active, S_DFLT_IRC_SERVER)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_DFLT_IRC_SERVER))
    print_ips("ircs", rp->dflt_irc_server);
  if (isset(rp->active, S_STREETTALK_SERVER)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_STREETTALK_SERVER))
    print_ips("stsv", rp->streettalk_server);
  if (isset(rp->active, S_STDA_SERVER)) fprintf(dump_fp, "!");
  if (isset(rp->valid, S_STDA_SERVER)) print_ips("stda", rp->stda_server);

  fprintf(dump_fp, "\n");
  return;
}


void
finish()
{
  dump_bind_db();
  exit(0);
}


/*
 * make binding list
 */
int
add_bind(bind)
  struct dhcp_binding *bind;
{
  struct hash_member *bindptr;
  extern struct hash_member *bindlist;

  if (bindlist == NULL) {
    /* first time */
    if ((bindptr =
	 (struct hash_member *) calloc(1, sizeof(struct hash_member))) == NULL) {
      syslog(LOG_WARNING, "calloc error in add_bind()");
      return(-1);
    }
  } else {
    /* not first time */
    if ((bindptr =
	 (struct hash_member *) calloc(1, sizeof(struct hash_member))) == NULL) {
      syslog(LOG_WARNING, "calloc error in add_bind()");
      return(-1);
    }
  }
  bindptr->next = bindlist;
  bindptr->data = (hash_datum *) bind;
  bindlist = bindptr;
  nbind++;

  return(0);
}


/*
 * read idtype
 */
static int
read_idtype(cp, idtype)
  char **cp;
  u_char *idtype;
{
  char *tmpstr;
  int j;

  if ((tmpstr = get_string(cp)) == NULL) {
    errno = 0;
    syslog(LOG_WARNING, "Can't get strings");
    return(-1);
  }

  if (sscanf(tmpstr, "%d", &j) != 1) {
    syslog(LOG_WARNING, "sscanf error in read_idtype()");
    return(-1);
  }

  *idtype = (u_char) j;
  free(tmpstr);

  return(0);
}


/*
 * read client identifier
 */
static int
read_cid(cp, cid, flag)
  char **cp;
  struct client_id *cid;
  char *flag;
{
  char *tmp;
  int i, j;

  if ((tmp = get_string(cp)) == NULL) {
    errno = 0;
    syslog(LOG_WARNING, "Can't get strings");
    return(-1);
  }

  cid->idlen = (strlen(tmp) - 2) / 2;
  if (cid->idlen > MAXOPT) {
    cid->idlen = MAXOPT;
    syslog(LOG_INFO,
	   "client identifier is too long in binding database. truncate it.");
  }
  if ((cid->id = calloc(1, cid->idlen)) == NULL) {
    syslog(LOG_WARNING, "calloc error in read_cid()");
    return(-1);
  }

  for (i = 0; i < cid->idlen; i++) {
    if (sscanf(&tmp[i*2+2], "%2x", &j) != 1) {
      syslog(LOG_WARNING, "sscanf error in read_cid()");
      return(-1);
    }
    cid->id[i] = (char) j;
  }
  free(tmp);

  return(0);
}


/*
 * read chaddr
 */
static int
read_haddr(cp, haddr)
  char **cp;
  struct chaddr *haddr;
{
  char *tmp;
  int i, j;

  if ((tmp = get_string(cp)) == NULL) {
    errno = 0;
    syslog(LOG_WARNING, "Can't get strings");
    return(-1);
  }

  if (tmp[0] != '\0' && tmp[0] == '0' && (tmp[1] == 'x' || tmp[1] == 'X')) {
    haddr->hlen = (strlen(tmp) - 2) / 2;
    if (haddr->hlen > MAX_HLEN) {
	haddr->hlen = MAX_HLEN;
	syslog(LOG_INFO, "chaddr is too long in binding database. truncate it.");
      }
    if ((haddr->haddr = calloc(1, haddr->hlen)) == NULL) {
      syslog(LOG_WARNING, "calloc error in read_haddr()");
      return(-1);
    }

    for (i = 0; i < haddr->hlen; i++) {
      if (sscanf(&tmp[i*2+2], "%2x", &j) != 1) {
	syslog(LOG_WARNING, "sscanf error in read_haddr()");
	return(-1);
      }
      haddr->haddr[i] = (char) j;
    }
    free(tmp);
  } else {
    haddr->hlen = strlen(tmp);
    if (haddr->hlen > MAX_HLEN) {   
      haddr->hlen = MAX_HLEN;
      syslog(LOG_INFO,
	     "chaddr \"%s\" is too long in binding database. truncate it.",
	     tmp);
    }
    haddr->haddr = tmp;
  }

  return(0);
}


/*
 * read subnet
 */
static int
read_subnet(cp, subnet)
  char **cp;
  struct in_addr *subnet;
{
  char *tmpstr;
  struct in_addr *tmpaddr;

  if ((tmpstr = get_string(cp)) == NULL) {
    errno = 0;
    syslog(LOG_WARNING, "Can't get strings");
    return(-1);
  }

  if ((tmpaddr = get_ip(&tmpstr)) == NULL) {
    errno = 0;
    syslog(LOG_WARNING, "get_ip() error in read_subnet()");
    return(-1);
  }

  *subnet = *tmpaddr;
  free(tmpstr);
  free(tmpaddr);

  return(0);
}


/*
 * called at once after read_addrpool_db()
 */
void
read_bind_db()
{
  char buffer[MAXENTRYLEN],
       *tmp = NULL,
       *bufptr = NULL;
  unsigned buflen = 0;
  struct dhcp_binding *binding = NULL;

  /*
   * read binding information 
   *
   * idtype:id:subnet:htype:haddr:"expire_date":resource_name
   */
  while(1) {
    buflen = sizeof(buffer);
    read_entry(buffer, &buflen, binding_dbfp);
    if (buflen == 0) {		/* More entries? */
      break;
    }
    bufptr = buffer;
    
    if ((binding = (struct dhcp_binding *)
	 calloc(1, sizeof(struct dhcp_binding))) == NULL) {
      syslog(LOG_ERR, "calloc error in read_bind_db");
      exit(1);
    }

    if (buffer[0] == '\0') {
      return;
    }

    /* read idtype */
    if (read_idtype(&bufptr, &binding->cid.idtype) != 0)
      exit(1);

    /* read client identifier */
    adjust(&bufptr);
    if (read_cid(&bufptr, &binding->cid, &binding->flag) != 0)
      exit(1);

    /* read subnet */
    adjust(&bufptr);
    if (read_subnet(&bufptr, &binding->cid.subnet) != 0)
      exit(1);

    /* read htype */
    adjust(&bufptr);
    if (read_idtype(&bufptr, &binding->haddr.htype) != 0)
      exit(1);

    /* read haddr */
    adjust(&bufptr);
    if (read_haddr(&bufptr, &binding->haddr) != 0)
      exit(1);

    /* read expired_epoch */
    adjust(&bufptr);
    tmp = get_string(&bufptr);
    if (strcmp(tmp, "infinity") == 0 || strcmp(tmp, "bootp") == 0) {
      binding->expire_epoch = 0xffffffff;
    } else {
      binding->expire_epoch = strtotime(tmp);
    }
    free(tmp);

    /* read resource entry name */
    adjust(&bufptr);
    binding->res_name = get_string(&bufptr);

    /* hash search with res_name */
    if ((binding->res = (struct dhcp_resource *)
	 hash_find(&nmhashtable, binding->res_name, strlen(binding->res_name),
		   resnmcmp, binding->res_name)) == NULL) {
      syslog(LOG_WARNING, "can't find the resource \"%s\"", binding->res_name);
      /*
       * free binding info
       */
      free(binding->res_name);
      free(binding->res);
      free(binding->cid.id);
      free(binding);
      continue;
    }

    binding->res->binding = binding;
    binding->flag |= COMPLETE_ENTRY;

    if (hash_ins(&cidhashtable, binding->cid.id, binding->cid.idlen,
		 bindcidcmp, &binding->cid, binding) < 0) {
      syslog(LOG_ERR, "hash_ins() with client identifier failed in read_bind_db()");
      exit(1);
    }
    if (add_bind(binding) != 0)
      exit(1);
  }

  errno = 0;
  syslog(LOG_INFO,
	 "read %d entries from binding and addr-pool database", nbind);
  return;
}


/*
 * called at once after read_bind_db()
 */
void
read_relay_db()
{
  char buffer[MAXENTRYLEN],
       relayIP[sizeof("255.255.255.255")],
       subnet_mask[sizeof("255.255.255.255")];
  int nrelay = 0;
  struct relay_acl *acl = NULL;

  /*
   * read pairs of relay agents' IP and subnet number
   */
  while(1) {
    errno = 0;
    if (fgets(buffer, MAXENTRYLEN, relay_dbfp) == NULL) {
      if (errno != 0)
	syslog(LOG_WARNING, "fgets() error in read_relay_db()");
      break;
    }
    if (sscanf(buffer, "%s %s", relayIP, subnet_mask) != 2) {
      syslog(LOG_WARNING, "sscanf() error in read_relay_db()");
      break;
    }

    if ((acl = (struct relay_acl *) calloc(1, sizeof(struct relay_acl))) == NULL) {
      syslog(LOG_WARNING, "calloc() error in read_relay_db()");
      break;
    }

    if ((acl->relay.s_addr = inet_addr(relayIP)) == -1 ||
	(acl->subnet_mask.s_addr = inet_addr(subnet_mask)) == -1) {
      syslog(LOG_WARNING, "inet_aton() error in read_relay_db()");
      break;
    }

    if (hash_ins(&relayhashtable, (char *) &acl->relay, sizeof(struct in_addr),
		 relayipcmp, &acl->relay, acl) < 0) {
      syslog(LOG_WARNING, "hash_ins() failed in read_relay_db()");
      break;
    }

    nrelay++;
  }

  errno = 0;
  syslog(LOG_INFO, "read %d entries from relay agent database", nrelay);
  return;
}


/*
 * Convert date string to time_t
 * return 0 if there is any error.
 */
static time_t
strtotime(date_str)
  char *date_str;
{
  struct tm   tmval;
  char        week[4], month[4];
  int         year = 0, i = 0;
  struct map {
    char symbol[4];
    int code;
  };
  struct map week_list[7];
  struct map month_list[12];

  week_list[0].code = 0;
  strcpy(week_list[0].symbol, "Sun");
  week_list[1].code = 1;
  strcpy(week_list[1].symbol, "Mon");
  week_list[2].code = 2;
  strcpy(week_list[2].symbol, "Tue");
  week_list[3].code = 3;
  strcpy(week_list[3].symbol, "Wed");
  week_list[4].code = 4;
  strcpy(week_list[4].symbol, "Thu");
  week_list[5].code = 5;
  strcpy(week_list[5].symbol, "Fri");
  week_list[6].code = 6;
  strcpy(week_list[6].symbol, "Sat");

  month_list[0].code = 0;
  strcpy(month_list[0].symbol, "Jan");
  month_list[1].code = 1;
  strcpy(month_list[1].symbol, "Feb");
  month_list[2].code = 2;
  strcpy(month_list[2].symbol, "Mar");
  month_list[3].code = 3;
  strcpy(month_list[3].symbol, "Apr");
  month_list[4].code = 4;
  strcpy(month_list[4].symbol, "May");
  month_list[5].code = 5;
  strcpy(month_list[5].symbol, "Jun");
  month_list[6].code = 6;
  strcpy(month_list[6].symbol, "Jul");
  month_list[7].code = 7;
  strcpy(month_list[7].symbol, "Aug");
  month_list[8].code = 8;
  strcpy(month_list[8].symbol, "Sep");
  month_list[9].code = 9;
  strcpy(month_list[9].symbol, "Oct");
  month_list[10].code = 10;
  strcpy(month_list[10].symbol, "Nov");
  month_list[11].code = 11;
  strcpy(month_list[11].symbol, "Dec");

  bzero(&tmval, sizeof(tmval));
  bzero(week, sizeof(week));
  bzero(month, sizeof(month));

  if (*date_str == '\0')
    return(0);

  sscanf(date_str, "%s %s %u %u:%u:%u %u", week, month, &(tmval.tm_mday),
	 &(tmval.tm_hour), &(tmval.tm_min), &(tmval.tm_sec), &year);

  tmval.tm_year = year - 1900;
  
  for (i = 0; i < 7; i++) {
    if (strcmp(week_list[i].symbol, week) == 0)
	break;
  }
  if (i < 7) tmval.tm_wday = week_list[i].code;
  else return(0);

  for (i = 0; i < 12; i++) {
    if (strcmp(month_list[i].symbol, month) == 0)
	break;
  }
  if (i < 12) tmval.tm_mon = month_list[i].code;
  else return(0);

#ifdef solaris
  return(mktime(&tmval));
#else
  return(_mktime(&tmval));
#endif
}


static struct in_addr *
get_ip(src)
  char **src;
{
  struct in_addr *tmp = NULL;

  if ((tmp = (struct in_addr *) calloc(1, sizeof(struct in_addr))) == NULL) {
    syslog(LOG_WARNING, "calloc error in get_inetaddr()");
    return(NULL);
  }

  if (prs_inaddr(src, &tmp->s_addr) != 0) {
    free(tmp);
    return(NULL);
  }

  return(tmp);
}


int
resipcmp(key, rp)
  struct in_addr *key;
  struct dhcp_resource *rp;
{
  return (key->s_addr == rp->ip_addr->s_addr);
}


int
relayipcmp(key, aclp)
  struct in_addr *key;
  struct relay_acl *aclp;
{
  return (key->s_addr == aclp->relay.s_addr);
}


int
bindcidcmp(key, bp)
  struct client_id *key;
  struct dhcp_binding *bp;
{
  return (key != NULL && key->id != NULL && bp != NULL && bp->cid.id != NULL &&
	  key->subnet.s_addr == bp->cid.subnet.s_addr &&
	  key->idtype == bp->cid.idtype && key->idlen == bp->cid.idlen &&
	  bcmp(key->id, bp->cid.id, key->idlen) == 0);
}


static void
set_default(rp)
  struct dhcp_resource *rp;
{
  if (isclr(rp->valid, S_DEFAULT_LEASE)) {
    setbit(rp->valid, S_DEFAULT_LEASE);
    rp->default_lease = D_DFLTLEASE;
  }

  if (isclr(rp->valid, S_MAX_LEASE)) {
    setbit(rp->valid, S_MAX_LEASE);
    rp->max_lease = D_MAXLEASE;
  }
  if (isset(rp->valid, S_CLIENT_ID)) {
    setbit(rp->valid, S_ALLOW_BOOTP);
    rp->allow_bootp = TRUE;
  }
  if (isset(rp->valid, S_ALLOW_BOOTP) && rp->allow_bootp == TRUE) {
    setbit(rp->valid, S_MAX_LEASE);
    rp->max_lease = 0xffffffff;   /* infinity */
  }

  if (isclr(rp->valid, S_SUBNET_MASK)) {
    setbit(rp->valid, S_SUBNET_MASK);
    rp->subnet_mask = default_netmask(rp->ip_addr);
  }
  return;
}


static struct in_addr *
default_netmask(ipaddr)
  struct in_addr *ipaddr;
{
  int top = 0;
  struct in_addr *dfltnetmask = NULL;

  if ((dfltnetmask =
       (struct in_addr *) calloc(1, sizeof(struct in_addr))) == NULL) {
    syslog(LOG_ERR, "calloc error in default_netmask()");
    exit(1);
  }

  top = ntohl(ipaddr->s_addr) >> 30;

  switch (top) {
  case 0:  /* class A */
    dfltnetmask->s_addr = htonl(0xff000000);
    break;
  case 2:  /* class B */
    dfltnetmask->s_addr = htonl(0xffff0000);
    break;
  case 3:  /* class C */
    dfltnetmask->s_addr = htonl(0xffffff00);
    break;
  default:
    errno = 0;
    syslog(LOG_ERR, "Can't get default netmask of %s", inet_ntoa(*ipaddr));
    exit(1);
    break;
  }

  return(dfltnetmask);
}


/*
 * processing the sname field which appear in the address pool database.
 */
/* ARGSUSED */
int
proc_sname(code, optype, symbol, rp)
  int code;
  int optype;
  char **symbol;
  struct dhcp_resource *rp;
{
  if (optype == OP_ADDITION) {
    rp->sname = get_string(symbol);
    if (strlen(rp->sname) > MAX_SNAME) {
      rp->sname[MAX_SNAME - 1] = '\0';
    }
  } else {
    rp->sname = NULL;
  }

  return(0);
}


/*
 * processing the file field which appear in the address pool database.
 */
/* ARGSUSED */
int
proc_file(code, optype, symbol, rp)
  int code;
  int optype;
  char **symbol;
  struct dhcp_resource *rp;
{
  if (optype == OP_ADDITION) {
    rp->file = get_string(symbol);
    if (strlen(rp->file) > MAX_FILE) {
      rp->file[MAX_FILE - 1] = '\0';
    }
  } else {
    rp->file = NULL;
  }

  return(0);
}


/*
 * processing client identifier of static resource
 */
/* ARGSUSED */
int
proc_clid(code, optype, symbol, rp)
  int code;
  int optype;
  char **symbol;
  struct dhcp_resource *rp;
{
  char *bufptr;
  struct in_addr *tmpsubnet;
  struct dhcp_binding *binding;

  /* if no IP address, return error immediately */
  if (rp->ip_addr == NULL)
    return(-1);

  bufptr = get_string(symbol);
  if ((binding = (struct dhcp_binding *)
       calloc(1, sizeof(struct dhcp_binding))) == NULL) {
    syslog(LOG_WARNING, "calloc error in proc_clid");
    return(-1);
  }
  if (bufptr[0] == '\0') {
    return(-1);
  }

  /* read idtype */
  if (read_idtype(&bufptr, &binding->cid.idtype) != 0)
    return(-1);

  /* read client identifier */
  adjust(&bufptr);
  if (read_cid(&bufptr, &binding->cid, &binding->flag) != 0)
    return(-1);

  /*
   * get subnet
   */
  if (rp->subnet_mask != NULL) {
    binding->cid.subnet.s_addr = rp->ip_addr->s_addr & rp->subnet_mask->s_addr;  } else {
    tmpsubnet = default_netmask(rp->ip_addr);
    binding->cid.subnet.s_addr = rp->ip_addr->s_addr & tmpsubnet->s_addr;
    free(tmpsubnet);
  }

  /*
   * htype and haddr could be anything,
   * so copy client identifer as temporary
   */
  binding->haddr.htype = binding->cid.idtype;
  binding->haddr.hlen = binding->cid.idlen;
  if ((binding->haddr.haddr = calloc(1, binding->cid.idlen)) == NULL) {
    syslog(LOG_WARNING, "calloc error in proc_clid()");
    return(-1);
  }
  bcopy(binding->cid.id, binding->haddr.haddr, binding->cid.idlen);


  binding->expire_epoch = 0xffffffff;
  if ((binding->res_name = calloc(1, strlen(rp->entryname) + 1)) == NULL) {
    syslog(LOG_WARNING, "calloc error in proc_clid()");
    return(-1);
  }
  strcpy(binding->res_name, rp->entryname);

  binding->res = rp;
  binding->res->binding = binding;
  binding->flag = (COMPLETE_ENTRY | STATIC_ENTRY);

  if (hash_ins(&cidhashtable, binding->cid.id, binding->cid.idlen,
	       bindcidcmp, &binding->cid, binding) < 0) {
    syslog(LOG_WARNING, "hash_ins() with client identifier failed in proc_clid()");
    return(-1);
  }
  if (add_bind(binding) != 0)
    return(-1);

  return(0);
}


/*
 * processing the similar entry (table continuation).
 */
/* ARGSUSED */
int
proc_tblc(code, optype, symbol, rp1)
  int code;
  int optype;
  char **symbol;
  struct dhcp_resource *rp1;
{
  struct dhcp_resource *rp2 = NULL;
  char *tmpstr;
  int i = 0;

  tmpstr = get_string(symbol);
  if ((rp2 = (struct dhcp_resource *)
       hash_find(&nmhashtable, tmpstr, strlen(tmpstr), resnmcmp, tmpstr)) == NULL) {
    errno = 0;
    syslog(LOG_WARNING, "Can't find tblc=\"%s\"\n", tmpstr);
    return(-1);
  }

  if (isclr(rp1->valid, S_SNAME)) rp1->sname = rp2->sname;
  if (isclr(rp1->valid, S_FILE)) rp1->file = rp2->file;
  if (isclr(rp1->valid, S_SIADDR)) rp1->siaddr = rp2->siaddr;
  if (isclr(rp1->valid, S_ALLOW_BOOTP)) rp1->allow_bootp = rp2->allow_bootp;
  if (isclr(rp1->valid, S_IP_ADDR)) rp1->ip_addr = rp2->ip_addr;
  if (isclr(rp1->valid, S_MAX_LEASE)) rp1->max_lease = rp2->max_lease;
  if (isclr(rp1->valid, S_DEFAULT_LEASE)) rp1->default_lease = rp2->default_lease;
  if (isclr(rp1->valid, S_SUBNET_MASK)) rp1->subnet_mask = rp2->subnet_mask;
  if (isclr(rp1->valid, S_TIME_OFFSET)) rp1->time_offset = rp2->time_offset;
  if (isclr(rp1->valid, S_ROUTER)) rp1->router = rp2->router;
  if (isclr(rp1->valid, S_TIME_SERVER)) rp1->time_server = rp2->time_server;
  if (isclr(rp1->valid, S_NAME_SERVER)) rp1->name_server = rp2->name_server;
  if (isclr(rp1->valid, S_DNS_SERVER)) rp1->dns_server = rp2->dns_server;
  if (isclr(rp1->valid, S_LOG_SERVER)) rp1->log_server = rp2->log_server;
  if (isclr(rp1->valid, S_COOKIE_SERVER)) rp1->cookie_server = rp2->cookie_server;
  if (isclr(rp1->valid, S_LPR_SERVER)) rp1->lpr_server = rp2->lpr_server;
  if (isclr(rp1->valid, S_IMPRESS_SERVER)) rp1->impress_server = rp2->impress_server;
  if (isclr(rp1->valid, S_RLS_SERVER)) rp1->rls_server = rp2->rls_server;
  if (isclr(rp1->valid, S_HOSTNAME)) rp1->hostname = rp2->hostname;
  if (isclr(rp1->valid, S_BOOTSIZE)) rp1->bootsize = rp2->bootsize;
  if (isclr(rp1->valid, S_MERIT_DUMP)) rp1->merit_dump = rp2->merit_dump;
  if (isclr(rp1->valid, S_DNS_DOMAIN)) rp1->dns_domain = rp2->dns_domain;
  if (isclr(rp1->valid, S_SWAP_SERVER)) rp1->swap_server = rp2->swap_server;
  if (isclr(rp1->valid, S_ROOT_PATH)) rp1->root_path = rp2->root_path;
  if (isclr(rp1->valid, S_EXTENSIONS_PATH))
    rp1->extensions_path = rp2->extensions_path;
  if (isclr(rp1->valid, S_IP_FORWARD)) rp1->ip_forward = rp2->ip_forward;
  if (isclr(rp1->valid, S_NONLOCAL_SRCROUTE))
    rp1->nonlocal_srcroute = rp2->nonlocal_srcroute;
  if (isclr(rp1->valid, S_POLICY_FILTER)) rp1->policy_filter = rp2->policy_filter;
  if (isclr(rp1->valid, S_MAX_DGRAM_SIZE)) rp1->max_dgram_size = rp2->max_dgram_size;
  if (isclr(rp1->valid, S_DEFAULT_IP_TTL)) rp1->default_ip_ttl = rp2->default_ip_ttl;
  if (isclr(rp1->valid, S_MTU_AGING_TIMEOUT))
    rp1->mtu_aging_timeout = rp2->mtu_aging_timeout;
  if (isclr(rp1->valid, S_MTU_PLATEAU_TABLE))
    rp1->mtu_plateau_table = rp2->mtu_plateau_table;
  if (isclr(rp1->valid, S_IF_MTU)) rp1->intf_mtu = rp2->intf_mtu;
  if (isclr(rp1->valid, S_ALL_SUBNET_LOCAL))
    rp1->all_subnet_local = rp2->all_subnet_local;
  if (isclr(rp1->valid, S_BRDCAST_ADDR)) rp1->brdcast_addr = rp2->brdcast_addr;
  if (isclr(rp1->valid, S_MASK_DISCOVER)) rp1->mask_discover = rp2->mask_discover;
  if (isclr(rp1->valid, S_MASK_SUPPLIER)) rp1->mask_supplier = rp2->mask_supplier;
  if (isclr(rp1->valid, S_ROUTER_DISCOVER))
    rp1->router_discover = rp2->router_discover;
  if (isclr(rp1->valid, S_ROUTER_SOLICIT)) rp1->router_solicit = rp2->router_solicit;
  if (isclr(rp1->valid, S_STATIC_ROUTE)) rp1->static_route = rp2->static_route;
  if (isclr(rp1->valid, S_TRAILER)) rp1->trailer = rp2->trailer;
  if (isclr(rp1->valid, S_ARP_CACHE_TIMEOUT))
    rp1->arp_cache_timeout = rp2->arp_cache_timeout;
  if (isclr(rp1->valid, S_ETHER_ENCAP)) rp1->ether_encap = rp2->ether_encap;
  if (isclr(rp1->valid, S_DEFAULT_TCP_TTL))
    rp1->default_tcp_ttl = rp2->default_tcp_ttl;
  if (isclr(rp1->valid, S_KEEPALIVE_INTER))
    rp1->keepalive_inter = rp2->keepalive_inter;
  if (isclr(rp1->valid, S_KEEPALIVE_GARBA))
    rp1->keepalive_garba = rp2->keepalive_garba;
  if (isclr(rp1->valid, S_NIS_DOMAIN)) rp1->nis_domain = rp2->nis_domain;
  if (isclr(rp1->valid, S_NIS_SERVER)) rp1->nis_server = rp2->nis_server;
  if (isclr(rp1->valid, S_NTP_SERVER)) rp1->ntp_server = rp2->ntp_server;
  if (isclr(rp1->valid, S_NBN_SERVER)) rp1->nbn_server = rp2->nbn_server;
  if (isclr(rp1->valid, S_NBDD_SERVER)) rp1->nbdd_server = rp2->nbdd_server;
  if (isclr(rp1->valid, S_NB_NODETYPE)) rp1->nb_nodetype = rp2->nb_nodetype;
  if (isclr(rp1->valid, S_NB_SCOPE)) rp1->nb_scope = rp2->nb_scope;
  if (isclr(rp1->valid, S_XFONT_SERVER)) rp1->xfont_server = rp2->xfont_server;
  if (isclr(rp1->valid, S_XDISPLAY_MANAGER))
    rp1->xdisplay_manager = rp2->xdisplay_manager;
  if (isclr(rp1->valid, S_DHCP_T1)) rp1->dhcp_t1 = rp2->dhcp_t1;
  if (isclr(rp1->valid, S_DHCP_T2)) rp1->dhcp_t2 = rp2->dhcp_t2;
  if (isclr(rp1->valid, S_NISP_DOMAIN)) rp1->nisp_domain = rp2->nisp_domain;
  if (isclr(rp1->valid, S_NISP_SERVER)) rp1->nisp_server = rp2->nisp_server;
  if (isclr(rp1->valid, S_MOBILEIP_HA)) rp1->mobileip_ha = rp2->mobileip_ha;
  if (isclr(rp1->valid, S_SMTP_SERVER)) rp1->smtp_server = rp2->smtp_server;
  if (isclr(rp1->valid, S_POP3_SERVER)) rp1->pop3_server = rp2->pop3_server;
  if (isclr(rp1->valid, S_NNTP_SERVER)) rp1->nntp_server = rp2->nntp_server;
  if (isclr(rp1->valid, S_DFLT_WWW_SERVER))
    rp1->dflt_www_server = rp2->dflt_www_server;
  if (isclr(rp1->valid, S_DFLT_FINGER_SERVER))
    rp1->dflt_finger_server = rp2->dflt_finger_server;
  if (isclr(rp1->valid, S_DFLT_IRC_SERVER))
    rp1->dflt_irc_server = rp2->dflt_irc_server;
  if (isclr(rp1->valid, S_STREETTALK_SERVER))
    rp1->streettalk_server = rp2->streettalk_server;
  if (isclr(rp1->valid, S_STDA_SERVER)) rp1->stda_server = rp2->stda_server;

  for (i = 0; i < VALIDSIZE; i++) {
    rp1->active[i] |= (~rp1->valid[i] & rp2->active[i]);
    rp1->valid[i] |= rp2->valid[i];
  }

  return(0);
}


/*
 * processing the mtu plateau table entry.
 */
/* ARGSUSED */
int
proc_mtpt(code, optype, symbol, rp)
  int code;
  int optype;
  char **symbol;
  struct dhcp_resource *rp;
{
  int i = 0;
  unsigned short tmpnum[MAX_MTUPLTSZ];

  if (optype == OP_ADDITION) {
    for (i = 0; i <= MAX_MTUPLTSZ; i++) {
      while (**symbol && isspace(**symbol)) {  /* Skip whitespace */
	(*symbol)++;
      }
      if (! **symbol) {                        /* Quit if nothing more */
	break;
      }
      if (isdigit(**symbol)) tmpnum[i] = (u_short) get_integer(symbol);
      else break;
    }

    if (i == 0) {
      errno = 0;
      syslog(LOG_WARNING, "Bad mtpt entry.");
      return(-1);
    }

    if ((rp->mtu_plateau_table =
	 (struct u_shorts *) calloc(1, sizeof(struct u_shorts))) == NULL) {
      syslog(LOG_WARNING, "calloc error in proc_mtpt()");
      return(-1);
    }
    rp->mtu_plateau_table->num = i;
    if ((rp->mtu_plateau_table->shorts =
	 (u_short *) calloc(i, sizeof(u_short))) == NULL) {
      syslog(LOG_WARNING, "calloc error in proc_mtpt()");
      return(-1);
    }

    for (i = 0; i < rp->mtu_plateau_table->num; i++) {
      rp->mtu_plateau_table->shorts[i] = htons (tmpnum[i]);
    }
  } else {
    rp->mtu_plateau_table = NULL;
  }

  return(0);
}


/*
 * processing the IP address which appear in the resouce database.
 */
int
proc_ip(code, optype, symbol, rp)
  int code;
  int optype;
  char **symbol;
  struct dhcp_resource *rp;
{
  struct in_addr **ipp = NULL;

  switch (code) {
  case S_SIADDR:
    ipp = &rp->siaddr;
    break;
  case S_IP_ADDR:
    ipp = &rp->ip_addr;
    break;
  case S_SUBNET_MASK:
    ipp = &rp->subnet_mask;
    break;
  case S_SWAP_SERVER:
    ipp = &rp->swap_server;
    break;
  case S_BRDCAST_ADDR:
    ipp = &rp->brdcast_addr;
    break;
  case S_ROUTER_SOLICIT:
    ipp = &rp->router_solicit;
    break;
  }

  if (optype == OP_ADDITION) {
    if ((*ipp = get_ip(symbol)) == NULL) {
      errno = 0;
      syslog(LOG_WARNING, "Bad IP address");
      return(-1);
    }
  } else {
    *ipp = NULL;
  }

  return(0);
}


/*
 * processing the IP addresses which appear in the resouce database.
 */
int
proc_ips(code, optype, symbol, rp)
  int code;
  int optype;
  char **symbol;
  struct dhcp_resource *rp;
{
  struct in_addrs **ipp = NULL;
  int i = 0;
  struct in_addr *tmpaddr[MAX_IPS];

  switch (code) {
  case S_ROUTER:
    ipp = &rp->router;
    break;
  case S_TIME_SERVER:
    ipp = &rp->time_server;
    break;
  case S_NAME_SERVER:
    ipp = &rp->name_server;
    break;
  case S_DNS_SERVER:
    ipp = &rp->dns_server;
    break;
  case S_LOG_SERVER:
    ipp = &rp->log_server;
    break;
  case S_COOKIE_SERVER:
    ipp = &rp->cookie_server;
    break;
  case S_LPR_SERVER:
    ipp = &rp->lpr_server;
    break;
  case S_IMPRESS_SERVER:
    ipp = &rp->impress_server;
    break;
  case S_RLS_SERVER:
    ipp = &rp->rls_server;
    break;
  case S_NIS_SERVER:
    ipp = &rp->nis_server;
    break;
  case S_NTP_SERVER:
    ipp = &rp->ntp_server;
    break;
  case S_NBN_SERVER:
    ipp = &rp->nbn_server;
    break;
  case S_NBDD_SERVER:
    ipp = &rp->nbdd_server;
    break;
  case S_XFONT_SERVER:
    ipp = &rp->xfont_server;
    break;
  case S_XDISPLAY_MANAGER:
    ipp = &rp->xdisplay_manager;
    break;
  case S_NISP_SERVER:
    ipp = &rp->nisp_server;
    break;
  case S_MOBILEIP_HA:
    ipp = &rp->mobileip_ha;
    break;
  case S_SMTP_SERVER:
    ipp = &rp->smtp_server;
    break;
  case S_POP3_SERVER:
    ipp = &rp->pop3_server;
    break;
  case S_NNTP_SERVER:
    ipp = &rp->nntp_server;
    break;
  case S_DFLT_WWW_SERVER:
    ipp = &rp->dflt_www_server;
    break;
  case S_DFLT_FINGER_SERVER:
    ipp = &rp->dflt_finger_server;
    break;
  case S_DFLT_IRC_SERVER:
    ipp = &rp->dflt_irc_server;
    break;
  case S_STREETTALK_SERVER:
    ipp = &rp->streettalk_server;
    break;
  case S_STDA_SERVER:
    ipp = &rp->stda_server;
    break;
  }

  if (optype == OP_ADDITION) {
    if ((*ipp = (struct in_addrs *) calloc(1, sizeof(struct in_addrs))) == NULL) {
      syslog(LOG_WARNING, "calloc error in proc_ips()");
      return(-1);
    }

    for (i = 0; i <= MAX_IPS; i++) {
      while (**symbol && isspace(**symbol)) {    /* Skip whitespace */
	(*symbol)++;
      }
      if (! **symbol) {                          /* Quit if nothing more */
	break;
      }
      if ((tmpaddr[i] = get_ip(symbol)) == NULL) {
	break;
      }
    }

    if (i == 0) {
      errno = 0;
      syslog(LOG_WARNING, "Bad IP addresses.");
      return(-1);
    }

    (*ipp)->num = i;
    if (((*ipp)->addr =
	 (struct in_addr *) calloc(i, sizeof(struct in_addr))) == NULL) {
      syslog(LOG_WARNING, "calloc error in proc_ips()");
      return(-1);
    }
    for (i = 0; i < (*ipp)->num; i++) {
      (*ipp)->addr[i] = *tmpaddr[i];
      free(tmpaddr[i]);
    }
  } else {
    *ipp = NULL;
  }

  return(0);
}


/*
 * processing the IP address pairs which appear in the resouce database.
 */
int
proc_ippairs(code, optype, symbol, rp)
  int code;
  int optype;
  char **symbol;
  struct dhcp_resource *rp;
{
  struct ip_pairs **ipp = NULL;
  int i = 0;
  struct in_addr *tmpaddr1[MAX_IPPAIR], *tmpaddr2[MAX_IPPAIR];

  switch (code) {
  case S_POLICY_FILTER:
    ipp = &rp->policy_filter;
    break;
  case S_STATIC_ROUTE:
    ipp = &rp->static_route;
    break;
  }

  if (optype == OP_ADDITION) {
    if ((*ipp = (struct ip_pairs *) calloc(1, sizeof(struct ip_pairs))) == NULL) {
      syslog(LOG_WARNING, "calloc error in proc_ippairs()");
      return(-1);
    }

    for (i = 0; i <= MAX_IPPAIR; i++) {
      while (**symbol && isspace(**symbol)) {    /* Skip whitespace */
	(*symbol)++;
      }
      if (! **symbol) {                          /* Quit if nothing more */
	break;
      }
      if ((tmpaddr1[i] = get_ip(symbol)) == NULL) {
	break;
      }

      while (**symbol && isspace(**symbol)) {    /* Skip whitespace */
	(*symbol)++;
      }
      if (! **symbol) {                          /* Quit if nothing more */
	errno = 0;
	syslog(LOG_WARNING, "There is no buddy IP address");
	break;
      }
      if ((tmpaddr2[i] = get_ip(symbol)) == NULL) {
	errno = 0;
	syslog(LOG_WARNING, "There is no buddy IP address");
	break;
      }

    }

    if (i == 0) {
      errno = 0;
      syslog(LOG_WARNING, "Bad IP addresses.");
      return(-1);
    }

    (*ipp)->num = i;
    if (((*ipp)->addr1 =
	 (struct in_addr *) calloc(i, sizeof(struct in_addr))) == NULL) {
      syslog(LOG_WARNING, "calloc error in proc_ips()");
      return(-1);
    }
    if (((*ipp)->addr2 =
	 (struct in_addr *) calloc(i, sizeof(struct in_addr))) == NULL) {
      syslog(LOG_WARNING, "calloc error in proc_ips()");
      return(-1);
    }
    for (i = 0; i < (*ipp)->num; i++) {
      (*ipp)->addr1[i] = *tmpaddr1[i];
      (*ipp)->addr2[i] = *tmpaddr2[i];
      free(tmpaddr1[i]);
      free(tmpaddr2[i]);
    }
  } else {
    *ipp = NULL;
  }

  return(0);
}


/*
 * processing the host-byte-order long
 */
int
proc_hl(code, optype, symbol, rp)
  int code;
  int optype;
  char **symbol;
  struct dhcp_resource *rp;
{
  u_Long *lp;

  switch (code) {
  case S_MAX_LEASE:
    lp = &rp->max_lease;
    break;
  case S_DEFAULT_LEASE:
    lp = &rp->default_lease;
    break;
  }

  if (optype == OP_ADDITION) {
    *lp = get_integer(symbol);
  } else {
    *lp = 0;
  }

  return(0);
}


/*
 * processing the host-byte-order short
 */
int
proc_hs(code, optype, symbol, rp)
  int code;
  int optype;
  char **symbol;
  struct dhcp_resource *rp;
{
  u_short *sp;

  switch (code) {
  case S_DHCP_T1:
    sp = &rp->dhcp_t1;
    break;
  case S_DHCP_T2:
    sp = &rp->dhcp_t2;
    break;
  }

  if (optype == OP_ADDITION) {
    *sp = (short) get_integer(symbol);
  } else {
    *sp = 0;
  }

  return(0);
}


/*
 * processing the network-byte-order long
 */
int
proc_nl(code, optype, symbol, rp)
  int code;
  int optype;
  char **symbol;
  struct dhcp_resource *rp;
{
  u_Long *lp;

  switch (code) {
  case S_TIME_OFFSET:
    lp = (u_Long *) &rp->time_offset;
    break;
  case S_MTU_AGING_TIMEOUT:
    lp = &rp->mtu_aging_timeout;
    break;
  case S_ARP_CACHE_TIMEOUT:
    lp = &rp->arp_cache_timeout;
    break;
  case S_KEEPALIVE_INTER:
    lp = &rp->keepalive_inter;
    break;
  }

  if (optype == OP_ADDITION) {
    *lp = htonl(get_integer(symbol));
  } else {
    *lp = 0;
  }

  return(0);
}


/*
 * processing the host-byte-order short
 */
int
proc_ns(code, optype, symbol, rp)
  int code;
  int optype;
  char **symbol;
  struct dhcp_resource *rp;
{
  u_short *sp;

  switch (code) {
  case S_BOOTSIZE:
    sp = &rp->bootsize;
    break;
  case S_MAX_DGRAM_SIZE:
    sp = &rp->max_dgram_size;
    break;
  case S_IF_MTU:
    sp = &rp->intf_mtu;
    break;
  }

  if (optype == OP_ADDITION) {
    *sp = htons((short) get_integer(symbol));
  } else {
    *sp = 0;
  }

  return(0);
}


/*
 * processing the octet number
 */
int
proc_octet(code, optype, symbol, rp)
  int code;
  int optype;
  char **symbol;
  struct dhcp_resource *rp;
{
  u_char *cp;

  switch (code) {
  case S_DEFAULT_IP_TTL:
    cp = &rp->default_ip_ttl;
    break;
  case S_DEFAULT_TCP_TTL:
    cp = &rp->default_tcp_ttl;
    break;
  case S_NB_NODETYPE:
    cp = &rp->nb_nodetype;
    break;
  }

  if (optype == OP_ADDITION) {
    *cp = (u_char) get_integer(symbol);
  } else {
    *cp = 0;
  }

  return(0);
}


/*
 * processing the strings.
 */
int
proc_str(code, optype, symbol, rp)
  int code;
  int optype;
  char **symbol;
  struct dhcp_resource *rp;
{
  char **cp;

  switch (code) {
  case S_HOSTNAME:
    cp = &rp->hostname;
    break;
  case S_MERIT_DUMP:
    cp = &rp->merit_dump;
    break;
  case S_DNS_DOMAIN:
    cp = &rp->dns_domain;
    break;
  case S_ROOT_PATH:
    cp = &rp->root_path;
    break;
  case S_EXTENSIONS_PATH:
    cp = &rp->extensions_path;
    break;
  case S_NIS_DOMAIN:
    cp = &rp->nis_domain;
    break;
  case S_NB_SCOPE:
    cp = &rp->nb_scope;
    break;
  case S_NISP_DOMAIN:
    cp = &rp->nisp_domain;
    break;
  }

  if (optype == OP_ADDITION) {
    if ((*cp = get_string(symbol)) == NULL) {
      errno = 0;
      syslog(LOG_WARNING, "Can't get strings");
      return(-1);
    }
    if (strlen(*cp) > MAXOPT) *cp[MAXOPT] = '\0';
  } else {
    *cp = NULL;
  }

  return(0);
}


/*
 * processing the boolean
 */
int
proc_bool(code, optype, symbol, rp)
  int code;
  int optype;
  char **symbol;
  struct dhcp_resource *rp;
{
  char *tmpstr;
  char *cp;

  switch (code) {
  case S_ALLOW_BOOTP:
    cp = &rp->allow_bootp;
    break;
  case S_IP_FORWARD:
    cp = &rp->ip_forward;
    break;
  case S_NONLOCAL_SRCROUTE:
    cp = &rp->nonlocal_srcroute;
    break;
  case S_ALL_SUBNET_LOCAL:
    cp = &rp->all_subnet_local;
    break;
  case S_MASK_DISCOVER:
    cp = &rp->mask_discover;
    break;
  case S_MASK_SUPPLIER:
    cp = &rp->mask_supplier;
    break;
  case S_ROUTER_DISCOVER:
    cp = &rp->router_discover;
    break;
  case S_TRAILER:
    cp = &rp->trailer;
    break;
  case S_ETHER_ENCAP:
    cp = &rp->ether_encap;
    break;
  case S_KEEPALIVE_GARBA:
    cp = &rp->keepalive_garba;
    break;
  }

  if (optype == OP_ADDITION) {
    if ((tmpstr = get_string(symbol)) == NULL) {
      errno = 0;
      syslog(LOG_WARNING, "Can't get strings");
      return(-1);
    } else {
      if (strcmp(tmpstr, "true") == 0)
	*cp = TRUE;
      else if (strcmp(tmpstr, "false") == 0)
	*cp = FALSE;
      else {
	errno = 0;
	syslog(LOG_WARNING, "Bad boolean");
	*cp = FALSE;
	free(tmpstr);
	return(-1);
      }
      free(tmpstr);
    }
  } else {
    *cp = 0;
  }

  return(0);
}


/*
 *   Only the following portions are Copyright (c) 1988 by Carnegie Mellon.
 *   Modified by Akihiro Tominaga, 1994.
 */
/*
 * Copyright (c) 1988 by Carnegie Mellon.
 *
 * Permission to use, copy, modify, and distribute this program for any
 * purpose and without fee is hereby granted, provided that this copyright
 * and permission notice appear on all copies and supporting documentation,
 * the name of Carnegie Mellon not be used in advertising or publicity
 * pertaining to distribution of the program without specific prior
 * permission, and notice be given in supporting documentation that copying
 * and distribution is by permission of Carnegie Mellon and Stanford
 * University.  Carnegie Mellon makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 */
void
read_addrpool_db()
{
  struct dhcp_resource *rp = NULL;
  struct hash_member *resptr = NULL;
  char buffer[MAXENTRYLEN];
  unsigned buflen = 0;
  int nresource = 0;

  while(1) {
    buflen = sizeof(buffer);
    read_entry(buffer, &buflen, addrpool_dbfp);
    if (buflen == 0) {		/* More entries? */
      break;
    }
    
    if ((rp = (struct dhcp_resource *)
	 calloc(1, sizeof(struct dhcp_resource))) == NULL) {
      syslog(LOG_ERR, "calloc error in read_addrpool_db");
      exit(1);
    }
    
    /*
     * Get individual info
     */
    if (process_entry(rp, buffer) < 0) {
      syslog(LOG_ERR, "An inappropriate entry appears: \"%s\"", buffer);
      continue;
    }

    /*
     * set the default value
     */
    if (isset(rp->valid, S_IP_ADDR)) {   /* it is not dummy entry */
      set_default(rp);
    }

    /* make resource's list */
    if (reslist == NULL) {
      /* first time */
      if ((resptr = reslist =
	   (struct hash_member *) calloc(1, sizeof(struct hash_member))) == NULL) {
	syslog(LOG_ERR, "calloc error in read_addrpool_db");
	exit(1);
      }
    } else {
      /* not first time */
      if ((resptr->next =
	   (struct hash_member *) calloc(1, sizeof(struct hash_member))) == NULL) {
	syslog(LOG_ERR, "calloc error in read_addrpool_db");
	exit(1);
      }
      resptr = resptr->next;
    }
    resptr->next = NULL;
    resptr->data = (hash_datum *) rp;

    /* make hash table by resource.entryname */
    if (hash_ins(&nmhashtable, rp->entryname, strlen(rp->entryname),
		 resnmcmp, rp->entryname, rp) < 0) {
      syslog(LOG_ERR,
	     "hash_ins() failed in read_addrpool_db(): %s", buffer);
      exit(1);
    }

    if (isset(rp->valid, S_IP_ADDR)) {
      if (hash_ins(&iphashtable, (char *)&rp->ip_addr->s_addr,
		   sizeof(u_Long), resipcmp, rp->ip_addr, rp) < 0) {
	syslog(LOG_ERR,
	       "hash_ins() with IP address failed in read_addrpool_db()");
	exit(1);
      }
    }

    nresource++;
  }

  errno = 0;
  syslog(LOG_INFO, "read %d entries from addr-pool database", nresource);
  return;
}


/*
 * Parse out all the various tags and parameters in the host entry pointed
 * to by "src".  Stuff all the data into the appropriate fields of the
 * host structure pointed to by "host".  If there is any problem with the
 * entry, an error message is reported via syslog(), no further processing
 * is done, and -1 is returned.  Successful calls return 0.
 *
 * (Some errors probably shouldn't be so completely fatal. . . .)
 */
static void
read_entry(buffer, bufsiz, fp)
  char *buffer;
  unsigned *bufsiz;
  FILE *fp;
{
  int c = 0, length = 0;

  length = 0;

  /*
   * Eat whitespace, blank lines, and comment lines.
   */
 top:
  c = fgetc(fp);
  if (c < 0) {
    goto done;			/* Exit if end-of-file */
  }
  if (isspace(c)) {
    goto top;			/* Skip over whitespace */
  }
  if (c == '#') {
    while (TRUE) {			/* Eat comments after # */
      c = fgetc(fp);
      if (c < 0) {
	goto done;		/* Exit if end-of-file */
      }
      if (c == '\n') {
	goto top;		/* Try to read the next line */
      }
    }
  }
  ungetc(c, fp);	/* Other character, push it back to reprocess it */
  
  
  /*
   * Now we're actually reading a data entry.  Get each character and
   * assemble it into the data buffer, processing special characters like
   * double quotes (") and backslashes (\).
   */
  
 mainloop:
  c = fgetc(fp);
  switch (c) {
  case EOF:		
  case '\n':
    goto done;			/* Exit on EOF or newline */
  case '\\':
    c = fgetc(fp);		/* Backslash, read a new character */
    if (c < 0) {
      goto done;		/* Exit on EOF */
    }
    *buffer++ = c;		/* Store the literal character */
    length++;
    if (length < *bufsiz - 1) {
      goto mainloop;
    } else {
      goto done;
    }
  case '"':
    *buffer++ = '"';		/* Store double-quote */
    length++;
    if (length >= *bufsiz - 1) {
      goto done;
    }
    while (TRUE) {		/* Special quote processing loop */
      c = fgetc(fp);
      switch (c) {
      case EOF:
	goto done;	    /* Exit on EOF . . . */
      case '"':
	*buffer++ = '"';    /* Store matching quote */
	length++;
	if (length < *bufsiz - 1) {
	  goto mainloop;	/* And continue main loop */
	} else {
	  goto done;
	}
      case '\\':
	if ((c = fgetc(fp)) < 0) {	/* Backslash */
	  goto done;			/* EOF. . . .*/
	}		    /* else fall through */
      default:
	*buffer++ = c;	    /* Other character, store it */
	length++;
	if (length >= *bufsiz - 1) {
	  goto done;
	}
      }
    }
  case ':':
    *buffer++ = c;		/* Store colons */
    length++;
    if (length >= *bufsiz - 1) {
      goto done;
    }
  colonloop:
    c = fgetc(fp);		/* But remove whitespace after them */
    if ((c < 0) || (c == '\n')) {
      goto done;
    }
    if (isspace(c)) {
      goto colonloop;		/* Skip whitespace */
    }
    if (c == '"') {
      ungetc(c, fp);
      goto mainloop;
    }
    if (c == '\\') {		/* Backslash quotes next character */
      c = fgetc(fp);
      if (c < 0) {
	goto done;
      }
      if (c == '\n') {
	goto top;		/* Backslash-newline continuation */
      }
    }
    /* fall through if "other" character */
  default:
    *buffer++ = c;		/* Store other characters */
    length++;
    if (length >= *bufsiz - 1) {
      goto done;
    }
  }
  goto mainloop;			/* Keep going */
  
 done:
  *buffer = '\0';			/* Terminate string */
  *bufsiz = length;			/* Tell the caller its length */
}


/*
 * Parse out all the various tags and parameters in the host entry pointed
 * to by "src".  Stuff all the data into the appropriate fields of the
 * host structure pointed to by "host".  If there is any problem with the
 * entry, an error message is reported via syslog(), no further processing
 * is done, and -1 is returned.  Successful calls return 0.
 *
 * (Some errors probably shouldn't be so completely fatal. . . .)
 */
static int
process_entry(rp, src)
  struct dhcp_resource *rp;
  char *src;
{
  if (!rp || *src == '\0') {
    return(-1);
  }
  rp->entryname = get_string(&src);
  
  while (1) {
    adjust(&src);
    switch (eval_symbol(&src, rp)) {
    case E_END_OF_ENTRY:
      /*
       * And now we're done with this entry
       */
      return(0);
    case SUCCESS:
      break;
    default:
      return(-1);
      break;
    }
  }
}

/*
 * This function adjusts the caller's pointer to point just past the
 * first-encountered colon.  If it runs into a null character, it leaves
 * the pointer pointing to it.
 */
static void adjust(s)
  char **s;
{
  register char *t = NULL;

  t = *s;
  while (*t && (*t != ':')) {
    t++;
  }
  if (*t) {
    t++;
  }
  *s = t;
}


/*
 * Evaluate the two-character tag symbol pointed to by "symbol" and place
 * the data in the structure pointed to by "hostinfo".  The pointer pointed to
 * by "symbol" is updated to point past the source string (but may not
 * point to the next tag entry).
 *
 * Obviously, this need a few more comments. . . .
 */

static int
eval_symbol(symbol, rp)
  char **symbol;
  struct dhcp_resource *rp;
{
  struct symbolmap *symbolptr = NULL;
  int i = 0;
  int result;
  int optype = 0;                    /* Indicates boolean, addition, or deletion */
  int active = 0;
  
  if ((*symbol)[0] == '\0') {
    return(E_END_OF_ENTRY);
  }
  if ((*symbol)[0] == ':') {
    return(SUCCESS);
  }
  
  eat_whitespace(symbol);
  
  /*
   * Is it active parameter? 
   * (this parameter is not default of Host Requirements RFC)
   */

  if ((*symbol)[0] == '!') {
    active = TRUE;
    (*symbol)++;
  }
  else {
    active = FALSE;
  }
  /*
   * Determine the type of operation to be done on this symbol
   */
  switch ((*symbol)[4]) {
  case '=':
    optype = OP_ADDITION;
    break;
  case '@':
    optype = OP_DELETION;
    break;
  default:
    syslog(LOG_WARNING,
	   "Syntax error in the resource database. there is neither = nor @ after the symbol");
    return(-1);
  }
  
  symbolptr = symbol_list;
  for (i = 0; i <= S_LAST_OPTION; i++) {
    if (((symbolptr->symbol)[0] == (*symbol)[0]) &&
	((symbolptr->symbol)[1] == (*symbol)[1]) &&
	((symbolptr->symbol)[2] == (*symbol)[2]) &&
	((symbolptr->symbol)[3] == (*symbol)[3])) {
      break;
    }
    symbolptr++;
  }
  if (i > S_LAST_OPTION) {
    syslog(LOG_WARNING, "unknown symbol \"%c%c%c%c\" in \"%s\"", (*symbol)[0],
	   (*symbol)[1], (*symbol)[2], (*symbol)[3], rp->entryname);
    return(-1);
  }
  
  /*
   * Skip past the = or @ character (to point to the data) if this
   * isn't a boolean operation.  For boolean operations, just skip
   * over the two-character tag symbol (and nothing else. . . .).
   */
  (*symbol) += 5;

  if (symbolptr->func != NULL) {
    result = (*symbolptr->func)(symbolptr->code, optype, symbol, rp);
    if (symbolptr->code != S_TABLE_CONT) {
      if (optype == OP_DELETION || result != 0) {
	clrbit(rp->valid, symbolptr->code);
	clrbit(rp->active, symbolptr->code);
      } else {
	setbit(rp->valid, symbolptr->code);
	if (active)
          setbit(rp->active, symbolptr->code);
        else
          clrbit(rp->active, symbolptr->code);
      }
    }
    return(result);
  } else {
    errno = 0;
    syslog(LOG_WARNING, "There is no corresponding function.");
    return(-1);
  }
}


/*
 * This function adjusts the caller's pointer to point to the first
 * non-whitespace character.  If it runs into a null character, it leaves
 * the pointer pointing to it.
 */

static void
eat_whitespace(s)
  char **s;
{
  register char *t = NULL;

  t = *s;
  while (*t && isspace(*t)) {
    t++;
  }
  *s = t;
}


static char *
get_string(src)
  char **src;
{
  int n = 0, len = 0, quoteflag = 0;
  char tmp[MAXSTRINGLEN], *retp = NULL, *tmpp = NULL;

  tmpp = tmp;
  quoteflag = FALSE;
  n = 0;
  len = MAXSTRINGLEN - 1;
  while ((n < MAXSTRINGLEN - 1) && (**src)) {
    if (!quoteflag && (**src == ':')) {
      break;
    }
    if (**src == '"') {
      (*src)++;
      quoteflag = !quoteflag;
      continue;
    }
    if (**src == '\\') {
      (*src)++;
      if (! **src) {
	break;
      }
    }
    *tmpp++ = *(*src)++;
    n++;
    }
  
  /*
   * Remove that troublesome trailing whitespace. . .
   */
  while ((n > 0) && isspace(tmpp[-1])) {
    tmpp--;
    n--;
  }

  *tmpp = '\0';
  if ((retp = calloc(1, strlen(tmp) + 1)) == NULL) {
    syslog(LOG_ERR, "calloc error in get_string()");
    exit(1);
  }
  strcpy(retp, tmp);
  return(retp);
}


/*
 * The parameter "src" points to a character pointer which points to an
 * ASCII string representation of an unsigned number.  The number is
 * returned as an unsigned long and the character pointer is updated to
 * point to the first illegal character.
 */

static Long
get_integer(src)
  char **src;
{
  register Long value = 0, base = 0;
  int invert = 1;
  char c = 0;

  /*
   * Collect number up to first illegal character.  Values are specified
   * as for C:  0x=hex, 0=octal, other=decimal.
   */
  value = 0;
  base = 10;
  if (**src == '-') {
    invert = -1;
    (*src)++;
  }
  if (**src == '0') {
    base = 8;
    (*src)++;
  }
  if (**src == 'x' || **src == 'X') {
    base = 16;
    (*src)++;
  }
  while ((c = **src)) {
    if (isdigit(c)) {
      value = (value * base) + (c - '0');
      (*src)++;
      continue;
    }
    if (base == 16 && isxdigit(c)) {
      value = (value << 4) + ((c & ~32) + 10 - 'A');
      (*src)++;
      continue;
    }
    break;
  }
  return(invert * value);
}


/*
 * prs_inaddr(src, result)
 *
 * "src" is a value-result parameter; the pointer it points to is updated
 * to point to the next data position.   "result" points to an unsigned long
 * in which an address is returned.
 *
 * This function parses the IP address string in ASCII "dot notation" pointed
 * to by (*src) and places the result (in network byte order) in the unsigned
 * long pointed to by "result".  For malformed addresses, -1 is returned,
 * (*src) points to the first illegal character, and the unsigned long pointed
 * to by "result" is unchanged.  Successful calls return 0.
 */

static int
prs_inaddr(src, result)
  char **src;
  u_Long *result;
{
  register u_Long value = 0;
  u_Long parts[4], *pp = parts;
  int n;
  
  if (!isdigit(**src)) {
    return(-1);
  }
 loop:
  value = get_integer(src);
  if (**src == '.') {
    /*
     * Internet format:
     *	a.b.c.d
     *	a.b.c	(with c treated as 16-bits)
     *	a.b	(with b treated as 24 bits)
     */
    if (pp >= parts + 4) {
      return(-1);
    }
    *pp++ = value;
    (*src)++;
    goto loop;
  }
  /*
   * Check for trailing characters.
   */
  if (**src && !(isspace(**src) || (**src == ':'))) {
    return(-1);
  }
  *pp++ = value;
  /*
   * Construct the address according to
   * the number of parts specified.
   */
  n = pp - parts;
  switch (n) {
  case 1:				/* a -- 32 bits */
    value = parts[0];
    break;
  case 2:				/* a.b -- 8.24 bits */
    value = (parts[0] << 24) | (parts[1] & 0xFFFFFF);
    break;
  case 3:				/* a.b.c -- 8.8.16 bits */
    value = (parts[0] << 24) | ((parts[1] & 0xFF) << 16) |
      (parts[2] & 0xFFFF);
    break;
  case 4:				/* a.b.c.d -- 8.8.8.8 bits */
    value = (parts[0] << 24) | ((parts[1] & 0xFF) << 16) |
      ((parts[2] & 0xFF) << 8) | (parts[3] & 0xFF);
    break;
  default:
    return(-1);
  }
  *result = htonl(value);
  return(0);
}


/*
 * Function for comparing a string with the entryname field of a 
 * resource information structure.
 */
static int
resnmcmp(name, rp)
  char *name;
  struct dhcp_resource *rp;
{
  return !strcmp(name, rp->entryname);
}
