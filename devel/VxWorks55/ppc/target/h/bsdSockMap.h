/* bsdSockMap.h -  BSD socket name mapping */

/* Copyright 1995 Wind River Systems, Inc. */

/*
modification history
--------------------
01c,29apr99,pul  Upgraded NPT phase3 code to tor2.0.0
01b,02feb99,sgv  added mapping for zbufsockrtn
01a,21jul95,dzb  written.
*/
 
#ifndef __INCbsdSockMaph
#define __INCbsdSockMaph

#ifdef __cplusplus
extern "C" {
#endif

/* defines */

#define sockLibInit		bsdSockLibInit
#define accept			bsdAccept
#define bind			bsdBind
#define connect			bsdConnect
#define connectWithTimeout	bsdConnectWithTimeout
#define getpeername		bsdGetpeername
#define getsockname		bsdGetsockname
#define listen			bsdListen
#define recv			bsdRecv
#define recvfrom		bsdRecvfrom
#define recvmsg			bsdRecvmsg
#define send			bsdSend
#define sendto			bsdSendto
#define sendmsg			bsdSendmsg
#define shutdown		bsdShutdown
#define socket			bsdSocket
#define getsockopt		bsdGetsockopt
#define setsockopt 		bsdSetsockopt
#define zbufsockrtn             bsdZbufSockRtn

#ifdef __cplusplus
}
#endif

#endif /* __INCbsdSockMaph */
