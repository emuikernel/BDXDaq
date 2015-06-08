/* simpleSprite.c - VxWorks RPC demonstration client */

#ifndef	LINT
static char *copyright = "Copyright 1988-1991, Wind River Systems, Inc.";
#endif	/* LINT */

/*
modification history
--------------------
02d,31oct91,rrr  passed through the ansification filter
		  -changed includes to have absolute path from h/
		  -fixed #else and #endif
		  -changed VOID to void
		  -changed copyright notice
02c,08aug91,gae  added compile option for TCP/UDP transports; cleanup.
02b,25oct90,dnw  removed include of utime.h.
02a,29jun90,rbr  Moved main() and UNIX routines into ssMain.c.
01f,31may89,dab  added check for drawServer reset for sprite deletion.
01e,19apr89,gae  used rpc error reporting routines; UNIX version; lint.
01d,07jun88,gae  more name changes.
01c,30may88,dnw  changed to v4 names.
01b,17apr88,gae  made simpleSprite not loop forever, sends final
		   DELETE_SPRITE message, close sockets, destroys client.
01a,10feb88,rdc  written.
*/

/*
DESCRIPTION
This is a VxWorks demonstration program that
connects to the RPC server `ramaServer', and calculates
sprite positions to be displayed ultimately on a UNIX host.
*/


#include "vxWorks.h"
#include "rpc/rpc.h"
#include "rama.h"
#include "in.h"
#include "socket.h"

/*
TCP version by default
#define	UDP_RAMA_SERVER
*/

BOOL spriteLives = TRUE;


/*******************************************************************************
*
* simpleSprite - sprite demo task
*
* simpleSprite connects to the RPC server `ramaServer', and calculates
* sprite positions to be displayed ultimately on a UNIX host.
*/

void simpleSprite (ramaHost, initXVel, initYVel)
    char *ramaHost; 	/* name of vxWorks host on which ramaServer resides */
    int initXVel;	/* initial x velocity */
    int initYVel;	/* initial y velocity */

    {
    /* number of seconds to wait before timing out on rpc call */
    static struct timeval rpcTimeout = { 10, 0 };	/* sec, usecs */
    RAMA_REQUEST ramaRequest;
    RAMA_REPLY ramaReply;
    int status;
    int mySpriteId;
    int myXPos;
    int myYPos;
    int myXVelocity;
    int myYVelocity;
    CLIENT *pClient;
    struct sockaddr_in destAddr;
    int sock = -1;

    rpcTaskInit ();

    bzero (&destAddr, sizeof (destAddr));

    destAddr.sin_family      = AF_INET;
    destAddr.sin_addr.s_addr = hostGetByName (ramaHost);
    destAddr.sin_port        = htons (INADDR_ANY);

    if (destAddr.sin_addr.s_addr == (u_long)ERROR)
	{
        printErr ("simpleSprite: nonexistent host <%s>\n", ramaHost);
	return;
	}

    /* create a tcp/udp client transport on which to issue calls to rama */

#ifdef	UDP_RAMA_SERVER
    pClient = clntudp_create (&destAddr, RAMA, 0, rpcTimeout, &sock);
#else
    pClient = clnttcp_create (&destAddr, RAMA, 0, &sock, 512, 512);
#endif	/* UDP_RAMA_SERVER */

    if (pClient == NULL)
	{
	clnt_pcreateerror ("simpleSprite: tcp/udp_create");
	return;
	}

    ramaRequest.command = ADD_SPRITE;
    ramaRequest.RAMA_REQUEST_u.addSprite.initialXPos      = 100;
    ramaRequest.RAMA_REQUEST_u.addSprite.initialYPos      = 100;
    ramaRequest.RAMA_REQUEST_u.addSprite.initialXVelocity = initXVel;
    ramaRequest.RAMA_REQUEST_u.addSprite.initialYVelocity = initYVel;

    /* issue the add sprite request */

    status = (int) clnt_call (pClient, 0, xdr_RAMA_REQUEST, &ramaRequest,
			      xdr_RAMA_REPLY, &ramaReply, rpcTimeout);

    if (status != (int) RPC_SUCCESS)
	{
	clnt_perror (pClient, "simpleSprite: clnt_call (1)");
	goto goodbye;
	}

    mySpriteId = ramaReply.status;

    myXVelocity = initXVel;
    myYVelocity = initYVel;
    myXPos      = 100;
    myYPos      = 100;

    ramaRequest.command = UPDATE_SPRITE;
    ramaRequest.RAMA_REQUEST_u.updateSprite.spriteId = mySpriteId;

    while (spriteLives && !ramaReply.resetFlag)
	{
	taskDelay (1);	/* go to sleep for a clock tick */

	ramaRequest.RAMA_REQUEST_u.updateSprite.newXVelocity = myXVelocity;
	ramaRequest.RAMA_REQUEST_u.updateSprite.newYVelocity = myYVelocity;
	ramaRequest.RAMA_REQUEST_u.updateSprite.newXPos      = myXPos;
	ramaRequest.RAMA_REQUEST_u.updateSprite.newYPos      = myYPos;

	status = (int) clnt_call (pClient, 0, xdr_RAMA_REQUEST, &ramaRequest,
				  xdr_RAMA_REPLY, &ramaReply, rpcTimeout);

	if (status != (int) RPC_SUCCESS)
	    {
	    clnt_perror (pClient, "simpleSprite: clnt_call (2)");
	    goto goodbye;
	    }

	myXVelocity = ramaReply.xVelocity;
	myYVelocity = ramaReply.yVelocity;
	myXPos     += myXVelocity;
	myYPos     += myYVelocity;
	}

    /* send death notice */

    ramaRequest.command = DELETE_SPRITE;

    status = (int) clnt_call (pClient, 0, xdr_RAMA_REQUEST, &ramaRequest,
			    xdr_RAMA_REPLY, &ramaReply, rpcTimeout);

    if (status != (int) RPC_SUCCESS)
	{
	clnt_perror (pClient, "simpleSprite: clnt_call");
	}

goodbye:
    clnt_destroy (pClient);
    close (sock);
    }
