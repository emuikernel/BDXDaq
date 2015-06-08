/* p2vTblLib.c -  local object table manipulation routines */

/* Copyright 1984-1996 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01j,22jan97,jmb  Added semaphore to table entry remove routine.
01i,22jan97,jmb  Eliminated duplicate #include
01h,09jan97,jmb  Added intLocks in table creation and delete routines because
		 q_*send can do a table lookups at interrupt level.
01g,03jun96,jmb  Increase P2V_MAX_TABLES
01f,13jun96,jmb  Comment clean-up
01e,12jun96,jmb  Scalability...don't want to always pull in formatted I/O
01d,25apr96,jmb  Made the tables static and eliminated the list of tables
                 Also added p2vObjToShadowInt for accessing tables at
		 interrupt level (used in pt_getbuf, pt_retbuf).
01c,17apr96,jmb  comments and formatting
01b,14apr96,jmb  added table dumping and message dumping functions
01a,14mar96,cym  created
*/

#include "vxWorks.h"
#include "p2vLib.h"
#include "intLib.h"
#include "semLib.h"
#include "stdlib.h"
#include "stdio.h"


int numP2vTables = 0;

#define P2V_MAX_TABLES 5

P2V_TABLE p2vTableArray[P2V_MAX_TABLES];

/******************************************************************************
*
* p2vNameToObj - local name table name to object conversion
*
* This function searches through the specified local name table, and returns
* the object id entered in the table to the caller.  It is used in most of the
* various *_ident routines.
*
* INCLUDE FILES: p2vLib.h
*
* RETURNS:
* The object id corresponding to <name>.
* ERROR if <name> not found in <table>.
*/ 

void * p2vNameToObj
    (
    P2V_TABLE *table,  	   /* ptr to table to search */
    unsigned int name              /* name to look up */
    )
    {
    int	loop;
    int	offset = table->count / 2;
    int	increment = (table->count + 1) / 2;

    void *rvalue = (void *) ERROR;

    /* check the table has been created and has entries */

    if( !table->initialized ) 
	return ((void *) ERROR);
    if (table->count == 0) 
	return ((void *) ERROR);

    /* wait until the table is free */

    semTake (table->mutexSem, WAIT_FOREVER);

    /* Binary search of the table */

    for (loop = table->log2Count ; loop > 0 ; loop--)
	{
	/* Halve the increment for binary search */

	increment = (increment + 1) / 2;

	/* Quit when the name is found */
	
	if (name == table->nodes[offset].name)
            break;

	/* Otherwise move towards the name */

	if (name > table->nodes[offset].name)
	    {
            offset += increment;
	    if (offset > (table->count - 1)) 
		offset = table->count - 1;
	    }
        else 
	    {
	    offset -= increment;
	    if (offset < 0 ) 
		offset = 0;
	    }
        }

    /* If the name was found, return the corresponding object */

    if (loop > 0) 
	rvalue = table->nodes[offset].pObject;

    /* Done with table, release semaphore */

    semGive (table->mutexSem);

    return (rvalue);
    }

/******************************************************************************
*
* p2vObjToShadow - get the "shadow" of the given vxWorks object
*   from the given object table
*
* INCLUDE FILES: p2vLib.h
*
* This function is used to look up auxiliary information about a local object.
* It can also be used simply to verify whether an object has been installed
* in a local object table.
*
* RETURNS: objects shadow, or ERROR if an error occurs or the object is not 
* Object's "shadow".  The shadow is a pointer to auxiliary info about an object.
* ERROR if object not found.
*/
void * p2vObjToShadow
    (
    P2V_TABLE *pTable,
    void *pObject
    )
    {
    int 	loop;
    void *	rvalue = (void *) ERROR;

    /* Quit if the table isn't initialized */

    if (!pTable->initialized) 
	return ((void *) ERROR);

    /* Wait for the table to free */

    semTake (pTable->mutexSem, WAIT_FOREVER);

    /* Linear search since the contents aren't ordered by object */

    for (loop = 0; loop < pTable->count; loop++)
	if (pTable->nodes[loop].pObject == pObject)
	    rvalue = pTable->nodes[loop].pAux;

    /* Free the table */

    semGive (pTable->mutexSem);

    return (rvalue);
    }

/******************************************************************************
*
* p2vObjToShadowInt - get the "shadow" of the given vxWorks object
*   from the given object table -- for use at interrupt level.
*
* INCLUDE FILES: p2vLib.h
*
* This function is used to look up auxiliary information about a local object.
* It can also be used simply to verify whether an object has been installed
* in a local object table.
*
* RETURNS: objects shadow, or ERROR if an error occurs or the object is not 
* Object's "shadow".  The shadow is a pointer to auxiliary info about an object.
* ERROR if object not found.
*/
void * p2vObjToShadowInt
    (
    P2V_TABLE *pTable,
    void *pObject
    )
    {
    int 	loop;
    void *	rvalue = (void *) ERROR;

    /* Quit if the table isn't initialized */

    if (!pTable->initialized) 
	return ((void *) ERROR);

    /* Linear search since the contents aren't ordered by object */

    for (loop = 0; loop < pTable->count; loop++)
	if (pTable->nodes[loop].pObject == pObject)
	    rvalue = pTable->nodes[loop].pAux;

    return (rvalue);
    }

/******************************************************************************
*
* p2vTblRemove - remove an entry from a local object table
*
* INCLUDE FILES: p2vLib.h
*
* This function remove an entry from a local object table, and keeps the table 
* ordered by object ID.
*
* RETURNS: 
* OK if successful
* ERROR otherwise.
*/
STATUS p2vTblRemove
    (
    P2V_TABLE *pTable,
    void *pObject
    )
    {
    int 	loop;
    BOOL 	found = FALSE;
    UINT32	key;

    semTake (pTable->mutexSem, WAIT_FOREVER);
    /* 
    * Move through the table until the right node is found, then move 
    * every node back one, overwriting the found node
    * Don't need to intLock this loop since p2vObjShadowInt does a
    * forward linear search.
    */
    for (loop = 0; loop < pTable->count; loop++)
	if (pTable->nodes[loop].pObject == pObject)
	   {
	   found = TRUE;
	   break;
	   }

    /* If the node was found, lower the count and binary count #s */

    if (found)
	{
	key = intLock(); 	/* LOCK INTERRUPTS */
        for (loop = loop+1; loop < pTable->count; loop++)
	   {
	   pTable->nodes[loop-1].name   = pTable->nodes[loop].name;
	   pTable->nodes[loop-1].pObject = pTable->nodes[loop].pObject;
	   pTable->nodes[loop-1].pAux = pTable->nodes[loop].pAux;
	   }
	pTable->count--;
	if ((pTable->count + 1) < (1 << (pTable->log2Count - 1))) 
	    pTable->log2Count--;
        intUnlock(key);		/* UNLOCK INTERRUPTS */
	}

    semGive (pTable->mutexSem);

    if (found == TRUE)
       return (OK);
    else
       return (ERROR);
    }

/******************************************************************************
*
* p2vTblAdd - add an entry to a local object table 
*
* this function adds an entry to a local object table, keeping the table 
* ordered by object ID.
*
* returns - OK if the entry is added to the table, ERROR otherwise.
*/

STATUS p2vTblAdd
    (
    P2V_TABLE *pTable,
    unsigned int name,
    void *pObject,
    void *pAux
    )
    {
    int 	loop;
    STATUS 	rvalue = OK;
    UINT32	key;

    if (!pTable->initialized) 
	return (ERROR);
    if (pTable->count == pTable->size) 
	return (ERROR);

    semTake (pTable->mutexSem, WAIT_FOREVER);

    /* 
    * move back from the end of the table, moving each node down to make 
    * room for the new one, until the right place for the node is found,
    * so that the table is ordered by name.
    * Don't need to intLock this loop since p2vObjShadowInt
    * does a forward linear search.
    */
    for (loop = pTable->count - 1; loop >= 0; loop--)
        {
	if(pTable->nodes[loop].name > name)
	    {
	    pTable->nodes[loop+1].name   = pTable->nodes[loop].name;
	    pTable->nodes[loop+1].pObject = pTable->nodes[loop].pObject;
	    pTable->nodes[loop+1].pAux = pTable->nodes[loop].pAux;
	    }
	else
	    break;
	    /*
	    if( pTable->nodes[loop].name == name)
		{
	        rvalue=ERROR;
		break;
		}
	    else
		break;
            */
	}

    /* increase the count, and add the entry to the table */

    loop++;
    key = intLock ();		/* LOCK INTERRUPTS */
    pTable->nodes[loop].name   = name;
    pTable->nodes[loop].pObject = pObject;
    pTable->nodes[loop].pAux = pAux;
    pTable->count++;

    if ((pTable->count + 1) > (1 << pTable->log2Count))
        pTable->log2Count++;
    intUnlock (key);		/* UNLOCK INTERRUPTS */

    semGive (pTable->mutexSem);

    return (rvalue);
    }

/******************************************************************************
*
* p2vTblCreate - create a local object table
*
* This function creates a local object table and a mutex semaphore for
* the object table.  The name of the table is added to a list
* of local object tables to enable user queries of tables.
*
* RETURNS:
* A pointer to the new table, or 
* ERROR 
*/

P2V_TABLE *p2vTblCreate(
    char * name,
    P2V_TABLE_NODE * nodeArray,
    int size
    )
    {
    P2V_TABLE *		rvalue;

    if ( numP2vTables < P2V_MAX_TABLES )
	rvalue = & p2vTableArray[numP2vTables++];
    else
	rvalue = NULL;

    if (rvalue != NULL) 
	{

	/* malloc space for the nodes, and create a semaphore for exclusion */

        rvalue->nodes = nodeArray;

	rvalue->name = name;

	rvalue->mutexSem = semMCreate (SEM_Q_PRIORITY);

	/* if an error occurred, report it and return system resources */

	if ((rvalue->nodes == NULL) || (rvalue->mutexSem == NULL))
	    {
	    if (rvalue->nodes == NULL) 
		free (rvalue->nodes);
	    if (rvalue->mutexSem == NULL) 
		semDelete (rvalue->mutexSem);
	    free (rvalue);
	    rvalue = NULL;
	    }
        else
	    {
	    /* otherwise, initialize the table */
	    rvalue->size = size;
	    rvalue->count = 0;
	    rvalue->log2Count = 1;
	    rvalue->initialized = 1;
	    }
        }

    return (rvalue);
    }
