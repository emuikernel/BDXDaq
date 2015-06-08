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
#ifdef solaris
#include <sys/sockio.h>
#include <sys/termio.h>
#include <sys/filio.h>
#endif
#include <sys/param.h>
#include <sys/socket.h>

#include <net/if.h>
#ifndef sun
#include <net/bpf.h>
#endif
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/if_ether.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>

#ifdef solaris              /* Bit map macros */
#include <sys/fs/ufs_fs.h>
#endif

#include "dhcp.h"
#include "common.h"
#include "common_subr.h"
#include "hash.h"
#include "dhcps.h"

#define DHCPLEN(UDP)  (ntohs(UDP->uh_ulen) - UDPHL)  /* get DHCP message size from
							struct udp */
#define DHCPOPTLEN(TAGP) (*(((char *)TAGP) + 1)) /* get length of DHCP option */
#define OPTBODY(TAGP)  (((char *)TAGP) + 2)    /* get content of DHCP option */

/* handle the word alignment */
#define  GETHS(PTR)   (*((u_char *)PTR)*256 + *(((u_char *)PTR)+1))
#define  GETHL(PTR)   (*((u_char *)PTR)*256*256*256 + *(((u_char *)PTR)+1)*256*256 +\
		       *(((u_char *)PTR)+2)*256 + *(((u_char *)PTR)+3))

#define MEMORIZE      90  /* server avoid to reuse the address between 90 secs */
#define E_NOMORE      (-2)    /* Error code "There is no more space" */

#define WITH_CID      1
#define WITH_REQIP    2
#define FREE          3

/*
 *  global variable
 */
static int overload;      /* flag for option overload */
static int off_options;   /* offset of DHCP options field */
static int off_extopt;    /* offset of extended options */
static int off_file;      /* offset of DHCP file field */
static int off_sname;     /* offset of DHCP sname field */
static int maxoptlen;     /* maximum offset of DHCP options */
static int timeoutflag;   /* used in icmp_check */
static int debug;
static int rdhcplen;

static char *rbufp = NULL;                 /* receive buffer */
static char *sbufp = NULL;
static struct iovec sbufvec[2];            /* send buffer */
static struct msg snd;
static struct if_info *if_list = NULL;      /* interfaces list */

unsigned char magic_c[] = RFC1048_MAGIC;  /* magic cookie which defined in
					     RFC1048 */
FILE *addrpool_dbfp;
FILE *binding_dbfp;
FILE *relay_dbfp;
FILE *dump_fp;
char binding_db[MAXPATHLEN];
char addrpool_db[MAXPATHLEN];
char relay_db[MAXPATHLEN];
int nbind;
struct msg rcv;
struct bpf_hdr *rbpf;

void
main(argc, argv)
  int argc;
  char **argv;
{
  int n = 0;
  int loop;
  FILE *pid_file;
  struct if_info *ifp = NULL;          /* pointer to interface */
  char *option = NULL;                 /* command line option */
  char msgtype;                        /* DHCP message type */
  struct sockaddr_in my_addr, any_addr;
  struct ifreq ifreq;

  bzero(&any_addr, sizeof(any_addr));
  bzero(&ifreq, sizeof(ifreq));

  debug = 0;                           /* debug flag */
  srandom(getpid());


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
      case 'b':               /* option which specify the binding db file */
	--argc, ++argv;
	if (argc <= 0) usage();
	else strcpy(binding_db, argv[0]);
	break;
      case 'a':               /* option which specify the resource db file */
	--argc,	++argv;
	if (argc <= 0) usage();
	else strcpy(addrpool_db, argv[0]);
	break;
      case 'r':              /* option which specify the relay agent db file */
	--argc,	++argv;
	if (argc <= 0) usage();
	else strcpy(relay_db, argv[0]);
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
#define LOG_PERROR      0
#endif
  openlog("dhcps", LOG_PID | LOG_CONS | LOG_PERROR, LOG_LOCAL0);

  if (if_list == NULL) usage();

  if (debug == 0) become_daemon();
  init_db();               /* initialize databases */
  read_addrpool_db();      /* read address pool database */
  read_bind_db();          /* read binding database */
  read_relay_db();         /* read relay agents database */
  set_sighand();           /* Set up signal handlers */
  set_srvport();           /* determine dhcp port */

  /* open the bpf for each interface */
  ifp = if_list;
  while (ifp != NULL) {
    if (open_if(ifp) < 0) {
      exit(1);
    }
    ifp = ifp->next;
  }

  /* open normal socket */
  if ((ifp = (struct if_info *) calloc(1, sizeof(struct if_info))) == NULL) {
    syslog(LOG_ERR, "calloc error in main()");
    exit(1);
  }
  strcpy(ifp->name, argv[0]);

  if ((ifp->fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    syslog(LOG_INFO, "Socket open failed.");
    free(ifp);
    exit(1);
  } else {
    strcpy(ifreq.ifr_name, ifp->name);
    if (ioctl(ifp->fd, SIOCGIFNETMASK, &ifreq) < 0) {
      syslog(LOG_ERR, "ioctl(SIOCGIFNETMASK) error in main()");
      exit(1);
    }
    if ((ifp->subnetmask =
         (struct in_addr *) calloc(1, sizeof(struct in_addr))) == NULL) {
      syslog(LOG_ERR, "calloc error in main()");
      exit(1);
    }
    ifp->subnetmask->s_addr =
      ((struct sockaddr_in *)&ifreq.ifr_addr)->sin_addr.s_addr;
    if (ioctl(ifp->fd, SIOCGIFADDR, &ifreq) < 0) {
      syslog(LOG_ERR, "ioctl(SIOCGIFADDR) error in main()");
      exit(1);
    }
    if ((ifp->ipaddr =
         (struct in_addr *) calloc(1, sizeof(struct in_addr))) == NULL) {
      syslog(LOG_ERR, "calloc error in main()");
      exit(1);
    }
    ifp->ipaddr->s_addr =
      ((struct sockaddr_in *)&ifreq.ifr_addr)->sin_addr.s_addr;

    ifp->buf_size = DFLTDHCPLEN;
    if ((ifp->buf = calloc(1, ifp->buf_size)) == NULL) {
      syslog(LOG_ERR, "calloc error in main()");
      exit(1);
    } 
    /* get space for ETHER, IP and UDP header */
    bzero(&my_addr, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    my_addr.sin_port = dhcps_port;
    if (bind(ifp->fd, (struct sockaddr *) &my_addr, sizeof(my_addr)) < 0) {
      syslog(LOG_ERR, "bind error in main()");
      exit(1);
    }
    ifp->next = if_list;
    if_list = ifp;
   }

  /* buffer allocation for sending */
  alloc_sbuf();

  /* initialization works well, so recored its own pid */
  if ((pid_file = fopen(PATH_PID, "w")) != NULL) {
    fprintf(pid_file, "%d\n", (int) getpid());
    fclose(pid_file);
  }

  /****************************
   * Main loop                *
   * Process incoming message *
   ****************************/
  while(1) {
    garbage_collect();

    /* select and read from interfaces */
    if ((ifp = read_interfaces(if_list, &n)) == NULL) {
      continue;
    }

#ifdef sun
    if (ifp != if_list) {  /* receive from NIT */
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
	  rcv.dhcp->giaddr.s_addr == 0 &&
	  bcmp(rcv.dhcp->options, magic_c, MAGIC_LEN) == 0) {
	rdhcplen = DHCPLEN(rcv.udp);

	/* process the packet */
	msgtype = 0;
	if ((option = pickup_opt(rcv.dhcp, rdhcplen, DHCP_MSGTYPE)) != NULL) {
	  msgtype = *OPTBODY(option);
	}

        if (msgtype < BOOTP || msgtype > DHCPRELEASE)
	  syslog(LOG_INFO, "unknown message");
	else if (process_msg[(int) msgtype] != NULL)
	  (*process_msg[(int) msgtype])(ifp);
      }
    } else {
      rcv.dhcp = (struct dhcp *) ifp->buf;
      rdhcplen = n;

      if (check_pkt2(rdhcplen)) {
	/* process the packet */
	msgtype = 0;
	if ((option = pickup_opt(rcv.dhcp, rdhcplen, DHCP_MSGTYPE)) != NULL) {
	  msgtype = *OPTBODY(option);
	}

	if (msgtype < BOOTP || msgtype > DHCPRELEASE)
	  syslog(LOG_INFO, "unknown message");
	else if (process_msg[(int) msgtype] != NULL)
	  (*process_msg[(int) msgtype])(ifp);
      }
    }
  }
#else /* not sun */
    if (ifp != if_list) {  /* receive from BPF */
      rbufp = ifp->buf;

      while (n > 0) {
	/* convert buffer to struct bpf_hdr, ether, ip, udp and dhcp */
	align_msg(rbufp);
	if (check_pkt1(ifp)) {      /* final check the packet */
	  rdhcplen = DHCPLEN(rcv.udp);

	  /* process the packet */
	  msgtype = 0;
	  if ((option = pickup_opt(rcv.dhcp, rdhcplen,
				   DHCP_MSGTYPE)) != NULL) {
	    msgtype = *OPTBODY(option);
	  }

	  if (msgtype < BOOTP || msgtype > DHCPRELEASE)
	    syslog(LOG_INFO, "unknown message");
	  else if (process_msg[(int) msgtype] != NULL)
	    (*process_msg[(int) msgtype])(ifp);
	}

	/* It is not a DHCP or BOOTP message. Skip it */
	rbufp += BPF_WORDALIGN(rbpf->bh_hdrlen + rbpf->bh_caplen);
	n -= (char *) rbufp - (char *) rbpf;
      }
    } else {
      rcv.dhcp = (struct dhcp *) ifp->buf;
      rdhcplen = n;

      if (check_pkt2(rdhcplen)) {
	/* process the packet */
	msgtype = 0;
	if ((option = pickup_opt(rcv.dhcp, rdhcplen, DHCP_MSGTYPE)) != NULL) {
	  msgtype = *OPTBODY(option);
	}

	if (msgtype < BOOTP || msgtype > DHCPRELEASE)
	  syslog(LOG_INFO, "unknown message");
	else if (process_msg[(int) msgtype] != NULL)
	  (*process_msg[(int) msgtype])(ifp);
      }
    }
  }
#endif /* not sun */
}


/*********************************
 * print out usage. Never return *  
 *********************************/
static void
usage()
{
  fprintf(stderr, "Usage: dhcps [-d] [-b binding_db] [-a addrpool_db] ");
  fprintf(stderr, "[-r relay_db] if_name [ifname] ... \n");
  exit(1);
}


/*
 * print out version  
 */
static void
version()
{
  fprintf(stderr, "WIDE DHCP server version 1.3\n");
  exit(1);
}


/*
 * final check the packet
 */
static int
check_pkt2(len)
  int len;
{
  if (len < DFLTBOOTPLEN || rcv.dhcp->op != BOOTREQUEST)
    return(FALSE);
  if (rcv.dhcp->giaddr.s_addr == 0)    /* it should be read from bpf or nit */
    return(FALSE);
  if (bcmp(rcv.dhcp->options, magic_c, MAGIC_LEN) != 0)
    return(FALSE);

  return(TRUE);
}


/*
 * final check the packet
 */
#ifndef sun
static int
check_pkt1(ifp)
  struct if_info *ifp;
{
  if (ntohs(rcv.ip->ip_len) < DFLTBOOTPLEN + UDPHL + IPHL)
    return(FALSE);
  if (rcv.ip->ip_dst.s_addr != 0xffffffff &&
      rcv.ip->ip_dst.s_addr != 0 &&   /* old BOOTP client */
      rcv.ip->ip_dst.s_addr != ifp->ipaddr->s_addr)
    return(FALSE);
  if (ntohs(rcv.udp->uh_ulen) < DFLTBOOTPLEN + UDPHL)
    return(FALSE);
  if (check_ipsum(rcv.ip) == 0)
    return(FALSE);
  if (check_udpsum(rcv.ip, rcv.udp) == 0)
    return(FALSE);
  if (rcv.dhcp->op != BOOTREQUEST ||
      rcv.dhcp->giaddr.s_addr != 0)  /* it should be read from normal socket */
    return(FALSE);
  if (bcmp(rcv.dhcp->options, magic_c, MAGIC_LEN) != 0 &&
      GETHL(rcv.dhcp->options) != 0)  /* old BOOTP client */
    return(FALSE);

  return(TRUE);
}
#endif /* not sun */


/*
 * initialize databases
 */
static void
init_db()
{
  if (addrpool_db[0] == '\0') strcpy(addrpool_db, ADDRPOOL_DB);
  if ((addrpool_dbfp = fopen(addrpool_db, "r")) == NULL) {
    syslog(LOG_ERR, "Cannot open the resource database \"%s\"", addrpool_db);
    exit(1);
  }
  if (relay_db[0] == '\0') strcpy(relay_db, RELAY_DB);
  if ((relay_dbfp = fopen(relay_db, "r")) == NULL) {
    syslog(LOG_ERR, "Cannot open the relay agent database \"%s\"", relay_db);
    exit(1);
  }
  if (binding_db[0] == '\0') strcpy(binding_db, BINDING_DB);
  if ((binding_dbfp = fopen(binding_db, "r+")) == NULL &&
      (binding_dbfp = fopen(binding_db, "w+")) == NULL) {
    syslog(LOG_ERR, "Cannot open the binding database \"%s\"", binding_db);
    exit(1);
  }
  if ((dump_fp = fopen(ADDRPOOL_DUMP, "w+")) == NULL) {
    syslog(LOG_ERR, "Cannot open the resource dump file \"%s\"",ADDRPOOL_DUMP);
    exit(1);
  }
  return;
}


/*
 * allocate buffer for sending
 */
static void
alloc_sbuf()
{
  sbufvec[0].iov_len = ETHERHL + IPHL + UDPHL + DFLTDHCPLEN;

  if ((sbufp = calloc(1, sbufvec[0].iov_len + QWOFF)) == NULL) {
    syslog(LOG_ERR, "calloc error send buffer in alloc_buf()");
    exit(1);
  }
  sbufvec[0].iov_base = &sbufp[QWOFF];
  snd.ether = (struct ether_header *) &sbufp[QWOFF];
  snd.ip = (struct ip *) &sbufp[QWOFF + ETHERHL];
  snd.udp = (struct udphdr *) &sbufp[QWOFF + ETHERHL + IPHL];
  snd.dhcp = (struct dhcp *) &sbufp[QWOFF + ETHERHL + IPHL + UDPHL];

  return;
}


/*
 * return strings which include the haddr in the form of "type:haddr...."
 */
char *
haddrtos(haddr)
  struct chaddr *haddr;
{
  int i, fin;
  char tmp[3];
  static char result[MAX_HLEN*2+8];     /* it seems enough */

  bzero(result, sizeof(result));
  if ((fin = haddr->hlen) > MAX_HLEN)
    fin = MAX_HLEN;
  sprintf(result, "%d:0x", haddr->htype);
  for (i = 0; i < fin; i++) {
    sprintf(tmp, "%.2x", haddr->haddr[i] & 0xff);
    strcat(result, tmp);
  }

  return(result);
}


/*
 * return strings which include the cid in the form of "type:id...."
 */
char *
cidtos(cid, withsubnet)
  struct client_id *cid;
  int withsubnet;
{
  int i = 0;
  static char result[MAXOPT*2+sizeof("255.255.255.255")+3];   /* it seems enough */
  char tmp[sizeof("255.255.255.255")+1];

  sprintf(result, "%d:0x", cid->idtype);
  for (i = 0; i < cid->idlen; i++) {
    sprintf(tmp, "%.2x", cid->id[i] & 0xff);
    strcat(result, tmp);
  }
  if (withsubnet) {
    sprintf(tmp, ":%s", inet_ntoa(cid->subnet));
    strcat(result, tmp);
  }

  return(result);
}

  
/*
 * set signal handlers
 */
static void
set_sighand()
{
  if ((int) signal(SIGINT, finish) < 0) {
    syslog(LOG_ERR, "cannot set signal handler(SIGINT)");
    exit(1);
  }
  if ((int) signal(SIGTERM, finish) < 0) {
    syslog(LOG_ERR, "cannot set signal handler(SIGTERM)");
    exit(1);
  }
  if ((int) signal(SIGUSR1, dump_bind_db) < 0) {
    syslog(LOG_ERR, "cannot set signal handler(SIGUSR1)");
    exit(1);
  }
  if ((int) signal(SIGUSR2, dump_addrpool_db) < 0) {
    syslog(LOG_ERR, "cannot set signal handler(SIGUSR2)");
    exit(1);
  }
  return;
}


/*
 * become daemon
 */
static void
become_daemon()
{
  int n = 0;

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


/*
 * return requested lease in host byte order
 */
static u_Long
get_reqlease(msg, length)
  struct dhcp *msg;
  int length;
{
  char *option = NULL;
  u_Long retval = 0;

  if ((option = pickup_opt(msg, length, LEASE_TIME)) != NULL) {
    retval = GETHL(OPTBODY(option));
  }

  return(retval);
}


/*
 * return client identifier (if there is no id, return haddr)
 */
static void
get_cid(msg, length, cid)
  struct dhcp *msg;
  int length;
  struct client_id *cid;
{
  char *option = NULL;

  if ((option = pickup_opt(msg, length, CLIENT_ID)) != NULL) {
    cid->idlen = ((int) DHCPOPTLEN(option)) - 1;  /* -1 == identifier type */
    cid->id = (OPTBODY(option) + 1);
    cid->idtype = *OPTBODY(option);
  } else {      /* haddr is used to substitute for client identifier */
    cid->idlen = msg->hlen;
    cid->idtype = msg->htype;
    cid->id = msg->chaddr;
  }

  return;
}


/*
 * get maximum length of option field
 */
static int
get_maxoptlen(msg, length)
  struct dhcp *msg;
  int length;
{
  char *option = NULL;
  int retval = DFLTOPTLEN;

  if ((option = pickup_opt(msg, length, DHCP_MAXMSGSIZE)) != NULL) {
    retval = GETHS(OPTBODY(option)) - IPHL - UDPHL - DFLTDHCPLEN + DFLTOPTLEN;
  }

  /* XXX assume default mtu */
  if (retval - DFLTOPTLEN + DFLTDHCPLEN + UDPHL + IPHL > ETHERMTU) {
    retval = ETHERMTU - IPHL - UDPHL - DFLTDHCPLEN + DFLTOPTLEN;
  }

  return(retval);
}


/*
 * return subnet mask
 */
static int
get_subnet(msg, length, subn, ifp)
  struct dhcp *msg;
  int length;
  struct in_addr *subn;
  struct if_info *ifp;
{
  char *option = NULL;
  struct relay_acl *acl = NULL;
  struct dhcp_resource *res = NULL;

  if (msg->ciaddr.s_addr != 0) {
    if ((option = pickup_opt(msg, length, SUBNET_MASK)) != NULL) {
      subn->s_addr = msg->ciaddr.s_addr & htonl(GETHL(OPTBODY(option)));
      return(0);
    }
    else if ((res = (struct dhcp_resource *)
	      hash_find(&iphashtable, (char *) &msg->ciaddr.s_addr,
			sizeof(u_Long), resipcmp, &msg->ciaddr)) == NULL) {
      errno = 0;
      syslog(LOG_INFO, "Can't find corresponding resource in get_subnet()");
    } else {
      subn->s_addr = msg->ciaddr.s_addr & res->subnet_mask->s_addr;
      return(0);
    }
  }

  if (msg->giaddr.s_addr != 0) {
    if ((option = pickup_opt(msg, length, SUBNET_MASK)) != NULL) {
      subn->s_addr = msg->giaddr.s_addr & htonl(GETHL(OPTBODY(option)));
      return(0);
    }
    else if ((acl = (struct relay_acl *)
	      hash_find(&relayhashtable, (char *) &msg->giaddr,
			sizeof(struct in_addr), relayipcmp,
			&msg->giaddr)) == NULL) {
      errno = 0;
      syslog(LOG_INFO, "relayed packet come from invalid relay agent(%s).",
	     inet_ntoa(msg->giaddr));
      return(-1);
    } else {
      subn->s_addr = (acl->relay.s_addr & acl->subnet_mask.s_addr);
      return(0);
    }
  }

  subn->s_addr = ifp->ipaddr->s_addr & ifp->subnetmask->s_addr;
  return(0);
}


/*
 * return subnet mask
 */
static int
get_snmk(msg, length, subn, ifp)
  struct dhcp *msg;
  int length;
  struct in_addr *subn;
  struct if_info *ifp;
{
  struct relay_acl *acl = NULL;

  if (msg->giaddr.s_addr != 0) {
    if ((acl = (struct relay_acl *)
	 hash_find(&relayhashtable, (char *) &msg->giaddr,
		   sizeof(struct in_addr), relayipcmp,
		   &msg->giaddr)) == NULL) {
      errno = 0;
      syslog(LOG_INFO, "relayed packet come from invalid relay agent(%s).",
	     inet_ntoa(msg->giaddr));
      return(-1);
    } else {
      subn->s_addr = acl->subnet_mask.s_addr;
      return(0);
    }
  }

  subn->s_addr = ifp->subnetmask->s_addr;
  return(0);
}


/*
 * return true if resource is available
 */
static int
available_res(res, cid, curr_epoch)
  struct dhcp_resource *res;
  struct client_id *cid;
  time_t curr_epoch;
{
  return(res->binding == NULL ||
	 (res->binding->expire_epoch != 0xffffffff &&
	  res->binding->expire_epoch < curr_epoch &&
	  res->binding->temp_epoch < curr_epoch) ||
	 cidcmp(&res->binding->cid, cid));
}


static int
cidcopy(src, dst)
  struct client_id *src;
  struct client_id *dst;
{
  dst->subnet.s_addr = src->subnet.s_addr;
  dst->idtype = src->idtype;
  dst->idlen = src->idlen;
  if (dst->id != NULL) free(dst->id);
  if ((dst->id = (char *) calloc(1, src->idlen)) == NULL) {
    syslog(LOG_WARNING, "calloc error in cidcopy()");
    return(-1);
  }
  bcopy(src->id, dst->id, src->idlen);

  return(0);
}


/*
 * choose lease duration
 */
static int
choose_lease(reqlease, curr_epoch, offer_res)
  int reqlease;
  time_t curr_epoch;
  struct dhcp_resource *offer_res;
{
  u_Long offer_lease = 0;

  if (isset(offer_res->valid, S_CLIENT_ID)) {
    offer_lease = 0xffffffff;
  }
  else if (reqlease != 0) {
    if (reqlease <= offer_res->max_lease)
      offer_lease = reqlease;
    else
      offer_lease = offer_res->max_lease;
  }
  else if (offer_res->binding == NULL) {
    offer_lease = offer_res->default_lease;
  } else {
    if (offer_res->binding->expire_epoch == 0xffffffff) {
      offer_lease = 0xffffffff;
    }
    else if (offer_res->binding->expire_epoch > curr_epoch) {
      offer_lease = offer_res->binding->expire_epoch - curr_epoch;
      if (offer_lease < MIN_LEASE) {
	offer_lease = MIN_LEASE;
      }
    } else {
      offer_lease = offer_res->default_lease;
    }
  }

  return(offer_lease);
}


static void clean_sbuf()
{
  if (sbufvec[0].iov_base != NULL)
    bzero(sbufvec[0].iov_base, sbufvec[0].iov_len);
  if (sbufvec[1].iov_base != NULL)
    bzero(sbufvec[1].iov_base, sbufvec[1].iov_len);
  return;
}


/*
 * choose resource with client identifier
 */
static struct dhcp_resource *
select_wcid(msgtype, cid, curr_epoch)
  int msgtype;
  struct client_id *cid;
  time_t curr_epoch;
{
  struct dhcp_binding *binding = NULL;

  if ((binding = (struct dhcp_binding *)
       hash_find(&cidhashtable, cid->id, cid->idlen, bindcidcmp, cid)) != NULL) {
    /*
     * Is the resource used ?
     */
    if (available_res(binding->res, cid, curr_epoch)) {
      if (icmp_check(msgtype, binding->res->ip_addr) == GOOD) {
	return(binding->res);
      } else {
	turnoff_bind(binding);
	return(NULL);
      }
    }
  }

  return(NULL);
}


/*
 * choose resource with requested IP
 */
static struct dhcp_resource *
select_wreqip(msgtype, cid, curr_epoch)
  int msgtype;
  struct client_id *cid;
  time_t curr_epoch;
{
  char *option = NULL;
  char tmp[sizeof("255.255.255.255")];
  struct dhcp_resource *res = NULL;
  struct in_addr reqip;

  bzero(tmp, sizeof(tmp));
  bzero(&reqip, sizeof(reqip));

  if ((option = pickup_opt(rcv.dhcp, rdhcplen, REQUEST_IPADDR)) != NULL) {
    reqip.s_addr = htonl(GETHL(OPTBODY(option)));
    if ((res = (struct dhcp_resource *)
	 hash_find(&iphashtable, (char *) &reqip.s_addr,
			 sizeof(u_Long), resipcmp, &reqip)) == NULL) {
      errno = 0;
      syslog(LOG_INFO, "No such IP addr %s in address pool", inet_ntoa(reqip));
      return(NULL);
    } else {      /* find out */
      /* check the subnet number */
      if (cid->subnet.s_addr != (res->ip_addr->s_addr & res->subnet_mask->s_addr)) {
	strcpy(tmp, inet_ntoa(reqip));
	errno = 0;
	syslog(LOG_INFO,
	       "DHCP%s(from cid:\"%s\"): invalid requested IP addr %s (different subnet)",
	       (msgtype == DHCPDISCOVER) ? "DISCOVER" : "REQUEST",
	       cidtos(cid, 1), tmp);
	return(NULL);
      }

      /* is it manual allocation ? */
      if (isset(res->valid, S_CLIENT_ID)) {

	/* is there corresponding binding ? */
	if (res->binding == NULL) {
	  strcpy(tmp, inet_ntoa(reqip));
	  errno = 0;
	  syslog(LOG_INFO,
		 "DHCP%s(from cid:\"%s\"): No corresponding binding for %s",
		 (msgtype == DHCPDISCOVER) ? "DISCOVER" : "REQUEST",
		 cidtos(cid, 1), tmp);
	  return(NULL);
	}

	/* for the client ? */
	if (cidcmp(&res->binding->cid, cid) != TRUE) {
	  strcpy(tmp, inet_ntoa(reqip));
	  errno = 0;
	  syslog(LOG_INFO,
		 "DHCP%s(from cid:\"%s\"): requested IP addr %s is not for the client",
		 (msgtype == DHCPDISCOVER) ? "DISCOVER" : "REQUEST",
		 cidtos(cid, 1), tmp);
	  return(NULL);
	}

	/* is there no host which has the same IP ? */
	if (icmp_check(msgtype, res->ip_addr) == GOOD) {
	  return(res);
	} else {
	  turnoff_bind(res->binding);
	  return(NULL);
	}
      }

      /* is the requested IP available ? */
      else if (available_res(res, cid, curr_epoch)) {
	if (icmp_check(msgtype, res->ip_addr) == GOOD) {
	  return(res);
	} else {
	  turnoff_bind(res->binding);
	  return(NULL);
	}
      }
    }
  }
  return(NULL);
}


/*
 * choose a new address
 *         firstly, choose (never used && reqlease > max_lease),
 *         if not found, choose (LRU && reqlease > max_lease),
 *         if not found, choose ((never used || LRU) && maximum(max_lease)),
 *         if not found, choose the first available one.
 */
static struct dhcp_resource *
select_newone(msgtype, cid, curr_epoch, reqlease)
  int msgtype;
  struct client_id *cid;
  time_t curr_epoch;
  u_Long reqlease;
{
  struct dhcp_resource *res = NULL,
                       *best = NULL;
  struct hash_member *resptr = NULL;

  resptr = reslist;
  while (resptr != NULL) {
    res = (struct dhcp_resource *) resptr->data;

    /* if it is dummy entry, skip it */
    if (res->ip_addr == NULL) {
      resptr = resptr->next;
      continue;
    }

    /* check the resource. valid subnet?, available resource? */
    if (cid->subnet.s_addr ==
	(res->ip_addr->s_addr & res->subnet_mask->s_addr) &&
	available_res(res, cid, curr_epoch)) {

      /*
       * choose the best one
       */
      if (best == NULL) {
	if (icmp_check(msgtype, res->ip_addr) == GOOD)
	  best = res;
	else
	  turnoff_bind(res->binding);
	resptr = resptr->next;
	continue;
      }

      if (best->binding == NULL && res->binding != NULL) {
	resptr = resptr->next;
	continue;
      }
      else if (best->binding != NULL && res->binding == NULL) {
	if (icmp_check(msgtype, res->ip_addr) == GOOD)
	  best = res;
	else
	  turnoff_bind(res->binding);
	resptr = resptr->next;
	continue;
      }

      if (best->allow_bootp == FALSE && res->allow_bootp == TRUE) {
	resptr = resptr->next;
	continue;
      }
      else if (best->allow_bootp == TRUE && res->allow_bootp == FALSE) {
	if (icmp_check(msgtype, res->ip_addr) == GOOD)
	  best = res;
	else
	  turnoff_bind(res->binding);
	resptr = resptr->next;
	continue;
      }

      if ((best->binding == NULL && res->binding == NULL) ||
	  (best->binding != NULL && res->binding != NULL &&
	   best->binding->expire_epoch == res->binding->expire_epoch)) {
	if (reqlease >= res->max_lease && res->max_lease > best->max_lease) {
	  if (icmp_check(msgtype, res->ip_addr) == GOOD)
	    best = res;
	  else
	    turnoff_bind(res->binding);
	  resptr = resptr->next;
	  continue;
	}

	if (reqlease != INFINITY && reqlease <= res->max_lease &&
	    res->max_lease < best->max_lease) {
	  if (icmp_check(msgtype, res->ip_addr) == GOOD)
	    best = res;
	  else
	    turnoff_bind(res->binding);
	  resptr = resptr->next;
	  continue;
	}

	if (reqlease != INFINITY && res->max_lease >= reqlease &&
	    reqlease > best->max_lease) {
	  if (icmp_check(msgtype, res->ip_addr) == GOOD)
	    best = res;
	  else
	    turnoff_bind(res->binding);
	  resptr = resptr->next;
	  continue;
	}

	resptr = resptr->next;
	continue;
      }

      if (best->binding != NULL && res->binding != NULL &&
	  best->binding->expire_epoch > res->binding->expire_epoch) {
	if (icmp_check(msgtype, res->ip_addr) == GOOD)
	  best = res;
	else
	  turnoff_bind(res->binding);
	resptr = resptr->next;
	continue;
      } else {
	resptr = resptr->next;
	continue;
      }
    }

    resptr = resptr->next;
  }

  return(best);
}


/*
 * choose the resource
 */
static struct dhcp_resource *
choose_res(cid, curr_epoch, reqlease)
  struct client_id *cid;
  time_t curr_epoch;
  u_Long reqlease;
{
  struct dhcp_resource *res = NULL;

  /* 1. select with client identifier */
  if ((res = select_wcid(DHCPDISCOVER, cid, curr_epoch)) != NULL)
    return(res);

  /* 2. select with requested IP */
  if ((res = select_wreqip(DHCPDISCOVER, cid, curr_epoch)) != NULL)
    return(res);

  /* 3. select the address newly */
  if ((res = select_newone(DHCPDISCOVER, cid, curr_epoch, reqlease)) != NULL)
    return(res);

  errno = 0;
  syslog(LOG_WARNING, "DHCPDISCOVER: No more available address in the pool");
  return(NULL);
}


/*
 * update binding database, and hashtable
 */
static int
update_db(msgtype, cid, res, lease, curr_epoch)
  int msgtype;
  struct client_id *cid;
  struct dhcp_resource *res;
  u_Long lease;
  time_t curr_epoch;
{
  struct dhcp_binding *binding = NULL;

  if (res->binding != NULL && (res->binding->flag & STATIC_ENTRY) != 0)
    return(0);

  if (res->binding != NULL) {
    hash_del(&cidhashtable, res->binding->cid.id, res->binding->cid.idlen,
	     bindcidcmp, &res->binding->cid, free_bind);
  }
  if ((binding = (struct dhcp_binding *)
       calloc(1, sizeof(struct dhcp_binding))) == NULL) {
    syslog(LOG_WARNING, "calloc error in update_db()");
    return(-1);
  }
  if (cidcopy(cid, &binding->cid) != 0)
    return(-1);
  if (msgtype == DHCPDISCOVER) {
    binding->temp_epoch = curr_epoch + MEMORIZE;
  } else {
    if (lease == 0xffffffff) {
      binding->expire_epoch = 0xffffffff;
    } else {
      binding->expire_epoch = curr_epoch + lease;
    }
  }
  binding->res = res;
  if (binding->res_name != NULL) {
    free(binding->res_name);
  }
  if ((binding->res_name = (char *) calloc(1, strlen(res->entryname) + 1)) == NULL) {
    syslog(LOG_WARNING, "calloc error in update_db()");
    return(-1);
  }
  strcpy(binding->res_name, res->entryname);
  res->binding = binding;

  /* record chaddr */
  binding->haddr.htype = rcv.dhcp->htype;
  binding->haddr.hlen = rcv.dhcp->hlen;
  if (binding->haddr.haddr != NULL) {
    free(binding->haddr.haddr);
  }
  if ((binding->haddr.haddr = calloc(1, binding->haddr.hlen)) == NULL) {
    syslog(LOG_WARNING, "calloc error in update_db()");
    return(-1);
  }
  bcopy(rcv.dhcp->chaddr, binding->haddr.haddr, rcv.dhcp->hlen);

  if ((hash_ins(&cidhashtable, binding->cid.id, binding->cid.idlen, bindcidcmp,
		&binding->cid, binding) < 0)) {
    syslog(LOG_WARNING, "hash_ins() with client identifier failed in update_db()");
    return(-1);
  }
  if (add_bind(binding) != 0)
    return(-1);

  return(0);
}


static void
turnoff_bind(binding)
  struct dhcp_binding *binding;
{
  time_t curr_epoch = 0;

  if (binding == NULL)
    return;

  if (time(&curr_epoch) == -1) {
    syslog(LOG_WARNING, "time() error in turnoff_bind()");
    return;
  }

  binding->expire_epoch = binding->temp_epoch = curr_epoch + 1800;
  hash_del(&cidhashtable, binding->cid.id, binding->cid.idlen, bindcidcmp,
	   &binding->cid, free_fake);
  bzero(binding->cid.id, binding->cid.idlen);
  if (hash_ins(&cidhashtable, binding->cid.id, binding->cid.idlen, bindcidcmp,
	       &binding->cid, binding) < 0) {
    syslog(LOG_WARNING,
	   "hash_ins() with client identifier failure in turnoff_bind()");
  }
  binding->flag &= ~COMPLETE_ENTRY;

  return;
}


/*
 * construct dhcp message
 */
static void
construct_msg(msgtype, res, lease, ifp)
  u_char msgtype;
  struct dhcp_resource *res;
  u_Long lease;
  struct if_info *ifp;
{
  int i = 0;
  int reqoptlen = 0;
  u_Long tmp = 0;
  char *reqopt = NULL,
       inserted[32],
       *option = NULL;

  bzero(inserted, sizeof(inserted));

  clean_sbuf();
  snd.dhcp->op = BOOTREPLY;
  snd.dhcp->htype = rcv.dhcp->htype;
  snd.dhcp->hlen = rcv.dhcp->hlen;
  snd.dhcp->hops = 0;
  snd.dhcp->xid = rcv.dhcp->xid;
  snd.dhcp->secs = 0;
  snd.dhcp->flags = rcv.dhcp->flags;
  snd.dhcp->giaddr.s_addr = rcv.dhcp->giaddr.s_addr;
  bcopy(rcv.dhcp->chaddr, snd.dhcp->chaddr, rcv.dhcp->hlen);

  if (msgtype == DHCPACK)
    snd.dhcp->ciaddr.s_addr = rcv.dhcp->ciaddr.s_addr;

  if (msgtype != DHCPNAK) {
    snd.dhcp->yiaddr.s_addr = res->ip_addr->s_addr;
    if (isset(res->valid, S_SIADDR)) {
      snd.dhcp->siaddr.s_addr = res->siaddr->s_addr;
    } else {
      snd.dhcp->siaddr.s_addr = 0;
    }
    overload = BOTH_AREOPT;
    if (isset(res->valid, S_SNAME)) {
      strncpy(snd.dhcp->sname, res->sname, MAX_SNAME);
      snd.dhcp->sname[MAX_SNAME - 1] = '\0';  /* NULL terminate certainly */
      overload -= SNAME_ISOPT;
    }
    if (isset(res->valid, S_FILE)) {
      strncpy(snd.dhcp->file, res->file, MAX_FILE);
      snd.dhcp->file[MAX_FILE - 1] = '\0';    /* NULL terminate cerntainly */
      overload -= FILE_ISOPT;
    }
  } else {
    snd.dhcp->ciaddr.s_addr = 0;
  }

  /* insert magic cookie */
  bcopy(magic_c, snd.dhcp->options, MAGIC_LEN);
  off_options = MAGIC_LEN;
  off_extopt = 0;

  /* insert dhcp message type option */
  snd.dhcp->options[off_options++] = DHCP_MSGTYPE;
  snd.dhcp->options[off_options++] = 1;
  snd.dhcp->options[off_options++] = msgtype;

  if (msgtype == DHCPNAK) {
    SETBRDCST(snd.dhcp->flags);
    if ((option = pickup_opt(rcv.dhcp, rdhcplen, CLIENT_ID)) != NULL) {
      snd.dhcp->options[off_options++] = CLIENT_ID;
      snd.dhcp->options[off_options++] = DHCPOPTLEN(option);
      bcopy(option, &snd.dhcp->options[off_options], DHCPOPTLEN(option));
      off_options += DHCPOPTLEN(option);
    }
    return;
  }

  /* insert "server identifier" */
  snd.dhcp->options[off_options++] = SERVER_ID;
  snd.dhcp->options[off_options++] = 4;
  bcopy(&ifp->ipaddr->s_addr, &snd.dhcp->options[off_options], 4);
  off_options += 4;

  /* insert "subnet mask" */
  if (insert_opt(res, lease, SUBNET_MASK, inserted, PASSIVE) == E_NOMORE) {
    errno = 0;
    syslog(LOG_INFO, "No more space left to insert options to DHCP%s",
	   (msgtype == DHCPOFFER) ? "OFFER" : "ACK");
  }

  /* insert "lease duration" */
  tmp = htonl(lease);
  snd.dhcp->options[off_options++] = LEASE_TIME;
  snd.dhcp->options[off_options++] = 4;
  bcopy(&tmp, &snd.dhcp->options[off_options], 4);
  off_options += 4;

  /* insert "option overload" */
  if (overload != 0) {
    snd.dhcp->options[off_options++] = OPT_OVERLOAD;
    snd.dhcp->options[off_options++] = 1;        /* 1 == option body's length */
    snd.dhcp->options[off_options++] = overload;
  }

  /* insert the requested options */
  if ((option = pickup_opt(rcv.dhcp, rdhcplen, REQ_LIST)) != NULL) {
    reqopt = OPTBODY(option);
    reqoptlen = DHCPOPTLEN(option);
    for (i = 0; i < reqoptlen; i++) {
      if (isclr(inserted, *(reqopt + i))) {
	if (insert_opt(res, lease, *(reqopt + i), inserted, PASSIVE) == E_NOMORE) {
	  errno = 0;
	  syslog(LOG_INFO, "No more space left to insert options to DHCP%s",
		 (msgtype == DHCPOFFER) ? "OFFER" : "ACK");
	  break;
	}
      }
    }
  }

  /* insert that is different from "Host requirement RFC" defaults */
  for (i = 0; i < LAST_OPTION; i++) {
    if (isclr(inserted, i)) {
      if (insert_opt(res, lease, i, inserted, ACTIVE) == E_NOMORE) {
	errno = 0;
	syslog(LOG_INFO, "No more space left to insert options to DHCP%s",
	       (msgtype == DHCPOFFER) ? "OFFER" : "ACK");
	break;
      }
    }
  }

  return;
}


/*
 * choose resource with ciaddr
 */
static struct dhcp_resource *
select_wciaddr(cid, curr_epoch, nosuchaddr)
  struct client_id *cid;
  time_t curr_epoch;
  int *nosuchaddr;
{
  struct dhcp_resource *res = NULL;
  char tmp[sizeof("255.255.255.255")];

  *nosuchaddr = FALSE;
  if ((res = (struct dhcp_resource *)
       hash_find(&iphashtable, (char *)&rcv.dhcp->ciaddr.s_addr,
		       sizeof(u_Long), resipcmp, &rcv.dhcp->ciaddr)) == NULL) {
    *nosuchaddr = TRUE;
    return(NULL);
  } else {
    if (cid->subnet.s_addr != (res->ip_addr->s_addr & res->subnet_mask->s_addr)) {
      strcpy(tmp, inet_ntoa(rcv.dhcp->ciaddr));
      errno = 0;
      syslog(LOG_INFO,
	     "DHCPREQUEST(from cid:\"%s\", ciaddr:%s): it's for different subnet",
	     cidtos(cid, 1), tmp);
      return(NULL);
    }
    else if (isset(res->valid, S_CLIENT_ID)) {
      /* manual allocation must have corresponding binding */
      if (res->binding == NULL) {
	strcpy(tmp, inet_ntoa(rcv.dhcp->ciaddr));
	errno = 0;
	syslog(LOG_INFO,
	       "DHCPREQUEST(from cid:\"%s\"): No corresponding binding for %s",
	       cidtos(cid, 1), tmp);
	*nosuchaddr = TRUE;
	return(NULL);
      }
      else if (res->binding->cid.idtype != cid->idtype ||
	       res->binding->cid.idlen != cid->idlen ||
	       bcmp(res->binding->cid.id, cid->id, cid->idlen) != 0) {
	strcpy(tmp, inet_ntoa(rcv.dhcp->ciaddr));
	errno = 0;
	syslog(LOG_INFO,
	       "DHCPREQUEST(from cid:\"%s\", ciaddr:%s): it isn't for the client",
	       cidtos(cid, 1), tmp);
	return(NULL);
      }
    }
    else if (res->binding == NULL ||
	     (res->binding->expire_epoch != 0xffffffff &&
	      res->binding->expire_epoch <= curr_epoch) ||
	     res->binding->cid.idtype != cid->idtype ||
	     res->binding->cid.idlen != cid->idlen ||
	     bcmp(res->binding->cid.id, cid->id, cid->idlen) != 0) {
      strcpy(tmp, inet_ntoa(rcv.dhcp->ciaddr));
      errno = 0;
      syslog(LOG_INFO,
	     "DHCPREQUEST(from cid:\"%s\", ciaddr:%s): it isn't available",
	     cidtos(cid, 1), tmp);
      return(NULL);
    }
  }

  return(res);
}


/********************************
 * process DHCPDISCOVER message *
 * which incoming from ether    *
 ********************************/
static int
discover(ifp)
  struct if_info *ifp;
{
  struct dhcp_resource *offer_res = NULL;
  struct client_id cid;
  u_Long offer_lease = 0,                 /* offering lease */
         reqlease = 0;                    /* requested lease duration */
  time_t curr_epoch = 0;                  /* current epoch */

  bzero(&cid, sizeof(cid));

  if (time(&curr_epoch) == -1) {
    syslog(LOG_WARNING, "time() error in discover()");
    return(-1);
  }

  reqlease = get_reqlease(rcv.dhcp, rdhcplen);
  maxoptlen = get_maxoptlen(rcv.dhcp, rdhcplen);
  get_cid(rcv.dhcp, rdhcplen, &cid);
  if (get_subnet(rcv.dhcp, rdhcplen, &cid.subnet, ifp) != 0)
    return(-1);
  if ((offer_res = choose_res(&cid, curr_epoch, reqlease)) == NULL)
    return(-1);
  offer_lease = choose_lease(reqlease, curr_epoch, offer_res);

  if (update_db(DHCPDISCOVER, &cid, offer_res, offer_lease, curr_epoch) != 0)
    return(-1);

  construct_msg(DHCPOFFER, offer_res, offer_lease, ifp);

  /* xxx insert options for the specific client */
  /* xxx insert options for the specific client-type */
  /* send DHCPOFFER from the interface
   * xxx must be able to handle the fragments, but currently not implemented
   */

  send_dhcp(ifp, DHCPOFFER);
 
  return(0);
}


/*******************************
 * process DHCPREQUEST message *
 * which incoming from ether   *
 *******************************/
static int
request(ifp)
  struct if_info *ifp;
{
  int reqforme = 0, nosuchaddr = 0;
  struct dhcp_resource *res = NULL;
  struct client_id cid;
  struct in_addr reqip, netmask;
  unsigned Long offer_lease = 0,         /* offerring lease */
                reqlease = 0;            /* requested lease duration */
  char *option = NULL;
#define EPOCH      "Thu Jan  1 00:00:00 1970\n"
#define BRDCSTSTR  "255.255.255.255"
  char datestr[sizeof(EPOCH)];
  char addrstr[sizeof(BRDCSTSTR)];
  time_t curr_epoch = 0;                 /* current epoch */

  bzero(&cid, sizeof(cid));
  bcopy(EPOCH, datestr, sizeof(EPOCH));
  bcopy(BRDCSTSTR, addrstr, sizeof(BRDCSTSTR));

  if (time(&curr_epoch) == -1) {
    syslog(LOG_WARNING, "time() error in request()");
    return(-1);
  }

  reqlease = get_reqlease(rcv.dhcp, rdhcplen);
  maxoptlen = get_maxoptlen(rcv.dhcp, rdhcplen);
  get_cid(rcv.dhcp, rdhcplen, &cid);
  if (get_subnet(rcv.dhcp, rdhcplen, &cid.subnet, ifp) != 0)
    return(-1);

  /* check REQUEST for me ? */
  if ((option = pickup_opt(rcv.dhcp, rdhcplen, SERVER_ID)) != NULL) {
    if (htonl(GETHL(OPTBODY(option))) == ifp->ipaddr->s_addr)
      reqforme = 1;
  }

  if (rcv.dhcp->ciaddr.s_addr != 0) {
    if (get_snmk(rcv.dhcp, rdhcplen, &netmask, ifp) == 0) {
      if (rcv.dhcp->giaddr.s_addr != 0) {
	if ((rcv.dhcp->giaddr.s_addr & netmask.s_addr) !=
	    (rcv.dhcp->ciaddr.s_addr & netmask.s_addr))
	  goto nak;
      }
    }

    if ((res = select_wciaddr(&cid, curr_epoch, &nosuchaddr)) == NULL) {

      if (nosuchaddr == TRUE)
	return(-1);
#ifndef COMPAT_RFC1541
      else
	goto nak;
#endif
    } else {
      goto ack;
    }
  }

  reqip.s_addr = 0;
  if ((option = pickup_opt(rcv.dhcp, rdhcplen, REQUEST_IPADDR)) != NULL) {
    reqip.s_addr = htonl(GETHL(OPTBODY(option)));
  }

#ifdef COMPAT_RFC1541
  if (reqip.s_addr == 0)
    reqip.s_addr = rcv.dhcp->ciaddr.s_addr;
#endif

  if (reqip.s_addr != 0) {
    if (get_snmk(rcv.dhcp, rdhcplen, &netmask, ifp) == 0) {
      if (rcv.dhcp->giaddr.s_addr != 0) {
	if ((rcv.dhcp->giaddr.s_addr & netmask.s_addr) !=
	    (reqip.s_addr & netmask.s_addr))
	  goto nak;
      } else {
	if ((ifp->ipaddr->s_addr & netmask.s_addr) !=
	    (reqip.s_addr & netmask.s_addr))
	  goto nak;
      }
    }

    res = NULL;
    res = select_wcid(DHCPREQUEST, &cid, curr_epoch);
    if (res != NULL && res->ip_addr != NULL &&
        res->ip_addr->s_addr == reqip.s_addr)
      goto ack;
    else if (reqforme == 1)
      goto nak;
    else
      return(-1);
    
    if ((res = select_wreqip(DHCPREQUEST, &cid, curr_epoch)) != NULL)
      goto ack;
    else if (reqforme == 1)
      goto nak;
  }
  return(-1);

 ack:
  offer_lease = choose_lease(reqlease, curr_epoch, res);
      
  if (update_db(DHCPREQUEST, &cid, res, offer_lease, curr_epoch) != 0) {
    if (reqforme == 1)
      goto nak;
    else
      return(-1);
  }

  construct_msg(DHCPACK, res, offer_lease, ifp);

  /* xxx insert options for the specific client */
  /* xxx insert options for the specific client-type */
  /* send DHCPACK from the interface
   * xxx must be able to handle the fragments, but currently not implemented
   */
  send_dhcp(ifp, DHCPACK);

  res->binding->flag |= COMPLETE_ENTRY;    /* binding is made completely */

#ifdef solaris
  strcpy(datestr, ctime(&res->binding->expire_epoch));
#else
  strcpy(datestr, _ctime(&res->binding->expire_epoch));
#endif
  datestr[strlen(datestr) - 1] = '\0';
  strcpy(addrstr, inet_ntoa(*res->ip_addr));
  errno = 0;
  syslog(LOG_INFO, "Assign %s to the client(cid is \"%s\") till \"%s\".",
	 addrstr, cidtos(&res->binding->cid, 1), datestr);
  return(0);

 nak:
  construct_msg(DHCPNAK, NULL, offer_lease, ifp);
  /* send DHCPNAK from the interface
   * xxx must be able to handle the fragments, but currently not implemented
   */
  send_dhcp(ifp, DHCPNAK);

  return(0);
}


/*******************************
 * process DHCPDECLINE message *
 *******************************/
static int
decline(ifp)
  struct if_info *ifp;
{
  struct dhcp_binding *binding = NULL;
  struct dhcp_resource *res = NULL;
  struct client_id cid;
  char *option = NULL,
       *msg = NULL;

  bzero(&cid, sizeof(cid));

  /* DECLINE for another server */
  if ((option = pickup_opt(rcv.dhcp, rdhcplen, SERVER_ID)) == NULL ||
      htonl(GETHL(OPTBODY(option))) != ifp->ipaddr->s_addr) {
    return(0);
  }

  get_cid(rcv.dhcp, rdhcplen, &cid);
  if (get_subnet(rcv.dhcp, rdhcplen, &cid.subnet, ifp) != 0)
    return(-1);

  /* search with haddr (haddr is same as client identifier) */
  if ((binding = (struct dhcp_binding *)
       hash_find(&cidhashtable, cid.id, cid.idlen,
		 bindcidcmp, &cid)) == NULL) {
    errno = 0;
    syslog(LOG_INFO,
	   "Receive DHCPDECLINE, but can't find binding for such client");
    return(-1);
  } else {
    res = binding->res;
  }

  /* now, lease_id is the combination of "chaddr" and "assigned address" */
  if (binding->res != NULL) {
    /* for a while, this resource turn off
       (expect that illegal host will be disconnected soon until 30 min )*/
    turnoff_bind(binding);
  } else {
    errno = 0;
    syslog(LOG_INFO, "Receive the invalid DHCPDECLINE.");
    return(0);
  }

  errno = 0;
  syslog(LOG_INFO, "Receive the DHCPDECLINE (entryname=%s IP=%s).",
	 binding->res->entryname, inet_ntoa(*binding->res->ip_addr));
  if ((option = pickup_opt(rcv.dhcp, rdhcplen, DHCP_ERRMSG)) != NULL) {
    msg = nvttostr(OPTBODY(option), (int) DHCPOPTLEN(option));
    if (msg != NULL) {
      if (msg[0] != '\0') {
	errno = 0;
	syslog(LOG_INFO, "\"%s\" is the message from client.", msg);
      }
      free(msg);
    }
  }

  return(0);
}


/*******************************
 * process DHCPRELEASE message *
 *******************************/
static int
release(ifp)
  struct if_info *ifp;
{
  struct dhcp_binding *binding = NULL;
  struct dhcp_resource *res = NULL;
  char *option = NULL;

  /* release for another server */
  if ((option = pickup_opt(rcv.dhcp, rdhcplen, SERVER_ID)) == NULL ||
      htonl(GETHL(OPTBODY(option))) != ifp->ipaddr->s_addr) {
    return(0);
  }

  /* search with ciaddr */
  if ((res = (struct dhcp_resource *)
       hash_find(&iphashtable, (char *)&rcv.dhcp->ciaddr.s_addr,
		       sizeof(u_Long), resipcmp, &rcv.dhcp->ciaddr)) == NULL) {
    errno = 0;
    syslog(LOG_INFO,
	   "Receive DHCPRELEASE(IP:%s), but can't find binding for such client",
	   inet_ntoa(rcv.dhcp->ciaddr));
    return(-1);
  } else {
    binding = res->binding;
  }

  /* now, lease_id is the combination of "chaddr" and "assigned address" */
  if (binding != NULL && binding->res != NULL &&
      binding->res == res &&
      isclr(binding->res->valid, S_CLIENT_ID) &&
      binding->haddr.htype == rcv.dhcp->htype &&
      binding->haddr.hlen == rcv.dhcp->hlen &&
      bcmp(binding->haddr.haddr, rcv.dhcp->chaddr, rcv.dhcp->hlen) == 0) {
    binding->expire_epoch = 0;
  } else {
    errno = 0;
    if (binding != NULL && binding->res != NULL &&
	isset(binding->res->valid, S_CLIENT_ID)) {
      syslog(LOG_INFO, "Receive DHCPRELEASE for static entry (IP:%s).",
	     inet_ntoa(rcv.dhcp->ciaddr));
    } else {
      syslog(LOG_INFO, "Receive an inappropriate DHCPRELEASE(IP:%s).",
	     inet_ntoa(rcv.dhcp->ciaddr));
    }
    return(0);
  }

  errno = 0;
  syslog(LOG_INFO, "Receive DHCPRELEASE and release %s", inet_ntoa(rcv.dhcp->ciaddr));
  return(0);
}


/************************************
 * send DHCP message from interface *
 ************************************/
static int
send_dhcp(ifp, msgtype)
  struct if_info *ifp;
  int msgtype;
{
  int i = 0, buflen = 0;
  struct iovec bufvec[2];
  struct sockaddr_in dst;
  struct msghdr msg;

  bzero(&dst, sizeof(dst));

  if (overload & FILE_ISOPT) {
    snd.dhcp->file[off_file] = END;
  }
  if (overload & SNAME_ISOPT) {
    snd.dhcp->sname[off_sname] = END;
  }
  if (off_options < DFLTOPTLEN) {
    snd.dhcp->options[off_options] = END;
  } else if (off_extopt > 0 && off_extopt < maxoptlen - DFLTOPTLEN &&
	     sbufvec[1].iov_base != NULL) {
    sbufvec[1].iov_base[off_extopt++] = END;
  }

  if (off_extopt < sbufvec[1].iov_len) {
    sbufvec[1].iov_len = off_extopt;
  }

  /* if the message was not received from bpf or nit,
     send reply from normal socket */
  if (rcv.dhcp->giaddr.s_addr != 0 ||
      (rcv.dhcp->ciaddr.s_addr != 0 &&
       (rcv.dhcp->ciaddr.s_addr & ifp->subnetmask->s_addr) !=
       (ifp->ipaddr->s_addr & ifp->subnetmask->s_addr))) {

    dst.sin_family = AF_INET;
    if (rcv.dhcp->ciaddr.s_addr != 0) {
      dst.sin_port = dhcpc_port;
      bcopy(&rcv.dhcp->ciaddr, &dst.sin_addr, sizeof(u_Long));
    }
    else if (rcv.dhcp->giaddr.s_addr != 0) {
      dst.sin_port = dhcps_port;
      bcopy(&rcv.dhcp->giaddr, &dst.sin_addr, sizeof(u_Long));
    }

    bufvec[0].iov_base = (char *) snd.dhcp;
    buflen = bufvec[0].iov_len = DFLTDHCPLEN;
    bufvec[1].iov_base = sbufvec[1].iov_base;
    if (bufvec[1].iov_base == NULL) {
      bufvec[1].iov_len = 0;
    } else {
      bufvec[1].iov_len = sbufvec[1].iov_len;
    }
    buflen += bufvec[1].iov_len;
    if (setsockopt(if_list->fd, SOL_SOCKET, SO_SNDBUF, &buflen,
		   sizeof(buflen)) < 0) {
      syslog(LOG_WARNING, "setsockopt() in send_dhcp()");
      return(-1);
    }

    bzero(&msg, sizeof(msg));
    msg.msg_name = (caddr_t) &dst;
    msg.msg_namelen = sizeof(dst);
    msg.msg_iov = bufvec;
    if (bufvec[1].iov_base == NULL)
      msg.msg_iovlen = 1;
    else
      msg.msg_iovlen = 2;

    if (sendmsg(if_list->fd, &msg, 0) < 0) {
      syslog(LOG_WARNING, "send DHCP message failed");
      return(-1);
    }

    return(0);
  }

  /* 
   * Message was not relayed and client either has an IP address or is on the
   * same subnet as the server.
   */

  /* fill pseudo header to calculating checksum */
  bcopy(&ifp->ipaddr->s_addr, &snd.ip->ip_src, sizeof(u_Long));
  if (snd.dhcp->yiaddr.s_addr != 0 && !ISBRDCST(rcv.dhcp->flags)) {
    bcopy(&snd.dhcp->yiaddr.s_addr, &snd.ip->ip_dst.s_addr, sizeof(u_Long));
  } else {
    snd.ip->ip_dst.s_addr = 0xffffffff;   /* default dst */
  }

  snd.udp->uh_sport = dhcps_port;
  snd.udp->uh_dport = dhcpc_port;
  snd.udp->uh_ulen = htons(off_extopt + DFLTDHCPLEN + UDPHL);
  snd.udp->uh_sum = get_udpsum(snd.ip, snd.udp);

  snd.ip->ip_v = IPVERSION;
  snd.ip->ip_hl = IPHL >> 2;
  snd.ip->ip_tos = 0;
  snd.ip->ip_len = htons(off_extopt + DFLTDHCPLEN + UDPHL + IPHL);
  snd.ip->ip_id = snd.udp->uh_sum;
  snd.ip->ip_off = htons(IP_DF);    /* XXX */
  snd.ip->ip_ttl = 0x20;            /* XXX */
  snd.ip->ip_p = IPPROTO_UDP;
  snd.ip->ip_sum = get_ipsum(snd.ip);

  if (rcv.dhcp->htype == ETHER) {
    if (ISBRDCST(rcv.dhcp->flags) || (snd.dhcp->yiaddr.s_addr == 0)) {
      for (i = 0; i < 6; i++) {
#ifdef sun
	snd.ether->ether_dhost.ether_addr_octet[i] = 0xff;
#else
	snd.ether->ether_dhost[i] = 0xff;
#endif
      }
    }
    else {
      for (i = 0; i < 6; i++) {
#ifdef sun
	snd.ether->ether_dhost.ether_addr_octet[i] = rcv.dhcp->chaddr[i];
#else
	snd.ether->ether_dhost[i] = rcv.dhcp->chaddr[i];
#endif
      }
    }
  }
  else {
    for (i = 0; i < 6; i++) {
#ifdef sun
      snd.ether->ether_dhost.ether_addr_octet[i] =
	rcv.ether->ether_shost.ether_addr_octet[i];
#else
      snd.ether->ether_dhost[i] = rcv.ether->ether_shost[i];
#endif
    }
  }

  for (i = 0; i < 6; i++) {
#ifdef sun
    snd.ether->ether_shost.ether_addr_octet[i] = ifp->haddr[i];
#else
    snd.ether->ether_shost[i] = ifp->haddr[i];
#endif
  }
  snd.ether->ether_type = htons(ETHERTYPE_IP);

  if (sbufvec[1].iov_base == NULL) {
    i = ether_write(ifp->fd, sbufvec[0].iov_base, sbufvec[0].iov_len);
    if (i < 0) {
      syslog(LOG_WARNING, "send DHCP message failed");
      return(-1);
    }
  } else {
    if (ether_writev(ifp->fd, sbufvec, 2) < 0) {
      syslog(LOG_WARNING, "send DHCP message failed");
      return(-1);
    }
  }
 
  return(0);
}


#ifndef NOBOOTP
/*
 * return true if resource is available for bootp
 */
static int
available_forbootp(res, cid, curr_epoch)
  struct dhcp_resource *res;
  struct client_id *cid;
  time_t curr_epoch;
{
  if (res->allow_bootp == FALSE)
    return(FALSE);

  if (res->binding == NULL ||
      cidcmp(&res->binding->cid, cid) ||
      (res->binding->expire_epoch != 0xffffffff &&
       res->binding->expire_epoch < curr_epoch)) {
    return(TRUE);
  }

  return(FALSE);
}


/*
 * choose the address newly for bootp client
 */
static struct dhcp_resource *
choose_forbootp(cid, curr_epoch)
  struct client_id *cid;
  time_t curr_epoch;
{
  struct dhcp_resource *res = NULL, *offer = NULL;
  struct hash_member *resptr = NULL;

  resptr = reslist;
  while (resptr != NULL) {
    res = (struct dhcp_resource *) resptr->data;

    /* if it is dummy entry, skip it */
    if (res->ip_addr == NULL) {
      resptr = resptr->next;
      continue;
    }

    /* check the resource. valid subnet?, available resource? */
    if (cid->subnet.s_addr == (res->ip_addr->s_addr & res->subnet_mask->s_addr) &&
	available_forbootp(res, cid, curr_epoch)) {

      if (rcv.dhcp->ciaddr.s_addr != 0) {
	offer = res;
	break;
      }
      /* specify DHCPDISCOVER as dummy */
      else if (icmp_check(DHCPDISCOVER, res->ip_addr) == GOOD) {
	offer = res;
	break;
      } else {
	turnoff_bind(res->binding);
      }

    }

    resptr = resptr->next;
  }
  if (offer == NULL) {
    errno = 0;
    syslog(LOG_WARNING, "BOOTP: No more available address in the pool");
  }
  return(offer);
}


/*
 * construct BOOTP REPLY
 */
static void
construct_bootp(res)
  struct dhcp_resource *res;
{
  int i = 0;
  char inserted[32];

  bzero(inserted, sizeof(inserted));

  clean_sbuf();
  overload = 0;
  snd.dhcp->op = BOOTREPLY;
  snd.dhcp->htype = rcv.dhcp->htype;
  snd.dhcp->hlen = rcv.dhcp->hlen;
  snd.dhcp->hops = 0;
  snd.dhcp->xid = rcv.dhcp->xid;
  snd.dhcp->secs = 0;
  if (rcv.dhcp->giaddr.s_addr != 0) {
    snd.dhcp->flags = rcv.dhcp->flags;
  } else {
    snd.dhcp->flags = 0;
  }
  snd.dhcp->giaddr.s_addr = rcv.dhcp->giaddr.s_addr;
  bcopy(rcv.dhcp->chaddr, snd.dhcp->chaddr, rcv.dhcp->hlen);

  snd.dhcp->yiaddr.s_addr = res->ip_addr->s_addr;
  if (isset(res->valid, S_SIADDR)) {
    snd.dhcp->siaddr.s_addr = res->siaddr->s_addr;
  } else {
    snd.dhcp->siaddr.s_addr = 0;
  }
  if (isset(res->valid, S_SNAME)) {
    strncpy(snd.dhcp->sname, res->sname, MAX_SNAME);
    snd.dhcp->sname[MAX_SNAME - 1] = '\0';  /* NULL terminate cerntainly */
  }
  if (isset(res->valid, S_FILE)) {
    strncpy(snd.dhcp->file, res->file, MAX_FILE);
    snd.dhcp->file[MAX_FILE - 1] = '\0';    /* NULL terminate cerntainly */
  }

  /* insert magic cookie */
  bcopy(magic_c, snd.dhcp->options, MAGIC_LEN);
  off_options = MAGIC_LEN;
  off_extopt = 0;

  /* insert "subnet mask" */
  if (insert_opt(res, 0xffffffff, SUBNET_MASK, inserted, PASSIVE) == E_NOMORE) {
    errno = 0;
    syslog(LOG_INFO, "No more space left to insert options to BOOTP");
  }

  /* insert that is different from "Host requirement RFC" defaults */
  for (i = 0; i < LAST_OPTION; i++) {
    if (isclr(inserted, i)) {
      if (insert_opt(res, 0xffffffff, i, inserted, PASSIVE) == E_NOMORE) {
	errno = 0;
	syslog(LOG_INFO, "No more space left to insert options to BOOTP");
	break;
      }
    }
  }

  return;
}


/*************************
 * process BOOTP message *
 *************************/
/* argsused */
static int
bootp(ifp)
  struct if_info *ifp;
{
  char addrstr[sizeof(BRDCSTSTR)];
  struct client_id cid;
  struct dhcp_binding *binding = NULL;
  struct dhcp_resource *res = NULL;
  time_t curr_epoch = 0;

  bzero(&cid, sizeof(cid));
  bcopy(BRDCSTSTR, addrstr, sizeof(BRDCSTSTR));

#ifdef NOBOOTP
  return(0);
#endif

  if (time(&curr_epoch) == -1) {
    syslog(LOG_WARNING, "time() error in bootp()");
    return(-1);
  }
  get_cid(rcv.dhcp, rdhcplen, &cid);
  if (get_subnet(rcv.dhcp, rdhcplen, &cid.subnet, ifp) != 0)
    return(-1);
  maxoptlen = BOOTPOPTLEN;

  /* search with haddr (haddr is same as client identfier) */
  if ((binding = (struct dhcp_binding *)
       hash_find(&cidhashtable, cid.id, cid.idlen, bindcidcmp, &cid)) != NULL) {
    if (cidcmp(&binding->cid, &cid))
      res = binding->res;
  }
  if (res == NULL && (res = choose_forbootp(&cid, curr_epoch)) == NULL)
    return(-1);

  if (update_db(BOOTP, &cid, res, 0xffffffff, curr_epoch) != 0)
    return(-1);

  /* binding is made completely */
  res->binding->flag |= (COMPLETE_ENTRY | BOOTP_ENTRY);

  construct_bootp(res);
  send_bootp(ifp);
  strcpy(addrstr, inet_ntoa(*res->ip_addr));
  errno = 0;
  syslog(LOG_INFO, "Reply to the bootp client(IP:%s, cid:\"%s\").",
	 addrstr, cidtos(&res->binding->cid, 1));

  return(0);
}


/*************************************
 * send BOOTP message from interface *
 *************************************/
static int
send_bootp(ifp)
  struct if_info *ifp;
{
  int i = 0, buflen = 0;
  struct sockaddr_in srcaddr, dstaddr;

  bzero(&srcaddr, sizeof(srcaddr));
  bzero(&dstaddr, sizeof(dstaddr));

  if (off_options < BOOTPOPTLEN) {
    snd.dhcp->options[off_options] = END;
  }

  /* if received message was relayed from relay agent,
     send reply from normal socket */
  if (rcv.dhcp->giaddr.s_addr != 0) {
    dstaddr.sin_family = AF_INET;
    if (rcv.dhcp->ciaddr.s_addr == 0 ||
        rcv.dhcp->ciaddr.s_addr != snd.dhcp->yiaddr.s_addr) {
      dstaddr.sin_port = dhcps_port;
      bcopy(&rcv.dhcp->giaddr, &dstaddr.sin_addr, sizeof(u_Long));
    } else {
      dstaddr.sin_port = dhcpc_port;
      bcopy(&snd.dhcp->yiaddr, &dstaddr.sin_addr, sizeof(u_Long));
    }

    buflen = DFLTBOOTPLEN;
    if (setsockopt(ifp->fd, SOL_SOCKET, SO_SNDBUF, &buflen, sizeof(buflen)) < 0) {
      syslog(LOG_WARNING, "setsockopt() in send_bootp()");
      return(-1);
    }

    if (ifp == if_list) {
      if (sendto(ifp->fd, snd.dhcp, buflen, 0, (struct sockaddr *)&dstaddr,
		 sizeof(dstaddr)) < 0) {
	syslog(LOG_WARNING, "send BOOTP message failed");
	return(-1);
      }
    } else {
      if (write(ifp->fd, snd.dhcp, buflen) < 0) {
	syslog(LOG_WARNING, "send BOOTP message failed");
	return(-1);
      }
    }

    return(0);
  }

  /* if directly received packet.... */

  /* fill pseudo header to calculating checksum */
  bcopy(&ifp->ipaddr->s_addr, &snd.ip->ip_src, sizeof(u_Long));
  if (ISBRDCST(rcv.dhcp->flags) || rcv.dhcp->ciaddr.s_addr != snd.dhcp->yiaddr.s_addr) {
    snd.ip->ip_dst.s_addr = 0xffffffff;   /* default dst */
  } else {
    bcopy(&snd.dhcp->yiaddr.s_addr, &snd.ip->ip_dst.s_addr, sizeof(u_Long));
  }
  snd.udp->uh_sport = dhcps_port;
  snd.udp->uh_dport = dhcpc_port;
  snd.udp->uh_ulen = htons(DFLTBOOTPLEN + UDPHL);
  snd.udp->uh_sum = get_udpsum(snd.ip, snd.udp);

  snd.ip->ip_v = IPVERSION;
  snd.ip->ip_hl = IPHL >> 2;
  snd.ip->ip_tos = 0;
  snd.ip->ip_len = htons(DFLTBOOTPLEN + UDPHL + IPHL);
  snd.ip->ip_id = snd.udp->uh_sum;
  snd.ip->ip_off = htons(IP_DF);    /* XXX */
  snd.ip->ip_ttl = 0x20;            /* XXX */
  snd.ip->ip_p = IPPROTO_UDP;
  snd.ip->ip_sum = get_ipsum(snd.ip);

  if (ISBRDCST(rcv.dhcp->flags) || (rcv.dhcp->ciaddr.s_addr != snd.dhcp->yiaddr.s_addr)) {
    for (i = 0; i < 6; i++) {
#ifdef sun
      snd.ether->ether_shost.ether_addr_octet[i] = ifp->haddr[i];
      snd.ether->ether_dhost.ether_addr_octet[i] = 0xff;
#else
      snd.ether->ether_shost[i] = ifp->haddr[i];
      snd.ether->ether_dhost[i] = 0xff;
#endif
    }
  }
  else {
    for (i = 0; i < 6; i++) {
#ifdef sun
      snd.ether->ether_shost.ether_addr_octet[i] = ifp->haddr[i];
      snd.ether->ether_dhost.ether_addr_octet[i] = rcv.dhcp->chaddr[i];
#else
      snd.ether->ether_shost[i] = ifp->haddr[i];
      snd.ether->ether_dhost[i] = rcv.dhcp->chaddr[i];
#endif
    }
  }
  snd.ether->ether_type = htons(ETHERTYPE_IP);

  buflen = DFLTBOOTPLEN + UDPHL + IPHL + ETHERHL;
  if (ether_write(ifp->fd, (char *)snd.ether, buflen) < 0) {
    syslog(LOG_WARNING, "send BOOTP message failed");
    return(-1);
  }

  return(0);
}
#endif /* NOBOOTP */


/*
 * insert the IP address
 */
/* ARGSUSED */
static int
ins_ip(res, lease, tagnum, inserted, flag)
  struct dhcp_resource *res;
  u_Long lease;
  int tagnum;
  char *inserted;
  char flag;
{
  u_Long *addr = 0;
  char option[6];
  int symbol = 0;
  int retval = 0;

  bzero(option, sizeof(option));

  switch(tagnum) {
  case SUBNET_MASK:
    symbol = S_SUBNET_MASK, addr = &res->subnet_mask->s_addr;
    break;
  case SWAP_SERVER:
    symbol = S_SWAP_SERVER, addr = &res->swap_server->s_addr;
    break;
  case BRDCAST_ADDR:
    symbol = S_BRDCAST_ADDR, addr = &res->brdcast_addr->s_addr;
    break;
  case ROUTER_SOLICIT:
    symbol = S_ROUTER_SOLICIT, addr = &res->router_solicit->s_addr;
    break;
  default:
    return(-1);
  }
    
  if ((flag == PASSIVE && isset(res->valid, symbol)) ||
      (flag == ACTIVE && isset(res->active, symbol))) {
    option[0] = tagnum;
    option[1] = 4;
    bcopy(addr, &option[2], 4);
    if ((retval = insert_it(option)) == 0)
      setbit(inserted, tagnum);
  }
  return(retval);
}


/*
 * insert IP addresses
 */
/* ARGSUSED */
static int
ins_ips(res, lease, tagnum, inserted, flag)
  struct dhcp_resource *res;
  u_Long lease;
  int tagnum;
  char *inserted;
  char flag;
{
  struct in_addrs *addr = NULL;
  char option[254];
  int symbol = 0;
  int retval = 0;
  int i = 0;

  bzero(option, sizeof(option));

  switch(tagnum) {
  case ROUTER:
    symbol = S_ROUTER, addr = res->router;
    break;
  case TIME_SERVER:
    symbol = S_TIME_SERVER, addr = res->time_server;
    break;
  case NAME_SERVER:
    symbol = S_NAME_SERVER, addr = res->name_server;
    break;
  case DNS_SERVER:
    symbol = S_DNS_SERVER, addr = res->dns_server;
    break;
  case LOG_SERVER:
    symbol = S_LOG_SERVER, addr = res->log_server;
    break;
  case COOKIE_SERVER:
    symbol = S_COOKIE_SERVER, addr = res->cookie_server;
    break;
  case LPR_SERVER:
    symbol = S_LPR_SERVER, addr = res->lpr_server;
    break;
  case IMPRESS_SERVER:
    symbol = S_IMPRESS_SERVER, addr = res->impress_server;
    break;
  case RLS_SERVER:
    symbol = S_RLS_SERVER, addr = res->rls_server;
    break;
  case NIS_SERVER:
    symbol = S_NIS_SERVER, addr = res->nis_server;
    break;
  case NTP_SERVER:
    symbol = S_NTP_SERVER, addr = res->ntp_server;
    break;
  case NBN_SERVER:
    symbol = S_NBN_SERVER, addr = res->nbn_server;
    break;
  case NBDD_SERVER:
    symbol = S_NBDD_SERVER, addr = res->nbdd_server;
    break;
  case XFONT_SERVER:
    symbol = S_XFONT_SERVER, addr = res->xfont_server;
    break;
  case XDISPLAY_MANAGER:
    symbol = S_XDISPLAY_MANAGER, addr = res->xdisplay_manager;
    break;
  case NISP_SERVER:
    symbol = S_NISP_SERVER, addr = res->nisp_server;
    break;
  case MOBILEIP_HA:
    symbol = S_MOBILEIP_HA, addr = res->mobileip_ha;
    break;
  case SMTP_SERVER:
    symbol = S_SMTP_SERVER, addr = res->smtp_server;
    break;
  case POP3_SERVER:
    symbol = S_POP3_SERVER, addr = res->pop3_server;
    break;
  case NNTP_SERVER:
    symbol = S_NNTP_SERVER, addr = res->nntp_server;
    break;
  case DFLT_WWW_SERVER:
    symbol = S_DFLT_WWW_SERVER, addr = res->dflt_www_server;
    break;
  case DFLT_FINGER_SERVER:
    symbol = S_DFLT_FINGER_SERVER, addr = res->dflt_finger_server;
    break;
  case DFLT_IRC_SERVER:
    symbol = S_DFLT_IRC_SERVER, addr = res->dflt_irc_server;
    break;
  case STREETTALK_SERVER:
    symbol = S_STREETTALK_SERVER, addr = res->streettalk_server;
    break;
  case STDA_SERVER:
    symbol = S_STDA_SERVER, addr = res->stda_server;
    break;
  default:
    return(-1);
  }

  if ((flag == PASSIVE && isset(res->valid, symbol)) ||
      (flag == ACTIVE && isset(res->active, symbol))) {
    option[0] = tagnum;
    option[1] = addr->num * 4;
    for (i = 0; i < addr->num; i++)
      bcopy(&addr->addr[i].s_addr, &option[i * 4 + 2], 4);
    if ((retval = insert_it(option)) == 0)
      setbit(inserted, tagnum);
  }
  return(retval);
}


/*
 * insert pairs of the IP address
 */
/* ARGSUSED */
static int
ins_ippairs(res, lease, tagnum, inserted, flag)
  struct dhcp_resource *res;
  u_Long lease;
  int tagnum;
  char *inserted;
  char flag;
{
  struct ip_pairs *pair = NULL;
  char option[254];
  int symbol = 0;
  int retval = 0;
  int i = 0;

  bzero(option, sizeof(option));

  switch(tagnum) {
  case POLICY_FILTER:
    symbol = S_POLICY_FILTER, pair = res->policy_filter;
    break;
  case STATIC_ROUTE:
    symbol = S_STATIC_ROUTE, pair = res->static_route;
    break;
  default:
    return(-1);
  }

  if ((flag == PASSIVE && isset(res->valid, symbol)) ||
      (flag == ACTIVE && isset(res->active, symbol))) {
    option[0] = tagnum;
    option[1] = pair->num * 8;
    for (i = 0; i < pair->num; i++) {
      bcopy(&pair->addr1[i].s_addr, &option[i * 8 + 2], 4);
      bcopy(&pair->addr2[i].s_addr, &option[i * 8 + 6], 4);
    }
    if ((retval = insert_it(option)) == 0)
      setbit(inserted, tagnum);
  }
  return(retval);
}


/*
 * insert long
 */
/* ARGSUSED */
static int
ins_long(res, lease, tagnum, inserted, flag)
  struct dhcp_resource *res;
  u_Long lease;
  int tagnum;
  char *inserted;
  char flag;
{
  Long *num = NULL;
  char option[6];
  int symbol = 0;
  int retval = 0;

  bzero(option, sizeof(option));

  switch(tagnum) {
  case TIME_OFFSET:
    symbol = S_TIME_OFFSET, num = &res->time_offset;
    break;
  case MTU_AGING_TIMEOUT:
    symbol = S_MTU_AGING_TIMEOUT, num = (Long *) &res->mtu_aging_timeout;
    break;
  case ARP_CACHE_TIMEOUT:
    symbol = S_ARP_CACHE_TIMEOUT, num = (Long *) &res->arp_cache_timeout;
    break;
  case KEEPALIVE_INTER:
    symbol = S_KEEPALIVE_INTER, num = (Long *) &res->keepalive_inter;
    break;
  default:
    return(-1);
  }
    
  if ((flag == PASSIVE && isset(res->valid, symbol)) ||
      (flag == ACTIVE && isset(res->active, symbol))) {
    option[0] = tagnum;
    option[1] = 4;
    bcopy(num, &option[2], 4);
    if ((retval = insert_it(option)) == 0)
      setbit(inserted, tagnum);
  }
  return(retval);
}


/*
 * insert short
 */
/* ARGSUSED */
static int
ins_short(res, lease, tagnum, inserted, flag)
  struct dhcp_resource *res;
  u_Long lease;
  int tagnum;
  char *inserted;
  char flag;
{
  short *num = NULL;
  char option[4];
  int symbol = 0;
  int retval = 0;

  bzero(option, sizeof(option));

  switch(tagnum) {
  case BOOTSIZE:
    symbol = S_BOOTSIZE, num = (short *) &res->bootsize;
    break;
  case MAX_DGRAM_SIZE:
    symbol = S_MAX_DGRAM_SIZE, num = (short *) &res->max_dgram_size;
    break;
  case IF_MTU:
    symbol = S_IF_MTU, num = (short *) &res->intf_mtu;
    break;
  default:
    return(-1);
  }
    
  if ((flag == PASSIVE && isset(res->valid, symbol)) ||
      (flag == ACTIVE && isset(res->active, symbol))) {
    option[0] = tagnum;
    option[1] = 2;
    bcopy(num, &option[2], 2);
    if ((retval = insert_it(option)) == 0)
      setbit(inserted, tagnum);
  }
  return(retval);
}


/*
 * insert octet
 */
/* ARGSUSED */
static int
ins_octet(res, lease, tagnum, inserted, flag)
  struct dhcp_resource *res;
  u_Long lease;
  int tagnum;
  char *inserted;
  char flag;
{
  char num = 0;
  char option[3];
  int symbol = 0;
  int retval = 0;

  bzero(option, sizeof(option));

  switch(tagnum) {
  case IP_FORWARD:
    symbol = S_IP_FORWARD, num = res->ip_forward;
    break;
  case NONLOCAL_SRCROUTE:
    symbol = S_NONLOCAL_SRCROUTE, num = res->nonlocal_srcroute;
    break;
  case DEFAULT_IP_TTL:
    symbol = S_DEFAULT_IP_TTL, num = res->default_ip_ttl;
    break;
  case ALL_SUBNET_LOCAL:
    symbol = S_ALL_SUBNET_LOCAL, num = res->all_subnet_local;
    break;
  case MASK_DISCOVER:
    symbol = S_MASK_DISCOVER, num = res->mask_discover;
    break;
  case MASK_SUPPLIER:
    symbol = S_MASK_SUPPLIER, num = res->mask_supplier;
    break;
  case ROUTER_DISCOVER:
    symbol = S_ROUTER_DISCOVER, num = res->router_discover;
    break;
  case TRAILER:
    symbol = S_TRAILER, num = res->trailer;
    break;
  case ETHER_ENCAP:
    symbol = S_ETHER_ENCAP, num = res->ether_encap;
    break;
  case DEFAULT_TCP_TTL:
    symbol = S_DEFAULT_TCP_TTL, num = res->default_tcp_ttl;
    break;
  case KEEPALIVE_GARBA:
    symbol = S_KEEPALIVE_GARBA, num = res->keepalive_garba;
    break;
  case NB_NODETYPE:
    symbol = S_NB_NODETYPE, num = res->nb_nodetype;
    break;
  default:
    return(-1);
  }
    
  if ((flag == PASSIVE && isset(res->valid, symbol)) ||
      (flag == ACTIVE && isset(res->active, symbol))) {
    option[0] = tagnum;
    option[1] = 1;
    option[2] = num;
    if ((retval = insert_it(option)) == 0)
      setbit(inserted, tagnum);
  }
  return(retval);
}


/*
 * insert string
 */
/* ARGSUSED */
static int
ins_str(res, lease, tagnum, inserted, flag)
  struct dhcp_resource *res;
  u_Long lease;
  int tagnum;
  char *inserted;
  char flag;
{
  char *str = NULL;
  char option[258];
  int symbol = 0;
  int retval = 0;
  int i = 0;

  bzero(option, sizeof(option));

  switch(tagnum) {
  case HOSTNAME:
    symbol = S_HOSTNAME, str = res->hostname;
    break;
  case MERIT_DUMP:
    symbol = S_MERIT_DUMP, str = res->merit_dump;
    break;
  case DNS_DOMAIN:
    symbol = S_DNS_DOMAIN, str = res->dns_domain;
    break;
  case ROOT_PATH:
    symbol = S_ROOT_PATH, str = res->root_path;
    break;
  case EXTENSIONS_PATH:
    symbol = S_EXTENSIONS_PATH, str = res->extensions_path;
    break;
  case NIS_DOMAIN:
    symbol = S_NIS_DOMAIN, str = res->nis_domain;
    break;
  case NB_SCOPE:
    symbol = S_NB_SCOPE, str = res->nb_scope;
    break;
  case NISP_DOMAIN:
    symbol = S_NISP_DOMAIN, str = res->nisp_domain;
    break;
  default:
    return(-1);
  }
    
  if ((flag == PASSIVE && isset(res->valid, symbol)) ||
      (flag == ACTIVE && isset(res->active, symbol))) {
    option[0] = tagnum;
    option[1] = ((i = strlen(str)) > MAXOPT) ? MAXOPT : i;
    bcopy(str, &option[2], option[1]);
    if ((retval = insert_it(option)) == 0)
      setbit(inserted, tagnum);
  }
  return(retval);
}


/*
 * insert dhcp_t1 or dhcp_t2
 */
static int
ins_dht(res, lease, tagnum, inserted, flag)
  struct dhcp_resource *res;
  u_Long lease;
  int tagnum;
  char *inserted;
  char flag;
{
  char option[6];
  int symbol = 0;
  int retval = 0;
  Long num = 0;

  bzero(option, sizeof(option));

  switch(tagnum) {
  case DHCP_T1:
    symbol = S_DHCP_T1;
    num = htonl(lease * (res->dhcp_t1 + (random() & 0x03)) / 1000);
    break;
  case DHCP_T2:
    symbol = S_DHCP_T2;
    num = htonl(lease * (res->dhcp_t2 + (random() & 0x03)) / 1000);
    break;
  default:
    return(-1);
  }
    
  if ((flag == PASSIVE && isset(res->valid, symbol)) ||
      (flag == ACTIVE && isset(res->active, symbol))) {
    option[0] = tagnum;
    option[1] = 4;
    bcopy(&num, &option[2], option[1]);
    if ((retval = insert_it(option)) == 0)
      setbit(inserted, tagnum);
  }
  return(retval);
}


/*
 * insert mtu_plateau_table
 */
static int
ins_mtpt(res, lease, tagnum, inserted, flag)
  struct dhcp_resource *res;
  u_Long lease;
  int tagnum;
  char *inserted;
  char flag;
{
  char option[256];
  int retval = 0;
  int i = 0;

  bzero(option, sizeof(option));

  if (tagnum != MTU_PLATEAU_TABLE) {
    return(-1);
  }

  if ((flag == PASSIVE && isset(res->valid, S_MTU_PLATEAU_TABLE)) ||
      (flag == ACTIVE && isset(res->active, S_MTU_PLATEAU_TABLE))) {
    option[0] = tagnum;
    option[1] = res->mtu_plateau_table->num * 2;
    for (i = 0; i < res->mtu_plateau_table->num; i++)
       bcopy(&res->mtu_plateau_table->shorts[i], &option[i * 2 + 2], 2); 
    if ((retval = insert_it(option)) == 0)
      setbit(inserted, tagnum);
  }
  return(retval);
}


/*
 * Insert the specified option
 */
static int
insert_opt(res, lease, tagnum, inserted, flag)
  struct dhcp_resource *res;
  u_Long lease;
  int tagnum;
  char *inserted;
  char flag;
{
  if (tagnum < PAD || tagnum > LAST_OPTION || ins_opt[tagnum] == NULL)
    return(-1);

  return((*ins_opt[tagnum])(res, lease, tagnum, inserted, flag));
}


static int
insert_it(opt)
  char *opt;
{
  char len = 0;
  int done = 0;

  len = opt[1] + 2;   /* 2 == tag number and length field */
  if (off_options + len < maxoptlen && off_options + len < DFLTOPTLEN) {
    bcopy(opt, &snd.dhcp->options[off_options], len);
    off_options += len;
    return(0);
  }
  else if ((overload & FILE_ISOPT) != 0 && off_file + len < MAX_FILE) {
    bcopy(opt, &snd.dhcp->file[off_file], len);
    off_file += len;
    return(0);
  }
  else if ((overload & SNAME_ISOPT) != 0 && off_sname + len < MAX_SNAME) {
    bcopy(opt, &snd.dhcp->sname[off_sname], len);
    off_sname += len;
    return(0);
  }
  else if (len < maxoptlen - off_options - off_extopt) {
    if (maxoptlen > DFLTOPTLEN) {
      if (sbufvec[1].iov_base == NULL) {
	if ((sbufvec[1].iov_base = calloc(1, maxoptlen - DFLTOPTLEN)) == NULL) {
	  syslog(LOG_WARNING, "calloc error in insert_it()");
	  return(-1);
	}
      }
      else if (sbufvec[1].iov_len < maxoptlen - DFLTOPTLEN) {
	free(sbufvec[1].iov_base);
	if ((sbufvec[1].iov_base = calloc(1, maxoptlen - DFLTOPTLEN)) == NULL) {
	  syslog(LOG_WARNING, "calloc error in insert_it()");
	  return(-1);
	}
      }
      sbufvec[1].iov_len = maxoptlen - DFLTOPTLEN;

      done = DFLTOPTLEN - off_options;
      bcopy(opt, &snd.dhcp->options[off_options], done);
      len -= done;
      off_options += done; /* invalid offset, So, to access
					    here will cause fatal error */
      bcopy(&opt[done], &sbufvec[1].iov_base[off_extopt], len);
      off_extopt += len;
      return(0);
    }
  }

  if ((off_options + off_extopt >= maxoptlen) &&
      ((overload & FILE_ISOPT) == 0 || off_file >= MAX_FILE) &&
      ((overload & SNAME_ISOPT) == 0 || off_sname >= MAX_SNAME)) {
    return(E_NOMORE);
  }
 
  return(-1);
}


/*
 * compare client identifier
 */
static int
cidcmp(cid1, cid2)
  struct client_id *cid1;
  struct client_id *cid2;
{
  return (cid1->subnet.s_addr == cid2->subnet.s_addr &&
	  cid1->idtype == cid2->idtype && cid1->idlen == cid2->idlen &&
	  bcmp(cid1->id, cid2->id, cid1->idlen) == 0);
}


/* 
 * do icmp echo request
 * if get reply, return BAD
 */
static int
icmp_check(msgtype, ip)
  int msgtype;
  struct in_addr *ip;
{
  struct protoent *protocol = NULL;
  struct sockaddr_in dst,
                     from;
  struct icmp *sicmp = NULL,
              *ricmp = NULL;
  struct ip *ipp = NULL;
  char rcvbuf[1024], sndbuf[ICMP_MINLEN];
  int rlen = 0,
      fromlen = 0,
      i = 0;
  int sockfd = 0;
  u_short pid = 0;

#ifdef NOICMPCHK
  return(GOOD);
#endif
  if (msgtype != DHCPDISCOVER)
    return(GOOD);

  bzero(&dst, sizeof(dst));
  bzero(&from, sizeof(from));
  bzero(sndbuf, sizeof(sndbuf));
  bzero(rcvbuf, sizeof(rcvbuf));

  sicmp = (struct icmp *) sndbuf;
  pid = (short) getpid() & 0xffff;

  if ((protocol = getprotobyname("icmp")) == NULL) {
    syslog(LOG_WARNING, "Can't get protocol \"icmp\" in icmp_check()");
    return(GOOD);
  }

  if ((sockfd = socket(AF_INET, SOCK_RAW, protocol->p_proto)) < 0) {
    syslog(LOG_WARNING, "socket error in icmp_check()");
    return(GOOD);
  }

#ifdef BSDOS
  delarp(ip);
#else
  delarp(ip, sockfd);
#endif

  i = 1;
  if (ioctl(sockfd, FIONBIO, &i) < 0) {
    syslog(LOG_WARNING, "ioctl(FIONBIO) in icmp_check()");
    return(GOOD);
  }

  dst.sin_family = AF_INET;
  dst.sin_addr.s_addr = ip->s_addr;

  sicmp->icmp_type = ICMP_ECHO;
  sicmp->icmp_code = 0;
  sicmp->icmp_cksum = 0;
  sicmp->icmp_id = pid;
  sicmp->icmp_seq = 0;

  sicmp->icmp_cksum =
    cksum((u_short *) sndbuf, sizeof(sndbuf) / sizeof(u_short));
  fromlen = sizeof(from);

  timeoutflag = 0;
  if ((int) signal(SIGALRM, timeout) < 0) {
    syslog(LOG_WARNING, "cannot set signal handler(SIGALRM)");
    return(GOOD);
  }
  ualarm(500000, 0);         /* wait 500 msec */

  i = sendto(sockfd, sndbuf, sizeof(sndbuf), 0,
	     (struct sockaddr *) &dst, sizeof(dst));
  if (i < 0 || i != sizeof(sndbuf)) {
    syslog(LOG_WARNING, "Can't send icmp echo request");
    return(GOOD);
  }

  while (1) { /* repeat until receive reply or timeout */
    rlen = recvfrom(sockfd, rcvbuf, sizeof(rcvbuf), 0,
		    (struct sockaddr *)&from, &fromlen);
    if (timeoutflag == 1) {
      break;
    }
    if (rlen < 0) {
      continue;
     }

    ipp = (struct ip *) rcvbuf;
    if (rlen < (ipp->ip_hl << 2) + ICMP_MINLEN) {
      continue;
    }
    ricmp = (struct icmp *) (rcvbuf + (ipp->ip_hl << 2));
    if (ricmp->icmp_type != ICMP_ECHOREPLY) {
      continue;
    }
    if (ricmp->icmp_id == pid) {
      break;
    }
  }
  close(sockfd);

  if (ricmp != NULL && ricmp->icmp_id == pid && timeoutflag != 1) {
    errno = 0;
    syslog(LOG_WARNING, "Maybe there is an illegal host which IP is %s",
	   inet_ntoa(*ip));
    return(BAD);
  }

  return(GOOD);
}


/*
 * timeout handling. only set timeoutflag to 1
 */
static void
timeout()
{
  timeoutflag = 1;
  return;
}

/*
 * free dhcp_binding
 */
static int
free_bind(hash_m)
  struct hash_member *hash_m;
{
  struct dhcp_binding *bp = NULL, *cbp = NULL;
  struct hash_member *current = NULL, *previous = NULL;

  bp = (struct dhcp_binding *) hash_m->data;
  previous = current = bindlist;
  while (current != NULL) {
    cbp = (struct dhcp_binding *) current->data;
    if (cbp != NULL && cidcmp(&bp->cid, &cbp->cid) == TRUE)
      break;
    else {
      previous = current;
      current = current->next;
    }
  }

  if (current == NULL) {
    if (previous != NULL) {
      previous->next = NULL;
    }
  } else {
    if (current == bindlist) {
      bindlist = current->next;
    } else {
      if (previous != NULL) {
	previous->next = current->next;
      }
    }
    free(current);
  }

  if (bp->res_name != NULL) free(bp->res_name);
  if (bp->cid.id != NULL) free(bp->cid.id);
  if (bp->haddr.haddr != NULL) free(bp->haddr.haddr);
  if (bp->res != NULL) {
    bp->res->binding = NULL;
    bp->res = NULL;
  }
  free(bp);
  free(hash_m);
  nbind--;

  return(0);
}


static int
free_fake(hash_m)
  struct hash_member *hash_m;
{
  nbind--;

  return(0);
}


/*
 * garbage collection. (timer driven. called once per an hour. )
 */
static void
garbage_collect()
{
#define MTOB(X)   ((struct dhcp_binding *)((X)->data))

  struct hash_member *bindptr = NULL,
                     *oldest = NULL;
  struct dhcp_binding *tmpptr;
  time_t curr_epoch = 0;
  static time_t prev_epoch = 0;

  if (time(&curr_epoch) == -1) {
    return;
  }
  if (curr_epoch - prev_epoch < GC_INTERVAL)
    return;

  prev_epoch = curr_epoch;
  dump_bind_db();

  bindptr = bindlist;
  while (bindptr != NULL) {
    tmpptr = MTOB(bindptr);
    if ((tmpptr->flag & COMPLETE_ENTRY) == 0 &&
	tmpptr->temp_epoch < curr_epoch) {
      hash_del(&cidhashtable, tmpptr->cid.id, tmpptr->cid.idlen, bindcidcmp,
	       &tmpptr->cid, free_bind);
    }
    bindptr = bindptr->next;
  }

  while (nbind > MAX_NBIND) {
    bindptr = bindlist;
    while (bindptr != NULL) {
      tmpptr = MTOB(bindptr);
      if ((tmpptr->flag & COMPLETE_ENTRY) != 0 &&
	  tmpptr->expire_epoch != 0xffffffff &&
	  tmpptr->expire_epoch < curr_epoch &&
	  (oldest == NULL ||
	   tmpptr->expire_epoch < MTOB(oldest)->expire_epoch)) {
	oldest = bindptr;
      }
      bindptr = bindptr->next;
    }

    if (oldest == NULL) {
      return;
    } else {
      tmpptr = MTOB(oldest);
      hash_del(&cidhashtable, tmpptr->cid.id, tmpptr->cid.idlen, bindcidcmp,
	       &tmpptr->cid, free_bind);
      oldest = NULL;
    }
  }

  return;
}


/*
 * convert NVT ASCII to strings
 * (actually, only remove null characters)
 */
static char *
nvttostr(nvtstr, length)
  char *nvtstr;
  int length;
{
  char *msg = NULL;
  register int i = 0;
  register char *tmp = NULL;

  if ((msg = calloc(1, length + 1)) == NULL) {
    syslog(LOG_WARNING, "calloc error in nvttostr()");
    return(msg);
  }

  tmp = msg;

  for (i = 0; i < length; i++) {
    if (nvtstr[i] != NULL) {
      *tmp = nvtstr[i];
      tmp++;
    }
  }

  msg[length] = '\0';
  return(msg);
}
