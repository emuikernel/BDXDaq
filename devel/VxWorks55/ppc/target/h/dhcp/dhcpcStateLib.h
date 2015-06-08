/* dhcpcStateLib.h - DHCP runtime client interface to state machine */

/* Copyright 1984 - 2001 Wind River Systems, Inc. */

/*
modification history
____________________
01f,10oct01,rae  merge from truestack
01e,13jun00,spm  increased size of function pointer array for new state
01d,24nov99,spm  upgraded to RFC 2131 and removed direct link-level access
01c,06oct97,spm  added relative paths to #include statements
01b,06aug97,spm  added definitions for C++ compilation
01a,03oct96,spm  created by modifying WIDE project DHCP implementation
*/

#ifndef __INCdhcpcStateLibh
#define __INCdhcpcStateLibh

#ifdef __cplusplus
extern "C" {
#endif

#include "dhcp/copyright_dhcp.h"

/* DHCP client states */

#define  INIT         0
#define  WAIT_OFFER   1
#define  SELECTING    2
#define  REQUESTING   3
#define  BOUND        4
#define  RENEWING     5
#define  REBINDING    6
#define  INIT_REBOOT  7
#define  VERIFY       8
#define  REBOOTING    9
#define  VERIFYING   10
#define  INFORMING   11
#define  MAX_STATES  INFORMING + 1

IMPORT int (*fsm[MAX_STATES])();

IMPORT STATUS dhcp_client_setup (int, int, int);
IMPORT int dhcp_client (void *, BOOL);

#ifdef __cplusplus
}
#endif

#endif
