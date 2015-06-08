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

struct client_id {
  unsigned char   type,
                  len;
  char            *id;
};

struct class_id {
  unsigned char  len;
  char           *id;
};

struct request_list {
  u_char len;
  char   list[0xff];     /* 0xff == maximum length */
};

struct dhcp_reqspec {         /* parameter for address allocation */
  int select_offer;
  int waitsecs;                  /* after got first OFFER, collects other
				    OFFER till 'waitsecs' expires */
  u_Long lease;                  /* suggestion of lease duration */
  struct in_addr  ipaddr;        /* suggestion of IP address */
  struct in_addr  srvaddr;
  struct request_list reqlist;
  struct client_id *clid;
  struct class_id *class;
  char *dhcp_errmsg;
};
#define DHCPOFFER_SELECT_FIRST   0
#define DHCPOFFER_SELECT_LONG    1

struct dhcp_declinfo {         /* parameter for DHCPRELEASE and DHCPDECLINE */
  struct in_addr    ipaddr,            /* DHCP client's IP addr */
                    srv_id;            /* DHCP server's IP addr */
  struct client_id  *cid;
  char              *errmsg;
};

struct chaddr {
  unsigned char htype,
                hlen;
  char          haddr[6];
};

struct if_info {
  char name[sizeof "enxx"];
  struct chaddr haddr;
};

struct in_addrs {
  unsigned char   num;
  struct in_addr  *addr;
};

struct u_shorts {
  unsigned char   num;
  unsigned short  *shortnum;
};

#define MAXTAGNUM     (LAST_OPTION + 1)
#define GOTOPTSIZ     (MAXTAGNUM / 8)
struct dhcp_param {
  struct dhcp_param *next;
  char got_option[GOTOPTSIZ];
  char              *sname,                 /* server host name */
                    *file,                  /* boot file name */
                    *hostname,
                    *merit_dump,
                    *dns_domain,
                    *root_path,
                    *extensions_path,
                    *nis_domain,
                    *nb_scope,              /* NetBIOS scope */
                    *errmsg,
                    *nisp_domain;           /* NIS+ domain name */
  unsigned          ip_forward        :1,   /* boolean */
                    nonlocal_srcroute :1,   /* boolean */
                    all_subnet_local  :1,   /* boolean */
                    mask_discover     :1,   /* boolean */
                    mask_supplier     :1,   /* boolean */
                    router_discover   :1,   /* boolean */
                    trailer           :1,   /* boolean */
                    ether_encap       :1,   /* boolean */
                    keepalive_garba   :1,   /* boolean */
                    nb_nodetype       :4;   /* NetBIOS node type */
#define BNODE 0x1
#define PNODE 0x2
#define MNODE 0x4
#define HNODE 0x8
  unsigned char     default_ip_ttl,      /* time to live (1 - 255) */
                    default_tcp_ttl;
  unsigned short    bootsize,            /* bootfile size in 16 bits */
                    max_dgram_size,      /* max dgram reassembly */
                    intf_mtu;            /* interface mtu */
  Long              time_offset;         /* 32 bits integer(network byte order)
					    offset from UTC */
  unsigned Long     lease_origin,        /* time that begin the lease */
                    lease_duration,      /* lease duration */
                    dhcp_t1,
                    dhcp_t2,
                    mtu_aging_timeout,   /* unsigned 32 bits integer */
                    arp_cache_timeout,
                    keepalive_inter;
  struct u_shorts   *mtu_plateau_table;  /* array of 16 bits int */
  struct in_addr    server_id,           /* DHCP server identifier */
                    ciaddr,
                    yiaddr,              /* IP address */
                    siaddr,              /* (tftp) server */
                    giaddr,
                    *subnet_mask,
                    *swap_server,
                    *brdcast_addr,
                    *router_solicit;
  struct in_addrs   *router,
                    *time_server,
                    *name_server,
                    *dns_server,         /* DNS server */
                    *log_server,
                    *cookie_server,
                    *lpr_server,
                    *impress_server,
                    *rls_server,         /* Resource Location Server */
                    *policy_filter,   
                    *static_route,
                    *nis_server,
                    *ntp_server,
                    *nbn_server,         /* NetBIOS name server */
                    *nbdd_server,        /* NetBIOS dgram distrib server */
                    *xfont_server,       /* X font erver */
                    *xdisplay_manager,   /* X display manager */
                    *nisp_server,        /* NIS+ server */
                    *mobileip_ha,        /* Mobile IP Home Agent */
                    *smtp_server,
                    *pop3_server,
                    *nntp_server,
                    *dflt_www_server,
                    *dflt_finger_server,
                    *dflt_irc_server,
                    *streettalk_server,
                    *stda_server;       /* StreetTalk Directory Assistance */
};

/* these are setbit(), isset() and isclr() for NETWORK BIT ORDER */
#define SETBIT(a,i)   ((a)[(i) / 8] |= (1 << (7 - (i) % 8)))
#define ISSET(a,i)    ((a)[(i) / 8] & (1 << (7 - (i) % 8)))
#define ISCLR(a,i)    (((a)[(i) / 8] & (1 << (7 - (i) % 8))) == 0)

/* handle the word alignment */
#define   GETHS(PTR)  (*((u_char *)PTR)*256 + *(((u_char *)PTR)+1))
#define   GETHL(PTR)  (*((u_char *)PTR)*256*256*256 + *(((u_char *)PTR)+1)*256*256 +\
		       *(((u_char *)PTR)+2)*256 + *(((u_char *)PTR)+3))

int dhcp_client();

/*
 * following are for user's convenience
 */
int config_if();
void set_route();
