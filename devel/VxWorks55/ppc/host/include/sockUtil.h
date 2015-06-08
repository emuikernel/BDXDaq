/* sockUtil.h - socket utilities header file */

/* Copyright 1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,23apr98,pcn  Added netinet/in.h for sockaddr_in definition.
01a,02mar98,pcn  written.
*/

#ifndef __INCsockUtilh
#define __INCsockUtilh       1

#ifdef __cplusplus
extern "C" {
#endif

/* includes */

#ifndef WIN32
#include <netinet/in.h>
#endif

/* defines */

/* typedefs */

typedef struct sock_desc        /* Structure used to return socket info */
    {
    int      portNumber;        /* Free port number         */
    int      sockId;            /* Socket descriptor        */
    } SOCK_DESC;

/* globals */

/* function declarations */

#if defined(__STDC__) || defined(__cplusplus) || defined(WIN32_COMPILER)

extern int	sockResvPortBind
    (
    int 			sd,
    struct sockaddr_in * 	sin
    );
    
extern int	sockStart 
    (
    SOCK_DESC * 		pSockDesc
    );
             
#endif  /* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INCsockUtilh */
