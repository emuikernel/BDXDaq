/* dhcpcInternal.h - DHCP runtime client include file for internal usage */

/* Copyright 1984 - 2000 Wind River Systems, Inc. */

/*
modification history
____________________
01g,05apr00,spm  removed entries shared with boot-time client
01f,24nov99,spm  upgraded to RFC 2131 and removed direct link-level access
01e,04dec97,spm  added code review modifications
01d,06oct97,spm  added relative paths to #include statements
01c,06aug97,spm  added definitions for C++ compilation
01b,27jan97,spm  added support for little-endian architectures
01a,03oct96,spm  created by modifying WIDE project DHCP Implementation
*/

#ifndef __INCdhcpcInternalh
#define __INCdhcpcInternalh

#ifdef __cplusplus
extern "C" {
#endif

#include "netinet/in.h"

#include "dhcp/copyright_dhcp.h"
#include "dhcp/dhcp.h"
#include "dhcp/dhcpc.h"
#include "dhcp/dhcpcCommonLib.h"

IMPORT struct buffer sbuf;
IMPORT struct ps_udph spudph;
IMPORT struct msg dhcpcMsgIn;
IMPORT struct msg dhcpcMsgOut;
IMPORT unsigned char dhcpCookie [MAGIC_LEN];

IMPORT int initialize (int, int, int);
IMPORT int init (EVENT_DATA *);
IMPORT int wait_offer (EVENT_DATA *);
IMPORT int selecting (EVENT_DATA *);
IMPORT int requesting (EVENT_DATA *);
IMPORT int bound (EVENT_DATA *);
IMPORT int renewing (EVENT_DATA *);
IMPORT int rebinding (EVENT_DATA *);
IMPORT int init_reboot (EVENT_DATA *);
IMPORT int verify (EVENT_DATA *);
IMPORT int reboot_verify (EVENT_DATA *);
IMPORT int inform (EVENT_DATA *);

IMPORT void dhcpcPrivateCleanup (void);

IMPORT int config_if (struct if_info *, struct in_addr *, 
                      struct in_addr *, struct in_addr *);
IMPORT void set_route (struct dhcp_param *);
IMPORT void down_if (struct if_info *);
IMPORT void reset_if (struct if_info *);

IMPORT void align_msg (struct msg *, char *);
IMPORT char * pickup_opt (struct dhcp *, int, char);
IMPORT int dhcp_msgtoparam (struct dhcp *, int, struct dhcp_param *);
IMPORT int arp_check (struct in_addr *, struct if_info *);
IMPORT int arp_reply (struct in_addr *, struct if_info *);
IMPORT int merge_param (struct dhcp_param *, struct dhcp_param *);
IMPORT int clean_param (struct dhcp_param *);

IMPORT int make_discover (LEASE_DATA *, BOOL);
IMPORT int make_request (LEASE_DATA *, int, BOOL);
IMPORT long generate_xid (struct if_info *);
IMPORT int send_unicast (struct in_addr *, struct dhcp *, int);
IMPORT int gen_retransmit (LEASE_DATA *, int);

IMPORT void set_declinfo (struct dhcp_reqspec *, LEASE_DATA *, char *, int);
IMPORT int dhcp_decline (struct dhcp_reqspec *, struct if_info *);
IMPORT void set_relinfo (struct dhcp_reqspec *, LEASE_DATA *, char *);
IMPORT int dhcp_release (struct dhcp_reqspec *, struct if_info *, BOOL);

IMPORT int use_parameter (struct dhcp_param *, LEASE_DATA *);
IMPORT void release (LEASE_DATA *, BOOL);

IMPORT int handle_ip (char *, struct dhcp_param *);
IMPORT int handle_num (char *, struct dhcp_param *);
IMPORT int handle_ips (char *, struct dhcp_param *);
IMPORT int handle_str (char *, struct dhcp_param *);
IMPORT int handle_bool (char *, struct dhcp_param *);
IMPORT int handle_ippairs (char *, struct dhcp_param *);
IMPORT int handle_nums (char *, struct dhcp_param *);
IMPORT int handle_list (char *, struct dhcp_param *);

IMPORT int (*handle_param[MAXTAGNUM])();

#ifdef __cplusplus
}
#endif

#endif
