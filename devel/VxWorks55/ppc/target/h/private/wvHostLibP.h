/* wvHostLibP.h - host info library header */

/* Copyright 1997 Wind River Systems, Inc. */

/*
modification history
--------------------
01d,29aug97,cth  should have changed the initials on 01c
01c,21aug97,smb  added include of bootLib.h
01b,04mar94,smb  changed prototype for wvHostInfoInit (SPR #3089)
01a,10dec93,smb  written.
*/

#ifndef __INCwvhostlibph
#define __INCwvhostlibph

/* include bootLib.h just for BOOT_ADDR_LEN */

#include "bootLib.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DEFAULT_PORT 6164

typedef struct hostInfo              /* maintains host and port information */
    {
    char   	ipAddress [BOOT_ADDR_LEN];
    ushort_t    port;
    } HOSTINFO;

extern HOSTINFO * pHostInfo;

#if defined(__STDC__) || defined(__cplusplus)

#else   /* __STDC__ */

#endif  /* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INCwvhostlibph*/

