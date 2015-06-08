/* ei82596hkvEnd.c - Intel 82596 END driver for hkv3500 board */

/* Copyright 1989-1998 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01c,09jun98,dat  changed API (no workaround avail)
01b,14dec97,jdi  doc: added xref to ei82596End
01a,18apr97,map  created
*/

/*
This module defines a specific interface to the Intel 82596 ethernet
chip on an hkv3500 board.

NOTICE:
The API for this driver has changed.  The function sys596Init, which is defined
in the BSP, must use a single argument, 'unit'.  Previous versions of this driver for R3000 only, used a different argument list.  All R3000 users must
update their BSP code for this new API.

This module also adds 2 additional BSP routines, sys596IntConnect() and
sys596IntDisconnect().  

.CS
    STATUS sys596IntConnect (int unit, FUNCPTR rtn, ULONG arg)
    STATUS sys596IntDisconnect (int unit)
.CE

SEE ALSO: ei82596End
*/

#include "vxWorks.h"

#define ei82596EndLoad	ei82596hkvEndLoad
#define	DRV_NAME	"eihkv"
#define	DRV_NAME_LEN	6		/* strlen() + 1 */

IMPORT void   sys596Init (int);
IMPORT STATUS sys596IntConnect (int, FUNCPTR, int);
IMPORT STATUS sys596IntDisconnect (int);

#ifndef	SYS_INT_CONNECT
#define	SYS_INT_CONNECT(pDrvCtrl,rtn,arg,pResult)                       \
    {                                                                   \
    sys596Init ((pDrvCtrl)->unit);                                      \
    *pResult = sys596IntConnect ((pDrvCtrl)->unit, (FUNCPTR)rtn, arg);  \
    }
#endif /* SYS_INT_CONNECT */

#ifndef	SYS_INT_DISCONNECT
#define SYS_INT_DISCONNECT(pDrvCtrl,rtn,arg,pResult)                    \
    {                                                                   \
    *pResult = sys596IntDisconnect ((pDrvCtrl)->unit);                  \
    }
#endif /* SYS_INT_DISCONNECT */

#include "ei82596End.c"
