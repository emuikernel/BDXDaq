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
#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/uio.h>
#include <net/if.h>
#include <net/route.h>
#ifdef sun
#include <sys/stropts.h>
#include <net/nit.h>
#include <net/nit_if.h>
#include <net/nit_pf.h>
#include <net/packetfilt.h>
#else
#include <net/bpf.h>
#endif
#ifdef __osf__
#include <net/pfilt.h>
#endif
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/if_ether.h>
#include <sys/param.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <arpa/inet.h>

#include "dhcp.h"
#include "common.h"
#include "common_subr.h"
#include "dhcpc_subr.h"

#ifdef sun
struct packetfilt dhcpf;
struct packetfilt arpf;
#else
/*
 * bpf filter program
 */
#ifdef sony_news
struct bpf_insn dhcpf[] = {
  BPF_STMT(LdHOp, 12),                    /* A <- ETHER_TYPE */
  BPF_JUMP(EQOp, ETHERTYPE_IP, 1, 10),    /* is it IP ? */
  BPF_STMT(LdHOp, 20),                    /* A <- IP FRAGMENT */
  BPF_STMT(AndIOp, 0x1fff),               /* A <- A & 0x1fff */
  BPF_JUMP(EQOp, 0, 1, 1),                /* OFFSET == 0 ? */ /* bug bug ? */
  BPF_STMT(LdBOp, 23),                    /* A <- IP_PROTO */
  BPF_JUMP(EQOp, IPPROTO_UDP, 1, 5),      /* UDP ? */
  BPF_STMT(LdxmsOp, 14),                  /* X <- IPHDR LEN */
  BPF_STMT(ILdHOp, 16),                   /* A <- UDP DSTPORT */
  BPF_JUMP(EQOp, 0, 1, 2),                /* check DSTPORT */
  BPF_STMT(RetOp, (u_int)-1),             /* return all*/
  BPF_STMT(RetOp, 0)                      /* ignore */
};
#define DSTPORTIS  9
#else
struct bpf_insn dhcpf[] = {
  BPF_STMT(BPF_LD+BPF_H+BPF_ABS, 12),                     /* A <- ETHER_TYPE */
  BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, ETHERTYPE_IP, 0, 8),    /* is it IP ? */
  BPF_STMT(BPF_LD+BPF_H+BPF_ABS, 20),                     /* A <- IP FRAGMENT */
  BPF_JUMP(BPF_JMP+BPF_JSET+BPF_K, 0x1fff, 6, 0),         /* OFFSET == 0 ? */
  BPF_STMT(BPF_LD+BPF_B+BPF_ABS, 23),                     /* A <- IP_PROTO */
  BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, IPPROTO_UDP, 0, 4),     /* UDP ? */
  BPF_STMT(BPF_LDX+BPF_B+BPF_MSH, 14),                    /* X <- IPHDR LEN */
  BPF_STMT(BPF_LD+BPF_H+BPF_IND, 16),                     /* A <- UDP DSTPORT */
  BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, 0, 0, 1),               /* check DSTPORT */
  BPF_STMT(BPF_RET+BPF_K, (u_int)-1),                     /* return all*/
  BPF_STMT(BPF_RET+BPF_K, 0)                              /* ignore */
};
#define DSTPORTIS  8
#endif /* sony_news */

struct bpf_program dhcpfilter = {
  sizeof(dhcpf) / sizeof(struct bpf_insn),
  dhcpf
};

#ifdef sony_news
struct bpf_insn arpf[] = {
  BPF_STMT(LdHOp, 12),                     /* A <- ETHER_TYPE */
  BPF_JUMP(EQOp, ETHERTYPE_ARP, 1, 6),     /* is it ARP ? */
  BPF_STMT(LdHOp, 20),                     /* A <- ARP_OP */
  BPF_JUMP(EQOp, ARPOP_REPLY, 1, 4),       /* is it ARP reply ? */
  BPF_STMT(LdOp, 28),                      /* A <- target IP */
  BPF_JUMP(EQOp, 0, 1, 2),                 /* is it target ? */
  BPF_STMT(RetOp, (u_int)-1),
  BPF_STMT(RetOp, 0)
};
#else /* if sony_news */
struct bpf_insn arpf[] = {
  BPF_STMT(BPF_LD+BPF_H+BPF_ABS, 12),                     /* A <- ETHER_TYPE */
  BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, ETHERTYPE_ARP, 0, 5),   /* is it ARP ? */
  BPF_STMT(BPF_LD+BPF_H+BPF_ABS, 20),                     /* A <- ARP_OP */
  BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, ARPOP_REPLY, 0, 3),     /* is it ARP reply ? */
  BPF_STMT(BPF_LD+BPF_W+BPF_ABS, 28),                     /* A <- target IP */
  BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, 0, 0, 1),               /* is it target ? */
  BPF_STMT(BPF_RET+BPF_K, (u_int)-1),
  BPF_STMT(BPF_RET+BPF_K, 0)
};
#endif /* sony_news */
#define TARGETIP  5
    
struct bpf_program arpfilter = {
  sizeof(arpf) / sizeof(struct bpf_insn),
  arpf
};
#endif /* not sun */

struct msg snd;
struct msg rcv;
struct ps_udph spudph;
struct bpf_hdr *rbpf;

int config_if();
void set_route();
void make_decline();
void make_release();
Long generate_xid();


/*
 * generate transaction identifier
 */
Long
generate_xid()
{
  time_t current = 0;
  u_short result1 = 0, result2 = 0;
  static int first = 0;
  static u_short tmp[6];

  if (first == 0) {
    bcopy(intface.haddr.haddr, tmp, 6);
  }

  time(&current);
  result1 = cksum(tmp, 6);
  result2 = cksum((u_short *) &current, 2);

  return((result1 << 16) + result2);
}


/*
 * broadcast ARP_REQUEST and check reply
 *
 *   return -1, if there is the invalid client
 */
int
arp_check(target, ifp)
  struct in_addr *target;
  struct interface *ifp;
{
  int i = 0;
  Long tmp;
  char sbuf[ETHERHL + sizeof(struct ether_arp)];
  struct ether_header *sether = NULL;
  struct ether_arp *sarp = NULL;
#ifdef sun
  struct timeval timeout;
  fd_set readfds;
  struct strioctl si;
  register u_short *fwp = &arpf.Pf_Filter[0];
  u_short targets[2];

  bcopy(&target->s_addr, targets, sizeof(struct in_addr));
  bzero(&timeout, sizeof(timeout));
#endif

  bzero(sbuf, ETHERHL + sizeof(struct ether_arp));
  sether = (struct ether_header *) sbuf;
  sarp = (struct ether_arp *) &sbuf[ETHERHL];

#ifdef sun
  *fwp++ = ENF_PUSHWORD + 6;
  *fwp++ = ENF_PUSHLIT | ENF_CAND;
  *fwp++ = htons(ETHERTYPE_ARP);
  *fwp++ = ENF_PUSHWORD + 10;
  *fwp++ = ENF_PUSHLIT | ENF_CAND;
  *fwp++ = htons(ARPOP_REPLY);
  *fwp++ = ENF_PUSHWORD + 14;
  *fwp++ = ENF_PUSHLIT | ENF_CAND;
  *fwp++ = targets[0];
  *fwp++ = ENF_PUSHWORD + 15;
  *fwp++ = ENF_PUSHLIT | ENF_CAND;
  *fwp++ = targets[1];
  arpf.Pf_FilterLen = fwp - &arpf.Pf_Filter[0];

  bzero(&si, sizeof(si));
  si.ic_cmd = NIOCSETF;
  si.ic_timout = INFTIM;
  si.ic_len = sizeof(arpf);
  si.ic_dp = (char *)&arpf;
  if (ioctl(arpif.fd, I_STR, (char *)&si) < 0){
    syslog(LOG_WARNING, "ioctl(NIOCSETF) error in arp_check: %m");
    return(0);
  }
  if (ioctl(arpif.fd, I_FLUSH, (char *)FLUSHR) < 0){
    syslog(LOG_WARNING, "ioctl(I_FLUSH) error in arp_check: %m");
    return(0);
  }
#else
  arpf[TARGETIP].k = ntohl(target->s_addr);     /* must be host byte order */
  if (ioctl(ifp->fd, BIOCSETF, &arpfilter) < 0) {
    syslog(LOG_WARNING, "ioctl(BIOCSETF) for arp: %m");
    return(0);
  }
#endif

  sarp->arp_hrd = htons(ARPHRD_ETHER);
  sarp->arp_pro = htons(ETHERTYPE_IP);
  sarp->arp_hln = 6;
  sarp->arp_pln = 4;
  sarp->arp_op = htons(ARPOP_REQUEST);

  /* Set sender's hardware address to your hardware address. (RFC 1541). */
  for (i = 0; i < 6; i++) {

#ifdef sun
    sarp->arp_sha.ether_addr_octet[i] =
      sether->ether_shost.ether_addr_octet[i] =	ifp->ifinfo->haddr.haddr[i];
    sarp->arp_tha.ether_addr_octet[i] = 0;
    sether->ether_dhost.ether_addr_octet[i] = 0xff;
#else
    sarp->arp_sha[i] = sether->ether_shost[i] = ifp->ifinfo->haddr.haddr[i];
    sarp->arp_tha[i] = 0;
    sether->ether_dhost[i] = 0xff;
#endif
  }

  /* Set sender's IP address for ARP requests to 0 as per RFC 1541. */  
  tmp = 0;
  bcopy(&tmp, sarp->arp_spa, sarp->arp_pln);
  bcopy(&target->s_addr, sarp->arp_tpa, sarp->arp_pln);
  sether->ether_type = htons(ETHERTYPE_ARP);

  if (ether_write(ifp->fd, sbuf, ETHERHL + sizeof(struct ether_arp)) < 0) {
    return(0);                               /* it is harmless to return 0 */
  }

#ifdef sun
  /* if timeout, there is no replay and return 0 */
  timeout.tv_sec = 0;
  timeout.tv_usec = 500000;
  FD_ZERO(&readfds);
  FD_SET(ifp->fd, &readfds);
  if (select(ifp->fd + 1, &readfds, NULL, NULL, &timeout) <= 0) {
    return(0);
  } else {
    return(-1);
  }
#else
  if (read(ifp->fd, ifp->rbuf, ifp->rbufsize) <= 0)
    return(0);
  else
    return(-1);
#endif
}


/*
 * broadcast ARP_REPLY
 */
int
arp_reply(ipaddr, ifp)
  struct in_addr *ipaddr;
  struct interface *ifp;
{
  int i = 0;
  char sbuf[ETHERHL + sizeof(struct ether_arp)];
  struct ether_header *sether = NULL;
  struct ether_arp *sarp = NULL;

#ifdef DEBUG
  return(0);
#endif

  bzero(sbuf, ETHERHL + sizeof(struct ether_arp));
  sether = (struct ether_header *) sbuf;
  sarp = (struct ether_arp *) &sbuf[ETHERHL];

  sarp->arp_hrd = htons(ARPHRD_ETHER);
  sarp->arp_pro = htons(ETHERTYPE_IP);
  sarp->arp_hln = 6;
  sarp->arp_pln = 4;
  sarp->arp_op = htons(ARPOP_REPLY);

  for (i = 0; i < 6; i++) {
#ifdef sun
    sarp->arp_tha.ether_addr_octet[i] = sarp->arp_sha.ether_addr_octet[i] =
      sether->ether_shost.ether_addr_octet[i] = ifp->ifinfo->haddr.haddr[i];
    sether->ether_dhost.ether_addr_octet[i] = 0xff;
#else
    sarp->arp_tha[i] = sarp->arp_sha[i] =
      sether->ether_shost[i] = ifp->ifinfo->haddr.haddr[i];
    sether->ether_dhost[i] = 0xff;
#endif
  }

  bcopy(&ipaddr->s_addr, sarp->arp_spa, sarp->arp_pln);
  bcopy(&ipaddr->s_addr, sarp->arp_tpa, sarp->arp_pln);

  sether->ether_type = htons(ETHERTYPE_ARP);

  if (ether_write(ifp->fd, sbuf, ETHERHL + sizeof(struct ether_arp)) < 0) {
    return(-1);
  }

  return(0);
}


/*
 * convert NVT ASCII to strings
 * (actually, only remove null characters)
 */
int *
nvttostr(nvt, str, length)
  char *nvt;
  char *str;
  int length;
{
  register int i = 0;
  register char *tmp = NULL;

  tmp = str;

  for (i = 0; i < length; i++) {
    if (nvt[i] != NULL) {
      *tmp = nvt[i];
      tmp++;
    }
  }

  str[length] = '\0';
  return(0);
}


/*
 * convert DHCP message to struct dhcp_param
 *
 *
 */
int
dhcp_msgtoparam(msg, msglen, parameter)
  struct dhcp *msg;
  int msglen;
  struct dhcp_param *parameter;
{
  register char *optp = NULL;
  char tag = 0;
  int sname_is_opt = 0,
      file_is_opt = 0;
  int err = 0;

  char *endofopt = &msg->options[msglen - DFLTDHCPLEN + DFLTOPTLEN];
  bzero(parameter->got_option, GOTOPTSIZ);

  for (optp = &msg->options[MAGIC_LEN]; optp <= endofopt; optp++) {
    tag = *optp;

    /* skip the PAD */
    if (tag == PAD) {
      continue;
    }

    /* reach to the END */
    if (tag == END) {
      break;
    }

    /* handle the "Option Overload" */
    if (tag == OPT_OVERLOAD) {
      optp += 2;
      switch (*optp) {
      case FILE_ISOPT:
	file_is_opt = TRUE;
	break;
      case SNAME_ISOPT:
	sname_is_opt = TRUE;
	break;
      case BOTH_AREOPT:
	file_is_opt = sname_is_opt = TRUE;
	break;
      default:
	break;
      }
      continue;
    }

    if (handle_param[(int) tag] != NULL) {
      if ((err = (*handle_param[(int) tag])(optp, parameter)) != 0) {
	return(err);
      } else {
	SETBIT(parameter->got_option, tag);
      }
    }

    optp++;
    optp += *optp;
  }

  if (file_is_opt) {
    char *endofopt = &msg->file[MAX_FILE];
    for (optp = msg->file; optp <= endofopt; optp++) {

      tag = *optp;

      /* skip the PAD */
      if (tag == PAD) {
	continue;
      }

      /* reach to the END */
      if (tag == END) {
	break;
      }

      if (handle_param[(int) tag] != NULL) {
	if ((err = (*handle_param[(int) tag])(optp, parameter)) != 0) {
	  return(err);
	} else {
	  SETBIT(parameter->got_option, tag);
	}
      }

      optp++;
      optp += *optp;
    }
  } else {
    if ((parameter->file = calloc(1, strlen(msg->file) + 1)) == NULL) {
      return(-1);
    }
    strcpy(parameter->file, msg->file);
  }


  if (sname_is_opt) {
    char *endofopt = &msg->sname[MAX_SNAME];
    for (optp = msg->file; optp <= endofopt; optp++) {

      tag = *optp;

      /* skip the PAD */
      if (tag == PAD) {
	continue;
      }

      /* reach to the END */
      if (tag == END) {
	break;
      }

      if (handle_param[(int) tag] != NULL) {
	if ((err = (*handle_param[(int) tag])(optp, parameter)) != 0) {
	  return(err);
	} else {
	  SETBIT(parameter->got_option, tag);
	}
      }

      optp++;
      optp += *optp;
    }
  } else {
    if ((parameter->sname = calloc(1, strlen(msg->sname) + 1)) == NULL) {
      return(-1);
    }
    strcpy(parameter->sname, msg->sname);
  }

  parameter->ciaddr.s_addr = msg->ciaddr.s_addr;
  parameter->yiaddr.s_addr = msg->yiaddr.s_addr;
  parameter->siaddr.s_addr = msg->siaddr.s_addr;
  parameter->giaddr.s_addr = msg->giaddr.s_addr;

  if (parameter->dhcp_t1 == 0) {
    parameter->dhcp_t1 = parameter->lease_duration / 2;
    SETBIT(parameter->got_option, DHCP_T1);
  }
  if (parameter->dhcp_t2 == 0) {
    double tmp = (double) parameter->lease_duration * 0.875;
    parameter->dhcp_t2 = (unsigned short) tmp;
    SETBIT(parameter->got_option, DHCP_T2);
  }

  return(0);
}


/*
 * free memory which is used in struct dhcp_param
 */
int
clean_param(param)
  struct dhcp_param *param;
{
  if (param == NULL)
    return(0);

  if (param->sname != NULL) free(param->sname);
  if (param->file != NULL) free(param->file);
  if (param->hostname != NULL) free(param->hostname);
  if (param->merit_dump != NULL) free(param->merit_dump);
  if (param->dns_domain != NULL) free(param->dns_domain);
  if (param->root_path != NULL) free(param->root_path);
  if (param->extensions_path != NULL) free(param->extensions_path);
  if (param->nis_domain != NULL) free(param->nis_domain);
  if (param->nb_scope != NULL) free(param->nb_scope);
  if (param->errmsg != NULL) free(param->errmsg);
  if (param->nisp_domain != NULL) free(param->nisp_domain);

  if (param->mtu_plateau_table != NULL) {
    if (param->mtu_plateau_table->shortnum != NULL)
      free(param->mtu_plateau_table->shortnum);
    free(param->mtu_plateau_table);
  }
  if (param->subnet_mask != NULL) free(param->subnet_mask);
  if (param->swap_server != NULL) free(param->swap_server);
  if (param->brdcast_addr != NULL) free(param->brdcast_addr);
  if (param->router_solicit != NULL) free(param->router_solicit);
  if (param->router != NULL) {
    if (param->router->addr != NULL)
      free(param->router->addr);
    free(param->router);
  }
  if (param->time_server != NULL) {
    if (param->time_server->addr != NULL)
      free(param->time_server->addr);
    free(param->time_server);
  }
  if (param->name_server != NULL) {
    if (param->name_server->addr != NULL)
      free(param->name_server->addr);
    free(param->name_server);
  }
  if (param->dns_server != NULL) {
    if (param->dns_server->addr != NULL)
      free(param->dns_server->addr);
    free(param->dns_server);
  }
  if (param->log_server != NULL) {
    if (param->log_server->addr != NULL)
      free(param->log_server->addr);
    free(param->log_server);
  }
  if (param->cookie_server != NULL) {
    if (param->cookie_server->addr != NULL)
      free(param->cookie_server->addr);
    free(param->cookie_server);
  }
  if (param->lpr_server != NULL) {
    if (param->lpr_server->addr != NULL)
      free(param->lpr_server->addr);
    free(param->lpr_server);
  }
  if (param->impress_server != NULL) {
    if (param->impress_server->addr != NULL)
      free(param->impress_server->addr);
    free(param->impress_server);
  }
  if (param->rls_server != NULL) {
    if (param->rls_server->addr != NULL)
      free(param->rls_server->addr);
    free(param->rls_server);
  }
  if (param->policy_filter != NULL) {
    if (param->policy_filter->addr != NULL)
      free(param->policy_filter->addr);
    free(param->policy_filter);
  }
  if (param->static_route != NULL) {
    if (param->static_route->addr != NULL)
      free(param->static_route->addr);
    free(param->static_route);
  }
  if (param->nis_server != NULL) {
    if (param->nis_server->addr != NULL)
      free(param->nis_server->addr);
    free(param->nis_server);
  }
  if (param->ntp_server != NULL) {
    if (param->ntp_server->addr != NULL)
      free(param->ntp_server->addr);
    free(param->ntp_server);
  }
  if (param->nbn_server != NULL) {
    if (param->nbn_server->addr != NULL)
      free(param->nbn_server->addr);
    free(param->nbn_server);
  }
  if (param->nbdd_server != NULL) {
    if (param->nbdd_server->addr != NULL)
      free(param->nbdd_server->addr);
    free(param->nbdd_server);
  }
  if (param->xfont_server != NULL) {
    if (param->xfont_server->addr != NULL)
      free(param->xfont_server->addr);
    free(param->xfont_server);
  }
  if (param->xdisplay_manager != NULL) {
    if (param->xdisplay_manager->addr != NULL)
      free(param->xdisplay_manager->addr);
    free(param->xdisplay_manager);
  }
  if (param->nisp_server != NULL) {
    if (param->nisp_server->addr != NULL)
      free(param->nisp_server->addr);
    free(param->nisp_server);
  }
  if (param->mobileip_ha != NULL) {
    if (param->mobileip_ha->addr != NULL)
      free(param->mobileip_ha->addr);
    free(param->mobileip_ha);
  }
  if (param->smtp_server != NULL) {
    if (param->smtp_server->addr != NULL)
      free(param->smtp_server->addr);
    free(param->smtp_server);
  }
  if (param->pop3_server != NULL) {
    if (param->pop3_server->addr != NULL)
      free(param->pop3_server->addr);
    free(param->pop3_server);
  }
  if (param->nntp_server != NULL) {
    if (param->nntp_server->addr != NULL)
      free(param->nntp_server->addr);
    free(param->nntp_server);
  }
  if (param->dflt_www_server != NULL) {
    if (param->dflt_www_server->addr != NULL)
      free(param->dflt_www_server->addr);
    free(param->dflt_www_server);
  }
  if (param->dflt_finger_server != NULL) {
    if (param->dflt_finger_server->addr != NULL)
      free(param->dflt_finger_server->addr);
    free(param->dflt_finger_server);
  }
  if (param->dflt_irc_server != NULL) {
    if (param->dflt_irc_server->addr != NULL)
      free(param->dflt_irc_server->addr);
    free(param->dflt_irc_server);
  }
  if (param->streettalk_server != NULL) {
    if (param->streettalk_server->addr != NULL)
      free(param->streettalk_server->addr);
    free(param->streettalk_server);
  }
  if (param->stda_server != NULL) {
    if (param->stda_server->addr != NULL)
      free(param->stda_server->addr);
    free(param->stda_server);
  }
  bzero(param, sizeof(struct dhcp_param));
  return(0);
}


/*
 * if there is no information in newp but oldp, copy it to newp
 * else free the appropriate memory of oldp
 */
int
merge_param(oldp, newp)
  struct dhcp_param *oldp;
  struct dhcp_param *newp;
{
  if (oldp == NULL || newp == NULL)
    return(0);

  if (newp->sname == NULL && oldp->sname != NULL)
    newp->sname = oldp->sname;
  else 
    free(oldp->sname);

  if (newp->file == NULL && oldp->file != NULL)
    newp->file = oldp->file;
  else 
    free(oldp->file);

  if (newp->hostname == NULL && oldp->hostname != NULL)
    newp->hostname = oldp->hostname;
  else 
    free(oldp->hostname);

  if (newp->merit_dump == NULL && oldp->merit_dump != NULL)
    newp->merit_dump = oldp->merit_dump;
  else 
    free(oldp->merit_dump);

  if (newp->dns_domain == NULL && oldp->dns_domain != NULL)
    newp->dns_domain = oldp->dns_domain;
  else 
    free(oldp->dns_domain);

  if (newp->root_path == NULL && oldp->root_path != NULL)
    newp->root_path = oldp->root_path;
  else 
    free(oldp->root_path);

  if (newp->extensions_path == NULL && oldp->extensions_path != NULL)
    newp->extensions_path = oldp->extensions_path;
  else 
    free(oldp->extensions_path);

  if (newp->nis_domain == NULL && oldp->nis_domain != NULL)
    newp->nis_domain = oldp->nis_domain;
  else 
    free(oldp->nis_domain);

  if (newp->nb_scope == NULL && oldp->nb_scope != NULL)
    newp->nb_scope = oldp->nb_scope;
  else 
    free(oldp->nb_scope);

  if (newp->errmsg == NULL && oldp->errmsg != NULL)
    newp->errmsg = oldp->errmsg;
  else 
    free(oldp->errmsg);

  if (newp->nisp_domain == NULL && oldp->nisp_domain != NULL)
    newp->nisp_domain = oldp->nisp_domain;
  else 
    free(oldp->nisp_domain);

  if (newp->mtu_plateau_table == NULL && oldp->mtu_plateau_table != NULL) {
    newp->mtu_plateau_table = oldp->mtu_plateau_table;
  } else {
    if (oldp->mtu_plateau_table != NULL &&
	oldp->mtu_plateau_table->shortnum != NULL) {
      free(oldp->mtu_plateau_table->shortnum);
    }
    free(oldp->mtu_plateau_table);
  }

  if (newp->subnet_mask == NULL && oldp->subnet_mask != NULL)
    newp->subnet_mask = oldp->subnet_mask;
  else 
    free(oldp->subnet_mask);

  if (newp->swap_server == NULL && oldp->swap_server != NULL)
    newp->swap_server = oldp->swap_server;
  else 
    free(oldp->swap_server);

  if (newp->brdcast_addr == NULL && oldp->brdcast_addr != NULL)
    newp->brdcast_addr = oldp->brdcast_addr;
  else 
    free(oldp->brdcast_addr);

  if (newp->router_solicit == NULL && oldp->router_solicit != NULL)
    newp->router_solicit = oldp->router_solicit;
  else 
    free(oldp->router_solicit);

  if (newp->router == NULL && oldp->router != NULL) {
    newp->router = oldp->router;
  } else {
    if (oldp->router != NULL &&
	oldp->router->addr != NULL) {
      free(oldp->router->addr);
    }
    free(oldp->router);
  }

  if (newp->time_server == NULL && oldp->time_server != NULL) {
    newp->time_server = oldp->time_server;
  } else {
    if (oldp->time_server != NULL &&
	oldp->time_server->addr != NULL) {
      free(oldp->time_server->addr);
    }
    free(oldp->time_server);
  }

  if (newp->name_server == NULL && oldp->name_server != NULL) {
    newp->name_server = oldp->name_server;
  } else {
    if (oldp->name_server != NULL &&
	oldp->name_server->addr != NULL) {
      free(oldp->name_server->addr);
    }
    free(oldp->name_server);
  }

  if (newp->dns_server == NULL && oldp->dns_server != NULL) {
    newp->dns_server = oldp->dns_server;
  } else {
    if (oldp->dns_server != NULL &&
	oldp->dns_server->addr != NULL) {
      free(oldp->dns_server->addr);
    }
    free(oldp->dns_server);
  }

  if (newp->log_server == NULL && oldp->log_server != NULL) {
    newp->log_server = oldp->log_server;
  } else {
    if (oldp->log_server != NULL &&
	oldp->log_server->addr != NULL) {
      free(oldp->log_server->addr);
    }
    free(oldp->log_server);
  }

  if (newp->cookie_server == NULL && oldp->cookie_server != NULL) {
    newp->cookie_server = oldp->cookie_server;
  } else {
    if (oldp->cookie_server != NULL &&
	oldp->cookie_server->addr != NULL) {
      free(oldp->cookie_server->addr);
    }
    free(oldp->cookie_server);
  }

  if (newp->lpr_server == NULL && oldp->lpr_server != NULL) {
    newp->lpr_server = oldp->lpr_server;
  } else {
    if (oldp->lpr_server != NULL &&
	oldp->lpr_server->addr != NULL) {
      free(oldp->lpr_server->addr);
    }
    free(oldp->lpr_server);
  }

  if (newp->impress_server == NULL && oldp->impress_server != NULL) {
    newp->impress_server = oldp->impress_server;
  } else {
    if (oldp->impress_server != NULL &&
	oldp->impress_server->addr != NULL) {
      free(oldp->impress_server->addr);
    }
    free(oldp->impress_server);
  }

  if (newp->rls_server == NULL && oldp->rls_server != NULL) {
    newp->rls_server = oldp->rls_server;
  } else {
    if (oldp->rls_server != NULL &&
	oldp->rls_server->addr != NULL) {
      free(oldp->rls_server->addr);
    }
    free(oldp->rls_server);
  }

  if (newp->policy_filter == NULL && oldp->policy_filter != NULL) {
    newp->policy_filter = oldp->policy_filter;
  } else {
    if (oldp->policy_filter != NULL &&
	oldp->policy_filter->addr != NULL) {
      free(oldp->policy_filter->addr);
    }
    free(oldp->policy_filter);
  }

  if (newp->static_route == NULL && oldp->static_route != NULL) {
    newp->static_route = oldp->static_route;
  } else {
    if (oldp->static_route != NULL &&
	oldp->static_route->addr != NULL) {
      free(oldp->static_route->addr);
    }
    free(oldp->static_route);
  }

  if (newp->nis_server == NULL && oldp->nis_server != NULL) {
    newp->nis_server = oldp->nis_server;
  } else {
    if (oldp->nis_server != NULL &&
	oldp->nis_server->addr != NULL) {
      free(oldp->nis_server->addr);
    }
    free(oldp->nis_server);
  }

  if (newp->ntp_server == NULL && oldp->ntp_server != NULL) {
    newp->ntp_server = oldp->ntp_server;
  } else {
    if (oldp->ntp_server != NULL &&
	oldp->ntp_server->addr != NULL) {
      free(oldp->ntp_server->addr);
    }
    free(oldp->ntp_server);
  }

  if (newp->nbn_server == NULL && oldp->nbn_server != NULL) {
    newp->nbn_server = oldp->nbn_server;
  } else {
    if (oldp->nbn_server != NULL &&
	oldp->nbn_server->addr != NULL) {
      free(oldp->nbn_server->addr);
    }
    free(oldp->nbn_server);
  }

  if (newp->nbdd_server == NULL && oldp->nbdd_server != NULL) {
    newp->nbdd_server = oldp->nbdd_server;
  } else {
    if (oldp->nbdd_server != NULL &&
	oldp->nbdd_server->addr != NULL) {
      free(oldp->nbdd_server->addr);
    }
    free(oldp->nbdd_server);
  }

  if (newp->xfont_server == NULL && oldp->xfont_server != NULL) {
    newp->xfont_server = oldp->xfont_server;
  } else {
    if (oldp->xfont_server != NULL &&
	oldp->xfont_server->addr != NULL) {
      free(oldp->xfont_server->addr);
    }
    free(oldp->xfont_server);
  }

  if (newp->xdisplay_manager == NULL && oldp->xdisplay_manager != NULL) {
    newp->xdisplay_manager = oldp->xdisplay_manager;
  } else {
    if (oldp->xdisplay_manager != NULL &&
	oldp->xdisplay_manager->addr != NULL) {
      free(oldp->xdisplay_manager->addr);
    }
    free(oldp->xdisplay_manager);
  }

  if (newp->nisp_server == NULL && oldp->nisp_server != NULL) {
    newp->nisp_server = oldp->nisp_server;
  } else {
    if (oldp->nisp_server != NULL &&
	oldp->nisp_server->addr != NULL) {
      free(oldp->nisp_server->addr);
    }
    free(oldp->nisp_server);
  }

  if (newp->mobileip_ha == NULL && oldp->mobileip_ha != NULL) {
    newp->mobileip_ha = oldp->mobileip_ha;
  } else {
    if (oldp->mobileip_ha != NULL &&
	oldp->mobileip_ha->addr != NULL) {
      free(oldp->mobileip_ha->addr);
    }
    free(oldp->mobileip_ha);
  }

  if (newp->smtp_server == NULL && oldp->smtp_server != NULL) {
    newp->smtp_server = oldp->smtp_server;
  } else {
    if (oldp->smtp_server != NULL &&
	oldp->smtp_server->addr != NULL) {
      free(oldp->smtp_server->addr);
    }
    free(oldp->smtp_server);
  }

  if (newp->pop3_server == NULL && oldp->pop3_server != NULL) {
    newp->pop3_server = oldp->pop3_server;
  } else {
    if (oldp->pop3_server != NULL &&
	oldp->pop3_server->addr != NULL) {
      free(oldp->pop3_server->addr);
    }
    free(oldp->pop3_server);
  }

  if (newp->nntp_server == NULL && oldp->nntp_server != NULL) {
    newp->nntp_server = oldp->nntp_server;
  } else {
    if (oldp->nntp_server != NULL &&
	oldp->nntp_server->addr != NULL) {
      free(oldp->nntp_server->addr);
    }
    free(oldp->nntp_server);
  }

  if (newp->dflt_www_server == NULL && oldp->dflt_www_server != NULL) {
    newp->dflt_www_server = oldp->dflt_www_server;
  } else {
    if (oldp->dflt_www_server != NULL &&
	oldp->dflt_www_server->addr != NULL) {
      free(oldp->dflt_www_server->addr);
    }
    free(oldp->dflt_www_server);
  }

  if (newp->dflt_finger_server == NULL && oldp->dflt_finger_server != NULL) {
    newp->dflt_finger_server = oldp->dflt_finger_server;
  } else {
    if (oldp->dflt_finger_server != NULL &&
	oldp->dflt_finger_server->addr != NULL) {
      free(oldp->dflt_finger_server->addr);
    }
    free(oldp->dflt_finger_server);
  }

  if (newp->dflt_irc_server == NULL && oldp->dflt_irc_server != NULL) {
    newp->dflt_irc_server = oldp->dflt_irc_server;
  } else {
    if (oldp->dflt_irc_server != NULL &&
	oldp->dflt_irc_server->addr != NULL) {
      free(oldp->dflt_irc_server->addr);
    }
    free(oldp->dflt_irc_server);
  }

  if (newp->streettalk_server == NULL && oldp->streettalk_server != NULL) {
    newp->streettalk_server = oldp->streettalk_server;
  } else {
    if (oldp->streettalk_server != NULL &&
	oldp->streettalk_server->addr != NULL) {
      free(oldp->streettalk_server->addr);
    }
    free(oldp->streettalk_server);
  }

  if (newp->stda_server == NULL && oldp->stda_server != NULL) {
    newp->stda_server = oldp->stda_server;
  } else {
    if (oldp->stda_server != NULL &&
	oldp->stda_server->addr != NULL) {
      free(oldp->stda_server->addr);
    }
    free(oldp->stda_server);
  }

  return(0);
}


/*
 * general initialization
 */
#ifdef sun
int
initialize()
{
  struct ifreq ifreq;
  struct strioctl si;
  register u_short *fwp = &dhcpf.Pf_Filter[0];
  u_Long tmp;

#ifndef LOG_CONS
#define LOG_CONS     0
#endif
#ifndef LOG_PERROR
#define LOG_PERROR   0
#endif
  openlog("dhcpc", LOG_PID | LOG_CONS | LOG_PERROR, LOG_LOCAL0);

  srandom(generate_xid());

  /* reset interface */
  reset_if(&intface);

  /* get default service */
  set_srvport();

  /* open bpf to read/write dhcp messages */
  dhcpif.fd = open("/dev/nit", O_RDWR);
  if (dhcpif.fd < 0) {
    syslog(LOG_WARNING, "Can't open nit to read the dhcp messages: %m");
    return(-1);
  }
  arpif.fd = open("/dev/nit", O_RDWR);
  if (arpif.fd < 0) {
    syslog(LOG_WARNING, "Can't open nit to read the arp messages: %m");
    return(-1);
  }

  if (ioctl(dhcpif.fd, I_SRDOPT, (char *)RMSGD) < 0) {
    syslog(LOG_WARNING, "ioctl(I_SRDOPT) error in initialize: %m");
    return(-1);
  }
  if (ioctl(arpif.fd, I_SRDOPT, (char *)RMSGD) < 0) {
    syslog(LOG_WARNING, "ioctl(I_SRDOPT) error in initialize: %m");
    return(-1);
  }

  bzero(&si, sizeof(si));
  bzero(&ifreq, sizeof(ifreq));
  strncpy(ifreq.ifr_name, intface.name, sizeof(ifreq.ifr_name));
  ifreq.ifr_name[sizeof(ifreq.ifr_name) - 1] = '\0';
  si.ic_cmd = NIOCBIND;
  si.ic_len = sizeof(ifreq);
  si.ic_dp = (char *) &ifreq;
  if (ioctl(dhcpif.fd, I_STR, (char *) &si) < 0) {
    syslog(LOG_WARNING, "ioctl(NIOCBIND) error in initialize: %m");
    return(-1);
  }
  bzero(&si, sizeof(si));
  bzero(&ifreq, sizeof(ifreq));
  strncpy(ifreq.ifr_name, intface.name, sizeof(ifreq.ifr_name));
  ifreq.ifr_name[sizeof(ifreq.ifr_name) - 1] = '\0';
  si.ic_cmd = NIOCBIND;
  si.ic_len = sizeof(ifreq);
  si.ic_dp = (char *) &ifreq;
  if (ioctl(arpif.fd, I_STR, (char *) &si) < 0) {
    syslog(LOG_WARNING, "ioctl(NIOCBIND) error in initialize: %m");
    return(-1);
  }

  /* set packet filter */
  if (ioctl(arpif.fd, I_PUSH, "pf") < 0) {
    syslog(LOG_WARNING, "ioctl(I_PUSH) error in initialize: %m");
    exit(-1);
  }

  /* set packet filter */
  if (ioctl(dhcpif.fd, I_PUSH, "pf") < 0) {
    syslog(LOG_WARNING, "ioctl(I_PUSH) error in initialize: %m");
    exit(-1);
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
  *fwp++ = ENF_PUSHLIT | ENF_CAND;
  *fwp++ = dhcpc_port;
#endif
  dhcpf.Pf_FilterLen = fwp - &dhcpf.Pf_Filter[0];

  bzero(&si, sizeof(si));
  si.ic_cmd = NIOCSETF;
  si.ic_timout = INFTIM;
  si.ic_len = sizeof(dhcpf);
  si.ic_dp = (char *)&dhcpf;
  if (ioctl(dhcpif.fd, I_STR, (char *)&si) < 0){
    syslog(LOG_WARNING, "ioctl(NIOCSETF) error in initialize: %m");
    return(-1);
  }

  bzero(&si, sizeof(si));
  tmp = 0;
  si.ic_cmd = NIOCSSNAP;
  si.ic_timout = INFTIM;
  si.ic_len = sizeof(tmp);
  si.ic_dp = (char *) &tmp;
  if (ioctl(dhcpif.fd, I_STR, (char *)&si) < 0){
    syslog(LOG_WARNING, "ioctl(NIOCSETF) error in initialize: %m");
    return(-1);
  }
  bzero(&si, sizeof(si));
  tmp = 0;
  si.ic_cmd = NIOCSSNAP;
  si.ic_timout = INFTIM;
  si.ic_len = sizeof(tmp);
  si.ic_dp = (char *) &tmp;
  if (ioctl(arpif.fd, I_STR, (char *)&si) < 0){
    syslog(LOG_WARNING, "ioctl(NIOCSETF) error in initialize: %m");
    return(-1);
  }

  dhcpif.rbufsize = DFLTDHCPLEN + UDPHL + IPHL + ETHERHL + QWOFF;
  if ((dhcpif.rbuf = (char *) calloc(1, dhcpif.rbufsize)) == NULL) {
    syslog(LOG_WARNING, "calloc() error for rbuf in initialize: %m");
    return(-1);
  }
  arpif.rbufsize = sizeof(struct ether_arp) + ETHERHL + QWOFF;
  if ((arpif.rbuf = (char *) calloc(1, arpif.rbufsize)) == NULL) {
    syslog(LOG_WARNING, "calloc() error for rbuf in initialize: %m");
    return(-1);
  }

  sbuf.size = ETHERHL + IPHL + UDPHL + DFLTDHCPLEN;
  if ((sbuf.buf = (char *) calloc(1, sbuf.size + QWOFF)) == NULL) {
    syslog(LOG_WARNING, "calloc() error for sbuf in initialize: %m");
    return(-1);
  }
  snd.ether = (struct ether_header *) &sbuf.buf[QWOFF];
  snd.ip = (struct ip *) &sbuf.buf[QWOFF + ETHERHL];
  snd.udp = (struct udphdr *) &sbuf.buf[QWOFF + ETHERHL + IPHL];
  snd.dhcp = (struct dhcp *) &sbuf.buf[QWOFF + ETHERHL + IPHL + UDPHL];

  dhcpif.ifinfo = &intface;
  arpif.ifinfo = &intface;

  /* get haddr of interface */
  intface.haddr.htype = ETHER;
  intface.haddr.hlen = 6;
  if (getmac(intface.name, intface.haddr.haddr) < 0) {
    return(-1);
  }

  return(0);
}

#else /* sun */

int
initialize()
{
  int n = 0;
  char dev[sizeof("/dev/pf/pfiltxx")];
  struct ifreq ifr;
  struct timeval timeout;
#ifdef __osf__
  u_short  bits;
  struct endevp endvp;

  bzero(&endvp, sizeof(endvp));
#endif

  bzero(dev, sizeof(dev));
  bzero(&timeout, sizeof(timeout));

#ifndef LOG_CONS
#define LOG_CONS     0
#endif
#ifndef LOG_PERROR
#define LOG_PERROR   0
#endif
  openlog("dhcpc", LOG_PID | LOG_CONS | LOG_PERROR, LOG_LOCAL0);

  srandom(generate_xid());

  /* reset interface */
  reset_if(&intface);

  /* get default service */
  set_srvport();

  /* open bpf to read/write dhcp messages */
  do {
#ifdef __osf__
    sprintf(dev, "/dev/pf/pfilt%d", n++);
#else
    sprintf(dev, "/dev/bpf%d", n++);
#endif
    dhcpif.fd = open(dev, O_RDWR);
  } while (dhcpif.fd < 0 && n < NBPFILTER);
  if (dhcpif.fd < 0) {
    syslog(LOG_WARNING, "Can't open bpf to read the dhcp messages: %m");
    return(-1);
  }

  /* open bpf to read/write arp messages */
  n = 0;
  do {
#ifdef __osf__
    sprintf(dev, "/dev/pf/pfilt%d", n++);
#else
    sprintf(dev, "/dev/bpf%d", n++);
#endif
    arpif.fd = open(dev, O_RDWR);
  } while (arpif.fd < 0 && n < NBPFILTER);
  if (arpif.fd < 0) {
    syslog(LOG_WARNING, "Can't open bpf to read the arp messages: %m");
    return(-1);
  }

#ifdef __osf__
  /*
   * make packetfilt use BPF header
   */
  bits = ENBPFHDR;
  if (ioctl(dhcpif.fd, EIOCMBIS, &bits) < 0) {
    syslog(LOG_ERR, "ioctl(EIOCMBIS) for dhcp: %m");
    exit(1);
  }
  if (ioctl(arpif.fd, EIOCMBIS, &bits) < 0) {
    syslog(LOG_ERR, "ioctl(EIOCMBIS) for arp: %m");
    exit(1);
  }
#endif

  /* set immediate mode */
  n = 1;
  if (ioctl(dhcpif.fd, BIOCIMMEDIATE, &n) < 0) {
    syslog(LOG_WARNING, "ioctl(BIOCIMMEDIATE) for dhcp: %m");
    return(-1);
  }
  if (ioctl(arpif.fd, BIOCIMMEDIATE, &n) < 0) {
    syslog(LOG_WARNING, "ioctl(BIOCIMMEDIATE) for arp: %m");
    return(-1);
  }

  /* bind interface to the bpfs */
  bzero(&ifr, sizeof(ifr));
  strcpy(ifr.ifr_name, intface.name);
  if (ioctl(dhcpif.fd, BIOCSETIF, &ifr) < 0) {
    syslog(LOG_WARNING, "ioctl(BIOCSETIF) for dhcp: %m");
    return(-1);
  }
  if (ioctl(arpif.fd, BIOCSETIF, &ifr) < 0) {
    syslog(LOG_WARNING, "ioctl(BIOCSETIF) for arp: %m");
    return(-1);
  }

  /* get buffer length for read and allocate buffer actually */
  if (ioctl(dhcpif.fd, BIOCGBLEN, &dhcpif.rbufsize) < 0) {
    syslog(LOG_WARNING, "ioctl(BIOCGBLEN) for dhcp: %m");
    return(-1);
  }
  if ((dhcpif.rbuf = (char *) calloc(1, dhcpif.rbufsize)) == NULL) {
    syslog(LOG_WARNING, "calloc() error for rbuf in initialize: %m");
    return(-1);
  }
  if (ioctl(arpif.fd, BIOCGBLEN, &arpif.rbufsize) < 0) {
    syslog(LOG_WARNING, "ioctl(BIOCGBLEN) for arp: %m");
    return(-1);
  }
  if ((arpif.rbuf = (char *) calloc(1, arpif.rbufsize)) == NULL) {
    syslog(LOG_WARNING, "calloc() error for rbuf in initialize: %m");
    return(-1);
  }

  dhcpf[DSTPORTIS].k = ntohs(dhcpc_port);

  /* bind filter program to the interfaces */
  if (ioctl(dhcpif.fd, BIOCSETF, &dhcpfilter) < 0) {
    syslog(LOG_WARNING, "ioctl(BIOCSETF) for dhcp: %m");
    return(-1);
  }

  /* set timeout */
  timeout.tv_sec = 0;
  timeout.tv_usec = 500000;
  if (ioctl(arpif.fd, BIOCSRTIMEOUT, &timeout) < 0) {
    syslog(LOG_WARNING, "ioctl(BIOCSRTIMEOUT) for arp: %m");
    return(-1);
  }

  sbuf.size = ETHERHL + IPHL + UDPHL + DFLTDHCPLEN;
  if ((sbuf.buf = (char *) calloc(1, sbuf.size + QWOFF)) == NULL) {
    syslog(LOG_WARNING, "calloc() error for sbuf in initialize: %m");
    return(-1);
  }
  snd.ether = (struct ether_header *) &sbuf.buf[QWOFF];
  snd.ip = (struct ip *) &sbuf.buf[QWOFF + ETHERHL];
  snd.udp = (struct udphdr *) &sbuf.buf[QWOFF + ETHERHL + IPHL];
  snd.dhcp = (struct dhcp *) &sbuf.buf[QWOFF + ETHERHL + IPHL + UDPHL];

  dhcpif.ifinfo = &intface;
  arpif.ifinfo = &intface;

  /* get haddr of interface */
  intface.haddr.htype = ETHER;
  intface.haddr.hlen = 6;
#if defined(sony_news) || defined(__FreeBSD__)
  if (ioctl(dhcpif.fd, SIOCGIFADDR, &ifr) < 0) {
    syslog(LOG_WARNING, "ioctl(SIOCGIFADDR) error in initialize(): %m");
    return(-1);
  }
  bcopy(ifr.ifr_addr.sa_data, intface.haddr.haddr, 6);
#else
#ifdef __osf__
  if (ioctl(dhcpif.fd, EIOCDEVP, &endvp) < 0) {
    syslog(LOG_ERR, "ioctl(EIOCDEVP) error in initialize()");
    exit(1);
  }
  bcopy(endvp.end_addr, intface.haddr.haddr, 6);
#else
  if (getmac(intface.name, intface.haddr.haddr) < 0) {
    return(-1);
  }
#endif
#endif

  return(0);
}
#endif /* sun */



/*
 * halt network, and reset the interface
 */
void
reset_if(ifp)
  struct if_info *ifp;
{
  struct in_addr addr, mask, brdaddr;

  addr.s_addr = htonl((generate_xid() & 0xfff) | 0x0a000000);
  mask.s_addr = htonl(0xff000000);
  brdaddr.s_addr = inet_addr("255.255.255.255");

#ifndef DEBUG
  config_if(ifp, &addr, &mask, &brdaddr);
  flushroutes();
#endif

  return;
}


#if defined(__bsdi__) || defined(__FreeBSD__)
/*
 * ifconfig down
 */
void
down_if(ifp)
  struct if_info *ifp;
{
  int sockfd;
  struct ifreq ridreq;

  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    return;
  }

  bzero(&ridreq, sizeof(struct ifreq));
  strncpy(ridreq.ifr_name, ifp->name, sizeof(ridreq.ifr_name));

  /* down interface */
  ioctl(sockfd, SIOCGIFFLAGS, (caddr_t) &ridreq);
  ridreq.ifr_flags &= (~IFF_UP);
  ioctl(sockfd, SIOCSIFFLAGS, (caddr_t) &ridreq);

  /* delete address */
  ioctl(sockfd, SIOCDIFADDR, (caddr_t) &ridreq);

  flushroutes();
  close(sockfd);

  return;
}
#else
void
down_if(ifp)
  struct if_info *ifp;
{
  int sockfd;
  struct ifreq ridreq;

  reset_if(ifp);

  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    return;
  }

  bzero(&ridreq, sizeof(struct ifreq));
  strncpy(ridreq.ifr_name, ifp->name, sizeof(ridreq.ifr_name));

  /* down interface */
  ioctl(sockfd, SIOCGIFFLAGS, (caddr_t) &ridreq);
  ridreq.ifr_flags &= (~IFF_UP);
  ioctl(sockfd, SIOCSIFFLAGS, (caddr_t) &ridreq);

  close(sockfd);

  return;
}
#endif


/*
 * configure network interface
 *    address, netmask, and broadcast address
 */
#if defined(__bsdi__) || defined(__FreeBSD__)

int
config_if(ifp, addr, mask, brdcst)
  struct if_info *ifp;
  struct in_addr *addr;
  struct in_addr *mask;
  struct in_addr *brdcst;
{
  int sockfd;
  struct ifreq ridreq;
  struct ifaliasreq addreq;
  struct in_addr current_addr, current_mask, current_brdcst;
#define SIN(x) ((struct sockaddr_in *) &(x))

  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    return(-1);
  }

  bzero(&current_addr, sizeof(current_addr));
  bzero(&current_mask, sizeof(current_mask));
  bzero(&current_brdcst, sizeof(current_brdcst));
  bzero(&ridreq, sizeof(struct ifreq));
  strncpy(ridreq.ifr_name, ifp->name, sizeof(ridreq.ifr_name));

  ioctl(sockfd, SIOCGIFADDR, &ridreq);
  current_addr.s_addr =
    ((struct sockaddr_in *)&ridreq.ifr_addr)->sin_addr.s_addr;
  ioctl(sockfd, SIOCGIFNETMASK, &ridreq);
  current_mask.s_addr =
    ((struct sockaddr_in *)&ridreq.ifr_addr)->sin_addr.s_addr;
  ioctl(sockfd, SIOCGIFBRDADDR, &ridreq);
  current_brdcst.s_addr =
    ((struct sockaddr_in *)&ridreq.ifr_addr)->sin_addr.s_addr;

  if (current_addr.s_addr == addr->s_addr &&
      current_mask.s_addr == mask->s_addr &&
      current_brdcst.s_addr == brdcst->s_addr) {
    close(sockfd);
    return(1);
  }

  bzero(&ridreq, sizeof(struct ifreq));
  strncpy(ridreq.ifr_name, ifp->name, sizeof(ridreq.ifr_name));
  ioctl(sockfd, SIOCDIFADDR, (caddr_t) &ridreq);
  flushroutes();

  bzero(&addreq, sizeof(struct ifaliasreq));
  strncpy(addreq.ifra_name, ifp->name, sizeof(ridreq.ifr_name));
  SIN(addreq.ifra_addr)->sin_len = sizeof(struct sockaddr_in);
  SIN(addreq.ifra_addr)->sin_family = AF_INET;
  if (addr) {
    SIN(addreq.ifra_addr)->sin_addr.s_addr = addr->s_addr;
  }
  if (mask) {
    SIN(addreq.ifra_mask)->sin_len = sizeof(struct sockaddr_in);
    SIN(addreq.ifra_mask)->sin_addr.s_addr = mask->s_addr;
  }
  if (brdcst) {
    SIN(addreq.ifra_broadaddr)->sin_len = sizeof(struct sockaddr_in);
    SIN(addreq.ifra_broadaddr)->sin_family = AF_INET;
    SIN(addreq.ifra_broadaddr)->sin_addr.s_addr = brdcst->s_addr;
  }
  else if (addr && mask) {
    SIN(addreq.ifra_broadaddr)->sin_len = sizeof(struct sockaddr_in);
    SIN(addreq.ifra_broadaddr)->sin_family = AF_INET;
    SIN(addreq.ifra_broadaddr)->sin_addr.s_addr = addr->s_addr | ~mask->s_addr;
  }

  if (ioctl(sockfd, SIOCAIFADDR, (caddr_t) &addreq) < 0) {
    perror("SIOCAIFADDR");
    close(sockfd);
    return(-1);
  }

  close(sockfd);
  return(0);
}

#else /* not __bsdi__ nor __FreeBSD__ */

int
config_if(ifp, addr, mask, brdcst)
  struct if_info *ifp;
  struct in_addr *addr;
  struct in_addr *mask;
  struct in_addr *brdcst;
{
  int sockfd = 0;
  struct ifreq ifr;
  struct in_addr current_addr, current_mask, current_brdcst;

  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    return(-1);
  }

  bzero(&current_addr, sizeof(current_addr));
  bzero(&current_mask, sizeof(current_mask));
  bzero(&current_brdcst, sizeof(current_brdcst));
  bzero(&ifr, sizeof(struct ifreq));
  strcpy(ifr.ifr_name, ifp->name);

  ioctl(sockfd, SIOCGIFADDR, &ifr);
  current_addr.s_addr =
    ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr;
  ioctl(sockfd, SIOCGIFNETMASK, &ifr);
  current_mask.s_addr =
    ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr;
  ioctl(sockfd, SIOCGIFBRDADDR, &ifr);
  current_brdcst.s_addr =
    ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr;

  if (current_addr.s_addr == addr->s_addr &&
      current_mask.s_addr == mask->s_addr &&
      current_brdcst.s_addr == brdcst->s_addr) {
    close(sockfd);
    return(1);
  }

  flushroutes();
  if (mask != NULL) {
    bzero(&ifr, sizeof(struct ifreq));
    strcpy(ifr.ifr_name, ifp->name);
    ((struct sockaddr_in *) &ifr.ifr_addr)->sin_family = AF_INET;
    ((struct sockaddr_in *) &ifr.ifr_addr)->sin_addr.s_addr = mask->s_addr;
    if (ioctl(sockfd, SIOCSIFNETMASK, &ifr) < 0) {
      close(sockfd);
      return(-1);
    }
  }
  if (addr != NULL) {
    bzero(&ifr, sizeof(struct ifreq));
    strcpy(ifr.ifr_name, ifp->name);
    ((struct sockaddr_in *) &ifr.ifr_addr)->sin_family = AF_INET;
    ((struct sockaddr_in *) &ifr.ifr_addr)->sin_addr.s_addr = addr->s_addr;
    if (ioctl(sockfd, SIOCSIFADDR, &ifr) < 0) {
      close(sockfd);
      return(-1);
    }
  }
  if (brdcst != NULL) {
    bzero(&ifr, sizeof(struct ifreq));
    strcpy(ifr.ifr_name, ifp->name);
    ((struct sockaddr_in *) &ifr.ifr_broadaddr)->sin_family = AF_INET;
    ((struct sockaddr_in *) &ifr.ifr_broadaddr)->sin_addr.s_addr = brdcst->s_addr;
    if (ioctl(sockfd, SIOCSIFBRDADDR, &ifr) < 0) {
      close(sockfd);
      return(-1);
    }
  }
  close(sockfd);

  return(0);
}

#endif /* defined(__bsdi__) || defined(__FreeBSD__) */


/*
 * set routing table
 */
#ifndef BSDOS
void
set_route(param)
  struct dhcp_param *param;
{
  int sockfd = 0;
#if !defined(__bsdi__) && !defined(__FreeBSD__) && !defined(__osf__)
#define  ortentry  rtentry
#endif
  struct ortentry rt;
  struct sockaddr dst, gateway;

  if (param == NULL)
    return;

  sockfd = socket(AF_INET, SOCK_RAW, 0);
  if (sockfd < 0) {
    syslog(LOG_WARNING, "socket() error in set_route(): %m");
    return;
  }

  /*
   * set default route
   */
  if (ISSET(param->got_option, ROUTER) &&
      param->router != NULL && param->router->addr != NULL) {
    bzero(&rt, sizeof(struct ortentry));
    bzero(&dst, sizeof(struct sockaddr));
    bzero(&gateway, sizeof(struct sockaddr));
    rt.rt_flags = RTF_UP | RTF_GATEWAY;
#if defined(__bsdi__) || defined(__FreeBSD__)
    ((struct sockaddr_in *) &dst)->sin_len = sizeof(struct sockaddr_in);
#endif
    ((struct sockaddr_in *) &dst)->sin_family = AF_INET;
    ((struct sockaddr_in *) &dst)->sin_addr.s_addr = INADDR_ANY;
#if defined(__bsdi__) || defined(__FreeBSD__)
    ((struct sockaddr_in *) &gateway)->sin_len = sizeof(struct sockaddr_in);
#endif
    ((struct sockaddr_in *) &gateway)->sin_family = AF_INET;
    ((struct sockaddr_in *) &gateway)->sin_addr.s_addr =
      param->router->addr->s_addr;
    rt.rt_dst = dst;
    rt.rt_gateway = gateway;
    if (ioctl(sockfd, SIOCADDRT, &rt) < 0) {
      syslog(LOG_WARNING, "SIOCADDRT (default route): %m");
      close(sockfd);
    }
  }

  close(sockfd);
  return;
}

#else  /* It's BSD/OS 2.0 or later */

void
set_route(param)
  struct dhcp_param *param;
{
  static int seq;
  int s;
  int rtm_addrs = RTA_DST | RTA_GATEWAY | RTA_NETMASK;
  struct sockaddr so_dst, so_gate, so_mask;
  struct {
    struct rt_msghdr m_rtm;
    char   m_space[512];
  } m_rtmsg;
  register char *cp = m_rtmsg.m_space;
  register int l;

  bzero(&so_dst, sizeof(struct sockaddr));
  bzero(&so_mask, sizeof(struct sockaddr));
  bzero(&so_gate, sizeof(struct sockaddr));
  bzero(&m_rtmsg, sizeof(m_rtmsg));

  s = socket(PF_ROUTE, SOCK_RAW, 0);
  if (s < 0) {
    syslog(LOG_WARNING, "socket() error in set_route(): %m");
    return;
  }

  so_dst.sa_family = so_mask.sa_family = so_gate.sa_family = AF_INET;
  so_dst.sa_len = so_gate.sa_len = sizeof(struct sockaddr_in);
  so_mask.sa_len = 0;
  ((struct sockaddr_in *)&so_gate)->sin_addr.s_addr =
    param->router->addr->s_addr;

#define rtmsg m_rtmsg.m_rtm
  rtmsg.rtm_type = RTM_ADD;
  rtmsg.rtm_flags = RTF_STATIC | RTF_UP | RTF_GATEWAY;
  rtmsg.rtm_version = RTM_VERSION;
  rtmsg.rtm_seq = ++seq;
  rtmsg.rtm_addrs = rtm_addrs;
  bzero(&(rtmsg.rtm_rmx), sizeof(struct rt_metrics));
  rtmsg.rtm_inits = 0;

#define ROUNDUP(a) \
  ((a) > 0 ? (1 + (((a) - 1) | (sizeof(long) - 1))) : sizeof(long))
#define NEXTADDR(u) \
  l = ROUNDUP(u.sa_len); bcopy((char *)&(u), cp, l); cp += l;\

  NEXTADDR(so_dst);
  NEXTADDR(so_gate);
  NEXTADDR(so_mask);
  rtmsg.rtm_msglen = l = cp - (char *)&m_rtmsg;

  write(s, (char *)&m_rtmsg, l);
  close(s);
}

#endif


void
make_decline(lsbuf, reqspecp)
  char *lsbuf;
  struct dhcp_reqspec *reqspecp;
{
  int offopt = 0;    /* offset in options field */
  int i = 0;
  u_Long tmpul = 0;
  struct dhcp    *lsdhcp = NULL;
  struct udphdr  *lsudp = NULL;
  struct ip      *lsip = NULL;
  struct ether_header *lsether = NULL;
  struct ps_udph pudph;

  bzero(&pudph, sizeof(pudph));

  lsether = (struct ether_header *) &lsbuf[QWOFF];
  lsip = (struct ip *) &lsbuf[QWOFF + ETHERHL];
  lsudp = (struct udphdr *) &lsbuf[QWOFF + ETHERHL + IPHL];
  lsdhcp = (struct dhcp *) &lsbuf[QWOFF + ETHERHL + IPHL + UDPHL];

  /*
   * construct dhcp part
   */
  lsdhcp->op = BOOTREQUEST;
  lsdhcp->htype = intface.haddr.htype;
  lsdhcp->hlen = intface.haddr.hlen;
  lsdhcp->xid = generate_xid();
  lsdhcp->giaddr = rcv.dhcp->giaddr;
  bcopy(intface.haddr.haddr, lsdhcp->chaddr, lsdhcp->hlen);

  /* insert magic cookie */
  bcopy(magic_c, lsdhcp->options, MAGIC_LEN);
  offopt = MAGIC_LEN;

  /* insert message type */
  lsdhcp->options[offopt++] = DHCP_MSGTYPE;
  lsdhcp->options[offopt++] = 1;
  lsdhcp->options[offopt++] = DHCPDECLINE;

  /* insert requested IP */
  if (reqspecp->ipaddr.s_addr == 0) {
    return;
  } else if (CHKOFF(4)) {
    lsdhcp->options[offopt++] = REQUEST_IPADDR;
    lsdhcp->options[offopt++] = 4;
    bcopy(&reqspecp->ipaddr, &lsdhcp->options[offopt], 4);
    offopt += 4;
  }

  /* insert client identifier */
  if (reqspecp->clid != NULL && CHKOFF(reqspecp->clid->len + 1)) {
    lsdhcp->options[offopt++] = CLIENT_ID;
    lsdhcp->options[offopt++] = reqspecp->clid->len + 1;
    lsdhcp->options[offopt++] = reqspecp->clid->type;
    bcopy(reqspecp->clid->id, &lsdhcp->options[offopt], reqspecp->clid->len);
    offopt += reqspecp->clid->len;
  }

  /* insert server identifier */
  if (CHKOFF(4)) {
    lsdhcp->options[offopt++] = SERVER_ID;
    lsdhcp->options[offopt++] = 4;
    bcopy(&reqspecp->srvaddr, &lsdhcp->options[offopt], 4);
    offopt += 4;
  }

  /* if necessary, insert error message */
  if (reqspecp->dhcp_errmsg != NULL && CHKOFF(strlen(reqspecp->dhcp_errmsg))) {
    lsdhcp->options[offopt++] = DHCP_ERRMSG;
    lsdhcp->options[offopt++] = strlen(reqspecp->dhcp_errmsg);
    bcopy(reqspecp->dhcp_errmsg, &lsdhcp->options[offopt],
	  strlen(reqspecp->dhcp_errmsg));
    offopt += strlen(reqspecp->dhcp_errmsg);
  }
  lsdhcp->options[offopt] = END;

  /*
   * construct udp part
   */
  lsudp->uh_sport = dhcpc_port;
  lsudp->uh_dport = dhcps_port;
  lsudp->uh_ulen = htons(DFLTDHCPLEN + UDPHL);
  lsudp->uh_sum = 0;

  /* fill pseudo udp header */
  pudph.srcip.s_addr = 0;
  pudph.dstip.s_addr = reqspecp->srvaddr.s_addr;
  pudph.zero = 0;
  pudph.prto = IPPROTO_UDP;
  pudph.ulen = lsudp->uh_ulen;
  lsudp->uh_sum = udp_cksum(&pudph, (char *) lsudp, ntohs(pudph.ulen));

  /*
   * construct ip part
   */
  lsip->ip_v = IPVERSION;
  lsip->ip_hl = IPHL >> 2;
  lsip->ip_tos = 0;
  lsip->ip_len = htons(DFLTDHCPLEN + UDPHL + IPHL);
  tmpul = generate_xid();
  tmpul += (tmpul >> 16);
  lsip->ip_id = (u_short) (~tmpul);
  lsip->ip_off = htons(IP_DF);                         /* XXX */
  lsip->ip_ttl = 0x20;                                 /* XXX */
  lsip->ip_p = IPPROTO_UDP;
  lsip->ip_src.s_addr = 0;
  lsip->ip_dst.s_addr = rcv.ip->ip_src.s_addr;
  lsip->ip_sum = 0;
  lsip->ip_sum = cksum((u_short *)lsip, lsip->ip_hl * 2);

  /*
   * construct ether part
   */
  for (i = 0; i < 6; i++) {
#ifdef sun
    lsether->ether_dhost.ether_addr_octet[i] =
      rcv.ether->ether_shost.ether_addr_octet[i];
    lsether->ether_shost.ether_addr_octet[i] = intface.haddr.haddr[i];
#else
    lsether->ether_dhost[i] = rcv.ether->ether_shost[i];
    lsether->ether_shost[i] = intface.haddr.haddr[i];
#endif
  }
  lsether->ether_type = htons(ETHERTYPE_IP);

  return;
}


void
make_release(lsdhcp, reqspecp)
  struct dhcp *lsdhcp;
  struct dhcp_reqspec *reqspecp;
{
  int offopt = 0;    /* offset in options field */

  /*
   * construct dhcp part
   */
  lsdhcp->op = BOOTREQUEST;
  lsdhcp->htype = intface.haddr.htype;
  lsdhcp->hlen = intface.haddr.hlen;
  lsdhcp->xid = generate_xid();
  lsdhcp->ciaddr = reqspecp->ipaddr;
  bcopy(intface.haddr.haddr, lsdhcp->chaddr, lsdhcp->hlen);

  /* insert magic cookie */
  bcopy(magic_c, lsdhcp->options, MAGIC_LEN);
  offopt = MAGIC_LEN;

  /* insert message type */
  lsdhcp->options[offopt++] = DHCP_MSGTYPE;
  lsdhcp->options[offopt++] = 1;
  lsdhcp->options[offopt++] = DHCPRELEASE;

  /* insert server identifier */
  if (CHKOFF(4)) {
    lsdhcp->options[offopt++] = SERVER_ID;
    lsdhcp->options[offopt++] = 4;
    bcopy(&reqspecp->srvaddr, &lsdhcp->options[offopt], 4);
    offopt += 4;
  }

  /* insert client identifier */
  if (reqspecp->clid != NULL && CHKOFF(reqspecp->clid->len + 1)) {
    lsdhcp->options[offopt++] = CLIENT_ID;
    lsdhcp->options[offopt++] = reqspecp->clid->len + 1;
    lsdhcp->options[offopt++] = reqspecp->clid->type;
    bcopy(reqspecp->clid->id, &lsdhcp->options[offopt], reqspecp->clid->len);
    offopt += reqspecp->clid->len;
  }

  /* if necessary, insert error message */
  if (reqspecp->dhcp_errmsg != NULL && CHKOFF(strlen(reqspecp->dhcp_errmsg))) {
    lsdhcp->options[offopt++] = DHCP_ERRMSG;
    lsdhcp->options[offopt++] = strlen(reqspecp->dhcp_errmsg);
    bcopy(reqspecp->dhcp_errmsg, &lsdhcp->options[offopt],
	  strlen(reqspecp->dhcp_errmsg));
    offopt += strlen(reqspecp->dhcp_errmsg);
  }
  lsdhcp->options[offopt] = END;

  return;
}


/*
 * send DHCPDECLINE
 */
int
dhcp_decline(reqspecp)
  struct dhcp_reqspec *reqspecp;
{
  int  lsbufsize = ETHERHL + IPHL + UDPHL + DFLTDHCPLEN;
  char lsbuf[ETHERHL + IPHL + UDPHL + DFLTDHCPLEN + QWOFF];

  if (reqspecp->srvaddr.s_addr == 0)
    return(-1);

  /* allocate clean memory for send DHCP message */
  bzero(lsbuf, lsbufsize + QWOFF);
  make_decline(lsbuf, reqspecp);

  if (ether_write(dhcpif.fd, &lsbuf[QWOFF], lsbufsize) < 0) {
    return(-1);
  }

  errno = 0;
  syslog(LOG_INFO, "send DHCPDECLINE(%s)", inet_ntoa(reqspecp->ipaddr));
  return(0);
}


/*
 * send DHCPRELEASE
 */
int
dhcp_release(reqspecp)
  struct dhcp_reqspec *reqspecp;
{
  int  lsbufsize = DFLTDHCPLEN;
  char lsbuf[DFLTDHCPLEN];
  struct dhcp *lsdhcp = NULL;

  bzero(lsbuf, lsbufsize);

  lsdhcp = (struct dhcp *) lsbuf;
  if (reqspecp->srvaddr.s_addr == 0)
    return(-1);

  /* send DHCP message */
  make_release(lsdhcp, reqspecp);

  if (send_unicast(&reqspecp->srvaddr, lsdhcp) < 0) {
    return(-1);
  }

  errno = 0;
  syslog(LOG_INFO, "send DHCPRELEASE(%s)", inet_ntoa(reqspecp->ipaddr));

  down_if(&intface);
  return(0);
}


void
set_declinfo(reqspecp, paramp, errmsg, arpans)
  struct dhcp_reqspec *reqspecp;
  struct dhcp_param *paramp;
  char *errmsg;
  int arpans;
{
  reqspecp->ipaddr = paramp->yiaddr;
  reqspecp->srvaddr = paramp->server_id;
  if (reqspec.clid != NULL)
    reqspecp->clid = reqspec.clid;
  else
    reqspecp->clid = NULL;
  if (errmsg[0] == 0) {
    if (arpans != OK) {
      sprintf(errmsg, "Such IP address (%s) is already in use",
	      inet_ntoa(paramp->yiaddr));
    }
    else {
      sprintf(errmsg, "Such IP address (%s) isn't preferable",
	      inet_ntoa(paramp->yiaddr));
    }
  }
  reqspecp->dhcp_errmsg = errmsg;

  return;
}


void
set_relinfo(reqspecp, paramp, errmsg)
  struct dhcp_reqspec *reqspecp;
  struct dhcp_param *paramp;
  char *errmsg;
{
  reqspecp->ipaddr.s_addr = paramp->yiaddr.s_addr;
  reqspecp->srvaddr.s_addr = paramp->server_id.s_addr;
  if (reqspec.clid != NULL)
    reqspecp->clid = reqspec.clid;
  else
    reqspecp->clid = NULL;
  sprintf(errmsg, "I'm no longer need the IP address (%s)",
	  inet_ntoa(paramp->yiaddr));
  reqspecp->dhcp_errmsg = errmsg;

  return;
}


/*
 * construct DHCPDISCOVER
 */
void
make_discover()
{
  int i = 0;
  int offopt = 0;                   /* offset in options field */
  u_Long tmpul = 0;
  u_short tmpus = 0;

  /*
   * construct dhcp part
   */
  bzero(sbuf.buf, sbuf.size + QWOFF);
  snd.dhcp->op = BOOTREQUEST;
  snd.dhcp->htype = intface.haddr.htype;
  snd.dhcp->hlen = intface.haddr.hlen;
  snd.dhcp->xid = generate_xid();
  bcopy(intface.haddr.haddr, snd.dhcp->chaddr, snd.dhcp->hlen);

  /* insert magic cookie */
  bcopy(magic_c, snd.dhcp->options, MAGIC_LEN);
  offopt = MAGIC_LEN;

  /* insert message type */
  snd.dhcp->options[offopt++] = DHCP_MSGTYPE;
  snd.dhcp->options[offopt++] = 1;
  snd.dhcp->options[offopt++] = DHCPDISCOVER;

  /* insert class identifier */
  if (reqspec.class != NULL && CHKOFF(reqspec.class->len)) {
    snd.dhcp->options[offopt++] = CLASS_ID;
    snd.dhcp->options[offopt++] = reqspec.class->len;
    bcopy(reqspec.class->id, &snd.dhcp->options[offopt], reqspec.class->len);
    offopt += reqspec.class->len;
  }

  /* insert client identifier */
  if (reqspec.clid != NULL && CHKOFF(reqspec.clid->len + 1)) {
    snd.dhcp->options[offopt++] = CLIENT_ID;
    snd.dhcp->options[offopt++] = reqspec.clid->len + 1;
    snd.dhcp->options[offopt++] = reqspec.clid->type;
    bcopy(reqspec.clid->id, &snd.dhcp->options[offopt], reqspec.clid->len);
    offopt += reqspec.clid->len;
  }

  /* insert requesting lease */
  if (reqspec.lease != 0 && CHKOFF(4)) {
    snd.dhcp->options[offopt++] = LEASE_TIME;
    snd.dhcp->options[offopt++] = 4;
    tmpul = htonl(reqspec.lease);
    bcopy(&tmpul, &snd.dhcp->options[offopt], 4);
    offopt += 4;
  }

  /* insert requesting ipaddr */
  if (reqspec.ipaddr.s_addr != 0 && CHKOFF(4)) {
    snd.dhcp->options[offopt++] = REQUEST_IPADDR;
    snd.dhcp->options[offopt++] = 4;
    bcopy(&reqspec.ipaddr.s_addr, &snd.dhcp->options[offopt], 4);
    offopt += 4;
  }

  /* insert Maximum DHCP message size */
  if (CHKOFF(2)) {
    snd.dhcp->options[offopt++] = DHCP_MAXMSGSIZE;
    snd.dhcp->options[offopt++] = 2;
    tmpus = htons(DFLTDHCPLEN + UDPHL + IPHL);
    bcopy(&tmpus, &snd.dhcp->options[offopt], 2);
    offopt += 2;
  }

  /* if necessary, insert request list */
  if (reqspec.reqlist.len != 0 && CHKOFF(reqspec.reqlist.len)) {
    snd.dhcp->options[offopt++] = REQ_LIST;
    snd.dhcp->options[offopt++] = reqspec.reqlist.len;
    bcopy(reqspec.reqlist.list, &snd.dhcp->options[offopt],
	  reqspec.reqlist.len);
    offopt += reqspec.reqlist.len;
  }

  snd.dhcp->options[offopt] = END;

  /*
   * make udp part
   */
  /* fill udp header */
  snd.udp->uh_sport = dhcpc_port;
  snd.udp->uh_dport = dhcps_port;
  snd.udp->uh_ulen = htons(DFLTDHCPLEN + UDPHL);

  /* fill pseudo udp header */
  spudph.srcip.s_addr = 0;
  spudph.dstip.s_addr = 0xffffffff;
  spudph.zero = 0;
  spudph.prto = IPPROTO_UDP;
  spudph.ulen = snd.udp->uh_ulen;

  /*
   * make ip part
   */
  /* fill ip header */
  snd.ip->ip_v = IPVERSION;
  snd.ip->ip_hl = IPHL >> 2;
  snd.ip->ip_tos = 0;
  snd.ip->ip_len = htons(DFLTDHCPLEN + UDPHL + IPHL);
  tmpul = generate_xid();
  tmpul += (tmpul >> 16);
  snd.ip->ip_id = (u_short) (~tmpul);
  snd.ip->ip_off = htons(IP_DF);                         /* XXX */
  snd.ip->ip_ttl = 0x20;                                 /* XXX */
  snd.ip->ip_p = IPPROTO_UDP;
  snd.ip->ip_src.s_addr = 0;
  snd.ip->ip_dst.s_addr = 0xffffffff;
  snd.ip->ip_sum = 0;
  snd.ip->ip_sum = cksum((u_short *)snd.ip, snd.ip->ip_hl * 2);

  /*
   * make ether part
   */
  /* fill ether frame header */
  for (i = 0; i < 6; i++) {
#ifdef sun
    snd.ether->ether_dhost.ether_addr_octet[i] = 0xff;
    snd.ether->ether_shost.ether_addr_octet[i] = intface.haddr.haddr[i];
#else
    snd.ether->ether_dhost[i] = 0xff;
    snd.ether->ether_shost[i] = intface.haddr.haddr[i];
#endif
  }
  snd.ether->ether_type = htons(ETHERTYPE_IP);

  return;
}


/*
 * construct DHCPREQUEST
 */
int
make_request(paramp, type)
  struct dhcp_param *paramp;
  int type;
{
  int i = 0;
  int offopt = 0;                   /* offset in options field */
  u_Long tmpul = 0;
  u_short tmpus = 0;

  /*
   * construct dhcp part
   */
  tmpus = snd.dhcp->secs;
  bzero(sbuf.buf, sbuf.size + QWOFF);
  snd.dhcp->op = BOOTREQUEST;
  snd.dhcp->htype = intface.haddr.htype;
  snd.dhcp->hlen = intface.haddr.hlen;
  snd.dhcp->xid = generate_xid();
  snd.dhcp->secs = tmpus;
  if (type == REQUESTING || type == REBOOTING || type == VERIFYING)
    snd.dhcp->ciaddr.s_addr = 0;
  else
    snd.dhcp->ciaddr.s_addr = paramp->yiaddr.s_addr;

  bcopy(intface.haddr.haddr, snd.dhcp->chaddr, snd.dhcp->hlen);

  /* insert magic cookie */
  bcopy(magic_c, snd.dhcp->options, MAGIC_LEN);
  offopt = MAGIC_LEN;

  /* insert message type */
  snd.dhcp->options[offopt++] = DHCP_MSGTYPE;
  snd.dhcp->options[offopt++] = 1;
  snd.dhcp->options[offopt++] = DHCPREQUEST;

  /* insert class identifier */
  if (reqspec.class != NULL && CHKOFF(reqspec.class->len)) {
    snd.dhcp->options[offopt++] = CLASS_ID;
    snd.dhcp->options[offopt++] = reqspec.class->len;
    bcopy(reqspec.class->id, &snd.dhcp->options[offopt], reqspec.class->len);
    offopt += reqspec.class->len;
  }

  /* insert client identifier */
  if (reqspec.clid != NULL && CHKOFF(reqspec.clid->len + 1)) {
    snd.dhcp->options[offopt++] = CLIENT_ID;
    snd.dhcp->options[offopt++] = reqspec.clid->len + 1;
    snd.dhcp->options[offopt++] = reqspec.clid->type;
    bcopy(reqspec.clid->id, &snd.dhcp->options[offopt], reqspec.clid->len);
    offopt += reqspec.clid->len;
  }

  /* insert requesting lease */
  if (type != VERIFYING && reqspec.lease != 0 && CHKOFF(4)) {
    snd.dhcp->options[offopt++] = LEASE_TIME;
    snd.dhcp->options[offopt++] = 4;
    tmpul = htonl(reqspec.lease);
    bcopy(&tmpul, &snd.dhcp->options[offopt], 4);
    offopt += 4;
  }

  /* insert requesting ipaddr */
  if (type == REQUESTING || type == REBOOTING || type == VERIFYING) {
    if (paramp->yiaddr.s_addr != 0 && CHKOFF(4)) {
      snd.dhcp->options[offopt++] = REQUEST_IPADDR;
      snd.dhcp->options[offopt++] = 4;
      bcopy(&paramp->yiaddr.s_addr, &snd.dhcp->options[offopt], 4);
      offopt += 4;
    }
  }

  /* insert server identifier */
  if (type == REQUESTING) {
    if (paramp->server_id.s_addr == 0) {
      return(-1);
    }
    if (CHKOFF(4)) {
      snd.dhcp->options[offopt++] = SERVER_ID;
      snd.dhcp->options[offopt++] = 4;
      bcopy(&paramp->server_id.s_addr, &snd.dhcp->options[offopt], 4);
      offopt += 4;
    }
  }

  /* insert Maximum DHCP message size */
  if (CHKOFF(2)) {
    snd.dhcp->options[offopt++] = DHCP_MAXMSGSIZE;
    snd.dhcp->options[offopt++] = 2;
    tmpus = htons(DFLTDHCPLEN + UDPHL + IPHL);
    bcopy(&tmpus, &snd.dhcp->options[offopt], 2);
    offopt += 2;
  }

  /* if necessary, insert request list */
  if (reqspec.reqlist.len != 0 && CHKOFF(reqspec.reqlist.len)) {
    snd.dhcp->options[offopt++] = REQ_LIST;
    snd.dhcp->options[offopt++] = reqspec.reqlist.len;
    bcopy(reqspec.reqlist.list, &snd.dhcp->options[offopt],
	  reqspec.reqlist.len);
    offopt += reqspec.reqlist.len;
  }

  snd.dhcp->options[offopt] = END;

  if (type == RENEWING)    /* RENEWING is unicast with the normal socket */
    return(0);

  /*
   * make udp part
   */
  /* fill udp header */
  snd.udp->uh_sport = dhcpc_port;
  snd.udp->uh_dport = dhcps_port;
  snd.udp->uh_ulen = htons(DFLTDHCPLEN + UDPHL);

  /* fill pseudo udp header */
  spudph.zero = 0;
  spudph.prto = IPPROTO_UDP;
  spudph.ulen = snd.udp->uh_ulen;

  /*
   * make ip part
   */
  /* fill ip header */
  snd.ip->ip_v = IPVERSION;
  snd.ip->ip_hl = IPHL >> 2;
  snd.ip->ip_tos = 0;
  snd.ip->ip_len = htons(DFLTDHCPLEN + UDPHL + IPHL);
  tmpul = generate_xid();
  tmpul += (tmpul >> 16);
  snd.ip->ip_id = (u_short) (~tmpul);
  snd.ip->ip_off = htons(IP_DF);                         /* XXX */
  snd.ip->ip_ttl = 0x20;                                 /* XXX */
  snd.ip->ip_p = IPPROTO_UDP;

  switch (type) {
  case REQUESTING:
  case REBOOTING:
  case VERIFYING:
    snd.ip->ip_src.s_addr = spudph.srcip.s_addr = 0;
    snd.ip->ip_dst.s_addr = spudph.dstip.s_addr = 0xffffffff;
    break;
  case REBINDING:
    snd.ip->ip_src.s_addr = spudph.srcip.s_addr = paramp->yiaddr.s_addr;
    snd.ip->ip_dst.s_addr = spudph.dstip.s_addr = 0xffffffff;
    break;
  }
  snd.ip->ip_sum = 0;
  snd.ip->ip_sum = cksum((u_short *)snd.ip, snd.ip->ip_hl * 2);

  /*
   * make ether part
   */
  /* fill ether frame header */
  for (i = 0; i < 6; i++) {
#ifdef sun
    snd.ether->ether_dhost.ether_addr_octet[i] = 0xff;
    snd.ether->ether_shost.ether_addr_octet[i] = intface.haddr.haddr[i];
#else
    snd.ether->ether_dhost[i] = 0xff;
    snd.ether->ether_shost[i] = intface.haddr.haddr[i];
#endif
  }
  snd.ether->ether_type = htons(ETHERTYPE_IP);

  return(0);
}


/*
 * send DHCP message within unicast
 */
int
send_unicast(dstip, sdhcp)
  struct in_addr *dstip;
  struct dhcp *sdhcp;
{
  static int sockfd = -1;
  struct sockaddr_in dst;
  struct msghdr msg;
  struct iovec bufvec[1];
  int bufsize = DFLTDHCPLEN;
#ifdef BSDOS
  int on;
#endif

  if (sockfd == -1) {
    struct sockaddr_in myaddr;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
      return(-1);

    bzero(&myaddr, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myaddr.sin_port = dhcpc_port;
    if (bind(sockfd, (struct sockaddr *) &myaddr, sizeof(myaddr)) < 0) {
      close(sockfd);
      sockfd = -1;
      return(-1);
    }
    if (setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &bufsize,
		   sizeof(bufsize)) < 0) {
      close(sockfd);
      sockfd = -1;
      return(-1);
    }

#ifdef BSDOS
    on = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on)) < 0) {
      close(sockfd);
      sockfd = -1;
      return(-1);
    }
#endif
  }

  bzero(&dst, sizeof(dst));
  dst.sin_family = AF_INET;
  dst.sin_addr.s_addr = dstip->s_addr;
  dst.sin_port = dhcps_port;

  bufvec[0].iov_base = (char *) sdhcp;
  bufvec[0].iov_len = bufsize;

  bzero(&msg, sizeof(msg));
  msg.msg_name = (caddr_t) &dst;
  msg.msg_namelen = sizeof(dst);
  msg.msg_iov = bufvec;
  msg.msg_iovlen = 1;

  if (sendmsg(sockfd, &msg, 0) < 0) {
    close(sockfd);
    sockfd = -1;
    return(-1);
  }

/*
  if (connect(sockfd, (struct sockaddr *) &dst, sizeof(dst)) < 0) {
    close(sockfd);
    sockfd = -1;
    return(-1);
  }

  if (write(sockfd, sdhcp, bufsize) < bufsize) {
    close(sockfd);
    sockfd = -1;
    return(-1);
  }
*/

  return(0);
}


/*
 * pickup the IP address and set it into appropriate field of dhcp_param
 */
int
handle_ip(buf, param)
  char *buf;
  struct dhcp_param *param;
{
  struct in_addr *addr = NULL;

  if (*buf == SERVER_ID) {
    addr = &param->server_id;
  } else {
    if ((addr = (struct in_addr *) calloc(1, sizeof(struct in_addr))) == NULL) {
      return(-1);
    }

    switch (*buf) {
    case SUBNET_MASK:
      if (param->subnet_mask != NULL) free(param->subnet_mask);
      param->subnet_mask = addr;
      break;
    case SWAP_SERVER:
      if (param->swap_server != NULL) free(param->swap_server);
      param->swap_server = addr;
      break;
    case BRDCAST_ADDR:
      if (param->brdcast_addr != NULL) free(param->brdcast_addr);
      param->brdcast_addr = addr;
      break;
    case ROUTER_SOLICIT:
      if (param->router_solicit != NULL) free(param->router_solicit);
      param->router_solicit = addr;
      break;
    default:
      free(addr);
      return(EINVAL);
    }
  }

  bcopy(OPTBODY(buf), addr, OPTLEN(buf));
  buf += OPTLEN(buf) + 1;
  return(0);
}


/*
 * pickup the number and set it into appropriate field of dhcp_param
 */
int
handle_num(buf, param)
  char *buf;
  struct dhcp_param *param;
{
  char   charnum = 0;
  short  shortnum = 0;
  Long   longnum = 0;

  switch (OPTLEN(buf)) {
  case 1:
    charnum = *OPTBODY(buf);
    break;
  case 2:
    shortnum = GETHS(OPTBODY(buf));
    break;
  case 4:
    longnum = GETHL(OPTBODY(buf));
    break;
  default:
    return(-1);
  }

  switch (*buf) {
  case TIME_OFFSET:
    param->time_offset = longnum;
    break;
  case BOOTSIZE:
    param->bootsize = (unsigned short) shortnum;
    break;
  case MAX_DGRAM_SIZE:
    param->max_dgram_size = (unsigned short) shortnum;
    break;
  case DEFAULT_IP_TTL:
    param->default_ip_ttl = (unsigned char) charnum;
    break;
  case MTU_AGING_TIMEOUT:
    param->mtu_aging_timeout = (unsigned Long) longnum;
    break;
  case IF_MTU:
    param->intf_mtu = (unsigned short) shortnum;
    break;
  case ARP_CACHE_TIMEOUT:
    param->arp_cache_timeout = (unsigned Long) longnum;
    break;
  case DEFAULT_TCP_TTL:
    param->default_tcp_ttl = (unsigned char) charnum;
    break;
  case KEEPALIVE_INTER:
    param->keepalive_inter = (unsigned Long) longnum;
    break;
  case NB_NODETYPE:
    param->nb_nodetype = (unsigned) charnum;
    break;
  case LEASE_TIME:
    param->lease_duration = (unsigned Long) longnum;
    break;
  case DHCP_T1:
    param->dhcp_t1 = (unsigned Long) longnum;
    break;
  case DHCP_T2:
    param->dhcp_t2 = (unsigned Long) longnum;
    break;
  default:
    return(EINVAL);
  }

  buf += OPTLEN(buf) + 1;
  return(0);
}


/*
 * pickup IP addresses and set them into appropriate field of dhcp_param
 */
int
handle_ips(buf, param)
  char *buf;
  struct dhcp_param *param;
{
  struct in_addr  *addr = NULL;
  struct in_addrs *addrs = NULL;
  unsigned char  num = 0;

  num = OPTLEN(buf) / 4;
  if ((addr = (struct in_addr *)calloc((int) num, sizeof(struct in_addr))) == NULL) {
    return(-1);
  }

  if ((addrs = (struct in_addrs *) calloc(1, sizeof(struct in_addrs))) == NULL) {
    free(addr);
    return(-1);
  }

  switch (*buf) {
  case ROUTER:
    if (param->router != NULL) {
      if (param->router->addr != NULL) free(param->router->addr);
    free(param->router);
    }
    param->router = addrs;
    param->router->num = num;
    param->router->addr = addr;
    break;
  case TIME_SERVER:
    if (param->time_server != NULL) {
      if (param->time_server->addr != NULL) free(param->time_server->addr);
      free(param->time_server);
    }
    param->time_server = addrs;
    param->time_server->num = num;
    param->time_server->addr = addr;
    break;
  case NAME_SERVER:
    if (param->name_server != NULL) {
      if (param->name_server->addr != NULL) free(param->name_server->addr);
      free(param->name_server);
    }
    param->name_server = addrs;
    param->name_server->num = num;
    param->name_server->addr = addr;
    break;
  case DNS_SERVER:
    if (param->dns_server != NULL) {
      if (param->dns_server->addr != NULL) free(param->dns_server->addr);
      free(param->dns_server);
    }
    param->dns_server = addrs;
    param->dns_server->num = num;
    param->dns_server->addr = addr;
    break;
  case LOG_SERVER:
    if (param->log_server != NULL) {
      if (param->log_server->addr != NULL) free(param->log_server->addr);
      free(param->log_server);
    }
    param->log_server = addrs;
    param->log_server->num = num;
    param->log_server->addr = addr;
    break;
  case COOKIE_SERVER:
    if (param->cookie_server != NULL) {
      if (param->cookie_server->addr != NULL) free(param->cookie_server->addr);
      free(param->cookie_server);
    }
    param->cookie_server = addrs;
    param->cookie_server->num = num;
    param->cookie_server->addr = addr;
    break;
  case LPR_SERVER:
    if (param->lpr_server != NULL) {
      if (param->lpr_server->addr != NULL) free(param->lpr_server->addr);
      free(param->lpr_server);
    }
    param->lpr_server = addrs;
    param->lpr_server->num = num;
    param->lpr_server->addr = addr;
    break;
  case IMPRESS_SERVER:
    if (param->impress_server != NULL) {
      if (param->impress_server->addr != NULL) free(param->impress_server->addr);
      free(param->impress_server);
    }
    param->impress_server = addrs;
    param->impress_server->num = num;
    param->impress_server->addr = addr;
    break;
  case RLS_SERVER:
    if (param->rls_server != NULL) {
      if (param->rls_server->addr != NULL) free(param->rls_server->addr);
      free(param->rls_server);
    }
    param->rls_server = addrs;
    param->rls_server->num = num;
    param->rls_server->addr = addr;
    break;
  case NIS_SERVER:
    if (param->nis_server != NULL) {
      if (param->nis_server->addr != NULL) free(param->nis_server->addr);
      free(param->nis_server);
    }
    param->nis_server = addrs;
    param->nis_server->num = num;
    param->nis_server->addr = addr;
    break;
  case NTP_SERVER:
    if (param->ntp_server != NULL) {
      if (param->ntp_server->addr != NULL) free(param->ntp_server->addr);
      free(param->ntp_server);
    }
    param->ntp_server = addrs;
    param->ntp_server->num = num;
    param->ntp_server->addr = addr;
    break;
  case NBN_SERVER:
    if (param->nbn_server != NULL) {
      if (param->nbn_server->addr != NULL) free(param->nbn_server->addr);
      free(param->nbn_server);
    }
    param->nbn_server = addrs;
    param->nbn_server->num = num;
    param->nbn_server->addr = addr;
    break;
  case NBDD_SERVER:
    if (param->nbdd_server != NULL) {
      if (param->nbdd_server->addr != NULL) free(param->nbdd_server->addr);
      free(param->nbdd_server);
    }
    param->nbdd_server = addrs;
    param->nbdd_server->num = num;
    param->nbdd_server->addr = addr;
    break;
  case XFONT_SERVER:
    if (param->xfont_server != NULL) {
      if (param->xfont_server->addr != NULL) free(param->xfont_server->addr);
      free(param->xfont_server);
    }
    param->xfont_server = addrs;
    param->xfont_server->num = num;
    param->xfont_server->addr = addr;
    break;
  case XDISPLAY_MANAGER:
    if (param->xdisplay_manager != NULL) {
      if (param->xdisplay_manager->addr != NULL) free(param->xdisplay_manager->addr);
      free(param->xdisplay_manager);
    }
    param->xdisplay_manager = addrs;
    param->xdisplay_manager->num = num;
    param->xdisplay_manager->addr = addr;
    break;
  case NISP_SERVER:
    if (param->nisp_server != NULL) {
      if (param->nisp_server->addr != NULL) free(param->nisp_server->addr);
      free(param->nisp_server);
    }
    param->nisp_server = addrs;
    param->nisp_server->num = num;
    param->nisp_server->addr = addr;
    break;
  case MOBILEIP_HA:
    if (param->mobileip_ha != NULL) {
      if (param->mobileip_ha->addr != NULL) free(param->mobileip_ha->addr);
      free(param->mobileip_ha);
    }
    param->mobileip_ha = addrs;
    param->mobileip_ha->num = num;
    param->mobileip_ha->addr = addr;
    break;
  case SMTP_SERVER:
    if (param->smtp_server != NULL) {
      if (param->smtp_server->addr != NULL) free(param->smtp_server->addr);
      free(param->smtp_server);
    }
    param->smtp_server = addrs;
    param->smtp_server->num = num;
    param->smtp_server->addr = addr;
    break;
  case POP3_SERVER:
    if (param->pop3_server != NULL) {
      if (param->pop3_server->addr != NULL) free(param->pop3_server->addr);
      free(param->pop3_server);
    }
    param->pop3_server = addrs;
    param->pop3_server->num = num;
    param->pop3_server->addr = addr;
    break;
  case NNTP_SERVER:
    if (param->nntp_server != NULL) {
      if (param->nntp_server->addr != NULL) free(param->nntp_server->addr);
      free(param->nntp_server);
    }
    param->nntp_server = addrs;
    param->nntp_server->num = num;
    param->nntp_server->addr = addr;
    break;
  case DFLT_WWW_SERVER:
    if (param->dflt_www_server != NULL) {
      if (param->dflt_www_server->addr != NULL) free(param->dflt_www_server->addr);
      free(param->dflt_www_server);
    }
    param->dflt_www_server = addrs;
    param->dflt_www_server->num = num;
    param->dflt_www_server->addr = addr;
    break;
  case DFLT_FINGER_SERVER:
    if (param->dflt_finger_server != NULL) {
      if (param->dflt_finger_server->addr != NULL)
	free(param->dflt_finger_server->addr);
      free(param->dflt_finger_server);
    }
    param->dflt_finger_server = addrs;
    param->dflt_finger_server->num = num;
    param->dflt_finger_server->addr = addr;
    break;
  case DFLT_IRC_SERVER:
    if (param->dflt_irc_server != NULL) {
      if (param->dflt_irc_server->addr != NULL) free(param->dflt_irc_server->addr);
      free(param->dflt_irc_server);
    }
    param->dflt_irc_server = addrs;
    param->dflt_irc_server->num = num;
    param->dflt_irc_server->addr = addr;
    break;
  case STREETTALK_SERVER:
    if (param->streettalk_server != NULL) {
      if (param->streettalk_server->addr != NULL) free(param->streettalk_server->addr);
      free(param->streettalk_server);
    }
    param->streettalk_server = addrs;
    param->streettalk_server->num = num;
    param->streettalk_server->addr = addr;
    break;
  case STDA_SERVER:
    if (param->stda_server != NULL) {
      if (param->stda_server->addr != NULL) free(param->stda_server->addr);
      free(param->stda_server);
    }
    param->stda_server = addrs;
    param->stda_server->num = num;
    param->stda_server->addr = addr;
    break;
  default:
    free(addr);
    free(addrs);
    return(EINVAL);
  }

  bcopy(OPTBODY(buf), addr, OPTLEN(buf));
  buf += OPTLEN(buf) + 1;
  return(0);
}


/*
 * pickup the strings and set it into appropriate field of dhcp_param
 */
int
handle_str(buf, param)
  char *buf;
  struct dhcp_param *param;
{
  char *str = NULL;

  if ((str = calloc(1, (OPTLEN(buf) + 1))) == NULL) { /* +1 for null terminate */
    return(-1);
  }

  switch (*buf) {
  case HOSTNAME:
    if (param->hostname != NULL) free(param->hostname);
    param->hostname = str;
    break;
  case MERIT_DUMP:
    if (param->merit_dump != NULL) free(param->merit_dump);
    param->merit_dump = str;
    break;
  case DNS_DOMAIN:
    if (param->dns_domain != NULL) free(param->dns_domain);
    param->dns_domain = str;
    break;
  case ROOT_PATH:
    if (param->root_path != NULL) free(param->root_path);
    param->root_path = str;
    break;
  case EXTENSIONS_PATH:
    if (param->extensions_path != NULL) free(param->extensions_path);
    param->extensions_path = str;
    break;
  case NIS_DOMAIN:
    if (param->nis_domain != NULL) free(param->nis_domain);
    param->nis_domain = str;
    break;
  case NB_SCOPE:
    if (param->nb_scope != NULL) free(param->nb_scope);
    param->nb_scope = str;
    break;
  case DHCP_ERRMSG:
    if (param->errmsg != NULL) free(param->errmsg);
    param->errmsg = str;
    break;
  case NISP_DOMAIN:
    if (param->nisp_domain != NULL) free(param->nisp_domain);
    param->nisp_domain = str;
    break;
  default:
    free(str);
    return(EINVAL);
  }

  bcopy(OPTBODY(buf), str, OPTLEN(buf));
  str[OPTLEN(buf)] = '\0';
  buf += OPTLEN(buf) + 1;
  return(0);
}


/*
 * pickup the boolean value and set it into appropriate field of dhcp_param
 */
int
handle_bool(buf, param)
  char *buf;
  struct dhcp_param *param;
{
  switch (*buf) {
  case IP_FORWARD:
    param->ip_forward = *OPTBODY(buf);
    break;
  case NONLOCAL_SRCROUTE:
    param->nonlocal_srcroute = *OPTBODY(buf);
    break;
  case ALL_SUBNET_LOCAL:
    param->all_subnet_local = *OPTBODY(buf);
    break;
  case MASK_DISCOVER:
    param->mask_discover = *OPTBODY(buf);
    break;
  case MASK_SUPPLIER:
    param->mask_supplier = *OPTBODY(buf);
    break;
  case ROUTER_DISCOVER:
    param->router_discover = *OPTBODY(buf);
    break;
  case TRAILER:
    param->trailer = *OPTBODY(buf);
    break;
  case ETHER_ENCAP:
    param->ether_encap = *OPTBODY(buf);
    break;
  case KEEPALIVE_GARBA:
    param->keepalive_garba = *OPTBODY(buf);
    break;
  default:
    return(EINVAL);
  }

  buf += OPTLEN(buf) + 1;
  return(0);
}


/*
 * pickup pairs of IP address and set them into appropriate field of dhcp_param
 */
int
handle_ippairs(buf, param)
  char *buf;
  struct dhcp_param *param;
{
  struct in_addr  *addr = NULL;
  struct in_addrs *addrs = NULL;
  unsigned char   num = 0;

  num = OPTLEN(buf) / 4;
  if ((addr = (struct in_addr *)calloc((int) num, sizeof(struct in_addr))) == NULL) {
    return(-1);
  }

  if ((addrs = (struct in_addrs *) calloc(1, sizeof(struct in_addrs))) == NULL) {
    free(addr);
    return(-1);
  }

  switch (*buf) {
  case POLICY_FILTER:
    if (param->policy_filter != NULL) {
      if (param->policy_filter->addr != NULL) free(param->policy_filter->addr);
      free(param->policy_filter);
    }
    param->policy_filter = addrs;
    param->policy_filter->num = num / 2;
    param->policy_filter->addr = addr;
    break;
  case TIME_SERVER:
    if (param->static_route != NULL) {
      if (param->static_route->addr != NULL) free(param->static_route->addr);
      free(param->static_route);
    }
    param->static_route = addrs;
    param->static_route->num = num / 2;
    param->static_route->addr = addr;
    break;
  default:
    free(addr);
    free(addrs);
    return(EINVAL);
  }

  bcopy(OPTBODY(buf), addr, OPTLEN(buf));
  buf += OPTLEN(buf) + 1;
  return(0);
}


/*
 * pickup numbers and set them into appropriate field of dhcp_param
 */
int
handle_nums(buf, param)
  char *buf;
  struct dhcp_param *param;
{
  int i = 0, max = 0;

  if (*buf != MTU_PLATEAU_TABLE) {
    return(EINVAL);
  } else {
    if (param->mtu_plateau_table != NULL) {
      if (param->mtu_plateau_table->shortnum != NULL)
	free(param->mtu_plateau_table->shortnum);
      free(param->mtu_plateau_table);
    }
    if ((param->mtu_plateau_table =
	 (struct u_shorts *) calloc(1, sizeof(struct u_shorts))) == NULL) {
      return(-1);
    }
    max = param->mtu_plateau_table->num = OPTLEN(buf) / 2;
    if ((param->mtu_plateau_table->shortnum =
	 (u_short *) calloc(max, sizeof(unsigned short))) == NULL) {
      return(-1);
    }
    for (i = 0; i < max; i++) {
      param->mtu_plateau_table->shortnum[i] = GETHS(&buf[i * 2 + 2]);
    }
  }

  buf += OPTLEN(buf) + 1;
  return(0);
}
