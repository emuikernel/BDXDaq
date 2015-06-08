/* msgQLibP.h - private message queue library header file */

/* Copyright 1984-2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01h,06sep01,bwa  Added VxWorks events support.
01g,19may98,drm  merged 3rd party code which added external declarations
                 of distributed msgQ routines.
                 - merged code was originally based on version 01d
01f,17apr98,rlp  canceled MSG_Q modification for backward compatibility.
01e,04nov97,rlp  modified MSG_Q structure for tracking messages sent.
01d,16jan94,c_s  added extern declaration for msgQInstClassId.
01c,22sep92,rrr  added support for c++
01b,19jul92,pme  added external declaration of shared msgQ show routine.
01a,04jul92,jcf  created.
*/

#ifndef __INCmsgQLibPh
#define __INCmsgQLibPh

#ifdef __cplusplus
extern "C" {
#endif

#include "vxWorks.h"
#include "vwModNum.h"
#include "msgQLib.h"
#include "classLib.h"
#include "qJobLib.h"
#include "private/objLibP.h"
#include "private/eventLibP.h"


typedef struct msg_q		/* MSG_Q */
    {
    OBJ_CORE		objCore;	/* object management */
    Q_JOB_HEAD		msgQ;		/* message queue head */
    Q_JOB_HEAD		freeQ;		/* free message queue head */
    int			options;	/* message queue options */
    int			maxMsgs;	/* max number of messages in queue */
    int			maxMsgLength;	/* max length of message */
    int			sendTimeouts;	/* number of send timeouts */
    int			recvTimeouts;	/* number of receive timeouts */
    EVENTS_RSRC		events;		/* VxWorks events */
    } MSG_Q;

typedef struct			/* MSG_NODE */
    {
    Q_JOB_NODE		node;		/* queue node */
    int			msgLength;	/* number of bytes of data */
    } MSG_NODE;

#define MSG_NODE_DATA(pNode)   (((char *) pNode) + sizeof (MSG_NODE))

/* variable definitions */

extern CLASS_ID msgQClassId;		/* message queue class id */
extern CLASS_ID msgQInstClassId;	/* message q instrumented class id */

/* shared memory objects function pointers */

extern FUNCPTR  msgQSmSendRtn;
extern FUNCPTR  msgQSmReceiveRtn;
extern FUNCPTR  msgQSmNumMsgsRtn;
extern FUNCPTR  msgQSmShowRtn;

/* distributed objects function pointers */

extern FUNCPTR  msgQDistSendRtn;
extern FUNCPTR  msgQDistReceiveRtn;
extern FUNCPTR  msgQDistNumMsgsRtn;
extern FUNCPTR  msgQDistShowRtn;

/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

extern STATUS	msgQTerminate (MSG_Q_ID msgQId);
extern STATUS	msgQInit (MSG_Q *pMsgQ, int maxMsgs, int maxMsgLength,
			  int options, void *pMsgPool);

#else	/* __STDC__ */

extern STATUS	msgQTerminate ();
extern STATUS	msgQInit ();

#endif	/* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INCmsgQLibPh */
