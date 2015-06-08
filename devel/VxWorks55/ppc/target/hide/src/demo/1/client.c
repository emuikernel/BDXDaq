/* client.c - client task for simple client/server network demo */

/* Copyright 1984-1993 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01j,15feb93,jcf  added mixed endian support.
01i,13feb93,jcf  cleaned.
01h,31oct91,rrr  passed through the ansification filter
		  -changed copyright notice
01g,07jun89,gae  used ntohs on SERVER_NUM; changed SOCKADDR to struct sockaddr.
01f,29aug88,gae  fixed printStatus, documentation, allowed inet addresses.
01e,30may88,dnw  changed to v4 names.
01d,28may88,dnw  changed call to fioStdIn to STD_IN.
01c,06apr87,gae  caught some lint.  Got rid of excess includes.
01b,08jan87,jlf  minor rearrangement, to meet coding conventions.
		 changed <> to " in includes.
01a,17sep86,llk  written.
*/

/*
DESCRIPTION
This is a simple demonstration of the server-client relationship.
This is the client. The other half of the demonstration is in server.c

EXAMPLE
On the UNIX host "wrs":
    % server

On the VxWorks target:
    -> ld < client.o
    -> client "wrs"

When you type characters into VxWorks, they will be echoed by the server
on UNIX.  Type control-D to terminate.
*/

#include "vxWorks.h"
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
#include "serverDemo.h"

IMPORT char sysBootHost [];	/* VxWorks saves name of host booted from */

/* make 'clientSock' a global variable so that if something goes wrong
 * when the client runs on VxWorks, the socket can be closed.
 */
int clientSock;		/* socket opened to server */


/*******************************************************************************
*
* client - client task
*
* This is a simple client program which connects to the server
* via a socket.  It reads characters from standard input and sends
* them onto the server through the socket.  It stops when it reads
* 0 characters (receives an EOF).
*
* The server runs on UNIX, client runs on VxWorks.
*
* RETURNS: OK or ERROR
*/

STATUS client (hostName)
    char *hostName; /* name of host running server, 0=boot host */

    {
    struct sockaddr_in	serverAddr;	/* server's address */
    struct sockaddr_in	clientAddr;	/* client's address */
    int		nBytes;			/* number of bytes read from stdin */
    char	c;

    if (hostName == NULL)
	hostName = sysBootHost;

    /* Zero out the sock_addr structures.
     * This MUST be done before the socket call.
     */

    bzero ((char *) &serverAddr, sizeof (serverAddr));
    bzero ((char *) &clientAddr, sizeof (clientAddr));

    /* Open the socket.
     * Use ARPA Internet address format and stream sockets.
     * Format described in "socket.h".
     */

    clientSock = socket (AF_INET, SOCK_STREAM, 0);

    if (clientSock == ERROR)
	return (ERROR);

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port   = htons(SERVER_NUM);

    /* get server's Internet address */

    if ((serverAddr.sin_addr.s_addr = inet_addr (hostName)) == ERROR &&
	(serverAddr.sin_addr.s_addr = hostGetByName (hostName)) == ERROR)
	{
	printf ("Invalid host: \"%s\"\n", hostName);
	printErrno (errnoGet ());
	close (clientSock);
	return (ERROR);
	}

    printf ("Server's address is %x:\n", htonl (serverAddr.sin_addr.s_addr));

    if (connect (clientSock, (struct sockaddr *)&serverAddr,
		    sizeof (serverAddr)) == ERROR)
	{
	printf ("Connect failed:\n");
	printErrno (errnoGet ());
	close (clientSock);
	return (ERROR);
	}

    printf ("Connected...\n");

    /* repeatedly read from standard input and send to socket until EOF */

    while (TRUE)
	{
	/* read a character from standard input */

	if ((nBytes = read (STD_IN, &c, 1)) == 0)
	    {
	    /* client read an EOF; exit the loop. */
	    break;
	    }
	else if (nBytes != 1)
	    {
	    printf ("CLIENT read error, %d bytes read\n", nBytes);
	    printErrno (errnoGet ());
	    break;
	    }

	/* send byte to server */

	if (send (clientSock, &c, 1, 0) != 1)
	    {
	    printf ("CLIENT write error:\n");
	    printErrno (errnoGet ());
	    }
	}

    /* close socket from the VxWorks side */

    close (clientSock);

    printf ("\n...goodbye\n");
    return (OK);
    }
