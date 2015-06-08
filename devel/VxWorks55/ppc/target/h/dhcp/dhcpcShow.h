/* dhcpcShow.h - DHCP runtime client include file for show routines */

/* Copyright 1984 - 1997 Wind River Systems, Inc. */

/*
modification history
____________________
01a,26oct97,spm  written.
*/

#ifndef __INCdhcpcShowh
#define __INCdhcpcShowh

#ifdef __cplusplus
extern "C" {
#endif

IMPORT void 	dhcpcShowInit (void);
IMPORT STATUS 	dhcpcServerShow (void *);
IMPORT STATUS 	dhcpcTimersShow (void *);
IMPORT STATUS 	dhcpcParamsShow (void *);

#ifdef __cplusplus
}
#endif

#endif
