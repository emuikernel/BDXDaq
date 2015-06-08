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
#include <string.h>
#include <unistd.h>
#include <syslog.h>
#include <netdb.h>
#ifdef sun
#include <stropts.h>
#endif
#include <sys/uio.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/param.h>
#include <net/if.h>
#ifndef sun
#include <net/bpf.h>
#endif
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/if_ether.h>

#include "dhcp.h"
#include "common.h"
#include "common_subr.h"


u_short dhcps_port;
u_short dhcpc_port;


/*
 * setup the service port
 */
void
set_srvport()
{
  struct servent *srv = NULL;

  if ((srv = getservbyname("dhcpc", "udp")) != NULL) {
    dhcpc_port = srv->s_port;
  } else {
    syslog(LOG_WARNING, "udp/dhcpc: unknown service -- use default port %d",
	   DHCPC_PORT);
    dhcpc_port = htons(DHCPC_PORT);
  }
  srv = getservbyname("dhcps", "udp");
  if (srv != NULL) {
    dhcps_port = srv->s_port;
  } else {
    syslog(LOG_WARNING, "udp/dhcps: unknown service -- use default port %d",
	   DHCPS_PORT);
    dhcps_port = htons(DHCPS_PORT);
  }
  return;
}


/**************************************************************
 *  pickup option from dhcp message                           *
 *                                                            *
 *  If there are many options which has the same tag,         *
 *  this function returns the first one.                      *
 *  searching sequence is :                                   *
 *                                                            *
 *    options field -> 'file' field -> 'sname' field          *
 **************************************************************/
char
*pickup_opt(msg, msglen, tag)
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


/*
 * check IP checksum
 */
int
check_ipsum(ipp)
  struct ip *ipp;
{
  u_short ripcksum;                   /* recv IP checksum */

  ripcksum = ipp->ip_sum;

  return(ripcksum == get_ipsum(ipp));
}


/*
 * return IP checksum in network byte order
 */
u_short
get_ipsum(ipp)
  struct ip *ipp;
{
  ipp->ip_sum = 0;

  return(cksum((u_short *)ipp, ipp->ip_hl * 2));
}


/*
 * check UDP checksum
 */
int
check_udpsum(ipp, udpp)
  struct ip *ipp;
  struct udphdr *udpp;
{
  u_short rudpcksum;                /* recv UDP checksum */

  if (udpp->uh_sum == 0) 
    return(TRUE);
  rudpcksum = udpp->uh_sum;

  return(rudpcksum == get_udpsum(ipp, udpp));
}


/*
 * return the UDP checksum in network byte order
 */
u_short
get_udpsum(ipp, udpp)
  struct ip *ipp;
  struct udphdr *udpp;
{
  struct ps_udph pudph;

  bzero(&pudph, sizeof(pudph));

  pudph.srcip.s_addr = ipp->ip_src.s_addr;
  pudph.dstip.s_addr = ipp->ip_dst.s_addr;
  pudph.zero = 0;
  pudph.prto = IPPROTO_UDP;
  pudph.ulen = udpp->uh_ulen;
  udpp->uh_sum = 0;

  return(udp_cksum(&pudph, (char *) udpp, ntohs(pudph.ulen)));
}


/*
 * calculate the udp check sum
 *        n is length in bytes
 */
u_short
udp_cksum(pudph, buf, n)
  struct ps_udph *pudph;
  char *buf;
  int n;
{
    u_Long sum = 0;
    u_short *tmp = NULL,
            result;
    register int i = 0;
    unsigned char pad[2];

    tmp = (u_short *) pudph;
    for (i = 0; i < 6; i++) {
      sum += *tmp++;
    }      

    tmp = (u_short *) buf;
    while (n > 1) {
      sum += *tmp++;
      n -= 2;
    }

    if (n == 1) {      /* n % 2 == 1, have to do padding */
      pad[0] = *tmp;
      pad[1] = 0;
      tmp = (u_short *) pad;
      sum += *tmp;
    }

    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    result = (u_short) ~sum;
    if (result == 0)
      result = 0xffff;

    return(result);
}


/*
 * calculate the check sum for IP suit
 */
u_short
cksum(buf, n)
  u_short *buf;
  int n;
{
    u_Long sum;
    u_short result;

    for (sum = 0; n > 0; n--) {
      sum += *buf++;
    }

    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    result = (u_short) (~sum);
    if (result == 0)
      result = 0xffff;

    return(result);
}


#ifdef sun
void
align_msg(rbuf)
  char *rbuf;
{
  rcv.ether = (struct ether_header *) rbuf;
  rcv.ip = (struct ip *) &rbuf[ETHERHL];
  if ((ntohs(rcv.ip->ip_off) & 0x1fff) == 0 &&
      ntohs(rcv.ip->ip_len) >= DFLTBOOTPLEN + UDPHL + IPHL) {
    rcv.udp = (struct udphdr *) &rbuf[ETHERHL + rcv.ip->ip_hl * WORD];
    rcv.dhcp = (struct dhcp *) &rbuf[ETHERHL + rcv.ip->ip_hl * WORD + UDPHL];
  } else {
    rcv.udp = NULL;
    rcv.dhcp = NULL;
  }

  return;
}
#else
void
align_msg(rbuf)
  char *rbuf;
{
  rbpf = (struct bpf_hdr *) rbuf;
  rcv.ether = (struct ether_header *) &rbuf[rbpf->bh_hdrlen];
  rcv.ip = (struct ip *) &rbuf[rbpf->bh_hdrlen + ETHERHL];
  rcv.udp = (struct udphdr *) &rbuf[rbpf->bh_hdrlen + ETHERHL +
				    rcv.ip->ip_hl * WORD];
  rcv.dhcp = (struct dhcp *) &rbuf[rbpf->bh_hdrlen + ETHERHL +
				   rcv.ip->ip_hl * WORD + UDPHL];

  return;
}
#endif /* not sun */


#ifdef sun
int
ether_write(fd, buf, size)
  int fd;
  char *buf;
  int size;
{
#ifndef solaris
  struct sockaddr sa;
#endif
  struct strbuf cbuf, dbuf;
  int result;

#ifdef solaris
  dbuf.maxlen = dbuf.len = size;
  dbuf.buf = buf;

  result = putmsg(fd, NULL, &dbuf, 0);
  if (result != 0)
    return (-1);
#else
  bzero(&sa, sizeof(sa));
  bzero(&cbuf, sizeof(cbuf));
  bzero(&dbuf, sizeof(dbuf));
  sa.sa_family = AF_UNSPEC;
  bcopy(buf, sa.sa_data, ETHERHL);

  cbuf.maxlen = cbuf.len = sizeof(sa);
  cbuf.buf = (char *)&sa;

  dbuf.maxlen = dbuf.len = size - ETHERHL;
  dbuf.buf = &buf[ETHERHL];

  result = putmsg(fd, &cbuf, &dbuf, 0);
  if (result != 0)
    return (-1);
#endif
  return (size);
}


int
ether_writev(fd, bufvec, nvec)
  int fd;
  struct iovec *bufvec;
  int nvec;
{
  int i;
  int len = 0;
  int result = 0;
  char *buf;

  for (i = 0; i < nvec; i++) {
    len += bufvec[i].iov_len;
  }
  if ((buf = (char *) calloc(1, len)) == NULL) {
    syslog(LOG_WARNING, "calloc error in ether_writev(): %m");
    return(-1);
  }
  len = 0;
  for (i = 0; i < nvec; i++) {
    bcopy(bufvec[i].iov_base, &buf[len], bufvec[i].iov_len);
    len += bufvec[i].iov_len;
  }

  result = ether_write(fd, buf, len);
  free(buf);

  return(result);
}
#endif /* sun */
