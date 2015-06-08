/* dhcprLib.h - DHCP relay agent include file for user interface */

/* Copyright 1984 - 1997 Wind River Systems, Inc. */

/*
modification history
____________________
01d,04dec97,spm  added code review modifications
01c,06aug97,spm  added definitions for C++ compilation
01b,28apr97,spm  moved DHCP_MAX_HOPS to configAll.h
01a,07apr97,spm  created.
*/

#ifndef __INCdhcprLibh
#define __INCdhcprLibh

#ifdef __cplusplus
extern "C" {
#endif

#if CPU_FAMILY==I960
#pragma align 1                 /* tell gcc960 not to optimize alignments */
#endif  /* CPU_FAMILY==I960 */

typedef struct serverDesc
    {
    char *pAddress;             /* IP address of DHCP server target */
    } DHCP_TARGET_DESC;

typedef struct server 
    {
    struct in_addr ip;
    struct server *next;
    } DHCP_SERVER_DESC;

IMPORT DHCP_TARGET_DESC * 	pDhcpRelayTargetTbl;
IMPORT int dhcpNumTargets;
IMPORT DHCP_SERVER_DESC * 	pDhcpTargetList;
IMPORT struct msg dhcpMsgIn;

#if CPU_FAMILY==I960
#pragma align 0                 /* turn off alignment requirement */
#endif  /* CPU_FAMILY==I960 */

IMPORT int open_if();
IMPORT void read_server_db (int);

#ifdef __cplusplus
}
#endif

#endif
