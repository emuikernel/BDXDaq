/* usrStandalone.c - standalone symbol table initialization library */

/* Copyright 1984-1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,23nov98,cdp  added support for ARM_THUMB.
01a,03jun98,ms   written
*/

/*
DESCRIPTION
Initialize the built-in symbol table.
*/

/******************************************************************************
*
* usrStandaloneInit - initialize the built-in symbol table
*/ 

void usrStandaloneInit (void)
    {
    int ix;

    sysSymTbl = symTblCreate (SYM_TBL_HASH_SIZE_LOG2, TRUE, memSysPartId);

    printf ("\nAdding %ld symbols for standalone.\n", standTblSize);

    for (ix = 0; ix < standTblSize; ix++)       /* fill in from built in table*/
#if     ((CPU_FAMILY == ARM) && ARM_THUMB)
        thumbSymTblAdd (sysSymTbl, &(standTbl[ix]));
#else
        symTblAdd (sysSymTbl, &(standTbl[ix]));
#endif  /* CPU_FAMILY == ARM */
    }

