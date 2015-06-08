/* dhcpcInit.h - DHCP runtime client include file for system startup */

/* Copyright 1984 - 2001 Wind River Systems, Inc. */

/*
modification history

---------------------
01g,09oct01,rae  merge from truestack: update dhcpcConfigSet call
01f,24nov99,spm  upgraded to RFC 2131 and removed direct link-level access
01e,06aug97,spm  removed parameters linked list to reduce memory required;
                 added definitions for C++ compilation; renamed class field
                 of dhcp_reqspec structure to prevent C++ errors (SPR #9079)
01d,02jun97,spm  changed DHCP option tags to prevent name conflicts (SPR #8667)
01c,07apr97,spm  removed #define's now in configAll.h, changed router_solicit
                 field in parameter descriptor from pointer to structure
01b,03jan97,spm  added little-endian support and moved macros to dhcp.h
01a,03oct96,spm  created by modifying WIDE project DHCP implementation
*/

#ifndef __INCdhcpcInith
#define __INCdhcpcInith

#ifdef __cplusplus
extern "C" {
#endif

#include "bootLib.h"

/* Functions called at system startup. */

IMPORT STATUS dhcpcShowInit (void);
IMPORT STATUS dhcpcLibInit (int, int, int, int, int, int, int);
IMPORT STATUS dhcpcLeaseGet (char *, BOOL *);
IMPORT STATUS dhcpcConfigSet (BOOT_PARAMS *, char *, int *, BOOL * pDhcpStatus,
                              BOOL configFlag);

#ifdef __cplusplus
}
#endif

#endif
