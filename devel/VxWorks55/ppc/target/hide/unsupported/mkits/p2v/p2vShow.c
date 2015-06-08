/* p2vShow.c -  Utility routines for dumping info from p2vLib */

/* Copyright 1984-1996 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01e,04feb97,jmb  roll version
01d,22jan97,jmb  add version
01c,13jun96,jmb  comment clean up
01b,13jun96,jmb  moved p2vTaskSpawn here because it calls sp(),
		 whichs causes shell to be linked in.
01a,12jun96,jmb  created
*/

#include "vxWorks.h"
#include "taskLib.h"
#include "usrLib.h"
#include "p2vLib.h"
#include "stdlib.h"
#include "stdio.h"

IMPORT int numP2vTables;
IMPORT P2V_TABLE p2vTableArray[];

/******************************************************************************
*
* p2vTableList - print a list of local object tables
*
* INCLUDE FILES: p2vLib.h
*
* This function prints the names and addresses of all the local object tables.
*
* RETURNS:
* <void>
*/
void p2vTableList()
    {
    int i;
    P2V_TABLE * pTable;

    printf ("P2V Tables Installed:\n");
    if ( numP2vTables == 0 )
	{
	printf("  <none>\n");
	return;
	}
    else
        for ( i = 0; i < numP2vTables; i++ )
	    {
	    pTable = &p2vTableArray[i];
	    printf ("  %s :  0x%x\n", 
		pTable->name, (unsigned int) pTable );
	    }
    }


/******************************************************************************
*
* p2vTableDump - print the contents of a local object table
*
* INCLUDE FILES: p2vLib.h
*
* This function prints the contents of a local object table.  If <dumpflag>
* is 0, only the table header is dumped.  If <dumpflag> is 1, all the nodes
* in the table are also dumped.
*
* RETURNS:
* <void>
*/
void p2vTableDump 
    ( 
    P2V_TABLE * pTable, 	/* pointer to a dark Table */
    int dumpflag                /* 0 for header only, 1 for full dump */
    )
    {
    int 	index;
    P2V_TABLE_NODE * node;
    char 	strName[5];

    strName[4] = EOS;    /* Convert name from int to string */
 
    /* Dump the Table characteristics */

    printf ("%s:\n", pTable->name);
    printf ("   size        : %d\n", pTable->size);
    printf ("   count       : %d\n", pTable->count);
    printf ("   log2count   : %d\n", pTable->log2Count);
    printf ("   initialized : %d\n", pTable->initialized);
    printf ("   semId       : 0x%x\n", (unsigned int) pTable->mutexSem);
    printf ("   nodes       : 0x%x\n", (unsigned int) pTable->nodes);

    if (dumpflag == 1)
	{
        node = pTable->nodes;
	for ( index = 0; index < pTable->count; index++ )
	    {
	    printf ("node number: %d\n",index);
	    *(unsigned int *) strName = node[index].name;
	    printf ("   name   : %s\n",strName);
	    printf ("   object : 0x%x\n",(unsigned int) node[index].pObject);
	    printf ("   extra  : 0x%x\n",(unsigned int) node[index].pAux);
	    }
        }
    }

/******************************************************************************
*
* p2vMessage - print the symbolic constant for a pSOS+(r) error code.
*
* INCLUDE FILES: p2vLib.h
*
* RETURNS:
* EOK if error code found.
* ERROR if error code not recognized.
*/

STATUS p2vMessage
    (
    unsigned int msgcode                  /*  message code */
    )
    {
    char * out_msg;

    switch ( msgcode )
	{
        case EOK:		/*  0x00 */
#ifdef VMEEXEC
            out_msg = "E_NOERR";
#else
            out_msg = "EOK";
#endif
	    break;
        case ERR_TIMEOUT:	/*  0x01 */
#ifdef VMEEXEC
            out_msg = "E_TIMOUT";
#else
            out_msg = "ERR_TIMEOUT";
#endif
	    break;
        case ERR_SSFN:		/*  0xff00 */
#ifdef VMEEXEC
            out_msg = "E_SSFN";
#else
            out_msg = "ERR_SSFN";
#endif
	    break;
        case ERR_NODENO:	/*  0x04 */
#ifdef VMEEXEC
            out_msg = "E_NODENO";
#else
            out_msg = "ERR_NODENO";
#endif
	    break;
        case ERR_OBJDEL:	/*  0x05 */
#ifdef VMEEXEC
            out_msg = "E_OBJDEL";
#else
            out_msg = "ERR_OBJDEL";
#endif
	    break;
        case ERR_OBJID:		/*  0x06 */
#ifdef VMEEXEC
            out_msg = "E_OBJID";
#else
            out_msg = "ERR_OBJID";
#endif
	    break;
        case ERR_OBJTFULL:	/*  0x08 */
#ifdef VMEEXEC
            out_msg = "E_OBJFUL";
#else
            out_msg = "ERR_OBJTFULL";
#endif
	    break;
        case ERR_OBJNF:		/*  0x09 */
#ifdef VMEEXEC
            out_msg = "E_OBJNF";
#else
            out_msg = "ERR_OBJNF";
#endif
	    break;
        case ERR_NOSTK:		/*  0x0f */
#ifdef VMEEXEC
            out_msg = "E_NOSTK";
#else
            out_msg = "ERR_NOSTK";
#endif
	    break;
        case ERR_TINYSTK:	/*  0x10 */
#ifdef VMEEXEC
            out_msg = "E_SMLSTK";
#else
            out_msg = "ERR_TINYSTK";
#endif
	    break;
        case ERR_PRIOR:		/*  0x11 */
#ifdef VMEEXEC
            out_msg = "E_PRIOR";
#else
            out_msg = "ERR_PRIOR";
#endif
	    break;
        case ERR_ACTIVE:	/*  0x12 */
#ifdef VMEEXEC
            out_msg = "E_ACTIVE";
#else
            out_msg = "ERR_ACTIVE";
#endif
	    break;
        case ERR_SUSP:		/*  0x14 */
#ifdef VMEEXEC
            out_msg = "E_SUSP";
#else
            out_msg = "ERR_SUSP";
#endif
	    break;
        case ERR_NOTSUSP:	/*  0x15 */
#ifdef VMEEXEC
            out_msg = "E_NOSUSP";
#else
            out_msg = "ERR_NOTSUSP";
#endif
	    break;
        case ERR_SETPRI:	/*  0x16 */
#ifdef VMEEXEC
            out_msg = "E_SETPRI";
#else
            out_msg = "ERR_SETPRI";
#endif
	    break;
        case ERR_REGNUM:	/*  0x17 */
#ifdef VMEEXEC
            out_msg = "E_REGNUM";
#else
            out_msg = "ERR_REGNUM";
#endif
	    break;
	case ERR_PTADDR:	/*  0x28 */
#ifdef VMEEXEC
            out_msg = "E_PTADDR";
#else
            out_msg = "ERR_PTADDR";
#endif
	    break;
	case ERR_BUFSIZE:	/*  0x29 */
#ifdef VMEEXEC
            out_msg = "E_BSIZE";
#else
            out_msg = "ERR_BUFSIZE";
#endif
	    break;
	case ERR_TINYPT:	/*  0x2A */
#ifdef VMEEXEC
            out_msg = "E_TINYPT";
#else
            out_msg = "ERR_TINYPT";
#endif
	    break;
	case ERR_BUFINUSE:	/*  0x2B */
#ifdef VMEEXEC
            out_msg = "E_BUFUSE";
#else
            out_msg = "ERR_BUFINUSE";
#endif
	    break;
	case ERR_NOBUF:		/*  0x2C */
#ifdef VMEEXEC
            out_msg = "E_NOBUF";
#else
            out_msg = "ERR_NOBUF";
#endif
	    break;
	case ERR_BUFADDR:	/*  0x2D */
#ifdef VMEEXEC
            out_msg = "E_BUFADR";
#else
            out_msg = "ERR_BUFADDR";
#endif
	    break;
	case ERR_BUFFREE:	/*  0x2F */
#ifdef VMEEXEC
            out_msg = "E_BUFFRE";
#else
            out_msg = "ERR_BUFFREE";
#endif
	    break;
        case ERR_MSGSIZ: 	/*  0x31 */ 
#ifdef VMEEXEC
	    return (ERROR);
#else
            out_msg = "ERR_MSGSIZ";
#endif
	    break;
        case ERR_BUFSIZ: 	/*  0x32 */ 
#ifdef VMEEXEC
	    return (ERROR);
#else
            out_msg = "ERR_BUFSIZ";
#endif
	    break;
        case ERR_NOMGB:		/*  0x34 */
#ifdef VMEEXEC
            out_msg = "E_NOMSG";
#else
            out_msg = "ERR_NOMSG";
#endif
	    break;
        case ERR_QFULL:		/*  0x35 */
#ifdef VMEEXEC
            out_msg = "E_QFULL";
#else
            out_msg = "ERR_QFULL";
#endif
	    break;
        case ERR_QKILLD:	/*  0x36 */
#ifdef VMEEXEC
            out_msg = "E_QKILLD";
#else
            out_msg = "ERR_QKILLD";
#endif
	    break;
        case ERR_NOMSG:		/*  0x37 */
#ifdef VMEEXEC
            out_msg = "E_NOMSG";
#else
            out_msg = "ERR_NOMSG";
#endif
	    break;
        case ERR_TATQDEL:	/*  0x38 */
#ifdef VMEEXEC
            out_msg = "E_TAQDEL";
#else
            out_msg = "ERR_TATQDEL";
#endif
	    break;
        case ERR_MATQDEL:	/*  0x39 */
#ifdef VMEEXEC
            out_msg = "E_MAQDEL";
#else
            out_msg = "ERR_MATQDEL";
#endif
	    break;
        case ERR_VARQ:		/*  0x3a */
#ifdef VMEEXEC
	    return (ERROR);
#else
            out_msg = "ERR_VARQ";
#endif
	    break;
        case ERR_NOTVARQ:	/*  0x3b */
#ifdef VMEEXEC
	    return (ERROR);
#else
            out_msg = "ERR_NOTVARQ";
#endif
	    break;
        case ERR_NOEVS:		/*  0x3C */
#ifdef VMEEXEC
            out_msg = "E_NOEVS";
#else
            out_msg = "ERR_NOEVS";
#endif
	    break;
        case ERR_NOSEM:		/*  0x42 */
#ifdef VMEEXEC
            out_msg = "E_NOSEM";
#else
            out_msg = "ERR_NOSEM";
#endif
	    break;
        case ERR_SKILLD:	/*  0x43 */
#ifdef VMEEXEC
            out_msg = "E_SKILLD";
#else
            out_msg = "ERR_SKILLD";
#endif
	    break;
        case ERR_TATSDEL:	/*  0x44 */
#ifdef VMEEXEC
            out_msg = "E_TASDEL";
#else
            out_msg = "ERR_TATSDEL";
#endif
	    break;
        case ERR_NOTIME:	/*  0x47 */
#ifdef VMEEXEC
            out_msg = "E_NOTIME";
#else
            out_msg = "ERR_NOTIME";
#endif
	    break;
        case ERR_ILLDATE:	/*  0x48 */
#ifdef VMEEXEC
            out_msg = "E_ILDATE";
#else
            out_msg = "ERR_ILLDATE";
#endif
	    break;
        case ERR_ILLTIME:	/*  0x49 */
#ifdef VMEEXEC
            out_msg = "E_ILTIME";
#else
            out_msg = "ERR_ILLTIME";
#endif
	    break;
        case ERR_ILLTICKS:	/*  0x4A */
#ifdef VMEEXEC
            out_msg = "E_ILTICK";
#else
            out_msg = "ERR_ILLTICKS";
#endif
	    break;
        case ERR_NOTIMERS:	/*  0x4B */
#ifdef VMEEXEC
            out_msg = "E_NOTIMR";
#else
            out_msg = "ERR_NOTIMERS";
#endif
	    break;
        case ERR_BADTMID:	/*  0x4C */
#ifdef VMEEXEC
            out_msg = "E_BDTMID";
#else
            out_msg = "ERR_BADTMID";
#endif
	    break;
        case ERR_TOOLATE:	/*  0x4E */
#ifdef VMEEXEC
            out_msg = "E_TMLATE";
#else
            out_msg = "ERR_TOOLATE";
#endif
	    break;
        case ERR_NOASR:		/*  0x53 */
#ifdef VMEEXEC
            out_msg = "E_NOASR";
#else
            out_msg = "ERR_NOASR";
#endif
	    break;
        default:
	    return(ERROR);
        }
    printf ("%s\n",out_msg);
    return (EOK);
    }

/*****************************************************************************
*
* p2vTaskSpawn - routine to spawn a vxWorks task with the p2v TCB extension
*
* This routine is a counterpart to sp in the vxWorks shell, except that it 
* adds the p2v tcb extension to the task.
*
* INCLUDE FILES: p2vLib.h
*
* RETURNS: 
* Task id of newly spawned task if successful.
* ERROR if failure.
*/

int p2vTaskSpawn
    (
    FUNCPTR entrypt,
    int arg1,
    int arg2,
    int arg3,
    int arg4
    )
    {
    int tid;				/* task ID of task to be spawned */

    /* Don't let the new task run till TCB extension added */

    taskLock();  			/* TASKLOCK */

    /* spawn the task with vxWorks defaults */

    if ( (tid = sp (entrypt, arg1, arg2, arg3, arg4, 
	0, 0, 0, 0, 0)) == ERROR)
	{
	taskUnlock();			/* TASK UNLOCK */
	return (ERROR);
	}

    /* Add the TCB extension */

    if (p2vTCBInit(tid) == ERROR )
	{
	taskDelete (tid);
	taskUnlock();			/* TASK UNLOCK */
	return (ERROR);
	}

    taskUnlock();  			/* TASK UNLOCK */

    return (tid);
    }

/******************************************************************************
*
* p2vVersion - print the version number of this transition kit
*
* INCLUDE FILES: p2vLib.h
*
* RETURNS:
* <void>
*/
void p2vVersion()
    {
    printf ("Version 1.2, February 4, 1997\n");
    }
