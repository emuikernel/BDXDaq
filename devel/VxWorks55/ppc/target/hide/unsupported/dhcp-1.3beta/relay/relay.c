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
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <syslog.h>
#ifdef sun
#include <stropts.h>
#endif

#include <sys/types.h>
#include <sys/time.h>
#include <sys/uio.h>
#include <sys/ioctl.h>
#include <sys/param.h>
#include <sys/socket.h>

#include <net/if.h>
#ifdef sun
#include <net/nit.h>
#include <net/nit_if.h>
#include <net/nit_pf.h>
#include <net/packetfilt.h>
#else
#include <net/bpf.h>
#endif
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/if_ether.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>

#include "dhcp.h"
#include "common.h"
#include "common_subr.h"

#define DHCPLEN(UDP)  (ntohs(UDP->uh_ulen) - UDPHL)  /* get DHCP message size from
							struct udp */

/*
 *  global variable
 */
unsigned char magic_c[MAGIC_LEN] = RFC1048_MAGIC;  /* magic cookie which defined in
					     RFC1048 */
struct msg rcv;
struct bpf_hdr *rbpf;

struct server {
  struct in_addr ip;
  struct server *next;
};


static char server_dbname[MAXPATHLEN];
static char *rbufp;                   /* receive buffer */
static struct msg snd;

static struct server *srvlist = NULL;
static int nserver = 0;
static int nsend = 0;

#ifndef sun
static int check_pkt1();
#endif

static void usage();
static void version();
static void become_daemon();
static void read_server_db();
static int check_pkt2();
static void relay_tosrv();
static void relay_tocli();
static void forwarding();
int open_if();

void
main(argc, argv)
  int argc;
  char **argv;
{
  int n = 0;
  struct if_info *ifp = NULL;          /* pointer to interface */
  struct if_info *if_list = NULL;      /* interfaces list */
  int debug = 0;                       /* debug flag */
  struct sockaddr_in my_addr;

  /*
   * command line parsing
   */
  if (argc < 2) usage();

  while (--argc) {
    ++argv;

    /*
     * parse option
     */
    if (argv[0][0] == '-') {
      switch (argv[0][1]) {
      case 'f':               /* option which specify the DHCP server db file */
	--argc, ++argv;
	if (argc <= 0) usage();
	else strcpy(server_dbname, argv[0]);
	break;
      case 'd':               /* debug option */
	debug = 1;
	break;
      case 'v':
	version();
	break;
      default:
	usage();
	break;
      }
    } else {
      /*
       * make interfaces list
       */
      if ((ifp = (struct if_info *) calloc(1, sizeof(struct if_info))) == NULL) {
	perror("calloc error in main()");
	exit(1);
      }
      ifp->next = if_list;
      if_list = ifp;
      strcpy(ifp->name, argv[0]);
    }
  }

  /*
   * Initialize logging.
   */
#ifndef LOG_CONS
#define LOG_CONS	0	/* Don't bother if not defined... */
#endif
#ifndef LOG_PERROR
#define LOG_PERROR	0
#endif
    openlog("relay", LOG_PID | LOG_CONS | LOG_PERROR, LOG_LOCAL0);

  if (debug == 0) become_daemon();
  if (if_list == NULL) usage();

  read_server_db();         /* read relay agents database */
  set_srvport();           /* determine dhcp port */

  /* open the bpf for each interface */
  ifp = if_list;
  while (ifp != NULL) {
    if (open_if(ifp) < 0) {
      exit(1);
    }
    ifp = ifp->next;
  }
  if ((ifp = (struct if_info *) calloc(1, sizeof(struct if_info))) == NULL) {
    perror("calloc error in main()");
    exit(1);
  }
  if ((ifp->fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    free(ifp);
  } else {
    ifp->buf_size = 0xffff + 1024;   /* it seems enough large */
    if ((ifp->buf = calloc(1, ifp->buf_size)) == NULL) {
      perror("calloc error in main()");
      exit(1);
    }
    /* get space for ETHER, IP and UDP header */
    ifp->buf_size -= (QWOFF + ETHERHL + IPHL + UDPHL);
    ifp->buf = &ifp->buf[QWOFF + ETHERHL + IPHL + UDPHL];

    bzero(&my_addr, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    my_addr.sin_port = dhcps_port;
    if (bind(ifp->fd, (struct sockaddr *) &my_addr, sizeof(my_addr)) < 0) {
      perror("bind error in main()");
      exit(1);
    }
    ifp->next = if_list;
    if_list = ifp;
  }

  /****************************
   * Main loop                *
   * Process incoming message *
   ****************************/
  while(1) {

    /* select and read from interfaces */
    if ((ifp = read_interfaces(if_list, &n)) == NULL) {
      continue;
    }
    if (ifp != if_list) {   /* receive from BPF or NIT */
#ifdef sun
      rbufp = &ifp->buf[QWOFF];

      /* convert buffer to struct ether, ip, udp and dhcp */
      rcv.ether = (struct ether_header *)rbufp;
      rcv.ip = (struct ip *)&rbufp[ETHERHL];

      if ((ntohs(rcv.ip->ip_off) & 0x1fff) == 0 &&
	  ntohs(rcv.ip->ip_len) >= DFLTBOOTPLEN + UDPHL + IPHL &&
	  (rcv.ip->ip_dst.s_addr == 0xffffffff ||
	   rcv.ip->ip_dst.s_addr == ifp->ipaddr->s_addr) &&
	  check_ipsum(rcv.ip))
	rcv.udp = (struct udphdr *)&rbufp[ETHERHL+rcv.ip->ip_hl*WORD];
      else
	continue;

      if (ntohs(rcv.udp->uh_ulen) >= DFLTBOOTPLEN + UDPHL &&
	  rcv.udp->uh_dport == dhcps_port &&
	  check_udpsum(rcv.ip, rcv.udp))
	rcv.dhcp = (struct dhcp *)&rbufp[ETHERHL+rcv.ip->ip_hl*WORD+UDPHL];
      else
	continue;

      /* final check the packet */
      if (rcv.dhcp->op == BOOTREQUEST &&
	  rcv.dhcp->hops <= HOPSTHRESHOLD &&
	  bcmp(rcv.dhcp->options, magic_c, MAGIC_LEN) == 0) {
	relay_tosrv(ifp);     /* process the packet */
      }
#else
      rbufp = ifp->buf;
      while (n > 0) {
	/* convert buffer to struct bpf_hdr, ether, ip, udp and dhcp */
	align_msg(rbufp);

	if (check_pkt1(ifp)) {             /* final check the packet */
	  relay_tosrv(ifp);     /* process the packet */
	}

	/* It is not a DHCP or BOOTP message. Skip it */
	rbufp += BPF_WORDALIGN(rbpf->bh_hdrlen + rbpf->bh_caplen);
	n -= (char *) rbufp - (char *) rbpf;
      }
#endif
    } else {
      rcv.dhcp = (struct dhcp *) ifp->buf;
      if (check_pkt2(n)) {
	relay_tocli(if_list, n);
      }
    }
  }
}


/*********************************
 * print out usage. Never return *  
 *********************************/
static void
usage()
{
  fprintf(stderr, "Usage: relay [-d] [-f server-db] if_name [if_name] ....\n");
  exit(1);
}


/*
 * print out version
 */
static void
version()
{
  fprintf(stderr, "WIDE DHCP relay agent version 1.3\n");
  exit(1);
}


#ifndef sun
/*
 * final check the packet
 */
static int
check_pkt1(ifp)
  struct if_info *ifp;
{
  return (ntohs(rcv.ip->ip_len) >= DFLTBOOTPLEN + UDPHL + IPHL &&
	  (rcv.ip->ip_dst.s_addr == 0xffffffff ||
	   rcv.ip->ip_dst.s_addr == ifp->ipaddr->s_addr) &&
	  ntohs(rcv.udp->uh_ulen) >= DFLTBOOTPLEN + UDPHL &&
	  check_ipsum(rcv.ip) &&
	  check_udpsum(rcv.ip, rcv.udp) &&
	  rcv.dhcp->op == BOOTREQUEST &&
	  rcv.dhcp->hops <= HOPSTHRESHOLD &&
	  bcmp(rcv.dhcp->options, magic_c, MAGIC_LEN) == 0);
}
#endif


/*
 * final check the packet
 */
static int
check_pkt2(len)
  int len;
{
  return (len >= DFLTBOOTPLEN &&
	  rcv.dhcp->op == BOOTREPLY && rcv.dhcp->hops <= HOPSTHRESHOLD &&
	  bcmp(rcv.dhcp->options, magic_c, MAGIC_LEN) == 0);
}


static void
read_server_db()
{
  FILE *server_dbfp;
  char buffer[MAXPATHLEN];
  struct server *srvp = NULL, *lastp = NULL;
  char tmp[sizeof("255.255.255.255")];

  if (server_dbname[0] == '\0') strcpy(server_dbname, SERVER_DB);
  if ((server_dbfp = fopen(server_dbname, "r")) == NULL) {
    syslog(LOG_ERR, "Cannot open the server database \"%s\"", server_dbname);
    exit(1);
  }
  if (fgets(buffer, MAXPATHLEN, server_dbfp) == NULL) {
    if (errno != 0)
      syslog(LOG_ERR, "fgets() error in read_server_db()");
    exit(1);
  }
  if (sscanf(buffer, "%d", &nsend) != 1) {
    syslog(LOG_ERR, "sscanf() error in read_server_db()");
    exit(1);
  }

  while(1) {
    if (fgets(buffer, MAXPATHLEN, server_dbfp) == NULL)
      break;
    if ((srvp = (struct server *) calloc(1, sizeof(struct server))) == NULL) {
      syslog(LOG_ERR, "calloc() error in read_server_db()");
      exit(1);
    }
    if (sscanf(buffer, "%s", tmp) != 1) {
      syslog(LOG_WARNING, "sscanf() error in read_server_db()");
      break;
    }
    if ((srvp->ip.s_addr = inet_addr(tmp)) == -1) {
      syslog(LOG_WARNING, "inet_addr() error in read_server_db()");
      break;
    }
    nserver++;
    srvp->next = srvlist;
    if (srvlist == NULL)
      lastp = srvp;
    srvlist = srvp;
  }
  if (lastp == NULL) {
     syslog(LOG_ERR, "There is no DHCP server in the server database \"%s\"", server_dbname);
     exit(1);
  }
  lastp->next = srvlist;      /* make loop */

  if (nserver < nsend)
    nsend = nserver;
  fclose(server_dbfp);

  return;
}
  

/*
 * become daemon
 */
static void
become_daemon()
{
  int n;

  /*
   * go into background and disassociate from controlling terminal
   */
  if(fork() != 0) exit(0);
  for (n = 0; n < 10; n++)
    close(n);
  open("/", O_RDONLY);
  dup2(0, 1);
  dup2(0, 2);
  if ((n = open("/dev/tty", O_RDWR)) >= 0) {
    ioctl(n, TIOCNOTTY, (char *) 0);
    close(n);
  }
}


static void
relay_tosrv(ifp)
  struct if_info *ifp;
{
  unsigned int hash;
  int i;
  struct server *srvp;

  rcv.dhcp->hops++;
  if (rcv.dhcp->giaddr.s_addr == 0)
    rcv.dhcp->giaddr.s_addr = ifp->ipaddr->s_addr;
  hash = (unsigned) cksum((u_short *) rcv.dhcp->chaddr, (MAX_HLEN / 2));

  hash %= nserver;

  srvp = srvlist;
  for (i = 0; i < hash; i++) {
    srvp = srvp->next;
  }

  forwarding(srvp);

  return;
}


static void
forwarding(srvp)
  struct server *srvp;
{
  int i, n;
  static int sockfd = -1;
  int msgsize = DHCPLEN(rcv.udp);
  struct sockaddr_in my_addr, serv_addr;

  if (sockfd == -1) {
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
      syslog(LOG_ERR, "socket() error in forwarding()");
      exit(1);
    }
    bzero(&my_addr, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    my_addr.sin_port = dhcpc_port;  /* to pass the check of src port at any server */
    if (bind(sockfd, (struct sockaddr *) &my_addr, sizeof(my_addr)) < 0) {
      syslog(LOG_ERR, "bind() error in forwarding()");
      exit(1);
    }
  }

  snd.dhcp = rcv.dhcp;
  if (setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &msgsize, sizeof(msgsize)) < 0) {
    syslog(LOG_WARNING, "setsockopt() error in forwarding()");
    return;
  }
  
  n = sizeof(serv_addr);
  for (i = 0; i < nsend; i++) {
    bzero(&serv_addr, n);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = srvp->ip.s_addr;
    serv_addr.sin_port = dhcps_port;
    sendto(sockfd, (char *)snd.dhcp, msgsize, 0, (struct sockaddr *)&serv_addr, n);
    srvp = srvp->next;
  }
}


static void
relay_tocli(list, dhcplen)
  struct if_info *list;
  int dhcplen;
{
  int i, total;
  struct if_info *ifp;
  int msgtype = 0;
  char *option = NULL;

  ifp = list;
  while (ifp != NULL) {
    if (ifp->ipaddr != NULL && ifp->ipaddr->s_addr == rcv.dhcp->giaddr.s_addr)
      break;
    ifp = ifp->next;
  }

  if (ifp == NULL) { /* there is no corresponding interfaces */
    syslog(LOG_WARNING, "DHCP message from server(%s) has a wrong giaddr",
	   inet_ntoa(rcv.ip->ip_src));
    return;
  }

  if ((option = pickup_opt(rcv.dhcp, dhcplen, DHCP_MSGTYPE)) != NULL) {
#define OPTBODY(TAGP)  (((char *)TAGP) + 2)
    msgtype = *OPTBODY(option);
  }


  /* Already, there is space for ETHER, IP, UDP header */
  snd.dhcp = rcv.dhcp;
  snd.udp = (struct udphdr *) (((char *) rcv.dhcp) - UDPHL);
  snd.ip = (struct ip *) (((char *) rcv.dhcp) - UDPHL - IPHL);
  snd.ether = (struct ether_header *) (((char *) rcv.dhcp) - UDPHL - IPHL - ETHERHL);

  if (rcv.dhcp->htype != ETHER || rcv.dhcp->hlen != 6)
    return;

  if (ISBRDCST(rcv.dhcp->flags)) {
    for (i = 0; i < 6; i++) {
#ifdef sun
      snd.ether->ether_dhost.ether_addr_octet[i] = 0xff;
      snd.ether->ether_shost.ether_addr_octet[i] = ifp->haddr[i];
#else
      snd.ether->ether_dhost[i] = 0xff;
      snd.ether->ether_shost[i] = ifp->haddr[i];
#endif
    }
    snd.ip->ip_dst.s_addr = 0xffffffff;
    snd.ip->ip_src.s_addr = ifp->ipaddr->s_addr;
  } else {
    for (i = 0; i < 6; i++) {
#ifdef sun
      snd.ether->ether_dhost.ether_addr_octet[i] = rcv.dhcp->chaddr[i];
      snd.ether->ether_shost.ether_addr_octet[i] = ifp->haddr[i];
#else
      snd.ether->ether_dhost[i] = rcv.dhcp->chaddr[i];
      snd.ether->ether_shost[i] = ifp->haddr[i];
#endif
    }
    if (msgtype == DHCPNAK)
      snd.ip->ip_dst.s_addr = 0xffffffff;
    else
      snd.ip->ip_dst.s_addr = rcv.dhcp->yiaddr.s_addr;
    snd.ip->ip_src.s_addr = ifp->ipaddr->s_addr;
  }
  snd.ether->ether_type = htons(ETHERTYPE_IP);
  snd.udp->uh_sport = dhcps_port;
  snd.udp->uh_dport = dhcpc_port;
  snd.udp->uh_ulen = htons(dhcplen + UDPHL);
  snd.udp->uh_sum = get_udpsum(snd.ip, snd.udp);
  snd.ip->ip_v = IPVERSION;
  snd.ip->ip_hl = IPHL >> 2;
  snd.ip->ip_tos = 0;
  snd.ip->ip_len = htons(dhcplen + UDPHL + IPHL);
  snd.ip->ip_id = snd.udp->uh_sum;
  snd.ip->ip_ttl = 0x20;                       /* XXX */
  snd.ip->ip_p = IPPROTO_UDP;

  total = dhcplen + UDPHL + IPHL + ETHERHL;
  if (total <= ETHERMTU) {
    snd.ip->ip_off = 0;
    snd.ip->ip_sum = get_ipsum(snd.ip);
    ether_write(ifp->fd, (char *)snd.ether, total);
  } else {
#define  MTU  (ETHERMTU - IPHL)
    char *n, *end, *begin;
    struct iovec sbufvec[2];

    sbufvec[0].iov_base = (char *) snd.ether;
    sbufvec[0].iov_len = ETHERHL + IPHL;
    begin = (char *) snd.udp;
    end = &begin[total];

    for (n = begin; n < end; n += MTU) {
      sbufvec[1].iov_base = n;
      if ((end - n) >= MTU) {
	sbufvec[1].iov_len = MTU;
	snd.ip->ip_off = htons(IP_MF | ((((n - begin) / 8)) & 0x1fff));
	snd.ip->ip_sum = get_ipsum(snd.ip);
      } else {
	sbufvec[1].iov_len = end - n;
	snd.ip->ip_off = htons(((n - begin) / 8) & 0x1fff);
	snd.ip->ip_sum = get_ipsum(snd.ip);
      }
      ether_writev(ifp->fd, sbufvec, 2);
    }
  }

  return;
};
