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
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <stropts.h>

#include <net/if.h>
#include <net/nit.h>
#include <net/nit_if.h>
#include <net/nit_pf.h>
#include <net/packetfilt.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/if_ether.h>
#include <arpa/inet.h>

#include "dhcp.h"

#define ETHERHL    sizeof(struct ether_header)
#define IPHL       sizeof(struct ip)
#define UDPHL      sizeof(struct udphdr)
#define DFLTDHCPLEN   sizeof(struct dhcp)   /* default DHCP message size */
#define WORD       4
#define OPTLEN(TAGP)   (*(((char *)TAGP) + 1))       /* get length of DHCP option */
#define OPTBODY(TAGP)  (((char *)TAGP) + 2)          /* get content of DHCP option */
#define DHCPLEN(UDP)  (UDP->uh_ulen - UDPHL)         /* get DHCP message size from
							struct udp */

/* handle word alignment */
#define GETHS(PTR)   (*((u_char *)PTR)*256 + *(((u_char *)PTR)+1))
#define GETHL(PTR)   (*((u_char *)PTR)*256*256*256 + *(((u_char *)PTR)+1)*256*256 +\
		      *(((u_char *)PTR)+2)*256 + *(((u_char *)PTR)+3))
#define DFLTOPTLEN   312
#define QWOFF          2                      /* to make dhcp long to 4w alignment */


struct if_info {
  int fd;
  char name[sizeof "enxx"];
  int bufsize;
  char *buf;
};

static unsigned char magic_c[MAGIC_LEN] = RFC1048_MAGIC;

int
if_init(ifinfo)
  struct if_info *ifinfo;
{
  u_long i = 0;
  struct ifreq ifreq;
  struct strioctl si;
  struct packetfilt pf;
  register u_short *fwp = &pf.Pf_Filter[0];

  /*
   *    open /dev/nit
   */
  if ((ifinfo->fd = open("/dev/nit", O_RDWR)) < 0) {        /* cannot open nit */
    fprintf(stderr, "open error in if_init()\n");
    exit(1);
  }

  if (ioctl(ifinfo->fd, I_SRDOPT, (char *)RMSGD) < 0) {
    fprintf(stderr, "ioctl(I_SRDOPT) error in if_init()\n");
    exit(1);
  }

  bzero(&si, sizeof(si));
  bzero(&ifreq, sizeof(ifreq));
  strncpy(ifreq.ifr_name, ifinfo->name, sizeof(ifreq.ifr_name));
  ifreq.ifr_name[sizeof(ifreq.ifr_name) - 1] = '\0';

  si.ic_cmd = NIOCBIND;
  si.ic_len = sizeof(ifreq);
  si.ic_dp = (char *) &ifreq;
  if (ioctl(ifinfo->fd, I_STR, (char *) &si) < 0) {
    fprintf(stderr, "ioctl(NIOCBIND) error in if_init()\n");
    exit(1);
  }

  bzero(&si, sizeof(si));
  si.ic_cmd = NIOCSSNAP;
  si.ic_len = sizeof(i);
  si.ic_dp = (char *)&i;
  if (ioctl(ifinfo->fd, I_STR, (char *) &si) < 0) {
    fprintf(stderr, "ioctl(NIOCSSNAP) error in if_init()\n");
    exit(1);
  }

  bzero(&si, sizeof(si));
  i = NI_PROMISC;
  si.ic_cmd = NIOCSFLAGS;
  si.ic_len = sizeof(i);
  si.ic_dp = (char *)&i;
  if (ioctl(ifinfo->fd, I_STR, (char *) &si) < 0) {
    fprintf(stderr, "ioctl(NIOCSFLAGS) error in if_init()\n");
    exit(1);
  }

  if (ioctl(ifinfo->fd, I_PUSH, "pf") < 0) {
    fprintf(stderr, "ioctl(I_PUSH) error in if_init()\n");
    exit(1);
  }

  *fwp++ = ENF_PUSHWORD + 6;
  *fwp++ = ENF_PUSHLIT | ENF_CAND;
  *fwp++ = htons(ETHERTYPE_IP);
  *fwp++ = ENF_PUSHWORD + 11;
  *fwp++ = ENF_PUSHLIT | ENF_AND;
  *fwp++ = htons(0x00ff);
  *fwp++ = ENF_PUSHLIT | ENF_CAND;
  *fwp++ = htons(IPPROTO_UDP);
#ifdef NOIPOPTION
  *fwp++ = ENF_PUSHWORD + 18;
  *fwp++ = ENF_PUSHLIT | ENF_COR;
  *fwp++ = htons(DHCPS_PORT);
  *fwp++ = ENF_PUSHWORD + 18;
  *fwp++ = ENF_PUSHLIT | ENF_CAND;
  *fwp++ = htons(DHCPC_PORT);
#endif
  pf.Pf_FilterLen = fwp - &pf.Pf_Filter[0];

  bzero(&si, sizeof(si));
  si.ic_cmd = NIOCSETF;
  si.ic_timout = INFTIM;
  si.ic_len = sizeof (pf);
  si.ic_dp = (char *)&pf;
  if (ioctl(ifinfo->fd, I_STR, (char *)&si) < 0){
    fprintf(stderr, "ioctl(NIOCSETF) error in if_init()\n");
    exit(1);
  }

  if (ioctl(ifinfo->fd, I_FLUSH, (char *)FLUSHR) < 0) {
    fprintf(stderr, "ioctl(I_FLUSH) error in if_init()\n");
    exit(1);
  }

  ifinfo->bufsize = 8192;  /* XXX */
  if ((ifinfo->buf = (char *)calloc(1, ifinfo->bufsize)) == NULL) {
    fprintf(stderr, "calloc() error in if_init()\n");
    exit(1);
  }

  return(0);
}


char *
pickup_opt(msg, msglen, tag)
  struct dhcp *msg;
  int msglen;
  char tag;
{
  int sname_is_opt = 0;
  int file_is_opt = 0;
  int i = 0;

  char *opt = NULL;
  char *found = NULL;

  /*
   *  search option field.
   */
  opt = &msg->options[MAGIC_LEN];
  /* DHCPLEN - OPTLEN == from 'op' to 'file' field */
  for (i = 0; i < msglen - DFLTDHCPLEN + DFLTOPTLEN - MAGIC_LEN; i++) {
    if (*(opt + i) == tag) {
      found = (opt + i);
      break;
    }
    else if (*(opt+i) == END) {
      break;
    }
    else if (*(opt+i) == OPT_OVERLOAD) {
      i += 2 ;
      if (*(opt+i) == 1)
	file_is_opt = 1;
      else if (*(opt+i) == 2)
	sname_is_opt = 1;
      else if (*(opt+i) == 3)
	file_is_opt = sname_is_opt = 1;
      continue;
    }
    else if (*(opt+i) == PAD) {
      continue;
    }
    else {
      i += *(u_char *)(opt + i + 1) + 1;
    }
  }
  if (found != NULL)
    return(found);

  /*
   *  if necessary, search file field
   */
  if (file_is_opt) {
    opt = msg->file;
    for (i = 0; i < sizeof(msg->file); i++) {
      if (*(opt+i) == PAD) {
	continue;
      }
      else if (*(opt+i) == END) {
	break;
      }
      else if (*(opt + i) == tag) {
	found = (opt + i);
	break;
      }
      else {
	i += *(u_char *)(opt + i + 1) + 1;
      }
    }
    if (found != NULL)
      return(found);
  }

  /*
   *  if necessary, search sname field
   */
  if (sname_is_opt) {
    opt = msg->sname;
    for (i = 0; i < sizeof(msg->sname); i++) {
      if (*(opt+i) == PAD) {
	continue;
      }
      else if (*(opt+i) == END) {
	break;
      }
      else if (*(opt + i) == tag) {
	found = (opt + i);
	break;
      }
      else {
	i += *(u_char *)(opt + i + 1) + 1;
      }
    }
    if (found != NULL)
      return(found);
  }

  return(NULL);
}


int
main(argc, argv)
  int argc;
  char **argv;
{
  int n;
  char *rbufp;
  char *option;
  char *tmp;
  struct if_info netif;
  struct ether_header *rether;
  struct ip *rip;
  struct udphdr *rudp;
  struct dhcp *rdhcp;
  struct in_addr tmpip;

  if (argc < 2) {
    fprintf(stderr, "Usage: %s interface_name [filename]\n", argv[0]);
    return(-1);
  }

  strcpy(netif.name, argv[1]);
  if (if_init(&netif) < 0) {
    return(-1);
  }

  if (argc > 2) {
    fclose(stdout);
    fopen(argv[2], "w");
  }

  while (1) {
    n = 0;
    if ((n = read(netif.fd, &netif.buf[QWOFF], netif.bufsize - QWOFF)) < 0) {
      return(-1);
    }

    rbufp = &netif.buf[QWOFF];
    rether = (struct ether_header *) rbufp;
    rip = (struct ip *) &rbufp[ETHERHL];

    if ((ntohs(rip->ip_off) & 0x1fff) == 0 &&
	ntohs(rip->ip_len) >= DFLTBOOTPLEN + UDPHL + IPHL)
      rudp = (struct udphdr *)&rbufp[ETHERHL+rip->ip_hl*WORD];
    else
      continue;

    if (ntohs(rudp->uh_ulen) >= DFLTBOOTPLEN + UDPHL &&
        (rudp->uh_dport == htons(DHCPS_PORT) ||
	 rudp->uh_dport == htons(DHCPC_PORT)))
      rdhcp = (struct dhcp *)&rbufp[ETHERHL+rip->ip_hl*WORD+UDPHL];
    else
      continue;

    if ((rdhcp->op == BOOTREQUEST || rdhcp->op == BOOTREPLY) &&
	(bcmp(rdhcp->options, magic_c, MAGIC_LEN) == 0 ||
         GETHL(rdhcp->options) == 0)) {
      time_t tmptime = time();
      tmp = ctime(&tmptime);
      tmp[strlen(tmp) - 6] = '\0';
      printf("\"%s\"\t", &tmp[4]);
      if ((option = pickup_opt(rdhcp, DHCPLEN(rudp), DHCP_MSGTYPE)) != NULL) {
	switch (*OPTBODY(option)) {
	case DHCPDISCOVER:
	  printf("DHCPDISCOVER\n");
	  break;
	case DHCPOFFER:
	  printf("DHCPOFFER\n");
	  break;
	case DHCPREQUEST:
	  printf("DHCPREQUEST\n");
	  break;
	case DHCPDECLINE:
	  printf("DHCPDECLINE\n");
	  break;
	case DHCPACK:
	  printf("DHCPACK\n");
	  break;
	case DHCPNAK:
	  printf("DHCPNAK\n");
	  break;
	case DHCPRELEASE:
	  printf("DHCPRELEASE\n");
	  break;
	default:
	  break;
	}
      } else {
	if (rdhcp->op == BOOTREQUEST)
	  printf("pure BOOTP REQUEST\n");
	else
	  printf("pure BOOTP REPLY\n");
      }

      printf("\tsrc_haddr: %.2x%.2x%.2x%.2x%.2x%.2x,",
	     rether->ether_shost.ether_addr_octet[0],
	     rether->ether_shost.ether_addr_octet[1],
	     rether->ether_shost.ether_addr_octet[2],
	     rether->ether_shost.ether_addr_octet[3],
	     rether->ether_shost.ether_addr_octet[4],
	     rether->ether_shost.ether_addr_octet[5]);
      printf(" dst_haddr: %.2x%.2x%.2x%.2x%.2x%.2x\n",
	     rether->ether_dhost.ether_addr_octet[0],
	     rether->ether_dhost.ether_addr_octet[1],
	     rether->ether_dhost.ether_addr_octet[2],
	     rether->ether_dhost.ether_addr_octet[3],
	     rether->ether_dhost.ether_addr_octet[4],
	     rether->ether_dhost.ether_addr_octet[5]);
      printf("\tsrc_IP: %s,", inet_ntoa(rip->ip_src));
      printf(" dst_IP: %s\n", inet_ntoa(rip->ip_dst));
        if ((option = pickup_opt(rdhcp, DHCPLEN(rudp), REQUEST_IPADDR)) != NULL)
          tmpip.s_addr = htonl(GETHL(OPTBODY(option)));
        else
          tmpip.s_addr = 0;
	printf("\trequested_IP: %s\n",
	       (tmpip.s_addr != 0) ? inet_ntoa(tmpip) : "None");
	printf("\trequested/assigned lease: %u\n",
	       ((option = pickup_opt(rdhcp, DHCPLEN(rudp), LEASE_TIME)) != NULL) ?
	       GETHL(OPTBODY(option)) : 0);
	printf("\tdhcp_t1: %u",
	       ((option = pickup_opt(rdhcp, DHCPLEN(rudp), DHCP_T1)) != NULL) ?
	       GETHL(OPTBODY(option)) : 0);
	printf("\tdhcp_t2: %u\n",
	       ((option = pickup_opt(rdhcp, DHCPLEN(rudp), DHCP_T2)) != NULL) ?
	       GETHL(OPTBODY(option)) : 0);
	printf("\top: %d, xid: %lx, secs: %d, BRDCST flag: %d\n", rdhcp->op,
	       ntohl(rdhcp->xid), ntohs(rdhcp->secs), ISBRDCST(rdhcp->flags) ? 1:0);
	printf("\tciaddr: %s, ", inet_ntoa(rdhcp->ciaddr));
	printf("yiaddr: %s, ", inet_ntoa(rdhcp->yiaddr));
	printf("siaddr: %s, ", inet_ntoa(rdhcp->siaddr));
	printf("giaddr: %s\n", inet_ntoa(rdhcp->giaddr));
        if ((option = pickup_opt(rdhcp, DHCPLEN(rudp), BRDCAST_ADDR)) != NULL)
          tmpip.s_addr = htonl(GETHL(OPTBODY(option)));
        else
          tmpip.s_addr = 0;
	printf("\tbroadcast: %s", (tmpip.s_addr != 0) ? inet_ntoa(tmpip) : "None");
        if ((option = pickup_opt(rdhcp, DHCPLEN(rudp), SUBNET_MASK)) != NULL)
          tmpip.s_addr = htonl(GETHL(OPTBODY(option)));
        else
          tmpip.s_addr = 0;
	printf("\tsubnetmask: %s", (tmpip.s_addr != 0) ? inet_ntoa(tmpip) : "None");
	if ((option = pickup_opt(rdhcp, DHCPLEN(rudp), SERVER_ID)) != NULL)
	  tmpip.s_addr = htonl(GETHL(OPTBODY(option)));
	else
	  tmpip.s_addr = 0;
	printf("\n\tserver_id: %s", (tmpip.s_addr != 0) ? inet_ntoa(tmpip) : "None");
	printf("\n");
        if ((option = pickup_opt(rdhcp, DHCPLEN(rudp), ROUTER)) != NULL)
          tmpip.s_addr = htonl(GETHL(OPTBODY(option)));
        else
          tmpip.s_addr = 0;
	printf("\tdefault router: %s",
               (tmpip.s_addr != 0) ? inet_ntoa(tmpip) : "None");
	printf("\n");
	fflush(stdout);
    }
  }

  return(0);
}
