/* errnoULib.c - dummy error status library for UNIX */

/* Copyright 1984,1985,1986,1987,1988,1989 Wind River Systems, Inc. */
extern char copyright_wind_river[]; static char *copyright=copyright_wind_river;

/*
modification history
--------------------
02c,05jun88,dnw  changed stsULib to errnoULib.
02b,30may88,dnw  changed to v4 names.
02a,26jan88,jcf  made kernel independent.
01e,01apr87,ecs  hushed lint in getTaskStatus.
01d,21dec86,dnw  changed to not get include files from default directories.
01c,21jul85,jlf  documentation.
01b,08sep84,jlf  added copyright and comments
01a,09aug84,ecs  written
*/

/*
DESCRIPTION
This library contains dummy versions of the routines in errnoLib.c.  It is
meant to be used under UNIX, when required by modules which are used both
under VxWorks and under UNIX.

SEE ALSO: errnoLib (1)
*/

#include "vxWorks.h"

LOCAL int errorStatus;

/*******************************************************************************
*
* errnoGet - get error status value of calling task
*/

int errnoGet ()

    {
    return (errorStatus);
    }
/*******************************************************************************
*
* errnoOfTaskGet - get error status value of specified task
*
* RETURNS: status value for given task, or ERROR if task doesn't exist.
*
* ARGSUSED
*/

int errnoOfTaskGet (tid)
    int tid;			/* task's id, 0 means current task */

    {
    return (errorStatus);
    }
/*******************************************************************************
*
* errnoSet - set error status value of calling task
*/

VOID errnoSet (errno)
    int errno;			/* error status value */

    {
    errorStatus = errno;
    }
