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

#include <net/if.h>
#include <net/bpf.h>
#ifdef __osf__
#include <net/pfilt.h>
#endif
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/if_ether.h>
#include <arpa/inet.h>

#include "dhcp.h"

#ifndef NBPFILTER
#define NBPFILTER              10
#endif /* NBPFILTER */
#define BPFHL      18

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

struct if_info {
  int fd;
  char name[sizeof "enxx"];
  int bufsize;
  char *buf;
};

/*
 * bpf filter program
 */
#ifndef sony_news
static struct bpf_insn dhcpf[] = {
  BPF_STMT(BPF_LD+BPF_H+BPF_ABS, 12),                   /* A <- ETHER_TYPE */
  BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, ETHERTYPE_IP, 0, 9),  /* is it IP ? */
  BPF_STMT(BPF_LD+BPF_H+BPF_ABS, 20),                   /* A <- IP FRAGMENT */
  BPF_JUMP(BPF_JMP+BPF_JSET+BPF_K, 0x1fff, 7, 0),       /* OFFSET == 0 ? */
  BPF_STMT(BPF_LD+BPF_B+BPF_ABS, 23),                   /* A <- IP_PROTO */
  BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, IPPROTO_UDP, 0, 5),   /* UDP ? */
  BPF_STMT(BPF_LDX+BPF_B+BPF_MSH, 14),                  /* X <- IPHDR LEN */
  BPF_STMT(BPF_LD+BPF_H+BPF_IND, 16),                   /* A <- UDP DSTPORT */
  BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, DHCPS_PORT, 1, 0),    /* check DSTPORT */
  BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, DHCPC_PORT, 0, 1),    /* check DSTPORT */
  BPF_STMT(BPF_RET+BPF_K, (u_int)-1),                   /* return all*/
  BPF_STMT(BPF_RET+BPF_K, 0)                            /* ignore */
};
#else /* not sony_news */
static struct bpf_insn dhcpf[] = {
  BPF_STMT(LdHOp, 12),                   /* A <- ETHER_TYPE */
  BPF_JUMP(EQOp, ETHERTYPE_IP, 1, 11),   /* is it IP ? */
  BPF_STMT(LdHOp, 20),                   /* A <- IP FRAGMENT */
  BPF_STMT(AndIOp, 0x1fff),              /* A <- A & 0x1fff */
  BPF_JUMP(EQOp, 0, 1, 8),               /* OFFSET == 0 ? */
  BPF_STMT(LdBOp, 23),                   /* A <- IP_PROTO */
  BPF_JUMP(EQOp, IPPROTO_UDP, 1, 6),     /* UDP ? */
  BPF_STMT(LdxmsOp, 14),                 /* X <- IPHDR LEN */
  BPF_STMT(ILdHOp, 16),                  /* A <- UDP DSTPORT */
  BPF_JUMP(EQOp, DHCPS_PORT, 2, 1),      /* check DSTPORT */
  BPF_JUMP(EQOp, DHCPC_PORT, 1, 2),      /* check DSTPORT */
  BPF_STMT(RetOp, (u_int)-1),            /* return all*/
  BPF_STMT(RetOp, 0)                     /* ignore */
};
#endif /* sony_news */

static struct bpf_program dhcpfilter = {
  sizeof(dhcpf) / sizeof(struct bpf_insn),
  dhcpf
};

static unsigned char magic_c[MAGIC_LEN] = RFC1048_MAGIC;

int
if_init(ifinfo)
  struct if_info *ifinfo;
{
  int n;
  char dev[sizeof "/dev/pf/pfiltxx"];
  struct ifreq ifr;
#ifdef __osf__
  u_short bits;
#endif

  /* open /dev/bpf?? */
  n = 0;
  do {
#ifdef __osf__
    sprintf(dev, "/dev/pf/pfilt%d", n++);
#else
    sprintf(dev, "/dev/bpf%d", n++);
#endif
    ifinfo->fd = open(dev, O_RDWR);
  } while (ifinfo->fd < 0 && n < NBPFILTER);
  if (ifinfo->fd < 0) {
    perror("can't open bpf");
    return(-1);
  }

  /* bind interface to bpf */
  strcpy(ifr.ifr_name, ifinfo->name);
  if (ioctl(ifinfo->fd, BIOCSETIF, &ifr) < 0) {
    perror("ioctl(BIOCSETIF error)");
    return(-1);
  }

  /* set promiscuous mode */
  n = 1;
  if (ioctl(ifinfo->fd, BIOCPROMISC, &n) < 0) {
    perror("ioctl(BIOCPROMISC) error");
    return(-1);
  }

#ifdef __osf__
  /* make packetfilt use BPF header */
  bits = ENBPFHDR;
  if (ioctl(ifinfo->fd, EIOCMBIS, &bits) < 0) {
    perror("ioctl(EIOCMBIS) error");
    exit(1);
  }
#endif

  /* set immediate mode */
  n = 1;
  if (ioctl(ifinfo->fd, BIOCIMMEDIATE, &n) < 0) {
    perror("ioctl(BIOCIMMEDIATE) error");
    return(-1);
  }

  /* get buffer_length and allocate buffer */
  if (ioctl(ifinfo->fd, BIOCGBLEN, &ifinfo->bufsize) < 0) {
    perror("ioctl(BIOCGBLEN) error");
    return(-1);
  }
  if ((ifinfo->buf = calloc(1, ifinfo->bufsize)) == NULL) {
    perror("calloc() error");
    return(-1);
  }

  /* bind filter program to interface */
  if (ioctl(ifinfo->fd, BIOCSETF, &dhcpfilter) < 0) {
    perror("ioctl(BIOCSETF) error");
    return(-1);
  }

  /* flush buffer */
  if (ioctl(ifinfo->fd, BIOCFLUSH) < 0) {
    perror("ioctl(BIOCFLUSH) error");
    return(-1);
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
  struct bpf_hdr *rbpf;
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
    if ((n = read(netif.fd, netif.buf, netif.bufsize)) < 0) {
      return(-1);
    }

    rbufp = netif.buf;
    while (n > 0) {
      rbpf = (struct bpf_hdr *) rbufp;
      rether = (struct ether_header *) &rbufp[rbpf->bh_hdrlen];
      rip = (struct ip *) &rbufp[rbpf->bh_hdrlen + ETHERHL];
      rudp = (struct udphdr *) &rbufp[rbpf->bh_hdrlen + ETHERHL + rip->ip_hl * WORD];
      rdhcp = (struct dhcp *)
	&rbufp[rbpf->bh_hdrlen + ETHERHL + rip->ip_hl * WORD + UDPHL];

      if (ntohs(rip->ip_len) >= DFLTBOOTPLEN + UDPHL + IPHL &&
	  ntohs(rudp->uh_ulen) >= DFLTBOOTPLEN + UDPHL &&
	  (rdhcp->op == BOOTREQUEST || rdhcp->op == BOOTREPLY) &&
	  (bcmp(rdhcp->options, magic_c, MAGIC_LEN) == 0 ||
           GETHL(rdhcp->options) == 0)) { /* permit old BOOTP packet */

	tmp = ctime((time_t *) &rbpf->bh_tstamp.tv_sec);
	tmp[strlen(tmp) - 6] = '\0';
	printf("\"%s.%ld\"\t", &tmp[4], rbpf->bh_tstamp.tv_usec);
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
	       rether->ether_shost[0], rether->ether_shost[1],
	       rether->ether_shost[2], rether->ether_shost[3],
	       rether->ether_shost[4], rether->ether_shost[5]);
	printf(" dst_haddr: %.2x%.2x%.2x%.2x%.2x%.2x\n",
	       rether->ether_dhost[0], rether->ether_dhost[1],
	       rether->ether_dhost[2], rether->ether_dhost[3],
	       rether->ether_dhost[4], rether->ether_dhost[5]);
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
      rbufp += BPF_WORDALIGN(rbpf->bh_hdrlen + rbpf->bh_caplen);
      n -= (char *) rbufp - (char *) rbpf;
    }
  }

  return(0);
}
