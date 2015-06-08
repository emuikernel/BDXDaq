/* q_Lib.c -  Fixed and Variable Queue Routines */

/* Copyright 1984-1996 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01h,22jan97,jmb  Add casts to get rid of compilation warnings.
01g,21jan97,jmb  Don't call taskLock when broadcasting at interrupt level.
01f,09jan97,jmb  Make queue send functions callable from interrupt level.
01e,08jan97,jmb  Improve error handling to avoid segfaults on bad qid's.
01d,16oct96,jmb  handle ident call before object table initialized
01c,14may96,jmb  corrected some error codes.
01b,16apr96,jmb  added q_vdelete, reformatted, etc.
01a,20mar96,cym  created
*/

#include "vxWorks.h"
#include "private/msgQLibP.h"
#include "p2vLib.h"
#include "msgQLib.h"
#include "taskLib.h"
#include "intLib.h"
#include "qLib.h"

IMPORT P2V_TABLE_NODE p2vFixQArray[];
IMPORT int p2vMaxFixQ;

IMPORT P2V_TABLE_NODE p2vVarQArray[];
IMPORT int p2vMaxVarQ;

/* Locals */

LOCAL P2V_TABLE *pFixQTbl = NULL;
LOCAL P2V_TABLE *pVarQTbl = NULL;
LOCAL char nameFixedQTbl[] = "Fixed Length Queues";
LOCAL char nameVarQTbl[] = "Variable Length Queues";

/*****************************************************************************
*
* q_create - create a fixed length message queue.
*
* INCLUDE FILES: p2vLib.h
*
* This function creates a message queue with a user-defined number
* of 16-bytes messages.  The Q_NOLIMIT option is unsupported.  A user
* must specify the Q_LIMIT option flag and the maximum number of messages.
* pSOS+(r) allows a count of zero, but this will fail in vxWorks.
*
* RETURNS:
* EOK if successful.
* ERR_NOMGB if unable to allocate message buffers.
* ERR_OBJTFULL if local object table for variable length queues is full.
* ERROR if an unknown failure in msgQCreate.
*/

UINT32 q_create
    (
    NAME_T(name),			/* four-byte queue name */
    UINT32 count,			/* max number of messages */
    UINT32 flags,			/* queue options */
    UINT32 *qid			/* queue id returned by this call */
    )
    {
    int vxOptions;			/* vxWorks queue options */

    /* convert pSOS+ options to vxWorks options */

    if(flags & Q_PRIOR)
	vxOptions=MSG_Q_PRIORITY;
    else
	vxOptions=MSG_Q_FIFO;

    if (!(flags & Q_LIMIT))  /* Unlimited queues not supported */
	{
	p2vUnsupported ("Q_NOLIMIT option not supported",
	    0, 0, 0, 0, 0, 0 );
	return (ERROR);
	}

    if ( count == 0)         /* Zero-length queues not supported */
	{
	p2vUnsupported ("Zero-length queues not supported",
	    0, 0, 0, 0, 0, 0 );
	return (ERROR);
	}

    /* create the message queue */

    *qid = (int) msgQCreate (count, 16, vxOptions);

    /* check for errors in queue creation */

    if(*qid == ERROR)
        {
	if (errno == S_memLib_NOT_ENOUGH_MEMORY) 
	    return (ERR_NOMGB);
	return (ERROR);
	}

    /* add the queue to the queue object table */

    if (pFixQTbl == NULL)
        pFixQTbl = p2vTblCreate (nameFixedQTbl, p2vFixQArray, p2vMaxFixQ);

    if (p2vTblAdd (pFixQTbl, NAME_TO_INT(name), (void *) *qid, NULL) == ERROR)
	{
	msgQDelete ((MSG_Q_ID) *qid);
	return (ERR_OBJTFULL);
	}
    return (EOK);
    } 

/*****************************************************************************
*
* q_delete - fixed length queue deletion routine
*
* INCLUDE FILES: p2vLib.h
*
* Delete a fixed length queue.
*
* RETURNS: 
* EOK if successful
* ERR_OBJID if <qid> is invalid
* ERR_OBJDEL if <qid> if vxWorks msgQDelete detects on invalid queue id.
* ERR_VARQ if queue is a variable length queue
* ERR_TATQDEL if tasks pended at queue on deletion (warning only)
* ERR_MATQDEL if messages in queue on delete (warning only)
* ERROR if an unknown failure in msgQDelete
*/

UINT32 q_delete
    (
    UINT32 qid			/* id of queue to delete */
    )
    {
    int 	rvalue = EOK;		/* keeps message code */
    MSG_Q_ID 	msgQId = (MSG_Q_ID) qid;
    unsigned int rc;

    /* 
    * make sure the queue is in the proper object table, 
    * return the appropriate error if it is not found, or is in the wrong table
    */
    if (pFixQTbl)
       rc = (unsigned int) p2vObjToShadow (pFixQTbl, (void *) qid);
    else
       rc = ERROR;

    if (rc == ERROR)
       {
       if (pVarQTbl)
	  if (p2vObjToShadow (pVarQTbl, (void *) qid) != (void *) ERROR)
	    return(ERR_VARQ);
       return(ERR_OBJID);
       }

    /* 
    * if there are tasks pending on the queue, eventually return the 
    * "informative message" ERR_TATQDEL
    */
    if (Q_FIRST (&msgQId->msgQ.pendQ) != NULL)
	rvalue = ERR_TATQDEL;
    /*
    * if there are messages in the queue, eventually return the 
    * "informative message" ERR_MATQDEL
    */
    if (msgQId->msgQ.count != 0)
	rvalue = ERR_MATQDEL;

    /* delete the queue and convert any errors returned */

    if (msgQDelete (msgQId) == ERROR)
	{
	if (errno==S_objLib_OBJ_ID_ERROR) 
	    rvalue = ERR_OBJDEL;
	rvalue = ERROR;
	}

    /* remove the queue from the fixed length table */
    p2vTblRemove (pFixQTbl, (void *) qid);

    return (rvalue);
    }

/*****************************************************************************
*
* q_send - fixed length queue send routine
*
* INCLUDE FILES:  p2vLib.h
*
* This function sends a 16-byte message to a queue.
*
* RETURNS: 
* EOK if successful
* ERR_OBJID if <qid> is invalid.
* ERR_OBJDEL if <qid> is invalid in msgQSend.
* ERR_VARQ if target queue is a variable length message queue.
* ERR_QFULL if queue is full
* ERR_OBJDEL if msgQSend detects a deleted queue.
* ERROR if an unknown error in msgQSend.
*/

UINT32 q_send
    (
    UINT32 qid,			/* ID of queue to send message to */
    UINT32 msg_buf[4]		/* pointer to data (4 long int s) */
    )
    {
    int 	head;           /* used to schedule head of wait queue */
    int 	mypri;          /* used to schedule head of wait queue */
    int 	headpri;	/* used to schedule head of wait queue */
    MSG_Q_ID 	msgqid = (MSG_Q_ID) qid;	/* clean typecast of qid */
    unsigned int rc;
    FUNCPTR func_ObjToShadow;

    if (!INT_CONTEXT ())
       func_ObjToShadow = (FUNCPTR) p2vObjToShadow;
    else
       func_ObjToShadow = (FUNCPTR) p2vObjToShadowInt;
    /* 
    * make sure the queue is in the proper object table, 
    * return the appropriate error if it is not found, or is in the wrong table
    */

    if (pFixQTbl)
       rc = (unsigned int) func_ObjToShadow (pFixQTbl, (void *) qid);
    else
       rc = ERROR;

    if (rc == ERROR)
       {
       if (pVarQTbl)
	  if (func_ObjToShadow (pVarQTbl, (void *) qid) != ERROR)
	    return (ERR_VARQ);
       return (ERR_OBJID);
       }

    /* get the priority of the task at the start of the wait queue */

    head = (int) Q_FIRST (&msgqid->msgQ.pendQ);

    /* Get my priority, and the priority of the task at the head of the queue
    *  These are needed to determine whether we should exit through the 
    *  scheduler.
    */

    taskPriorityGet (taskIdSelf (), &mypri);

    if (head != NULL) 
       taskPriorityGet (head, &headpri);

    /* and send the message */

    if (msgQSend( msgqid, (char *) msg_buf, 16, NO_WAIT, MSG_PRI_NORMAL) 
	== ERROR)
	{
	if (errno == S_objLib_OBJ_ID_ERROR) 
	    return (ERR_OBJDEL);
	if (errno == S_objLib_OBJ_UNAVAILABLE) 
	    return (ERR_QFULL);
	if (errno == S_objLib_OBJ_DELETED) 
	    return (ERR_OBJDEL);
        return (ERROR);
	}

    /* reschedule if the head was of greater priority */

    if( !(INT_CONTEXT ()) && (headpri > mypri))
	taskDelay (0);

    return (EOK);
    }

/*****************************************************************************
*
* q_urgent - fixed length queue urgent send routine
*
* INCLUDE FILES:  p2vLib.h
*
* This function sends a 16-byte message to the head of a queue.
*
* RETURNS: 
* EOK if successful
* ERR_OBJID if <qid> is invalid.
* ERR_VARQ if target queue is a variable length message queue.
* ERR_QFULL if queue is full
* ERR_OBJDEL if msgQSend detects a deleted queue or if <qid> is invalid
*            in msgQSend.
* ERROR if an unknown error in msgQSend.
*/

UINT32 q_urgent
    (
    UINT32 qid,			/* ID of queue to send message to */
    UINT32 msg_buf[4]		/* pointer to data (4 long int s) */
    )
    {
    int 	head; 		/* used to schedule head of wait queue */
    int 	mypri; 		/* used to schedule head of wait queue */
    int 	headpri;	/* used to schedule head of wait queue */
    MSG_Q_ID 	msgqid = (MSG_Q_ID) qid;     /* clean typecast of qid */
    unsigned int rc;
    FUNCPTR func_ObjToShadow;

    if (!INT_CONTEXT ())
       func_ObjToShadow = (FUNCPTR) p2vObjToShadow;
    else
       func_ObjToShadow = (FUNCPTR) p2vObjToShadowInt;
    /* 
    * make sure the queue is in the proper object table, 
    * return the appropriate error if it is not found, or is in the wrong table
    */

    if (pFixQTbl)
       rc = (unsigned int) func_ObjToShadow (pFixQTbl, (void *) qid);
    else
       rc = ERROR;

    if (rc == ERROR)
       {
       if (pVarQTbl)
          if (func_ObjToShadow (pVarQTbl, (void *) qid) != ERROR)
	     return (ERR_VARQ);
       return (ERR_OBJID);
       }

    /* get find the priority of the head of the pend queue */

    head = (int) Q_FIRST (&msgqid->msgQ.pendQ);
    taskPriorityGet (head, &headpri);
    taskPriorityGet (taskIdSelf(), &mypri);

    /* send the message and check for errors */

    if (msgQSend ((MSG_Q_ID) qid, (char *) msg_buf, 16, NO_WAIT, MSG_PRI_URGENT)
	== ERROR)
	{
	if (errno == S_objLib_OBJ_ID_ERROR)
	    return (ERR_OBJDEL);
	if (errno == S_objLib_OBJ_UNAVAILABLE)
	    return (ERR_QFULL);
	if (errno == S_objLib_OBJ_DELETED)
	    return (ERR_OBJDEL);
        return (ERROR);
	}

    /* if neccessary, allow head of pend queue to run */

    if( !(INT_CONTEXT()) && (headpri > mypri))
	taskDelay (0);

    return (EOK);
    }

/*****************************************************************************
*
* q_receive - fixed length message queue receive routine
*
* INCLUDE FILES: p2vLib.h
*
* Receive a 16-byte message from a queue
*
* RETURNS:
* EOK if successful
* ERR_OBJDEL if <qid> is invalid in call to msgQReceive.
* ERR_OBJID if <qid> is invalid.
* ERR_VARQ if target queue is a variable length message queue.
* ERR_TIMEOUT if timed out waiting for a message.
* ERR_NOMSG if doing a NO_WAIT receive and there is no message in the queue.
* ERR_QKILLD if queue deleted while waiting for a message.
* ERROR if unknown error in msgQReceive
*/

UINT32 q_receive
    (
    UINT32 qid,			/* ID of queue to receive from */
    UINT32 flags,		/* options */
    UINT32 timeout,		/* ticks to wait(0=forever) */
    UINT32 msg_buf[4]		/* 16 byte(4 long int) message */
    )
    {
    unsigned int rc;
    /* 
    * make sure the queue is in the proper object table, 
    * return the appropriate error if it is not found, or is in the wrong table
    */
    if (pFixQTbl)
       rc = (unsigned int) p2vObjToShadow (pFixQTbl, (void *) qid);
    else 
       rc = ERROR;

    if (rc == ERROR)
       {
       if (pVarQTbl)
	  if (p2vObjToShadow (pVarQTbl, (void *) qid) != (void *) ERROR)
	    return (ERR_VARQ);
       return (ERR_OBJID);
       }

    /* convert various timeouts to vxWorks */

    if (timeout == 0) 
	timeout = WAIT_FOREVER;
    if (flags & Q_NOWAIT) 
	timeout = NO_WAIT;

    /* and receive the message, checking for errors */

    if (msgQReceive((MSG_Q_ID) qid, (char *) msg_buf, 16, timeout) == ERROR)
	{
	if (errno == S_objLib_OBJ_ID_ERROR) 
	    return (ERR_OBJDEL);
	if (errno == S_objLib_OBJ_TIMEOUT) 
	    return (ERR_TIMEOUT);
	if (errno == S_objLib_OBJ_UNAVAILABLE) 
	    return (ERR_NOMSG);
	if (errno == S_objLib_OBJ_DELETED) 
	    return (ERR_QKILLD);
        return (ERROR);
	}

    return (EOK);
    }

/*****************************************************************************
*
* q_broadcast - fixed length message queue broadcast routine
*
* INCLUDE FILES:  p2vLib.h
*
* This function sends a 16-byte message to all tasks waiting at
* a given message queue.
*
* RETURNS: 
* EOK if successful
* ERR_OBJID if <qid> is invalid.
* ERR_VARQ if target queue is a variable length message queue.
* ERR_OBJDEL if msgQSend detects a deleted queue.
* ERROR if an unknown error in msgQSend.
*/

UINT32 q_broadcast
    (
    UINT32 qid,		/* ID of queue to broadcast to */
    UINT32 msg_buf[4],	/* 16-byte message */
    UINT32 *count		/* return # of task broadcast to here */
    )
    {
    int 	rvalue = EOK;	/* keeps track of any errors */
    int 	head;		/* used to schedule released tasks */
    int		mypri;		/* used to schedule released tasks */
    int 	headpri;	/* used to schedule released tasks */
    int 	hipri = 0;	/* used to schedule released tasks */
    MSG_Q_ID 	msgqid = (MSG_Q_ID) qid;	/* clean typecast of qid */
    unsigned int rc;
    FUNCPTR func_ObjToShadow;

    if (!INT_CONTEXT ())
       func_ObjToShadow = (FUNCPTR) p2vObjToShadow;
    else
       func_ObjToShadow = (FUNCPTR) p2vObjToShadowInt;
    /* 
    * make sure the queue is in the proper object table, 
    * return the appropriate error if it is not found, or is in the wrong table
    */
    if (pFixQTbl)
       rc = (unsigned int) func_ObjToShadow (pFixQTbl, (void *) qid);
    else
       rc = ERROR;

    if (rc == ERROR)
       {
       if (pVarQTbl)
	  if (func_ObjToShadow (pVarQTbl, (void *) qid) != ERROR)
	     return (ERR_VARQ);
       return (ERR_OBJID);
       }

    *count=0;

    /* get this tasks priority to check for scheduling */

    taskPriorityGet (taskIdSelf(), &mypri);

    /* make sure no one butts in while you're broadcasting */

    if (!INT_CONTEXT ())
        taskLock();					/* TASKLOCK */

    /* broadcast until the waiting queue is empty */

    while ((head = (int) Q_FIRST (&msgqid->msgQ.pendQ)) != NULL)
	{
	/* keep track of the highest priority task for scheduling */

        taskPriorityGet (head, &headpri);
	if (headpri > hipri) 
	    hipri = headpri;

	/* send the message to the first waiting task */
        if (msgQSend( (MSG_Q_ID) qid, (char *) msg_buf, 16, NO_WAIT, 
	    MSG_PRI_URGENT) == ERROR)
	    {
            rvalue = ERROR;
	    break;
	    }

	/* and increment the count */
	*count = *count + 1;
        }
    if (!INT_CONTEXT ())
        taskUnlock();				/* TASK UNLOCK */

    /* handle any errors encountered */

    if (rvalue == ERROR)
	{
	if (errno == S_objLib_OBJ_ID_ERROR) 
	    return (ERR_OBJID);
	if (errno == S_objLib_OBJ_DELETED) 
	    return (ERR_OBJDEL);
        return (ERROR);
	}

    /* if a higher priority task was freed, let it work */

    if( !(INT_CONTEXT()) && (mypri > hipri) )
        taskDelay (0);

    return (rvalue);
    }

/*****************************************************************************
*
* q_ident - fixed length queue identification routine
*
* INCLUDE FILES: p2vLib.h

* Check both queue local object tables (fixed and variable)
* for the queue name, and return its ID.  Note:  global queues
* are currently unsupported.
*
* RETURNS:
* ERR_NODENO if <node> indicates a remote processor (is not 0).
* ERR_OBJNF if <name> is not found in object tables.
*/

UINT32 q_ident
    (
    NAME_T (name),
    UINT32 node,
    UINT32 *qid
    )
    {
    /* the API does not currently support multiproccessor systems */

    if (node != 0) 
        return (ERR_NODENO);

    /* look in the fixed length name queue table for the object */

    if (pFixQTbl)
        *qid = (int) p2vNameToObj (pFixQTbl, NAME_TO_INT(name));
    else
	*qid = ERROR;

    if (*qid == ERROR)
	{
	/* if it isn't there, look in the variable length queue name table */

        if (pVarQTbl)
           *qid = (int) p2vNameToObj (pVarQTbl, NAME_TO_INT(name));

	/* if it isn't there, it isn't a p2v queue */

	if (*qid == ERROR) 
	    return (ERR_OBJNF);
        }

    return (EOK);
    }

/*****************************************************************************
*
* Variable Length Message Queue Routines 
*
******************************************************************************/

/*****************************************************************************
*
* q_vident - variable length queue identification routine
*
* INCLUDE FILES: p2vLib.h

* Check both queue local object tables (fixed and variable)
* for the queue name, and return its ID.  Note:  global queues
* are currently unsupported.
*
* RETURNS:
* EOK if successful.
* ERR_NODENO if <node> indicates a remote processor (is not 0).
* ERR_OBJNF if <name> is not found in object tables.
*/

UINT32 q_vident
    (
    NAME_T (name),		/* name of the variable length queue */
    UINT32 node,		/* node it resides on */
    UINT32 *qid			/* return queue ID here */
    )
    {
    /* the API does not currently support multiproccessor systems */

    if (node != 0) 
	return (ERR_NODENO);

    /* check in the variable length queue name table */
       
    if (pVarQTbl)
       *qid = (int) p2vNameToObj (pVarQTbl, NAME_TO_INT(name));
    else
       *qid = ERROR;

    if (*qid == ERROR)
	{
	/* if it isn't there, check in the fixed length queue table */

        if (pFixQTbl)
           *qid = (int) p2vNameToObj (pFixQTbl, NAME_TO_INT(name));

	/* if it isn't there, it isn't a p2v queue */

	if (*qid == ERROR) 
	    return (ERR_OBJNF);
        }

    return (EOK);
    }
     
/*****************************************************************************
*
* q_vcreate - create a variable length message queue.
*
* INCLUDE FILES: p2vLib.h
*
* This function creates a message queue with a user-defined number
* of variable-length messages.  The maximum number of messages must
* be supplied. pSOS+(R) allows a zero length queue, but this will fail in vxWorks.
*
* RETURNS:
* EOK if successful.
* ERR_NOMGB if unable to allocate message buffers.
* ERR_OBJTFULL if local object table for variable length queues is full.
* ERROR if an unknown failure in msgQCreate.
*/

UINT32 q_vcreate
    (
    NAME_T(name),			/* 4-byte name of the queue */
    UINT32 flags,			/* flags for queue type */
    UINT32 count,			/* max # of messages */
    UINT32 maxlength,		/* max length of messages */
    UINT32 *qid			/* ID of the queue is returned here */
    )
    {
    int vxOptions;			/* equivalent vxWorks options */

    /* convert to vxWorks options */
    if (flags & Q_PRIOR)
	vxOptions = MSG_Q_PRIORITY;
    else
	vxOptions = MSG_Q_FIFO;

    /* create the message queue */
    *qid = (int) msgQCreate (count, maxlength, MSG_Q_PRIORITY);

    /* convert vxWorks error codes */
    if (*qid == ERROR)
	{
	if (errno == S_memLib_NOT_ENOUGH_MEMORY) 
	    return (ERR_NOMGB);
	return (ERROR);
	}

    /* add the queue to the variable length que object table */

    if (pVarQTbl == NULL)
        pVarQTbl = p2vTblCreate (nameVarQTbl, p2vVarQArray, p2vMaxVarQ);

    if (p2vTblAdd (pVarQTbl, NAME_TO_INT(name), (void *) *qid, NULL) == ERROR)
	{
	msgQDelete ((MSG_Q_ID) *qid);
	return (ERR_OBJTFULL);
	}

    return (EOK);
    } 

/*****************************************************************************
*
* q_vdelete - variable length queue deletion routine
*
* INCLUDE FILES: p2vLib.h
*
* Delete a variable length queue.
*
* RETURNS: 
* EOK if successful
* ERR_OBJID if <qid> is invalid
* ERR_OBJDEL if <qid> if vxWorks msgQDelete detects on invalid queue id.
* ERR_NOTVARQ if queue is a fixed length queue
* ERR_TATQDEL if tasks pended at queue on deletion (warning only)
* ERR_MATQDEL if messages in queue on delete (warning only)
* ERROR if an unknown failure in msgQDelete
*/

UINT32 q_vdelete
    (
    UINT32 qid			/* id of queue to delete */
    )
    {
    int 	rvalue = EOK;		/* return message code */
    MSG_Q_ID 	msgQId = (MSG_Q_ID) qid;
    unsigned int rc;

    /* 
    * make sure the queue is in the proper object table, 
    * return the appropriate error if it is not found, or is in the wrong table
    */
    if (pVarQTbl)
       rc = (unsigned int) p2vObjToShadow (pVarQTbl, (void *) qid);
    else
       rc = ERROR;

    if (rc == ERROR)
       {
       if (pFixQTbl)
	  if (p2vObjToShadow (pFixQTbl, (void *) qid) != (void *) ERROR)
	     return(ERR_NOTVARQ);
       return(ERR_OBJID);
       }

    /* 
    * if there are tasks pending on the queue, eventually return the 
    * "informative message" ERR_TATQDEL
    */
    if (Q_FIRST (&msgQId->msgQ.pendQ) != NULL)
	rvalue=ERR_TATQDEL;
    /*
    * if there are messages in the queue, eventually return the 
    * "informative message" ERR_MATQDEL
    */
    if (msgQId->msgQ.count != 0)
	rvalue=ERR_MATQDEL;

    /* delete the queue and convert any errors returned */
    if (msgQDelete (msgQId) == ERROR)
	{
	if(errno==S_objLib_OBJ_ID_ERROR) 
	    rvalue = ERR_OBJDEL;
	rvalue = ERROR;
	}

    /* remove the queue from the fixed length table */

    p2vTblRemove (pVarQTbl, (void *) qid);

    return (rvalue);
    }

/*****************************************************************************
*
* q_vsend - variable length queue send routine
*
* INCLUDE FILES:  p2vLib.h
*
* This function sends a variable-length message to a queue.
*
* RETURNS: 
* EOK if successful
* ERR_OBJID if <qid> is invalid.
* ERR_NOTVARQ if target queue is a fixed length message queue.
* ERR_QFULL if queue is full
* ERR_OBJDEL if msgQSend detects a deleted queue or an invalid <qid>.
* ERR_MSGSIZ if <msg_len> is invalid.
* ERROR if an unknown error in msgQSend.
*/

UINT32 q_vsend
    (
    UINT32 qid,		/* ID of queue to send to */
    void *msg_buf,		/* pointer to variable length message */
    UINT32 msg_len	/* length of message */
    )
    {
    int 	head;			/* used to schedule head of queue */
    int		mypri;			/* used to schedule head of queue */
    int		headpri;		/* used to schedule head of queue */
    MSG_Q_ID 	msgqid = (MSG_Q_ID) qid;	/* clean typecast of qid */
    unsigned int rc;
    FUNCPTR func_ObjToShadow;

    if (!INT_CONTEXT ())
       func_ObjToShadow = (FUNCPTR) p2vObjToShadow;
    else
       func_ObjToShadow = (FUNCPTR) p2vObjToShadowInt;
    /* 
    * make sure the queue is in the proper object table, 
    * return the appropriate error if it is not found, or is in the wrong table
    */

    if (pVarQTbl)
       rc = (unsigned int) func_ObjToShadow (pVarQTbl, (void *) qid);
    else
       rc = ERROR;

    if (rc == ERROR)
       {
       if (pFixQTbl)
          if (func_ObjToShadow (pFixQTbl, (void *) qid) == ERROR)
	     return (ERR_NOTVARQ);
       return (ERR_OBJID);
       }

    /* get the priority of the caller and the head of the queue */

    head = (int) Q_FIRST (&msgqid->msgQ.pendQ);
    taskPriorityGet (taskIdSelf(), &mypri);
    if (head != NULL) 
	taskPriorityGet (head, &headpri);

    if (msgQSend (msgqid, (char *) msg_buf, msg_len, NO_WAIT, MSG_PRI_NORMAL) 
	== ERROR)
	{
	if (errno == S_objLib_OBJ_ID_ERROR) 
	    return (ERR_OBJDEL);
	if (errno == S_objLib_OBJ_UNAVAILABLE) 
	    return (ERR_QFULL);
	if (errno == S_objLib_OBJ_DELETED) 
	    return (ERR_OBJDEL);
	if (errno == S_msgQLib_INVALID_MSG_LENGTH) 
	    return(ERR_MSGSIZ);
        return (ERROR);
	}

    /* if the head is higher priority, let it go */

    if( !(INT_CONTEXT()) && (headpri > mypri) )
	taskDelay (0);

    return (EOK);
    }

/*****************************************************************************
*
* q_vurgent - variable length queue urgent message send
*
* INCLUDE FILES:  p2vLib.h
*
* This function sends a variable-length message to the head of
* a message queue.
*
* RETURNS: 
* EOK if successful
* ERR_OBJID if <qid> is invalid.
* ERR_NOTVARQ if target queue is a variable length message queue.
* ERR_QFULL if queue is full
* ERR_OBJDEL if msgQSend detects a deleted queue.
* ERR_MSGSIZ if <msg_len> is invalid.
* ERROR if an unknown error in msgQSend.
*/

UINT32 q_vurgent
    (
    UINT32 qid,			/* ID of queue to send to */
    void *msg_buf,			/* pointer to urgent message */
    UINT32 msg_len		/* length of urgent message */
    )
    {
    int 	head;		/* used to schedule head of queue */
    int 	mypri;		/* used to schedule head of queue */
    int 	headpri;	/* used to schedule head of queue */
    MSG_Q_ID 	msgqid = (MSG_Q_ID) qid;     /* clean typecast of qid */
    unsigned int rc;
    FUNCPTR func_ObjToShadow;

    if (!INT_CONTEXT ())
       func_ObjToShadow = (FUNCPTR) p2vObjToShadow;
    else
       func_ObjToShadow = (FUNCPTR) p2vObjToShadowInt;

    /* 
    * make sure the queue is in the proper object table, 
    * return the appropriate error if it is not found, or is in the wrong table
    */
    if (pVarQTbl)
       rc = (unsigned int) func_ObjToShadow (pVarQTbl, (void *) qid);
    else
       rc = ERROR;

    if (rc == ERROR)
       {
       if (pFixQTbl)
	  if (func_ObjToShadow (pFixQTbl, (void *) qid) != ERROR)
	     return (ERR_NOTVARQ);
       return (ERR_OBJID);
       }

    /* get the priority of the calling task and the head of the waiting queue */

    head = (int) Q_FIRST (&msgqid->msgQ.pendQ);
    taskPriorityGet (taskIdSelf(), &mypri);
    taskPriorityGet (head, &headpri);

    if (msgQSend ((MSG_Q_ID) qid, (char *) msg_buf, msg_len, NO_WAIT, 
	MSG_PRI_URGENT) == ERROR )
	{
	if (errno == S_objLib_OBJ_ID_ERROR) 
	    return (ERR_OBJID);
	if (errno == S_objLib_OBJ_UNAVAILABLE) 
	    return (ERR_QFULL);
	if (errno == S_objLib_OBJ_DELETED) 
	    return (ERR_OBJDEL);
	if (errno == S_msgQLib_INVALID_MSG_LENGTH) 
	    return(ERR_MSGSIZ);
        return (ERROR);
	}

    /* if the head of the wait queue is higher priority, let it go */

    if( !(INT_CONTEXT()) && (headpri > mypri) )
	taskDelay (0);

    return (EOK);
    }

/*****************************************************************************
*
* q_vreceive -  variable length message queue receive routine
*
* INCLUDE FILES: p2vLib.h
*
* Receive a variable length message from a queue
* Note the following differences from PSOS+(r) behavior:
* If the message length is greater than 0x0fffffff, an
* ERROR will be returned from msgQReceive.  If the message is
* longer than the buffer length, vxWorks does not detect an
* error, it simply discards the remainder of the message.
*
* RETURNS:
* EOK if successful
* ERR_OBJID if <qid> is invalid.
* ERR_NOTVARQ if target queue is a variable length message queue.
* ERR_TIMEOUT if timed out waiting for a message.
* ERR_NOMSG if doing a NO_WAIT receive and there is no message in the queue.
* ERR_QKILLD if queue deleted while waiting for a message.
* ERROR if unknown error in msgQReceive.
*/

UINT32 q_vreceive
    (
    UINT32 qid,
    UINT32 flags,
    UINT32 timeout,
    void *msg_buf,
    UINT32 buf_len,
    UINT32 *msg_len
    )
    {
    unsigned int rc;
    /* 
    * make sure the queue is in the proper object table, 
    * return the appropriate error if it is not found, or is in the wrong table
    */
    if (pVarQTbl)
       rc = (unsigned int) p2vObjToShadow (pVarQTbl, (void *) qid);
    else
       rc = ERROR;

    if (rc == ERROR)
       {
       if (pFixQTbl)
          if (p2vObjToShadow (pFixQTbl, (void *) qid) != (void *) ERROR)
	     return (ERR_NOTVARQ);
       return (ERR_OBJID);
       }

    /* convert to vxWorks timeout */

    if (timeout == 0) 
	timeout = WAIT_FOREVER;
    if (flags & Q_NOWAIT) 
	timeout = NO_WAIT;

    if ((*msg_len = msgQReceive ((MSG_Q_ID) qid, (char *) msg_buf, buf_len, 
	timeout)) == ERROR )
	{
	if (errno == S_objLib_OBJ_ID_ERROR) 
	    return (ERR_OBJID);
	if (errno == S_objLib_OBJ_TIMEOUT) 
	    return (ERR_TIMEOUT);
	if (errno == S_objLib_OBJ_UNAVAILABLE) 
	    return (ERR_NOMSG);
	if (errno == S_objLib_OBJ_DELETED) 
	    return (ERR_QKILLD);
        return (ERROR);
	}

    return (EOK);
    }

/*****************************************************************************
*
* q_vbroadcast - variable length message queue broadcast routine
*
* INCLUDE FILES:  p2vLib.h
*
* This function sends a variable length message to all tasks waiting at
* a given message queue.
*
* RETURNS: 
* EOK if successful
* ERR_OBJID if <qid> is invalid.
* ERR_NOTVARQ if target queue is a fixed length message queue.
* ERR_OBJDEL if msgQSend detects a deleted queue.
* ERR_MSGSIZ if <msg_len> is invalid.
* ERROR if an unknown error in msgQSend.
*/

UINT32 q_vbroadcast
    (
    UINT32 qid,			/* ID of queue to send to */
    void *msg_buf,		/* pointer to urgent message */
    UINT32 msg_len,		/* length of urgent message */
    UINT32 *count
    )
    {
    int 	rvalue = EOK;
    int 	head;		/* used to schedule head of queue */
    int 	mypri;		/* used to schedule head of queue */
    int 	headpri;	/* used to schedule head of queue */
    int 	hipri=0;	/* used to schedule head of queue */
    MSG_Q_ID 	msgqid = (MSG_Q_ID) qid;     /* clean typecast of qid */
    unsigned int rc;
    FUNCPTR func_ObjToShadow;

    if (!INT_CONTEXT ())
       func_ObjToShadow = (FUNCPTR) p2vObjToShadow;
    else
       func_ObjToShadow = (FUNCPTR) p2vObjToShadowInt;

    /* 
    * make sure the queue is in the proper object table, 
    * return the appropriate error if it is not found, or is in the wrong table
    */
    if (pVarQTbl)
       rc = (unsigned int) func_ObjToShadow( pVarQTbl, (void *) qid);
    else
       rc = ERROR;

    if (rc == ERROR)
       {
       if (pFixQTbl)
	  if (func_ObjToShadow (pFixQTbl, (void *) qid) != ERROR)
	     return (ERR_NOTVARQ);
       return (ERR_OBJID);
       }

    *count=0;

    /* get the callers priority for scheduling */

    taskPriorityGet (taskIdSelf(), &mypri);

    /* keep other tasks from interfering during the broadcast */

    if (!INT_CONTEXT ())
        taskLock();						/* TASKLOCK */
    
    /* send messages to the queue until it is empty */

    while ((head = (int) Q_FIRST (&msgqid->msgQ.pendQ)) != NULL)
	{
        taskPriorityGet (head, &headpri);
	if (headpri > hipri) 
	    hipri = headpri;
        if (msgQSend ((void *) qid, (char *) msg_buf, msg_len, NO_WAIT, 
	    MSG_PRI_URGENT) == ERROR )
	    {
            rvalue = ERROR;
	    break;
	    }
	*count = *count + 1;
        }

    if (!INT_CONTEXT ())
        taskUnlock();					/* TASK UNLOCK */

    /* handle any errors that occurred */

    if (rvalue == ERROR)
	{
	if(errno == S_objLib_OBJ_ID_ERROR) 
	    return (ERR_OBJDEL);
	if(errno == S_objLib_OBJ_DELETED) 
	    return (ERR_OBJDEL);
	if(errno == S_msgQLib_INVALID_MSG_LENGTH) 
	    return (ERR_MSGSIZ);
        return (ERROR);
	}

    /* and, if necessary, let a higher priority task go */

    if( !(INT_CONTEXT()) && (headpri > hipri) )
        taskDelay (0);

    return (EOK);
    }
