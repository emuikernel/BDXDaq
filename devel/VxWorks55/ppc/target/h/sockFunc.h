/* sockFunc.h - socket function table header */

/* Copyright 1984 - 2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01f,12oct01,rae  merge from truestack (add sockLibMap)
01e,29apr99,pul  Upgraded NPT phase3 code to tor2.0.0
01d,02feb99,sgv  added zbufRtn to sockFunc structure.
01c,02aug95,dzb  removed inclusion of iosLib.h, added sys/socket.h.
01b,25jul95,dzb  removed SOCKET_DEV structure.
01a,21jul95,dzb  created.
*/

#ifndef __INCsockFunch
#define __INCsockFunch

#ifdef __cplusplus
extern "C" {
#endif

/* includes */

#include "sys/times.h"
#include "sys/socket.h"

/* HIDDEN */

/* typedefs */

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 1                 /* tell gcc960 not to optimize alignments */
#endif  /* CPU_FAMILY==I960 */

typedef struct sockFunc			/* SOCK_FUNC */
    {
    FUNCPTR	libInitRtn;		/* sockLibInit()	*/
    FUNCPTR	acceptRtn;		/* accept()		*/
    FUNCPTR	bindRtn;		/* bind()		*/
    FUNCPTR	connectRtn;		/* connect()		*/
    FUNCPTR	connectWithTimeoutRtn;	/* connectWithTimeout()	*/
    FUNCPTR	getpeernameRtn;		/* getpeername()	*/
    FUNCPTR	getsocknameRtn;		/* getsockname()	*/
    FUNCPTR	listenRtn;		/* listen()		*/
    FUNCPTR	recvRtn;		/* recv()		*/
    FUNCPTR	recvfromRtn;		/* recvfrom()		*/
    FUNCPTR	recvmsgRtn;		/* recvmsg()		*/
    FUNCPTR	sendRtn;		/* send()		*/
    FUNCPTR	sendtoRtn;		/* sendto()		*/
    FUNCPTR	sendmsgRtn;		/* sendmsg()		*/
    FUNCPTR	shutdownRtn;		/* shutdown()		*/
    FUNCPTR	socketRtn;		/* socket()		*/
    FUNCPTR	getsockoptRtn;		/* getsockopt()		*/
    FUNCPTR	setsockoptRtn;		/* setsockopt()		*/
    FUNCPTR     zbufRtn;		/* ZBUF support 	*/
    } SOCK_FUNC;

/* typedefs */

typedef struct sockLibMap
    {
    int			domainMap;	/* mapping address family  */
    int			domainReal;	/* real address family     */
    SOCK_FUNC *		pSockFunc;	/* socket function table   */
    struct sockLibMap *	pNext;		/* next socket lib mapping */
    } SOCK_LIB_MAP;
    
#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 0                 /* turn off alignment requirement */
#endif  /* CPU_FAMILY==I960 */

/* END_HIDDEN */

#ifdef __cplusplus
}
#endif

#endif /* __INCsockFunch */
