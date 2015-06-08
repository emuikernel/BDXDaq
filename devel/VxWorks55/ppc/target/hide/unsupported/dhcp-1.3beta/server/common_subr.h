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

struct ps_udph {              /* pseudo udp header for checksum */
  struct in_addr srcip,
                 dstip;
  char           zero,
                 prto;
  short          ulen;
};

#ifndef sun
void     align_msg();
#endif
void     set_srvport();
char     *pickup_opt();
int      check_ipsum();
int      check_udpsum();
u_short  udp_cksum();
u_short  get_ipsum();
u_short  get_udpsum();
u_short  cksum();
struct if_info *read_interfaces();

#ifdef sun
int ether_write();
int ether_writev();
#else
#define ether_write   write
#define ether_writev  writev
#endif
