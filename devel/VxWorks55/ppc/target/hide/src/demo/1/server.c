/* server.c - server process for simple client/server network demo */

static char *copyright = "Copyright 1986-1989, Wind River Systems, Inc.";

/*
modification history
--------------------
01i,31oct91,rrr  passed through the ansification filter
		  -changed copyright notice
01h,25oct90,lpf  put back the HP version's bcopy.
01g,07jun89,gae  used ntohs on SERVER_NUM; changed SOCKADDR to struct sockaddr.
01c,06apr87,gae  caught some lint.  Got rid of excess includes.
01b,12jan87,jlf  minor rearrangement, to meet WRS coding conventions.
		 changed <> to " in includes.
01a,17sep86,llk  written.
*/

/*
DESCRIPTION
This is a simple demonstration of the server-client relationship.
This is the server. The other half of the demonstration is in client.c
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "serverDemo.h"
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
* main - server process
*
* This is a simple server program which communicates with a client
* through a socket.  It reads (recv's) characters, one at a time,
* from the client and echos them to standard output.  When the client
* goes away, the server also goes away (the server reads 0 characters
* from the socket).
*
* The server runs on UNIX, client runs on VxWorks.
*
* See the manual page on sockets for more information.
*/

main ()
    {
    int			sock, snew;	/* socket fd's */
    struct sockaddr_in	serverAddr;	/* server's address */
    struct sockaddr_in 	clientAddr;	/* client's address */
    int			client_len;	/* length of clientAddr */
    char		c;
    extern int		errno;		/* for UNIX error referencing */

    /* Zero out the sock_addr structures.
     * This MUST be done before the socket calls.
     */

    bzero (&serverAddr, sizeof (serverAddr));
    bzero (&clientAddr, sizeof (clientAddr));

    /* Open the socket.
     * Use ARPA Internet address format and stream sockets.
     * Format described in "socket.h".
     */

    sock = socket (AF_INET, SOCK_STREAM, 0);

    if (sock == -1)
	exit (1);

    /* Set up our internet address, and bind it so the client can connect. */

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_NUM);

    printf ("\nBinding SERVER\n", serverAddr.sin_port);

    if (bind (sock, (struct sockaddr *)&serverAddr, sizeof (serverAddr))
							== -1)
	{
	printf ("bind failed, errno = %d\n", errno);
	close (sock);
	exit (1);
	}

    /* Listen, for the client to connect to us. */

    printf ("Listening to client\n");

    if (listen (sock, 2) == -1)
	{
	printf ("listen failed\n");
	close (sock);
	exit (1);
	}

    /* The client has connected.  Accept, and receive chars */

    printf ("Accepting CLIENT\n");

    client_len = sizeof (clientAddr);

    snew = accept (sock, (struct sockaddr *)&clientAddr, &client_len);

    if (snew == -1)
	{
	printf ("accept failed\n");
	close (sock);
	exit (1);
	}

    printf ("CLIENT: port = %d: family = %d: addr = %lx:\n",
		ntohs(clientAddr.sin_port), clientAddr.sin_family,
		ntohl(clientAddr.sin_addr.s_addr));

    /* repeatedly recieve characters from client and put on stdout */

    for (;;)
	{
	if (recv (snew, &c, 1, 0) == 0)
	    {
	    /* client has disappeared */
	    break;
	    }

	putchar (c);
	}

    /* close the socket from the UNIX side */

    close (sock);
    close (snew);

    printf ("\n...goodbye\n");
    }
