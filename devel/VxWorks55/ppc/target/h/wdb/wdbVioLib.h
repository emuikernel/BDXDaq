/* wdbVioLib.h - VIO header file for remote debug server */

/* Copyright 1984-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01h,25apr02,jhw  Added C++ support (SPR 76304).
01g,27may98,cth removed get snd/rcv buffer ioctl commands for tsfs sockets
01f,19mar98,cth add ioctl commands for tsfs sockets
01e,18aug97,cth add FIOSNDURGB ioctl cmd for tsfs
01d,12nov96,c_s add wdbTsfsDrv prototype
01c,24may95,ms  added wdbVioChannelUnregister().
01b,05apr95,ms  new data types.
01a,15nov94,ms  written.
*/

#ifndef __INCwdbVioLibh
#define __INCwdbVioLibh

#ifdef __cplusplus
extern "C" {
#endif

/* includes */

#include "wdb/wdb.h"
#include "wdb/dll.h"

/* data types */

typedef struct wdb_vio_node
    {
    dll_t	node;
    uint_t      channel;
    int         (*inputRtn) (struct wdb_vio_node *pNode, char *pData,
                             uint_t nBytes);
    void *      pVioDev;
    } WDB_VIO_NODE;

/* ioctl commands for Target-Server File System (TSFS) */

#define SO_SNDURGB	5000	/* send an out-of-band byte on sock */
#define SO_SETDEBUG	5001	/* setsockopt SO_DEBUG for sock (int) */
#define SO_GETDEBUG	5002	/* getsockopt SO_DEBUG for sock (int) */
#define SO_SETSNDBUF	5003	/* setsockopt SO_SNDBUF for sock (int) */
#define SO_SETRCVBUF	5005	/* setsockopt SO_RCVBUF for sock (int) */
#define SO_SETDONTROUTE	5007	/* setsockopt SO_DONTROUTE for sock (int) */
#define SO_GETDONTROUTE	5008	/* getsockopt SO_DONTROUTE for sock (int) */
#define SO_SETOOBINLINE	5009	/* setsockopt SO_OOBINLINE for sock (int) */
#define SO_GETOOBINLINE	5010	/* getsockopt SO_OOBINLINE for sock (int) */
			

/* function prototypes */

#if defined(__STDC__)

extern void	wdbVioLibInit	  	(void);
extern STATUS	wdbVioChannelRegister	(WDB_VIO_NODE *pVioNode);
extern void	wdbVioChannelUnregister (WDB_VIO_NODE *pVioNode);
extern STATUS	wdbTsfsDrv		(char * root);

#else   /* __STDC__ */

extern void	wdbVioLibInit	 	();
extern STATUS	wdbVioDevRegister	();
extern void	wdbVioChannelUnregister	();
extern STATUS	wdbTsfsDrv		();

#endif  /* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INCwdbVioLibh */
