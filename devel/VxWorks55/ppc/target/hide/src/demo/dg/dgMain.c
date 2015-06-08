/* dgMain.c - UNIX main for datagram test sender/receiver demo */

/*
modification history
--------------------
01d,31oct91,rrr  passed through the ansification filter
		  -changed includes to have absolute path from h/
		  -fixed #else and #endif
		  -changed VOID to void
		  -changed copyright notice
01c,15oct91,ajm  made mips look to bsd includes
01b,25oct90,lpf  #ifdef <sys/socket.h>.
		 put back bzero() for HP.
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
    % dgReciver -v
    % dgSender -h localhost -v
    ... output from dgReceiver

OPTIONS
    -v verbose flag causes extra diagnostics to be printed.
    -b broadcast flag causes dgSender to broadcast the packets.
    -h <host> host name flag indicates the network to send on.
    -p <port> selects the port number (default is 1101).
    -n <#> is the number of packets to send (default 5).
    -help shows the options.
*/

#if 0
#include "vxWorks.h"
#include "sys/types.h"
#include "netdb.h"
#include "inetLib.h"
#include "in.h"
#else
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>

#define IMPORT extern
#define LOCAL static
#define BOOL int
#define ERROR -1
#define NULL 0
#endif

#if	!defined(HOST_HP)
#include <sys/socket.h>	/* Do not include this file for HP/UX 7.0 */
#endif

#if	!defined(HOST_MIPS)
#include <sys/ioctl.h>
#else
#include <bsd43/sys/ioctl.h>
#endif

#define	TORF(x)	((x) ? "True" : "False")

#define PACKET_NUM	500	/* maximum number of packets */

IMPORT BOOL broadcast;		/* when TRUE dgSender will broadcast */
IMPORT BOOL verbose;		/* when TRUE extra diagnostics are displayed */
IMPORT int dgPort;
IMPORT int dgPackets;

IMPORT BOOL is42;		/* SUNOS 3.4 is not completely 4.3 compatible */
#ifdef POLL
IMPORT int fionread;		/* FIONREAD value (different from VxWorks) */
#endif

IMPORT int errno;

char sysBootHost [40];		/* VxWorks boot host name / our host name */

/* UNIX program must be called either: */

#define	SENDNAME	"dgSender"
#define	RECVNAME	"dgReceiver"

LOCAL char *toolname;

LOCAL char *dgSendUsage =
    "usage: %s [-help] [-b] [-h host] [-p port] [-n packets] [-v]\n";
LOCAL char *dgRecvUsage =
    "usage: %s [-help] [-p port] [-n packets] [-v]\n";

LOCAL char *dgSendHelp =
"show defaults\n\
-b    broadcast         = %s\n\
-h    host              = %s\n\
-p    port number       = %d\n\
-n    number of packets = %d\n\
-v    verbose           = %s\n";

LOCAL char *dgRecvHelp =
"show defaults\n\
-p    port number       = %d\n\
-n    number of packets = %d\n\
-v    verbose           = %s\n";

#if     defined(HOST_HP)
void bzero (s, n)
    char *s;
    int n;

    {
    memset (s, '\0', n);
    }
#endif


/*******************************************************************************
*
* main - send/recieve UDP datagrams
*
* -help		show defaults
* -b 		(broadcast [send only])
* -h host	(broadcast [send only])
* -n port	(1101)
* -p packets	(5)
* -v            (verbose)
*/

void main (argc, argv)
    int argc;
    char *argv [];

    {
    char **argp = argv;
    BOOL send;

    toolname  = *argp;
    send = strcmp (SENDNAME, toolname) == 0;

    gethostname (sysBootHost, sizeof (sysBootHost));

    while (--argc > 0)
	{
	argp++;
	switch (argp [0][0])
	    {
	    case '-':
		if (strcmp (*argp, "-help") == 0)
		    {
		    if (send)
			printf (dgSendHelp, TORF(broadcast), sysBootHost,
				dgPort, dgPackets, TORF(verbose));
		    else
			printf (dgRecvHelp,
				dgPort, dgPackets, TORF(verbose));
		    exit (0);
		    }
		switch (argp [0][1])
		    {
		    case 'b':
			if (!send)
			    goto l_usage;
			broadcast = !broadcast;
			break;
		    case 'h':
			if (!send)
			    goto l_usage;
			if (argc-- > 0)
			    strcpy (sysBootHost, *++argp);
			else
			    {
			    printf ("%s: missing <%s>\n", toolname, "host");
			    goto l_usage;
			    }
			break;
		    case 'n':
			if (argc-- > 0)
			    {
			    dgPackets = atoi (*++argp);
			    if (dgPackets > PACKET_NUM)
				{
				printf ("%s: too many packets <%d>\n",
					toolname, dgPackets);
				goto l_usage;
				}
			    }
			else
			    {
			    printf ("%s: missing <%s>\n", toolname, "packet");
			    goto l_usage;
			    }
			break;
		    case 'p':
			if (argc-- > 0)
			    dgPort = atoi (*++argp);
			else
			    {
			    printf ("%s: missing <%s>\n", toolname, "port");
			    goto l_usage;
			    }
			break;
		    case 'v':
			verbose = !verbose;
			break;
		    default:
			printf ("%s: bad flag <%s>\n", toolname, *argp);
			goto l_usage;
		    }
		break;

	    default:
		printf ("%s: no such option <%s>\n", toolname, *argp);
		goto l_usage;
	    }
	}

    if (argc > 0)
	{
	printf ("%s: no such option <%s> ...\n", toolname, *argp);
	goto l_usage;
	}

#ifdef POLL
    fionread = FIONREAD;	/* VxWorks differs */
#endif

#ifndef	SO_BROADCAST
    /* BSD 4.2 doesn't have or require turning on of broadcasting */
    is42 = TRUE;
#endif	/* SO_BROADCAST */

    if (send)
	dgSender (sysBootHost, dgPort, dgPackets);
    else
	dgReceiver (dgPort, dgPackets);

    exit (0);

l_usage:
    printf ((send ? dgSendUsage : dgRecvUsage), toolname);
    exit (0);
    }
/*******************************************************************************
*
* hostGetByName - VxWorks compatible function to return inet address as long
*/

long hostGetByName (hostname)
    char *hostname;

    {
    struct hostent *destHost;

    /* get the internet address for the given host */

    if ((destHost = (struct hostent *) gethostbyname (hostname)) == NULL)
	{
	printf ("%s:non existant host <%s>\n", toolname, hostname);
	return ((u_long)ERROR);
	}

    return (*(u_long *)destHost->h_addr);
    }
/*******************************************************************************
*
* errnoGet - VxWorks compatible function to return errno
*/

int errnoGet ()

    {
    return (errno);
    }
