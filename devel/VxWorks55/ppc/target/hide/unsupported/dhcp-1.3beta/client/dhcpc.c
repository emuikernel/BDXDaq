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
#include <errno.h>
#include <syslog.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#ifndef sun
#include <net/bpf.h>
#endif
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/if_ether.h>
#include <sys/param.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <arpa/inet.h>

#include "dhcp.h"
#include "config.h"
#include "common.h"
#include "common_subr.h"
#include "dhcpc_subr.h"

#ifdef sony_news
typedef int sigset_t;
#endif

#define	SLEEP_RANDOM(timer) (((timer - 1) * 1000000) + (random() % 2000000))
#define REQUEST_RETRANS   4

/*
 * some global value definitions
 */
unsigned char 	magic_c[MAGIC_LEN] = RFC1048_MAGIC;
struct buffer 	sbuf;
int curr_state, prev_state;
int interrupt, interrupt2;
time_t init_epoch;
time_t send_epoch;
struct dhcp_reqspec reqspec;
struct if_info intface;
struct dhcp_param *param_list;

int (*fsm[MAX_STATES])();

int (*handle_param[MAXTAGNUM])() = {
  NULL,           /* PAD */
  handle_ip,      /* SUBNET_MASK */
  handle_num,     /* TIME_OFFSET */
  handle_ips,     /* ROUTER */
  handle_ips,     /* TIME_SERVER */
  handle_ips,     /* NAME_SERVER */
  handle_ips,     /* DNS_SERVER */
  handle_ips,     /* LOG_SERVER */
  handle_ips,     /* COOKIE_SERVER */
  handle_ips,     /* LPR_SERVER */
  handle_ips,     /* IMPRESS_SERVER */
  handle_ips,     /* RLS_SERVER */
  handle_str,     /* HOSTNAME */
  handle_num,     /* BOOTSIZE */
  handle_str,     /* MERIT_DUMP */
  handle_str,     /* DNS_DOMAIN */
  handle_ip,      /* SWAP_SERVER */
  handle_str,     /* ROOT_PATH */
  handle_str,     /* EXTENSIONS_PATH */
  handle_bool,    /* IP_FORWARD */
  handle_bool,    /* NONLOCAL_SRCROUTE */
  handle_ippairs, /* POLICY_FILTER */
  handle_num,     /* MAX_DGRAM_SIZE */
  handle_num,     /* DEFAULT_IP_TTL */
  handle_num,     /* MTU_AGING_TIMEOUT */
  handle_nums,    /* MTU_PLATEAU_TABLE */
  handle_num,     /* IF_MTU */
  handle_bool,    /* ALL_SUBNET_LOCAL */
  handle_ip,      /* BRDCAST_ADDR */
  handle_bool,    /* MASK_DISCOVER */
  handle_bool,    /* MASK_SUPPLIER */
  handle_bool,    /* ROUTER_DISCOVER */
  handle_ip,      /* ROUTER_SOLICIT */
  handle_ippairs, /* STATIC_ROUTE */
  handle_bool,    /* TRAILER */
  handle_num,     /* ARP_CACHE_TIMEOUT */
  handle_bool,    /* ETHER_ENCAP */
  handle_num,     /* DEFAULT_TCP_TTL */
  handle_num,     /* KEEPALIVE_INTER */
  handle_bool,    /* KEEPALIVE_GARBA */
  handle_str,     /* NIS_DOMAIN */
  handle_ips,     /* NIS_SERVER */
  handle_ips,     /* NTP_SERVER */
  NULL,           /* VENDOR_SPEC */      /* XXX not implemented */
  handle_ips,     /* NBN_SERVER */
  handle_ips,     /* NBDD_SERVER */
  handle_num,     /* NB_NODETYPE */
  handle_str,     /* NB_SCOPE */
  handle_ips,     /* XFONT_SERVER */
  handle_ips,     /* XDISPLAY_MANAGER */
  NULL,           /* REQUEST_IPADDR */
  handle_num,     /* LEASE_TIME */
  NULL,           /* OPT_OVERLOAD */
  NULL,           /* DHCP_MSGTYPE */
  handle_ip,      /* SERVER_ID */
  NULL,           /* REQ_LIST */
  handle_str,     /* DHCP_ERRMSG */
  NULL,           /* DHCP_MAXMSGSIZE */
  handle_num,     /* DHCP_T1 */
  handle_num,     /* DHCP_T2  */
  NULL,           /* CLASS_ID */
  NULL,           /* CLIENT_ID */
  NULL,
  NULL,
  handle_str,     /* NISP_DOMAIN */
  handle_ips,     /* NISP_SERVER */
  NULL,           /* TFTP_SERVERNAME */
  NULL,           /* BOOTFILE */
  handle_ips,     /* MOBILEIP_HA */
  handle_ips,     /* SMTP_SERVER */
  handle_ips,     /* POP3_SERVER */
  handle_ips,     /* NNTP_SERVER */
  handle_ips,     /* DFLT_WWW_SERVER */
  handle_ips,     /* DFLT_FINGER_SERVER */
  handle_ips,     /* DFLT_IRC_SERVER */
  handle_ips,     /* STREETTALK_SERVER */
  handle_ips      /* STDA_SERVER */
};

/*
 * prototype definition
 */
int check_wait_offer();
int check_requesting();
Long generate_xid();


/*
 * select offer which has longest lease
 */
struct dhcp_param *
select_longest(plist)
  struct dhcp_param *plist;
{
  struct dhcp_param *result, *paramp;

  if (plist == NULL)
    return(NULL);

  result = paramp = plist;
  while (paramp != NULL) {
    if (paramp->lease_duration > result->lease_duration)
      result = paramp;
    paramp = paramp->next;
  }

  return(result);
}


void
use_parameter(paramp)
  struct dhcp_param *paramp;
{
  struct in_addr addr, mask, brdaddr;

  bzero(&addr, sizeof(struct in_addr));
  bzero(&mask, sizeof(struct in_addr));
  bzero(&brdaddr, sizeof(struct in_addr));

  addr.s_addr = paramp->yiaddr.s_addr;
  if (paramp->subnet_mask != NULL) {
    mask.s_addr = paramp->subnet_mask->s_addr;
  } else {
    mask.s_addr = 0;
  }
  if (paramp->brdcast_addr != NULL) {
    brdaddr.s_addr = paramp->brdcast_addr->s_addr;
  } else {
    brdaddr.s_addr = 0;
  }

#ifndef DEBUG
  if (config_if(&intface, &addr, ((mask.s_addr != 0) ? &mask : NULL),
		((brdaddr.s_addr != 0) ? &brdaddr : NULL)) != 1) {
    set_route(paramp);
  }
#endif

  return;
}


/*
 * check received dhcp message for wait_offer()
 * return 1 if there is no problem.
 */
int
check_wait_offer()
{
  char *option = NULL;
  u_short ripcksum = 0,
          rudpcksum = 0;
  struct ps_udph rpudph;

  /* message size has been already checked */
  if (rcv.udp == NULL || rcv.dhcp == NULL)
    return(0);

  bzero(&rpudph, sizeof(rpudph));

  ripcksum = rcv.ip->ip_sum;
  rcv.ip->ip_sum = 0;
  rudpcksum = rcv.udp->uh_sum;
  rcv.udp->uh_sum = 0;
  rpudph.zero = 0;
  rpudph.prto = IPPROTO_UDP;
  rpudph.srcip.s_addr = rcv.ip->ip_src.s_addr;
  rpudph.dstip.s_addr = rcv.ip->ip_dst.s_addr;
  rpudph.ulen = rcv.udp->uh_ulen;

  /*
   * split conditions into pieces for debugging
   */
#ifndef sun
  if (ntohs(rcv.ip->ip_len) < DFLTBOOTPLEN + UDPHL + IPHL)
    return(0);
  if (ntohs(rcv.udp->uh_ulen) < DFLTBOOTPLEN + UDPHL)
    return(0);
#else
  if (rcv.udp->uh_dport != dhcpc_port)
    return(0);
#endif 
  if (ripcksum != cksum((u_short *) rcv.ip, rcv.ip->ip_hl * 2))
    return(0);
  if (rcv.udp->uh_sum != 0 &&
      rudpcksum != udp_cksum(&rpudph, (char *) rcv.udp, ntohs(rpudph.ulen)))
    return(0);
  if (rcv.dhcp->op != BOOTREPLY || rcv.dhcp->xid != snd.dhcp->xid)
    return(0);
  if (bcmp(rcv.dhcp->options, magic_c, MAGIC_LEN) != 0)
    return(0);
  if ((option = (char *)
       pickup_opt(rcv.dhcp, DHCPLEN(rcv.udp), DHCP_MSGTYPE)) == NULL ||
      *OPTBODY(option) != DHCPOFFER)
    return(0);

  return(1);
}


/*
 * check received dhcp message for requesting()
 * return 1 if there is no problem.
 */
int
check_requesting()
{
  u_short ripcksum = 0,
          rudpcksum = 0;
  struct ps_udph rpudph;

  /* message size has been already checked */
  if (rcv.udp == NULL || rcv.dhcp == NULL)
    return(0);

  bzero(&rpudph, sizeof(rpudph));

  ripcksum = rcv.ip->ip_sum;
  rcv.ip->ip_sum = 0;
  rudpcksum = rcv.udp->uh_sum;
  rcv.udp->uh_sum = 0;
  rpudph.zero = 0;
  rpudph.prto = IPPROTO_UDP;
  rpudph.srcip.s_addr = rcv.ip->ip_src.s_addr;
  rpudph.dstip.s_addr = rcv.ip->ip_dst.s_addr;
  rpudph.ulen = rcv.udp->uh_ulen;

  /*
   * split conditions into pieces for debugging
   */
#ifndef sun
  if (ntohs(rcv.ip->ip_len) < DFLTBOOTPLEN + UDPHL + IPHL)
    return(0);
  if (ntohs(rcv.udp->uh_ulen) < DFLTBOOTPLEN + UDPHL)
    return(0);
#else
  if (rcv.udp->uh_dport != dhcpc_port)
    return(0);
#endif 
  if (ripcksum != cksum((u_short *) rcv.ip, rcv.ip->ip_hl * 2))
    return(0);
  if (rcv.udp->uh_sum != 0 &&
      rudpcksum != udp_cksum(&rpudph, (char *) rcv.udp, ntohs(rpudph.ulen)))
    return(0);
  if (rcv.dhcp->op != BOOTREPLY || rcv.dhcp->xid != snd.dhcp->xid)
    return(0);
  if (bcmp(rcv.dhcp->options, magic_c, MAGIC_LEN) != 0)
    return(0);

  return(1);
}


/*
 * signal handler for SIGUSR1
 */
void
go_verify()
{
  interrupt2 = 1;
  return;
}


/*
 * signal handler for SIGUSR2
 *
 */
void
release()
{
  char errmsg[255];
  int fd;
  int zero;
  struct dhcp_reqspec tmp_reqspec;
  sigset_t newsigmask, oldsigmask;

  bzero(&tmp_reqspec, sizeof(tmp_reqspec));
  bzero(errmsg, sizeof(errmsg));
  bzero(&newsigmask, sizeof(newsigmask));
  bzero(&oldsigmask, sizeof(oldsigmask));

  newsigmask = sigmask(SIGUSR1) | sigmask(SIGALRM);
  oldsigmask = sigblock(newsigmask); /* begin critical */

  switch (curr_state) {
  case BOUND:
  case RENEWING:
  case REBINDING:
  case VERIFY:
  case VERIFYING:
    if (param_list != NULL) {
      set_relinfo(&tmp_reqspec, param_list, errmsg);
      dhcp_release(&tmp_reqspec);
      if ((fd = open(DHCP_CACHE, O_WRONLY, 0444)) >= 0) {
	zero = 0;
	write(fd, (void *) &zero, sizeof(zero));
	close(fd);
      }
    }
    break;
  default:
    break;
  }
  sigsetmask(oldsigmask); /* end critical */

  exit(0);
}


/*
 * signal handler for SIGTERM and SIGINT
 */
void
finish()
{
  down_if(&intface);

  exit(0);
}


/*
 * retransmission for generic use
 */
int
gen_retransmit()
{
  time_t curr_epoch = 0;

  interrupt = 1;
  if (time(&curr_epoch) == -1)
    return(-1);

  if (curr_state != REQUESTING)
    snd.dhcp->secs = htons(curr_epoch - init_epoch);
  snd.dhcp->xid = generate_xid();
  snd.udp->uh_sum = 0;
  snd.udp->uh_sum = udp_cksum(&spudph, (char *) snd.udp, ntohs(spudph.ulen));
  if (ether_write(dhcpif.fd, &sbuf.buf[QWOFF], sbuf.size) < sbuf.size)
    return(-2);

  return(0);
}


/*
 * retransmission in WAIT_OFFER state
 */
void
retrans_wait_offer()
{
  if (curr_state != WAIT_OFFER)
    return;

  switch (gen_retransmit()) {
  case 0:
    errno = 0;
    syslog(LOG_NOTICE, "retransmit DHCPDISCOVER");
    break;
  case -1:
    syslog(LOG_WARNING, "time() error in retrans_wait_offer(): %m");
    break;
  case -2:
    syslog(LOG_WARNING, "Can't retransmit DHCPDISCOVER");
    break;
  default:
    break;
  }

  return;
}


/*
 * SIGALRM handler in SELECTING state
 */
void
alarm_selecting()
{
  if (curr_state == SELECTING) {
    interrupt = 1;
  }
  return;
}


/*
 * retransmission in REQUESTING state
 */
void
retrans_requesting()
{
  time_t curr_epoch = 0;

  if (curr_state != REQUESTING)
    return;

  interrupt = 1;
  if (time(&curr_epoch) == -1) {
    syslog(LOG_WARNING, "time() error in retrans_requesting(): %m");
    return;
  }

  snd.udp->uh_sum = 0;
  snd.udp->uh_sum = udp_cksum(&spudph, (char *) snd.udp, ntohs(spudph.ulen));
  if (ether_write(dhcpif.fd, &sbuf.buf[QWOFF], sbuf.size) < sbuf.size) {
    syslog(LOG_WARNING, "Can't retransmit DHCPREQUEST(REQUESTING)");
    return;
  }
  errno = 0;
  syslog(LOG_NOTICE, "retransmit DHCPREQUEST(REQUESTING)");

  return;
}


/*
 * retransmission in RENEWING state
 */
void
retrans_renewing()
{
  if (curr_state != RENEWING)
    return;

  switch (gen_retransmit()) {
  case 0:
    errno = 0;
    syslog(LOG_NOTICE, "retransmit DHCPREQUEST(RENEWING)");
    break;
  case -1:
    syslog(LOG_WARNING, "time() error in retrans_renewing(): %m");
    break;
  case -2:
    syslog(LOG_WARNING, "Can't retransmit DHCPREQUEST(RENEWING)");
    break;
  default:
    break;
  }

  return;
}


/*
 * retransmission in REBINDING state
 */
void
retrans_rebinding()
{
  if (curr_state != REBINDING)
    return;

  switch (gen_retransmit()) {
  case 0:
    errno = 0;
    syslog(LOG_NOTICE, "retransmit DHCPREQUEST(REBINDING)");
    break;
  case -1:
    syslog(LOG_WARNING, "time() error in retrans_rebinding(): %m");
    break;
  case -2:
    syslog(LOG_WARNING, "Can't retransmit DHCPREQUEST(REBINDING)");
    break;
  default:
    break;
  }

  return;
}


/*
 * retransmission in REBOOTING state / VERIFYING state
 */
void
retrans_reboot_verify()
{
  if (curr_state != REBOOTING && curr_state != VERIFYING)
    return;

  switch (gen_retransmit()) {
  case 0:
    errno = 0;
    syslog(LOG_NOTICE, "retransmit DHCPREQUEST(%s)",
	   (curr_state == REBOOTING) ? "REBOOTING" : "VERIFYING");
    break;
  case -1:
    syslog(LOG_WARNING, "time() error in retrans_reboot_verify(): %m");
    break;
  case -2:
    syslog(LOG_WARNING, "Can't retransmit DHCPREQUEST(%s)",
	   (curr_state == REBOOTING) ? "REBOOTING" : "VERIFYING");
    break;
  default:
    break;
  }

  return;
}


/*
 * INIT state
 */
int
init()
{
  bzero(sbuf.buf, sbuf.size + QWOFF);

  alarm(0);
  make_discover();

  /* sleep to avoid congestion */
  usleep(1000000 + (random() % ((INIT_WAITING - 1) * 1000000)));

  reset_if(&intface);

  snd.dhcp->secs = 0;
  snd.udp->uh_sum = 0;
  snd.udp->uh_sum = udp_cksum(&spudph, (char *) snd.udp, ntohs(spudph.ulen));
  if (ether_write(dhcpif.fd, &sbuf.buf[QWOFF], sbuf.size) < sbuf.size) {
    syslog(LOG_WARNING, "Can't send DHCPDISCOVER: %m");
    return(-1);
  }

  errno = 0;
  syslog(LOG_INFO, "send DHCPDISCOVER");

  return(WAIT_OFFER);                  /* Next state is WAIT_OFFER */
}


/*
 * WAIT_OFFER state
 */
int
wait_offer()
{
  int n = 0;
  int arpans = 0;
  char *rbufp = NULL;
  char errmsg[255];
  static int timer = 0;
  static int retry = 0;
  fd_set readfds;
  struct dhcp_param *paramp = NULL;

  bzero(errmsg, sizeof(errmsg));

  if (prev_state == INIT) {
    timer = FIRSTTIMER;
    retry = 0;
    if (time(&init_epoch) == -1) {
      syslog(LOG_WARNING, "time() error in wait_offer(): %m");
      return(-1);
    }

    /* garbage collection to make sure */
    while (param_list != NULL) {
      paramp = param_list;
      param_list = param_list->next;
      clean_param(paramp);
      free(paramp);
    }
    param_list = NULL;
  }
  else if (retry == DISCOVER_RETRANS) {
    syslog(LOG_NOTICE, "Client can get no DHCPOFFER");
    return(-1);
  }
  else if (timer < MAXTIMER) {
    timer *= 2;
  }

  alarm(0);
  ualarm(SLEEP_RANDOM(timer), 0);
  if (retry < DISCOVER_RETRANS)
    signal(SIGALRM, (void *) retrans_wait_offer);

  interrupt = 0;
  while (interrupt == 0) {
    FD_ZERO(&readfds);
    FD_SET(dhcpif.fd, &readfds);
    if (select(dhcpif.fd + 1, &readfds, NULL, NULL, NULL) < 0) {
      if (errno == EINTR) {
	continue;
      }
      else {
	syslog(LOG_WARNING, "select() error in wait_offer(): %m");
	return(-1);
      }
    }
#ifndef sun
    else if ((n = read(dhcpif.fd, dhcpif.rbuf, dhcpif.rbufsize)) < 0) {
      syslog(LOG_WARNING, "read() error in wait_offer(): %m");
      return(-1);
    }
#else
    else if ((n = read(dhcpif.fd, &dhcpif.rbuf[QWOFF],
		       dhcpif.rbufsize - QWOFF)) < 0) {
      syslog(LOG_WARNING, "read() error in wait_offer(): %m");
      return(-1);
    }
#endif

#ifndef sun
    rbufp = dhcpif.rbuf;
#else
    rbufp = &dhcpif.rbuf[QWOFF];
#endif

    while (n > 0) {
      align_msg(rbufp);         /* align message within buffer */

      if (check_wait_offer()) {

	/* allocate memory for dhcp_param */
	if ((paramp = (struct dhcp_param *)
	     calloc(1, sizeof(struct dhcp_param))) == NULL) {
	  continue;
	}

	/* convert rdhcp to dhcp_param */
	if (dhcp_msgtoparam(rcv.dhcp, DHCPLEN(rcv.udp), paramp) == OK) {
	  if (paramp->server_id.s_addr != 0 &&
	      (arpans = arp_check(&paramp->yiaddr, &arpif)) == OK) {
	    paramp->lease_origin = init_epoch;
	    paramp->next = param_list;
	    param_list = paramp;
	    return(SELECTING);     /* Next state is SELECTING */
	  }
	  else {
	    clean_param(paramp);
	    free(paramp);
	    paramp = NULL;
	  }
	}
      }
#ifndef sun
      rbufp += BPF_WORDALIGN(rbpf->bh_hdrlen + rbpf->bh_caplen);
      n -= (char *) rbufp - (char *) rbpf;
#else /* sun just drops remains */
      n = 0;
#endif
    }
  }

  retry++;
  return(WAIT_OFFER);
}


/*
 * SELECTING state
 */
int
selecting()
{
  int n = 0;
  int arpans = 0;
  char *rbufp = NULL;
  char errmsg[255];
  fd_set readfds;
  struct dhcp_param *paramp = NULL, *selected_paramp = NULL;
  struct dhcp_reqspec tmp_reqspec;

  bzero(errmsg, sizeof(errmsg));
  bzero(&tmp_reqspec, sizeof(tmp_reqspec));

  alarm(0);
  if (reqspec.select_offer == DHCPOFFER_SELECT_FIRST)
    goto go_requesting;

  alarm(reqspec.waitsecs);
  signal(SIGALRM, (void *) alarm_selecting);

  interrupt = 0;
  while (interrupt == 0) {
    FD_ZERO(&readfds);
    FD_SET(dhcpif.fd, &readfds);
    if (select(dhcpif.fd + 1, &readfds, NULL, NULL, NULL) < 0) {
      if (errno == EINTR) {
	continue;
      }
      else {
	syslog(LOG_WARNING, "select() error in selecting(): %m");
	return(INIT);
      }
    }
#ifndef sun
    else if ((n = read(dhcpif.fd, dhcpif.rbuf, dhcpif.rbufsize)) < 0) {
      syslog(LOG_WARNING, "read() error in selecting(): %m");
      return(INIT);
    }
#else
    else if ((n = read(dhcpif.fd, &dhcpif.rbuf[QWOFF],
		       dhcpif.rbufsize - QWOFF)) < 0) {
      syslog(LOG_WARNING, "read() error in selecting(): %m");
      return(INIT);
    }
#endif

#ifndef sun
    rbufp = dhcpif.rbuf;
#else
    rbufp = &dhcpif.rbuf[QWOFF];
#endif
    while (n > 0) {
      align_msg(rbufp);         /* align message within buffer */

      if (check_wait_offer()) {

	/* allocate memory for dhcp_param */
	if ((paramp = (struct dhcp_param *)
	     calloc(1, sizeof(struct dhcp_param))) == NULL) {
	  continue;
	}

	/* convert rdhcp to dhcp_param */
	if (dhcp_msgtoparam(rcv.dhcp, DHCPLEN(rcv.udp), paramp) == OK) {
	  if (paramp->server_id.s_addr != 0 &&
	      (arpans = arp_check(&paramp->yiaddr, &arpif)) == OK) {
	    paramp->lease_origin = init_epoch;
	    paramp->next = param_list;
	    param_list = paramp;
	  }
	  else {
	    clean_param(paramp);
	    free(paramp);
	    paramp = NULL;
	  }
	}
      }

#ifndef sun
      rbufp += BPF_WORDALIGN(rbpf->bh_hdrlen + rbpf->bh_caplen);
      n -= (char *) rbufp - (char *) rbpf;
#else /* sun just droppping remains */
      n = 0;
#endif
    }
  }

  selected_paramp = select_longest(param_list);

  while (param_list != NULL) {
    paramp = param_list;
    if (paramp == selected_paramp) {
      param_list = paramp->next;
      paramp->next = NULL;
    }
    else {
      param_list = param_list->next;
      clean_param(paramp);
      free(paramp);
      paramp = NULL;
    }
  }
  param_list = selected_paramp;


 go_requesting:

  if (make_request(param_list, REQUESTING) < 0) {
    syslog(LOG_WARNING, "non recoverable error in make_request(). Go to INIT state.");
    return(INIT);
  }
  snd.udp->uh_sum = 0;
  snd.udp->uh_sum = udp_cksum(&spudph, (char *) snd.udp, ntohs(spudph.ulen));
  if (ether_write(dhcpif.fd, &sbuf.buf[QWOFF], sbuf.size) < sbuf.size) {
    syslog(LOG_WARNING, "Can't send DHCPREQUEST: %m");
    return(INIT);
  }

  return(REQUESTING);
}


/*
 * REQUESTING state
 */
int
requesting()
{
  int n = 0;
  int arpans = 0;
  char *option = NULL;
  char *rbufp = NULL;
  char errmsg[255];
  static int timer = 0;
  static int retry = 0;
  fd_set readfds;
  struct dhcp_param tmpparam;
  struct dhcp_reqspec tmp_reqspec;

  bzero(errmsg, sizeof(errmsg));
  bzero(&tmpparam, sizeof(tmpparam));
  bzero(&tmp_reqspec, sizeof(tmp_reqspec));

  if (prev_state == SELECTING) {
    timer = FIRSTTIMER;
    retry = 0;
  }
  else if (retry == REQUEST_RETRANS) {
    syslog(LOG_NOTICE, "Client can get neither DHCPACK nor DHCPNAK");
    return(INIT);           /* Next state is INIT */
  }
  else if (timer < MAXTIMER) {
    timer *= 2;
  }

  alarm(0);
  ualarm(SLEEP_RANDOM(timer), 0);
  if (retry < REQUEST_RETRANS)
    signal(SIGALRM, (void *) retrans_requesting);

  interrupt = 0;
  while (interrupt == 0) {
    FD_ZERO(&readfds);
    FD_SET(dhcpif.fd, &readfds);
    if (select(dhcpif.fd + 1, &readfds, NULL, NULL, NULL) < 0) {
      if (errno == EINTR) {
	continue;
      }
      else {
	syslog(LOG_WARNING, "select() error in requesting(): %m");
	return(INIT);
      }
    }
#ifndef sun
    else if ((n = read(dhcpif.fd, dhcpif.rbuf, dhcpif.rbufsize)) < 0) {
      syslog(LOG_WARNING, "read() error in requesting(): %m");
      return(INIT);
    }
#else
    else if ((n = read(dhcpif.fd, &dhcpif.rbuf[QWOFF],
		       dhcpif.rbufsize - QWOFF)) < 0) {
      syslog(LOG_WARNING, "read() error in requesting(): %m");
      return(-1);
    }
#endif

#ifndef sun
    rbufp = dhcpif.rbuf;
#else
    rbufp = &dhcpif.rbuf[QWOFF];
#endif

    while (n > 0) {
      align_msg(rbufp);         /* align message within buffer */
      bzero(&tmpparam, sizeof(tmpparam));

      if (check_requesting() &&
	  (option = (char *)
	   pickup_opt(rcv.dhcp, DHCPLEN(rcv.udp), DHCP_MSGTYPE)) != NULL) {

	if (*OPTBODY(option) == DHCPNAK) {
	  syslog(LOG_NOTICE, "Got DHCPNAK in requesting()");
	  if ((option = (char *)
	      pickup_opt(rcv.dhcp, DHCPLEN(rcv.udp), DHCP_ERRMSG)) != NULL &&
	      nvttostr(OPTBODY(option), errmsg, (int) OPTLEN(option)) == 0) {
	    syslog(LOG_NOTICE, "DHCPNAK contains the error message \"%s\"",
		   errmsg);
	  }
	  clean_param(param_list);
	  free(param_list);
	  param_list = NULL;
	  return(INIT);               /* Next state is INIT */
	}
	else if (*OPTBODY(option) == DHCPACK &&
		 dhcp_msgtoparam(rcv.dhcp, DHCPLEN(rcv.udp),
				 &tmpparam) == OK) {
	  if ((arpans = arp_check(&tmpparam.yiaddr, &arpif)) == OK) {
	    merge_param(param_list, &tmpparam);
	    *param_list = tmpparam;
	    param_list->lease_origin = init_epoch;
	    param_list->next = NULL;
	    syslog(LOG_INFO, "Got DHCPACK (IP = %s, duration = %d secs)",
		   inet_ntoa(param_list->yiaddr), param_list->lease_duration);
	    arp_reply(&param_list->yiaddr, &arpif);
	    return(BOUND);
	  }
	  else {
	    /* make dhcp_declinfo and send DHCPDECLINE */
	    set_declinfo(&tmp_reqspec, param_list, errmsg, arpans);
	    dhcp_decline(&tmp_reqspec);
	    clean_param(param_list);
	    free(param_list);
	    param_list = NULL;
	    syslog(LOG_NOTICE,
		   "Got non-preferable DHCPACK, so go into INIT state");
	    return(INIT);
	  }
	}
      }

#ifndef sun
      rbufp += BPF_WORDALIGN(rbpf->bh_hdrlen + rbpf->bh_caplen);
      n -= (char *) rbufp - (char *) rbpf;
#else /* sun just droppping remains */
      n = 0;
#endif
    }
  }

  retry++;
  return(REQUESTING);
}


/*
 * BOUND state
 */
int
bound()
{
  int fd = 0;
  int length = 0, n = 0;
  char *bufp = NULL;
  time_t curr_epoch = 0;
  bzero(sbuf.buf, sbuf.size + QWOFF);

  if ((fd = open(DHCP_CACHE, O_WRONLY | O_CREAT | O_TRUNC, 0444)) >= 0) {
    length = ntohs(rcv.udp->uh_ulen) + rcv.ip->ip_hl * WORD + ETHERHL;
    write(fd, (void *) &param_list->lease_origin,
	  sizeof(param_list->lease_origin));
    write(fd, (void *) &length, sizeof(length));

    bufp = (char *) rcv.ether;
    while (length > 0) {
      n = write(fd, (void *) bufp, length);
      length -= n;
      bufp += n;
    }

    close(fd);
  }

  /*
   * use allocated info (user MUST define)
   */
  use_parameter(param_list);

  while (param_list->lease_duration == ~0) {
    sigpause(0);
  }

  if (time(&curr_epoch) == -1) {
    syslog(LOG_WARNING, "time() error in bound(): %m");
    return(INIT);
  }
  if (param_list->lease_origin + param_list->dhcp_t2 <= curr_epoch &&
      curr_epoch < param_list->lease_origin + param_list->lease_duration) {
    syslog(LOG_INFO, "Go to REBINDING state");
    make_request(param_list, REBINDING);
    snd.udp->uh_sum = 0;
    snd.udp->uh_sum = udp_cksum(&spudph, (char *) snd.udp, ntohs(spudph.ulen));
    if (ether_write(dhcpif.fd, &sbuf.buf[QWOFF], sbuf.size) < sbuf.size)
      syslog(LOG_WARNING, "Can't send DHCPREQUEST(REBINDING): %m");
    return(REBINDING);
  }
  if (param_list->lease_origin + param_list->dhcp_t1 <= curr_epoch &&
      curr_epoch < param_list->lease_origin + param_list->dhcp_t2) {
    syslog(LOG_INFO, "Go to RENEWING state");
    make_request(param_list, RENEWING);
    if (send_unicast(&param_list->server_id, snd.dhcp) < 0)
      syslog(LOG_WARNING, "Can't send DHCPREQUEST(RENEWING)");
    return(RENEWING);
  }

  interrupt2 = 0;
  alarm(0);
  alarm(param_list->lease_origin + param_list->dhcp_t1 - curr_epoch);
  sigpause(0);
  if (interrupt2 == 1)
    return(VERIFY);

  if (time(&init_epoch) == -1) {
    syslog(LOG_WARNING, "time() error in bound(): %m");
    return(INIT);
  }
  if (make_request(param_list, RENEWING) < 0) {
    syslog(LOG_WARNING, "non recoverable error in make_request(). Go to INIT state.");
    return(INIT);
  }
  snd.dhcp->secs = 0;
  send_epoch = init_epoch;
  if (send_unicast(&param_list->server_id, snd.dhcp) < 0) {
    syslog(LOG_WARNING, "Can't send DHCPREQUEST(RENEWING)");
    return(INIT);
  }

  errno = 0;
  syslog(LOG_INFO, "send DHCPREQUEST(RENEWING)");

  return(RENEWING);                  /* Next state is RENEWING */
}


/*
 * RENEWING state
 */
int
renewing()
{
  int n = 0;
  char *option = NULL;
  char *rbufp = NULL;
  char errmsg[255];
  time_t curr_epoch = 0;
  static int timer = 0;
  fd_set readfds;
  sigset_t newsigmask, oldsigmask;
  struct dhcp_param tmpparam;

  bzero(errmsg, sizeof(errmsg));
  bzero(&newsigmask, sizeof(newsigmask));
  bzero(&oldsigmask, sizeof(oldsigmask));
  bzero(&tmpparam, sizeof(tmpparam));

  if (prev_state != RENEWING) {
    if (time(&init_epoch) == -1) {
      syslog(LOG_WARNING, "time() error in renewing(): %m");
      return(INIT);
    }
    send_epoch = init_epoch;
  }
  if (time(&curr_epoch) == -1) {
    syslog(LOG_WARNING, "time() error in renewing(): %m");
    return(INIT);
  }
  if (param_list->lease_origin + param_list->dhcp_t2 <= curr_epoch) {
    if (make_request(param_list, REBINDING) < 0) {
      syslog(LOG_WARNING, "non recoverable error in make_request(). Go to INIT state.");
      return(INIT);
    }
    snd.udp->uh_sum = 0;
    snd.udp->uh_sum = udp_cksum(&spudph, (char *) snd.udp, ntohs(spudph.ulen));
    if (time(&send_epoch) == -1) {
      syslog(LOG_WARNING, "time() error in renewing(): %m");
      return(INIT);
    }
    if (ether_write(dhcpif.fd, &sbuf.buf[QWOFF], sbuf.size) < sbuf.size) {
      syslog(LOG_WARNING, "Can't send DHCPREQUEST(REBINDING): %m");
      return(INIT);
    }
    return(REBINDING);
  }

  timer = param_list->lease_origin + param_list->dhcp_t2 - curr_epoch;
  if (timer > 60) {
    timer /= 2;
  }
  alarm(0);
  alarm(timer);
  signal(SIGALRM, (void *) retrans_renewing);

  interrupt = 0;
  interrupt2 = 0;
  while (interrupt == 0 && interrupt2 == 0) {
    FD_ZERO(&readfds);
    FD_SET(dhcpif.fd, &readfds);
    if (select(dhcpif.fd + 1, &readfds, NULL, NULL, NULL) < 0) {
      if (errno == EINTR) {
	continue;
      }
      else {
	syslog(LOG_WARNING, "select() error in renewing(): %m");
	return(INIT);
      }
    }
#ifndef sun
    else if ((n = read(dhcpif.fd, dhcpif.rbuf, dhcpif.rbufsize)) < 0) {
      syslog(LOG_WARNING, "read() error in renewing(): %m");
      return(INIT);
    }
#else
    else if ((n = read(dhcpif.fd, &dhcpif.rbuf[QWOFF],
		       dhcpif.rbufsize - QWOFF)) < 0) {
      syslog(LOG_WARNING, "read() error in renewing(): %m");
      return(INIT);
    }
#endif

#ifndef sun
    rbufp = dhcpif.rbuf;
#else
    rbufp = &dhcpif.rbuf[QWOFF];
#endif
    while (n > 0) {
      align_msg(rbufp);         /* align message within buffer */
      bzero(&tmpparam, sizeof(tmpparam));

      if (check_requesting() &&
	  (option = (char *)
	   pickup_opt(rcv.dhcp, DHCPLEN(rcv.udp), DHCP_MSGTYPE)) != NULL) {

	if (*OPTBODY(option) == DHCPNAK) {
	  syslog(LOG_NOTICE, "Got DHCPNAK in renewing()");
	  if ((option = (char *)
	      pickup_opt(rcv.dhcp, DHCPLEN(rcv.udp), DHCP_ERRMSG)) != NULL &&
	      nvttostr(OPTBODY(option), errmsg, (int) OPTLEN(option)) == 0) {
	    syslog(LOG_NOTICE, "DHCPNAK contains the error message \"%s\"",
		   errmsg);
	  }

	  newsigmask = sigmask(SIGUSR1) | sigmask(SIGUSR2);
	  oldsigmask = sigblock(newsigmask); /* begin critical */
	  clean_param(param_list);
	  free(param_list);
	  param_list = NULL;
	  sigsetmask(oldsigmask); /* end critical */

	  return(INIT);               /* Next state is INIT */
	}
	else if (*OPTBODY(option) == DHCPACK &&
		 dhcp_msgtoparam(rcv.dhcp, DHCPLEN(rcv.udp),
				 &tmpparam) == OK) {

	  newsigmask = sigmask(SIGUSR1) | sigmask(SIGUSR2);
	  oldsigmask = sigblock(newsigmask); /* begin critical */
	  merge_param(param_list, &tmpparam);
	  *param_list = tmpparam;
	  param_list->lease_origin = send_epoch;
	  param_list->next = NULL;
	  sigsetmask(oldsigmask); /* end critical */

	  syslog(LOG_INFO, "Got DHCPACK (IP = %s, duration = %d secs)",
		 inet_ntoa(param_list->yiaddr), param_list->lease_duration);
	  arp_reply(&param_list->yiaddr, &arpif);
	  return(BOUND);
	}
      }

#ifndef sun
      rbufp += BPF_WORDALIGN(rbpf->bh_hdrlen + rbpf->bh_caplen);
      n -= (char *) rbufp - (char *) rbpf;
#else /* sun just droppping remains */
      n = 0;
#endif
    }
  }

  if (interrupt2 == 1)
    return(VERIFY);

  if (time(&send_epoch) == -1) {
    syslog(LOG_WARNING, "time() error in renewing(): %m");
    return(INIT);
  }
  if (send_unicast(&param_list->server_id, snd.dhcp) < 0) {
    syslog(LOG_WARNING, "Can't send DHCPREQUEST(RENEWING)");
    return(INIT);
  }
  return(RENEWING);
}


/*
 * REBINDING state
 */
int
rebinding()
{
  int n = 0;
  char *option = NULL;
  char *rbufp = NULL;
  char errmsg[255];
  time_t curr_epoch = 0;
  static int timer = 0;
  fd_set readfds;
  sigset_t newsigmask, oldsigmask;
  struct dhcp_param tmpparam;

  bzero(errmsg, sizeof(errmsg));
  bzero(&newsigmask, sizeof(newsigmask));
  bzero(&oldsigmask, sizeof(oldsigmask));
  bzero(&tmpparam, sizeof(tmpparam));

  if (time(&curr_epoch) == -1) {
    syslog(LOG_WARNING, "time() error in rebinding(): %m");
    return(INIT);
  }
  if (param_list->lease_origin + param_list->lease_duration <= curr_epoch) {
    syslog(LOG_NOTICE, "Can't extend lease, and go to INIT state");
    return(INIT);
  }

  timer = param_list->lease_origin + param_list->lease_duration - curr_epoch;
  if (timer > 60) {
    timer /= 2;
  }
  alarm(0);
  alarm(timer);
  signal(SIGALRM, (void *) retrans_rebinding);

  interrupt = 0;
  interrupt2 = 0;
  while (interrupt == 0 && interrupt2 == 0) {
    FD_ZERO(&readfds);
    FD_SET(dhcpif.fd, &readfds);
    if (select(dhcpif.fd + 1, &readfds, NULL, NULL, NULL) < 0) {
      if (errno == EINTR) {
	continue;
      }
      else {
	syslog(LOG_WARNING, "select() error in rebinding(): %m");
	return(INIT);
      }
    }
#ifndef sun
    else if ((n = read(dhcpif.fd, dhcpif.rbuf, dhcpif.rbufsize)) < 0) {
      syslog(LOG_WARNING, "read() error in rebinding(): %m");
      return(INIT);
    }
#else
    else if ((n = read(dhcpif.fd, &dhcpif.rbuf[QWOFF],
		       dhcpif.rbufsize - QWOFF)) < 0) {
      syslog(LOG_WARNING, "read() error in rebinding(): %m");
      return(INIT);
    }
#endif

#ifndef sun
    rbufp = dhcpif.rbuf;
#else
    rbufp = &dhcpif.rbuf[QWOFF];
#endif
    while (n > 0) {
      align_msg(rbufp);         /* align message within buffer */
      bzero(&tmpparam, sizeof(tmpparam));

      if (check_requesting() &&
	  (option = (char *)
	   pickup_opt(rcv.dhcp, DHCPLEN(rcv.udp), DHCP_MSGTYPE)) != NULL) {

	if (*OPTBODY(option) == DHCPNAK) {
	  syslog(LOG_NOTICE, "Got DHCPNAK in rebinding()");
	  if ((option = (char *)
	      pickup_opt(rcv.dhcp, DHCPLEN(rcv.udp), DHCP_ERRMSG)) != NULL &&
	      nvttostr(OPTBODY(option), errmsg, (int) OPTLEN(option)) == 0) {
	    syslog(LOG_NOTICE, "DHCPNAK contains the error message \"%s\"",
		   errmsg);
	  }

	  newsigmask = sigmask(SIGUSR1) | sigmask(SIGUSR2);
	  oldsigmask = sigblock(newsigmask); /* begin critical */
	  clean_param(param_list);
	  free(param_list);
	  param_list = NULL;
	  sigsetmask(oldsigmask); /* end critical */

	  return(INIT);               /* Next state is INIT */
	}
	else if (*OPTBODY(option) == DHCPACK &&
		 dhcp_msgtoparam(rcv.dhcp, DHCPLEN(rcv.udp),
				 &tmpparam) == OK) {

	  newsigmask = sigmask(SIGUSR1) | sigmask(SIGUSR2);
	  oldsigmask = sigblock(newsigmask); /* begin critical */
	  merge_param(param_list, &tmpparam);
	  *param_list = tmpparam;
	  param_list->lease_origin = send_epoch;
	  param_list->next = NULL;
	  sigsetmask(oldsigmask); /* end critical */

	  syslog(LOG_INFO, "Got DHCPACK (IP = %s, duration = %d secs)",
		 inet_ntoa(param_list->yiaddr), param_list->lease_duration);
	  arp_reply(&param_list->yiaddr, &arpif);
	  return(BOUND);
	}
      }

#ifndef sun
      rbufp += BPF_WORDALIGN(rbpf->bh_hdrlen + rbpf->bh_caplen);
      n -= (char *) rbufp - (char *) rbpf;
#else /* sun just droppping remains */
      n = 0;
#endif
    }
  }

  if (interrupt2 == 1)
    return(VERIFY);

  snd.udp->uh_sum = 0;
  snd.udp->uh_sum = udp_cksum(&spudph, (char *) snd.udp, ntohs(spudph.ulen));
  if (time(&send_epoch) == -1) {
    syslog(LOG_WARNING, "time() error in rebinding(): %m");
    return(INIT);
  }
  if (ether_write(dhcpif.fd, &sbuf.buf[QWOFF], sbuf.size) < sbuf.size) {
    syslog(LOG_WARNING, "Can't send DHCPREQUEST(REBINDING): %m");
    return(INIT);
  }
  return(REBINDING);
}


/*
 * Firstly, read previously allocated address, if exists.
 * This function returns "INIT" when non-serious error occurs.
 */
int
init_reboot()
{
  char *rbufp = NULL;
  int fd = 0;
  int length = 0, tmplength = 0;
  int n = 0;
  time_t lease_origin = 0;

  /* if DHCP_CACHE doesn't exist, this is the first time.
     So we start from INIT state */
  if ((fd = open(DHCP_CACHE, O_RDONLY, 0)) < 0)
    return(INIT);

  /* read lease origin epoch */
  if (read(fd, (void *) &lease_origin, sizeof(lease_origin)) !=
      sizeof(lease_origin)) {
    syslog(LOG_WARNING, "read() error in init_reboot(): %m");
    close(fd);
    return(INIT);
  }
  else if (lease_origin == 0) {
    syslog(LOG_NOTICE, "The lease has already expired. Go to INIT state");
    return(INIT);
  }

  /* read length of DHCP message size */
  if (read(fd, (void *) &length, sizeof(length)) != sizeof(length)) {
    syslog(LOG_WARNING, "read() error in init_reboot(): %m");
    close(fd);
    return(INIT);
  }

  if (dhcpif.rbufsize < length + QWOFF) {
    syslog(LOG_WARNING, "%s is too long", DHCP_CACHE);
    close(fd);
    return(INIT);
  }

  rbufp = &dhcpif.rbuf[QWOFF];
  tmplength = length;
  while ((n = (read(fd, (void *) rbufp, tmplength))) > 0) {
    tmplength -= n;
    rbufp += n;
  }
  if (tmplength != 0) {
    close(fd);
    return(INIT);
  }
  rbufp = &dhcpif.rbuf[QWOFF];

  rcv.ether = (struct ether_header *) rbufp;
  rcv.ip = (struct ip *) &rbufp[ETHERHL];
  rcv.udp = (struct udphdr *) &rbufp[ETHERHL + rcv.ip->ip_hl * WORD];
  rcv.dhcp = (struct dhcp *) &rbufp[ETHERHL + rcv.ip->ip_hl * WORD + UDPHL];

  if (param_list == NULL) {
    if ((param_list = (struct dhcp_param *)
	 calloc(1, sizeof(struct dhcp_param))) == NULL) {
      syslog(LOG_WARNING, "calloc() error in init_reboot(): %m");
      close(fd);
      return(INIT);
    }
  }

  if (dhcp_msgtoparam(rcv.dhcp, DHCPLEN(rcv.udp), param_list) != OK) {
    syslog(LOG_WARNING, "dhcp_msgtoparam() error in init_reboot()");
    clean_param(param_list);
    free(param_list);
    param_list = NULL;
    close(fd);
    return(INIT);
  }

  param_list->lease_origin = lease_origin;
  param_list->next = NULL;

  if (make_request(param_list, REBOOTING) < 0) {
    syslog(LOG_WARNING, "non recoverable error in make_request(). Go to INIT state.");
    return(INIT);
  }
  snd.udp->uh_sum = 0;
  snd.udp->uh_sum = udp_cksum(&spudph, (char *) snd.udp, ntohs(spudph.ulen));
  if (ether_write(dhcpif.fd, &sbuf.buf[QWOFF], sbuf.size) < sbuf.size) {
    syslog(LOG_WARNING, "Can't send DHCPREQUEST(REBOOTING): %m");
    return(INIT);
  }

  return(REBOOTING);
}


/*
 * VERIFY state
 */
int
verify()
{
  if (make_request(param_list, VERIFYING) < 0) {
    syslog(LOG_WARNING, "non recoverable error in make_request(). Go to INIT state.");
    return(INIT);
  }
  snd.udp->uh_sum = 0;
  snd.udp->uh_sum = udp_cksum(&spudph, (char *) snd.udp, ntohs(spudph.ulen));
  if (ether_write(dhcpif.fd, &sbuf.buf[QWOFF], sbuf.size) < sbuf.size) {
    syslog(LOG_WARNING, "Can't send DHCPREQUEST(VERIFY): %m");
    return(INIT);
  }

  return(VERIFYING);
}


/*
 * REBOOTING state and VERIFYING state
 */
int
reboot_verify()
{
  int n = 0;
  int arpans = 0;
  time_t curr_epoch = 0;
  char *option = NULL;
  char *rbufp = NULL;
  char errmsg[255];
  static int timer = 0;
  static int retry = 0;
  fd_set readfds;
  sigset_t newsigmask, oldsigmask;
  struct dhcp_param tmpparam;
  struct dhcp_reqspec tmp_reqspec;

  bzero(errmsg, sizeof(errmsg));
  bzero(&newsigmask, sizeof(newsigmask));
  bzero(&oldsigmask, sizeof(oldsigmask));
  bzero(&tmpparam, sizeof(tmpparam));
  bzero(&tmp_reqspec, sizeof(tmp_reqspec));

  if (prev_state == INIT_REBOOT || prev_state == VERIFY) {
    timer = FIRSTTIMER;
    retry = 0;
    if (time(&init_epoch) == -1) {
      syslog(LOG_WARNING, "time() error in reboot_verify(): %m");
      return(INIT);
    }
  }
  else if (retry == REQUEST_RETRANS) {
    syslog(LOG_NOTICE, "Client can get neither DHCPACK nor DHCPNAK. Give up");

    if (time(&curr_epoch) == -1) {
      syslog(LOG_WARNING, "time() error in reboot_verify(): %m");
      return(INIT);
    }
    if (param_list->lease_origin == 0 ||
	param_list->lease_origin + param_list->lease_duration <= curr_epoch) {
      syslog(LOG_NOTICE, "The lease has already expired. Go to INIT state");
      return(INIT);
    }
    else {
      syslog(LOG_INFO, "Go to BOUND state");
      arp_reply(&param_list->yiaddr, &arpif);
      return(BOUND);
    }
  }
  else if (timer < MAXTIMER) {
    timer *= 2;
  }

  alarm(0);
  ualarm(SLEEP_RANDOM(timer), 0);
  if (retry < REQUEST_RETRANS)
    signal(SIGALRM, (void *) retrans_reboot_verify);

  interrupt = 0;
  while (interrupt == 0) {
    FD_ZERO(&readfds);
    FD_SET(dhcpif.fd, &readfds);
    if (select(dhcpif.fd + 1, &readfds, NULL, NULL, NULL) < 0) {
      if (errno == EINTR) {
	continue;
      }
      else {
	syslog(LOG_WARNING, "select() error in reboot_verify(): %m");
	return(INIT);
      }
    }
#ifndef sun
    else if ((n = read(dhcpif.fd, dhcpif.rbuf, dhcpif.rbufsize)) < 0) {
      syslog(LOG_WARNING, "read() error in reboot_verify(): %m");
      return(INIT);
    }
#else
    else if ((n = read(dhcpif.fd, &dhcpif.rbuf[QWOFF],
		       dhcpif.rbufsize - QWOFF)) < 0) {
      syslog(LOG_WARNING, "read() error in reboot_verify(): %m");
      return(INIT);
    }
#endif

#ifndef sun
    rbufp = dhcpif.rbuf;
#else
    rbufp = &dhcpif.rbuf[QWOFF];
#endif

    while (n > 0) {
      align_msg(rbufp);         /* align message within buffer */
      bzero(&tmpparam, sizeof(tmpparam));

      if (check_requesting() &&
	  (option = (char *)
	   pickup_opt(rcv.dhcp, DHCPLEN(rcv.udp), DHCP_MSGTYPE)) != NULL) {

	if (*OPTBODY(option) == DHCPNAK) {
	  syslog(LOG_NOTICE, "Got DHCPNAK in reboot_verify()");
	  if ((option = (char *)
	      pickup_opt(rcv.dhcp, DHCPLEN(rcv.udp), DHCP_ERRMSG)) != NULL &&
	      nvttostr(OPTBODY(option), errmsg, (int) OPTLEN(option)) == 0) {
	    syslog(LOG_NOTICE, "DHCPNAK contains the error message \"%s\"",
		   errmsg);
	  }

	  newsigmask = sigmask(SIGUSR1) | sigmask(SIGUSR2);
	  oldsigmask = sigblock(newsigmask); /* begin critical */
	  clean_param(param_list);
	  free(param_list);
	  param_list = NULL;
	  sigsetmask(oldsigmask); /* end critical */

	  return(INIT);               /* Next state is INIT */
	}
	else if (*OPTBODY(option) == DHCPACK &&
		 dhcp_msgtoparam(rcv.dhcp, DHCPLEN(rcv.udp),
				 &tmpparam) == OK) {
	  if ((arpans = arp_check(&tmpparam.yiaddr, &arpif)) == OK) {
	    newsigmask = sigmask(SIGUSR1) | sigmask(SIGUSR2);
	    oldsigmask = sigblock(newsigmask); /* begin critical */
	    merge_param(param_list, &tmpparam);
	    *param_list = tmpparam;
	    param_list->lease_origin = init_epoch;
	    param_list->next = NULL;
	    sigsetmask(oldsigmask); /* end critical */

	    syslog(LOG_INFO, "Got DHCPACK (IP = %s, duration = %d secs)",
		   inet_ntoa(param_list->yiaddr), param_list->lease_duration);
	    arp_reply(&param_list->yiaddr, &arpif);
	    return(BOUND);
	  }
	  else {
	    /* make dhcp_declinfo and send DHCPDECLINE */
	    set_declinfo(&tmp_reqspec, param_list, errmsg, arpans);
	    dhcp_decline(&tmp_reqspec);

	    newsigmask = sigmask(SIGUSR1) | sigmask(SIGUSR2);
	    oldsigmask = sigblock(newsigmask); /* begin critical */
	    clean_param(param_list);
	    free(param_list);
	    param_list = NULL;
	    sigsetmask(oldsigmask); /* end critical */

	    syslog(LOG_NOTICE,
		   "Got non-preferable DHCPACK, so go into INIT state");
	    return(INIT);
	  }
	}
      }

#ifndef sun
      rbufp += BPF_WORDALIGN(rbpf->bh_hdrlen + rbpf->bh_caplen);
      n -= (char *) rbufp - (char *) rbpf;
#else /* sun just droppping remains */
      n = 0;
#endif
    }
  }

  retry++;
  return(curr_state);        /* Next state is same as current state */
}


/*
 * finite state machine
 */
int
dhcp_client(ifp)
  struct if_info *ifp;
{
  int retval = 0;
  int next_state = 0;

  fsm[INIT] = init;
  fsm[WAIT_OFFER] = wait_offer;
  fsm[SELECTING] = selecting;
  fsm[REQUESTING] = requesting;
  fsm[BOUND] = bound;
  fsm[RENEWING] = renewing;
  fsm[REBINDING] = rebinding;
  fsm[INIT_REBOOT] = init_reboot;
  fsm[VERIFY] = verify;
  fsm[REBOOTING] = reboot_verify;
  fsm[VERIFYING] = reboot_verify;

  intface = *ifp;

  if ((retval = initialize()) < 0)
    return(retval);

  if ((int) signal(SIGUSR1, (void *) go_verify) < 0) {
    syslog(LOG_WARNING, "cannot set signal handler(SIGUSR1): %m");
    return(-1);
  }
  if ((int) signal(SIGUSR2, (void *) release) < 0) {
    syslog(LOG_WARNING, "cannot set signal handler(SIGUSR2): %m");
    return(-1);
  }
  if ((int) signal(SIGTERM, (void *) finish) < 0) {
    syslog(LOG_WARNING, "cannot set signal handler(SIGTERM): %m");
    return(-1);
  }
  if ((int) signal(SIGINT, (void *) finish) < 0) {
    syslog(LOG_WARNING, "cannot set signal handler(SIGINT): %m");
    return(-1);
  }

  prev_state = curr_state = INIT_REBOOT;
  while (1) {
    if ((next_state = (*fsm[curr_state])()) < 0) {
      syslog(LOG_WARNING, "Some exception occurs in finite state machine");
      down_if(ifp);
      return(curr_state);
    }
    prev_state = curr_state;
    curr_state = next_state;
#ifdef DEBUG
    fprintf(stderr, "next = %d\n", next_state);
#endif
  }

}


void
usage()
{
  fprintf(stderr, "Usage: dhcpc [-d] if_name\n");
  exit(1);
}


void
version()
{
  fprintf(stderr, "WIDE DHCP client version 1.3n");
  exit(1);
}


int
main(argc, argv)
  int argc;
  char **argv;
{
  int debug = 0;
  int n = 0;
  struct if_info ifinfo;

  bzero(&reqspec, sizeof(reqspec));
  bzero(&ifinfo, sizeof(ifinfo));
  if (argc < 2) usage();
  --argc, ++argv;
  if (argv[0][0] == '-' && argv[0][1] == 'v') version();
  if (argv[0][0] == '-' && argv[0][1] == 'd') {
    debug = 1;
    --argc, ++argv;
  }

  if (argc < 1) usage();
  strcpy(ifinfo.name, argv[0]);

  /*
   * go into background and disassociate from controlling terminal
   */
  if (debug == 0) {
    if(fork() != 0) exit(0);  /* Exit if error or parent process. */
    for (n = 0; n < 10; n++)  /* Close all open file descriptors. */
      close(n);
    open("/", O_RDONLY);
    dup2(0, 1);               /* Reassign standard output. */
    dup2(0, 2);               /* Reassign standard error. */
    if ((n = open("/dev/tty", O_RDWR)) >= 0) {
      ioctl(n, TIOCNOTTY, (char *) 0);
      close(n);
    }
  }

  /*
   * set request specification
   */
  reqspec.select_offer = DHCPOFFER_SELECT_FIRST;
  reqspec.waitsecs = 0;
  reqspec.lease = REQLEASE;

  reqspec.reqlist.len = 0;
  reqspec.reqlist.list[reqspec.reqlist.len++] = SUBNET_MASK;
  reqspec.reqlist.list[reqspec.reqlist.len++] = ROUTER;
  reqspec.reqlist.list[reqspec.reqlist.len++] = BRDCAST_ADDR;
  
  return(dhcp_client(&ifinfo));
}
