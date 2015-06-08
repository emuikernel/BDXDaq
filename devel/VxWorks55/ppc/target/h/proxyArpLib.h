/* proxyArpLib.h - proxy ARP server include file */

/* Copyright 1984 - 2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01g,10oct01,rae  merge from truestack
01f,07feb01,spm  added merge record for 30jan01 update from version 01e of
                 tor2_0_x branch (base 01d) and fixed modification history
01e,30jan01,ijm  merged SPR# 28602 fixes: proxy ARP services are obsolete
01d,22sep92,rrr  added support for c++
01c,04jul92,jcf  cleaned up.
01b,26may92,rrr  the tree shuffle
		  -changed includes to have absolute path from h/
01a,20sep91,elh	  written.
*/

#ifndef __INCproxyArpLibh
#define __INCproxyArpLibh

#ifdef __cplusplus
extern "C" {
#endif

/* includes */

#include "vwModNum.h"
#include "lstLib.h"
#include "semLib.h"
#include "net/if.h"
#include "netinet/if_ether.h"
#include "netinet/in.h"
#include "hashLib.h"

/* defines */

/* errors */

#define	S_proxyArpLib_INVALID_PARAMETER		(M_proxyArpLib | 1)
#define S_proxyArpLib_INVALID_INTERFACE		(M_proxyArpLib | 2)
#define	S_proxyArpLib_INVALID_PROXY_NET		(M_proxyArpLib | 3)
#define	S_proxyArpLib_INVALID_CLIENT		(M_proxyArpLib | 4)
#define S_proxyArpLib_INVALID_ADDRESS		(M_proxyArpLib | 5)
#define S_proxyArpLib_TIMEOUT			(M_proxyArpLib | 6)

					/* proxy message types 	*/

#define PROXY_PROBE		0 	/* probe 	*/
#define PROXY_REG		1	/* register 	*/
#define PROXY_UNREG		2	/* unregister	*/
#define PROXY_ACK		50	/* ack		*/
#define PROXY_TYPE		0x3000  /* doesn't leave backplane */

#define XMIT_MAX		5
#define XMIT_DELAY		5

/* typedefs */

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 1                 /* tell gcc960 not to optimize alignments */
#endif  /* CPU_FAMILY==I960 */

typedef struct proxy_net		/* proxy network structure 	*/
    {
    NODE		netNode;	/* node for net list		*/
    struct in_addr 	proxyAddr;	/* proxy interface address 	*/
    struct in_addr	mainAddr;	/* main interface address 	*/
    LIST	 	clientList; 	/* list of clients 		*/
    } PROXY_NET;

typedef struct proxy_clnt		/* proxy client structure	*/
    {
    HASH_NODE		hashNode;	/* hash node 			*/
    struct in_addr	ipaddr;		/* client ip address (key)	*/
    u_char 		hwaddr [6]; 	/* client hw address 		*/
    PROXY_NET *		pNet;		/* proxy network information 	*/
    NODE		clientNode;	/* client node on network list 	*/
    } PROXY_CLNT;

typedef struct port_node
    {
    HASH_NODE		hashNode;	/* hash  node			*/
    int			port;		/* port number enabled		*/
    } PORT_NODE;

typedef struct proxy_msg		/* proxy message 		*/
    {
    int			op;			/* operation		*/
    struct in_addr	clientAddr;		/* client address	*/
    struct in_addr	serverAddr;		/* server ip address	*/
    u_char 		clientHwAddr [6];	/* client hw address	*/
    u_char 		serverHwAddr [6];	/* server hw address	*/
    } PROXY_MSG;

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 0                 /* turn off alignment requirement */
#endif  /* CPU_FAMILY==I960 */

#if defined(__STDC__) || defined(__cplusplus)

extern STATUS 	proxyArpLibInit (int clientSizeLog2, int portSizeLog2);
extern void 	proxyPortShow (void);
extern STATUS 	proxyPortFwdOn (int port);
extern STATUS 	proxyPortFwdOff (int port);
extern STATUS 	proxyNetCreate (char * proxyAddr, char * mainAddr);
extern STATUS 	proxyNetDelete (char * proxyAddr);

#else   /* __STDC__ */

extern STATUS 	proxyArpLibInit ();
extern void 	proxyPortShow ();
extern STATUS 	proxyPortFwdOn ();
extern STATUS 	proxyPortFwdOff ();
extern STATUS 	proxyNetCreate ();
extern STATUS 	proxyNetDelete ();

#endif  /* __STDC__ */


#ifdef __cplusplus
}
#endif

#endif /* __INCproxyArpLibh */
