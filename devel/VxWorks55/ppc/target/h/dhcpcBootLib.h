/* dhcpcBootLib.h - DHCP client include file for boot-time interface */

/* Copyright 1984 - 2000 Wind River Systems, Inc. */

/*
modification history
____________________
01f,13jun00,spm  changed interface to support adding options to messages;
                 added prototype to prevent compiler warnings
01e,05apr00,spm  upgraded to RFC 2131 and combined with run-time client
01d,04dec97,spm  added code review modifications
01c,19aug97,spm  major overhaul: preserved single-lease interface by
                 merging include files - 01f of dhcpClientCommon.h, 
                 01d of dhcp.h, 01c of dhcpCommonSubr.h, 01e of dhcpc.h
01b,06aug97,spm  added conditional include and definitions for C++ compilation
01a,07apr97,spm  created.
*/

#ifndef __INCdhcpcBootLibh
#define __INCdhcpcBootLibh

#ifdef __cplusplus
extern "C" {
#endif

#include "netinet/in.h"

#include "dhcp/dhcpcBoot.h"

/* DHCP boot-time high-level interface functions */

IMPORT void * 	dhcpcBootInit (struct ifnet *, int, int, int, int, int, int);
IMPORT STATUS 	dhcpcBootBind (void);
IMPORT STATUS 	dhcpcBootInformGet (char *);
IMPORT STATUS 	dhcpcBootParamsGet (struct dhcp_param *);

#ifdef __cplusplus
}
#endif

#endif
