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

struct interface {
  int fd;
  char *rbuf;
  int rbufsize;
  struct if_info *ifinfo;
};

struct interface dhcpif;
struct interface arpif;

/*
 * following are included in dhcpc_subr.c
 */
int   initialize();
int   dhcp_decline();
int   dhcp_release();
int   clean_param();
int   merge_param();
int   dhcp_msgtoparam();
int   arp_check();
int   arp_reply();
int   *nvttostr();
int   send_unicast();
int   make_request();
char  *pickup_opt();
void  reset_if();
void  down_if();
void  set_declinfo();
void  set_relinfo();
void  make_discover();

/*
 * flushroutes() is included in flushroute.c
 */
extern int  flushroutes();

/*
 * getmac() is included in getmac.c
 */
#if !defined(sony_news) && !defined(__FreeBSD__) && !defined(__osf__)
extern int  getmac();
#endif
