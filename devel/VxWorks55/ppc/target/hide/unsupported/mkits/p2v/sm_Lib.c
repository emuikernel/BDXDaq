/* sm_Lib.c -  Semaphores */

/* Copyright 1984-1996 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01e,16oct96,jmb  handle ident call before object table initialized
01d,14jun96,jmb  comment clean-up
01c,25apr96,jmb  changed local object table initialization
01b,16apr96,jmb  reformatted

*/


#include "vxWorks.h"
#include "qLib.h"
#include "p2vLib.h"
#include "semLib.h"
#include "objLib.h"
#include "private/taskLibP.h"

/* Global variables  */

IMPORT P2V_TABLE_NODE p2vSemArray[];
IMPORT int p2vMaxSem;

/* Local variables  */

LOCAL BOOL initialized = FALSE;
LOCAL P2V_TABLE *pSemTbl;		/* semaphore local name table */
LOCAL char semTableName[] = "Semaphores";

/*****************************************************************************
*
* sm_create - semaphore creation routine
*
* This function creates a semaphore and adds its name to
* the local semaphore object table.  Note:  global semaphores are currently
* unsupported.
*
* INCLUDE FILES: p2vLib.h 
*
* RETURNS: 
* EOK if successful.
* ERR_OBJTFULL if local object table is full.
* ERROR if unknown failure in semCCreate.
*/

UINT32 sm_create
    (
    NAME_T (name),			/* four-byte semaphore object name */
    UINT32 count,			/* initial count of the semaphore */
    UINT32 flags,			/* options */
    UINT32 *smid			/* semaphore ID is returned here */
    )
    {
    int vxOptions;			/* equivalent vxWorks options */

    /* convert options to vxWorks */
         
    if( flags & SM_PRIOR) 
	vxOptions = SEM_Q_PRIORITY;
    else
	vxOptions = SEM_Q_FIFO;

    /* create semaphore (these semaphores are vxWorks counting semaphores) */

    *smid = (UINT32) semCCreate (vxOptions, count);

    if(*smid == NULL) 
	return (ERROR);

    /* add the semaphore to the object table */

    if (!initialized)
	{
        pSemTbl = p2vTblCreate (semTableName, p2vSemArray, p2vMaxSem);
	initialized = TRUE;
	}

    if ( p2vTblAdd (pSemTbl, NAME_TO_INT(name), (void *) *smid, NULL) == ERROR) 
	{
	semDelete ((SEM_ID) smid);
	return (ERR_OBJTFULL);
	}

    return (EOK);
    }

/*****************************************************************************
*
* sm_delete - Semaphore deletion routine 
*
* This function deletes the specified semaphore, and removes it from 
* the local semaphore object table.
*
* INCLUDE FILES: p2vLib.h 
*
* RETURNS: 
* EOK if successful.
* ERR_OBJDEL if <smid> is invalid in semDelete().
* ERR_OBJID if <smid> is invalid.
* ERR_TATSDEL if tasks waiting on semaphore when semaphore deleted.
* ERROR if unknown failure in semDelete.
*/

UINT32 sm_delete
    (
    UINT32 smid				/* ID of semaphore to delete */
    )
    {
    int rvalue = EOK;			/* return value if no error occur */  

    /* make sure the semphore is in the local object table */

    if (p2vObjToShadow (pSemTbl, (void *) smid) == (void *) ERROR)
	return (ERR_OBJID);

    /* 
    * if there are any tasks waiting on the semaphore, returns a
    * warningmessage" to the caller
    */
    if (Q_FIRST (&((SEM_ID) smid)->qHead) != NULL)
	rvalue = ERR_TATSDEL;

    /* remove the semaphore from the table */
     
    p2vTblRemove (pSemTbl, (void *) smid);

    /* actually delete the semaphore */

    if (semDelete ((SEM_ID) smid) == ERROR)
	{
	if (errno == S_objLib_OBJ_ID_ERROR) 
	    return (ERR_OBJDEL);
	return (ERROR);
	}

    return (rvalue);
    }

/*****************************************************************************
*
* sm_p - Semaphore access routine. 
*
* This routine is analogous to semTake in vxWorks.
*
* INCLUDE FILES: p2vLib.h 
*
* RETURNS: 
* EOK if successful.
* ERR_OBJID if <smid> is invalid.
* ERR_TIMEOUT if <timeout> expires before semaphore can be taken.
* ERR_NOSEM if semaphore is unavailable and NO_WAIT option specified.
* ERR_SKILLD if semaphore deleted while task is waiting.
* ERROR if unknown failure in semTake.
*/

UINT32 sm_p
    (
    UINT32 smid,			/* ID of semaphore to access */
    UINT32 flags,			/* WAIT/NO_WAIT flag */
    UINT32 timeout    		/* expiration in ticks */
    )
    {

    /* if timeouts are enabled, a timeout of zero means wait forever */

    if (timeout == 0) 
	timeout = WAIT_FOREVER;

    /* if the SM_NOWAIT flag is set, then don't wait */

    if (flags & SM_NOWAIT) 
	timeout = NO_WAIT;

    /* take the semaphore */

    if (semTake ((SEM_ID) smid, timeout) == ERROR)
	{
	if (errno == S_objLib_OBJ_TIMEOUT) 
	    return (ERR_TIMEOUT);
	if (errno== S_objLib_OBJ_ID_ERROR) 
	    return (ERR_OBJID);
	if (errno== S_objLib_OBJ_UNAVAILABLE) 
	    return (ERR_NOSEM);
	if (errno== S_objLib_OBJ_DELETED) 
	    return (ERR_SKILLD);
	return (ERROR);
	}

    return (EOK);
    }

/*****************************************************************************
*
* sm_v - semaphore release routine.
*
* This routine is analogous to a semGive in vxWorks.
*
* INCLUDE FILES: p2vLib.h 
*
* RETURNS: 
* EOK if successful.
* ERR_OBJID if <smid> is invalid.
* ERROR if unknown failure in semGive.
*/

UINT32 sm_v
    (
    UINT32 smid			/* ID of semaphore to release */
    )
    {

    /* give the semaphore */

    if (semGive ((SEM_ID) smid) == ERROR)
	{
	if (errno == S_objLib_OBJ_ID_ERROR) 
	    return (ERR_OBJID);
	return (ERROR);
	}
    return (EOK);
    }

/*****************************************************************************
*
* sm_ident - semaphore identify routine.
*
* This function checks the local semaphore table and gets the ID of a 
* semaphore from its name.  Note:  global semaphores are currently
* unsupported -- the value of <node> must be 0.
*
* RETURNS:
* ERR_NODENO if <node> indicates a remote processor (is not 0).
* ERR_OBJNF if <name> is not found in object tables.
*/

UINT32 sm_ident
    (
    NAME_T (name),		/* four-byte semaphore name */
    UINT32 node,		/* node the semaphore resides on */
    UINT32 *smid		/* semaphore ID returned by this call */
    )
    {

    /* multiproccessor semaphores are currrently unsupported */

    if (node != 0)
	return (ERR_NODENO);

    if (!initialized)
	return (ERR_OBJNF);

    /* get the semaphore ID from the local semaphore object table */
           
    *smid = (UINT32) p2vNameToObj (pSemTbl, NAME_TO_INT(name));

    /* return the proper error code */

    if(*smid == ERROR) 
	return (ERR_OBJNF);

    return (EOK);
    }
