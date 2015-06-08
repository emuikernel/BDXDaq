/* semULib.c - semaphore library for UNIX */

/* Copyright 1984,1985,1986,1987,1988,1989 Wind River Systems, Inc. */
extern char copyright_wind_river[]; static char *copyright=copyright_wind_river;

/*
modification history
--------------------
01h,26may88,jcf  fixed bad changes introduced in 01g.
01g,04may88,jcf  changed to match new semLib.
01f,03nov87,ecs  documentation.
01e,21dec86,dnw  changed to not get include files from default directories.
01d,20jul85,jlf  documentation.
01c,10sep84,jlf  added copyright, comments.  removed GLOBAL.
01b,24jul84,ecs  appeased lint by making routines actually do something.
01a,10jul84,ecs  written
*/

/*
This library contains dummy versions of semLib routines so that UNIX programs
can make use of libraries that have calls to semLib routines.
These routines don't really do anything, since UNIX doesn't have
an equivalent facility, but they do allow some libraries to be linked into
UNIX tasks that would otherwise have problems.

SEE ALSO: semLib (1)
*/

/* LINTLIBRARY */

#include "vxWorks.h"
#include "semLib.h"


/***********************************************************************
*
* semCreate - create semaphore
*/

SEM_ID semCreate ()

    {
    }

/***********************************************************************
*
* semGive - give semaphore
*/

VOID semGive (semId)
    SEM_ID semId;	/* semaphore id */

    {
    semId->count = 1;
    }
/***********************************************************************
*
* semTake - take semaphore
*/

VOID semTake (semId)
    SEM_ID semId;	/* semaphore id */

    {
    semId->count = 0;
    }
/***********************************************************************
*
* semClear - clear semaphore
*/

VOID semClear (semId)
    SEM_ID semId;	/* semaphore id */

    {
    semId->count = 0;
    }
/***********************************************************************
*
* semDelete - delete semaphore
*/

VOID semDelete (semId)
    SEM_ID semId;	/* semaphore id */

    {
    }
/***********************************************************************
*
* semInit - initialize semaphore
*/

VOID semInit (pSemaphore)
    SEMAPHORE *pSemaphore;	/* pointer to semaphore to init */

    {
    pSemaphore->count = 0;
    }
