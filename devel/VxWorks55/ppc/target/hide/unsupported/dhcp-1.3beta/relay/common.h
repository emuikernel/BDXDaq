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

#ifndef SERVER_DB
#define SERVER_DB                   "/etc/dhcpdb.server"
#endif
#ifndef TRUE
#define TRUE    1
#endif
#ifndef NBPFILTER
#define NBPFILTER 10
#endif

#define HOPSTHRESHOLD             4

#define ETHERHL       sizeof(struct ether_header)  /* ethernet header length */
#define IPHL          sizeof(struct ip)            /* IP header length */
#define UDPHL         sizeof(struct udphdr)        /* UDP header length */
#define BOOTPLEN      300
#define WORD          4                            /* word alignment in bits */
#define	QWOFF         2                 /* to make dhcp long to 4w alignment */

/* struct definition */
struct if_info {
  int fd;
  char name[sizeof "enxx"];
  unsigned int htype;
  unsigned char hlen;
  char haddr[6];
  struct in_addr *ipaddr;
  struct in_addr *subnetmask;
  int buf_size;
  char *buf;
  struct if_info *next;				/* Null terminate */
};

struct msg
{
  struct ether_header *ether;
  struct ip *ip;
  struct udphdr *udp;
  struct dhcp *dhcp;
};

extern u_short dhcps_port;      /* DHCP server port (network byte order) */
extern u_short dhcpc_port;

#ifndef sun
extern struct bpf_hdr *rbpf;        /* pointer to bpf header */
#endif
extern struct msg rcv;
