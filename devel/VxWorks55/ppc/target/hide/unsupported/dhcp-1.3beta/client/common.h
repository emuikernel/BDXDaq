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

#include "dhcpc.h"

#ifndef TRUE
#define TRUE   1
#endif
#ifndef FALSE
#define FALSE  0
#endif
#ifndef NBPFILTER
#define NBPFILTER   10
#endif

#define  INIT         0
#define  WAIT_OFFER   1
#define  SELECTING    2
#define  REQUESTING   3
#define  BOUND        4
#define  RENEWING     5
#define  REBINDING    6
#define  INIT_REBOOT  7
#define  VERIFY       8
#define  REBOOTING    9
#define  VERIFYING   10
#define  MAX_STATES  VERIFYING + 1

#define OPTLEN(TAGP)   (*(((char *)TAGP) + 1))       /* get length of DHCP option */
#define OPTBODY(TAGP)  (((char *)TAGP) + 2)          /* get content of DHCP option */
#define DHCPLEN(UDP)  (ntohs(UDP->uh_ulen) - UDPHL)  /* get DHCP message size from
							struct udp */
#define CHKOFF(LEN)  (offopt + 2 + (LEN) < DFLTOPTLEN) /* check the option offset */

#define ETHERHL    sizeof(struct ether_header)
#define IPHL       sizeof(struct ip)
#define UDPHL      sizeof(struct udphdr)
#define WORD       4
#define QWOFF      2

#define OK         0

extern unsigned char magic_c[MAGIC_LEN];

int init();
int wait_offer();
int selecting();
int requesting();
int bound();
int renewing();
int rebinding();
int init_reboot();
int verify();
int reboot_verify();

int handle_ip();
int handle_num();
int handle_ips();
int handle_str();
int handle_bool();
int handle_ippairs();
int handle_nums();


/*
 * global variable
 */
extern int (*handle_param[MAXTAGNUM])();
extern int (*fsm[MAX_STATES])();

struct buffer
{
  char *buf;
  int size;
};

struct msg
{
  struct ether_header *ether;
  struct ip *ip;
  struct udphdr *udp;
  struct dhcp *dhcp;
};

extern struct buffer sbuf;
extern int    curr_state, prev_state;
extern int    interrupt, interrupt2;
extern u_short   dhcps_port,     /* DHCP server port (network byte order) */
                 dhcpc_port;     /* DHCP client port (network byte order) */
extern time_t init_epoch;
extern time_t send_epoch;
extern struct dhcp_reqspec reqspec;
extern struct if_info intface;
extern struct dhcp_param *param_list;

#ifndef sun
extern struct bpf_hdr *rbpf;
#endif
extern struct ps_udph spudph;
extern struct msg snd;
extern struct msg rcv;
