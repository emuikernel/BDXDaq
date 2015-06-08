/* dgTest.c - datagram test sender/receiver demo */

/*
modification history
--------------------
01e,25may95,jcf  new environment variables
01d,16mar94,yao  fixed to support IRIX 4.0.5 tool.
01c,15feb93,jcf  added mixed endian support.
01b,31oct91,rrr  passed through the ansification filter
		  -removed include of netdb.h
		  -changed includes to have absolute path from h/
		  -fixed #else and #endif
		  -changed VOID to void
		  -changed copyright notice
01a,10jun88,gae  written based on rdc's test programs.
*/

/*
DESCRIPTION
This demonstration module shows how to send and receive UDP packets
from VxWorks to UNIX and any combination thereof.
Firstly, a receiver, or receivers, are initiated on VxWorks and/or
UNIX.  It will wait for packets on an agreed port number.
Then a sender, again on either UNIX or VxWorks, begins sending packets.
The receiver will terminate after an agreed number of packets are sent.

EXAMPLE
    -> sp dgReceiver
    -> dgSender "127.1"
    ... output from dgReceiver
    -> dgHelp

A few global variables are defined: verbose, terminate, and broadcast.
If verbose is true then extra diagnostics are printed.
If terminate is set to true while dgReceiver is running then it will
exit gracefully (after the first packet).
If broadcast is set to true then dgSender will broadcast the packets.
*/

#ifdef CPU
#include "vxWorks.h"
#include "sys/types.h"
#include "ioLib.h"
#include "fioLib.h"
#include "stdio.h"
#include "unistd.h"
#include "string.h"
#include "usrLib.h"
#include "errnoLib.h"
#include "hostLib.h"
#include "sockLib.h"
#include "socket.h"
#include "inetLib.h"
#include "in.h"
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BOOL int
#define LOCAL static
#define IMPORT extern
#define ERROR -1
#define NULL 0
#define TRUE 1
#define FALSE 0
#endif

#define	TORF(x)	((x) ? "True" : "False")

#define PACKET_SIZE	100 	/* size of UDP packets */
#define PACKET_NUM	500	/* maximum number of packets */

IMPORT char sysBootHost[];	/* VxWorks boot hostname */

typedef struct	/* PACKET */
    {
    int sequence;
    int spare;
    char data [PACKET_SIZE - 2 * sizeof (int)];
    } PACKET;

BOOL broadcast       = FALSE;	/* when TRUE dgSender will broadcast */
BOOL verbose         = FALSE;	/* when TRUE extra diagnostics are displayed */
BOOL terminate       = FALSE;	/* when TRUE dgReceiver will quit */
BOOL is42            = FALSE;	/* when FALSE socket is set for broadcast */
int dgPort           = 1101;	/* default */
int dgPackets        = 5;	/* default */
#ifdef POLL
int fionread         = FIONREAD;/* VxWorks value (different from UNIX) */
#endif


/*******************************************************************************
*
* dgSender - send UDP datagrams
*/

void dgSender (host, port, packets)
    char *host;		/* NULL = use default */
    int port;		/* 0 = use default */
    int packets;	/* 0 = use default */

    {
    struct in_addr destNet;
    u_long inetaddr;
    struct sockaddr_in sockaddr;
    int optval = 1;
    PACKET packet;
    int ix;
    int s;

    if (host == NULL)
	host = sysBootHost;

    if ((inetaddr = hostGetByName (host)) == ERROR &&
	(inetaddr = inet_addr (host)) == ERROR)
	{
	printf ("invalid host: <%s>\n", host);
	return;
	}

    if (broadcast)
	{
#ifdef CPU
	struct in_addr tmp;

	tmp.s_addr = inetaddr;
	destNet  = inet_makeaddr (inet_netof (tmp), INADDR_BROADCAST);
	inetaddr = destNet.s_addr;
#else
#if (WIND_HOST_TYPE==iris)
	struct in_addr tmp;

	tmp.s_addr = inetaddr;
	destNet  = inet_makeaddr (inet_netof (tmp), INADDR_BROADCAST);
	inetaddr = destNet.s_addr;
#else
	destNet = inet_makeaddr (inet_netof (inetaddr), INADDR_BROADCAST);
#endif	/* WIND_HOST_TYPE==iris */
#endif

	if (verbose)
	    printf ("broadcast ");
	}

    if (verbose)
	printf ("inet address = %#x\n", htonl(inetaddr));

    s = socket (AF_INET, SOCK_DGRAM, 0);	/* get a udp socket */

    if (s < 0)
	{
	printf ("socket error (errno = %#x)\n", errnoGet ());
	return;
	}

    if (!is42)
	{
	/* BSD 4.2 doesn't have or require turning on of broadcasting */

	if (setsockopt (s, SOL_SOCKET, SO_BROADCAST,
			(caddr_t) &optval, sizeof (optval)) < 0)
	    {
	    printf ("setsockopt error (errno = %#x)\n", errnoGet ());
	    return;
	    }
	}

    if (port == 0)
	port = dgPort;

    if (packets == 0)
	packets = dgPackets;

    bzero ((char *) &packet, sizeof (PACKET));
    bzero ((char *) &sockaddr, sizeof (sockaddr));

    sockaddr.sin_family      = AF_INET;
    sockaddr.sin_addr.s_addr = inetaddr;
    sockaddr.sin_port        = htons (port);

    /* send the datagrams */

    for (ix = 0; ix < packets; ix++)
	{
	packet.sequence = htonl (ix);

	/* send it */

	if (sendto (s, (caddr_t) &packet, sizeof (packet), 0,
		    (struct sockaddr *)&sockaddr, sizeof (sockaddr)) == -1)
	    {
	    printf ("sendto error on packet # %d (errno = %#x)\n",
		    ix, errnoGet ());
	    close (s);
	    return;
	    }
	}

    printf ("sent %d packets\n", ix);

    close (s);
    }
/*******************************************************************************
*
* dgReceiver -
*/

void dgReceiver (port, packets)
    int port;
    int packets;

    {
    struct sockaddr_in sockaddr;
    int sockaddrLen = sizeof (sockaddr);
    int nBytes;
    int ix;
    BOOL missing;
    char packetList [PACKET_NUM];	/* boolean array of received packets */
    PACKET packet;
    int nPacketsReceived = 0;
    int s = socket (AF_INET, SOCK_DGRAM, 0);

    if (s < 0)
	{
	printf ("socket error (errno = %#x)\n", errnoGet ());
	return;
	}

#ifdef	REUSE
    if (setsockopt (s, SOL_SOCKET, SO_REUSEADDR,
		     (caddr_t) &optval, sizeof (optval)) < 0)
	{
	printf ("setsockopt error (errno = %#x)\n", errnoGet ());
	return;
	}
#endif	/* REUSE */

    if (port == 0)
	port = dgPort;

    if (packets == 0)
	packets = dgPackets;

    /* clear out the packetList */

    bzero ((char *) packetList, sizeof (packetList));
    bzero ((char *) &sockaddr, sizeof (sockaddr));

    sockaddr.sin_family      = AF_INET;
    sockaddr.sin_port        = htons (port);

    if (bind (s, (struct sockaddr *) &sockaddr, sizeof (sockaddr)) == ERROR)
	{
	printf ("bind error (errno = %#x)\n", errnoGet ());
	return;
	}

    if (getsockname (s, (struct sockaddr *) &sockaddr, &sockaddrLen) == ERROR)
	{
	printf ("getsockname error (errno = %#x)\n", errnoGet ());
	return;
	}

    terminate = FALSE;

    while (packets > nPacketsReceived)
	{
#ifdef	POLL
	if (ioctl (s, fionread, &nBytes) == ERROR)
	    {
	    printf ("ioctl error (errno = %#x)\n", errnoGet ());
	    close (s);
	    return;
	    }

	if (nBytes > 0)
#endif	/* POLL */
	    {
	    nBytes = recvfrom (s, (caddr_t) &packet, sizeof (PACKET), 0,
			       (struct sockaddr *) &sockaddr, &sockaddrLen);

	    if (verbose)
		printf ("received packet # %d\n", ntohl(packet.sequence));

	    packetList [ntohl(packet.sequence)] = TRUE;
	    nPacketsReceived++;
	    }
	if (terminate)
	    {
	    printf ("terminated\n");
	    break;
	    }
	}

    missing = FALSE;
    nPacketsReceived = 0;
    for (ix = 0; ix < packets; ix++)
	{
	if (packetList[ix] == TRUE)
	    nPacketsReceived++;
	else
	    {
	    missing = TRUE;
	    printf ("%d\n", ix);
	    }
	}

    printf ("\n%smissing packets\n", missing ? "" : "no ");
    printf ("%d packets received\n", nPacketsReceived);

    close (s);
    }
/*******************************************************************************
*
* dgHelp -
*/

void dgHelp ()

    {
    printf ("dgHelp                             show this list\n");
    printf ("dgReceiver [port],[packets]        receive datagrams\n");
    printf ("dgSender [host],[port],[packets]   send datagrams\n");
    printf ("\n");
    printf ("broadcast                = %s\n", TORF(broadcast));
    printf ("host                     = %s\n", sysBootHost);
    printf ("port number              = %d\n", dgPort);
    printf ("number of packets        = %d\n", dgPackets);
    printf ("verbose                  = %s\n", TORF(verbose));
    printf ("\n");
    }
