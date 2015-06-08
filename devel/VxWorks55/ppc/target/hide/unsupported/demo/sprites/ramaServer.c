/* ramaServer.c - VxWorks RPC demonstration "RAMA" server */

#ifndef LINT
static char *copyright = "Copyright 1988-1991, Wind River Systems, Inc.";
#endif	/* LINT */

/*
modification history
--------------------
02e,31oct91,rrr  passed through the ansification filter
		  -changed includes to have absolute path from h/
		  -fixed #else and #endif
		  -changed VOID to void
		  -changed copyright notice
02d,08aug91,gae  removed usage of lstLib; removed sprite.h; cleanup.
		 Added compiler option for UDP/TCP transport.
02c,25oct90,dnw  removed include of utime.h.
02b,25oct90,dnw  fixed forward declaration of rama().
02a,29jun90,rbr  Moved main() and UNIX routines into rsMain.c.
01d,31may89,dab  now passes drawServer reset condition to simpleSprite.
                 changed drawServer transport from udp to tcp.
		 added hostGetByName() for UNIX version.
01c,19apr89,gae  used rpc error reporting routines; destroyed xprt; now exits
		 when unable to talk to drawServer; UNIX version; lint.
01b,19apr88,gae  fixed-up.
01a,10feb88,rdc  written.
*/

/*
DESCRIPTION
See README.
*/


#include "vxWorks.h"
#include "rpc/rpc.h"
#include "draw.h"
#include "rama.h"
#include "in.h"
#include "socket.h"

/*
TCP version by default
#define	UDP_DRAW_SERVER
#define	UDP_RAMA_SERVER
*/

/*
 * doubly linked list structure & routines declarations
 */

typedef struct dnode		/* Node of a doubly linked list */
    {
    struct dnode *next;		/* Points at the next node in the list */
    struct dnode *previous;	/* Points at the previous node in the list */
    } DNODE;

typedef struct			/* Header for a linked list */
    {
    DNODE dnode;		/* Header list node */
    int count;			/* Number of nodes in list */
    } DLIST;

void dlstInit (/* pList */);
void dlstAdd (/* pList, pNode */);
int dlstCount (/* pList */);
void dlstDelete (/* pList, pNode */);
DNODE *dlstFirst (/* pList */);
DNODE *dlstGet (/* pList */);
void dlstInsert (/* pList, pPrev, pNode */);
DNODE *dlstNext (/* pNode */);


#define SPRITE_SIZE	16	/* cursor/icon size */

typedef struct 	/* SPRITE */
    {
    DNODE links;
    int spriteId;
    BOOL collided;
    int xPos;
    int yPos;
    int xVelocity;
    int yVelocity;
    } SPRITE;


/* local variables */

LOCAL int nextSpriteId = 1;
LOCAL DLIST spriteList;
LOCAL int windowHeight;
LOCAL int windowWidth;
LOCAL char *drawServerHost;
LOCAL struct timeval rpcTimeout = { 10, 0 };
LOCAL CLIENT *pClient;
LOCAL struct sockaddr_in destAddr;
LOCAL int sock = -1;

/* forward declarations */

LOCAL void rama ();
LOCAL SPRITE *findSprite ();

/*******************************************************************************
*
* ramaServer - service sprite update requests
*
* ramaServer keeps track of the position of all active sprites.  When
* a sprite updates its position, ramaServer checks to see if this results
* in a collision with any other sprite, or a wall.  If a collision occurs
* between two sprites, their velocities are swapped.  If a collision occurs
* with a wall, the sprite's velocity in the direction of the wall is
* negated.
*/

void ramaServer (hostName)
    char *hostName; 	/* name of UNIX host on which drawServer resides */

    {
    SVCXPRT *xprt;
    DRAW_UPDATE drawUpdate;
    DRAW_REPLY drawReply;
    int status;

    rpcTaskInit ();

    drawServerHost = hostName;

    /* initialize the list of active sprites */

    dlstInit (&spriteList);

    bzero (&destAddr, sizeof (destAddr));

    destAddr.sin_family      = AF_INET;
    destAddr.sin_addr.s_addr = hostGetByName (drawServerHost);
    destAddr.sin_port        = htons (INADDR_ANY);

    if (destAddr.sin_addr.s_addr == (u_long)ERROR)
	{
	printErr ("ramaServer: nonexistent host <%s>\n", drawServerHost);
	exit (-1);
	}

    /* create a tcp/udp client transport on which to issue calls to server */


#ifdef	UDP_DRAW_SERVER
    pClient = clntudp_create (&destAddr, DRAWSRVR, 0, rpcTimeout, &sock);
#else
    pClient = clnttcp_create (&destAddr, DRAWSRVR, 0, &sock, 512, 512);
#endif	/* UDP_DRAW_SERVER */

    if (pClient == NULL)
        {
        clnt_pcreateerror ("ramaServer: tcp/udp_create");
        return;
        }

    /* ask the drawServer how big our window is */

    drawUpdate.drawType = DR_GET_WINDOW_SIZE;

    status = (int) clnt_call (pClient, 0, xdr_DRAW_UPDATE, &drawUpdate,
			      xdr_DRAW_REPLY, &drawReply, rpcTimeout);

    if (status != (int) RPC_SUCCESS)
	{
	clnt_perror (pClient, "ramaServer: clnt_call");
	return;
	}
    else
	{
	printf ("ramaServer: connected to draw server (draw-%s).\n",
#ifdef  UDP_DRAW_SERVER
            "udp"
#else
            "tcp"
#endif	/* UDP_DRAW_SERVER */
            );

	}

    windowHeight = drawReply.height;
    windowWidth  = drawReply.width;

    /* create a tcp/udp transport to service sprite update requests */

#ifdef	UDP_RAMA_SERVER
    if ((xprt = svcudp_create (RPC_ANYSOCK, 0, 0)) == (SVCXPRT *)NULL)
#else
    if ((xprt = svctcp_create (RPC_ANYSOCK, 0, 0)) == (SVCXPRT *)NULL)
#endif	/* UDP_RAMA_SERVER */
	{
	clnt_pcreateerror ("tcp/udp_create");
	return;
	}

    /* un-register any previous incarnations in the local portmap daemon */

    pmap_unset (RAMA, 0);

    /* register the service with the transport we created */

    status = svc_register (xprt, RAMA, 0, rama,
#ifdef	UDP_DRAW_SERVER
		IPPROTO_UDP);
#else
		IPPROTO_TCP);
#endif	/* UDP_RAMA_SERVER */

    if (status == FALSE)
	{
	printErr ("ramaServer: register failed\n");
	svc_destroy (xprt);
	return;
	}

    printf ("ramaServer: registered (rama-%s)\n",
#ifdef  UDP_RAMA_SERVER
            "udp"
#else
            "tcp"
#endif	/* UDP_RAMA_SERVER */
	    );

    svc_run ();
    }
/*******************************************************************************
*
* rama - handle a sprite's update request
*
* ARGSUSED0
*/

LOCAL void rama (rqstp, xprt)
    struct svc_req *rqstp;
    SVCXPRT *xprt;

    {
    FAST SPRITE *pSprite;
    DRAW_UPDATE drawUpdate;
    RAMA_REPLY ramaReply;
    DRAW_REPLY drawReply;
    RAMA_REQUEST ramaRequest;
    int status;

    /* get the request */

    if (!svc_getargs(xprt, xdr_RAMA_REQUEST, &ramaRequest))
	{
	svcerr_decode (xprt);
	printErr ("svc_getargs trouble!\n");
	return;
	}

    ramaReply.status = ERROR;
    ramaReply.resetFlag = FALSE;

    switch (ramaRequest.command)
	{
	case ADD_SPRITE:
	    if ((pSprite = (SPRITE *) calloc (1, sizeof (SPRITE))) == NULL)
		{
		printErr ("ramaServer: not enough memory\n");
		goto bad;
		}

	    pSprite->spriteId = nextSpriteId++;
	    pSprite->xPos =
		 ramaRequest.RAMA_REQUEST_u.addSprite.initialXPos;
	    pSprite->yPos =
		 ramaRequest.RAMA_REQUEST_u.addSprite.initialYPos;
	    pSprite->xVelocity =
		  ramaRequest.RAMA_REQUEST_u.addSprite.initialXVelocity;
	    pSprite->yVelocity =
		  ramaRequest.RAMA_REQUEST_u.addSprite.initialYVelocity;
	    pSprite->collided = FALSE;

	    dlstAdd (&spriteList, (DNODE *) pSprite);

	    ramaReply.status = pSprite->spriteId;

	    /* draw the sprite */
	    drawUpdate.oldx = 0;
	    drawUpdate.oldy = 0;
	    drawUpdate.newx = pSprite->xPos;
	    drawUpdate.newy = pSprite->yPos;
	    drawUpdate.drawType = DR_INITIAL;

	    status = (int) clnt_call (pClient, 0, xdr_DRAW_UPDATE, &drawUpdate,
				      xdr_DRAW_REPLY, &drawReply, rpcTimeout);

	    if (status != (int) RPC_SUCCESS)
		{
		clnt_perror (pClient, "ramaServer: clnt_call");
		goto bad;
		}

	    if (drawReply.height != windowHeight ||
		drawReply.width  != windowWidth)
		{
		windowHeight = drawReply.height;
		windowWidth  = drawReply.width;
		}

	    /* delete sprites that have exited ungracefully */

	    while (drawReply.resetFlag && dlstCount (&spriteList) > 1)
		free ((char *) dlstGet (&spriteList));

	    break;

	case UPDATE_SPRITE:

	    pSprite =
		findSprite (ramaRequest.RAMA_REQUEST_u.updateSprite.spriteId);

	    if (pSprite == NULL)
		break;

	    /* redraw the sprite */

	    drawUpdate.oldx = pSprite->xPos;
	    drawUpdate.oldy = pSprite->yPos;
	    drawUpdate.newx = ramaRequest.RAMA_REQUEST_u.updateSprite.newXPos;
	    drawUpdate.newy = ramaRequest.RAMA_REQUEST_u.updateSprite.newYPos;
	    drawUpdate.drawType = DR_UPDATE;

	    status = (int) clnt_call (pClient, 0, xdr_DRAW_UPDATE, &drawUpdate,
				      xdr_DRAW_REPLY, &drawReply, rpcTimeout);

	    if (status != (int) RPC_SUCCESS)
		{
		clnt_perror (pClient, "ramaServer: clnt_call");
		goto bad;
		}

	    if (drawReply.height != windowHeight ||
		drawReply.width  != windowWidth)
		{
		windowHeight = drawReply.height;
		windowWidth = drawReply.width;
		}

	    pSprite->xPos = ramaRequest.RAMA_REQUEST_u.updateSprite.newXPos;
	    pSprite->yPos = ramaRequest.RAMA_REQUEST_u.updateSprite.newYPos;

	    /* has someone else collided with us ? */

	    if (pSprite->collided)
		{
		pSprite->collided = FALSE;
		(void) checkForBoundry (pSprite);
		}
	    else
		{
		pSprite->xVelocity =
		      ramaRequest.RAMA_REQUEST_u.updateSprite.newXVelocity;
		pSprite->yVelocity =
		      ramaRequest.RAMA_REQUEST_u.updateSprite.newYVelocity;

		/* have we collided with anything ? */
		if (!checkForBoundry (pSprite))
		    (void)checkForCollision (pSprite);
		}

	    ramaReply.status = OK;
	    ramaReply.xVelocity = pSprite->xVelocity;
	    ramaReply.yVelocity = pSprite->yVelocity;
	    ramaReply.resetFlag = drawReply.resetFlag;
	    break;

	case DELETE_SPRITE:
	    pSprite =
		findSprite (ramaRequest.RAMA_REQUEST_u.deleteSprite.spriteId);
	    if (pSprite == NULL)
		printErr ("ramaServer: no sprite?\n");
	    else
		{
		dlstDelete (&spriteList, (DNODE *)pSprite);

		drawUpdate.oldx = pSprite->xPos;
		drawUpdate.oldy = pSprite->yPos;
		drawUpdate.drawType = DR_FINAL;

		status = (int) clnt_call (pClient, 0, xdr_DRAW_UPDATE,
					  &drawUpdate, xdr_DRAW_REPLY,
					  &drawReply, rpcTimeout);

	        if (status != (int) RPC_SUCCESS)
	            {
	            clnt_perror (pClient, "ramaServer: clnt_call");
		    goto bad;
		    }

		ramaReply.resetFlag = drawReply.resetFlag;
		}
	    break;

	default:
	    printErr ("ramaServer: invalid rama request type %d\n",
		     ramaRequest.command);
	    break;
	}

    if (svc_sendreply (xprt, (xdrproc_t) xdr_RAMA_REPLY, (caddr_t) &ramaReply))
	{
	return;
	}

    printErr ("ramaServer: couldn't reply to RPC call\n");

bad:

    svc_unregister (RAMA, 0);
    svc_destroy (xprt);

    exit (-1);
    }
/*******************************************************************************
*
* checkForCollision - check for collision with other sprites
*
* RETURNS: TRUE collision, or FALSE no collision
*/

LOCAL BOOL checkForCollision (pMySprite)
    FAST SPRITE *pMySprite;

    {
    FAST SPRITE *pSprite;
    FAST int xVelBuf;
    FAST int yVelBuf;
    int relXVel;
    int relYVel;
    int relXPos;
    int relYPos;

    for (pSprite = (SPRITE *) dlstFirst (&spriteList);
	 pSprite != NULL;
	 pSprite = (SPRITE *) dlstNext ((DNODE *) pSprite))
	{
	if (pMySprite->spriteId == pSprite->spriteId)
	    continue;

	if ((abs (pMySprite->xPos - pSprite->xPos) < SPRITE_SIZE) &&
	    (abs (pMySprite->yPos - pSprite->yPos) < SPRITE_SIZE))
	    {
	    /* collision */

	    /* check to see if our relative velocities are opposite */

	    relXPos = pSprite->xPos - pMySprite->xPos;
	    relXVel = pSprite->xVelocity - pMySprite->xVelocity;
	    relYPos = pSprite->yPos - pMySprite->yPos;
	    relYVel = pSprite->yVelocity - pMySprite->yVelocity;

	    if ((((relXVel > 0) && (relXPos >= 0)) ||
		((relXVel < 0) && (relXPos <= 0))) &&

		(((relYVel > 0) && (relYPos >= 0)) ||
		((relYVel < 0) && (relYPos <= 0))) )
		continue;		/* receding, no collision */

	    /* exchange the sprites' velocities (elastic collision) */

	    xVelBuf = pMySprite->xVelocity;
	    yVelBuf = pMySprite->yVelocity;
	    pMySprite->xVelocity = pSprite->xVelocity;
	    pMySprite->yVelocity = pSprite->yVelocity;
	    pSprite->xVelocity = xVelBuf;
	    pSprite->yVelocity = yVelBuf;

	    /* mark the two sprites as collided */
	    pMySprite->collided = TRUE;
	    pSprite->collided = TRUE;
	    return (TRUE);
	    }
	}

    return (FALSE);
    }
/*******************************************************************************
*
* checkForBoundry - check for collision with a wall
*
* RETURNS: TRUE hit wall, or FALSE didn't
*/

LOCAL BOOL checkForBoundry (pMySprite)
    FAST SPRITE *pMySprite;

    {
    /* left wall */

    if ((pMySprite->xPos <= 0) && (pMySprite->xVelocity < 0))
	{
	pMySprite->xVelocity = -pMySprite->xVelocity;
	return (TRUE);
	}

    /* right wall */

    if ((pMySprite->xPos >= windowWidth - SPRITE_SIZE) &&
	(pMySprite->xVelocity > 0))
	{
	pMySprite->xVelocity = -pMySprite->xVelocity;
	return (TRUE);
	}

    /* bottom wall */

    if ((pMySprite->yPos >= windowHeight - SPRITE_SIZE) &&
	(pMySprite->yVelocity > 0))
	{
	pMySprite->yVelocity = -pMySprite->yVelocity;
	return (TRUE);
	}

    /* top wall */

    if ((pMySprite->yPos <= 0) && (pMySprite->yVelocity < 0))
	{
	pMySprite->yVelocity = -pMySprite->yVelocity;
	return (TRUE);
	}

    return (FALSE);
    }
/****************************************************************************
*
* findSprite - find a sprite in the active sprite list given its sprite id
*
* RETURNS: sprite, or NULL
*/

LOCAL SPRITE *findSprite (spriteId)
    int spriteId;

    {
    FAST SPRITE *pMySprite;

    for (pMySprite = (SPRITE *) dlstFirst (&spriteList);
	 pMySprite != NULL;
	 pMySprite = (SPRITE *) dlstNext ((DNODE *) pMySprite))
	{
	if (pMySprite->spriteId == spriteId)
	    return (pMySprite);
	}

    return (NULL);
    }

/*
 * doubly linked list routines -- dlst*
 */

#define HEAD	dnode.next		/* first node in list */
#define TAIL	dnode.previous		/* last node in list */

/*******************************************************************************
*
* dlstInit - initialize a list descriptor
*
* RETURNS: N/A
*/

void dlstInit (pList)
    FAST DLIST *pList;    /* pointer to list descriptor to be initialized */

    {
    pList->HEAD	 = NULL;
    pList->TAIL  = NULL;
    pList->count = 0;
    }
/*******************************************************************************
*
* dlstAdd - add a node to the end of a list
*
* RETURNS: N/A
*/

void dlstAdd (pList, pNode)
    DLIST *pList;	/* pointer to list descriptor */
    DNODE *pNode;	/* pointer to node to be added */

    {
    dlstInsert (pList, pList->TAIL, pNode);
    }
/*******************************************************************************
*
* dlstCount - report the number of nodes in a list
*
* RETURNS:
* The number of nodes in the list.
*/

int dlstCount (pList)
    DLIST *pList;	/* pointer to list descriptor */

    {
    return (pList->count);
    }
/*******************************************************************************
*
* dlstDelete - delete a specified node from a list
*
* RETURNS: N/A
*/

void dlstDelete (pList, pNode)
    FAST DLIST *pList;	/* pointer to list descriptor */
    FAST DNODE *pNode;	/* pointer to node to be deleted */

    {
    if (pNode->previous == NULL)
	pList->HEAD = pNode->next;
    else
	pNode->previous->next = pNode->next;

    if (pNode->next == NULL)
	pList->TAIL = pNode->previous;
    else
	pNode->next->previous = pNode->previous;

    /* update node count */

    pList->count--;
    }
/*******************************************************************************
*
* dlstFirst - find first node in list
*
* RETURNS
* A pointer to the first node in a list, or
* NULL if the list is empty.
*/

DNODE *dlstFirst (pList)
    DLIST *pList;	/* pointer to list descriptor */

    {
    return (pList->HEAD);
    }
/*******************************************************************************
*
* dlstGet - delete and return the first node from a list
*
* RETURNS
* A pointer to the node gotten, or
* NULL if the list is empty.
*/

DNODE *dlstGet (pList)
    FAST DLIST *pList;	/* pointer to list from which to get node */

    {
    FAST DNODE *pNode = pList->HEAD;

    if (pNode != NULL)
	dlstDelete (pList, pNode);

    return (pNode);
    }
/************************************************************************
*
* dlstInsert - insert a node in a list after a specified node
*
* RETURNS: N/A
*/

void dlstInsert (pList, pPrev, pNode)
    FAST DLIST *pList;	/* pointer to list descriptor */
    FAST DNODE *pPrev;	/* pointer to node after which to insert */
    FAST DNODE *pNode;	/* pointer to node to be inserted */

    {
    FAST DNODE *pNext;

    if (pPrev == NULL)
	{				/* new node is to be first in list */
	pNext = pList->HEAD;
	pList->HEAD = pNode;
	}
    else
	{				/* make prev node point fwd to new */
	pNext = pPrev->next;
	pPrev->next = pNode;
	}

    if (pNext == NULL)
	pList->TAIL = pNode;		/* new node is to be last in list */
    else
	pNext->previous = pNode;	/* make next node point back to new */


    /* set pointers in new node, and update node count */

    pNode->next		= pNext;
    pNode->previous	= pPrev;

    pList->count++;
    }
/*******************************************************************************
*
* dlstNext - find the next node in a list
*
* RETURNS:
* A pointer to the next node in the list, or
* NULL if there is no next node.
*/

DNODE *dlstNext (pNode)
    DNODE *pNode;	/* pointer to node whose successor
			 * is to be found */

    {
    return (pNode->next);
    }
