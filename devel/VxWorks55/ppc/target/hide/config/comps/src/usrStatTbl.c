/* usrStatTbl.c - error status symbol table initialization library */

/* Copyright 1984-1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,03jun98,ms   written
*/

/*
DESCRIPTION
Initialize the error status table used by perror and printErrno.
*/

/******************************************************************************
*
* usrStatTblInit - initialize the error status table
*/ 

void usrStatTblInit (void)
    {
    int ix;

    statSymTbl = symTblCreate (STAT_TBL_HASH_SIZE_LOG2, FALSE, memSysPartId);

    for (ix = 0; ix < statTblSize; ix ++)       /* from built in table */
        symTblAdd (statSymTbl, &(statTbl [ix]));
    }

