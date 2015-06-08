/* bsdSockLib.h -  UNIX BSD 4.4 compatible socket library header */

/* Copyright 1984 - 1997 Wind River Systems, Inc. */

/*
modification history
--------------------
01e,29apr99,pul  Upgraded NPT phase3 code to tor2.0.0
01d,02feb99,sgv  added definition of bsdZbufSockRtn.
01c,06oct97,spm  added optional binary compatibility for BSD 4.3 applications
01b,07aug95,dzb  moved sys/socket.h inclusion to sockFunc.h.
01a,21jul95,dzb  created from sockLib.h, ver 02d.
*/

#ifndef __INCbsdSockLibh
#define __INCbsdSockLibh

#ifdef __cplusplus
extern "C" {
#endif

/* includes */

#include "sockFunc.h"

/* externals */

#ifdef BSD43_COMPATIBLE
IMPORT BOOL 	bsdSock43ApiFlag;
#endif

/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

extern SOCK_FUNC *bsdSockLibInit (void);
extern STATUS 	bsdBind (int s, struct sockaddr *name, int namelen);
extern STATUS 	bsdConnect (int s, struct sockaddr *name, int namelen);
extern STATUS 	bsdConnectWithTimeout (int sock, struct sockaddr *adrs,
				    int adrsLen, struct timeval *timeVal);
extern STATUS 	bsdGetpeername (int s, struct sockaddr *name, int *namelen);
extern STATUS 	bsdGetsockname (int s, struct sockaddr *name, int *namelen);
extern STATUS 	bsdGetsockopt (int s, int level, int optname, char *optval,
			    int *optlen);
extern STATUS 	bsdListen (int s, int backlog);
extern STATUS 	bsdSetsockopt (int s, int level, int optname, char *optval,
			    int optlen);
extern STATUS 	bsdShutdown (int s, int how);
extern int 	bsdAccept (int s, struct sockaddr *addr, int *addrlen);
extern int 	bsdRecv (int s, char *buf, int bufLen, int flags);
extern int 	bsdRecvfrom (int s, char *buf, int bufLen, int flags,
			  struct sockaddr *from, int *pFromLen);
extern int 	bsdRecvmsg (int sd, struct msghdr *mp, int flags);
extern int 	bsdSend (int s, char *buf, int bufLen, int flags);
extern int 	bsdSendmsg (int sd, struct msghdr *mp, int flags);
extern int 	bsdSendto (int s, caddr_t buf, int bufLen, int flags,
			struct sockaddr *to, int tolen);
extern int 	bsdSocket (int domain, int type, int protocol);
extern STATUS   bsdZbufSockRtn ( void );

#else	/* __STDC__ */

extern SOCK_FUNC *bsdSockLibInit ();
extern STATUS 	bsdBind ();
extern STATUS 	bsdConnect ();
extern STATUS 	bsdConnectWithTimeout ();
extern STATUS 	bsdGetpeername ();
extern STATUS 	bsdGetsockname ();
extern STATUS 	bsdGetsockopt ();
extern STATUS 	bsdListen ();
extern STATUS 	bsdSetsockopt ();
extern STATUS 	bsdShutdown ();
extern int 	bsdAccept ();
extern int 	bsdRecv ();
extern int 	bsdRecvfrom ();
extern int 	bsdRecvmsg ();
extern int 	bsdSend ();
extern int 	bsdSendmsg ();
extern int 	bsdSendto ();
extern int 	bsdSocket ();
extern STATUS   bsdZbufSockRtn ();

#endif	/* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INCbsdSockLibh */
