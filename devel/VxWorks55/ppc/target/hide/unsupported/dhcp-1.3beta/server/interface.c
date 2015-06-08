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
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <syslog.h>
#ifdef sun
#include <stropts.h>
#endif
#include <sys/ioctl.h>
#ifdef solaris
#include <sys/sockio.h>
#endif 
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <net/if.h>
#ifdef sun
#ifndef solaris
#include <net/nit.h>
#include <net/nit_if.h>
#include <net/nit_pf.h>
#include <net/packetfilt.h>
#else
#include <signal.h>
#include <sys/dlpi.h>
#include <sys/pfmod.h>

extern void sigalrm (void);
#endif
#else
#include <net/bpf.h>
#endif
#ifdef __osf__
#include <net/pfilt.h>
#endif
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/if_ether.h>

#ifdef __alpha
#define    Long    int
#define  u_Long  u_int
#else
#define    Long   long
#define  u_Long u_long
#endif
#include "common.h"

#if !defined(sony_news) && !defined(__FreeBSD__) && !defined(__osf__)
int getmac();
#endif

#ifdef sun
/********************************
 *    open the nit and setup    *
 ********************************/
int
open_if(ifinfo)
  struct if_info *ifinfo;
{
  u_Long i = 0;
  int n;
  struct ifreq ifreq;
  struct strioctl si;
  struct packetfilt pf;
  register u_short *fwp = &pf.Pf_Filter[0];
#ifdef solaris
  char buffer [10];           /* sizeof "/dev/enxx" */
  int  ppa = -1;               /* Device number of attachment. */
  int result;
  char DLPIbuf[256];
  struct strbuf ctl;
  struct timeval timeout;
  char *bufptr;
  union DL_primitives *dlp;
#endif

  /*
   *    open /dev/nit
   */
#ifndef solaris
  if ((ifinfo->fd = open("/dev/nit", O_RDWR)) < 0) {        /* cannot open nit */
    syslog(LOG_ERR, "open error in open_if()");
    exit(1);
  }
  if (ioctl(ifinfo->fd, I_SRDOPT, (char *)RMSGD) < 0) {
    syslog(LOG_ERR, "ioctl(I_SRDOPT) error in open_if()");
    exit(1);
  }
#else
  /* Separate the device into name and unit number for use with DLPI. */

  sprintf (buffer, "/dev/%s", ifinfo->name);
  bufptr = buffer + 5;
  while (*bufptr && !isdigit (*bufptr))
      bufptr++;
  if (!*bufptr)
      {
      syslog(LOG_ERR, "bad device name %s", buffer);
      exit (1);
      }
  
  ppa = atoi (bufptr);
  *bufptr = '\0';    /* Solaris machines do not use unit number in open(). */

  /* Open device for packet filter. */

  ifinfo->fd = open (buffer, O_RDWR);
  if (ifinfo->fd == -1) {    /* cannot open device */
    syslog(LOG_ERR, "open error in open_if()");
    exit(1);
  }
#endif

  bzero(&si, sizeof(si));
  bzero(&ifreq, sizeof(ifreq));
  strncpy(ifreq.ifr_name, ifinfo->name, sizeof(ifreq.ifr_name));
  ifreq.ifr_name[sizeof(ifreq.ifr_name) - 1] = '\0';

#ifndef solaris
  si.ic_cmd = NIOCBIND;
  si.ic_len = sizeof(ifreq);
  si.ic_dp = (char *) &ifreq;
  if (ioctl(ifinfo->fd, I_STR, (char *) &si) < 0) {
    syslog(LOG_ERR, "ioctl(NIOCBIND) error in open_if()");
    exit(1);
  }

  bzero(&si, sizeof(si));
  si.ic_cmd = NIOCSSNAP;
  si.ic_len = sizeof(i);
  si.ic_dp = (char *)&i;
  if (ioctl(ifinfo->fd, I_STR, (char *) &si) < 0) {
    syslog(LOG_ERR, "ioctl(NIOCSSNAP) error in open_if()");
    exit(1);
  }
#else
  if (dlattachreq(ifinfo->fd, ppa) == -1)
      {
      syslog (LOG_ERR, "DLPI error\n");
      exit (1);
      }
  if (dlokack (ifinfo->fd, DLPIbuf) == -1)
      {
      syslog (LOG_ERR, "DLPI error\n");
      exit (1);
      }

  dlbindreq (ifinfo->fd, ETHERTYPE_IP, 0, DL_CLDLS, 0, 0);
  dlbindack (ifinfo->fd, DLPIbuf);
#endif

  /*
   * Initialize the interface information. subnet and IP address
   */
  if ((n = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    syslog(LOG_ERR, "socket() in open_if()");
    exit(1);
  }
  if (ioctl(n, SIOCGIFNETMASK, &ifreq) < 0) {
    syslog(LOG_ERR, "ioctl(SIOCGIFNETMASK) in open_if()");
    exit(1);
  }
  if ((ifinfo->subnetmask =
       (struct in_addr *) calloc(1, sizeof(struct in_addr))) == NULL) {
    syslog(LOG_ERR, "calloc error in open_if()");
    exit(1);
  }
  ifinfo->subnetmask->s_addr =
    ((struct sockaddr_in *)&ifreq.ifr_addr)->sin_addr.s_addr;

  if (ioctl(n, SIOCGIFADDR, &ifreq) < 0) {
    syslog(LOG_ERR, "ioctl(SIOCGIFADDR) in open_if()");
    exit(1);
  }
  if ((ifinfo->ipaddr =
       (struct in_addr *) calloc(1, sizeof(struct in_addr))) == NULL) {
    syslog(LOG_ERR, "calloc in open_if()");
    exit(1);
  }
  ifinfo->ipaddr->s_addr = ((struct sockaddr_in *)&ifreq.ifr_addr)->sin_addr.s_addr;
  close(n);

  /* 1 = Ethernet address. 6 bytes long. */

  ifinfo->htype = 1;
  ifinfo->hlen = 6;
#ifndef solaris
  if (getmac(ifinfo->name, ifinfo->haddr) < 0) {
    exit(1);
  }

  if (ioctl(ifinfo->fd, I_PUSH, "pf") < 0) {
    syslog(LOG_ERR, "ioctl(I_PUSH) error in open_if()");
    exit(1);
  }
#else 
  /* Use DLPI to get interface hardware address. */

  dlphysaddrreq (ifinfo->fd, DL_CURR_PHYS_ADDR);
  dlphysaddrack (ifinfo->fd, DLPIbuf);

   dlp = (union DL_primitives *) DLPIbuf;

   bufptr = (char *)dlp + dlp->physaddr_ack.dl_addr_offset;
   bcopy (bufptr, ifinfo->haddr, 6);
#endif

#ifdef solaris
   if (strioctl (ifinfo->fd, DLIOCRAW, -1, 0, NULL) == -1)
       {
       syslog (LOG_ERR, "DLIOCRAW error\n");
       exit (-1);
       }
#endif

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
  *fwp++ = ENF_PUSHLIT | ENF_CAND;
  *fwp++ = dhcps_port;
#endif

  pf.Pf_FilterLen = fwp - &pf.Pf_Filter[0];

  bzero(&si, sizeof(si));

#ifndef solaris
  si.ic_cmd = NIOCSETF;
  si.ic_timout = INFTIM;
  si.ic_len = sizeof (pf);
  si.ic_dp = (char *)&pf;
  if (ioctl(ifinfo->fd, I_STR, (char *)&si) < 0){
    syslog(LOG_ERR, "ioctl(NIOCSETF) error in open_if()");
    exit(1);
  }
#else
  if (ioctl(ifinfo->fd, I_SRDOPT, (char *)(RMSGD | RPROTDIS)) < 0) {
    syslog(LOG_ERR, "ioctl(I_SRDOPT) error in open_if()");
    exit(1);
  }

  if (ioctl(ifinfo->fd, I_PUSH, "pfmod") < 0) {
    syslog(LOG_ERR, "ioctl(I_PUSH pfmod) error in open_if()");
    exit(1);
  }

  if (strioctl (ifinfo->fd, PFIOCSETF, -1, sizeof (pf), (char *)&pf) == -1)
      {
      syslog (LOG_ERR, "ioctl (PFIOCSETF) error in open_if()");
      exit (1);
      }
#endif

  if (ioctl(ifinfo->fd, I_FLUSH, (char *)FLUSHR) < 0) {
    syslog(LOG_ERR, "ioctl(I_FLUSH) error in open_if()");
    exit(1);
  }

  ifinfo->buf_size = 8192;  /* XXX */
  if ((ifinfo->buf = (char *)calloc(1, ifinfo->buf_size)) == NULL) {
    syslog(LOG_ERR, "calloc() error in open_if()");
    exit(1);
  }
 
  return(0);
}
#else /* not sun */


/*
 * bpf filter program which select only DHCP(BOOTP) Packet
 */
#ifdef sony_news
struct bpf_insn etherfilter[] = {
  BPF_STMT(LdHOp, 12),                    /* A <- ETHER_TYPE */
  BPF_JUMP(EQOp, ETHERTYPE_IP, 1, 10),    /* IP ? */
  BPF_STMT(LdHOp, 20),                    /* A <- IP FRAGMENT */
  BPF_STMT(AndIOp, 0x1fff),               /* A <- A & 0x1fff */
  BPF_JUMP(EQOp, 0, 1, 7),                /* OFFSET == 0 ? */
  BPF_STMT(LdBOp, 23),                    /* A <- IP_PROTO */
  BPF_JUMP(EQOp, IPPROTO_UDP, 1, 5),      /* UDP ? */
  BPF_STMT(LdxmsOp, 14),                  /* X <- IPHDR LEN */
  BPF_STMT(ILdHOp, 16),                   /* A <- UDP DSTPORT */
  BPF_JUMP(EQOp, 0, 1, 2),                /* check DSTPORT */
  BPF_STMT(RetOp, (u_int)-1),             /* return all*/
  BPF_STMT(RetOp, 0)                      /* ignore */
};
#define DSTPORTIS  9
#else /* if sony_news */
struct bpf_insn etherfilter[] = {
  BPF_STMT(BPF_LD+BPF_H+BPF_ABS, 12),                    /* A <- ETHER_TYPE */
  BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, ETHERTYPE_IP, 0, 8),   /* IP ? */
  BPF_STMT(BPF_LD+BPF_H+BPF_ABS, 20),                    /* A <- IP FRAGMENT */
  BPF_JUMP(BPF_JMP+BPF_JSET+BPF_K, 0x1fff, 6, 0),        /* OFFSET == 0 ? */
  BPF_STMT(BPF_LD+BPF_B+BPF_ABS, 23),                    /* A <- IP_PROTO */
  BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, IPPROTO_UDP, 0, 4),    /* UDP ? */
  BPF_STMT(BPF_LDX+BPF_B+BPF_MSH, 14),                   /* X <- IPHDR LEN */
  BPF_STMT(BPF_LD+BPF_H+BPF_IND, 16),                    /* A <- UDP DSTPORT */
  BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, 0, 0, 1),              /* check DSTPORT */
  BPF_STMT(BPF_RET+BPF_K, (u_int)-1),	                 /* return all*/
  BPF_STMT(BPF_RET+BPF_K, 0)			         /* ignore */
};
#define DSTPORTIS   8    /* UDP destination port variable */
#endif /* sony_news */

/********************************
 *    open the bpf and setup    *
 ********************************/
int
open_if(ifinfo)
  struct if_info *ifinfo;
{
  char dev[sizeof "/dev/pf/pfiltxx"];
#ifdef __osf__
  u_short  bits;
  struct endevp endvp;
#endif
  int n = 0;
  int immediate = 0;                     /* immediately return from select() */
  struct ifreq ifreq;
  static struct bpf_program etherprogram = {
    sizeof(etherfilter) / sizeof(struct bpf_insn),
    etherfilter
  };
#ifdef sony_news
  struct  bpf_devp  bpfdevp;
#endif /* sony_news */

  /*
   *	open /dev/bpf*  (or /dev/pfilt*)
   */
  n = 0;
  do {
#ifdef __osf__
    (void) sprintf(dev, "/dev/pf/pfilt%d", n++);
#else
    (void) sprintf(dev, "/dev/bpf%d", n++);
#endif
    ifinfo->fd = open(dev, O_RDWR);
  } while (ifinfo->fd < 0 && n < NBPFILTER);

  if (ifinfo->fd < 0) {           /* cannot open bpf */
    syslog(LOG_ERR, "can't open bpf in open_if()");
    exit(1);
  }

  /*
   * Initialize the interface information. subnet and IP address
   */
  bzero(&ifreq, sizeof(ifreq));
  if ((n = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    syslog(LOG_ERR, "socket() in open_if()");
    exit(1);
  }
  strcpy(ifreq.ifr_name, ifinfo->name);
  if (ioctl(n, SIOCGIFNETMASK, &ifreq) < 0) {
    syslog(LOG_ERR, "ioctl(SIOCGIFNETMASK) in open_if()");
    exit(1);
  }
  if ((ifinfo->subnetmask = 
       (struct in_addr *) calloc(1, sizeof(struct in_addr))) == NULL) {
    syslog(LOG_ERR, "calloc error in open_if()");
    exit(1);
  }
  ifinfo->subnetmask->s_addr =
    ((struct sockaddr_in *)&ifreq.ifr_addr)->sin_addr.s_addr;

  if (ioctl(n, SIOCGIFADDR, &ifreq) < 0) {
    syslog(LOG_ERR, "ioctl(SIOCGIFADDR) in open_if()");
    exit(1);
  }
  if ((ifinfo->ipaddr =
       (struct in_addr *) calloc(1, sizeof(struct in_addr))) == NULL) {
    syslog(LOG_ERR, "calloc in open_if()");
    exit(1);
  }
  ifinfo->ipaddr->s_addr = ((struct sockaddr_in *)&ifreq.ifr_addr)->sin_addr.s_addr;
  close(n);

#ifdef __osf__
  /*
   * make packetfilt use BPF header
   */
  bits = ENBPFHDR;
  if (ioctl(ifinfo->fd, EIOCMBIS, &bits) < 0) {
    syslog(LOG_ERR, "ioctl(EIOCMBIS) in open_if()");
    exit(1);
  }
#endif

  /*
   * set immediate mode
   */
  immediate = 1;
  if (ioctl(ifinfo->fd, BIOCIMMEDIATE, &immediate) < 0) {
    syslog(LOG_ERR, "ioctl(BIOCIMMEDIATE) in open_if()");
    exit(1);
  }

  /*
   * bind interface to bpf
   */
  strcpy(ifreq.ifr_name, ifinfo->name);
  if (ioctl(ifinfo->fd, BIOCSETIF, &ifreq) < 0) {
    syslog(LOG_ERR, "ioctl(BIOCSETIF) in open_if()");
    exit(1);
  }

  /*
   *	get buffer_length and allocate buffer
   */
  if (ioctl(ifinfo->fd, BIOCGBLEN, &ifinfo->buf_size) < 0) {
    syslog(LOG_ERR, "ioctl(BIOCGBLEN) in open_if()");
    exit(1);
  }

  if ((ifinfo->buf = calloc(1, ifinfo->buf_size)) == NULL) {
    syslog(LOG_ERR, "calloc in open_if()");
    exit(1);
  }

  /*
   * get the datalink type
   */
#ifdef __bsdi__
  if (ioctl(ifinfo->fd, BIOCGDLT, &ifinfo->htype) < 0) {
    syslog(LOG_ERR, "ioctl(BIOCGDLT) in open_if()");
    exit(1);
  }
#endif /* __bsdi__ */
#ifdef sony_news
  if (ioctl(ifinfo->fd, BIOCDEVP, &bpfdevp) < 0) {
    syslog(LOG_ERR, "ioctl(BIOCDEVP) in open_if()");
    exit(1);
  }
  ifinfo->htype = bpfdevp.bdev_type;
#endif /* sony_news */

  ifinfo->hlen = 6;
#if defined(sony_news) || defined(__FreeBSD__)
  if (ioctl(ifinfo->fd, SIOCGIFADDR, &ifreq) < 0) {
    syslog(LOG_ERR, "ioctl(SIOCGIFADDR) error in open_if()"); 
    exit(1);
  }
  bcopy(ifreq.ifr_addr.sa_data, ifinfo->haddr, 6);
#else
#ifdef __osf__
  bzero(&endvp, sizeof(endvp));
  if (ioctl(ifinfo->fd, EIOCDEVP, &endvp) < 0) {
    syslog(LOG_ERR, "ioctl(EIOCDEVP) error in open_if()"); 
    exit(1);
  }
  bcopy(endvp.end_addr, ifinfo->haddr, 6);
#else
  if (getmac(ifinfo->name, ifinfo->haddr) < 0) {
    exit(1);
  }
#endif
#endif

  /* setup filter */
  etherfilter[DSTPORTIS].k = ntohs(dhcps_port);   /* host byte order */

  /* bind filter to bpf */
  if (ioctl(ifinfo->fd, BIOCSETF, &etherprogram) < 0) {
    syslog(LOG_ERR, "ioctl(BIOCSETF) to ether in open_if()");
    exit(1);
  }

  if (ioctl(ifinfo->fd, BIOCFLUSH) < 0) {
    syslog(LOG_ERR, "ioctl(BIOCFLUSH) in open_if()");
    exit(1);
  }

  return(0);
}
#endif /* not sun */


/*
 * select and read from the interface
 */
struct if_info *
read_interfaces(iflist, n)
  struct if_info *iflist;
  int *n;
{
  struct if_info *ifp;
  int nfound = 0;                      /* used in select */
  fd_set readfds;                      /* used in select */
  static int maxfd = 0;                /* maximum number of bpf descriptors */
  static int first = 0;
  static fd_set backup;
#ifdef sun 
  int offset;
#endif

  /* set up for select() */
  if (first == 0) {
    first = 1;
    FD_ZERO(&backup);
    ifp = iflist;
    while (ifp != NULL) {
      FD_SET(ifp->fd, &backup);
      if (maxfd < ifp->fd)
	maxfd = ifp->fd;
      ifp = ifp->next;
    }
  }
  readfds = backup;

  /* block till some packet arrive */
  if ((nfound = select(maxfd + 1, &readfds, NULL, NULL, NULL)) < 0) {
    syslog(LOG_WARNING, "select() fail in read_interfaces()");
    return(NULL);
  }

  /* determine the descriptor to be read */
  ifp = iflist;
  while (ifp != NULL) {
    if (FD_ISSET(ifp->fd, &readfds)) break;
    else ifp = ifp->next;
  }
  if (ifp == NULL) {
    syslog(LOG_WARNING, "select() fail in read_interfaces()");
    return(NULL);
  }

#ifdef sun
  if (ifp == iflist) offset = 0;  /* It's normal socket */
  else offset = QWOFF; 

  if ((*n = read(ifp->fd, &ifp->buf[offset], ifp->buf_size - offset)) < 0) {
    syslog(LOG_WARNING, "read from nit failed", *n);
    return(NULL);
  }
#else /* not sun */
  if ((*n = read(ifp->fd, ifp->buf, ifp->buf_size)) < 0) {
    syslog(LOG_WARNING, "read from bpf or socket failed");
    return(NULL);
  }
#endif

  return(ifp);
}  
